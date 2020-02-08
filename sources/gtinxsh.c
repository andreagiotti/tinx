
/*
  GTINXSH - Temporal Inference Network eXecutor Suite
  Design & coding by Andrea Giotti, 1998-1999
  Revised 2016-2019
*/

#define NDEBUG

#define ANSI_FILE_IO
/* #define UNIX_FILE_IO */

#include "gtinxsh.h"

#define PACK_VER "9.2.0"
#define VER "4.1.0"

INLINE m_time get_time()
{
  struct timespec ts;

  clock_gettime(CLOCK_TYPE, &ts);

  return ts.tv_sec + 0.000000001 * ts.tv_nsec;
}

void plot(cairo_t *cr, s_base *sb, int x, int y, double offset_x, double offset_y, double width, double height, char truth, bool text)
{
  double rectw, recth, fonth;
  char buffer[2];
  cairo_text_extents_t te;

  recth = (double)height / min(sb->fn + sb->gn, sb->cp_display_rows);
  rectw = (double)width / (sb->cp_horizon_size - 1);

  if(!text)
    {
      switch(truth)
       {
         case DISPLAY_HI_CHAR:
           cairo_rectangle(cr, offset_x + (x + BORDER_TRUE) * rectw, offset_y + (y + BORDER_TRUE) * recth, (1 - 2 * BORDER_TRUE) * rectw, (1 - 2 * BORDER_TRUE) * recth);
         break;

         case DISPLAY_LO_CHAR:
           cairo_rectangle(cr, offset_x + (x + BORDER_FALSE) * rectw, offset_y + (y + BORDER_FALSE) * recth, (1 - 2 * BORDER_FALSE) * rectw, (1 - 2 * BORDER_FALSE) * recth);
         break;

         case DISPLAY_UNKNOWN_CHAR:
           if(sb->cfg.draw_undef)
             cairo_rectangle(cr, offset_x + (x + 0.5) * rectw - 0.5, offset_y + (y + 0.5) * recth - 0.5, 1, 1);
         break;

         case DISPLAY_EMPTY_CHAR:
         break;
       }

      cairo_fill(cr);
    }
  else
    if(isprint(truth))
      {
        fonth = min(FONT_SIZE * recth, MAX_FONT_PIXELS);
        buffer[0] = truth;
        buffer[1] = '\0';

        cairo_text_extents(cr, buffer, &te);

        cairo_move_to(cr, offset_x + (x + 0.5) * rectw - te.width / 2 - te.x_bearing, offset_y + (y + 0.5) * recth + fonth / 4);
        cairo_show_text(cr, buffer);
      }
}

