
/*
  Skeleton of a TINX graphic client, version 1.0
  Design & coding by Andrea Giotti, 2023
*/

#define ANSI_FILE_IO

#include "tinx.h"
#include <gtk/gtk.h>

#define WIN_TITLE "Skeleton"

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
/*
	Keep here the state of the system
*/
} user_data;

typedef struct gfx_data
{
/*
	Keep here custom widgets of the GUI
*/
} gfx_data;

#define NUM_INPUTS 2
#define NUM_OUTPUTS 2

int get_names(char *name[], user_data *udp, stream_class sc)
{
	int i, num;
	const char **source;
	static const char *in_name[NUM_INPUTS] =	{
						"varout(0)",
						"varout(1)"
						};
	static const char *out_name[NUM_OUTPUTS] =	{
						"varin(0)",
						"varin(1)"
						};
/*
	Generate names with your favorite method and return their number
*/

	if(sc == input_stream)
		{
		source = in_name;
		num = NUM_INPUTS;
		}
	else
		{
		source = out_name;
		num = NUM_OUTPUTS;
		}

	for(i = 0; i < num; i++)
		name[i] = source[i];
	
	return num;
}

int reset_state(user_data *udp)
{
/*
	Here the state (udp) is reset
*/

	return 0;
}

int setup_state(user_data *udp, int argc, char *argv[])
{
/*
	Here the state (udp) is initialized
*/
	reset_state(udp);

	return 0;
}

int update_state(user_data *udp, io_bank *inp, io_bank *outp)
{
/*
	Here the inputs (inp) must update the state (udp) and the state (udp) must update the outputs (outp)
*/

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

int update_gui(user_data *udp, gfx_data *gdp)
{
/*
	Here the GUI (gdp) is updated on the basis of the state (udp)
*/

	return 0;
}

GtkWidget *create_gui(client_base *clb)
{
	GtkWidget *widget;
/*
	Here the custom section of the GUI is created
*/
	return widget;
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


