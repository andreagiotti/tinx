
/*
  GTINXSH - Temporal Inference Network eXecutor Suite
  Design & coding by Andrea Giotti, 1998-1999
  Revised 2016-2018
*/

#define NDEBUG

#define ANSI_FILE_IO
/* #define UNIX_FILE_IO */
#define POSIX_IPC_IO
/* #define UNIX_IPC_IO */

#include "gtinxsh.h"

#define PACK_VER "4.8.1"
#define VER "1.3.2"

INLINE m_time get_time()
{
  struct timespec ts;

  clock_gettime(CLOCK_TYPE, &ts);

  return ts.tv_sec + 0.000000001 * ts.tv_nsec;
}

void plot(GtkWidget *widget, cairo_t *cr, s_base *sb, int x, int y, char truth)
{
  float rectw, recth;
  int height, offset;

  height = gtk_widget_get_allocated_height(widget);
  recth = height / (sb->fn + sb->gn);
  rectw = (gtk_widget_get_allocated_width(widget) - (sb->maxlen + 4) * ASPECT * min(FONT_SIZE * recth, MAX_FONT_PIXELS)) / HORIZON_SIZE;
  offset = (height - round(recth) * (sb->fn + sb->gn)) / 2;

  switch(truth)
   {
     case DISPLAY_HI_CHAR:
       cairo_rectangle(cr, round((x + BORDER_TRUE) * rectw), round(offset + (y + BORDER_TRUE) * recth), round((1 - 2 * BORDER_TRUE) * rectw), round((1 - 2 * BORDER_TRUE) * recth));
     break;

     case DISPLAY_LO_CHAR:
       cairo_rectangle(cr, round((x + BORDER_FALSE) * rectw), round(offset + (y + BORDER_FALSE) * recth), round((1 - 2 * BORDER_FALSE) * rectw), round((1 - 2 * BORDER_FALSE) * recth));
     break;

     case DISPLAY_UNKNOWN_CHAR:
     break;
   }

  cairo_fill(cr);
}

