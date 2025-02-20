
/*
  Nim graphic client, version 1.1
  Design & coding by Andrea Giotti, 2023
*/

#define NUM_CH_LEV_BITS 2
#define NUM_CH_TAKE_BITS 3
#define NUM_LEVELS 4
#define MAX_LEVELS 8
#define REC_MARGIN 0.95

#define ANSI_FILE_IO

#include "tinx.h"
#include <gtk/gtk.h>

#define WIN_TITLE "Nim"

#define MAX_IO_IPC 128

#define REC_WIDTH 640
#define REC_HEIGHT 400

#define EMPTY_CHAR '0'

typedef struct io_channel
{
	char flow;
	bool ok;
	channel_posix ch;
	char name[MAX_STRLEN];
} io_channel;

typedef struct io_bank
{
	stream_class sc;
	io_channel io[MAX_IO_IPC];
	int size;
	int count;
} io_bank;

typedef struct user_data
{
	int num_levels;
	char drawstate[MAX_LEVELS];
	char outstate_lev[NUM_CH_LEV_BITS];
	char outstate_take[NUM_CH_TAKE_BITS];
	int num_ch_lev_bits;
	int num_ch_take_bits;
	char win_user;
	char win_system;
	int level;
	int number;
	int tot_user;
	int tot_system;
} user_data;

typedef struct gfx_data
{
	GtkWidget *drawingarea;
	GtkWidget *stats;
} gfx_data;

int get_names(char *name[], user_data *udp, stream_class sc)
{
	int i;

	if(sc == input_stream)
		{
		for(i = 0; i < udp->num_levels; i++)
			{
			name[i] = malloc(MAX_STRLEN);
			sprintf(name[i], "nim_level(%d)", i);				
			}

		name[i] = malloc(MAX_STRLEN);
		strcpy(name[i], "win_user");

		name[i + 1] = malloc(MAX_STRLEN);
		strcpy(name[i + 1], "win_system");

		return i + 2;
		}
	else
		{
		for(i = 0; i < udp->num_ch_lev_bits; i++)
			{
			name[i] = malloc(MAX_STRLEN);
			sprintf(name[i], "choose_level(%d)", i);				
			}

		for(i = 0; i < udp->num_ch_take_bits; i++)
			{
			name[udp->num_ch_lev_bits + i] = malloc(MAX_STRLEN);
			sprintf(name[udp->num_ch_lev_bits + i], "choose_number(%d)", i);				
			}

		return udp->num_ch_lev_bits + i;
		}
}

int reset_state(user_data *udp)
{
	int i;

	for(i = 0; i < udp->num_levels; i++)
		udp->drawstate[i] = EMPTY_CHAR;

	for(i = 0; i < udp->num_ch_lev_bits; i++)
		udp->outstate_lev[i] = EMPTY_CHAR;

	for(i = 0; i < udp->num_ch_take_bits; i++)
		udp->outstate_take[i] = EMPTY_CHAR;

	udp->level = 0;
	udp->number = 0;
	
	udp->tot_user = 0;
	udp->tot_system = 0;

	return 0;
}

int setup_state(user_data *udp, int argc, char *argv[])
{
	if(argc > 1)
		{
		udp->num_levels = atoi(argv[1]);
		
		if(udp->num_levels < 0 || udp->num_levels > MAX_LEVELS)
			{
			printf("Argument out of range\n");
			
			return 1;
			}
		}
	else
		udp->num_levels = NUM_LEVELS;

	udp->num_ch_lev_bits = NUM_CH_LEV_BITS;
	udp->num_ch_take_bits = NUM_CH_TAKE_BITS;

	udp->level = 0;
	udp->number = 0;
	
	udp->tot_user = 0;
	udp->tot_system = 0;

	reset_state(udp);

	return 0;
}

