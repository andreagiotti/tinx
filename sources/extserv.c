/*
  EXTSERV - Extension server
  Design & coding by Andrea Giotti, 2024
*/

#define ANSI_FILE_IO

#include "tinx.h"
#include <math.h>
#include <gtk/gtk.h>

#define VER "1.1.0"

#define TITLE "ExtServ user interface "VER

#define CMDNAME MAGIC_PREFIX"command"
#define CLKNAME MAGIC_PREFIX"Clock"
#define RNDNAME MAGIC_PREFIX"Random"
#define BTNNAME MAGIC_PREFIX"button"
#define SLDNAME MAGIC_PREFIX"Slider"
#define LGTNAME MAGIC_PREFIX"light"
#define DSPNAME MAGIC_PREFIX"Display"
#define MSGNAME MAGIC_PREFIX"message"
#define SLPNAME MAGIC_PREFIX"Sleep"

#define NUM_RANDOMS 4
#define NUM_BUTTONS 4
#define NUM_SLIDERS 4
#define NUM_LIGHTS 4
#define NUM_DISPLAYS 4

#define GUI_WIDTH 500
#define GUI_HEIGHT 500

typedef enum opcode
{
  op_clock,
  op_random_0,
  op_random_1,
  op_random_2,
  op_random_3,
  op_button_0,
  op_button_1,
  op_button_2,
  op_button_3,
  op_slider_0,
  op_slider_1,
  op_slider_2,
  op_slider_3,
  op_message,
  op_light_0,
  op_light_1,
  op_light_2,
  op_light_3,
  op_display_0,
  op_display_1,
  op_display_2,
  op_display_3,
  op_sleep,
  op_quit,
  OPCODE_NUMBER
} opcode;

typedef struct client_base
  {
    int num_button;
    real val[NUM_SLIDERS];
    GtkWidget *button[NUM_BUTTONS];
    GtkWidget *slider[NUM_SLIDERS];
    GtkWidget *message;
    GtkWidget *light[NUM_LIGHTS];
    GtkWidget *display[NUM_DISPLAYS];
    bool gui;
  } client_base;

void button_clicked(GtkWidget *widget, client_base *clb)
{
  int i;

  for(i = 0; i < NUM_BUTTONS; i++)
    if(widget == clb->button[i])
      {
        clb->num_button = i;
        break;
      }
}

void slider_value(GtkWidget *widget, client_base *clb)
{
  int i;

  for(i = 0; i < NUM_SLIDERS; i++)
    if(widget == clb->slider[i])
      {
        clb->val[i] = gtk_range_get_value(GTK_RANGE(widget));
        break;
      }
}

void end_gui(GtkWidget *widget, client_base *clb)
{
  clb->gui = FALSE;
}