gboolean draw_callback(GtkWidget *widget, cairo_t *cr, s_base *sb)
{
  cairo_t *dr;
  cairo_surface_t *surface;
  int i, k, row, col;
  int width, height, offset;
  float recth, fonth;
  d_time t;

  width = gtk_widget_get_allocated_width(widget);
  height = gtk_widget_get_allocated_height(widget);

  surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
  dr = cairo_create(surface);

  if(sb->fn || sb->gn)
    {
      recth = height / (sb->fn + sb->gn);
      fonth = min(FONT_SIZE * recth, MAX_FONT_PIXELS);
      offset = (height - round(recth) * (sb->fn + sb->gn)) / 2;

      cairo_select_font_face(dr, "cairo : sans-serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
      cairo_set_font_size(dr, fonth);
    }
  else
   {
     recth = 0;
     fonth = 0;
     offset = 0;
   }

  cairo_set_source_rgb(dr, 0, 0, 0);
  cairo_rectangle(dr, 0, 0, width, height);
  cairo_fill(dr);

  t = sb->t;        /* mt cache */

  cairo_set_source_rgb(dr, 1, 0, 0);

  row = 0;
  for(i = 0; i < sb->fn; i++)
    {
      col = 1;
      for(k = t; k < t + (HORIZON_SIZE - 1); k++)
        plot(widget, dr, sb, col++, row, sb->memory_f[i][k % HORIZON_SIZE]);

      cairo_move_to(dr, round(width - (sb->maxlen + 1) * ASPECT * fonth), round(offset + (row + 0.5) * recth + fonth / 4));
      cairo_show_text(dr, sb->fnames[i]);

      row++;
    }

  cairo_set_source_rgb(dr, 0, 1, 0);

  for(i = 0; i < sb->gn; i++)
    {
      col = 1;
      for(k = t; k < t + (HORIZON_SIZE - 1); k++)
        plot(widget, dr, sb, col++, row, sb->memory_g[i][k % HORIZON_SIZE]);

      cairo_move_to(dr, round(width - (sb->maxlen + 1) * ASPECT * fonth), round(offset + (row + 0.5) * recth + fonth / 4));
      cairo_show_text(dr, sb->gnames[i]);

      if(sb->memory_g[i][(t + (HORIZON_SIZE - 2)) % HORIZON_SIZE] == DISPLAY_UNKNOWN_CHAR)
        {
          cairo_set_source_rgb(dr, 1, 1, 0);

          cairo_arc(dr, round(width - (sb->maxlen + 2.5) * ASPECT * fonth), round(offset + (row + 0.5) * recth), round(BALL_RATIO * fonth / 2), 0, 2 * G_PI);
          cairo_fill(dr);

          cairo_set_source_rgb(dr, 0, 1, 0);
        }

      row++;
    }

  cairo_destroy(dr);

  cairo_set_source_surface(cr, surface, 0, 0);
  cairo_paint(cr);

  cairo_surface_destroy(surface);

  return FALSE;
}

gboolean tick_callback(GtkWidget *widget, GdkFrameClock *frame_clock, s_base *sb)
{
  char timerstring[MAX_STRLEN_IF];
  cairo_t *cr;
  d_time t;
  m_time time;

  if(sb->rs != stopped)
    {
      cr = gdk_cairo_create(gtk_widget_get_window(widget));

      draw_callback(widget, cr, sb);

      cairo_destroy(cr);

      t = sb->t;        /* mt cache */
      time = sb->time;

      sprintf(timerstring, TIME_FMT" (%.3f s) %s %.3f s", t, t * sb->cp_step, ((t + 1) * sb->cp_step) < time? "<" : (((t - 1) * sb->cp_step > time)? ">" : "~"), time);
      gtk_label_set_label(sb->timer, timerstring);

      if(sb->cp_echo_stdout)
        g_idle_add((gboolean (*)(gpointer))update_view, sb);
    }

  return G_SOURCE_CONTINUE;
}

void tintloop(s_base *sb)
{
  d_time tau[MAX_FILES];
  m_time time_base;
  char ic, oc;
  int i;

  for(i = 0; i < sb->gn; i++)
    tau[i] = 0;

  sb->t = 0;

  time_base = get_time();

  for(;;)
    {
      sb->time = get_time() - time_base;
      if(sb->time >= sb->t * sb->cp_step * (1 + sb->correction))
        {
          if(!sb->batch)
            {
              for(i = 0; i < sb->gn; i++)
                {
                  while(tau[i] < sb->t)
                    {
                      if(sb->cp_file_io)
                        {
                          if(get_file(sb->gp[i], &ic) && file_error(sb->gp[i]))
                            {
                              print_error(sb, sb->gnames[i]);
                              pthread_exit(NULL);
                            }

                          if(ic == EOF)
                            {
                              reset_file(sb->gp[i]);
                              break;
                            }
                        }
                      else
                        {
                          read_message(sb->dp[i], &ic);

                          if(ic == EOF)
                            break;
                        }

                      switch(ic)
                        {
                          case UNKNOWN_CHAR:
                            sb->memory_g[i][tau[i] % HORIZON_SIZE] = DISPLAY_UNKNOWN_CHAR;
                          break;

                          case LO_CHAR:
                            sb->memory_g[i][tau[i] % HORIZON_SIZE] = DISPLAY_LO_CHAR;
                          break;

                          case HI_CHAR:
                            sb->memory_g[i][tau[i] % HORIZON_SIZE] = DISPLAY_HI_CHAR;
                          break;

                          case END_CHAR:
                            sb->memory_g[i][tau[i] % HORIZON_SIZE] = DISPLAY_UNKNOWN_CHAR;
                          break;

                          default:
                            print(sb, "%s, %c (dec %d): Invalid character in stream\n", sb->gnames[i], ic, ic);
                            pthread_exit(NULL);
                          break;
                        }

                      tau[i]++;
                    }
                }
            }

          if(sb->rs == stopping || (sb->cp_max_time && sb->t >= sb->cp_max_time))
            break;

          for(i = 0; i < sb->fn; i++)
            {
              oc = (rand() <= sb->prob * RAND_MAX)? HI_CHAR : LO_CHAR;

              if(sb->cp_file_io)
                {
                  if(put_file(sb->fp[i], &oc))
                    {
                      print_error(sb, sb->fnames[i]);
                      pthread_exit(NULL);
                    }

                  if(sync_file(sb->fp[i]))
                    {
                      print_error(sb, sb->fnames[i]);
                      pthread_exit(NULL);
                    }
                  }
                else
                  send_message(sb->cp[i], &oc);

              sb->memory_f[i][sb->t % HORIZON_SIZE] = (oc == LO_CHAR)? DISPLAY_LO_CHAR : DISPLAY_HI_CHAR;
            }

          for(i = 0; i < sb->gn; i++)
            sb->memory_g[i][sb->t % HORIZON_SIZE] = DISPLAY_UNKNOWN_CHAR;

          sb->t++;
        }
    }

  pthread_exit(NULL);
}

void tinxpipe(s_base *sb)
{
  FILE *fp;
  char ch[2];
  bool got;

  print(sb, "%s\n", sb->cmd);

  fp = popen(sb->cmd, "r");

  if(!fp)
    print_error(sb, "Error launching TINX\n");
  else
    {
      ch[1] = '\0';
      do
        {
          got = fread(ch, 1, sizeof(char), fp);
          if(ferror(fp))
            {
              print_error(sb, "Pipe");
              break;
            }

          if(got)
            {
              if(ch[0] == '%')
                print_add(sb, "%%");
              else
                print_add(sb, ch);
            }
        }
      while(!feof(fp));

      pclose(fp);
    }

  sb->term = TRUE;

  if(sb->rs == starting)
    sleep(DELAY);

  if(sb->rs == started)
    g_signal_emit_by_name(sb->run_button, "clicked");

  pthread_exit(NULL);
}

void run_button_clicked(GtkWidget *widget, s_base *sb)
{
  FILE *bp;
  char file_name[MAX_STRLEN], name[MAX_STRLEN];
  char cmd[MAX_STRLEN_IF], arg[MAX_STRLEN_IF];
  pthread_attr_t attributes;
  int i, k, len;
  char c, oc;
  pid_t pid;

  switch(sb->rs)
    {
      case stopped:
        sb->rs = starting;

        sb->cp_step = sb->step;
        sb->cp_max_time = sb->max_time;
        sb->cp_echo_stdout = sb->echo_stdout;
        sb->cp_file_io = sb->file_io;
        sb->cp_quiet = sb->quiet;

        if(!sb->cp_quiet)
          {
            for(i = 0; i < sb->fn; i++)
              {
                sb->fp[i] = NULL;
                sb->cp[i] = -1;
              }

            for(i = 0; i < sb->gn; i++)
              {
                sb->gp[i] = NULL;
                sb->dp[i] = -1;
              }

            strcpy(file_name, sb->base_name);
            strcat(file_name, NETWORK_EXT);

            bp = fopen(file_name, "r");
            if(!bp)
              {
                print_error(sb, file_name);
                sb->rs = stopped;
                return;
              }

            sb->fn = sb->gn = 0;
            sb->maxlen = 0;
            while(fscanf(bp, SKIP_FMT" "OP_FMT" "FUN_FMT, &c, name) == 2)
              {
                switch(c)
                  {
                    case '!':
                      if(sb->fn >= MAX_FILES)
                        {
                          print(sb, "%s: Too many input files\n", file_name);
                          sb->rs = stopped;
                          return;
                        }

                      if(sb->cp_file_io)
                        {
                          strcpy(sb->fnames[sb->fn], name);
                          strcat(sb->fnames[sb->fn], STREAM_EXT);

                          sb->fp[sb->fn] = open_output_file(sb->fnames[sb->fn]);
                          if(!is_file_open(sb->fp[sb->fn]))
                            {
                              print_error(sb, sb->fnames[sb->fn]);
                              sb->rs = stopped;
                              return;
                            }
                        }
                      else
                        {
                          strcpy(sb->fnames[sb->fn], MAGIC_PREFIX);
                          strcat(sb->fnames[sb->fn], name);

                          remove_queue(sb->fnames[sb->fn]);

                          sb->cp[sb->fn] = add_queue(sb->fnames[sb->fn], output_stream);
                          if(failed_queue(sb->cp[sb->fn]))
                            {
                              print_error(sb, sb->fnames[sb->fn]);
                              sb->rs = stopped;
                              return;
                            }
                        }

                      len = strlen(sb->fnames[sb->fn]);
                      if(sb->maxlen < len)
                        sb->maxlen = len;

                      sb->fn++;

                    break;

                    case '?':
                      if(sb->gn >= MAX_FILES)
                        {
                          print(sb, "%s: Too many output files\n", file_name);
                          sb->rs = stopped;
                          return;
                        }

                      if(sb->cp_file_io)
                        {
                          strcpy(sb->gnames[sb->gn], name);
                          strcat(sb->gnames[sb->gn], STREAM_EXT);

                          clean_file(sb->gnames[sb->gn]);
                        }
                      else
                        {
                          strcpy(sb->gnames[sb->gn], MAGIC_PREFIX);
                          strcat(sb->gnames[sb->gn], name);

                          remove_queue(sb->gnames[sb->gn]);
                        }

                      len = strlen(sb->gnames[sb->gn]);
                      if(sb->maxlen < len)
                        sb->maxlen = len;

                      sb->gn++;

                    break;

                    default:
                      print(sb, "%s, "OP_FMT": Invalid stream class\n", file_name, c);
                      sb->rs = stopped;
                      return;
                  }
              }

            if(ferror(bp))
              {
                print_error(sb, name);
                sb->rs = stopped;
                return;
              }

            if(fclose(bp))
              {
                print_error(sb, name);
                sb->rs = stopped;
                return;
              }
          }

        sb->mt = (sb->num_threads > 1);

        strcpy(cmd, CMD_PATH);
        strcat(cmd, sb->mt? "tinx_mt" : "tinx");
        strcat(cmd, " 2>&1");

        sprintf(arg, " -t "REAL_FMT, sb->step);
        strcat(cmd, arg);

        sprintf(arg, " -z "TIME_FMT, sb->max_time);
        strcat(cmd, arg);

        sprintf(arg, " -n %d", sb->num_threads);
        strcat(cmd, arg);

        sprintf(arg, " -r %d", sb->bsbt);
        strcat(cmd, arg);

        if(sb->load_state)
          {
            strcat(cmd, " -I ");
            strcat(cmd, sb->state_name);
          }

        if(sb->echo_logfile)
          {
            strcat(cmd, " -L ");
            strcat(cmd, sb->logfile_name);
          }

        if(sb->use_xref)
          {
            strcat(cmd, " -X ");
            strcat(cmd, sb->xref_name);
          }

        if(sb->strictly_causal)
          strcat(cmd, " -c");

        if(sb->soundness_check)
          strcat(cmd, " -v");

        if(sb->echo_stdout)
          strcat(cmd, " -d");

        if(sb->file_io)
          strcat(cmd, " -f");

        if(sb->quiet)
          strcat(cmd, " -q");

        if(sb->hard)
          strcat(cmd, " -s");

        if(sb->sturdy)
          strcat(cmd, " -y");

        strcat(cmd, " ");
        strcat(cmd, sb->base_name);

        strcpy(sb->cmd, cmd);

        pthread_attr_init(&attributes);

        sb->term = FALSE;

        if(pthread_create(&sb->tinxpipe, &attributes, (void *)&tinxpipe, (void *)sb))
          {
            print_error(sb, "POSIX thread error");
            sb->rs = stopped;
            return;
          }

        if(!sb->cp_quiet)
          {
            for(i = 0; i < sb->fn; i++)
              for(k = 0; k < HORIZON_SIZE; k++)
                sb->memory_f[i][k] = DISPLAY_UNKNOWN_CHAR;

            for(i = 0; i < sb->gn; i++)
              for(k = 0; k < HORIZON_SIZE; k++)
                sb->memory_g[i][k] = DISPLAY_UNKNOWN_CHAR;

            for(i = 0; i < sb->gn; i++)
              {
                if(sb->cp_file_io)
                  do
                    sb->gp[i] = open_input_file(sb->gnames[i]);
                  while(!sb->term && !is_file_open(sb->gp[i]));
                else
                  {
                    sb->dp[i] = add_queue(sb->gnames[i], input_stream);
                    if(failed_queue(sb->dp[i]))
                      {
                        print_error(sb, sb->gnames[i]);
                        sb->rs = stopped;
                        return;
                      }
                  }
              }

            if(!sb->term && pthread_create(&sb->tintloop, &attributes, (void *)&tintloop, (void *)sb))
              {
                print_error(sb, "POSIX thread error");
                sb->rs = stopped;
                return;
              }
          }

        gtk_button_set_label(GTK_BUTTON(widget), "Stop execution");

        sb->rs = started;
      break;

      case started:
        sb->rs = stopping;

        if(!sb->cp_quiet)
          {
            pthread_join(sb->tintloop, NULL);

            for(i = 0; i < sb->fn; i++)
              {
                oc = END_CHAR;

                if(sb->cp_file_io)
                  {
                    if(sb->fp[i])
                      {
                        if(put_file(sb->fp[i], &oc))
                          {
                            print_error(sb, sb->fnames[i]);
                            break;
                          }

                        if(close_file(sb->fp[i]))
                          {
                            print_error(sb, sb->fnames[i]);
                            break;
                          }
                      }
                  }
                else
                  {
                    if(!failed_queue(sb->cp[i]))
                      {
                        send_message(sb->cp[i], &oc);

                        if(commit_queue(sb->cp[i]))
                          {
                            print_error(sb, sb->fnames[i]);
                            break;
                          }
                      }
                  }
              }

            for(i = 0; i < sb->gn; i++)
              if(sb->cp_file_io)
                {
                  if(sb->gp[i] && close_file(sb->gp[i]))
                    {
                      print_error(sb, sb->gnames[i]);
                      break;
                    }
                }
              else
                {
                  if(!failed_queue(sb->dp[i]) && commit_queue(sb->dp[i]))
                    {
                      print_error(sb, sb->gnames[i]);
                      break;
                    }
                }
          }

        pid = pidof(sb, sb->mt? "tinx_mt" : "tinx");
        if(pid > 0)
          {
            kill(pid, SIGINT);
            waitpid(pid);
          }

        pthread_join(sb->tinxpipe, NULL);

        g_idle_add((gboolean (*)(gpointer))reset_view, sb);

        gtk_button_set_label(GTK_BUTTON(widget), "Execute network");

        sb->rs = stopped;
      break;

      default:
      break;
    }
}

pid_t pidof(s_base *sb, char *name)
{
  FILE *fp;
  char cmd[MAX_STRLEN_IF], buffer[MAX_STRLEN];
  pid_t pid;

  strcpy(cmd, "pidof ");
  strcat(cmd, name);

  pid = 0;

  fp = popen(cmd, "r");
  if(!fp)
    print(sb, "Error retrieving PID\n");
  else
    {
      fread(buffer, MAX_STRLEN, sizeof(char), fp);
      if(ferror(fp))
        print_error(sb, "Pipe");
      else
        pid = atoi(buffer);

      pclose(fp);
    }

  return pid;
}

void gen_button_clicked(GtkWidget *widget, s_base *sb)
{
  FILE *fp;
  char cmd[MAX_STRLEN_IF];
  char ch[2];
  bool got;

  strcpy(cmd, CMD_PATH"ting 2>&1 -o ");
  strcat(cmd, sb->base_name);
  strcat(cmd, " -I ");
  strcat(cmd, sb->state_name);

  if(sb->use_xref)
    {
      strcat(cmd, " -X ");
      strcat(cmd, sb->xref_name);
    }

  strcat(cmd, " ");
  strcat(cmd, sb->source_name);

  print(sb, "%s\n", cmd);

  fp = popen(cmd, "r");

  if(!fp)
    print_error(sb, "Error launching TING\n");
  else
    {
      ch[1] = '\0';
      do
        {
          got = fread(ch, 1, sizeof(char), fp);
          if(ferror(fp))
            {
              print_error(sb, "Pipe");
              break;
            }

          if(got)
            {
              if(ch[0] == '%')
                print_add(sb, "%%");
              else
                print_add(sb, ch);
            }
        }
      while(!feof(fp));

      pclose(fp);
    }
}

void about_button_clicked(GtkWidget *widget, s_base *sb)
{
  print(sb, BANNER);
}

void exit_button_clicked(GtkWidget *widget, s_base *sb)
{
  if(sb->rs == starting || sb->rs == stopping)
    sleep(DELAY);

  if(sb->rs == started)
    run_button_clicked(GTK_WIDGET(sb->run_button), sb);

  gtk_main_quit();
}

void load_state_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->load_state = TRUE;
  else
    sb->load_state = FALSE;
}