int update_state(user_data *udp, io_bank *inp, io_bank *outp)
{
	int i;

	for(i = 0; i < udp->num_levels; i++)
		udp->drawstate[i] = inp->io[i].flow;

	if(inp->io[i].flow == '1')
		udp->tot_user++;

	if(inp->io[i + 1].flow == '1')
		udp->tot_system++;

	for(i = 0; i < udp->num_ch_lev_bits; i++)
		{
		outp->io[i].flow = udp->outstate_lev[i];
		udp->outstate_lev[i] = EMPTY_CHAR;
		}

	for(i = 0; i < udp->num_ch_take_bits; i++)
		{
		outp->io[udp->num_ch_lev_bits + i].flow = udp->outstate_take[i];
		udp->outstate_take[i] = EMPTY_CHAR;
		}

	return 0;
}

typedef struct client_base
{
	user_data ud;
	io_bank in;
	io_bank out;
	bool running;
	gfx_data gd;
	GtkWidget *message;
	GtkWidget *resetbutton;
} client_base;

gboolean draw_callback(GtkWidget *widget, cairo_t *cr, client_base *clb)
{
	int i, j, k, width, height, x, y;
	int num;
	double hue;

	width = gtk_widget_get_allocated_width(widget);
	height = gtk_widget_get_allocated_height(widget);

	cairo_set_line_width(cr, 1);
	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_rectangle(cr, 0, 0, width, height);
	cairo_fill(cr);

	for(j = 0; j < clb->ud.num_levels; j++)
		{
		if(clb->ud.drawstate[j] != EMPTY_CHAR)
			{
			num = clb->ud.drawstate[j] - '0';
			k = 2 * (clb->ud.num_levels - 1) + 1;

			hue = 0.1 + (0.8 * num) / k;

			x = width / k;
			y = height / clb->ud.num_levels;

			cairo_set_source_rgb(cr, 1 - hue, 0, hue);

			for(i = 0; i < num; i++)
				cairo_rectangle(cr, ((1 - REC_MARGIN) / 2 + i) * x, ((1 - REC_MARGIN) / 2 + j) * y, REC_MARGIN * x, REC_MARGIN * y);

			cairo_fill(cr);
			}
		}

	return FALSE;
}

void level_value(GtkWidget *widget, client_base *clb)
{
  clb->ud.level = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget)) - 1;
}

void number_value(GtkWidget *widget, client_base *clb)
{
  clb->ud.number = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
}

int update_gui(user_data *udp, gfx_data *gdp)
{
	char statstring[MAX_STRLEN];

	sprintf(statstring, "User <b>%d</b> vs System <b>%d</b>", udp->tot_user, udp->tot_system);

	gtk_label_set_markup(gdp->stats, statstring);

	gtk_widget_queue_draw(gdp->drawingarea);

	return 0;
}

void commit_button_clicked(GtkWidget *widget, client_base *clb)
{
	int i;

	for(i = 0; i < clb->ud.num_ch_lev_bits; i++)
		{
		if(clb->ud.level & 1 << i)
			clb->ud.outstate_lev[i] = '1';
		else
			clb->ud.outstate_lev[i] = '0';
		}	

	for(i = 0; i < clb->ud.num_ch_take_bits; i++)
		{
		if(clb->ud.number & 1 << i)
			clb->ud.outstate_take[i] = '1';
		else
			clb->ud.outstate_take[i] = '0';
		}
}