GtkWidget *create_gui(client_base *clb)
{
  GtkWidget *window, *vbox, *tabdyn, *tabdyn2, *tabdyn3, *tabdyn4, *tabdyn5, *button[NUM_BUTTONS], *slider[NUM_SLIDERS], *message, *light[NUM_LIGHTS], *display[NUM_DISPLAYS], *label,
            *frame, *frame2, *frame3, *frame4, *frame5;
  char buttonname[MAX_NAMELEN], lightname[MAX_NAMELEN], displayname[MAX_NAMELEN];
  int i, n;

  gtk_disable_setlocale();

  gtk_init(NULL, NULL);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), GUI_WIDTH, GUI_HEIGHT);
  gtk_container_set_border_width(GTK_CONTAINER(window), 8);
  gtk_window_set_title(GTK_WINDOW(window), TITLE);
  gtk_window_set_icon_name(GTK_WINDOW(window), "tinx");
  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(end_gui), (gpointer)clb);

  n = ceil(sqrt(NUM_BUTTONS));

  tabdyn = gtk_table_new(n, n, FALSE);

  frame = gtk_frame_new("Buttons");
  gtk_label_set_markup(GTK_LABEL(gtk_frame_get_label_widget(GTK_FRAME(frame))), "<b>Buttons</b>");

  gtk_container_add(GTK_CONTAINER(frame), tabdyn);
  gtk_container_set_border_width(GTK_CONTAINER(frame), 4);
  gtk_container_set_border_width(GTK_CONTAINER(tabdyn), 8);
  
  for(i = 0; i < NUM_BUTTONS; i++)
    {
      sprintf(buttonname, "%c", 'A' + i);
      button[i] = gtk_button_new_with_label(buttonname);
      gtk_widget_set_size_request(button[i], 64, 32);
      gtk_widget_set_sensitive(GTK_WIDGET(button[i]), FALSE);
      g_signal_connect(G_OBJECT(button[i]), "clicked", G_CALLBACK(button_clicked), (gpointer)clb);
      gtk_table_attach_defaults(GTK_TABLE(tabdyn), button[i], i % n, i % n + 1, i / n, i / n + 1);

      clb->button[i] = button[i];
    }

  n = ceil(sqrt(NUM_SLIDERS));

  tabdyn2 = gtk_table_new(n, n, FALSE);

  frame2 = gtk_frame_new("Sliders");
  gtk_label_set_markup(GTK_LABEL(gtk_frame_get_label_widget(GTK_FRAME(frame2))), "<b>Sliders</b>");

  gtk_container_add(GTK_CONTAINER(frame2), tabdyn2);
  gtk_container_set_border_width(GTK_CONTAINER(frame2), 4);
  gtk_container_set_border_width(GTK_CONTAINER(tabdyn2), 8);

  for(i = 0; i < NUM_SLIDERS; i++)
    {
      slider[i] = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 1, 0.01);
      gtk_scale_set_value_pos(GTK_SCALE(slider[i]), GTK_POS_LEFT);
      gtk_scale_add_mark(GTK_SCALE(slider[i]), 0.5,  GTK_POS_TOP, NULL);
      gtk_scale_add_mark(GTK_SCALE(slider[i]), 0.5,  GTK_POS_BOTTOM, NULL);
      gtk_range_set_value(GTK_RANGE(slider[i]), clb->val[i]);
      gtk_widget_set_sensitive(GTK_WIDGET(slider[i]), FALSE);
      g_signal_connect(G_OBJECT(slider[i]), "value-changed", G_CALLBACK(slider_value), (gpointer)clb);
      gtk_table_attach_defaults(GTK_TABLE(tabdyn2), slider[i], i % n, i % n + 1, i / n, i / n + 1);

      clb->slider[i] = slider[i];
    }

  n = ceil(sqrt(NUM_LIGHTS));

  tabdyn3 = gtk_table_new(n, 2 * n, FALSE);

  frame3 = gtk_frame_new("Lights");
  gtk_label_set_markup(GTK_LABEL(gtk_frame_get_label_widget(GTK_FRAME(frame3))), "<b>Lights</b>");

  gtk_container_add(GTK_CONTAINER(frame3), tabdyn3);
  gtk_container_set_border_width(GTK_CONTAINER(frame3), 4);
  gtk_container_set_border_width(GTK_CONTAINER(tabdyn3), 8);

  for(i = 0; i < NUM_LIGHTS; i++)
    {
      sprintf(lightname, "%c", 'A' + i);
      label = gtk_label_new(lightname);
      gtk_misc_set_alignment(GTK_MISC(label), 0.5, 0.5);
      gtk_table_attach_defaults(GTK_TABLE(tabdyn3), label, 2 * (i % n), 2 * (i % n) + 1, i / n, i / n + 1);
      light[i] = gtk_image_new();
      gtk_image_set_from_stock(GTK_IMAGE(light[i]), GTK_STOCK_DIALOG_QUESTION, GTK_ICON_SIZE_DND);
      gtk_misc_set_alignment(GTK_MISC(light[i]), 0.5, 0.5);
      gtk_table_attach_defaults(GTK_TABLE(tabdyn3), light[i], 2 * (i % n) + 1, 2 * (i % n) + 2, i / n, i / n + 1);

      clb->light[i] = light[i];
    }

  n = ceil(sqrt(NUM_DISPLAYS));

  tabdyn4 = gtk_table_new(n, 2 * n, FALSE);

  frame4 = gtk_frame_new("Displays");
  gtk_label_set_markup(GTK_LABEL(gtk_frame_get_label_widget(GTK_FRAME(frame4))), "<b>Displays</b>");

  gtk_container_add(GTK_CONTAINER(frame4), tabdyn4);
  gtk_container_set_border_width(GTK_CONTAINER(frame4), 4);
  gtk_container_set_border_width(GTK_CONTAINER(tabdyn4), 8);

  for(i = 0; i < NUM_DISPLAYS; i++)
    {
      sprintf(displayname, "\t%c\t", 'A' + i);
      label = gtk_label_new(displayname);
      gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
      gtk_table_attach_defaults(GTK_TABLE(tabdyn4), label, 2 * (i % n), 2 * (i % n) + 1, i / n, i / n + 1);
      display[i] = gtk_label_new("--");
      gtk_misc_set_alignment(GTK_MISC(display[i]), 0, 0.5);
      gtk_widget_set_size_request(display[i], 192, 32);
      gtk_table_attach_defaults(GTK_TABLE(tabdyn4), display[i], 2 * (i % n) + 1, 2 * (i % n) + 2, i / n, i / n + 1);

      clb->display[i] = display[i];
    }

  tabdyn5 = gtk_table_new(1, 1, FALSE);

  frame5 = gtk_frame_new("Message");
  gtk_label_set_markup(GTK_LABEL(gtk_frame_get_label_widget(GTK_FRAME(frame5))), "<b>Message</b>");

  gtk_container_add(GTK_CONTAINER(frame5), tabdyn5);
  gtk_container_set_border_width(GTK_CONTAINER(frame5), 4);
  gtk_container_set_border_width(GTK_CONTAINER(tabdyn5), 16);

  message = gtk_label_new("--");
  gtk_misc_set_alignment(GTK_MISC(message), 0.5, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabdyn5), message, 0, 1, 0, 1);

  clb->message = message;

  vbox = gtk_vbox_new(FALSE, 0);

  gtk_box_pack_start(GTK_BOX(vbox), frame3, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), frame4, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), frame5, FALSE, FALSE, 0);
  gtk_box_pack_end(GTK_BOX(vbox), frame2, FALSE, FALSE, 0);
  gtk_box_pack_end(GTK_BOX(vbox), frame, FALSE, FALSE, 0);

  gtk_container_add(GTK_CONTAINER(window), vbox);

  gtk_widget_show_all(window);

  return window;
}