void soundness_check_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->soundness_check = TRUE;
  else
    sb->soundness_check = FALSE;
}

void strictly_causal_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->strictly_causal = TRUE;
  else
    sb->strictly_causal = FALSE;
}

void echo_stdout_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->echo_stdout = TRUE;
  else
    sb->echo_stdout = FALSE;
}

void echo_logfile_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->echo_logfile = TRUE;
  else
    sb->echo_logfile = FALSE;
}

void file_io_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->file_io = TRUE;
  else
    sb->file_io = FALSE;
}

void quiet_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->quiet = TRUE;
  else
    sb->quiet = FALSE;
}

void hard_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->hard = TRUE;
  else
    sb->hard = FALSE;
}

void sturdy_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->sturdy = TRUE;
  else
    sb->sturdy = FALSE;
}

void use_xref_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->use_xref = TRUE;
  else
    sb->use_xref = FALSE;
}

void source_fname(GtkWidget *widget, s_base *sb)
{
  strcpy(sb->source_name, gtk_entry_get_text(GTK_ENTRY(widget)));

  strcpy(sb->base_name, sb->source_name);
  strcpy(sb->state_name, sb->base_name);
  strcat(sb->state_name, STATE_SUFFIX);
  strcpy(sb->logfile_name, sb->base_name);
  strcat(sb->logfile_name, LOG_SUFFIX);
  strcpy(sb->xref_name, sb->base_name);

  gtk_widget_queue_draw(GTK_WIDGET(sb->window));
}

void base_fname(GtkWidget *widget, s_base *sb)
{
  strcpy(sb->base_name, gtk_entry_get_text(GTK_ENTRY(widget)));

  strcpy(sb->state_name, sb->base_name);
  strcat(sb->state_name, STATE_SUFFIX);
  strcpy(sb->logfile_name, sb->base_name);
  strcat(sb->logfile_name, LOG_SUFFIX);
  strcpy(sb->xref_name, sb->base_name);

  gtk_widget_queue_draw(GTK_WIDGET(sb->window));
}

void state_fname(GtkWidget *widget, s_base *sb)
{
  strcpy(sb->state_name, gtk_entry_get_text(GTK_ENTRY(widget)));
}

void logfile_fname(GtkWidget *widget, s_base *sb)
{
  strcpy(sb->logfile_name, gtk_entry_get_text(GTK_ENTRY(widget)));
}

void xref_fname(GtkWidget *widget, s_base *sb)
{
  strcpy(sb->xref_name, gtk_entry_get_text(GTK_ENTRY(widget)));
}

gboolean source_default(GtkWidget *widget, cairo_t *cr, s_base *sb)
{
  gtk_entry_set_text(GTK_ENTRY(widget), sb->source_name);

  return FALSE;
}

gboolean base_default(GtkWidget *widget, cairo_t *cr, s_base *sb)
{
  gtk_entry_set_text(GTK_ENTRY(widget), sb->base_name);

  return FALSE;
}

gboolean state_default(GtkWidget *widget, cairo_t *cr, s_base *sb)
{
  gtk_entry_set_text(GTK_ENTRY(widget), sb->state_name);

  return FALSE;
}

gboolean logfile_default(GtkWidget *widget, cairo_t *cr, s_base *sb)
{
  gtk_entry_set_text(GTK_ENTRY(widget), sb->logfile_name);

  return FALSE;
}

gboolean xref_default(GtkWidget *widget, cairo_t *cr, s_base *sb)
{
  gtk_entry_set_text(GTK_ENTRY(widget), sb->xref_name);

  return FALSE;
}

void source_dialog(GtkEntry *widget, GtkEntryIconPosition pos, GdkEvent *event, s_base *sb)
{
  GtkDialog *dialog;
  char *ext;

  dialog = GTK_DIALOG(gtk_file_chooser_dialog_new("Select temporal logic source file", sb->window, GTK_FILE_CHOOSER_ACTION_OPEN, "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL));

  if(gtk_dialog_run(dialog) == GTK_RESPONSE_ACCEPT)
    strcpy(sb->source_name, gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog)));

  gtk_widget_destroy(GTK_WIDGET(dialog));

  ext = strstr(sb->source_name, SOURCE_EXT);
  if(ext && !strcmp(ext, SOURCE_EXT))
    *ext = '\0';
}

void base_dialog(GtkEntry *widget, GtkEntryIconPosition pos, GdkEvent *event, s_base *sb)
{
  GtkDialog *dialog;
  char *ext;

  dialog = GTK_DIALOG(gtk_file_chooser_dialog_new("Select network object file", sb->window, GTK_FILE_CHOOSER_ACTION_SAVE, "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL));

  if(gtk_dialog_run(dialog) == GTK_RESPONSE_ACCEPT)
    strcpy(sb->base_name, gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog)));

  gtk_widget_destroy(GTK_WIDGET(dialog));

  ext = strstr(sb->base_name, NETWORK_EXT);
  if(ext && !strcmp(ext, NETWORK_EXT))
    *ext = '\0';
}