GtkWidget *create_gui(client_base *clb)
{
	GtkWidget *drawingarea, *vbox, *hbox, *cbox, *stats, *lbl1, *ent1, *lbl2, *ent2, *commitbutton;

	drawingarea = gtk_drawing_area_new();
	gtk_widget_set_size_request(drawingarea, REC_WIDTH, REC_HEIGHT);
	g_signal_connect(G_OBJECT(drawingarea), "draw", G_CALLBACK(draw_callback), clb);

	clb->gd.drawingarea = drawingarea;

	stats = gtk_label_new(NULL);
	gtk_misc_set_alignment(GTK_MISC(stats), 0.5, 0.5);

	clb->gd.stats = stats;

	lbl1 = gtk_label_new("Nim level ");
	gtk_misc_set_alignment(GTK_MISC(lbl1), 1, 0.5);
	ent1 = gtk_spin_button_new_with_range(1, MAX_LEVELS, 1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(ent1), clb->ud.level);
	g_signal_connect(G_OBJECT(ent1), "value-changed", G_CALLBACK(level_value), (gpointer)clb);

	lbl2 = gtk_label_new("Number of items ");
	gtk_misc_set_alignment(GTK_MISC(lbl2), 1, 0.5);
	ent2 = gtk_spin_button_new_with_range(0, (2 * MAX_LEVELS + 1), 1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(ent2), clb->ud.number);
	g_signal_connect(G_OBJECT(ent2), "value-changed", G_CALLBACK(number_value), (gpointer)clb);

	commitbutton = gtk_button_new_with_label("Take");
	gtk_widget_set_size_request(commitbutton, 70, 30);
	g_signal_connect(G_OBJECT(commitbutton), "clicked", G_CALLBACK(commit_button_clicked), (gpointer)clb);

	vbox = gtk_vbox_new(FALSE, 0);

	gtk_box_pack_start(GTK_BOX(vbox), drawingarea, TRUE, TRUE, 5);
	
	cbox = gtk_hbox_new(FALSE, 0);

	gtk_box_pack_start(GTK_BOX(cbox), lbl1, FALSE, FALSE, 10);
	gtk_box_pack_start(GTK_BOX(cbox), ent1, TRUE, TRUE, 10);
	gtk_box_pack_start(GTK_BOX(cbox), lbl2, FALSE, FALSE, 10);
	gtk_box_pack_start(GTK_BOX(cbox), ent2, FALSE, FALSE, 10);
	gtk_box_pack_end(GTK_BOX(cbox), stats, FALSE, FALSE, 10);
	gtk_box_pack_end(GTK_BOX(cbox), commitbutton, FALSE, FALSE, 10);

	gtk_box_pack_end(GTK_BOX(vbox), cbox, FALSE, FALSE, 10);

	return vbox;
}

bool check_queue_posix(char *name, stream_class sclass)
{
  struct mq_attr attr;

  attr.mq_flags = O_NONBLOCK;
  attr.mq_maxmsg = MAX_MESSAGES;
  attr.mq_msgsize = MSG_SIZE;
  attr.mq_curmsgs = 0;

  mq_open(name, (sclass == input_stream? O_RDONLY : O_WRONLY) | O_CREAT | O_NONBLOCK | O_EXCL, PERMS, &attr);
  if(errno == EEXIST)
    return TRUE;
  else
    return FALSE;
}

void exit_button_clicked(GtkWidget *widget, client_base *clb)
{
	gtk_main_quit();
}

void reset_bank(io_bank *iobk)
{
	int i;

	for(i = 0; i < iobk->size; i++)
		{
		iobk->io[i].flow = EMPTY_CHAR;

		if(!check_queue_posix(iobk->io[i].name, iobk->sc))
			remove_queue_posix(iobk->io[i].name);

		iobk->io[i].ch = add_queue_posix(iobk->io[i].name, iobk->sc);

		iobk->io[i].ok = FALSE;
		}

	iobk->count = iobk->size;
}

void setup_bank(io_bank *iobk, char *name[])
{
	int i;

	for(i = 0; i < iobk->size; i++)
		sprintf(iobk->io[i].name, MAGIC_PREFIX"%s", name[i]);

	reset_bank(iobk);
}

void reset_button_clicked(GtkWidget *widget, client_base *clb)
{
	reset_bank(&clb->in);
	reset_bank(&clb->out);

	reset_state(&clb->ud);

	clb->running = FALSE;
}