gboolean draw_callback(GtkWidget *widget, cairo_t *cr, s_base *sb)
{
  cairo_t *dr;
  cairo_surface_t *surface;
  int i, k, row, col, fn0, gn0, fn1, gn1, fpos, gpos;
  int width, height;
  double offset, offset_x1, offset_x2, recth, fonth, fontw;
  d_time t;
  char c;

  t = sb->t;        /* mt cache */
  fpos = sb->pos;
  gpos = max(0, fpos - sb->fn);

  width = gtk_widget_get_allocated_width(widget);
  height = gtk_widget_get_allocated_height(widget);

  surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, width, height);
  dr = cairo_create(surface);

  if(sb->fn || sb->gn)
    {
      fn0 = max(0, sb->fn - fpos);
      fn1 = min(fn0, sb->cp_display_rows);

      gn0 = max(0, sb->gn - gpos);
      gn1 = min(gn0, sb->cp_display_rows - fn1);

      offset = min(FONT_SIZE * (double)height / (fn1 + gn1), MAX_FONT_PIXELS);

      recth = (height - 2 * offset) / (fn1 + gn1);
      fonth = min(FONT_SIZE * recth, MAX_FONT_PIXELS);
      fontw = ASPECT * fonth;

      offset_x1 = width - offset - (sb->maxlen + 3) * fontw;
      offset_x2 = width - offset - sb->maxlen * fontw;

      cairo_select_font_face(dr, "cairo : sans-serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
      cairo_set_font_size(dr, fonth);
    }
  else
    {
      fn1 = 0;
      gn1 = 0;
      offset = 0;
      recth = 0;
      fonth = 0;
      fontw = 0;
      offset_x1 = 0;
      offset_x2 = 0;
    }

  cairo_set_source_rgb(dr, 0, 0, 0);
  cairo_rectangle(dr, 0, 0, width, height);
  cairo_fill(dr);

  row = 0;
  for(i = 0; i < fn1; i++)
    {
      if(!sb->fpacked[fpos + i])
        cairo_set_source_rgb(dr, 1, 0, 0);
      else
        cairo_set_source_rgb(dr, 1, 0.5, 0);

      col = 0;
      for(k = t; k < t + (sb->cp_horizon_size - 1); k++)
        plot(dr, sb, col++, row, offset, offset, offset_x1 - offset, height - 2 * offset, sb->memory_f[fpos + i][k % sb->cp_horizon_size], sb->fpacked[fpos + i]);

      cairo_move_to(dr, offset_x2, offset + (row + 0.5) * recth + fonth / 4);
      cairo_show_text(dr, sb->cp_full_names? sb->fnames_full[fpos + i] : sb->fnames[fpos + i]);

      row++;
    }

  for(i = 0; i < gn1; i++)
    {
      if(sb->gaux[gpos + i])
        cairo_set_source_rgb(dr, 0, 0.75, 0.75);
      else
        if(!sb->gpacked[gpos + i])
          cairo_set_source_rgb(dr, 0, 1, 0);
        else
          cairo_set_source_rgb(dr, 1, 1, 0);

      col = 0;
      for(k = t; k < t + (sb->cp_horizon_size - 1); k++)
        plot(dr, sb, col++, row, offset, offset, offset_x1 - offset, height - 2 * offset, sb->memory_g[gpos + i][k % sb->cp_horizon_size], sb->gpacked[gpos + i]);

      cairo_move_to(dr, offset_x2, offset + (row + 0.5) * recth + fonth / 4);
      cairo_show_text(dr, sb->cp_full_names? sb->gnames_full[gpos + i] : sb->gnames[gpos + i]);

      c = sb->memory_g[gpos + i][(t + (sb->cp_horizon_size - 2)) % sb->cp_horizon_size];

      if(!sb->gsync[gpos + i] || (!sb->gpacked[gpos + i] && c == DISPLAY_UNKNOWN_CHAR))
        {
          cairo_set_source_rgb(dr, 1, 1, 0);

          cairo_arc(dr, (offset_x1 + offset_x2) / 2, offset + (row + 0.5) * recth, BALL_RATIO * fonth / 2, 0, 2 * G_PI);
          cairo_fill(dr);

          if(!sb->gpacked[gpos + i] && c == DISPLAY_UNKNOWN_CHAR)
            {
              cairo_set_source_rgb(dr, 0, 0, 0);

              cairo_arc(dr, (offset_x1 + offset_x2) / 2, offset + (row + 0.5) * recth, RING_RATIO * fonth / 2, 0, 2 * G_PI);
              cairo_fill(dr);
            }
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
  if(sb->rs != stopped)
    {
      update_drawing(sb);

      if(sb->cp_echo_stdout)
        g_idle_add((gboolean (*)(gpointer))update_view, sb);
    }

  return G_SOURCE_CONTINUE;
}

void update_drawing(s_base *sb)
{
  char timerstring[MAX_STRLEN_IF];
  cairo_t *cr;
  d_time t;
  m_time time;

  cr = gdk_cairo_create(gtk_widget_get_window(GTK_WIDGET(sb->drawingarea)));

  draw_callback(GTK_WIDGET(sb->drawingarea), cr, sb);

  cairo_destroy(cr);

  t = sb->t;        /* mt cache */
  time = sb->time;

  sprintf(timerstring, TIME_FMT" ("REAL_FMT" s) %s "REAL_FMT" s", t, t * sb->cp_step, ((t + 1) * sb->cp_step) < time? "<" : (((t - 1) * sb->cp_step > time)? ">" : "~"), time);
  gtk_label_set_label(sb->timer, timerstring);
}

void tintloop(s_base *sb)
{
  bool sync_char[MAX_FILES][MAX_HORIZON_SIZE];
  d_time tau_f[MAX_FILES], tau_g[MAX_FILES];
  char ic, oc;
  int i;
  bool emit;

  for(i = 0; i < sb->fn; i++)
    tau_f[i] = 0;

  for(i = 0; i < sb->gn; i++)
    tau_g[i] = 0;

  sb->t = 0;

  while(sb->rs != stopping && (!sb->cp_max_time || sb->t < sb->cp_max_time))
    {
      sb->time = get_time() - sb->time_base;
      if(sb->time >= (sb->t + 1) * sb->cp_step * (1 + sb->cfg.correction))
        {
          sb->t++;

          for(i = 0; i < sb->gn; i++)
            {
              sb->memory_g[i][(sb->t + (sb->cp_horizon_size - 1)) % sb->cp_horizon_size] = DISPLAY_EMPTY_CHAR;
              sync_char[i][(sb->t + (sb->cp_horizon_size - 1)) % sb->cp_horizon_size] = FALSE;
            }
        }

      if(!sb->cp_batch_in)
        {
          for(i = 0; i < sb->fn; i++)
            {
              if(tau_f[i] < sb->t)
                {
                  oc = sb->cfg.alpha[rand() <= sb->cfg.prob * sb->cfg.inprob[i] * RAND_MAX? true_symbol : false_symbol];

                  if(sb->ffile_io[i])
                    {
                      if(put_file(sb->fp[i], &oc))
                        {
                          print_error(sb, sb->fnames_full[i]);
                          pthread_exit(NULL);
                        }

                      if(sync_file(sb->fp[i]))
                        {
                          print_error(sb, sb->fnames_full[i]);
                          pthread_exit(NULL);
                        }
                    }
                  else
                    if(sb->cp_sys5? send_message_sys5(sb->cp5[i], &oc) : send_message_posix(sb->cp[i], &oc))
                      {
                        if(errno != EAGAIN)
                          {
                            print_error(sb, sb->fnames_full[i]);
                            pthread_exit(NULL);
                          }

                        oc = EOF;
                      }

                  if(oc != EOF)
                    {
                      if(!sb->fpacked[i])
                        sb->memory_f[i][tau_f[i] % sb->cp_horizon_size] = (oc == sb->cfg.alpha[false_symbol])? DISPLAY_LO_CHAR : DISPLAY_HI_CHAR;
                      else
                        sb->memory_f[i][tau_f[i] % sb->cp_horizon_size] = oc;

                      tau_f[i]++;
                    }
                }
            }
        }

      if(!sb->cp_batch_out)
        {
          for(i = 0; i < sb->gn; i++)
            {
              if(tau_g[i] < sb->t)
                {
                  if(sb->gfile_io[i])
                    {
                      if(get_file(sb->gp[i], &ic) && file_error(sb->gp[i]))
                        {
                          print_error(sb, sb->gnames_full[i]);
                          pthread_exit(NULL);
                        }

                      if(ic == EOF)
                        reset_file(sb->gp[i]);
                    }
                  else
                    {
                      if(sb->cp_sys5? read_message_sys5(sb->dp5[i], &ic) && errno != ENOMSG : read_message_posix(sb->dp[i], &ic) && errno != EAGAIN)
                        {
                          print_error(sb, sb->gnames_full[i]);
                          pthread_exit(NULL);
                        }
                    }

                  if(ic != EOF)
                    {
                      if(!sb->gpacked[i])
                        {
                          switch(strchr(sb->cfg.alpha, ic) - sb->cfg.alpha)
                            {
                              case unknown_symbol:
                                sb->memory_g[i][tau_g[i] % sb->cp_horizon_size] = DISPLAY_UNKNOWN_CHAR;
                              break;

                              case false_symbol:
                                sb->memory_g[i][tau_g[i] % sb->cp_horizon_size] = DISPLAY_LO_CHAR;
                              break;

                              case true_symbol:
                                sb->memory_g[i][tau_g[i] % sb->cp_horizon_size] = DISPLAY_HI_CHAR;
                              break;

                              case end_symbol:
                                sb->memory_g[i][tau_g[i] % sb->cp_horizon_size] = DISPLAY_EMPTY_CHAR;
                              break;

                              default:
                                print(sb, "%s, %c (dec %d): Invalid character in stream\n", sb->gnames_full[i], ic, ic);
                                pthread_exit(NULL);
                              break;
                            }
                        }
                      else
                        sb->memory_g[i][tau_g[i] % sb->cp_horizon_size] = ic;

                      sync_char[i][tau_g[i] % sb->cp_horizon_size] = TRUE;

                      tau_g[i]++;
                    }
                  else
                    if(sb->gomit[i] && sb->time >= (tau_g[i] + 1) * sb->cp_step * (1 + sb->cfg.correction))
                      tau_g[i]++;
                }

              sb->gsync[i] = sync_char[i][(sb->t + (sb->cp_horizon_size - 2)) % sb->cp_horizon_size];
            }
        }
    }

  while(sb->rs == starting)
    usleep(DELAY);

  pthread_mutex_lock(&sb->mutex_sent);

  if(sb->rs == started && !sb->sent)
    {
      sb->sent = TRUE;
      emit = TRUE;
    }
  else
    emit = FALSE;

  pthread_mutex_unlock(&sb->mutex_sent);

  if(emit)
    g_signal_emit_by_name(sb->dummy_button, "clicked");

  pthread_exit(NULL);
}

void tinxpipe(s_base *sb)
{
  FILE *fp;
  char ch[2];
  bool got, emit;

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

  while(sb->rs == starting)
    usleep(DELAY);

  pthread_mutex_lock(&sb->mutex_sent);

  if(sb->rs == started && !sb->sent)
    {
      sb->sent = TRUE;
      emit = TRUE;
    }
  else
    emit = FALSE;

  pthread_mutex_unlock(&sb->mutex_sent);

  if(emit)
    g_signal_emit_by_name(sb->dummy_button, "clicked");

  pthread_exit(NULL);
}

gboolean flip_controls(s_base *sb)
{
  if(sb->rs == starting || sb->rs == started)
    {
      gtk_button_set_label(sb->run_button, "Stop execution");
      gtk_menu_item_set_label(sb->run_menu, "Stop execution");
    }
  else
    {
      gtk_button_set_label(sb->run_button, "Execute network");
      gtk_menu_item_set_label(sb->run_menu, "Execute network");
    }

  return G_SOURCE_REMOVE;
}

void dummy_button_clicked(GtkWidget *widget, s_base *sb)
{
  FILE *bp;
  char file_name[MAX_STRLEN], name[MAX_STRLEN];
  char cmd[MAX_STRLEN_IF], arg[MAX_STRLEN_IF];
  pthread_attr_t attributes;
  int i, k, len, count, tot_rows, page_rows;
  int packed, packedbit;
  io_type_4 omissions;
  char c, ic, oc;
  io_type stype;
  pid_t pid;
  m_time halt;
  bool file_io;

  switch(sb->rs)
    {
      case stopped:
        sb->rs = starting;

        sb->cp_step = sb->cfg.step;
        sb->cp_max_time = sb->cfg.max_time;
        sb->cp_echo_stdout = sb->cfg.echo_stdout;
        sb->cp_file_io = sb->cfg.file_io;
        sb->cp_quiet = sb->cfg.quiet;
        sb->cp_sys5 = sb->cfg.sys5;
        sb->cp_batch_in = sb->cfg.batch_in;
        sb->cp_batch_out = sb->cfg.batch_out;
        sb->cp_full_names = sb->cfg.full_names;
        sb->cp_horizon_size = sb->cfg.horizon_size;
        sb->cp_display_rows = sb->cfg.display_rows;

        if(!sb->cp_quiet)
          {
            for(i = 0; i < sb->fn; i++)
              {
                sb->fp[i] = NULL;
                sb->cp[i] = -1;
                sb->cp5[i].paddr = -1;
                sb->cp5[i].saddr = -1;
              }

            for(i = 0; i < sb->gn; i++)
              {
                sb->gp[i] = NULL;
                sb->dp[i] = -1;
                sb->dp5[i].paddr = -1;
                sb->dp5[i].saddr = -1;
              }

            strcpy(file_name, sb->cfg.base_name);
            strcat(file_name, NETWORK_EXT);

            bp = fopen(file_name, "r");
            if(!bp)
              {
                print_error(sb, file_name);
                sb->rs = stopped;
                return;
              }

            sb->fn = 0;
            sb->gn = 0;
            sb->pos = 0;
            sb->maxlen = 0;

            sb->cfg.fn = 0;
            sb->cfg.gn = 0;

            stype = io_any;
            packed = 0;
            packedbit = 0;
            omissions = io_raw;

            fscanf(bp, " "SKIP_FMT" ");

            if(ferror(bp))
              {
                print_error(sb, file_name);
                sb->rs = stopped;
                return;
              }

            file_io = TRUE;
            while(fscanf(bp, " "OP_FMT" "FUN_FMT" ( %*[^"SEPARATORS"] , %*[^"SEPARATORS"] ) # %*d / %u , %u , %u , %*u, %u @ %*ld ", &c, name, &stype, &packed, &packedbit, &omissions) >= 2)
              {
                if(!packed || !packedbit)
                  {
                    switch(c)
                      {
                        case '!':
                          if(!sb->cp_batch_in && !sb->cfg.fexcl[sb->cfg.fn++])
                            {
                              if(sb->fn >= MAX_FILES)
                                {
                                  print(sb, "%s: Too many input signals\n", file_name);
                                  sb->rs = stopped;
                                  return;
                                }

                              strcpy(sb->fnames[sb->fn], name);

                              sb->ffile_io[sb->fn] = (sb->cp_file_io && stype == io_any) || stype == io_file;
                              file_io &= sb->ffile_io[sb->fn];

                              if(sb->ffile_io[sb->fn])
                                {
                                  if(*sb->cfg.path)
                                    {
                                      strcpy(sb->fnames_full[sb->fn], sb->cfg.path);
                                      strcat(sb->fnames_full[sb->fn], "/");
                                    }
                                  else
                                    *sb->fnames_full[sb->fn] = '\0';

                                  strcat(sb->fnames_full[sb->fn], name);
                                  strcat(sb->fnames_full[sb->fn], STREAM_EXT);
                                }
                              else
                                {
                                  strcpy(sb->fnames_full[sb->fn], sb->cfg.prefix);
                                  strcat(sb->fnames_full[sb->fn], name);

                                  if(!sb->cp_sys5)
                                    remove_queue_posix(sb->fnames_full[sb->fn]);
                                }

                              sb->fpacked[sb->fn] = packed;

                              len = strlen(sb->cfg.full_names? sb->fnames_full[sb->fn] : sb->fnames[sb->fn]);
                              if(sb->maxlen < len)
                                sb->maxlen = len;

                              sb->fn++;
                            }
                        break;

                        case '?':
                        case '.':
                          if(!sb->cp_batch_out && !sb->cfg.gexcl[sb->cfg.gn++])
                            {
                              if(sb->gn >= MAX_FILES)
                                {
                                  print(sb, "%s: Too many output signals\n", file_name);
                                  sb->rs = stopped;
                                  return;
                                }

                              strcpy(sb->gnames[sb->gn], name);

                              sb->gfile_io[sb->gn] = (sb->cp_file_io && stype == io_any) || stype == io_file;
                              file_io &= sb->gfile_io[sb->gn];

                              if(sb->gfile_io[sb->gn])
                                {
                                  if(*sb->cfg.path)
                                    {
                                      strcpy(sb->gnames_full[sb->gn], sb->cfg.path);
                                      strcat(sb->gnames_full[sb->gn], "/");
                                    }
                                  else
                                    *sb->gnames_full[sb->gn] = '\0';

                                  strcat(sb->gnames_full[sb->gn], name);
                                  strcat(sb->gnames_full[sb->gn], STREAM_EXT);

                                  clean_file(sb->gnames_full[sb->gn]);
                                }
                              else
                                {
                                  strcpy(sb->gnames_full[sb->gn], sb->cfg.prefix);
                                  strcat(sb->gnames_full[sb->gn], name);

                                  if(!sb->cp_sys5)
                                    remove_queue_posix(sb->gnames_full[sb->gn]);
                                }

                              sb->gpacked[sb->gn] = packed;
                              sb->gomit[sb->gn] = (omissions != io_raw);
                              sb->gaux[sb->gn] = (c == '.');

                              len = strlen(sb->cfg.full_names? sb->gnames_full[sb->gn] : sb->gnames[sb->gn]);
                              if(sb->maxlen < len)
                                sb->maxlen = len;

                              sb->gn++;
                            }
                        break;

                        default:
                          print(sb, "%s, "OP_FMT": Invalid stream class\n", file_name, c);
                          sb->rs = stopped;
                        return;
                      }
                  }

                stype = io_any;
                packed = 0;
                packedbit = 0;
                omissions = io_raw;
              }

            if(ferror(bp))
              {
                print_error(sb, file_name);
                sb->rs = stopped;
                return;
              }

            if(!feof(bp))
              {
                print(sb, "%s: Parser error\n", file_name);
                sb->rs = stopped;
                return;
               }

            if(fclose(bp))
              {
                print_error(sb, file_name);
                sb->rs = stopped;
                return;
              }

            if(!file_io && sb->cp_sys5)
              delete_queues_sys5();

            if(!sb->cp_batch_in)
              {
                for(i = 0; i < sb->fn; i++)
                  {
                    if(sb->ffile_io[i])
                      {
                        sb->fp[i] = open_output_file(sb->fnames_full[i]);
                        if(!is_file_open(sb->fp[i]))
                          {
                            print_error(sb, sb->fnames_full[i]);
                            sb->rs = stopped;
                            return;
                          }
                      }
                    else
                      if(!sb->cp_sys5)
                        {
                          sb->cp[i] = add_queue_posix(sb->fnames_full[i], output_stream);
                          if(failed_queue_posix(sb->cp[i]))
                            {
                              print_error(sb, sb->fnames_full[i]);
                              sb->rs = stopped;
                              return;
                            }
                        }
                      else
                        {
                          sb->cp5[i] = add_queue_sys5(sb->fnames_full[i], input_stream);
                          if(failed_queue_sys5(sb->cp5[i]))
                            {
                              print_error(sb, sb->fnames_full[i]);
                              sb->rs = stopped;
                              return;
                            }
                        }
                  }
              }
          }

        tot_rows = max(1, sb->fn + sb->gn);
        page_rows = min(tot_rows, sb->cfg.display_rows);
        gtk_adjustment_configure(sb->area_adj, 0, 0, tot_rows, 1, page_rows, page_rows);

        sb->time_base = get_time();
        sb->mt = (sb->cfg.num_threads > 1);

        strcpy(cmd, CMD_PATH);
        strcat(cmd, sb->mt? "tinx_mt" : "tinx");
        strcat(cmd, " 2>&1");

        sprintf(arg, " -n %d", sb->cfg.num_threads);
        strcat(cmd, arg);

        sprintf(arg, " -r %d", sb->cfg.bsbt);
        strcat(cmd, arg);

        sprintf(arg, " -t "REAL_FMT, sb->cfg.step);
        strcat(cmd, arg);

        sprintf(arg, " -z "TIME_FMT, sb->cfg.max_time);
        strcat(cmd, arg);

        if(*sb->cfg.prefix)
          {
            strcat(cmd, " -e '");
            strcat(cmd, sb->cfg.prefix);
            strcat(cmd, "'");
          }

        if(*sb->cfg.path)
          {
            strcat(cmd, " -p '");
            strcat(cmd, sb->cfg.path);
            strcat(cmd, "'");
          }

        strcat(cmd, " -a '");
        strcat(cmd, sb->cfg.alpha);
        strcat(cmd, "'");

        if(sb->cfg.load_state)
          {
            strcat(cmd, " -I '");
            strcat(cmd, sb->cfg.state_name);
            strcat(cmd, "'");
          }

        if(sb->cfg.echo_logfile)
          {
            strcat(cmd, " -L '");
            strcat(cmd, sb->cfg.logfile_name);
            strcat(cmd, "'");
          }

        if(sb->cfg.use_xref)
          {
            strcat(cmd, " -X '");
            strcat(cmd, sb->cfg.xref_name);
            strcat(cmd, "'");
          }

        if(sb->cfg.strictly_causal)
          strcat(cmd, " -c");

        if(sb->cfg.soundness_check)
          strcat(cmd, " -v");

        if(sb->cfg.echo_stdout)
          strcat(cmd, " -d");

        if(sb->cfg.file_io)
          strcat(cmd, " -f");

        if(sb->cfg.quiet)
          strcat(cmd, " -q");

        if(sb->cfg.hard)
          strcat(cmd, " -s");

        if(sb->cfg.sys5)
          strcat(cmd, " -V");

        if(sb->cfg.sturdy)
          strcat(cmd, " -y");

        if(sb->cfg.busywait)
          strcat(cmd, " -S");

        strcat(cmd, " '");
        strcat(cmd, sb->cfg.base_name);
        strcat(cmd, "'");

        sprintf(arg, " -g "ORIGIN_FMT, sb->time_base);
        strcat(cmd, arg);

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
            if(!sb->cp_batch_in)
              for(i = 0; i < sb->fn; i++)
                for(k = 0; k < sb->cp_horizon_size; k++)
                  sb->memory_f[i][k] = DISPLAY_EMPTY_CHAR;

            if(!sb->cp_batch_out)
              {
                for(i = 0; i < sb->gn; i++)
                  for(k = 0; k < sb->cp_horizon_size; k++)
                    sb->memory_g[i][k] = DISPLAY_EMPTY_CHAR;

                for(i = 0; i < sb->gn; i++)
                  {
                    if(sb->gfile_io[i])
                      do
                        sb->gp[i] = open_input_file(sb->gnames_full[i]);
                      while(!sb->term && !is_file_open(sb->gp[i]));
                    else
                      if(!sb->cp_sys5)
                        {
                          sb->dp[i] = add_queue_posix(sb->gnames_full[i], input_stream);
                          if(failed_queue_posix(sb->dp[i]))
                            {
                              print_error(sb, sb->gnames_full[i]);
                              sb->rs = stopped;
                              return;
                            }
                        }
                      else
                        {
                          sb->dp5[i] = add_queue_sys5(sb->gnames_full[i], output_stream);
                          if(failed_queue_sys5(sb->dp5[i]))
                            {
                              print_error(sb, sb->gnames_full[i]);
                              sb->rs = stopped;
                              return;
                            }
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

        g_idle_add((gboolean (*)(gpointer))flip_controls, sb);

        sb->rs = started;
      break;

      case started:
        sb->rs = stopping;

        if(!sb->cp_quiet)
          {
            pthread_join(sb->tintloop, NULL);

            update_drawing(sb);

            if(!sb->cp_batch_in)
              {
                for(i = 0; i < sb->fn; i++)
                  {
                    oc = sb->cfg.alpha[end_symbol];

                    if(sb->ffile_io[i])
                      {
                        if(sb->fp[i])
                          {
                            if(!sb->fpacked[i] && put_file(sb->fp[i], &oc))
                              {
                                print_error(sb, sb->fnames_full[i]);
                                break;
                              }

                            if(close_file(sb->fp[i]))
                              {
                                print_error(sb, sb->fnames_full[i]);
                                break;
                              }
                          }
                      }
                    else
                      if(!sb->cp_sys5)
                        {
                          if(!failed_queue_posix(sb->cp[i]))
                            {
                              if(!sb->fpacked[i])
                                send_message_posix(sb->cp[i], &oc);

                              if(commit_queue_posix(sb->cp[i]))
                                {
                                  print_error(sb, sb->fnames_full[i]);
                                  break;
                                }
                            }
                        }
                      else
                        if(!failed_queue_sys5(sb->cp5[i]) && !sb->fpacked[i])
                          send_message_sys5(sb->cp5[i], &oc);
                  }
              }

            if(!sb->cp_batch_out)
              {
                count = 0;
                halt = get_time();
                do
                  {
                    for(i = 0; i < sb->gn; i++)
                      {
                        if(sb->gfile_io[i])
                          {
                            if(sb->gp[i])
                              {
                                if(get_file(sb->gp[i], &ic) && file_error(sb->gp[i]))
                                  {
                                    print_error(sb, sb->gnames_full[i]);
                                    break;
                                  }

                                if(ic == EOF)
                                  reset_file(sb->gp[i]);
                              }
                          }
                        else
                          if(!sb->cp_sys5)
                            read_message_posix(sb->dp[i], &ic);
                          else
                            read_message_sys5(sb->dp5[i], &ic);

                        if(!sb->gpacked[i] && ic == sb->cfg.alpha[end_symbol])
                          count += TAIL_LEN;
                        else
                          if(ic != EOF)
                            count++;
                      }
                  }
                while(!sb->term && count < sb->gn * TAIL_LEN && get_time() - halt < MAX_SEC_HALT);

                for(i = 0; i < sb->gn; i++)
                  if(sb->gfile_io[i])
                    {
                      if(sb->gp[i] && close_file(sb->gp[i]))
                        {
                          print_error(sb, sb->gnames_full[i]);
                          break;
                        }
                    }
                  else
                    if(!sb->cp_sys5 && !failed_queue_posix(sb->dp[i]) && commit_queue_posix(sb->dp[i]))
                      {
                        print_error(sb, sb->gnames_full[i]);
                        break;
                      }
              }
          }

        pid = 0;

        for(k = 0; k < MAX_WAIT; k++)
          {
            pid = pidof(sb, sb->mt? "tinx_mt" : "tinx");

            if(!pid)
              break;

            usleep(DELAY);
          }

        if(pid > 0)
          {
            kill(pid, SIGINT);
            waitpid(pid);
          }

        pthread_join(sb->tinxpipe, NULL);

        g_idle_add((gboolean (*)(gpointer))flip_controls, sb);
        g_idle_add((gboolean (*)(gpointer))reset_view, sb);

        sb->rs = stopped;
      break;

      default:
      break;
    }
}

void run_button_clicked(GtkWidget *widget, s_base *sb)
{
  bool emit;

  pthread_mutex_lock(&sb->mutex_button);

  if(sb->rs == stopped)
    {
      sb->sent = FALSE;

      dummy_button_clicked(widget, sb);
    }
  else
    {
      pthread_mutex_lock(&sb->mutex_sent);

      if(sb->rs == started && !sb->sent)
        {
          sb->sent = TRUE;
          emit = TRUE;
        }
      else
        emit = FALSE;

      pthread_mutex_unlock(&sb->mutex_sent);

      if(emit)
        dummy_button_clicked(widget, sb);
    }

  pthread_mutex_unlock(&sb->mutex_button);
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

  pthread_mutex_lock(&sb->mutex_button);

  strcpy(cmd, CMD_PATH"ting 2>&1 -o '");
  strcat(cmd, sb->cfg.base_name);
  strcat(cmd, "'");

  strcat(cmd, " -I '");
  strcat(cmd, sb->cfg.state_name);
  strcat(cmd, "'");

  if(sb->cfg.use_xref)
    {
      strcat(cmd, " -X '");
      strcat(cmd, sb->cfg.xref_name);
      strcat(cmd, "'");
    }

  if(*sb->cfg.include_path)
    {
      strcat(cmd, " -P '");
      strcat(cmd, sb->cfg.include_path);
      strcat(cmd, "'");
    }

  if(sb->cfg.seplit_fe)
    strcat(cmd, " -w");

  if(sb->cfg.seplit_su)
    strcat(cmd, " -W");

  if(sb->cfg.merge)
    strcat(cmd, " -u");

  if(sb->cfg.constout)
    strcat(cmd, " -k");

  if(sb->cfg.outaux)
    strcat(cmd, " -b");

  if(sb->cfg.outint)
    strcat(cmd, " -B");

  strcat(cmd, " '");
  strcat(cmd, sb->cfg.source_name);
  strcat(cmd, "'");

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

  if(sb->regenerate)
    {
      sb->regenerate = FALSE;
      gtk_image_clear(sb->reg_warning_icon);
      gtk_label_set_markup(sb->reg_warning, NULL);
    }

  pthread_mutex_unlock(&sb->mutex_button);
}

void about_button_clicked(GtkWidget *widget, s_base *sb)
{
  pthread_mutex_lock(&sb->mutex_button);

  print(sb, BANNER);

  pthread_mutex_unlock(&sb->mutex_button);
}

gboolean exit_button_clicked_if(GtkWidget *widget, GdkEvent *event, s_base *sb)
{
  exit_button_clicked(widget, sb);

  return TRUE;
}

void exit_button_clicked(GtkWidget *widget, s_base *sb)
{
  bool emit;

  pthread_mutex_lock(&sb->mutex_button);

  while(sb->rs == starting)
    usleep(DELAY);

  pthread_mutex_lock(&sb->mutex_sent);

  if(sb->rs == started && !sb->sent)
    {
      sb->sent = TRUE;
      emit = TRUE;
    }
  else
    emit = FALSE;

  pthread_mutex_unlock(&sb->mutex_sent);

  if(emit)
    dummy_button_clicked(GTK_WIDGET(sb->run_button), sb);

  while(sb->rs != stopped)
    usleep(DELAY);

  gtk_main_quit();

  pthread_mutex_unlock(&sb->mutex_button);
}

void edit_button_clicked(GtkWidget *widget, s_base *sb)
{
  char cmd[MAX_STRLEN_IF];

  pthread_mutex_lock(&sb->mutex_button);

  if(*sb->cfg.editor_name)
    {
      strcpy(cmd, CMD_PATH);
      strcat(cmd, sb->cfg.editor_name);
      strcat(cmd, " '");
      strcat(cmd, sb->cfg.source_name);
      strcat(cmd, SOURCE_EXT"' &");

      system(cmd);
    }

  pthread_mutex_unlock(&sb->mutex_button);
}

void help_button_clicked(GtkWidget *widget, s_base *sb)
{
  char cmd[MAX_STRLEN_IF];

  pthread_mutex_lock(&sb->mutex_button);

  if(*sb->cfg.viewer_name)
    {
      strcpy(cmd, CMD_PATH);
      strcat(cmd, sb->cfg.viewer_name);
      strcat(cmd, " "HELP_FILENAME" &");

      system(cmd);
    }

  pthread_mutex_unlock(&sb->mutex_button);
}

void save_button_clicked(GtkWidget *widget, s_base *sb)
{
  FILE *fp;

  pthread_mutex_lock(&sb->mutex_button);

  fp = fopen(CONFIG_FILENAME, "w");
  if(fp)
    {
      fwrite(&sb->cfg, sizeof(sb->cfg), 1, fp);
      fclose(fp);

      sb->changed = FALSE;
      sb->configured = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), FALSE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), FALSE);

      gtk_widget_set_sensitive(GTK_WIDGET(sb->erase_menu), TRUE);
      if(sb->erase_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->erase_button), TRUE);
    }

  pthread_mutex_unlock(&sb->mutex_button);
}

void erase_button_clicked(GtkWidget *widget, s_base *sb)
{
  pthread_mutex_lock(&sb->mutex_button);

  remove(CONFIG_FILENAME);

  sb->changed = TRUE;
  sb->configured = FALSE;

  gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
  if(sb->save_button)
    gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);

  gtk_widget_set_sensitive(GTK_WIDGET(sb->erase_menu), FALSE);
  if(sb->erase_button)
    gtk_widget_set_sensitive(GTK_WIDGET(sb->erase_button), FALSE);

  pthread_mutex_unlock(&sb->mutex_button);
}