void state_dialog(GtkEntry *widget, GtkEntryIconPosition pos, GdkEvent *event, s_base *sb)
{
  GtkDialog *dialog;
  char *ext;

  dialog = GTK_DIALOG(gtk_file_chooser_dialog_new("Select initial conditions file", sb->window, GTK_FILE_CHOOSER_ACTION_SAVE, "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL));

  if(gtk_dialog_run(dialog) == GTK_RESPONSE_ACCEPT)
    strcpy(sb->state_name, gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog)));

  gtk_widget_destroy(GTK_WIDGET(dialog));

  ext = strstr(sb->state_name, EVENT_LIST_EXT);
  if(ext && !strcmp(ext, EVENT_LIST_EXT))
    *ext = '\0';
}

void logfile_dialog(GtkEntry *widget, GtkEntryIconPosition pos, GdkEvent *event, s_base *sb)
{
  GtkDialog *dialog;
  char *ext;

  dialog = GTK_DIALOG(gtk_file_chooser_dialog_new("Select log file", sb->window, GTK_FILE_CHOOSER_ACTION_SAVE, "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL));

  if(gtk_dialog_run(dialog) == GTK_RESPONSE_ACCEPT)
    strcpy(sb->logfile_name, gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog)));

  gtk_widget_destroy(GTK_WIDGET(dialog));

  ext = strstr(sb->logfile_name, EVENT_LIST_EXT);
  if(ext && !strcmp(ext, EVENT_LIST_EXT))
    *ext = '\0';
}

void xref_dialog(GtkEntry *widget, GtkEntryIconPosition pos, GdkEvent *event, s_base *sb)
{
  GtkDialog *dialog;
  char *ext;

  dialog = GTK_DIALOG(gtk_file_chooser_dialog_new("Select symbol table file", sb->window, GTK_FILE_CHOOSER_ACTION_SAVE, "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL));

  if(gtk_dialog_run(dialog) == GTK_RESPONSE_ACCEPT)
    strcpy(sb->xref_name, gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog)));

  gtk_widget_destroy(GTK_WIDGET(dialog));

  ext = strstr(sb->xref_name, XREF_EXT);
  if(ext && !strcmp(ext, XREF_EXT))
    *ext = '\0';
}

void step_value(GtkWidget *widget, s_base *sb)
{
  sb->step = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
}

void max_time_value(GtkWidget *widget, s_base *sb)
{
  sb->max_time = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
}

void num_threads_value(GtkWidget *widget, s_base *sb)
{
  sb->num_threads = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
}

void bsbt_value(GtkWidget *widget, s_base *sb)
{
  sb->bsbt = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
}

void prob_value(GtkWidget *widget, s_base *sb)
{
  sb->prob = gtk_range_get_value(GTK_RANGE(widget));
}

void correction_value(GtkWidget *widget, s_base *sb)
{
  sb->correction = gtk_range_get_value(GTK_RANGE(widget));
}

gboolean update_view(s_base *sb)
{
  char buffer[XBUFSIZE];
  int i, n;
  bool xcat;

  GtkTextBuffer *textbuffer;
  GtkTextMark *mark;
  GtkTextIter iter;

  pthread_mutex_lock(&sb->mutex);

  n = (sb->xend - sb->xstart + XBUFSIZE) % XBUFSIZE;

  for(i = 0; i < n; i++)
    buffer[i] = sb->xbuffer[(sb->xstart + i) % XBUFSIZE];

  sb->xstart = sb->xend;

  buffer[n] = '\0';

  xcat = sb->xcat;
  if(!xcat)
    sb->xcat = TRUE;

  pthread_mutex_unlock(&sb->mutex);

  textbuffer = gtk_text_view_get_buffer(sb->textarea);
  mark = gtk_text_buffer_get_mark(textbuffer, "end");

  if(!xcat)
    gtk_text_buffer_set_text(textbuffer, buffer, -1);
  else
    {
      gtk_text_buffer_get_iter_at_mark(textbuffer, &iter, mark);
      gtk_text_buffer_insert(textbuffer, &iter, buffer, -1);
    }

  gtk_text_view_scroll_mark_onscreen(sb->textarea, mark);

  return G_SOURCE_REMOVE;
}

gboolean reset_view(s_base *sb)
{
  sb->cp_echo_stdout = FALSE;

  update_view(sb);

  return G_SOURCE_REMOVE;
}

void print(s_base *sb, char *string, ...)
{
  char buffer[XBUFSIZE];
  va_list arglist;
  int i, n;
  bool flush;

  va_start(arglist, string);
  vsnprintf(buffer, XBUFSIZE, string, arglist);
  va_end(arglist);

  pthread_mutex_lock(&sb->mutex);

  sb->xstart = 0;
  sb->xend = 0;

  flush = FALSE;
  n = strlen(buffer);
  if(n < XBUFSIZE)
    {
      for(i = 0; i < n; i++)
        {
          sb->xbuffer[sb->xend] = buffer[i];
          if(buffer[i] == '\r' || buffer[i] == '\n')
            flush = TRUE;

          sb->xend = (sb->xend + 1) % XBUFSIZE;
          if(sb->xstart == sb->xend)
            sb->xstart = (sb->xstart + 1) % XBUFSIZE;
        }

      sb->xbuffer[sb->xend] = '\0';
    }
  else
    {
      strcpy(sb->xbuffer, "Buffer overflow");
      sb->xstart = 0;
      sb->xend = strlen(sb->xbuffer);
    }

  sb->xcat = FALSE;

  pthread_mutex_unlock(&sb->mutex);

  if(!sb->cp_echo_stdout && flush)
    g_idle_add((gboolean (*)(gpointer))update_view, sb);
}

void print_add(s_base *sb, char *string, ...)
{
  char buffer[XBUFSIZE];
  va_list arglist;
  int i, n;
  bool flush;

  va_start(arglist, string);
  vsnprintf(buffer, XBUFSIZE, string, arglist);
  va_end(arglist);

  pthread_mutex_lock(&sb->mutex);

  flush = FALSE;
  n = strlen(buffer);
  if(n < XBUFSIZE)
    {
      for(i = 0; i < n; i++)
        {
          sb->xbuffer[sb->xend] = buffer[i];
          if(buffer[i] == '\r' || buffer[i] == '\n')
            flush = TRUE;

          sb->xend = (sb->xend + 1) % XBUFSIZE;
          if(sb->xstart == sb->xend)
            sb->xstart = (sb->xstart + 1) % XBUFSIZE;
        }

      sb->xbuffer[sb->xend] = '\0';
    }
  else
    {
      strcpy(sb->xbuffer, "Buffer overflow");
      sb->xstart = 0;
      sb->xend = strlen(sb->xbuffer);
    }

  pthread_mutex_unlock(&sb->mutex);

  if(!sb->cp_echo_stdout && flush)
    g_idle_add((gboolean (*)(gpointer))update_view, sb);
}