int main(int argc, char *argv[])
{
  channel_posix cmd, clk, rnd[NUM_RANDOMS], btn[NUM_BUTTONS], sld[NUM_SLIDERS], msg, lgt[NUM_LIGHTS], dsp[NUM_DISPLAYS], slp;
  char cmdflow[MAX_STRLEN], dataflow[MAX_STRLEN], buffer[MAX_STRLEN], portname[MAX_NAMELEN];
  struct timespec ts;
  GtkWidget *window;
  int i, k;
  bool quit, active_btn[NUM_BUTTONS], active_sld[NUM_SLIDERS];
  real time;
  client_base clb;

  cmd = add_queue_posix(CMDNAME, input_stream);

  clk = -1;
  
  for(i = 0; i < NUM_RANDOMS; i++)
    rnd[i] = -1;

  for(i = 0; i < NUM_BUTTONS; i++)
    {
      btn[i] = -1;
      active_btn[i] = FALSE;
    }

  clb.num_button = -1;

  for(i = 0; i < NUM_SLIDERS; i++)
    {
      sld[i] = -1;
      active_sld[i] = FALSE;
      clb.val[i] = 0;
    }

  msg = -1;

  for(i = 0; i < NUM_LIGHTS; i++)
    lgt[i] = -1;

  for(i = 0; i < NUM_DISPLAYS; i++)
    dsp[i] = -1;

  slp = -1;

  time = 0;

  quit = FALSE;
  window = NULL;
  clb.gui = FALSE;
  while(!quit)
    {
      memset(cmdflow, 0, ceil(OPCODE_NUMBER / 8.0));
      if(mread_message_posix(cmd, cmdflow, ceil(OPCODE_NUMBER / 8.0)) && errno != EAGAIN)
        {
          perror("Command channel");
          exit(EXIT_FAILURE);
        }

      if(*cmdflow != EOF)
        {
          clb.gui = FALSE;

          for(i = 0; i < OPCODE_NUMBER; i++)
            if(cmdflow[i / 8] & (1 << (i % 8)))
              {
                switch(i)
                  {
                    case op_clock:
                      if(clk < 0)
                        clk = add_queue_posix(CLKNAME, output_stream);

                      clock_gettime(CLOCK_TYPE, &ts);
                      sprintf(buffer, REAL_OUT_FMT"\n", ts.tv_sec + 0.000000001 * ts.tv_nsec);

                      if(msend_message_posix(clk, buffer, strlen(buffer)))
                        {
                          perror("Data output channel");
                          exit(EXIT_FAILURE);
                        }
                    break;

                    case op_random_0:
                    case op_random_1:
                    case op_random_2:
                    case op_random_3:
                      k = i - op_random_0;

                      if(rnd[k] < 0)
                        {
                          sprintf(portname, RNDNAME"(%d)", k);
                          rnd[k] = add_queue_posix(portname, output_stream);
                        }

                      sprintf(buffer, REAL_OUT_FMT"\n", (real)rand() / RAND_MAX);

                      if(msend_message_posix(rnd[k], buffer, strlen(buffer)))
                        {
                          perror("Data output channel");
                          exit(EXIT_FAILURE);
                        }
                    break;

                    case op_button_0:
                    case op_button_1:
                    case op_button_2:
                    case op_button_3:
                      k = i - op_button_0;

                      if(!window)
                        window = create_gui(&clb);

                      if(!active_btn[k])
                        {
                          active_btn[k] = TRUE;
                          gtk_widget_set_sensitive(GTK_WIDGET(clb.button[k]), TRUE);
                        }

                      if(btn[k] < 0)
                        {
                          sprintf(portname, BTNNAME"(%d)", k);
                          btn[k] = add_queue_posix(portname, output_stream);
                        }

                      if(clb.num_button == k)
                        {
                          clb.num_button = -1;
                          strcpy(buffer, "1");
                        }
                      else
                        strcpy(buffer, "0");

                      if(msend_message_posix(btn[k], buffer, strlen(buffer)))
                        {
                          perror("Data output channel");
                          exit(EXIT_FAILURE);
                        }

                      clb.gui = TRUE;
                    break;

                    case op_slider_0:
                    case op_slider_1:
                    case op_slider_2:
                    case op_slider_3:
                      k = i - op_slider_0;

                      if(!window)
                        window = create_gui(&clb);

                      if(!active_sld[k])
                        {
                          active_sld[k] = TRUE;
                          gtk_widget_set_sensitive(GTK_WIDGET(clb.slider[k]), TRUE);
                        }

                      if(sld[k] < 0)
                        {
                          sprintf(portname, SLDNAME"(%d)", k);
                          sld[k] = add_queue_posix(portname, output_stream);
                        }

                      sprintf(buffer, REAL_OUT_FMT"\n", clb.val[k]);

                      if(msend_message_posix(sld[k], buffer, strlen(buffer)))
                        {
                          perror("Data output channel");
                          exit(EXIT_FAILURE);
                        }

                      clb.gui = TRUE;
                    break;

                    case op_message:
                      if(!window)
                        window = create_gui(&clb);

                      if(msg < 0)
                        msg = add_queue_posix(MSGNAME, input_stream);

                      if(mread_message_posix(msg, dataflow, MAX_STRLEN) && errno != EAGAIN)
                        {
                          perror("Data input channel");
                          exit(EXIT_FAILURE);
                        }

                      if(*dataflow != EOF)
                        gtk_label_set_label(GTK_LABEL(clb.message), dataflow);

                      clb.gui = TRUE;
                    break;

                    case op_light_0:
                    case op_light_1:
                    case op_light_2:
                    case op_light_3:
                      k = i - op_light_0;

                      if(!window)
                        window = create_gui(&clb);

                      if(lgt[k] < 0)
                        {
                          sprintf(portname, LGTNAME"(%d)", k);
                          lgt[k] = add_queue_posix(portname, input_stream);
                        }

                      if(read_message_posix(lgt[k], dataflow) && errno != EAGAIN)
                        {
                          perror("Data input channel");
                          exit(EXIT_FAILURE);
                        }

                      if(*dataflow != EOF)
                        {
                          if(*dataflow == '1')
                            gtk_image_set_from_stock(GTK_IMAGE(clb.light[k]), GTK_STOCK_OK, GTK_ICON_SIZE_DND);
                          else
                            if(*dataflow == '0')
                              gtk_image_set_from_stock(GTK_IMAGE(clb.light[k]), GTK_STOCK_CANCEL, GTK_ICON_SIZE_DND);
                            else
                              gtk_image_set_from_stock(GTK_IMAGE(clb.light[k]), GTK_STOCK_DIALOG_QUESTION, GTK_ICON_SIZE_DND);
                        }

                      clb.gui = TRUE;
                    break;

                    case op_display_0:
                    case op_display_1:
                    case op_display_2:
                    case op_display_3:
                      k = i - op_display_0;

                      if(!window)
                        window = create_gui(&clb);

                      if(dsp[k] < 0)
                        {
                          sprintf(portname, DSPNAME"(%d)", k);
                          dsp[k] = add_queue_posix(portname, input_stream);
                        }

                      if(mread_message_posix(dsp[k], dataflow, MAX_STRLEN) && errno != EAGAIN)
                        {
                          perror("Data input channel");
                          exit(EXIT_FAILURE);
                        }

                      if(*dataflow != EOF)
                        {
                          *strchr(dataflow, '\n') = '\0';
                          gtk_label_set_label(GTK_LABEL(clb.display[k]), dataflow);
                        }

                      clb.gui = TRUE;
                    break;

                    case op_sleep:
                      if(slp < 0)
                        slp = add_queue_posix(SLPNAME, input_stream);

                      if(mread_message_posix(slp, dataflow, MAX_STRLEN) && errno != EAGAIN)
                        {
                          perror("Data input channel");
                          exit(EXIT_FAILURE);
                        }

                      if(*dataflow != EOF)
                        sscanf(dataflow, REAL_IN_FMT, &time);
                    break;

                    case op_quit:
                      quit = TRUE;
                    break;
                  }
              }
            else
              {
                switch(i)
                  {
                    case op_button_0:
                    case op_button_1:
                    case op_button_2:
                    case op_button_3:
                      k = i - op_button_0;

                      if(window && active_btn[k])
                        {
                          active_btn[k] = FALSE;
                          gtk_widget_set_sensitive(GTK_WIDGET(clb.button[k]), FALSE);
                        }
                    break;

                    case op_slider_0:
                    case op_slider_1:
                    case op_slider_2:
                    case op_slider_3:
                      k = i - op_slider_0;

                      if(window && active_sld[k])
                        {
                          active_sld[k] = FALSE;
                          gtk_widget_set_sensitive(GTK_WIDGET(clb.slider[k]), FALSE);
                        }
                    break;

                    case op_sleep:
                      time = 0;
                    break;

                    default:
                    break;
                  }              
              }
        }

      if(window)
        {
          if(!clb.gui)
            {
              gtk_widget_destroy(window);
              window = NULL;

              for(i = 0; i < NUM_BUTTONS; i++)
                active_btn[i] = FALSE;

              for(i = 0; i < NUM_SLIDERS; i++)
                active_sld[i] = FALSE;
            }

          while(gtk_events_pending())
            gtk_main_iteration_do(FALSE);
        }

      if(time)
        usleep(floor(1000000 * time));
    }

  remove_queue_posix(CMDNAME);

  if(clk >= 0)
    remove_queue_posix(CLKNAME);

  for(i = 0; i < NUM_RANDOMS; i++)
    if(rnd[i] >= 0)
      {
        sprintf(portname, RNDNAME"(%d)", i);
        remove_queue_posix(portname);
      }

  for(i = 0; i < NUM_BUTTONS; i++)
    if(btn[i] >= 0)
      {
        sprintf(portname, BTNNAME"(%d)", i);
        remove_queue_posix(portname);
      }

  for(i = 0; i < NUM_SLIDERS; i++)
    if(sld[i] >= 0)
      {
        sprintf(portname, SLDNAME"(%d)", i);
        remove_queue_posix(portname);
      }

  if(msg >= 0)
    remove_queue_posix(MSGNAME);

  for(i = 0; i < NUM_LIGHTS; i++)
    if(lgt[i] >= 0)
      {
        sprintf(portname, LGTNAME"(%d)", i);
        remove_queue_posix(portname);
      }

  for(i = 0; i < NUM_DISPLAYS; i++)
    if(dsp[i] >= 0)
      {
        sprintf(portname, DSPNAME"(%d)", i);
        remove_queue_posix(portname);
      }

  if(slp >= 0)
    remove_queue_posix(SLPNAME);

  return EXIT_SUCCESS;
}