void load_state_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->cfg.load_state = TRUE;
  else
    sb->cfg.load_state = FALSE;

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void soundness_check_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->cfg.soundness_check = TRUE;
  else
    sb->cfg.soundness_check = FALSE;

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void strictly_causal_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->cfg.strictly_causal = TRUE;
  else
    sb->cfg.strictly_causal = FALSE;

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void echo_stdout_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->cfg.echo_stdout = TRUE;
  else
    sb->cfg.echo_stdout = FALSE;

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void echo_logfile_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->cfg.echo_logfile = TRUE;
  else
    sb->cfg.echo_logfile = FALSE;

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void file_io_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->cfg.file_io = TRUE;
  else
    sb->cfg.file_io = FALSE;

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void quiet_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->cfg.quiet = TRUE;
  else
    sb->cfg.quiet = FALSE;

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void hard_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->cfg.hard = TRUE;
  else
    sb->cfg.hard = FALSE;

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void sys5_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->cfg.sys5 = TRUE;
  else
    sb->cfg.sys5 = FALSE;

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void sturdy_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->cfg.sturdy = TRUE;
  else
    sb->cfg.sturdy = FALSE;

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void busywait_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->cfg.busywait = TRUE;
  else
    sb->cfg.busywait = FALSE;

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void use_xref_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    {
      sb->cfg.use_xref = TRUE;

      if(!sb->regenerate)
        {
          sb->regenerate = TRUE;
          gtk_image_set_from_stock(sb->reg_warning_icon, GTK_STOCK_DIALOG_WARNING, GTK_ICON_SIZE_BUTTON);
          gtk_label_set_markup(sb->reg_warning, "  <i>Please regenerate network</i>");
        }
    }
  else
    sb->cfg.use_xref = FALSE;

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void seplit_fe_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->cfg.seplit_fe = TRUE;
  else
    sb->cfg.seplit_fe = FALSE;

  if(!sb->regenerate)
    {
      sb->regenerate = TRUE;
      gtk_image_set_from_stock(sb->reg_warning_icon, GTK_STOCK_DIALOG_WARNING, GTK_ICON_SIZE_BUTTON);
      gtk_label_set_markup(sb->reg_warning, "  <i>Please regenerate network</i>");
    }

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void seplit_su_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->cfg.seplit_su = TRUE;
  else
    sb->cfg.seplit_su = FALSE;

  if(!sb->regenerate)
    {
      sb->regenerate = TRUE;
      gtk_image_set_from_stock(sb->reg_warning_icon, GTK_STOCK_DIALOG_WARNING, GTK_ICON_SIZE_BUTTON);
      gtk_label_set_markup(sb->reg_warning, "  <i>Please regenerate network</i>");
    }

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void merge_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->cfg.merge = TRUE;
  else
    sb->cfg.merge = FALSE;

  if(!sb->regenerate)
    {
      sb->regenerate = TRUE;
      gtk_image_set_from_stock(sb->reg_warning_icon, GTK_STOCK_DIALOG_WARNING, GTK_ICON_SIZE_BUTTON);
      gtk_label_set_markup(sb->reg_warning, "  <i>Please regenerate network</i>");
    }

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void constout_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->cfg.constout = TRUE;
  else
    sb->cfg.constout = FALSE;

  if(!sb->regenerate)
    {
      sb->regenerate = TRUE;
      gtk_image_set_from_stock(sb->reg_warning_icon, GTK_STOCK_DIALOG_WARNING, GTK_ICON_SIZE_BUTTON);
      gtk_label_set_markup(sb->reg_warning, "  <i>Please regenerate network</i>");
    }

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void outaux_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->cfg.outaux = TRUE;
  else
    sb->cfg.outaux = FALSE;

  if(!sb->regenerate)
    {
      sb->regenerate = TRUE;
      gtk_image_set_from_stock(sb->reg_warning_icon, GTK_STOCK_DIALOG_WARNING, GTK_ICON_SIZE_BUTTON);
      gtk_label_set_markup(sb->reg_warning, "  <i>Please regenerate network</i>");
    }

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void outint_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->cfg.outint = TRUE;
  else
    sb->cfg.outint = FALSE;

  if(!sb->regenerate)
    {
      sb->regenerate = TRUE;
      gtk_image_set_from_stock(sb->reg_warning_icon, GTK_STOCK_DIALOG_WARNING, GTK_ICON_SIZE_BUTTON);
      gtk_label_set_markup(sb->reg_warning, "  <i>Please regenerate network</i>");
    }

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void batch_in_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->cfg.batch_in = TRUE;
  else
    sb->cfg.batch_in = FALSE;

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void batch_out_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->cfg.batch_out = TRUE;
  else
    sb->cfg.batch_out = FALSE;

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void draw_undef_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->cfg.draw_undef = TRUE;
  else
    sb->cfg.draw_undef = FALSE;

  gtk_widget_queue_draw(GTK_WIDGET(sb->drawingarea));

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void full_names_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->cfg.full_names = TRUE;
  else
    sb->cfg.full_names = FALSE;

  gtk_widget_queue_draw(GTK_WIDGET(sb->drawingarea));

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void source_fname(GtkWidget *widget, s_base *sb)
{
  strcpy(sb->cfg.source_name, gtk_entry_get_text(GTK_ENTRY(widget)));

  strcpy(sb->cfg.base_name, sb->cfg.source_name);
  strcpy(sb->cfg.state_name, sb->cfg.base_name);
  strcat(sb->cfg.state_name, STATE_SUFFIX);
  strcpy(sb->cfg.logfile_name, sb->cfg.base_name);
  strcat(sb->cfg.logfile_name, LOG_SUFFIX);
  strcpy(sb->cfg.xref_name, sb->cfg.base_name);

  if(sb->config_window)
    gtk_widget_queue_draw(GTK_WIDGET(sb->config_window));

  if(!sb->regenerate)
    {
      sb->regenerate = TRUE;
      gtk_image_set_from_stock(sb->reg_warning_icon, GTK_STOCK_DIALOG_WARNING, GTK_ICON_SIZE_BUTTON);
      gtk_label_set_markup(sb->reg_warning, "  <i>Please regenerate network</i>");
    }

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void base_fname(GtkWidget *widget, s_base *sb)
{
  strcpy(sb->cfg.base_name, gtk_entry_get_text(GTK_ENTRY(widget)));

  strcpy(sb->cfg.state_name, sb->cfg.base_name);
  strcat(sb->cfg.state_name, STATE_SUFFIX);
  strcpy(sb->cfg.logfile_name, sb->cfg.base_name);
  strcat(sb->cfg.logfile_name, LOG_SUFFIX);
  strcpy(sb->cfg.xref_name, sb->cfg.base_name);

  if(sb->config_window)
    gtk_widget_queue_draw(GTK_WIDGET(sb->config_window));

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void state_fname(GtkWidget *widget, s_base *sb)
{
  strcpy(sb->cfg.state_name, gtk_entry_get_text(GTK_ENTRY(widget)));

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void logfile_fname(GtkWidget *widget, s_base *sb)
{
  strcpy(sb->cfg.logfile_name, gtk_entry_get_text(GTK_ENTRY(widget)));

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void xref_fname(GtkWidget *widget, s_base *sb)
{
  strcpy(sb->cfg.xref_name, gtk_entry_get_text(GTK_ENTRY(widget)));

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void prefix_fname(GtkWidget *widget, s_base *sb)
{
  strcpy(sb->cfg.prefix, gtk_entry_get_text(GTK_ENTRY(widget)));

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void path_fname(GtkWidget *widget, s_base *sb)
{
  strcpy(sb->cfg.path, gtk_entry_get_text(GTK_ENTRY(widget)));

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void include_path_fname(GtkWidget *widget, s_base *sb)
{
  strcpy(sb->cfg.include_path, gtk_entry_get_text(GTK_ENTRY(widget)));

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void editor_fname(GtkWidget *widget, s_base *sb)
{
  strcpy(sb->cfg.editor_name, gtk_entry_get_text(GTK_ENTRY(widget)));

  if(*sb->cfg.editor_name)
    gtk_widget_set_sensitive(GTK_WIDGET(sb->edit_button), TRUE);
  else
    gtk_widget_set_sensitive(GTK_WIDGET(sb->edit_button), FALSE);

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void viewer_fname(GtkWidget *widget, s_base *sb)
{
  strcpy(sb->cfg.viewer_name, gtk_entry_get_text(GTK_ENTRY(widget)));

  if(*sb->cfg.viewer_name)
    {
      gtk_widget_set_sensitive(GTK_WIDGET(sb->help_button), TRUE);
      gtk_widget_set_sensitive(GTK_WIDGET(sb->help_menu), TRUE);
    }
  else
    {
      gtk_widget_set_sensitive(GTK_WIDGET(sb->help_button), FALSE);
      gtk_widget_set_sensitive(GTK_WIDGET(sb->help_menu), FALSE);
    }

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void false_sym(GtkWidget *widget, s_base *sb)
{
  strncpy(&sb->cfg.alpha[false_symbol], gtk_entry_get_text(GTK_ENTRY(widget)), 1);

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void true_sym(GtkWidget *widget, s_base *sb)
{
  strncpy(&sb->cfg.alpha[true_symbol], gtk_entry_get_text(GTK_ENTRY(widget)), 1);

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void unknown_sym(GtkWidget *widget, s_base *sb)
{
  strncpy(&sb->cfg.alpha[unknown_symbol], gtk_entry_get_text(GTK_ENTRY(widget)), 1);

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void end_sym(GtkWidget *widget, s_base *sb)
{
  strncpy(&sb->cfg.alpha[end_symbol], gtk_entry_get_text(GTK_ENTRY(widget)), 1);

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

gboolean source_default(GtkWidget *widget, cairo_t *cr, s_base *sb)
{
  gtk_entry_set_text(GTK_ENTRY(widget), sb->cfg.source_name);

  return FALSE;
}

gboolean base_default(GtkWidget *widget, cairo_t *cr, s_base *sb)
{
  gtk_entry_set_text(GTK_ENTRY(widget), sb->cfg.base_name);

  return FALSE;
}

gboolean state_default(GtkWidget *widget, cairo_t *cr, s_base *sb)
{
  gtk_entry_set_text(GTK_ENTRY(widget), sb->cfg.state_name);

  return FALSE;
}

gboolean logfile_default(GtkWidget *widget, cairo_t *cr, s_base *sb)
{
  gtk_entry_set_text(GTK_ENTRY(widget), sb->cfg.logfile_name);

  return FALSE;
}

gboolean xref_default(GtkWidget *widget, cairo_t *cr, s_base *sb)
{
  gtk_entry_set_text(GTK_ENTRY(widget), sb->cfg.xref_name);

  return FALSE;
}

gboolean prefix_default(GtkWidget *widget, cairo_t *cr, s_base *sb)
{
  gtk_entry_set_text(GTK_ENTRY(widget), sb->cfg.prefix);

  return FALSE;
}

gboolean path_default(GtkWidget *widget, cairo_t *cr, s_base *sb)
{
  gtk_entry_set_text(GTK_ENTRY(widget), sb->cfg.path);

  return FALSE;
}

gboolean include_path_default(GtkWidget *widget, cairo_t *cr, s_base *sb)
{
  gtk_entry_set_text(GTK_ENTRY(widget), sb->cfg.include_path);

  return FALSE;
}

gboolean editor_default(GtkWidget *widget, cairo_t *cr, s_base *sb)
{
  gtk_entry_set_text(GTK_ENTRY(widget), sb->cfg.editor_name);

  return FALSE;
}

gboolean viewer_default(GtkWidget *widget, cairo_t *cr, s_base *sb)
{
  gtk_entry_set_text(GTK_ENTRY(widget), sb->cfg.viewer_name);

  return FALSE;
}

void source_dialog_if(GtkEntry *widget, s_base *sb)
{
  source_dialog(widget, 0, NULL, sb);
}

void source_dialog(GtkEntry *widget, GtkEntryIconPosition pos, GdkEvent *event, s_base *sb)
{
  GtkDialog *dialog;
  char *ext;

  dialog = GTK_DIALOG(gtk_file_chooser_dialog_new("Select temporal logic source file", sb->window, GTK_FILE_CHOOSER_ACTION_OPEN, "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL));

  if(gtk_dialog_run(dialog) == GTK_RESPONSE_ACCEPT)
    {
      strcpy(sb->cfg.source_name, gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog)));
      if(!sb->changed)
        {
          sb->changed = TRUE;

          gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
          if(sb->save_button)
            gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
        }
    }

  gtk_widget_destroy(GTK_WIDGET(dialog));

  ext = strstr(sb->cfg.source_name, SOURCE_EXT);
  if(ext && !strcmp(ext, SOURCE_EXT))
    *ext = '\0';
}

void base_dialog(GtkEntry *widget, GtkEntryIconPosition pos, GdkEvent *event, s_base *sb)
{
  GtkDialog *dialog;
  char *ext;

  dialog = GTK_DIALOG(gtk_file_chooser_dialog_new("Select network object file", sb->config_window, GTK_FILE_CHOOSER_ACTION_SAVE, "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL));

  if(gtk_dialog_run(dialog) == GTK_RESPONSE_ACCEPT)
    {
      strcpy(sb->cfg.base_name, gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog)));
      if(!sb->changed)
        {
          sb->changed = TRUE;

          gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
          if(sb->save_button)
            gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
        }
    }

  gtk_widget_destroy(GTK_WIDGET(dialog));

  ext = strstr(sb->cfg.base_name, NETWORK_EXT);
  if(ext && !strcmp(ext, NETWORK_EXT))
    *ext = '\0';
}

void state_dialog(GtkEntry *widget, GtkEntryIconPosition pos, GdkEvent *event, s_base *sb)
{
  GtkDialog *dialog;
  char *ext;

  dialog = GTK_DIALOG(gtk_file_chooser_dialog_new("Select initial conditions file", sb->config_window, GTK_FILE_CHOOSER_ACTION_SAVE, "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL));

  if(gtk_dialog_run(dialog) == GTK_RESPONSE_ACCEPT)
    {
      strcpy(sb->cfg.state_name, gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog)));
      if(!sb->changed)
        {
          sb->changed = TRUE;

          gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
          if(sb->save_button)
            gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
        }
    }

  gtk_widget_destroy(GTK_WIDGET(dialog));

  ext = strstr(sb->cfg.state_name, EVENT_LIST_EXT);
  if(ext && !strcmp(ext, EVENT_LIST_EXT))
    *ext = '\0';
}

void logfile_dialog(GtkEntry *widget, GtkEntryIconPosition pos, GdkEvent *event, s_base *sb)
{
  GtkDialog *dialog;
  char *ext;

  dialog = GTK_DIALOG(gtk_file_chooser_dialog_new("Select log file", sb->config_window, GTK_FILE_CHOOSER_ACTION_SAVE, "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL));

  if(gtk_dialog_run(dialog) == GTK_RESPONSE_ACCEPT)
    {
      strcpy(sb->cfg.logfile_name, gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog)));
      if(!sb->changed)
        {
          sb->changed = TRUE;

          gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
          if(sb->save_button)
            gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
        }
    }

  gtk_widget_destroy(GTK_WIDGET(dialog));

  ext = strstr(sb->cfg.logfile_name, EVENT_LIST_EXT);
  if(ext && !strcmp(ext, EVENT_LIST_EXT))
    *ext = '\0';
}

void xref_dialog(GtkEntry *widget, GtkEntryIconPosition pos, GdkEvent *event, s_base *sb)
{
  GtkDialog *dialog;
  char *ext;

  dialog = GTK_DIALOG(gtk_file_chooser_dialog_new("Select symbol table file", sb->config_window, GTK_FILE_CHOOSER_ACTION_SAVE, "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL));

  if(gtk_dialog_run(dialog) == GTK_RESPONSE_ACCEPT)
    {
      strcpy(sb->cfg.xref_name, gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog)));
      if(!sb->changed)
        {
          sb->changed = TRUE;

          gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
          if(sb->save_button)
            gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
        }
    }

  gtk_widget_destroy(GTK_WIDGET(dialog));

  ext = strstr(sb->cfg.xref_name, XREF_EXT);
  if(ext && !strcmp(ext, XREF_EXT))
    *ext = '\0';
}

void path_dialog(GtkEntry *widget, GtkEntryIconPosition pos, GdkEvent *event, s_base *sb)
{
  GtkDialog *dialog;

  dialog = GTK_DIALOG(gtk_file_chooser_dialog_new("Select input and output file path", sb->config_window, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL));

  if(gtk_dialog_run(dialog) == GTK_RESPONSE_ACCEPT)
    {
      strcpy(sb->cfg.path, gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog)));
      if(!sb->changed)
        {
          sb->changed = TRUE;

          gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
          if(sb->save_button)
            gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
        }
    }

  gtk_widget_destroy(GTK_WIDGET(dialog));
}

void include_path_dialog(GtkEntry *widget, GtkEntryIconPosition pos, GdkEvent *event, s_base *sb)
{
  GtkDialog *dialog;

  dialog = GTK_DIALOG(gtk_file_chooser_dialog_new("Select module file path", sb->config_window, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL));

  if(gtk_dialog_run(dialog) == GTK_RESPONSE_ACCEPT)
    {
      strcpy(sb->cfg.include_path, gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog)));
      if(!sb->changed)
        {
          sb->changed = TRUE;

          gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
          if(sb->save_button)
            gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
        }
    }

  gtk_widget_destroy(GTK_WIDGET(dialog));
}

void step_value(GtkWidget *widget, s_base *sb)
{
  sb->cfg.step = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void max_time_value(GtkWidget *widget, s_base *sb)
{
  sb->cfg.max_time = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void num_threads_value(GtkWidget *widget, s_base *sb)
{
  sb->cfg.num_threads = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void bsbt_value(GtkWidget *widget, s_base *sb)
{
  sb->cfg.bsbt = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void pos_value(GtkWidget *widget, s_base *sb)
{
  sb->pos = gtk_adjustment_get_value(sb->area_adj);

  if(sb->rs == stopped)
    update_drawing(sb);
}

void prob_value(GtkWidget *widget, s_base *sb)
{
  sb->cfg.prob = gtk_range_get_value(GTK_RANGE(widget));

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void correction_value(GtkWidget *widget, s_base *sb)
{
  sb->cfg.correction = gtk_range_get_value(GTK_RANGE(widget));

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void horizon_value(GtkWidget *widget, s_base *sb)
{
  sb->cfg.horizon_size = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget)) + 1;

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void rows_value(GtkWidget *widget, s_base *sb)
{
  sb->cfg.display_rows = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

gboolean goto_end(s_base *sb)
{
  GtkTextMark *mark;

  mark = gtk_text_buffer_get_mark(gtk_text_view_get_buffer(sb->textarea), "end");

  gtk_text_view_scroll_mark_onscreen(sb->textarea, mark);

  return G_SOURCE_REMOVE;
}

gboolean update_view(s_base *sb)
{
  char buffer[XBUFSIZE];
  int i, n;
  bool xcat;
  GtkTextBuffer *textbuffer;
  GtkTextMark *mark;
  GtkTextIter iter;

  pthread_mutex_lock(&sb->mutex_xbuffer);

  n = (sb->xend - sb->xstart + XBUFSIZE) % XBUFSIZE;

  for(i = 0; i < n; i++)
    buffer[i] = sb->xbuffer[(sb->xstart + i) % XBUFSIZE];

  sb->xstart = sb->xend;

  buffer[n] = '\0';

  xcat = sb->xcat;
  if(!xcat)
    sb->xcat = TRUE;

  pthread_mutex_unlock(&sb->mutex_xbuffer);

  textbuffer = gtk_text_view_get_buffer(sb->textarea);
  mark = gtk_text_buffer_get_mark(textbuffer, "end");

  if(!xcat)
    gtk_text_buffer_set_text(textbuffer, buffer, -1);
  else
    {
      gtk_text_buffer_get_iter_at_mark(textbuffer, &iter, mark);
      gtk_text_buffer_insert(textbuffer, &iter, buffer, -1);
    }

  g_idle_add((gboolean (*)(gpointer))goto_end, sb);

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

  pthread_mutex_lock(&sb->mutex_xbuffer);

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

  pthread_mutex_unlock(&sb->mutex_xbuffer);

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

  pthread_mutex_lock(&sb->mutex_xbuffer);

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

  pthread_mutex_unlock(&sb->mutex_xbuffer);

  if(!sb->cp_echo_stdout && flush)
    g_idle_add((gboolean (*)(gpointer))update_view, sb);
}

void inprob_value(GtkWidget *widget, s_base *sb)
{
  int i;

  for(i = 0; i < sb->cfg.fn; i++)
    if(widget == sb->inprob_widget[i])
      {
        sb->cfg.inprob[i] = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));

        if(!sb->changeprob)
          {
            sb->changeprob = TRUE;
            gtk_widget_set_sensitive(GTK_WIDGET(sb->clear_button), TRUE);
          }

        if(!sb->changed)
          {
            sb->changed = TRUE;

            gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
            if(sb->save_button)
              gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
          }

        break;
      }
}

void inprob_clear(GtkWidget *widget, s_base *sb)
{
  int i;

  pthread_mutex_lock(&sb->mutex_button);

  if(sb->changeprob)
    {
      for(i = 0; i < MAX_FILES; i++)
        sb->cfg.inprob[i] = 1;

      for(i = 0; i < sb->cfg.fn; i++)
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(sb->inprob_widget[i]), 1);

      sb->changeprob = FALSE;
      gtk_widget_set_sensitive(GTK_WIDGET(sb->clear_button), FALSE);

      if(!sb->changed)
        {
          sb->changed = TRUE;

          gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
          if(sb->save_button)
            gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
        }
    }

  pthread_mutex_unlock(&sb->mutex_button);
}

void end_prob(GtkWidget *widget, s_base *sb)
{
  sb->prob_window = NULL;
  sb->clear_button = NULL;
}

void prob_button_clicked(GtkWidget *widget, s_base *sb)
{
  FILE *bp;
  char file_name[MAX_STRLEN], name[MAX_STRLEN];
  char fnames[MAX_FILES][MAX_STRLEN];
  int fn, i, n, packed, packedbit;
  char c;
  GtkWidget *window;
  GtkWidget *tabdyn;
  GtkWidget *lbl, *ent;
  GtkWidget *btn;
  GtkWidget *extbox, *bbox;

  pthread_mutex_lock(&sb->mutex_button);

  if(sb->prob_window)
    {
      gtk_window_present(sb->prob_window);
      pthread_mutex_unlock(&sb->mutex_button);
      return;
    }

  strcpy(file_name, sb->cfg.base_name);
  strcat(file_name, NETWORK_EXT);

  bp = fopen(file_name, "r");
  if(!bp)
    {
      print_error(sb, file_name);
      pthread_mutex_unlock(&sb->mutex_button);
      return;
    }

  fn = 0;

  packed = 0;
  packedbit = 0;

  fscanf(bp, " "SKIP_FMT" ");

  if(ferror(bp))
    {
      print_error(sb, file_name);
      pthread_mutex_unlock(&sb->mutex_button);
      return;
    }

  while(fscanf(bp, " "OP_FMT" "FUN_FMT" ( %*[^"SEPARATORS"] , %*[^"SEPARATORS"] ) # %*d / %*u , %u , %u , %*u, %*u @ %*ld ", &c, name, &packed, &packedbit) >= 2)
    {
      if(!packed || !packedbit)
        {
          switch(c)
            {
              case '!':
                if(fn >= MAX_FILES)
                  {
                    print(sb, "%s: Too many input signals\n", file_name);
                    pthread_mutex_unlock(&sb->mutex_button);
                    return;
                  }

                strcpy(fnames[fn], name);

                fn++;
              break;

              case '?':
              case '.':
              break;

              default:
                print(sb, "%s, "OP_FMT": Invalid stream class\n", file_name, c);
                pthread_mutex_unlock(&sb->mutex_button);
              return;
            }
        }

      packed = 0;
      packedbit = 0;
    }

  if(ferror(bp))
    {
      print_error(sb, file_name);
      pthread_mutex_unlock(&sb->mutex_button);
      return;
    }

  if(!feof(bp))
    {
      print(sb, "%s: Parser error\n", file_name);
      pthread_mutex_unlock(&sb->mutex_button);
      return;
    }

  if(fclose(bp))
    {
      print_error(sb, file_name);
      pthread_mutex_unlock(&sb->mutex_button);
      return;
    }

  sb->cfg.fn = fn;

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), PROB_WINDOW_WIDTH, PROB_WINDOW_HEIGHT);
  gtk_container_set_border_width(GTK_CONTAINER(window), 10);
  gtk_window_set_title(GTK_WINDOW(window), PROB_TITLE);
  gtk_window_set_icon_name(GTK_WINDOW(window), "tinx");
  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(end_prob), (gpointer)sb);

  sb->prob_window = GTK_WINDOW(window);

  extbox = gtk_vbox_new(FALSE, 15);

  if(fn > 0)
    {
      n = ceil(sqrt(fn / 2.0));

      tabdyn = gtk_table_new(2 * n, n, FALSE);

      sb->changeprob = FALSE;
      for(i = 0; i < fn; i++)
        {
          strcpy(name, " ");
          strcat(name, fnames[i]);
          strcat(name, " ");

          lbl = gtk_label_new(name);
          gtk_misc_set_alignment(GTK_MISC(lbl), 1, 0.5);
          gtk_table_attach_defaults(GTK_TABLE(tabdyn), lbl, 2 * (i % n), 2 * (i % n) + 1, i / n, i / n + 1);
          ent = gtk_spin_button_new_with_range(0, 1.000, 0.001);
          gtk_spin_button_set_value(GTK_SPIN_BUTTON(ent), sb->cfg.inprob[i]);
          gtk_table_attach_defaults(GTK_TABLE(tabdyn), ent, 2 * (i % n) + 1, 2 * (i % n) + 2, i / n, i / n + 1);
          g_signal_connect(G_OBJECT(ent), "value-changed", G_CALLBACK(inprob_value), (gpointer)sb);

          sb->inprob_widget[i] = ent;

          if(sb->cfg.inprob[i] != 1)
            sb->changeprob = TRUE;
        }

      gtk_box_pack_start(GTK_BOX(extbox), tabdyn, TRUE, TRUE, 0);

      bbox = gtk_hbox_new(FALSE, 15);

      btn = gtk_button_new_with_label("Clear");
      gtk_widget_set_size_request(btn, 70, 30);
      gtk_box_pack_start(GTK_BOX(bbox), btn, FALSE, FALSE, 0);
      gtk_box_pack_end(GTK_BOX(extbox), bbox, FALSE, FALSE, 0);
      g_signal_connect(G_OBJECT(btn), "clicked", G_CALLBACK(inprob_clear), (gpointer)sb);

      sb->clear_button = GTK_BUTTON(btn);

      if(sb->changeprob)
        gtk_widget_set_sensitive(GTK_WIDGET(btn), TRUE);
      else
        gtk_widget_set_sensitive(GTK_WIDGET(btn), FALSE);
    }
  else
    {
      lbl = gtk_label_new("Current project has no inputs");
      gtk_box_pack_start(GTK_BOX(extbox), lbl, TRUE, TRUE, 0);
    }

  gtk_container_add(GTK_CONTAINER(window), extbox);

  gtk_widget_show_all(window);

  pthread_mutex_unlock(&sb->mutex_button);
}

void infilter_box(GtkWidget *widget, s_base *sb)
{
  int i;

  for(i = 0; i < sb->cfg.fn; i++)
    if(widget == sb->infilter_widget[i])
      {
        if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
          sb->cfg.fexcl[i] = TRUE;
        else
          sb->cfg.fexcl[i] = FALSE;

        if(!sb->changed)
          {
            sb->changed = TRUE;

            gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
            if(sb->save_button)
              gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
          }

        break;
      }
}

void outfilter_box(GtkWidget *widget, s_base *sb)
{
  int i;

  for(i = 0; i < sb->cfg.gn; i++)
    if(widget == sb->outfilter_widget[i])
      {
        if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
          sb->cfg.gexcl[i] = TRUE;
        else
          sb->cfg.gexcl[i] = FALSE;

        if(!sb->changed)
          {
            sb->changed = TRUE;

            gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
            if(sb->save_button)
              gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
          }

        break;
      }
}

void end_filter(GtkWidget *widget, s_base *sb)
{
  sb->filter_window = NULL;
}

void filter_button_clicked(GtkWidget *widget, s_base *sb)
{
  FILE *bp;
  char file_name[MAX_STRLEN], name[MAX_STRLEN];
  char fnames[MAX_FILES][MAX_STRLEN], gnames[MAX_FILES][MAX_STRLEN];
  int fn, gn, i, n, packed, packedbit;
  char c;
  GtkWidget *window;
  GtkWidget *fr1, *fr2;
  GtkWidget *tabdyn, *tabdyn2;
  GtkWidget *cb, *lbl, *lbl2;
  GtkWidget *extbox, *inbox, *outbox;

  pthread_mutex_lock(&sb->mutex_button);

  if(sb->filter_window)
    {
      gtk_window_present(sb->filter_window);
      pthread_mutex_unlock(&sb->mutex_button);
      return;
    }

  strcpy(file_name, sb->cfg.base_name);
  strcat(file_name, NETWORK_EXT);

  bp = fopen(file_name, "r");
  if(!bp)
    {
      print_error(sb, file_name);
      pthread_mutex_unlock(&sb->mutex_button);
      return;
    }

  fn = 0;
  gn = 0;

  packed = 0;
  packedbit = 0;

  fscanf(bp, " "SKIP_FMT" ");

  if(ferror(bp))
    {
      print_error(sb, file_name);
      pthread_mutex_unlock(&sb->mutex_button);
      return;
    }

  while(fscanf(bp, " "OP_FMT" "FUN_FMT" ( %*[^"SEPARATORS"] , %*[^"SEPARATORS"] ) # %*d / %*u , %u , %u , %*u, %*u @ %*ld ", &c, name, &packed, &packedbit) >= 2)
    {
      if(!packed || !packedbit)
        {
          switch(c)
            {
              case '!':
                if(fn >= MAX_FILES)
                  {
                    print(sb, "%s: Too many input signals\n", file_name);
                    pthread_mutex_unlock(&sb->mutex_button);
                    return;
                  }

                strcpy(fnames[fn], name);

                fn++;
              break;

              case '?':
              case '.':
                if(gn >= MAX_FILES)
                  {
                    print(sb, "%s: Too many output signals\n", file_name);
                    pthread_mutex_unlock(&sb->mutex_button);
                    return;
                  }

                strcpy(gnames[gn], name);

                gn++;
              break;

              default:
                print(sb, "%s, "OP_FMT": Invalid stream class\n", file_name, c);
                pthread_mutex_unlock(&sb->mutex_button);
              return;
            }
        }

      packed = 0;
      packedbit = 0;
    }

  if(ferror(bp))
    {
      print_error(sb, file_name);
      pthread_mutex_unlock(&sb->mutex_button);
      return;
    }

  if(!feof(bp))
    {
      print(sb, "%s: Parser error\n", file_name);
      pthread_mutex_unlock(&sb->mutex_button);
      return;
    }

  if(fclose(bp))
    {
      print_error(sb, file_name);
      pthread_mutex_unlock(&sb->mutex_button);
      return;
    }

  sb->cfg.fn = fn;
  sb->cfg.gn = gn;

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), FILTER_WINDOW_WIDTH, FILTER_WINDOW_HEIGHT);
  gtk_container_set_border_width(GTK_CONTAINER(window), 10);
  gtk_window_set_title(GTK_WINDOW(window), FILTER_TITLE);
  gtk_window_set_icon_name(GTK_WINDOW(window), "tinx");
  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(end_filter), (gpointer)sb);

  sb->filter_window = GTK_WINDOW(window);

  fr1 = gtk_frame_new("Input");
  gtk_label_set_markup(GTK_LABEL(gtk_frame_get_label_widget(GTK_FRAME(fr1))), "<b>Input</b>");

  inbox = gtk_vbox_new(FALSE, 15);
  gtk_container_add(GTK_CONTAINER(fr1), inbox);
  gtk_container_set_border_width(GTK_CONTAINER(fr1), 5);
  gtk_container_set_border_width(GTK_CONTAINER(inbox), 5);

  fr2 = gtk_frame_new("Output");
  gtk_label_set_markup(GTK_LABEL(gtk_frame_get_label_widget(GTK_FRAME(fr2))), "<b>Output</b>");

  outbox = gtk_vbox_new(FALSE, 15);
  gtk_container_add(GTK_CONTAINER(fr2), outbox);
  gtk_container_set_border_width(GTK_CONTAINER(fr2), 5);
  gtk_container_set_border_width(GTK_CONTAINER(outbox), 5);

  extbox = gtk_vbox_new(FALSE, 0);

  if(fn > 0)
    {
      n = ceil(sqrt(fn / 2.0));

      tabdyn = gtk_table_new(2 * n, n, FALSE);

      for(i = 0; i < fn; i++)
        {
          cb = gtk_check_button_new_with_label(fnames[i]);
          gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb), sb->cfg.fexcl[i]);
          gtk_table_attach_defaults(GTK_TABLE(tabdyn), cb, i % n, i % n + 1, i / n, i / n + 1);
          g_signal_connect(G_OBJECT(cb), "clicked", G_CALLBACK(infilter_box), (gpointer)sb);

          sb->infilter_widget[i] = cb;
        }

      gtk_box_pack_start(GTK_BOX(inbox), tabdyn, TRUE, TRUE, 0);
    }
  else
    {
      lbl = gtk_label_new("Current project has no inputs");
      gtk_box_pack_start(GTK_BOX(inbox), lbl, TRUE, TRUE, 0);
    }

  if(gn > 0)
    {
      n = ceil(sqrt(gn / 2.0));

      tabdyn2 = gtk_table_new(2 * n, n, FALSE);

      for(i = 0; i < gn; i++)
        {
          cb = gtk_check_button_new_with_label(gnames[i]);
          gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb), sb->cfg.gexcl[i]);
          gtk_table_attach_defaults(GTK_TABLE(tabdyn2), cb, i % n, i % n + 1, i / n, i / n + 1);
          g_signal_connect(G_OBJECT(cb), "clicked", G_CALLBACK(outfilter_box), (gpointer)sb);

          sb->outfilter_widget[i] = cb;
        }

      gtk_box_pack_start(GTK_BOX(outbox), tabdyn2, TRUE, TRUE, 0);
    }
  else
    {
      lbl2 = gtk_label_new("Current project has no outputs");
      gtk_box_pack_start(GTK_BOX(outbox), lbl2, TRUE, TRUE, 0);
    }

  gtk_box_pack_start(GTK_BOX(extbox), fr1, TRUE, TRUE, 0);
  gtk_box_pack_end(GTK_BOX(extbox), fr2, TRUE, TRUE, 0);

  gtk_container_add(GTK_CONTAINER(window), extbox);

  gtk_widget_show_all(window);

  pthread_mutex_unlock(&sb->mutex_button);
}

void end_configure(GtkWidget *widget, s_base *sb)
{
  sb->config_window = NULL;
  sb->save_button = NULL;
  sb->erase_button = NULL;
}

void configure(GtkWidget *widget, s_base *sb)
{
  GtkWidget *window;
  GtkWidget *btn5, *btn6, *btn7, *btn8;
  GtkWidget *tabf, *tabk, *tabh, *tabg, *tabs, *tabw;
  GtkWidget *cb8, *cb9, *cb10, *cb11, *cb12, *cb13, *cb14, *cb15, *cb16, *cb17, *cb18, *cb19, *cb20;
  GtkWidget *lbl2, *ent2, *lbl3, *ent3, *lbl4, *ent4, *lbl7, *ent7, *lbl10, *ent10, *lbl11, *ent11, *lbl12, *ent12, *lbl13, *ent13, *lbl14, *ent14, *lbl15, *ent15, *lbl16, *ent16, *lbl17, *ent17,
            *lbl18, *ent18, *lbl19, *ent19, *lbl20, *ent20, *lbl21, *ent21, *lbl22, *ent22;
  GtkWidget *fr1, *fr2, *fr3, *fr4;
  GtkWidget *vbox1, *vbox2, *vbox3, *vbox6, *vbox7, *vbox8, *hbox1, *hbox2, *hbox4, *extbox, *winbox;
  char buf[2];

  pthread_mutex_lock(&sb->mutex_button);

  if(sb->config_window)
    {
      gtk_window_present(sb->config_window);
      pthread_mutex_unlock(&sb->mutex_button);
      return;
    }

  buf[1] = '\0';

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), CONFIG_WINDOW_WIDTH, CONFIG_WINDOW_HEIGHT);
  gtk_container_set_border_width(GTK_CONTAINER(window), 0);
  gtk_window_set_title(GTK_WINDOW(window), CONFIG_TITLE);
  gtk_window_set_icon_name(GTK_WINDOW(window), "tinx");
  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(end_configure), (gpointer)sb);

  sb->config_window = GTK_WINDOW(window);

  winbox = gtk_vbox_new(FALSE, 0);
  extbox = gtk_vbox_new(FALSE, 0);

  gtk_container_set_border_width(GTK_CONTAINER(extbox), 10);

  fr1 = gtk_frame_new("Files");
  gtk_label_set_markup(GTK_LABEL(gtk_frame_get_label_widget(GTK_FRAME(fr1))), "<b>Files</b>");
  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(fr1), 5);
  gtk_container_set_border_width(GTK_CONTAINER(vbox1), 5);

  tabf = gtk_table_new(2, 7, FALSE);

  lbl2 = gtk_label_new("Network object file name ");
  gtk_misc_set_alignment(GTK_MISC(lbl2), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabf), lbl2, 0, 1, 0, 1);
  ent2 = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(ent2), 0);
  gtk_entry_set_width_chars(GTK_ENTRY(ent2), 50);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent2), 50);
  gtk_entry_set_icon_from_stock(GTK_ENTRY(ent2), GTK_ENTRY_ICON_SECONDARY, GTK_STOCK_FILE);
  gtk_entry_set_text(GTK_ENTRY(ent2), sb->cfg.base_name);
  gtk_table_attach_defaults(GTK_TABLE(tabf), ent2, 1, 2, 0, 1);
  g_signal_connect(G_OBJECT(ent2), "changed", G_CALLBACK(base_fname), (gpointer)sb);
  g_signal_connect(G_OBJECT(ent2), "draw", G_CALLBACK(base_default), (gpointer)sb);
  g_signal_connect(G_OBJECT(ent2), "icon-press", G_CALLBACK(base_dialog), (gpointer)sb);

  lbl3 = gtk_label_new("Initial conditions file name ");
  gtk_misc_set_alignment(GTK_MISC(lbl3), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabf), lbl3, 0, 1, 1, 2);
  ent3 = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(ent3), 0);
  gtk_entry_set_width_chars(GTK_ENTRY(ent3), 50);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent3), 50);
  gtk_entry_set_icon_from_stock(GTK_ENTRY(ent3), GTK_ENTRY_ICON_SECONDARY, GTK_STOCK_FILE);
  gtk_entry_set_text(GTK_ENTRY(ent3), sb->cfg.state_name);
  gtk_table_attach_defaults(GTK_TABLE(tabf), ent3, 1, 2, 1, 2);
  g_signal_connect(G_OBJECT(ent3), "changed", G_CALLBACK(state_fname), (gpointer)sb);
  g_signal_connect(G_OBJECT(ent3), "draw", G_CALLBACK(state_default), (gpointer)sb);
  g_signal_connect(G_OBJECT(ent3), "icon-press", G_CALLBACK(state_dialog), (gpointer)sb);

  lbl4 = gtk_label_new("Log file name ");
  gtk_misc_set_alignment(GTK_MISC(lbl4), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabf), lbl4, 0, 1, 2, 3);
  ent4 = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(ent4), 0);
  gtk_entry_set_width_chars(GTK_ENTRY(ent4), 50);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent4), 50);
  gtk_entry_set_icon_from_stock(GTK_ENTRY(ent4), GTK_ENTRY_ICON_SECONDARY, GTK_STOCK_FILE);
  gtk_entry_set_text(GTK_ENTRY(ent4), sb->cfg.logfile_name);
  gtk_table_attach_defaults(GTK_TABLE(tabf), ent4, 1, 2, 2, 3);
  g_signal_connect(G_OBJECT(ent4), "changed", G_CALLBACK(logfile_fname), (gpointer)sb);
  g_signal_connect(G_OBJECT(ent4), "draw", G_CALLBACK(logfile_default), (gpointer)sb);
  g_signal_connect(G_OBJECT(ent4), "icon-press", G_CALLBACK(logfile_dialog), (gpointer)sb);

  lbl10 = gtk_label_new("Symbol table file name ");
  gtk_misc_set_alignment(GTK_MISC(lbl10), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabf), lbl10, 0, 1, 3, 4);
  ent10 = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(ent10), 0);
  gtk_entry_set_width_chars(GTK_ENTRY(ent10), 50);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent10), 50);
  gtk_entry_set_icon_from_stock(GTK_ENTRY(ent10), GTK_ENTRY_ICON_SECONDARY, GTK_STOCK_FILE);
  gtk_entry_set_text(GTK_ENTRY(ent10), sb->cfg.xref_name);
  gtk_table_attach_defaults(GTK_TABLE(tabf), ent10, 1, 2, 3, 4);
  g_signal_connect(G_OBJECT(ent10), "changed", G_CALLBACK(xref_fname), (gpointer)sb);
  g_signal_connect(G_OBJECT(ent10), "draw", G_CALLBACK(xref_default), (gpointer)sb);
  g_signal_connect(G_OBJECT(ent10), "icon-press", G_CALLBACK(xref_dialog), (gpointer)sb);

  lbl20 = gtk_label_new("Module file path ");
  gtk_misc_set_alignment(GTK_MISC(lbl20), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabf), lbl20, 0, 1, 4, 5);
  ent20 = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(ent20), 0);
  gtk_entry_set_width_chars(GTK_ENTRY(ent20), 50);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent20), 50);
  gtk_entry_set_icon_from_stock(GTK_ENTRY(ent20), GTK_ENTRY_ICON_SECONDARY, GTK_STOCK_FILE);
  gtk_entry_set_text(GTK_ENTRY(ent20), sb->cfg.include_path);
  gtk_table_attach_defaults(GTK_TABLE(tabf), ent20, 1, 2, 4, 5);
  g_signal_connect(G_OBJECT(ent20), "changed", G_CALLBACK(include_path_fname), (gpointer)sb);
  g_signal_connect(G_OBJECT(ent20), "draw", G_CALLBACK(include_path_default), (gpointer)sb);
  g_signal_connect(G_OBJECT(ent20), "icon-press", G_CALLBACK(include_path_dialog), (gpointer)sb);

  lbl21 = gtk_label_new("Default text editor ");
  gtk_misc_set_alignment(GTK_MISC(lbl21), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabf), lbl21, 0, 1, 5, 6);
  ent21 = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(ent21), 0);
  gtk_entry_set_width_chars(GTK_ENTRY(ent21), 50);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent21), 50);
  gtk_entry_set_text(GTK_ENTRY(ent21), sb->cfg.editor_name);
  gtk_table_attach_defaults(GTK_TABLE(tabf), ent21, 1, 2, 5, 6);
  g_signal_connect(G_OBJECT(ent21), "changed", G_CALLBACK(editor_fname), (gpointer)sb);
  g_signal_connect(G_OBJECT(ent21), "draw", G_CALLBACK(editor_default), (gpointer)sb);

  lbl22 = gtk_label_new("Default help viewer ");
  gtk_misc_set_alignment(GTK_MISC(lbl22), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabf), lbl22, 0, 1, 6, 7);
  ent22 = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(ent22), 0);
  gtk_entry_set_width_chars(GTK_ENTRY(ent22), 50);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent22), 50);
  gtk_entry_set_text(GTK_ENTRY(ent22), sb->cfg.viewer_name);
  gtk_table_attach_defaults(GTK_TABLE(tabf), ent22, 1, 2, 6, 7);
  g_signal_connect(G_OBJECT(ent22), "changed", G_CALLBACK(viewer_fname), (gpointer)sb);
  g_signal_connect(G_OBJECT(ent22), "draw", G_CALLBACK(viewer_default), (gpointer)sb);

  gtk_container_add(GTK_CONTAINER(vbox1), tabf);
  gtk_container_add(GTK_CONTAINER(fr1), vbox1);
  gtk_box_pack_start(GTK_BOX(extbox), fr1, TRUE, TRUE, 0);

  fr4 = gtk_frame_new("Input and output parameters");
  gtk_label_set_markup(GTK_LABEL(gtk_frame_get_label_widget(GTK_FRAME(fr4))), "<b>Input and output parameters</b>");
  hbox4 = gtk_hbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(fr4), 5);
  gtk_container_set_border_width(GTK_CONTAINER(hbox4), 5);

  vbox8 = gtk_vbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(vbox8), 5);

  tabg = gtk_table_new(2, 2, FALSE);

  lbl13 = gtk_label_new("IPC prefix ");
  gtk_misc_set_alignment(GTK_MISC(lbl13), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabg), lbl13, 0, 1, 0, 1);
  ent13 = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(ent13), 0);
  gtk_entry_set_width_chars(GTK_ENTRY(ent13), 50);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent13), 50);
  gtk_entry_set_text(GTK_ENTRY(ent13), sb->cfg.prefix);
  gtk_table_attach_defaults(GTK_TABLE(tabg), ent13, 1, 2, 0, 1);
  g_signal_connect(G_OBJECT(ent13), "changed", G_CALLBACK(prefix_fname), (gpointer)sb);
  g_signal_connect(G_OBJECT(ent13), "draw", G_CALLBACK(prefix_default), (gpointer)sb);

  lbl12 = gtk_label_new("I/O file path ");
  gtk_misc_set_alignment(GTK_MISC(lbl12), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabg), lbl12, 0, 1, 1, 2);
  ent12 = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(ent12), 0);
  gtk_entry_set_width_chars(GTK_ENTRY(ent12), 50);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent12), 50);
  gtk_entry_set_icon_from_stock(GTK_ENTRY(ent12), GTK_ENTRY_ICON_SECONDARY, GTK_STOCK_FILE);
  gtk_entry_set_text(GTK_ENTRY(ent12), sb->cfg.path);
  gtk_table_attach_defaults(GTK_TABLE(tabg), ent12, 1, 2, 1, 2);
  g_signal_connect(G_OBJECT(ent12), "changed", G_CALLBACK(path_fname), (gpointer)sb);
  g_signal_connect(G_OBJECT(ent12), "draw", G_CALLBACK(path_default), (gpointer)sb);
  g_signal_connect(G_OBJECT(ent12), "icon-press", G_CALLBACK(path_dialog), (gpointer)sb);

  gtk_box_pack_start(GTK_BOX(vbox8), tabg, TRUE, TRUE, 0);

  tabs = gtk_table_new(8, 1, FALSE);

  buf[0] = sb->cfg.alpha[false_symbol];
  lbl14 = gtk_label_new("False char ");
  gtk_misc_set_alignment(GTK_MISC(lbl14), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabs), lbl14, 0, 1, 0, 1);
  ent14 = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(ent14), 1);
  gtk_entry_set_width_chars(GTK_ENTRY(ent14), 1);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent14), 1);
  gtk_entry_set_text(GTK_ENTRY(ent14), buf);
  gtk_table_attach_defaults(GTK_TABLE(tabs), ent14, 1, 2, 0, 1);
  g_signal_connect(G_OBJECT(ent14), "changed", G_CALLBACK(false_sym), (gpointer)sb);

  buf[0] = sb->cfg.alpha[true_symbol];
  lbl15 = gtk_label_new("True char ");
  gtk_misc_set_alignment(GTK_MISC(lbl15), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabs), lbl15, 2, 3, 0, 1);
  ent15 = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(ent15), 1);
  gtk_entry_set_width_chars(GTK_ENTRY(ent15), 1);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent15), 1);
  gtk_entry_set_text(GTK_ENTRY(ent15), buf);
  gtk_table_attach_defaults(GTK_TABLE(tabs), ent15, 3, 4, 0, 1);
  g_signal_connect(G_OBJECT(ent15), "changed", G_CALLBACK(true_sym), (gpointer)sb);

  buf[0] = sb->cfg.alpha[unknown_symbol];
  lbl16 = gtk_label_new("Unknown char ");
  gtk_misc_set_alignment(GTK_MISC(lbl16), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabs), lbl16, 4, 5, 0, 1);
  ent16 = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(ent16), 1);
  gtk_entry_set_width_chars(GTK_ENTRY(ent16), 1);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent16), 1);
  gtk_entry_set_text(GTK_ENTRY(ent16), buf);
  gtk_table_attach_defaults(GTK_TABLE(tabs), ent16, 5, 6, 0, 1);
  g_signal_connect(G_OBJECT(ent16), "changed", G_CALLBACK(unknown_sym), (gpointer)sb);

  buf[0] = sb->cfg.alpha[end_symbol];
  lbl17 = gtk_label_new("End char ");
  gtk_misc_set_alignment(GTK_MISC(lbl17), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabs), lbl17, 6, 7, 0, 1);
  ent17 = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(ent17), 1);
  gtk_entry_set_width_chars(GTK_ENTRY(ent17), 1);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent17), 1);
  gtk_entry_set_text(GTK_ENTRY(ent17), buf);
  gtk_table_attach_defaults(GTK_TABLE(tabs), ent17, 7, 8, 0, 1);
  g_signal_connect(G_OBJECT(ent17), "changed", G_CALLBACK(end_sym), (gpointer)sb);

  gtk_box_pack_end(GTK_BOX(vbox8), tabs, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(hbox4), vbox8, TRUE, TRUE, 0);

  vbox7 = gtk_vbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(vbox7), 5);

  tabw = gtk_table_new(2, 3, FALSE);

  cb10 = gtk_check_button_new_with_label("External inputs");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb10), sb->cfg.batch_in);
  gtk_table_attach_defaults(GTK_TABLE(tabw), cb10, 0, 1, 0, 1);
  g_signal_connect(G_OBJECT(cb10), "clicked", G_CALLBACK(batch_in_box), (gpointer)sb);

  cb11 = gtk_check_button_new_with_label("External outputs");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb11), sb->cfg.batch_out);
  gtk_table_attach_defaults(GTK_TABLE(tabw), cb11, 1, 2, 0, 1);
  g_signal_connect(G_OBJECT(cb11), "clicked", G_CALLBACK(batch_out_box), (gpointer)sb);

  cb18 = gtk_check_button_new_with_label("System V IPC");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb18), sb->cfg.sys5);
  gtk_table_attach_defaults(GTK_TABLE(tabw), cb18, 0, 1, 1, 2);
  g_signal_connect(G_OBJECT(cb18), "clicked", G_CALLBACK(sys5_box), (gpointer)sb);

  cb9 = gtk_check_button_new_with_label("Ignore IPC errors");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb9), sb->cfg.sturdy);
  gtk_table_attach_defaults(GTK_TABLE(tabw), cb9, 1, 2, 1, 2);
  g_signal_connect(G_OBJECT(cb9), "clicked", G_CALLBACK(sturdy_box), (gpointer)sb);

  cb14 = gtk_check_button_new_with_label("Display internal signals");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb14), sb->cfg.outint);
  gtk_table_attach_defaults(GTK_TABLE(tabw), cb14, 0, 1, 2, 3);
  g_signal_connect(G_OBJECT(cb14), "clicked", G_CALLBACK(outint_box), (gpointer)sb);

  cb15 = gtk_check_button_new_with_label("Display unknowns as dots");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb15), sb->cfg.draw_undef);
  gtk_table_attach_defaults(GTK_TABLE(tabw), cb15, 1, 2, 2, 3);
  g_signal_connect(G_OBJECT(cb15), "clicked", G_CALLBACK(draw_undef_box), (gpointer)sb);

  gtk_container_add(GTK_CONTAINER(vbox7), tabw);
  gtk_box_pack_end(GTK_BOX(hbox4), vbox7, TRUE, TRUE, 0);

  gtk_container_add(GTK_CONTAINER(fr4), hbox4);
  gtk_box_pack_start(GTK_BOX(extbox), fr4, TRUE, TRUE, 0);

  hbox1 = gtk_hbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(hbox1), 5);

  fr2 = gtk_frame_new("Resources");
  gtk_label_set_markup(GTK_LABEL(gtk_frame_get_label_widget(GTK_FRAME(fr2))), "<b>Resources</b>");
  vbox2 = gtk_vbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(fr2), 5);
  gtk_container_set_border_width(GTK_CONTAINER(vbox2), 5);

  tabk = gtk_table_new(2, 4, FALSE);

  lbl7 = gtk_label_new("Number of processes ");
  gtk_misc_set_alignment(GTK_MISC(lbl7), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabk), lbl7, 0, 1, 0, 1);
  ent7 = gtk_spin_button_new_with_range(1, MAX_THREADS, 1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(ent7), sb->cfg.num_threads);
  gtk_table_attach_defaults(GTK_TABLE(tabk), ent7, 1, 2, 0, 1);
  g_signal_connect(G_OBJECT(ent7), "value-changed", G_CALLBACK(num_threads_value), (gpointer)sb);

  lbl11 = gtk_label_new("Memory size logarithm ");
  gtk_misc_set_alignment(GTK_MISC(lbl11), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabk), lbl11, 0, 1, 1, 2);
  ent11 = gtk_spin_button_new_with_range(MIN_BSBT, MAX_BSBT, 1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(ent11), sb->cfg.bsbt);
  gtk_table_attach_defaults(GTK_TABLE(tabk), ent11, 1, 2, 1, 2);
  g_signal_connect(G_OBJECT(ent11), "value-changed", G_CALLBACK(bsbt_value), (gpointer)sb);

  lbl18 = gtk_label_new("History window columns ");
  gtk_misc_set_alignment(GTK_MISC(lbl18), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabk), lbl18, 0, 1, 2, 3);
  ent18 = gtk_spin_button_new_with_range(1, MAX_HORIZON_SIZE - 1, 1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(ent18), sb->cfg.horizon_size - 1);
  gtk_table_attach_defaults(GTK_TABLE(tabk), ent18, 1, 2, 2, 3);
  g_signal_connect(G_OBJECT(ent18), "value-changed", G_CALLBACK(horizon_value), (gpointer)sb);

  lbl19 = gtk_label_new("History window max rows ");
  gtk_misc_set_alignment(GTK_MISC(lbl19), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabk), lbl19, 0, 1, 3, 4);
  ent19 = gtk_spin_button_new_with_range(1, MAX_DISPLAY_ROWS, 1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(ent19), sb->cfg.display_rows);
  gtk_table_attach_defaults(GTK_TABLE(tabk), ent19, 1, 2, 3, 4);
  g_signal_connect(G_OBJECT(ent19), "value-changed", G_CALLBACK(rows_value), (gpointer)sb);

  gtk_container_add(GTK_CONTAINER(vbox2), tabk);
  gtk_container_add(GTK_CONTAINER(fr2), vbox2);
  gtk_box_pack_start(GTK_BOX(hbox1), fr2, TRUE, TRUE, 0);

  fr3 = gtk_frame_new("Miscellaneous");
  gtk_label_set_markup(GTK_LABEL(gtk_frame_get_label_widget(GTK_FRAME(fr3))), "<b>Miscellaneous</b>");
  vbox3 = gtk_vbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(fr3), 5);
  gtk_container_set_border_width(GTK_CONTAINER(vbox3), 5);

  tabh = gtk_table_new(2, 4, FALSE);

  cb16 = gtk_check_button_new_with_label("Optimize joints in intervals");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb16), sb->cfg.seplit_fe);
  gtk_table_attach_defaults(GTK_TABLE(tabh), cb16, 0, 1, 0, 1);
  g_signal_connect(G_OBJECT(cb16), "clicked", G_CALLBACK(seplit_fe_box), (gpointer)sb);

  cb17 = gtk_check_button_new_with_label("Optimize joints in recursions");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb17), sb->cfg.seplit_su);
  gtk_table_attach_defaults(GTK_TABLE(tabh), cb17, 1, 2, 0, 1);
  g_signal_connect(G_OBJECT(cb17), "clicked", G_CALLBACK(seplit_su_box), (gpointer)sb);

  cb13 = gtk_check_button_new_with_label("Optimize delays");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb13), sb->cfg.merge);
  gtk_table_attach_defaults(GTK_TABLE(tabh), cb13, 0, 1, 1, 2);
  g_signal_connect(G_OBJECT(cb13), "clicked", G_CALLBACK(merge_box), (gpointer)sb);

  cb19 = gtk_check_button_new_with_label("Generate constant signals");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb19), sb->cfg.constout);
  gtk_table_attach_defaults(GTK_TABLE(tabh), cb19, 1, 2, 1, 2);
  g_signal_connect(G_OBJECT(cb19), "clicked", G_CALLBACK(constout_box), (gpointer)sb);

  cb8 = gtk_check_button_new_with_label("Hard real time (root access only)");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb8), sb->cfg.hard);
  gtk_table_attach_defaults(GTK_TABLE(tabh), cb8, 0, 1, 2, 3);
  g_signal_connect(G_OBJECT(cb8), "clicked", G_CALLBACK(hard_box), (gpointer)sb);

  cb12 = gtk_check_button_new_with_label("Wait running");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb12), sb->cfg.busywait);
  gtk_table_attach_defaults(GTK_TABLE(tabh), cb12, 1, 2, 2, 3);
  g_signal_connect(G_OBJECT(cb12), "clicked", G_CALLBACK(busywait_box), (gpointer)sb);

  cb20 = gtk_check_button_new_with_label("Display full signal names");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb20), sb->cfg.full_names);
  gtk_table_attach_defaults(GTK_TABLE(tabh), cb20, 0, 1, 3, 4);
  g_signal_connect(G_OBJECT(cb20), "clicked", G_CALLBACK(full_names_box), (gpointer)sb);

  gtk_container_add(GTK_CONTAINER(vbox3), tabh);
  gtk_container_add(GTK_CONTAINER(fr3), vbox3);
  gtk_box_pack_end(GTK_BOX(hbox1), fr3, TRUE, TRUE, 0);

  gtk_box_pack_start(GTK_BOX(extbox), hbox1, TRUE, TRUE, 0);

  vbox6 = gtk_vbox_new(FALSE, 15);
  hbox2 = gtk_hbox_new(FALSE, 15);

  btn5 = gtk_button_new_with_label("Save configuration");
  gtk_widget_set_size_request(btn5, 70, 30);
  gtk_box_pack_start(GTK_BOX(hbox2), btn5, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(btn5), "clicked", G_CALLBACK(save_button_clicked), (gpointer)sb);

  btn7 = gtk_button_new_with_label("Probabilities...");
  gtk_widget_set_size_request(btn7, 70, 30);
  gtk_box_pack_start(GTK_BOX(hbox2), btn7, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(btn7), "clicked", G_CALLBACK(prob_button_clicked), (gpointer)sb);

  btn8 = gtk_button_new_with_label("External signals...");
  gtk_widget_set_size_request(btn8, 70, 30);
  gtk_box_pack_start(GTK_BOX(hbox2), btn8, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(btn8), "clicked", G_CALLBACK(filter_button_clicked), (gpointer)sb);

  btn6 = gtk_button_new_with_label("Clear configuration");
  gtk_widget_set_size_request(btn6, 70, 30);
  gtk_box_pack_end(GTK_BOX(hbox2), btn6, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(btn6), "clicked", G_CALLBACK(erase_button_clicked), (gpointer)sb);

  sb->save_button = GTK_BUTTON(btn5);
  sb->erase_button = GTK_BUTTON(btn6);

  if(sb->changed)
    gtk_widget_set_sensitive(btn5, TRUE);
  else
    gtk_widget_set_sensitive(btn5, FALSE);

  if(sb->configured)
    gtk_widget_set_sensitive(btn6, TRUE);
  else
    gtk_widget_set_sensitive(btn6, FALSE);

  gtk_box_pack_end(GTK_BOX(vbox6), hbox2, FALSE, FALSE, 0);

  gtk_box_pack_end(GTK_BOX(extbox), vbox6, FALSE, FALSE, 0);

  gtk_container_add(GTK_CONTAINER(winbox), extbox);

  gtk_container_add(GTK_CONTAINER(window), winbox);

  gtk_widget_show_all(window);

  pthread_mutex_unlock(&sb->mutex_button);
}