int execute(char *source_name, char *base_name, char *state_name, char *logfile_name, char *xref_name,
    bool strictly_causal, bool soundness_check, bool echo_stdout, bool file_io, bool quiet, bool hard, bool sturdy, int bufexp, int max_time, m_time step, int num_threads, float prob, bool batch)
{
  s_base sbs;
  GtkWidget *window;
  GtkWidget *drawingarea;
  GtkWidget *textarea, *scrollarea;
  GtkWidget *btn1, *btn2, *btn3, *btn4;
  GtkWidget *cb0, *cb1, *cb2, *cb3, *cb4, *cb5, *cb6, *cb7, *cb8, *cb9, *cb10;
  GtkWidget *tabf, *tabi, *tabt, *tabh, *tabx, *tabk;
  GtkWidget *extbox, *hboxctl1, *hboxctl2;
  GtkWidget *lbl1, *ent1, *lbl2, *ent2, *lbl3, *ent3, *lbl4, *ent4, *lbl5, *ent5, *lbl6, *ent6, *lbl7, *ent7, *lbl8, *ent8, *lbl9, *ent9, *lbl10, *ent10, *lbl11, *ent11;
  GtkWidget *frgfx, *vboxgfx, *frtxt, *vboxtxt, *fr1, *vbox1, *fr2, *vbox2, *fr3, *vbox3, *fr4, *vbox4, *fr6, *vbox6;
  GtkWidget *vbox5, *hbox5, *lbl0, *lblt;
  GtkTextBuffer *textbuffer;
  GtkTextIter iter;

  memset(&sbs, 0, sizeof(s_base));

  strcpy(sbs.source_name, source_name);

  if(!base_name)
    base_name = source_name;

  strcpy(sbs.base_name, base_name);

  if(state_name)
    {
      strcpy(sbs.state_name, state_name);
      sbs.load_state = TRUE;
    }
  else
    {
      strcpy(sbs.state_name, base_name);
      strcat(sbs.state_name, STATE_SUFFIX);
      sbs.load_state = FALSE;
    }

  if(logfile_name)
    {
      strcpy(sbs.logfile_name, logfile_name);
      sbs.echo_logfile = TRUE;
    }
  else
    {
      strcpy(sbs.logfile_name, base_name);
      strcat(sbs.logfile_name, LOG_SUFFIX);
      sbs.echo_logfile = FALSE;
    }

  if(xref_name)
    {
      strcpy(sbs.xref_name, xref_name);
      sbs.use_xref = TRUE;
    }
  else
    {
      strcpy(sbs.xref_name, base_name);
      sbs.use_xref = FALSE;
    }

  sbs.bsbt = bufexp;
  sbs.step = step;
  sbs.max_time = max_time;
  sbs.num_threads = num_threads;
  sbs.prob = prob;
  sbs.correction = 0;
  sbs.strictly_causal = strictly_causal;
  sbs.soundness_check = soundness_check;
  sbs.echo_stdout = echo_stdout;
  sbs.file_io = file_io;
  sbs.quiet = quiet;
  sbs.hard = hard;
  sbs.sturdy = sturdy;
  sbs.batch = batch;

  sbs.t = 0;
  sbs.time = 0;
  sbs.rs = stopped;
  sbs.xcat = FALSE;

  gtk_init(NULL, NULL);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_WIDTH, WINDOW_HEIGHT);
  gtk_container_set_border_width(GTK_CONTAINER(window), 10);
  gtk_window_set_title(GTK_WINDOW(window), TITLE);
  gtk_window_set_icon_name(GTK_WINDOW(window), "tinx");
  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(exit_button_clicked), (gpointer)&sbs);

  sbs.window = GTK_WINDOW(window);

  extbox = gtk_vbox_new(FALSE, 0);

  frgfx = gtk_frame_new("History of input and output signals");
  gtk_label_set_markup(GTK_LABEL(gtk_frame_get_label_widget(GTK_FRAME(frgfx))), "<b>History of input and output signals</b>");
  gtk_frame_set_label_align(GTK_FRAME(frgfx), 0.5, 0.5);

  vboxgfx = gtk_vbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(frgfx), 5);
  gtk_container_set_border_width(GTK_CONTAINER(vboxgfx), 5);

  drawingarea = gtk_drawing_area_new();
  gtk_widget_set_size_request(drawingarea, WINDOW_WIDTH, GRAPHICS_HEIGHT);
  g_signal_connect(G_OBJECT(drawingarea), "draw", G_CALLBACK(draw_callback), &sbs);
  gtk_widget_add_tick_callback(drawingarea, (gboolean (*)(GtkWidget *, GdkFrameClock *, gpointer))tick_callback, &sbs, NULL);

  gtk_box_pack_start(GTK_BOX(vboxgfx), drawingarea, TRUE, TRUE, 0);
  gtk_container_add(GTK_CONTAINER(frgfx), vboxgfx);

  frtxt = gtk_frame_new("Diagnostic output messages");
  gtk_label_set_markup(GTK_LABEL(gtk_frame_get_label_widget(GTK_FRAME(frtxt))), "<b>Diagnostic output messages</b>");
  gtk_frame_set_label_align(GTK_FRAME(frtxt), 0.5, 0.5);

  vboxtxt = gtk_vbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(frtxt), 5);
  gtk_container_set_border_width(GTK_CONTAINER(vboxtxt), 5);

  textbuffer = gtk_text_buffer_new(NULL);
  gtk_text_buffer_get_end_iter(textbuffer, &iter);
  gtk_text_buffer_create_mark(textbuffer, "end", &iter, FALSE);
  textarea = gtk_text_view_new();
  gtk_text_view_set_editable(GTK_TEXT_VIEW(textarea), FALSE);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(textarea), FALSE);
  gtk_text_view_set_buffer(GTK_TEXT_VIEW(textarea), textbuffer);
  scrollarea = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_set_size_request(scrollarea, WINDOW_WIDTH, TEXT_HEIGHT);
  gtk_container_add(GTK_CONTAINER(scrollarea), textarea);

  sbs.textarea = GTK_TEXT_VIEW(textarea);
  print(&sbs, BANNER);

  gtk_box_pack_start(GTK_BOX(vboxtxt), scrollarea, TRUE, TRUE, 0);
  gtk_container_add(GTK_CONTAINER(frtxt), vboxtxt);

  hboxctl1 = gtk_hbox_new(FALSE, 0);
  hboxctl2 = gtk_hbox_new(FALSE, 0);

  fr1 = gtk_frame_new("Files");
  gtk_label_set_markup(GTK_LABEL(gtk_frame_get_label_widget(GTK_FRAME(fr1))), "<b>Files</b>");
  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(fr1), 5);
  gtk_container_set_border_width(GTK_CONTAINER(vbox1), 5);
  gtk_container_add(GTK_CONTAINER(fr1), vbox1);

  tabf = gtk_table_new(2, 5, FALSE);

  lbl1 = gtk_label_new("Temporal logic source file name ");
  gtk_misc_set_alignment(GTK_MISC(lbl1), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabf), lbl1, 0, 1, 0, 1);
  ent1 = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(ent1), 0);
  gtk_entry_set_width_chars(GTK_ENTRY(ent1), 64);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent1), 64);
  gtk_entry_set_icon_from_stock(GTK_ENTRY(ent1), GTK_ENTRY_ICON_SECONDARY, GTK_STOCK_FILE);
  gtk_entry_set_text(GTK_ENTRY(ent1), sbs.source_name);
  gtk_table_attach_defaults(GTK_TABLE(tabf), ent1, 1, 2, 0, 1);
  g_signal_connect(G_OBJECT(ent1), "changed", G_CALLBACK(source_fname), (gpointer)&sbs);
  g_signal_connect(G_OBJECT(ent1), "draw", G_CALLBACK(source_default), (gpointer)&sbs);
  g_signal_connect(G_OBJECT(ent1), "icon-press", G_CALLBACK(source_dialog), (gpointer)&sbs);

  lbl2 = gtk_label_new("Network object file name ");
  gtk_misc_set_alignment(GTK_MISC(lbl2), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabf), lbl2, 0, 1, 1, 2);
  ent2 = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(ent2), 0);
  gtk_entry_set_width_chars(GTK_ENTRY(ent2), 64);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent2), 64);
  gtk_entry_set_icon_from_stock(GTK_ENTRY(ent2), GTK_ENTRY_ICON_SECONDARY, GTK_STOCK_FILE);
  gtk_entry_set_text(GTK_ENTRY(ent2), sbs.base_name);
  gtk_table_attach_defaults(GTK_TABLE(tabf), ent2, 1, 2, 1, 2);
  g_signal_connect(G_OBJECT(ent2), "changed", G_CALLBACK(base_fname), (gpointer)&sbs);
  g_signal_connect(G_OBJECT(ent2), "draw", G_CALLBACK(base_default), (gpointer)&sbs);
  g_signal_connect(G_OBJECT(ent2), "icon-press", G_CALLBACK(base_dialog), (gpointer)&sbs);

  lbl3 = gtk_label_new("Initial conditions file name ");
  gtk_misc_set_alignment(GTK_MISC(lbl3), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabf), lbl3, 0, 1, 2, 3);
  ent3 = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(ent3), 0);
  gtk_entry_set_width_chars(GTK_ENTRY(ent3), 64);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent3), 64);
  gtk_entry_set_icon_from_stock(GTK_ENTRY(ent3), GTK_ENTRY_ICON_SECONDARY, GTK_STOCK_FILE);
  gtk_entry_set_text(GTK_ENTRY(ent3), sbs.state_name);
  gtk_table_attach_defaults(GTK_TABLE(tabf), ent3, 1, 2, 2, 3);
  g_signal_connect(G_OBJECT(ent3), "changed", G_CALLBACK(state_fname), (gpointer)&sbs);
  g_signal_connect(G_OBJECT(ent3), "draw", G_CALLBACK(state_default), (gpointer)&sbs);
  g_signal_connect(G_OBJECT(ent3), "icon-press", G_CALLBACK(state_dialog), (gpointer)&sbs);

  lbl4 = gtk_label_new("Log file name ");
  gtk_misc_set_alignment(GTK_MISC(lbl4), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabf), lbl4, 0, 1, 3, 4);
  ent4 = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(ent4), 0);
  gtk_entry_set_width_chars(GTK_ENTRY(ent4), 64);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent4), 64);
  gtk_entry_set_icon_from_stock(GTK_ENTRY(ent4), GTK_ENTRY_ICON_SECONDARY, GTK_STOCK_FILE);
  gtk_entry_set_text(GTK_ENTRY(ent4), sbs.logfile_name);
  gtk_table_attach_defaults(GTK_TABLE(tabf), ent4, 1, 2, 3, 4);
  g_signal_connect(G_OBJECT(ent4), "changed", G_CALLBACK(logfile_fname), (gpointer)&sbs);
  g_signal_connect(G_OBJECT(ent4), "draw", G_CALLBACK(logfile_default), (gpointer)&sbs);
  g_signal_connect(G_OBJECT(ent4), "icon-press", G_CALLBACK(logfile_dialog), (gpointer)&sbs);

  lbl10 = gtk_label_new("Symbol table file name ");
  gtk_misc_set_alignment(GTK_MISC(lbl10), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabf), lbl10, 0, 1, 4, 5);
  ent10 = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(ent10), 0);
  gtk_entry_set_width_chars(GTK_ENTRY(ent10), 64);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent10), 64);
  gtk_entry_set_icon_from_stock(GTK_ENTRY(ent10), GTK_ENTRY_ICON_SECONDARY, GTK_STOCK_FILE);
  gtk_entry_set_text(GTK_ENTRY(ent10), sbs.xref_name);
  gtk_table_attach_defaults(GTK_TABLE(tabf), ent10, 1, 2, 4, 5);
  g_signal_connect(G_OBJECT(ent10), "changed", G_CALLBACK(xref_fname), (gpointer)&sbs);
  g_signal_connect(G_OBJECT(ent10), "draw", G_CALLBACK(xref_default), (gpointer)&sbs);
  g_signal_connect(G_OBJECT(ent10), "icon-press", G_CALLBACK(xref_dialog), (gpointer)&sbs);

  gtk_container_add(GTK_CONTAINER(vbox1), tabf);
  gtk_box_pack_start(GTK_BOX(hboxctl1), fr1, TRUE, TRUE, 0);

  fr2 = gtk_frame_new("Inference");
  gtk_label_set_markup(GTK_LABEL(gtk_frame_get_label_widget(GTK_FRAME(fr2))), "<b>Inference</b>");
  vbox2 = gtk_vbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(fr2), 5);
  gtk_container_set_border_width(GTK_CONTAINER(vbox2), 5);
  gtk_container_add(GTK_CONTAINER(fr2), vbox2);

  tabi = gtk_table_new(4, 1, FALSE);

  cb1 = gtk_check_button_new_with_label("Load initial conditions");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb1), sbs.load_state);
  gtk_table_attach_defaults(GTK_TABLE(tabi), cb1, 0, 1, 0, 1);
  g_signal_connect(G_OBJECT(cb1), "clicked", G_CALLBACK(load_state_box), (gpointer)&sbs);

  cb2 = gtk_check_button_new_with_label("Verify inference soundness");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb2), sbs.soundness_check);
  gtk_table_attach_defaults(GTK_TABLE(tabi), cb2, 0, 1, 1, 2);
  g_signal_connect(G_OBJECT(cb2), "clicked", G_CALLBACK(soundness_check_box), (gpointer)&sbs);

  cb3 = gtk_check_button_new_with_label("Causal inference only");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb3), sbs.strictly_causal);
  gtk_table_attach_defaults(GTK_TABLE(tabi), cb3, 0, 1, 2, 3);
  g_signal_connect(G_OBJECT(cb3), "clicked", G_CALLBACK(strictly_causal_box), (gpointer)&sbs);

  cb10 = gtk_check_button_new_with_label("Use symbol table");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb10), sbs.use_xref);
  gtk_table_attach_defaults(GTK_TABLE(tabi), cb10, 0, 1, 3, 4);
  g_signal_connect(G_OBJECT(cb10), "clicked", G_CALLBACK(use_xref_box), (gpointer)&sbs);

  tabk = gtk_table_new(2, 2, FALSE);

  lbl7 = gtk_label_new("Number of processes ");
  gtk_misc_set_alignment(GTK_MISC(lbl7), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabk), lbl7, 0, 1, 0, 1);
  ent7 = gtk_spin_button_new_with_range(1, MAX_THREADS, 1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(ent7), sbs.num_threads);
  gtk_table_attach_defaults(GTK_TABLE(tabk), ent7, 1, 2, 0, 1);
  g_signal_connect(G_OBJECT(ent7), "value-changed", G_CALLBACK(num_threads_value), (gpointer)&sbs);

  lbl11 = gtk_label_new("Core size logarithm ");
  gtk_misc_set_alignment(GTK_MISC(lbl11), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabk), lbl11, 0, 1, 1, 2);
  ent11 = gtk_spin_button_new_with_range(MIN_BSBT, MAX_BSBT, 1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(ent11), sbs.bsbt);
  gtk_table_attach_defaults(GTK_TABLE(tabk), ent11, 1, 2, 1, 2);
  g_signal_connect(G_OBJECT(ent11), "value-changed", G_CALLBACK(bsbt_value), (gpointer)&sbs);

  gtk_box_pack_start(GTK_BOX(vbox2), tabi, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox2), tabk, TRUE, TRUE, 0);

  gtk_box_pack_start(GTK_BOX(hboxctl1), fr2, TRUE, TRUE, 0);

  fr3 = gtk_frame_new("Time");
  gtk_label_set_markup(GTK_LABEL(gtk_frame_get_label_widget(GTK_FRAME(fr3))), "<b>Time</b>");
  vbox3 = gtk_vbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(fr3), 5);
  gtk_container_set_border_width(GTK_CONTAINER(vbox3), 5);
  gtk_container_add(GTK_CONTAINER(fr3), vbox3);

  tabt = gtk_table_new(2, 2, FALSE);

  lbl5 = gtk_label_new("Sampling time (s) ");
  gtk_misc_set_alignment(GTK_MISC(lbl5), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabt), lbl5, 0, 1, 0, 1);
  ent5 = gtk_spin_button_new_with_range(0, 9999.999, 0.001);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(ent5), sbs.step);
  gtk_table_attach_defaults(GTK_TABLE(tabt), ent5, 1, 2, 0, 1);
  g_signal_connect(G_OBJECT(ent5), "value-changed", G_CALLBACK(step_value), (gpointer)&sbs);

  lbl6 = gtk_label_new("Horizon lenght (0 = none) ");
  gtk_misc_set_alignment(GTK_MISC(lbl6), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabt), lbl6, 0, 1, 1, 2);
  ent6 = gtk_spin_button_new_with_range(0, MAX_RUN_LEN, 1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(ent6), sbs.max_time);
  gtk_table_attach_defaults(GTK_TABLE(tabt), ent6, 1, 2, 1, 2);
  g_signal_connect(G_OBJECT(ent6), "value-changed", G_CALLBACK(max_time_value), (gpointer)&sbs);

  cb8 = gtk_check_button_new_with_label("Hard real time (root access only)");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb8), sbs.hard);
  g_signal_connect(G_OBJECT(cb8), "clicked", G_CALLBACK(hard_box), (gpointer)&sbs);

  gtk_box_pack_start(GTK_BOX(vbox3), tabt, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox3), cb8, TRUE, TRUE, 0);

  gtk_box_pack_start(GTK_BOX(hboxctl2), fr3, TRUE, TRUE, 0);

  fr4 = gtk_frame_new("Input and output");
  gtk_label_set_markup(GTK_LABEL(gtk_frame_get_label_widget(GTK_FRAME(fr4))), "<b>Input and output</b>");
  vbox4 = gtk_vbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(fr4), 5);
  gtk_container_set_border_width(GTK_CONTAINER(vbox4), 5);
  gtk_container_add(GTK_CONTAINER(fr4), vbox4);

  tabh = gtk_table_new(2, 3, FALSE);

  cb0 = gtk_radio_button_new_with_label(NULL, "IPC input and output");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb0), !sbs.quiet && !sbs.file_io);
  gtk_table_attach_defaults(GTK_TABLE(tabh), cb0, 0, 1, 0, 1);

  cb7 = gtk_radio_button_new_with_label(gtk_radio_button_get_group(GTK_RADIO_BUTTON(cb0)), "File input and output");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb7), sbs.file_io);
  gtk_table_attach_defaults(GTK_TABLE(tabh), cb7, 0, 1, 1, 2);
  g_signal_connect(G_OBJECT(cb7), "clicked", G_CALLBACK(file_io_box), (gpointer)&sbs);

  cb6 = gtk_radio_button_new_with_label(gtk_radio_button_get_group(GTK_RADIO_BUTTON(cb7)), "Quiet mode");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb6), sbs.quiet);
  gtk_table_attach_defaults(GTK_TABLE(tabh), cb6, 0, 1, 2, 3);
  g_signal_connect(G_OBJECT(cb6), "clicked", G_CALLBACK(quiet_box), (gpointer)&sbs);

  cb4 = gtk_check_button_new_with_label("Trace inference");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb4), sbs.echo_stdout);
  gtk_table_attach_defaults(GTK_TABLE(tabh), cb4, 1, 2, 0, 1);
  g_signal_connect(G_OBJECT(cb4), "clicked", G_CALLBACK(echo_stdout_box), (gpointer)&sbs);

  cb5 = gtk_check_button_new_with_label("Log inference to file");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb5), sbs.echo_logfile);
  gtk_table_attach_defaults(GTK_TABLE(tabh), cb5, 1, 2, 1, 2);
  g_signal_connect(G_OBJECT(cb5), "clicked", G_CALLBACK(echo_logfile_box), (gpointer)&sbs);

  cb9 = gtk_check_button_new_with_label("Sturdy IPC");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb9), sbs.sturdy);
  gtk_table_attach_defaults(GTK_TABLE(tabh), cb9, 1, 2, 2, 3);
  g_signal_connect(G_OBJECT(cb9), "clicked", G_CALLBACK(sturdy_box), (gpointer)&sbs);

  gtk_box_pack_start(GTK_BOX(vbox4), tabh, TRUE, TRUE, 0);

  gtk_box_pack_start(GTK_BOX(hboxctl2), fr4, TRUE, TRUE, 0);

  fr6 = gtk_frame_new("Execution");
  gtk_label_set_markup(GTK_LABEL(gtk_frame_get_label_widget(GTK_FRAME(fr6))), "<b>Execution</b>");
  vbox6 = gtk_vbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(fr6), 5);
  gtk_container_set_border_width(GTK_CONTAINER(vbox6), 5);
  gtk_container_add(GTK_CONTAINER(fr6), vbox6);

  tabx = gtk_table_new(2, 3, FALSE);

  lbl8 = gtk_label_new("Input truth probability ");
  gtk_misc_set_alignment(GTK_MISC(lbl8), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabx), lbl8, 0, 1, 0, 1);
  ent8 = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 1, 0.01);
  gtk_scale_set_value_pos(GTK_SCALE(ent8), GTK_POS_LEFT);
  gtk_scale_add_mark(GTK_SCALE(ent8), 0.5,  GTK_POS_TOP, "");
  gtk_scale_add_mark(GTK_SCALE(ent8), 0.5,  GTK_POS_BOTTOM, "");
  gtk_range_set_value(GTK_RANGE(ent8), sbs.prob);
  gtk_table_attach_defaults(GTK_TABLE(tabx), ent8, 1, 2, 0, 1);
  g_object_set(G_OBJECT(ent8), "width-request", BAR_WIDTH, NULL); 
  g_signal_connect(G_OBJECT(ent8), "value-changed", G_CALLBACK(prob_value), (gpointer)&sbs);

  lbl9 = gtk_label_new("Time correction ");
  gtk_misc_set_alignment(GTK_MISC(lbl9), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabx), lbl9, 0, 1, 1, 2);
  ent9 = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, -1, 1, 0.01);
  gtk_scale_set_value_pos(GTK_SCALE(ent9), GTK_POS_LEFT);
  gtk_scale_add_mark(GTK_SCALE(ent9), 0,  GTK_POS_TOP, NULL);
  gtk_scale_add_mark(GTK_SCALE(ent9), 0,  GTK_POS_BOTTOM, NULL);
  gtk_range_set_value(GTK_RANGE(ent9), sbs.correction);
  gtk_table_attach_defaults(GTK_TABLE(tabx), ent9, 1, 2, 1, 2);
  g_object_set(G_OBJECT(ent9), "width-request", BAR_WIDTH, NULL); 
  g_signal_connect(G_OBJECT(ent9), "value-changed", G_CALLBACK(correction_value), (gpointer)&sbs);

  lbl0 = gtk_label_new("Elapsed time ");
  gtk_misc_set_alignment(GTK_MISC(lbl0), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabx), lbl0, 0, 1, 2, 3);
  lblt = gtk_label_new("0 (0.000 s) ~ 0.000 s");
  gtk_misc_set_alignment(GTK_MISC(lblt), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabx), lblt, 1, 2, 2, 3);

  sbs.timer = GTK_LABEL(lblt);

  gtk_box_pack_start(GTK_BOX(vbox6), tabx, TRUE, TRUE, 0);

  gtk_box_pack_start(GTK_BOX(hboxctl2), fr6, TRUE, TRUE, 0);

  vbox5 = gtk_vbox_new(FALSE, 15);
  hbox5 = gtk_hbox_new(FALSE, 15);

  btn1 = gtk_button_new_with_label("Generate network");
  gtk_widget_set_size_request(btn1, 70, 30);
  gtk_box_pack_start(GTK_BOX(hbox5), btn1, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(btn1), "clicked", G_CALLBACK(gen_button_clicked), (gpointer)&sbs);

  btn2 = gtk_button_new_with_label("Execute network");
  gtk_widget_set_size_request(btn2, 70, 30);
  gtk_box_pack_start(GTK_BOX(hbox5), btn2, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(btn2), "clicked", G_CALLBACK(run_button_clicked), (gpointer)&sbs);

  sbs.run_button = GTK_BUTTON(btn2);

  btn3 = gtk_button_new_with_label("Exit");
  gtk_widget_set_size_request(btn3, 70, 30);
  gtk_box_pack_end(GTK_BOX(hbox5), btn3, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(btn3), "clicked", G_CALLBACK(exit_button_clicked), (gpointer)&sbs);

  btn4 = gtk_button_new_with_label("About");
  gtk_widget_set_size_request(btn4, 70, 30);
  gtk_box_pack_end(GTK_BOX(hbox5), btn4, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(btn4), "clicked", G_CALLBACK(about_button_clicked), (gpointer)&sbs);

  gtk_box_pack_end(GTK_BOX(vbox5), hbox5, FALSE, FALSE, 0);

  gtk_box_pack_start(GTK_BOX(extbox), frgfx, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(extbox), frtxt, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(extbox), hboxctl1, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(extbox), hboxctl2, TRUE, TRUE, 0);
  gtk_box_pack_end(GTK_BOX(extbox), vbox5, TRUE, TRUE, 0);

  gtk_container_add(GTK_CONTAINER(window), extbox);

  gtk_widget_show_all(window);

  gtk_main();

  return 0;
}