gboolean tick_callback(GtkWidget *widget, GdkFrameClock *frame_clock, client_base *clb)
{
	int i;

	for(i = 0; i < clb->in.size; i++)
		 if(!clb->in.io[i].ok && !mread_message_posix(clb->in.io[i].ch, &clb->in.io[i].flow, 1))
		 	{
		 	clb->in.io[i].ok = TRUE;
		 	clb->in.count--;
		 	}

	for(i = 0; i < clb->out.size; i++)
		 if(!clb->out.io[i].ok && !msend_message_posix(clb->out.io[i].ch, &clb->out.io[i].flow, 1))
		 	{
		 	clb->out.io[i].ok = TRUE;
		 	clb->out.count--;
		 	}

	if(!clb->in.count && !clb->out.count)
		{
		update_state(&clb->ud, &clb->in, &clb->out);
		update_gui(&clb->ud, &clb->gd);

		for(i = 0; i < clb->in.size; i++)
			clb->in.io[i].ok = FALSE;

		clb->in.count = clb->in.size;

		for(i = 0; i < clb->out.size; i++)
			clb->out.io[i].ok = FALSE;

		clb->out.count = clb->out.size;
	
		clb->running = TRUE;

		gtk_label_set_markup(clb->message, "<i>Receiving data flow...</i>");
		gtk_widget_set_sensitive(clb->resetbutton, FALSE);
		}
	else
		if(clb->running)
			{
			gtk_label_set_markup(clb->message, "<i>Idle...</i>");
			gtk_widget_set_sensitive(clb->resetbutton, TRUE);
			}
		else
			{
			gtk_label_set_markup(clb->message, "<i>Waiting for data flow...</i>");
			gtk_widget_set_sensitive(clb->resetbutton, FALSE);
			
			if(!check_queue_posix(clb->in.io[0].name, clb->in.sc))
				reset_button_clicked(clb->resetbutton, clb);
			}

	return G_SOURCE_CONTINUE;
}

int main(int argc, char *argv[])
{
	char *name[MAX_IO_IPC];
	client_base clbs;
	int i;
	GtkWidget *window, *vbox, *hbox, *resetbutton, *exitbutton, *message;

	setup_state(&clbs.ud, argc, argv);

	clbs.in.sc = input_stream;
	clbs.in.size = get_names(name, &clbs.ud, clbs.in.sc);
	setup_bank(&clbs.in, name);

	clbs.out.sc = output_stream;
	clbs.out.size = get_names(name, &clbs.ud, clbs.out.sc);
	setup_bank(&clbs.out, name);

	clbs.running = FALSE;

	gtk_disable_setlocale();

	gtk_init(NULL, NULL);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window), REC_WIDTH, REC_HEIGHT);
	gtk_container_set_border_width(GTK_CONTAINER(window), 0);
	gtk_window_set_title(GTK_WINDOW(window), WIN_TITLE);
	gtk_window_set_icon_name(GTK_WINDOW(window), "tinx");
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(exit_button_clicked), (gpointer)&clbs);

	resetbutton = gtk_button_new_with_label("Reset");
	gtk_widget_set_size_request(resetbutton, 70, 30);
	g_signal_connect(G_OBJECT(resetbutton), "clicked", G_CALLBACK(reset_button_clicked), (gpointer)&clbs);

	clbs.resetbutton = resetbutton;

	exitbutton = gtk_button_new_with_label("Exit");
	gtk_widget_set_size_request(exitbutton, 70, 30);
	g_signal_connect(G_OBJECT(exitbutton), "clicked", G_CALLBACK(exit_button_clicked), (gpointer)&clbs);

	message = gtk_label_new(NULL);
	gtk_misc_set_alignment(GTK_MISC(message), 0.5, 0.5);

	clbs.message = message;

	vbox = gtk_vbox_new(FALSE, 0);

	gtk_box_pack_start(GTK_BOX(vbox), create_gui(&clbs), FALSE, FALSE, 10);

	hbox = gtk_hbox_new(FALSE, 0);

	gtk_box_pack_start(GTK_BOX(hbox), resetbutton, FALSE, FALSE, 10);
	gtk_box_pack_start(GTK_BOX(hbox), message, TRUE, TRUE, 10);
	gtk_box_pack_end(GTK_BOX(hbox), exitbutton, FALSE, FALSE, 10);

	gtk_box_pack_end(GTK_BOX(vbox), hbox, FALSE, FALSE, 10);

	gtk_container_add(GTK_CONTAINER(window), vbox);

        gtk_widget_add_tick_callback(window, (gboolean (*)(GtkWidget *, GdkFrameClock *, gpointer))tick_callback, &clbs, NULL);

	gtk_widget_show_all(window);

	gtk_main();

	for(i = 0; i < clbs.in.size; i++)
		remove_queue_posix(clbs.in.io[i].name);

	for(i = 0; i < clbs.out.size; i++)
		remove_queue_posix(clbs.out.io[i].name);

	return 0;
}