int execute(char *source_name, char *base_name, char *state_name, char *logfile_name, char *xref_name,
         bool strictly_causal, bool soundness_check, bool echo_stdout, bool file_io, bool quiet, bool hard, bool sys5, bool sturdy, bool busywait, bool seplit_fe, bool seplit_su, bool merge,
         bool outaux, bool outint, int bufexp, d_time max_time, m_time step, char *prefix, char *path, char *include_path, char *alpha, int num_threads, double prob,
         bool batch_in, bool batch_out, bool draw_undef)
{
  s_base sbs;
  GtkWidget *window;
  GtkWidget *drawingarea;
  GtkWidget *textarea, *scrollarea;
  GtkWidget *btn1, *btn2, *btn3, *btn4, *btn5, *btn6, *btn7;
  GtkWidget *cb0, *cb1, *cb2, *cb3, *cb4, *cb5, *cb6, *cb7, *cb10, *cb14;
  GtkWidget *tabf, *tabi, *tabt, *tabh, *tabx;
  GtkWidget *extbox, *winbox, *hboxctl1, *hboxctl2;
  GtkWidget *ent0, *lbl1, *ent1, *lbl5, *ent5, *lbl6, *ent6, *lbl8, *ent8, *lbl9, *ent9;
  GtkWidget *frgfx, *hboxgfx, *frtxt, *vboxtxt, *fr1, *vbox1, *fr2, *vbox2, *fr3, *vbox3, *fr4, *vbox4, *fr6, *vbox6;
  GtkWidget *vbox5, *hbox3, *hbox5, *hbox6, *lbl0, *lblt, *lblg, *icng;
  GtkWidget *menubar, *menu_items, *project_menu, *open_menu, *gen_menu, *run_menu, *config_menu, *prob_menu, *filter_menu, *save_menu, *erase_menu, *help_menu, *about_menu, *quit_menu, *sep1, *sep2;
  GtkTextBuffer *textbuffer;
  GtkTextIter iter;
  GtkAdjustment *adj;
  FILE *fp;
  int i;

  memset(&sbs, 0, sizeof(s_base));

  fp = fopen(CONFIG_FILENAME, "r");
  if(fp)
    {
      if(fread(&sbs.cfg, sizeof(sbs.cfg), 1, fp) == 1)
        sbs.configured = TRUE;
      else
        sbs.configured = FALSE;

      fclose(fp);
    }
  else
    sbs.configured = FALSE;

  if(!sbs.configured)
    {
      strcpy(sbs.cfg.source_name, source_name);

      if(!base_name)
        base_name = source_name;

      strcpy(sbs.cfg.base_name, base_name);

      if(state_name)
        {
          strcpy(sbs.cfg.state_name, state_name);
          sbs.cfg.load_state = TRUE;
        }
      else
        {
          strcpy(sbs.cfg.state_name, base_name);
          strcat(sbs.cfg.state_name, STATE_SUFFIX);
          sbs.cfg.load_state = FALSE;
        }

      if(logfile_name)
        {
          strcpy(sbs.cfg.logfile_name, logfile_name);
          sbs.cfg.echo_logfile = TRUE;
        }
      else
        {
          strcpy(sbs.cfg.logfile_name, base_name);
          strcat(sbs.cfg.logfile_name, LOG_SUFFIX);
          sbs.cfg.echo_logfile = FALSE;
        }

      if(xref_name)
        {
          strcpy(sbs.cfg.xref_name, xref_name);
          sbs.cfg.use_xref = TRUE;
        }
      else
        {
          strcpy(sbs.cfg.xref_name, base_name);
          sbs.cfg.use_xref = FALSE;
        }

      sbs.cfg.bsbt = bufexp;
      sbs.cfg.step = step;
      sbs.cfg.max_time = max_time;
      sbs.cfg.num_threads = num_threads;
      sbs.cfg.prob = prob;
      sbs.cfg.correction = 0;
      strcpy(sbs.cfg.prefix, prefix);
      strcpy(sbs.cfg.path, path);
      strcpy(sbs.cfg.include_path, include_path);
      strncpy(sbs.cfg.alpha, alpha, end_symbol - false_symbol + 1);
      sbs.cfg.strictly_causal = strictly_causal;
      sbs.cfg.soundness_check = soundness_check;
      sbs.cfg.echo_stdout = echo_stdout;
      sbs.cfg.file_io = file_io;
      sbs.cfg.quiet = quiet;
      sbs.cfg.hard = hard;
      sbs.cfg.sys5 = sys5;
      sbs.cfg.sturdy = sturdy;
      sbs.cfg.busywait = busywait;
      sbs.cfg.seplit_fe = seplit_fe;
      sbs.cfg.seplit_su = seplit_su;
      sbs.cfg.merge = merge;
      sbs.cfg.outaux = outaux;
      sbs.cfg.outint = outint;
      sbs.cfg.batch_in = batch_in;
      sbs.cfg.batch_out = batch_out;
      sbs.cfg.draw_undef = draw_undef;
      sbs.cfg.full_names = FALSE;
      sbs.cfg.horizon_size = DEFAULT_HORIZON_SIZE;
      sbs.cfg.display_rows = DEFAULT_DISPLAY_ROWS;
      strcpy(sbs.cfg.editor_name, EDITOR_FILENAME);
      strcpy(sbs.cfg.viewer_name, VIEWER_FILENAME);

      for(i = 0; i < MAX_FILES; i++)
        {
          sbs.cfg.inprob[i] = 1;
          sbs.cfg.fexcl[i] = FALSE;
          sbs.cfg.gexcl[i] = FALSE;
        }

      sbs.cfg.fn = 0;
      sbs.cfg.gn = 0;

      sbs.configured = FALSE;
    }

  sbs.cp_step = step;
  sbs.cp_max_time = max_time;
  sbs.cp_echo_stdout = echo_stdout;
  sbs.cp_file_io = file_io;
  sbs.cp_quiet = quiet;
  sbs.cp_sys5 = sys5;
  sbs.cp_batch_in = batch_in;
  sbs.cp_batch_out = batch_out;
  sbs.cp_full_names = FALSE;
  sbs.cp_horizon_size = DEFAULT_HORIZON_SIZE;
  sbs.cp_display_rows = DEFAULT_DISPLAY_ROWS;

  sbs.fn = 0;
  sbs.gn = 0;
  sbs.pos = 0;
  sbs.t = 0;
  sbs.time = 0;
  sbs.time_base = 0;
  sbs.maxlen = 0;
  sbs.xstart = 0;
  sbs.xend = 0;
  sbs.xcat = FALSE;
  sbs.mt = FALSE;
  sbs.rs = stopped;
  sbs.term = FALSE;
  sbs.sent = FALSE;
  sbs.regenerate = FALSE;
  sbs.changed = FALSE;
  sbs.changeprob = FALSE;

  gtk_disable_setlocale();

  gtk_init(NULL, NULL);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_WIDTH, WINDOW_HEIGHT);
  gtk_container_set_border_width(GTK_CONTAINER(window), 0);
  gtk_window_set_title(GTK_WINDOW(window), TITLE);
  gtk_window_set_icon_name(GTK_WINDOW(window), "tinx");
  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(exit_button_clicked), (gpointer)&sbs);
  g_signal_connect(G_OBJECT(window), "delete-event", G_CALLBACK(exit_button_clicked_if), (gpointer)&sbs);

  sbs.window = GTK_WINDOW(window);
  sbs.config_window = NULL;
  sbs.prob_window = NULL;

  sbs.save_button = NULL;
  sbs.erase_button = NULL;
  sbs.clear_button = NULL;
  sbs.edit_button = NULL;
  sbs.help_button = NULL;

  winbox = gtk_vbox_new(FALSE, 0);

  menubar = gtk_menu_bar_new();

  project_menu = gtk_menu_item_new_with_label("Project");
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), project_menu);

  menu_items = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(project_menu), menu_items);

  open_menu = gtk_menu_item_new_with_label("Open...");
  gen_menu = gtk_menu_item_new_with_label("Generate network");
  run_menu = gtk_menu_item_new_with_label("Execute network");

  sbs.run_menu = GTK_MENU_ITEM(run_menu);

  sep1 = gtk_separator_menu_item_new();

  config_menu = gtk_menu_item_new_with_label("Configure...");
  prob_menu = gtk_menu_item_new_with_label("Probabilities...");
  filter_menu = gtk_menu_item_new_with_label("External signals...");
  save_menu = gtk_menu_item_new_with_label("Save configuration");
  erase_menu = gtk_menu_item_new_with_label("Clear configuration");

  sbs.save_menu = GTK_MENU_ITEM(save_menu);
  sbs.erase_menu = GTK_MENU_ITEM(erase_menu);

  gtk_widget_set_sensitive(save_menu, FALSE);

  if(sbs.configured)
    gtk_widget_set_sensitive(erase_menu, TRUE);
  else
    gtk_widget_set_sensitive(erase_menu, FALSE);

  sep2 = gtk_separator_menu_item_new();

  help_menu = gtk_menu_item_new_with_label("Help...");

  sbs.help_menu = GTK_MENU_ITEM(help_menu);

  if(*sbs.cfg.viewer_name)
    gtk_widget_set_sensitive(help_menu, TRUE);
  else
    gtk_widget_set_sensitive(help_menu, FALSE);

  about_menu = gtk_menu_item_new_with_label("About");
  quit_menu = gtk_menu_item_new_with_label("Exit");

  gtk_menu_shell_append(GTK_MENU_SHELL(menu_items), open_menu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_items), gen_menu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_items), run_menu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_items), sep1);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_items), config_menu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_items), prob_menu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_items), filter_menu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_items), save_menu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_items), erase_menu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_items), sep2);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_items), help_menu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_items), about_menu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_items), quit_menu);

  g_signal_connect(open_menu, "activate", G_CALLBACK(source_dialog_if), (gpointer)&sbs);
  g_signal_connect(gen_menu, "activate", G_CALLBACK(gen_button_clicked), (gpointer)&sbs);
  g_signal_connect(run_menu, "activate", G_CALLBACK(run_button_clicked), (gpointer)&sbs);
  g_signal_connect(config_menu, "activate", G_CALLBACK(configure), (gpointer)&sbs);
  g_signal_connect(prob_menu, "activate", G_CALLBACK(prob_button_clicked), (gpointer)&sbs);
  g_signal_connect(filter_menu, "activate", G_CALLBACK(filter_button_clicked), (gpointer)&sbs);
  g_signal_connect(save_menu, "activate", G_CALLBACK(save_button_clicked), (gpointer)&sbs);
  g_signal_connect(erase_menu, "activate", G_CALLBACK(erase_button_clicked), (gpointer)&sbs);
  g_signal_connect(help_menu, "activate", G_CALLBACK(help_button_clicked), (gpointer)&sbs);
  g_signal_connect(about_menu, "activate", G_CALLBACK(about_button_clicked), (gpointer)&sbs);
  g_signal_connect(quit_menu, "activate", G_CALLBACK(exit_button_clicked), (gpointer)&sbs);

  gtk_box_pack_start(GTK_BOX(winbox), menubar, FALSE, FALSE, 0);

  extbox = gtk_vbox_new(FALSE, 0);

  gtk_container_set_border_width(GTK_CONTAINER(extbox), 10);

  frgfx = gtk_frame_new("History of input and output signals");
  gtk_label_set_markup(GTK_LABEL(gtk_frame_get_label_widget(GTK_FRAME(frgfx))), "<b>History of input and output signals</b>");
  gtk_frame_set_label_align(GTK_FRAME(frgfx), 0.5, 0.5);

  hboxgfx = gtk_hbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(frgfx), 5);
  gtk_container_set_border_width(GTK_CONTAINER(hboxgfx), 5);

  drawingarea = gtk_drawing_area_new();
  gtk_widget_set_size_request(drawingarea, WINDOW_WIDTH, GRAPHICS_HEIGHT);
  g_signal_connect(G_OBJECT(drawingarea), "draw", G_CALLBACK(draw_callback), &sbs);
  gtk_widget_add_tick_callback(drawingarea, (gboolean (*)(GtkWidget *, GdkFrameClock *, gpointer))tick_callback, &sbs, NULL);

  sbs.drawingarea = GTK_DRAWING_AREA(drawingarea);

  adj = gtk_adjustment_new(0, 0, 1, 1, 1, 1);
  ent0 = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, adj);
  g_signal_connect(G_OBJECT(ent0), "value-changed", G_CALLBACK(pos_value), (gpointer)&sbs);

  sbs.area_adj = adj;

  gtk_box_pack_start(GTK_BOX(hboxgfx), drawingarea, TRUE, TRUE, 0);
  gtk_box_pack_end(GTK_BOX(hboxgfx), ent0, FALSE, FALSE, 0);
  gtk_container_add(GTK_CONTAINER(frgfx), hboxgfx);

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

  gtk_container_add(GTK_CONTAINER(vboxtxt), scrollarea);
  gtk_container_add(GTK_CONTAINER(frtxt), vboxtxt);

  hboxctl1 = gtk_hbox_new(FALSE, 0);
  hboxctl2 = gtk_hbox_new(FALSE, 0);

  fr1 = gtk_frame_new("Project");
  gtk_label_set_markup(GTK_LABEL(gtk_frame_get_label_widget(GTK_FRAME(fr1))), "<b>Project</b>");
  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(fr1), 5);
  gtk_container_set_border_width(GTK_CONTAINER(vbox1), 5);

  tabf = gtk_table_new(3, 2, FALSE);

  lbl1 = gtk_label_new("Temporal logic source file name ");
  gtk_misc_set_alignment(GTK_MISC(lbl1), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabf), lbl1, 0, 1, 0, 1);
  ent1 = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(ent1), 0);
  gtk_entry_set_width_chars(GTK_ENTRY(ent1), 50);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent1), 50);
  gtk_entry_set_icon_from_stock(GTK_ENTRY(ent1), GTK_ENTRY_ICON_SECONDARY, GTK_STOCK_FILE);
  gtk_entry_set_text(GTK_ENTRY(ent1), sbs.cfg.source_name);
  gtk_table_attach_defaults(GTK_TABLE(tabf), ent1, 1, 2, 0, 1);

  hbox6 = gtk_hbox_new(FALSE, 15);
  btn6 = gtk_button_new_with_label("Edit...");
  gtk_box_pack_end(GTK_BOX(hbox6), btn6, FALSE, FALSE, 5);
  gtk_table_attach_defaults(GTK_TABLE(tabf), hbox6, 2, 3, 0, 1);
  g_signal_connect(G_OBJECT(btn6), "clicked", G_CALLBACK(edit_button_clicked), (gpointer)&sbs);

  sbs.edit_button = GTK_BUTTON(btn6);

  if(*sbs.cfg.editor_name)
    gtk_widget_set_sensitive(btn6, TRUE);
  else
    gtk_widget_set_sensitive(btn6, FALSE);

  lblg = gtk_label_new(NULL);
  icng = gtk_image_new();
  hbox3 = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(hbox3), icng, TRUE, TRUE, 0);
  gtk_box_pack_end(GTK_BOX(hbox3), lblg, TRUE, TRUE, 0);
  gtk_misc_set_alignment(GTK_MISC(icng), 1, 0.5);
  gtk_misc_set_alignment(GTK_MISC(lblg), 0, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabf), hbox3, 0, 1, 1, 2);
  g_signal_connect(G_OBJECT(ent1), "changed", G_CALLBACK(source_fname), (gpointer)&sbs);
  g_signal_connect(G_OBJECT(ent1), "draw", G_CALLBACK(source_default), (gpointer)&sbs);
  g_signal_connect(G_OBJECT(ent1), "icon-press", G_CALLBACK(source_dialog), (gpointer)&sbs);

  sbs.reg_warning = GTK_LABEL(lblg);
  sbs.reg_warning_icon = GTK_IMAGE(icng);

  cb1 = gtk_check_button_new_with_label("Load initial conditions");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb1), sbs.cfg.load_state);
  gtk_table_attach_defaults(GTK_TABLE(tabf), cb1, 1, 2, 1, 2);
  g_signal_connect(G_OBJECT(cb1), "clicked", G_CALLBACK(load_state_box), (gpointer)&sbs);

  gtk_container_add(GTK_CONTAINER(vbox1), tabf);
  gtk_container_add(GTK_CONTAINER(fr1), vbox1);
  gtk_box_pack_start(GTK_BOX(hboxctl1), fr1, TRUE, TRUE, 0);

  fr3 = gtk_frame_new("Time");
  gtk_label_set_markup(GTK_LABEL(gtk_frame_get_label_widget(GTK_FRAME(fr3))), "<b>Time</b>");
  vbox3 = gtk_vbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(fr3), 5);
  gtk_container_set_border_width(GTK_CONTAINER(vbox3), 5);

  tabt = gtk_table_new(2, 2, FALSE);

  lbl5 = gtk_label_new("Sampling time (s) ");
  gtk_misc_set_alignment(GTK_MISC(lbl5), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabt), lbl5, 0, 1, 0, 1);
  ent5 = gtk_spin_button_new_with_range(0, 999999.999999, 0.000001);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(ent5), sbs.cfg.step);
  gtk_table_attach_defaults(GTK_TABLE(tabt), ent5, 1, 2, 0, 1);
  g_signal_connect(G_OBJECT(ent5), "value-changed", G_CALLBACK(step_value), (gpointer)&sbs);

  lbl6 = gtk_label_new("Horizon length (0 = none) ");
  gtk_misc_set_alignment(GTK_MISC(lbl6), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabt), lbl6, 0, 1, 1, 2);
  ent6 = gtk_spin_button_new_with_range(0, MAX_RUN_LEN, 1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(ent6), sbs.cfg.max_time);
  gtk_table_attach_defaults(GTK_TABLE(tabt), ent6, 1, 2, 1, 2);
  g_signal_connect(G_OBJECT(ent6), "value-changed", G_CALLBACK(max_time_value), (gpointer)&sbs);

  gtk_container_add(GTK_CONTAINER(vbox3), tabt);
  gtk_container_add(GTK_CONTAINER(fr3), vbox3);
  gtk_box_pack_end(GTK_BOX(hboxctl1), fr3, TRUE, TRUE, 0);

  fr2 = gtk_frame_new("Inference");
  gtk_label_set_markup(GTK_LABEL(gtk_frame_get_label_widget(GTK_FRAME(fr2))), "<b>Inference</b>");
  vbox2 = gtk_vbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(fr2), 5);
  gtk_container_set_border_width(GTK_CONTAINER(vbox2), 5);

  tabi = gtk_table_new(1, 3, FALSE);

  cb2 = gtk_check_button_new_with_label("Verify inference soundness");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb2), sbs.cfg.soundness_check);
  gtk_table_attach_defaults(GTK_TABLE(tabi), cb2, 0, 1, 0, 1);
  g_signal_connect(G_OBJECT(cb2), "clicked", G_CALLBACK(soundness_check_box), (gpointer)&sbs);

  cb3 = gtk_check_button_new_with_label("Causal inference only");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb3), sbs.cfg.strictly_causal);
  gtk_table_attach_defaults(GTK_TABLE(tabi), cb3, 0, 1, 1, 2);
  g_signal_connect(G_OBJECT(cb3), "clicked", G_CALLBACK(strictly_causal_box), (gpointer)&sbs);

  cb10 = gtk_check_button_new_with_label("Use symbol table");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb10), sbs.cfg.use_xref);
  gtk_table_attach_defaults(GTK_TABLE(tabi), cb10, 0, 1, 2, 3);
  g_signal_connect(G_OBJECT(cb10), "clicked", G_CALLBACK(use_xref_box), (gpointer)&sbs);

  gtk_container_add(GTK_CONTAINER(vbox2), tabi);
  gtk_container_add(GTK_CONTAINER(fr2), vbox2);
  gtk_box_pack_start(GTK_BOX(hboxctl2), fr2, TRUE, TRUE, 0);

  fr4 = gtk_frame_new("Input and output");
  gtk_label_set_markup(GTK_LABEL(gtk_frame_get_label_widget(GTK_FRAME(fr4))), "<b>Input and output</b>");
  vbox4 = gtk_vbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(fr4), 5);
  gtk_container_set_border_width(GTK_CONTAINER(vbox4), 5);

  tabh = gtk_table_new(2, 3, FALSE);

  cb0 = gtk_radio_button_new_with_label(NULL, "IPC input and output");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb0), !sbs.cfg.quiet && !sbs.cfg.file_io);
  gtk_table_attach_defaults(GTK_TABLE(tabh), cb0, 0, 1, 0, 1);

  cb7 = gtk_radio_button_new_with_label(gtk_radio_button_get_group(GTK_RADIO_BUTTON(cb0)), "File input and output");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb7), sbs.cfg.file_io);
  gtk_table_attach_defaults(GTK_TABLE(tabh), cb7, 0, 1, 1, 2);
  g_signal_connect(G_OBJECT(cb7), "clicked", G_CALLBACK(file_io_box), (gpointer)&sbs);

  cb6 = gtk_radio_button_new_with_label(gtk_radio_button_get_group(GTK_RADIO_BUTTON(cb7)), "Quiet mode");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb6), sbs.cfg.quiet);
  gtk_table_attach_defaults(GTK_TABLE(tabh), cb6, 0, 1, 2, 3);
  g_signal_connect(G_OBJECT(cb6), "clicked", G_CALLBACK(quiet_box), (gpointer)&sbs);

  cb4 = gtk_check_button_new_with_label("Trace inference");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb4), sbs.cfg.echo_stdout);
  gtk_table_attach_defaults(GTK_TABLE(tabh), cb4, 1, 2, 0, 1);
  g_signal_connect(G_OBJECT(cb4), "clicked", G_CALLBACK(echo_stdout_box), (gpointer)&sbs);

  cb5 = gtk_check_button_new_with_label("Log inference to file");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb5), sbs.cfg.echo_logfile);
  gtk_table_attach_defaults(GTK_TABLE(tabh), cb5, 1, 2, 1, 2);
  g_signal_connect(G_OBJECT(cb5), "clicked", G_CALLBACK(echo_logfile_box), (gpointer)&sbs);

  cb14 = gtk_check_button_new_with_label("Display auxiliary signals");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb14), sbs.cfg.outaux);
  gtk_table_attach_defaults(GTK_TABLE(tabh), cb14, 1, 2, 2, 3);
  g_signal_connect(G_OBJECT(cb14), "clicked", G_CALLBACK(outaux_box), (gpointer)&sbs);

  gtk_container_add(GTK_CONTAINER(vbox4), tabh);
  gtk_container_add(GTK_CONTAINER(fr4), vbox4);
  gtk_box_pack_start(GTK_BOX(hboxctl2), fr4, TRUE, TRUE, 0);

  fr6 = gtk_frame_new("Execution");
  gtk_label_set_markup(GTK_LABEL(gtk_frame_get_label_widget(GTK_FRAME(fr6))), "<b>Execution</b>");
  vbox6 = gtk_vbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(fr6), 5);
  gtk_container_set_border_width(GTK_CONTAINER(vbox6), 5);

  tabx = gtk_table_new(2, 3, FALSE);

  lbl8 = gtk_label_new("Input truth probability ");
  gtk_misc_set_alignment(GTK_MISC(lbl8), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabx), lbl8, 0, 1, 0, 1);
  ent8 = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 1, 0.01);
  gtk_scale_set_value_pos(GTK_SCALE(ent8), GTK_POS_LEFT);
  gtk_scale_add_mark(GTK_SCALE(ent8), 0.5,  GTK_POS_TOP, NULL);
  gtk_scale_add_mark(GTK_SCALE(ent8), 0.5,  GTK_POS_BOTTOM, NULL);
  gtk_range_set_value(GTK_RANGE(ent8), sbs.cfg.prob);
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
  gtk_range_set_value(GTK_RANGE(ent9), sbs.cfg.correction);
  gtk_table_attach_defaults(GTK_TABLE(tabx), ent9, 1, 2, 1, 2);
  g_object_set(G_OBJECT(ent9), "width-request", BAR_WIDTH, NULL);
  g_signal_connect(G_OBJECT(ent9), "value-changed", G_CALLBACK(correction_value), (gpointer)&sbs);

  lbl0 = gtk_label_new("Elapsed time ");
  gtk_misc_set_alignment(GTK_MISC(lbl0), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabx), lbl0, 0, 1, 2, 3);
  lblt = gtk_label_new("0 (0.000000 s) ~ 0.000000 s");
  gtk_misc_set_alignment(GTK_MISC(lblt), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabx), lblt, 1, 2, 2, 3);

  sbs.timer = GTK_LABEL(lblt);

  gtk_container_add(GTK_CONTAINER(vbox6), tabx);
  gtk_container_add(GTK_CONTAINER(fr6), vbox6);
  gtk_box_pack_end(GTK_BOX(hboxctl2), fr6, TRUE, TRUE, 0);

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

  btn4 = gtk_button_new_with_label("Configure...");
  gtk_widget_set_size_request(btn4, 70, 30);
  gtk_box_pack_end(GTK_BOX(hbox5), btn4, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(btn4), "clicked", G_CALLBACK(configure), (gpointer)&sbs);

  btn7 = gtk_button_new_with_label("Help...");
  gtk_widget_set_size_request(btn7, 70, 30);
  gtk_box_pack_end(GTK_BOX(hbox5), btn7, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(btn7), "clicked", G_CALLBACK(help_button_clicked), (gpointer)&sbs);

  sbs.help_button = GTK_BUTTON(btn7);

  if(*sbs.cfg.viewer_name)
    gtk_widget_set_sensitive(btn7, TRUE);
  else
    gtk_widget_set_sensitive(btn7, FALSE);

  gtk_box_pack_end(GTK_BOX(vbox5), hbox5, FALSE, FALSE, 0);

  btn5 = gtk_button_new();
  g_signal_connect(G_OBJECT(btn5), "clicked", G_CALLBACK(dummy_button_clicked), (gpointer)&sbs);

  sbs.dummy_button = GTK_BUTTON(btn5);

  gtk_box_pack_start(GTK_BOX(extbox), frgfx, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(extbox), frtxt, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(extbox), hboxctl1, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(extbox), hboxctl2, FALSE, FALSE, 0);
  gtk_box_pack_end(GTK_BOX(extbox), vbox5, FALSE, FALSE, 0);

  gtk_box_pack_end(GTK_BOX(winbox), extbox, TRUE, TRUE, 0);

  gtk_container_add(GTK_CONTAINER(window), winbox);

  gtk_widget_show_all(window);

  pthread_mutex_init(&sbs.mutex_xbuffer, NULL);
  pthread_mutex_init(&sbs.mutex_button, NULL);
  pthread_mutex_init(&sbs.mutex_sent, NULL);

  gtk_main();

  pthread_mutex_destroy(&sbs.mutex_xbuffer);
  pthread_mutex_destroy(&sbs.mutex_button);
  pthread_mutex_destroy(&sbs.mutex_sent);

  return 0;
}