int main(int argc, char *argv[])
{
  char *source_name, *base_name, *state_name, *logfile_name, *xref_name, *option, *ext;
  char default_state_name[MAX_STRLEN], default_logfile_name[MAX_STRLEN], default_xref_name[MAX_STRLEN];
  bool strictly_causal, soundness_check, echo_stdout, file_io, quiet, hard, sturdy, batch;
  int i;
  d_time max_time;
  m_time step, default_step;
  int bufexp;
  int num_threads;
  float prob;

  source_name = base_name = state_name = logfile_name = xref_name = NULL;
  strictly_causal = soundness_check = echo_stdout = file_io = quiet = hard = sturdy = batch = FALSE;
  bufexp = DEFAULT_BUFEXP;
  max_time = 0;
  default_step = -1;
  num_threads = 1;
  prob = DEFAULT_PROB;

  for(i = 1; i < argc; i++)
    {
      if(*argv[i] == '-')
        {
          option = argv[i] + 1;
          switch(*option)
            {
            case 'h':
              fprintf(stderr, "Usage: %s [-bcdfilqsvxy] [-o base] [-I state] [-L log] [-X symbols] [-n processes] [-p probability] [-t step] [-z horizon] [source]\n",
                      argv[0]);
              exit(EXIT_SUCCESS);
            break;

            case 'I':
              if(*(++option))
                {
                  fprintf(stderr, "%s, %c: Invalid command line option"
                                  " (%s -h for help)\n",
                          argv[i], *option, argv[0]);
                  exit(EXIT_FAILURE);
                }

              if(state_name && state_name != default_state_name)
                {
                  fprintf(stderr, "%s: Duplicate option\n", argv[i]);
                  exit(EXIT_FAILURE);
                }

              if(++i < argc)
                state_name = argv[i];
              else
                {
                  fprintf(stderr, "%s: Missing argument\n", argv[--i]);
                  exit(EXIT_FAILURE);
                }

              ext = strstr(state_name, EVENT_LIST_EXT);
              if(ext && !strcmp(ext, EVENT_LIST_EXT))
                *ext = '\0';
            break;

            case 'L':
              if(*(++option))
                {
                  fprintf(stderr, "%s, %c: Invalid command line option"
                                  " (%s -h for help)\n",
                          argv[i], *option, argv[0]);
                  exit(EXIT_FAILURE);
                }

              if(logfile_name && logfile_name != default_logfile_name)
                {
                  fprintf(stderr, "%s: Duplicate option\n", argv[i]);
                  exit(EXIT_FAILURE);
                }

              if(++i < argc)
                logfile_name = argv[i];
              else
                {
                  fprintf(stderr, "%s: Missing argument\n", argv[--i]);
                  exit(EXIT_FAILURE);
                }

              ext = strstr(logfile_name, EVENT_LIST_EXT);
              if(ext && !strcmp(ext, EVENT_LIST_EXT))
                *ext = '\0';
            break;

            case 'n':
              if(*(++option))
                {
                  fprintf(stderr, "%s, %c: Invalid command line option"
                                  " (%s -h for help)\n",
                          argv[i], *option, argv[0]);
                  exit(EXIT_FAILURE);
                }

              if(++i < argc)
                {
                  num_threads = atoi(argv[i]);
                  if(num_threads < 1 || num_threads > MAX_THREADS)
                    {
                      fprintf(stderr, "%d: Argument out of range\n", num_threads);
                      exit(EXIT_FAILURE);
                    }
                }
              else
                {
                  fprintf(stderr, "%s: Missing argument\n", argv[--i]);
                  exit(EXIT_FAILURE);
                }
            break;

            case 'o':
              if(*(++option))
                {
                  fprintf(stderr, "%s, %c: Invalid command line option"
                                  " (%s -h for help)\n",
                          argv[i], *option, argv[0]);
                  exit(EXIT_FAILURE);
                }

              if(base_name)
                {
                  fprintf(stderr, "%s: Duplicate option\n", argv[i]);
                  exit(EXIT_FAILURE);
                }

              if(++i < argc)
                base_name = argv[i];
              else
                {
                  fprintf(stderr, "%s: Missing argument\n", argv[--i]);
                  exit(EXIT_FAILURE);
                }

              ext = strstr(base_name, NETWORK_EXT);
              if(ext && !strcmp(ext, NETWORK_EXT))
                *ext = '\0';
            break;

            case 'p':
              if(*(++option))
                {
                  fprintf(stderr, "%s, %c: Invalid command line option"
                                  " (%s -h for help)\n",
                          argv[i], *option, argv[0]);
                  exit(EXIT_FAILURE);
                }

              if(++i < argc)
                {
                  prob = atof(argv[i]);
                  if(prob < 0 || prob > 1)
                    {
                      fprintf(stderr, "%f: Argument out of range\n", prob);
                      exit(EXIT_FAILURE);
                    }
                }
              else
                {
                  fprintf(stderr, "%s: Missing argument\n", argv[--i]);
                  exit(EXIT_FAILURE);
                }
            break;

            case 'r':
              if(*(++option))
                {
                  fprintf(stderr, "%s, %c: Invalid command line option"
                                  " (%s -h for help)\n",
                          argv[i], *option, argv[0]);
                  exit(EXIT_FAILURE);
                }

              if(++i < argc)
                {
                  bufexp = atoi(argv[i]);
                  if(bufexp < MIN_BSBT || bufexp > MAX_BSBT)
                    {
                      fprintf(stderr, "%d: Argument out of range\n", bufexp);
                      exit(EXIT_FAILURE);
                    }
                } 
              else
                {
                  fprintf(stderr, "%s: Missing argument\n", argv[--i]);
                  exit(EXIT_FAILURE);
                }
            break;

            case 't':
              if(*(++option))
                {
                  fprintf(stderr, "%s, %c: Invalid command line option"
                                  " (%s -h for help)\n",
                          argv[i], *option, argv[0]);
                  exit(EXIT_FAILURE);
                }

              if(++i < argc)
                {
                  default_step = atof(argv[i]);
                  if(default_step < 0)
                    {
                      fprintf(stderr, "%f: Argument out of range\n", default_step);
                      exit(EXIT_FAILURE);
                    }
                }
              else
                {
                  fprintf(stderr, "%s: Missing argument\n", argv[--i]);
                  exit(EXIT_FAILURE);
                }
            break;

            case 'X':
              if(*(++option))
                {
                  fprintf(stderr, "%s, %c: Invalid command line option"
                                  " (%s -h for help)\n",
                          argv[i], *option, argv[0]);
                  exit(EXIT_FAILURE);
                }

              if(xref_name && xref_name != default_xref_name)
                {
                  fprintf(stderr, "%s: Duplicate option\n", argv[i]);
                  exit(EXIT_FAILURE);
                }

              if(++i < argc)
                xref_name = argv[i];
              else
                {
                  fprintf(stderr, "%s: Missing argument\n", argv[--i]);
                  exit(EXIT_FAILURE);
                }

              ext = strstr(xref_name, XREF_EXT);
              if(ext && !strcmp(ext, XREF_EXT))
                *ext = '\0';
            break;

            case 'z':
              if(*(++option))
                {
                  fprintf(stderr, "%s, %c: Invalid command line option"
                                  " (%s -h for help)\n",
                          argv[i], *option, argv[0]);
                  exit(EXIT_FAILURE);
                }

              if(++i < argc)
                {
                  max_time = atol(argv[i]);
                  if(max_time < 0 || max_time > MAX_RUN_LEN)
                    {
                      fprintf(stderr, TIME_FMT": Argument out of range\n", max_time);
                      exit(EXIT_FAILURE);
                    }
                }
              else
                {
                  fprintf(stderr, "%s: Missing argument\n", argv[--i]);
                  exit(EXIT_FAILURE);
                }
            break;

            default:
              do
                {
                  switch(*option)
                    {
                    case 'b':
                      batch = TRUE;
                    break;

                    case 'c':
                      strictly_causal = TRUE;
                    break;

                    case 'd':
                      echo_stdout = TRUE;
                    break;

                    case 'f':
                      file_io = TRUE;
                    break;

                    case 'i':
                      if(!state_name)
                        state_name = default_state_name;
                    break;

                    case 'l':
                      if(!logfile_name)
                        logfile_name = default_logfile_name;
                    break;

                    case 'q':
                      quiet = TRUE;
                    break;

                    case 's':
                      hard = TRUE;
                    break;

                    case 'v':
                      soundness_check = TRUE;
                    break;

                    case 'x':
                      if(!xref_name)
                        xref_name = default_xref_name;
                    break;

                    case 'y':
                      sturdy = TRUE;
                    break;

                    default:
                      fprintf(stderr, "%s, %c: Invalid command line option"
                                      " (%s -h for help)\n",
                              argv[i], *option, argv[0]);
                      exit(EXIT_FAILURE);
                    }
                }
              while(*(++option));
            }
        }
      else
        {
          if(!source_name)
            {
              source_name = argv[i];

              ext = strstr(source_name, SOURCE_EXT);
              if(ext && !strcmp(ext, SOURCE_EXT))
                *ext = '\0';
            }
          else
            fprintf(stderr, "%s: Extra argument ignored\n", argv[i]);
        }
    }

  if(!source_name)
    source_name = DEFAULT_NAME;

  if(!base_name)
    base_name = source_name;

  if(state_name == default_state_name)
    {
      strcpy(state_name, base_name);
      strcat(state_name, STATE_SUFFIX);
    }

  if(logfile_name == default_logfile_name)
    {
      strcpy(logfile_name, base_name);
      strcat(logfile_name, LOG_SUFFIX);
    }

  if(xref_name == default_xref_name)
    strcpy(xref_name, base_name);

  if(default_step < 0)
    step = quiet? 0 : DEFAULT_STEP_SEC;
  else
    step = default_step;

  return execute(source_name, base_name, state_name, logfile_name, xref_name,
      strictly_causal, soundness_check, echo_stdout, file_io, quiet, hard, sturdy, bufexp, max_time, step, num_threads, prob, batch);
}