int main(int argc, char *argv[])
{
  char *source_name, *base_name, *state_name, *logfile_name, *xref_name, *option, *ext, *prefix, *path, *include_path;
  char default_state_name[MAX_STRLEN], default_logfile_name[MAX_STRLEN], default_xref_name[MAX_STRLEN], alpha[SYMBOL_NUMBER + 1];
  bool strictly_causal, soundness_check, echo_stdout, file_io, quiet, hard, sys5, sturdy, busywait, seplit_fe, seplit_su, merge, outaux, outint, batch_in, batch_out, draw_undef;
  int i, k, n;
  d_time max_time;
  m_time step, default_step;
  int bufexp;
  int num_threads;
  double prob;

  source_name = base_name = state_name = logfile_name = xref_name = NULL;
  prefix = MAGIC_PREFIX;
  path = "";
  include_path = "";
  strcpy(alpha, IO_SYMBOLS);
  strictly_causal = soundness_check = echo_stdout = file_io = quiet = hard = sys5 = sturdy = busywait = seplit_fe = seplit_su = merge = outaux = outint = batch_in = batch_out = draw_undef = FALSE;
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
              fprintf(stderr,
              "Usage: %s [-AbBcdfilqsSuvVwWxyYZ] [-a alphabet] [-e prefix] [-I state] [-L log] [-n processes] [-o base] [-p path] [-P path] [-Q probability] [-r core] [-t step] [-X symbols] [-z horizon] [source]\n",
                      argv[0]);
              exit(EXIT_SUCCESS);
            break;

            case 'a':
              if(*(++option))
                {
                  fprintf(stderr, "%s, %c: Invalid command line option"
                                  " (%s -h for help)\n",
                          argv[i], *option, argv[0]);
                  exit(EXIT_FAILURE);
                }

              if(++i < argc)
                {
                  n = strlen(argv[i]);
                  if(n >= SYMBOL_NUMBER)
                    {
                      fprintf(stderr, "%s: Argument too long\n", argv[i]);
                      exit(EXIT_FAILURE);
                    }

                  for(k = 0; k < n; k++)
                    alpha[k] = argv[i][k];
                } 
              else
                {
                  fprintf(stderr, "%s: Missing argument\n", argv[--i]);
                  exit(EXIT_FAILURE);
                }
            break;

            case 'e':
              if(*(++option))
                {
                  fprintf(stderr, "%s, %c: Invalid command line option"
                                  " (%s -h for help)\n",
                          argv[i], *option, argv[0]);
                  exit(EXIT_FAILURE);
                }

              if(++i < argc)
                {
                  prefix = argv[i];
                  if(prefix[0] != '/')
                    {
                      fprintf(stderr, "%s: Invalid argument\n", prefix);
                      exit(EXIT_FAILURE);
                    }
                } 
              else
                {
                  fprintf(stderr, "%s: Missing argument\n", argv[--i]);
                  exit(EXIT_FAILURE);
                }
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
                path = argv[i]; 
              else
                {
                  fprintf(stderr, "%s: Missing argument\n", argv[--i]);
                  exit(EXIT_FAILURE);
                }
            break;

            case 'P':
              if(*(++option))
                {
                  fprintf(stderr, "%s, %c: Invalid command line option"
                                  " (%s -h for help)\n",
                          argv[i], *option, argv[0]);
                  exit(EXIT_FAILURE);
                }

              if(++i < argc)
                include_path = argv[i]; 
              else
                {
                  fprintf(stderr, "%s: Missing argument\n", argv[--i]);
                  exit(EXIT_FAILURE);
                }
            break;

            case 'Q':
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
                    case 'A':
                      draw_undef = TRUE;
                    break;

                    case 'b':
                      outaux = TRUE;
                    break;

                    case 'B':
                      outint = TRUE;
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

                    case 'S':
                      busywait = TRUE;
                    break;

                    case 'u':
                      merge = TRUE;
                    break;

                    case 'v':
                      soundness_check = TRUE;
                    break;

                    case 'V':
                      sys5 = TRUE;
                    break;

                    case 'w':
                      seplit_fe = TRUE;
                    break;

                    case 'W':
                      seplit_su = TRUE;
                    break;

                    case 'x':
                      if(!xref_name)
                        xref_name = default_xref_name;
                    break;

                    case 'y':
                      sturdy = TRUE;
                    break;

                    case 'Y':
                      batch_in = TRUE;
                    break;

                    case 'Z':
                      batch_out = TRUE;
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
         strictly_causal, soundness_check, echo_stdout, file_io, quiet, hard, sys5, sturdy, busywait, seplit_fe, seplit_su, merge, outaux, outint,
         bufexp, max_time, step, prefix, path, include_path, alpha, num_threads, prob, batch_in, batch_out, draw_undef);
}


