
/*
  GTINXSH - Temporal Inference Network eXecutor Suite
  Design & coding by Andrea Giotti, 1998-1999
  Revised 2016-2025
*/

#define NDEBUG
/* #define BUGGED_PTHREADS */

#define ANSI_FILE_IO
/* #define UNIX_FILE_IO */

#include "gtinxsh.h"

#define PACK_VER "13.0.2"
#define VER "7.0.2"

const char *color_name_light[NUM_COLORS] = { "green4", "yellow4", "orange4", "red4", "purple4" };
const char *color_name_dark[NUM_COLORS] = { "green1", "yellow1", "orange1", "red1", "purple1" };

INLINE m_time get_time()
{
  struct timespec ts;

  clock_gettime(CLOCK_TYPE, &ts);

  return ts.tv_sec + 0.000000001 * ts.tv_nsec;
}

#if defined BUGGED_PTHREADS

/*
void join_pipe(s_base *sb)
{
  lock_pipe(sb);

  sb->done = TRUE;

  do
    wait_pipe(sb);
  while(sb->done);

  unlock_pipe(sb);
}
*/

void join_pipe(s_base *sb)
{
  lock_pipe(sb);

  sb->done = TRUE;

  do
    {
      unlock_pipe(sb);
      usleep(DELAY);
      lock_pipe(sb);
    }
  while(sb->done);

  unlock_pipe(sb);
}

void leave_pipe(s_base *sb)
{
  lock_pipe(sb);

  sb->done = FALSE;
/*  signal_pipe(sb); */

  unlock_pipe(sb);
}

#endif

void init_dft(int num, real re[MAX_DFT][MAX_DFT], real im[MAX_DFT][MAX_DFT])
{
  int h, k;

  for(h = 0; h < num; h++)
    for(k = 0; k < num; k++)
      {
        re[h][k] = cos(2 * M_PI * h * k / num);
        im[h][k] = - sin(2 * M_PI * h * k / num);
      }
}

void calc_dft(int num, real *source, real *dest, real re[MAX_DFT][MAX_DFT], real im[MAX_DFT][MAX_DFT])
{
  real reacc, imacc;
  int h, k;

  for(h = 0; h < num; h++)
    {
      reacc = 0;
      imacc = 0;
      
      for(k = 0; k < num; k++)
        {
          reacc += re[h][k] * source[k];
          imacc += im[h][k] * source[k];
        }

      dest[h] = sqrt(reacc * reacc + imacc * imacc);
    }
}

void calc_dft_byte(int num, char *source, real *dest, real re[MAX_DFT][MAX_DFT], real im[MAX_DFT][MAX_DFT])
{
  real reacc, imacc;
  int h, k;

  for(h = 0; h < num; h++)
    {
      reacc = 0;
      imacc = 0;
      
      for(k = 0; k < num; k++)
        {
          reacc += re[h][k] * source[k];
          imacc += im[h][k] * source[k];
        }

      dest[h] = sqrt(reacc * reacc + imacc * imacc);
    }
}

gboolean plan_callback(GtkWidget *widget, cairo_t *cr, s_base *sb)
{
  cairo_t *dr;
  cairo_surface_t *surface;
  int width, height;
  double offset, hue1, hue2;
  int i, j, h, k, col;
  real val_x, val_y, maxval_x, maxval_y;
  real x[MAX_PLAN], y[MAX_PLAN];
  d_time t;

  t = sb->t;        /* mt cache */

  width = gtk_widget_get_allocated_width(widget);
  height = gtk_widget_get_allocated_height(widget);

  surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, width, height);
  dr = cairo_create(surface);

  if(sb->gn)
    offset = min(FONT_SIZE * (double)height / sb->gn, MAX_FONT_PIXELS);
  else
    offset = 0;

  cairo_set_source_rgb(dr, 0, 0, 0);
  cairo_rectangle(dr, 0, 0, width, height);
  cairo_fill(dr);

  h = 0;
  for(i = 0; i < sb->cfg.gn; i++)
    if(!sb->cfg.gexcl[i])
      {
        k = 0;
        for(j = 0; j < sb->cfg.gn; j++)
          if(!sb->cfg.gexcl[j])
            {
              if(sb->cfg.pplan[i][j])
                {
                  maxval_x = 0;
                  maxval_y = 0;
                  for(col = 0; col < sb->cp_horizon_size - 1; col++)
                    {
                      x[col] = sb->g[h].realbuf? sb->g[h].realmem[(t + col) % sb->cp_horizon_size] : sb->g[h].memory[(t + col) % sb->cp_horizon_size];
                      y[col] = sb->g[k].realbuf? sb->g[k].realmem[(t + col) % sb->cp_horizon_size] : sb->g[k].memory[(t + col) % sb->cp_horizon_size];

                      if(maxval_x < fabs(x[col]))
                        maxval_x = fabs(x[col]);

                      if(maxval_y < fabs(y[col]))
                        maxval_y = fabs(y[col]);
                    }

                  hue1 = (double)h / (sb->gn - 1);
                  hue2 = (double)k / (sb->gn - 1);

                  if(sb->g[h].aux || sb->g[k].aux)
                    cairo_set_source_rgb(dr, hue1, 1 - hue2, 1);
                  else
                    cairo_set_source_rgb(dr, hue1, 1 - hue2, 0);

                  cairo_new_path(dr);

                  for(col = 0; col < sb->cp_horizon_size - 1; col++)
                    {
                      if(maxval_x)
                        val_x = x[col] / (2 * maxval_x);
                      else
                        val_x = 0;

                      if(maxval_y)
                        val_y = y[col] / (2 * maxval_y);
                      else
                        val_y = 0;

                      if(col + t <= sb->cp_horizon_size)
                        cairo_move_to(dr, offset + (0.5 + val_x) * (width - 2 * offset), offset + (0.5 - val_y) * (height - 2 * offset));
                      else
                        cairo_line_to(dr, offset + (0.5 + val_x) * (width - 2 * offset), offset + (0.5 - val_y) * (height - 2 * offset));
                    }

                  cairo_stroke(dr);
                }
              
              k++;
            }
          
        h++;
      }    

  cairo_destroy(dr);

  cairo_set_source_surface(cr, surface, 0, 0);
  cairo_paint(cr);

  cairo_surface_destroy(surface);

  return FALSE;
}

gboolean spectrum_callback(GtkWidget *widget, cairo_t *cr, s_base *sb)
{
  cairo_t *dr;
  cairo_surface_t *surface;
  int width, height;
  double offset, offset_x1, offset_x2, recth, rectw, fonth, fontw;
  int i, row, col, fn0, fn1, fpos, gn0, gn1, gpos;
  real val, maxval;
  real spectrum[MAX_DFT];

  fpos = sb->specpos;
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

      rectw = (double)(offset_x1 - offset) / (sb->cp_horizon_size - 1);

      cairo_select_font_face(dr, "cairo : sans-serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
      cairo_set_font_size(dr, fonth);
    }
  else
    {
      fn1 = 0;
      gn1 = 0;
      offset = 0;
      recth = 0;
      rectw = 0;
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
      if(sb->f[fpos + i].realbuf)
        calc_dft(sb->cp_horizon_size - 1, sb->f[fpos + i].realmem, spectrum, sb->re, sb->im);
      else
        calc_dft_byte(sb->cp_horizon_size - 1, sb->f[fpos + i].memory, spectrum, sb->re, sb->im);
      
      maxval = 0;
      for(col = 0; col < sb->cp_horizon_size - 1; col++)
        if(maxval < fabs(spectrum[col]))
          maxval = fabs(spectrum[col]);

      if(!sb->f[fpos + i].packed)
        cairo_set_source_rgb(dr, 1, 0, 0);
      else
        cairo_set_source_rgb(dr, 1, 0.5, 0);

      cairo_new_path(dr);

      for(col = 1; col < 2 * (sb->cp_horizon_size - 1); col++)
         {
          if(maxval)
            val = GRAPH_SCALE * spectrum[abs(col - (sb->cp_horizon_size - 1))] / (2 * maxval);
          else
            val = 0;

           cairo_line_to(dr, offset + (col / 2.0 + 0.5) * rectw, offset + (row + 0.5 - val) * recth);
         }

      cairo_stroke(dr);

      cairo_move_to(dr, offset_x2, offset + (row + 0.5) * recth + fonth / 4);
      cairo_show_text(dr, sb->cp_full_names? sb->f[fpos + i].name_full : sb->f[fpos + i].name);

      row++;
    }

  for(i = 0; i < gn1; i++)
    {
      if(sb->g[gpos + i].realbuf)
        calc_dft(sb->cp_horizon_size - 1, sb->g[gpos + i].realmem, spectrum, sb->re, sb->im);
      else
        calc_dft_byte(sb->cp_horizon_size - 1, sb->g[gpos + i].memory, spectrum, sb->re, sb->im);

      maxval = 0;
      for(col = 0; col < sb->cp_horizon_size - 1; col++)
        if(maxval < fabs(spectrum[col]))
          maxval = fabs(spectrum[col]);

      if(sb->g[gpos + i].aux)
        cairo_set_source_rgb(dr, 0, 0.75, 0.75);
      else
        if(!sb->g[gpos + i].packed)
          cairo_set_source_rgb(dr, 0, 1, 0);
        else
          cairo_set_source_rgb(dr, 1, 1, 0);

      cairo_new_path(dr);

      for(col = 1; col < 2 * (sb->cp_horizon_size - 1); col++)
         {
          if(maxval)
            val = GRAPH_SCALE * spectrum[abs(col - (sb->cp_horizon_size - 1))] / (2 * maxval);
          else
            val = 0;

           cairo_line_to(dr, offset + (col / 2.0 + 0.5) * rectw, offset + (row + 0.5 - val) * recth);
         }

      cairo_stroke(dr);

      cairo_move_to(dr, offset_x2, offset + (row + 0.5) * recth + fonth / 4);
      cairo_show_text(dr, sb->cp_full_names? sb->g[gpos + i].name_full : sb->g[gpos + i].name);

      row++;
    }

  cairo_destroy(dr);

  cairo_set_source_surface(cr, surface, 0, 0);
  cairo_paint(cr);

  cairo_surface_destroy(surface);

  return FALSE;
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
         case DISPLAY_HI_CODE:
           cairo_rectangle(cr, offset_x + (x + BORDER_TRUE) * rectw, offset_y + (y + BORDER_TRUE) * recth, (1 - 2 * BORDER_TRUE) * rectw, (1 - 2 * BORDER_TRUE) * recth);
         break;

         case DISPLAY_LO_CODE:
           cairo_rectangle(cr, offset_x + (x + BORDER_FALSE) * rectw, offset_y + (y + BORDER_FALSE) * recth, (1 - 2 * BORDER_FALSE) * rectw, (1 - 2 * BORDER_FALSE) * recth);
         break;

         case DISPLAY_UNKNOWN_CODE:
           if(sb->cfg.draw_undef)
             cairo_rectangle(cr, offset_x + (x + 0.5) * rectw - 0.5, offset_y + (y + 0.5) * recth - 0.5, 1, 1);
         break;
         
         default:
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

void plotgraph(cairo_t *cr, s_base *sb, int y, double offset_x, double offset_y, double width, double height, real *realmem, int sync)
{
  double rectw, recth;
  int x;
  d_time t;
  real val, maxval;

  t = sb->t;        /* mt cache */

  recth = (double)height / min(sb->fn + sb->gn, sb->cp_display_rows);
  rectw = (double)width / (sb->cp_horizon_size - 1);

   maxval = 0;
   for(x = 0; x < sb->cp_horizon_size - 1; x++)
     if(maxval < fabs(realmem[x]))
       maxval = fabs(realmem[x]);

   cairo_new_path(cr);

   for(x = 0; x < sb->cp_horizon_size - 1; x++)
     {
       if(maxval)
         val = GRAPH_SCALE * realmem[(t + x) % sb->cp_horizon_size] / (2 * maxval);
       else
         val = 0;

       if(sync <= t + x || t + x <= sb->cp_horizon_size)
         cairo_move_to(cr, offset_x + (x + 0.5) * rectw, offset_y + (y + 0.5 - val) * recth);
       else
         cairo_line_to(cr, offset_x + (x + 0.5) * rectw, offset_y + (y + 0.5 - val) * recth);
     }

  cairo_stroke(cr);
}

gboolean draw_callback(GtkWidget *widget, cairo_t *cr, s_base *sb)
{
  cairo_t *dr;
  cairo_surface_t *surface;
  int i, row, col, fn0, gn0, fn1, gn1, fpos, gpos;
  int width, height;
  double offset, offset_x1, offset_x2, recth, fonth, fontw;
  d_time t;
  char c;
  bool empty;

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
      if(!sb->f[fpos + i].packed)
        cairo_set_source_rgb(dr, 1, 0, 0);
      else
        cairo_set_source_rgb(dr, 1, 0.5, 0);

      if(!sb->f[fpos + i].realbuf)
        {
          for(col = 0; col < sb->cp_horizon_size - 1; col++)
            plot(dr, sb, col, row, offset, offset, offset_x1 - offset, height - 2 * offset, sb->f[fpos + i].memory[(t + col) % sb->cp_horizon_size], sb->f[fpos + i].packed);
        }
      else
        plotgraph(dr, sb, row, offset, offset, offset_x1 - offset, height - 2 * offset, sb->f[fpos + i].realmem, INT_MAX);

      cairo_move_to(dr, offset_x2, offset + (row + 0.5) * recth + fonth / 4);
      cairo_show_text(dr, sb->cp_full_names? sb->f[fpos + i].name_full : sb->f[fpos + i].name);

      row++;
    }

  for(i = 0; i < gn1; i++)
    {
      if(sb->g[gpos + i].aux)
        cairo_set_source_rgb(dr, 0, 0.75, 0.75);
      else
        if(!sb->g[gpos + i].packed)
          cairo_set_source_rgb(dr, 0, 1, 0);
        else
          cairo_set_source_rgb(dr, 1, 1, 0);

      if(!sb->g[gpos + i].realbuf)
        {
          for(col = 0; col < sb->cp_horizon_size - 1; col++)
            plot(dr, sb, col, row, offset, offset, offset_x1 - offset, height - 2 * offset, sb->g[gpos + i].memory[(t + col) % sb->cp_horizon_size], sb->g[gpos + i].packed);
        }
      else
        plotgraph(dr, sb, row, offset, offset, offset_x1 - offset, height - 2 * offset, sb->g[gpos + i].realmem, sb->g[gpos + i].sync);

      cairo_move_to(dr, offset_x2, offset + (row + 0.5) * recth + fonth / 4);
      cairo_show_text(dr, sb->cp_full_names? sb->g[gpos + i].name_full : sb->g[gpos + i].name);

      c = sb->g[gpos + i].memory[(t + (sb->cp_horizon_size - 2)) % sb->cp_horizon_size];
      empty = !sb->g[gpos + i].realbuf && !sb->g[gpos + i].packed && c == DISPLAY_UNKNOWN_CODE;

      if(sb->g[gpos + i].sync != INT_MAX || empty)
        {
          cairo_set_source_rgb(dr, 1, 1, 0);

          cairo_arc(dr, (offset_x1 + offset_x2) / 2, offset + (row + 0.5) * recth, BALL_RATIO * fonth / 2, 0, 2 * G_PI);
          cairo_fill(dr);

          if(empty)
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
  if(sb->rs != stopped && sb->rs != frozen)
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
  d_time t;
  m_time time;

  gtk_widget_queue_draw(GTK_WIDGET(sb->drawingarea));

  if(sb->spectrum_window)
    gtk_widget_queue_draw(GTK_WIDGET(sb->spectrum_window));

  if(sb->plan_window)
    gtk_widget_queue_draw(GTK_WIDGET(sb->plan_window));

  t = sb->t;        /* mt cache */
  time = sb->time;

  sprintf(timerstring, TIME_FMT" ("REAL_FMT_IF" s) %s "REAL_FMT_IF" s", t, t * sb->cp_step, ((t + 1) * sb->cp_step) < time? "<" : (((t - 1) * sb->cp_step > time)? ">" : "~"), time);
  gtk_label_set_label(sb->timer, timerstring);
}

gint flexible_dialog(s_base *sp, char *one, char *two, char *format, ...)
{
  char buffer[MAX_STRLEN_IF];
  va_list arglist;
  GtkWidget *dialog;
  GtkWidget *label;
  GtkWidget *area;
  gint rv;

  va_start(arglist, format);
  vsprintf(buffer, format, arglist);
  va_end(arglist);

  if(two)
    dialog = gtk_dialog_new_with_buttons("TINX Request", GTK_WINDOW(sp->window), GTK_DIALOG_MODAL, two, GTK_RESPONSE_REJECT, one, GTK_RESPONSE_ACCEPT, NULL);
  else
    dialog = gtk_dialog_new_with_buttons("TINX Request", GTK_WINDOW(sp->window), GTK_DIALOG_MODAL, one, GTK_RESPONSE_ACCEPT, NULL);

  gtk_window_set_default_size(GTK_WINDOW(dialog), POPUP_WINDOW_WIDTH, POPUP_WINDOW_HEIGHT);

  area = gtk_dialog_get_action_area(GTK_DIALOG(dialog));
  gtk_container_set_border_width(GTK_CONTAINER(area), 10);
  gtk_widget_set_halign(area, GTK_ALIGN_CENTER);
	
  area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
  gtk_container_set_border_width(GTK_CONTAINER(area), 10);
  gtk_widget_set_halign(area, GTK_ALIGN_CENTER);
  gtk_widget_set_valign(area, GTK_ALIGN_CENTER);

  label = gtk_label_new(NULL);
  gtk_label_set_markup(GTK_LABEL(label), buffer);

  gtk_container_add(GTK_CONTAINER(area), label);

  gtk_widget_show_all(dialog);

  rv = gtk_dialog_run(GTK_DIALOG(dialog));

  gtk_widget_destroy(dialog);
  
  return rv;
}

void print_error(s_base *sb, bool dialog, char *format, ...)
{
  char buffer[MAX_STRLEN_IF], buffer2[MAX_STRLEN_IF];
  va_list arglist;

  va_start(arglist, format);
  vsprintf(buffer, format, arglist);
  va_end(arglist);

  if(errno)
    sprintf(buffer2, "%s: %s", buffer, strerror(errno));
  else
    strcpy(buffer2, buffer);

  fancyprint_add(sb, "\n<span foreground='%s'>%s</span>\n", sb->fancymsg > 1 ? color_name_dark[3] : color_name_light[3], buffer2);

  if(dialog)
    flexible_dialog(sb, "Ok", NULL, "\n%s\n", buffer2);
}

void print_message(s_base *sb, bool dialog, char *format, ...)
{
  char buffer[MAX_STRLEN_IF];
  va_list arglist;

  va_start(arglist, format);
  vsprintf(buffer, format, arglist);
  va_end(arglist);

  fancyprint_add(sb, "\n<span foreground='%s'>%s</span>\n", sb->fancymsg > 1 ? color_name_dark[0] : color_name_light[0], buffer);

  if(dialog)
    flexible_dialog(sb, "Ok", NULL, "\n%s\n", buffer);
}

bool runsystem(s_base *sb, char *cmd)
{
  int rv;

  if(!*cmd)
    return FALSE;

  rv = system(cmd);

  if(rv)
    {
       print_error(sb, TRUE, "Error launching the program %s", cmd);
       return TRUE;
     }
  else	
    return FALSE;
}

void tintloop(s_base *sb)
{
  int sync_mark[MAX_FILES][MAX_HORIZON_SIZE];
  d_time tau_f[MAX_FILES], tau_g[MAX_FILES];
  char inbuffer[MAX_FILES][MAX_STRLEN], outbuffer[MAX_STRLEN], blanks[MAX_STRLEN];
  real value;
  int i, pos[MAX_FILES];

  for(i = 0; i < sb->fn; i++)
    tau_f[i] = 0;

  for(i = 0; i < sb->gn; i++)
    {
      tau_g[i] = 0;
      pos[i] = 0;
    }

  sb->t = 0;

  while(sb->rs != stopping && (!sb->cp_max_time || sb->t < sb->cp_max_time))
    if(sb->rs == frozen)
      usleep(DELAY);
    else
      {
        sb->time = get_time() - sb->time_base;
        if(sb->time >= (sb->t + 1) * sb->cp_step * (1 + sb->cfg.correction))
          {
            sb->t++;

            for(i = 0; i < sb->gn; i++)
              {
                sb->g[i].memory[(sb->t + (sb->cp_horizon_size - 1)) % sb->cp_horizon_size] = DISPLAY_UNKNOWN_CODE;
                sync_mark[i][(sb->t + (sb->cp_horizon_size - 1)) % sb->cp_horizon_size] = sb->t;
              }
          }

        if(!sb->cp_batch_in)
          {
            for(i = 0; i < sb->fn; i++)
              {
                if(tau_f[i] < sb->t)
                  {
                    if(!sb->f[i].realbuf)
                      {
                        outbuffer[0] = sb->cfg.alpha[rand() <= sb->cfg.prob * sb->cfg.inprob[i] * RAND_MAX? true_symbol : false_symbol];
                        outbuffer[1] = '\0';
                      }
                    else
                      {
                        value = (2 * ((real)rand() / RAND_MAX) - 1) * sb->cfg.prob * sb->cfg.inprob[i];
                        sprintf(outbuffer, REAL_OUT_FMT"\n", value);
                      }

                    if(sb->f[i].file_io)
                      {
                        if(mput_file(sb->f[i].fp, outbuffer, strlen(outbuffer)))
                          {
                            print_error(sb, FALSE, sb->f[i].name_full);

                            sb->waitio = FALSE;
                            request_runstate(sb, stopped);

                            pthread_exit(NULL);
                          }

                        if(sync_file(sb->f[i].fp))
                          {
                            print_error(sb, FALSE, sb->f[i].name_full);

                            sb->waitio = FALSE;
                            request_runstate(sb, stopped);

                            pthread_exit(NULL);
                          }
                      }
                    else
                      if(sb->cp_sys5? msend_message_sys5(sb->f[i].cp5, outbuffer, strlen(outbuffer)) : msend_message_posix(sb->f[i].cp, outbuffer, strlen(outbuffer)))
                        {
                          if(errno != EAGAIN)
                            {
                              print_error(sb, FALSE, sb->f[i].name_full);

                              sb->waitio = FALSE;
                              request_runstate(sb, stopped);

                              pthread_exit(NULL);
                            }

                          outbuffer[0] = '\0';
                        }

                    if(outbuffer[0] != '\0')
                      {
                        if(!sb->f[i].realbuf)
                          {
                            if(!sb->f[i].packed)
                              {
                                if(outbuffer[0] == sb->cfg.alpha[false_symbol])
                                  sb->f[i].memory[tau_f[i] % sb->cp_horizon_size] = DISPLAY_LO_CODE;
                                else
                                  sb->f[i].memory[tau_f[i] % sb->cp_horizon_size] = DISPLAY_HI_CODE;
                              }
                            else
                              sb->f[i].memory[tau_f[i] % sb->cp_horizon_size] = outbuffer[0];
                          }
                        else
                          sb->f[i].realmem[tau_f[i] % sb->cp_horizon_size] = value;

                        tau_f[i]++;
                      }
                  }
              }
          }

        if(!sb->cp_batch_out)
          {
            for(i = 0; i < sb->gn; i++)
              {
                if(tau_g[i] < sb->t && !sb->g[i].excl)
                  {
                    inbuffer[i][pos[i]] = '\0';

                    if(sb->g[i].file_io)
                      {
                        if(!sb->g[i].realbuf)
                          {
                            if(get_file(sb->g[i].fp, inbuffer[i]))
                              {
                                if(file_error(sb->g[i].fp))
                                  {
                                    print_error(sb, FALSE, sb->g[i].name_full);

                                    sb->waitio = FALSE;
                                    request_runstate(sb, stopped);

                                    pthread_exit(NULL);
                                  }
                                else
                                  reset_file(sb->g[i].fp);
                              }
                          }
                        else
                          {
                            if(fscanf(sb->g[i].fp, FUN_FMT, &inbuffer[i][pos[i]]) == EOF)
                              {
                                if(file_error(sb->g[i].fp))
                                  {
                                    print_error(sb, FALSE, sb->g[i].name_full);

                                    sb->waitio = FALSE;
                                    request_runstate(sb, stopped);

                                    pthread_exit(NULL);
                                  }
                                else
                                  reset_file(sb->g[i].fp);                               
                              }

                            blanks[0] = '\0';
                            if(fscanf(sb->g[i].fp, "%"MAX_NAMEBUF_C"["BLANKS"]", blanks) == EOF)
                              {
                                if(file_error(sb->g[i].fp))
                                  {
                                    print_error(sb, FALSE, sb->g[i].name_full);

                                    sb->waitio = FALSE;
                                    request_runstate(sb, stopped);

                                    pthread_exit(NULL);
                                  }
                                else
                                  reset_file(sb->g[i].fp);                               
                              }

                            if(blanks[0] == '\0')
                              pos[i] = strlen(inbuffer[i]);
                            else
                              pos[i] = 0;
                          }
                      }
                    else
                      {
                        if(sb->cp_sys5? mread_message_sys5(sb->g[i].cp5, inbuffer[i], MSG_SIZE) && errno != ENOMSG : mread_message_posix(sb->g[i].cp, inbuffer[i], MSG_SIZE) && errno != EAGAIN)
                          {
                            print_error(sb, FALSE, sb->g[i].name_full);

                            sb->waitio = FALSE;
                            request_runstate(sb, stopped);

                            pthread_exit(NULL);
                          }
                      }

                    if(inbuffer[i][pos[i]] == EOF)
                      inbuffer[i][pos[i]] = '\0';

                    if(inbuffer[i][0] != '\0' && !pos[i])
                      {
                        if(!sb->g[i].realbuf)
                          {
                            if(!sb->g[i].packed)
                              {
                                switch(strchr(sb->cfg.alpha, inbuffer[i][0]) - sb->cfg.alpha)
                                  {
                                    case unknown_symbol:
                                      sb->g[i].memory[tau_g[i] % sb->cp_horizon_size] = DISPLAY_UNKNOWN_CODE;
                                    break;

                                    case false_symbol:
                                      sb->g[i].memory[tau_g[i] % sb->cp_horizon_size] = DISPLAY_LO_CODE;
                                    break;

                                    case true_symbol:
                                      sb->g[i].memory[tau_g[i] % sb->cp_horizon_size] = DISPLAY_HI_CODE;
                                    break;

                                    case end_symbol:
                                      sb->g[i].memory[tau_g[i] % sb->cp_horizon_size] = DISPLAY_UNKNOWN_CODE;
                                      sb->g[i].excl = TRUE;
                                    break;

                                    default:
                                      print_error(sb, FALSE, "%s, %c (dec %d): Invalid character in stream", sb->g[i].name_full, inbuffer[i][0], inbuffer[i][0]);

                                      sb->waitio = FALSE;
                                      request_runstate(sb, stopped);

                                      pthread_exit(NULL);
                                    break;
                                  }
                              }
                            else
                              sb->g[i].memory[tau_g[i] % sb->cp_horizon_size] = inbuffer[i][0];

                            sync_mark[i][tau_g[i] % sb->cp_horizon_size] = INT_MAX;
                            tau_g[i]++;
                          }
                        else
                          {
                            if(inbuffer[i][0] == sb->cfg.alpha[end_symbol])
                              sb->g[i].excl = TRUE;
                            else
                              {
                                if(sscanf(inbuffer[i], REAL_IN_FMT, &value) != 1)
                                  {
                                    print_error(sb, FALSE, "%s, %s: Invalid number in stream", sb->g[i].name_full, inbuffer[i]);

                                    sb->waitio = FALSE;
                                    request_runstate(sb, stopped);

                                    pthread_exit(NULL);
                                  }

                                sb->g[i].realmem[tau_g[i] % sb->cp_horizon_size] = value;

                                sync_mark[i][tau_g[i] % sb->cp_horizon_size] = INT_MAX;
                                tau_g[i]++;
                              }
                          }                        
                      }
                    else
                      if(sb->g[i].omit && sb->time >= (tau_g[i] + 1) * sb->cp_step * (1 + sb->cfg.correction))
                        {
                          sync_mark[i][tau_g[i] % sb->cp_horizon_size] = INT_MAX;
                          tau_g[i]++;
                        }
                  }

                sb->g[i].sync = sync_mark[i][(sb->t + (sb->cp_horizon_size - 2)) % sb->cp_horizon_size];
              }
          }
      }

  sb->waitio = TRUE;
  request_runstate(sb, stopped);

  pthread_exit(NULL);
}

char *filterchar(char c)
{
  static char dh[2];
  char *d;

  switch(c)
    {
      case '&':
        d = "&amp;";
      break;

      case '<':
        d = "&lt;";
      break;

      case '>':
        d = "&gt;";
      break;

      case '\'':
        d = "&#39;";
      break;

      default:
        dh[0] = c;
        dh[1] = '\0';

        d = dh;
      break;
   }

  return d;
}

void tinxpipe(s_base *sb)
{
  FILE *fp;
  char ch[LONG_STRLEN_IF];
  char *d, *e, *f;
  char a;
  bool got;

#if defined BUGGED_PTHREADS
  while(TRUE)
    {
      join_pipe(sb);
#endif

  d = sb->cmd;
  e = ch;

  *e = '\0';

  while(*d)
    {
      f = filterchar(*d);

      strcat(e, f);

      d++;
      e += strlen(f);
    }

  fancyprint(sb, "<b><i>%s</i></b>\n", ch);

  fp = popen(sb->cmd, "r");

  usleep(DELAY);

  if(!fp)
    print_error(sb, FALSE, "Error launching the executor");
  else
    {
      d = ch;

      do
        {
          a = '\0';

          got = fread(&a, sizeof(char), 1, fp);
          if(ferror(fp))
            {
              print_error(sb, FALSE, "Broken pipe");
              break;
            }

          if(got)
            {
              if(a == '%')
                {
                  *d = a;
                  d++;
                }
                
              *d = a;
              d++;
            }

          if(d != ch && (a == '\r' || a == '\n' || feof(fp)))
            {
              *d = '\0';
              print_add(sb, ch);
              d = ch;
            }
        }
      while(!feof(fp));

      usleep(DELAY);

      pclose(fp);
    }

  sb->term = TRUE;
  sb->waitio = FALSE;
  request_runstate(sb, stopped);

#if defined BUGGED_PTHREADS
    }
#endif

  pthread_exit(NULL);
}

gboolean update_gui(s_base *sb)
{
  switch(sb->rs)
    {
      case stopping:
      case stopped:
        if(sb->rsbu != sb->rs)
          {
            sb->rsbu = sb->rs;

            gtk_button_set_label(sb->run_button, "Execute network");
            gtk_menu_item_set_label(sb->run_menu, "Execute network");
            gtk_button_set_label(sb->freeze_button, "Pause execution");
            gtk_menu_item_set_label(sb->freeze_menu, "Pause execution");
            gtk_widget_set_sensitive(GTK_WIDGET(sb->freeze_button), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(sb->freeze_menu), FALSE);
          }

        sb->cp_echo_stdout = FALSE;
        update_view(sb);
      break;

      case starting:
      case started:
        if(sb->rsbu != sb->rs)
          {
            sb->rsbu = sb->rs;

            gtk_button_set_label(sb->run_button, "Stop execution");
            gtk_menu_item_set_label(sb->run_menu, "Stop execution");
            gtk_button_set_label(sb->freeze_button, "Pause execution");
            gtk_menu_item_set_label(sb->freeze_menu, "Pause execution");
            gtk_widget_set_sensitive(GTK_WIDGET(sb->freeze_button), TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(sb->freeze_menu), TRUE);
          }

        sb->cp_echo_stdout = sb->cfg.echo_stdout;
      break;

      case frozen:
        if(sb->rsbu != sb->rs)
          {
            sb->rsbu = sb->rs;

            gtk_button_set_label(sb->run_button, "Stop execution");
            gtk_menu_item_set_label(sb->run_menu, "Stop execution");
            gtk_button_set_label(sb->freeze_button, "Resume execution");
            gtk_menu_item_set_label(sb->freeze_menu, "Resume execution");
            gtk_widget_set_sensitive(GTK_WIDGET(sb->freeze_button), TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(sb->freeze_menu), TRUE);
          }
          
        sb->cp_echo_stdout = FALSE;
        update_view(sb);
      break;

      default:
      break;
    }

  return G_SOURCE_REMOVE;
}

bool switch_freeze(s_base *sb)
{
  pid_t pid;

  pid = pidof(sb, sb->cfg.engine_name[sb->version]);

  if(pid)
    {
      kill(pid, SIGUSR2);
      return TRUE;
    }
  else
    return FALSE;
}

void clean_io(s_base *sb)
{
  int fi, gi;
  bool ipc_io;

  ipc_io = FALSE;

  for(fi = 0; fi < sb->fn; fi++)
    {
      if(!sb->f[fi].file_io)
        {
          ipc_io = TRUE;

          if(!sb->cp_sys5)
            {
              if(!failed_queue_posix(sb->f[fi].cp))
                commit_queue_posix(sb->f[fi].cp);

              remove_queue_posix(sb->f[fi].name_full);
            }
        }
      else
        if(is_file_open(sb->f[fi].fp))
          close_file(sb->f[fi].fp);
    }

  for(gi = 0; gi < sb->gn; gi++)
    {
      if(!sb->g[gi].file_io)
        {
          ipc_io = TRUE;

          if(!sb->cp_sys5)
            {
              if(!failed_queue_posix(sb->g[gi].cp))
                commit_queue_posix(sb->g[gi].cp);

              remove_queue_posix(sb->g[gi].name_full);
            }
        }
      else
        {
          if(is_file_open(sb->g[gi].fp))
            close_file(sb->g[gi].fp);

          clean_file(sb->g[gi].name_full);
        }
    }

  if(ipc_io && sb->cp_sys5)
    delete_queues_sys5();
}

void begin_cfgcache(config *cfgp, cfgcache *cfgccp)
{
  int mark, i;

  for(mark = 0; mark < cfgp->fn; mark++)
    {
      cfgccp->inprob[mark] = cfgp->inprob[mark];
      cfgccp->fexcl[mark] = cfgp->fexcl[mark];
    }

  for(mark = 0; mark < cfgp->gn; mark++)
    {
      cfgccp->gexcl[mark] = cfgp->gexcl[mark];

      for(i = 0; i < cfgp->gn; i++)
        cfgccp->pplan[mark][i] = cfgp->pplan[mark][i];
    }
}

void end_cfgcache(config *cfgp, cfgcache *cfgccp, int cfn, int cgn)
{
  int mark, i;

  if(cfn >= 0)
    {
      cfgp->fn = cfn;
      for(mark = 0; mark < cfn; mark++)
        strcpy(cfgp->fname[mark], cfgccp->fname[mark]);
    }

  if(cgn >= 0)
    {
      cfgp->gn = cgn;
      for(mark = 0; mark < cgn; mark++)
        {
          if(cfgccp->indir[mark] >= 0)
            for(i = 0; i < mark; i++)
              if(cfgccp->indir[i] >= 0)
                {
                  cfgp->pplan[mark][i] = cfgccp->pplan[cfgccp->indir[mark]][cfgccp->indir[i]];
                  cfgp->pplan[i][mark] = cfgccp->pplan[cfgccp->indir[i]][cfgccp->indir[mark]];
                }

          strcpy(cfgp->gname[mark], cfgccp->gname[mark]);
        }
    }
}

void remap_cfgcache(config *cfgp, cfgcache *cfgccp, int cfn, int cgn, char *name)
{
  int mark;

  if(cfn >= 0)
    {
      cfgp->inprob[cfn] = 1;
      cfgp->fexcl[cfn] = FALSE;

      for(mark = 0; mark < cfgp->fn; mark++)
        if(!strcmp(cfgp->fname[mark], name))
          {
            cfgp->inprob[cfn] = cfgccp->inprob[mark];
            cfgp->fexcl[cfn] = cfgccp->fexcl[mark];            
            break;
          }

      strcpy(cfgccp->fname[cfn], name);
    }

  if(cgn >= 0)
    {
      cfgp->gexcl[cgn] = FALSE;
      cfgccp->indir[cgn] = -1;

      for(mark = 0; mark < cfgp->gn; mark++)
        {
          if(!strcmp(cfgp->gname[mark], name))
            {
              cfgp->gexcl[cgn] = cfgccp->gexcl[mark];
              cfgccp->indir[cgn] = mark;
              break;
            }
        }

      strcpy(cfgccp->gname[cgn], name);
    }
}

void runstop(s_base *sb)
{
  FILE *bp;
  char file_name[MAX_STRLEN], name[MAX_STRLEN];
  char cmd[MAX_STRLEN_IF], arg[MAX_STRLEN_IF];
  pthread_attr_t attributes;
  int fn, gn, cfn, cgn, i, k, len, count, tot_rows, page_rows;
  io_type stype;
  int packed, numqueues;
  bool known[MAX_FILES];
  io_type_4 omissions;
  char c, d, ic, oc, str[3];
  pid_t pid;
  m_time halt;
  cfgcache cfgcc;

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

        init_dft(sb->cp_horizon_size - 1, sb->re, sb->im);

        if(!sb->cp_quiet)
          {
            for(i = 0; i < sb->fn; i++)
              {
                sb->f[i].fp = NULL;
                sb->f[i].cp = -1;
                sb->f[i].cp5.paddr = -1;
                sb->f[i].cp5.saddr = -1;
              }

            for(i = 0; i < sb->gn; i++)
              {
                sb->g[i].fp = NULL;
                sb->g[i].cp = -1;
                sb->g[i].cp5.paddr = -1;
                sb->g[i].cp5.saddr = -1;
              }

            strcpy(file_name, sb->cfg.base_name);
            strcat(file_name, NETWORK_EXT);

            bp = fopen(file_name, "r");
            if(!bp)
              {
                print_error(sb, TRUE, file_name);
                sb->rs = stopped;
                sb->error = TRUE;
                g_idle_add((gboolean (*)(gpointer))update_gui, sb);
                return;
              }

            fn = 0;
            gn = 0;

            cfn = 0;
            cgn = 0;

            sb->pos = 0;
            sb->maxlen = 0;

            stype = io_any;
            packed = 0;
            omissions = io_raw;

            for(i = 0; i < MAX_FILES; i++)
              known[i] = FALSE;

            numqueues = 0;

            begin_cfgcache(&sb->cfg, &cfgcc);

            while(fscanf(bp, " "SKIP_FMT" ", name) >= 1);

            if(ferror(bp))
              {
                print_error(sb, TRUE, file_name);
                sb->rs = stopped;
                sb->error = TRUE;
                g_idle_add((gboolean (*)(gpointer))update_gui, sb);
                return;
              }

            while(fscanf(bp, " "FUN_FMT" "FUN_FMT" ( %*[^"SEPARATORS"] , %*[^"SEPARATORS"] ) # %*d / %u , %d , %*d , %*g, %u @ %*ld ", str, name, &stype, &packed, &omissions) >= 2)
              {
                d = '\0';
                sscanf(str, OP_FMT""OP_FMT, &c, &d);

                if(stype != io_socket && (packed <= 0 || !known[packed]))
                  {
                    if(packed > 0)
                      known[packed] = TRUE;

                    switch(c)
                      {
                        case '!':
                          if(!sb->cp_batch_in)
                            {
                              if(cfn >= MAX_FILES)
                                {
                                  print_error(sb, TRUE, "%s: Too many input signals", file_name);
                                  sb->rs = stopped;
                                  sb->error = TRUE;
                                  g_idle_add((gboolean (*)(gpointer))update_gui, sb);
                                  return;
                                }

                              remap_cfgcache(&sb->cfg, &cfgcc, cfn, -1, name);
                              
                              if(!sb->cfg.fexcl[cfn])
                                {
                                  strcpy(sb->f[fn].name, name);

                                  sb->f[fn].file_io = (sb->cp_file_io && stype == io_any) || stype == io_file;

                                  if(sb->f[fn].file_io)
                                    {
                                      if(*sb->cfg.path)
                                        {
                                          strcpy(sb->f[fn].name_full, sb->cfg.path);
                                          strcat(sb->f[fn].name_full, "/");
                                        }
                                      else
                                        *sb->f[fn].name_full = '\0';

                                      strcat(sb->f[fn].name_full, name);
                                      strcat(sb->f[fn].name_full, STREAM_EXT);
                                    }
                                  else
                                    {
                                      strcpy(sb->f[fn].name_full, sb->cfg.prefix);
                                      strcat(sb->f[fn].name_full, name);
                                      numqueues++;
                                    }

                                   if(packed < 0)
                                     {
                                       sb->f[fn].packed = 0;
                                       sb->f[fn].realbuf = TRUE;
                                     }
                                   else
                                     {
                                       sb->f[fn].packed = packed;
                                       sb->f[fn].realbuf = FALSE;
                                     }

                                  len = strlen(sb->cfg.full_names? sb->f[fn].name_full : name);
                                  if(sb->maxlen < len)
                                    sb->maxlen = len;

                                  fn++;
                                }
                                
                              cfn++;
                            }
                        break;

                        case '?':
                        case '.':
                          if(!sb->cp_batch_out)
                            {
                              if(cgn >= MAX_FILES)
                                {
                                  print_error(sb, TRUE, "%s: Too many output signals", file_name);
                                  sb->rs = stopped;
                                  sb->error = TRUE;
                                  g_idle_add((gboolean (*)(gpointer))update_gui, sb);
                                  return;
                                }

                              remap_cfgcache(&sb->cfg, &cfgcc, -1, cgn, name);

                              if(!sb->cfg.gexcl[cgn])
                                {
                                  strcpy(sb->g[gn].name, name);

                                  sb->g[gn].file_io = (sb->cp_file_io && stype == io_any) || stype == io_file;

                                  if(sb->g[gn].file_io)
                                    {
                                      if(*sb->cfg.path)
                                        {
                                          strcpy(sb->g[gn].name_full, sb->cfg.path);
                                          strcat(sb->g[gn].name_full, "/");
                                        }
                                      else
                                        *sb->g[gn].name_full = '\0';

                                      strcat(sb->g[gn].name_full, name);
                                      strcat(sb->g[gn].name_full, STREAM_EXT);
                                    }
                                  else
                                    {
                                      strcpy(sb->g[gn].name_full, sb->cfg.prefix);
                                      strcat(sb->g[gn].name_full, name);
                                      numqueues++;
                                    }

                                   if(packed < 0)
                                     {
                                       sb->g[gn].packed = 0;
                                       sb->g[gn].realbuf = TRUE;
                                     }
                                   else
                                     {
                                       sb->g[gn].packed = packed;
                                       sb->g[gn].realbuf = FALSE;
                                     }

                                  sb->g[gn].omit = (omissions != io_raw);
                                  sb->g[gn].aux = (c == '.');
                                  sb->g[gn].excl = FALSE;

                                  len = strlen(sb->cfg.full_names? sb->g[gn].name_full : name);
                                  if(sb->maxlen < len)
                                    sb->maxlen = len;

                                  gn++;
                                }
                                
                              cgn++;
                            }
                        break;

                        case '_':
                        break;

                        default:
                          print_error(sb, TRUE, "%s, "OP_FMT": Invalid stream class", file_name, c);
                          sb->rs = stopped;
                          sb->error = TRUE;
                          g_idle_add((gboolean (*)(gpointer))update_gui, sb);
                          return;
                        break;
                      }
                  }

                stype = io_any;
                packed = 0;
                omissions = io_raw;
              }

            if(ferror(bp))
              {
                print_error(sb, TRUE, file_name);
                sb->rs = stopped;
                sb->error = TRUE;
                g_idle_add((gboolean (*)(gpointer))update_gui, sb);
                return;
              }

            if(!feof(bp))
              {
                print_error(sb, TRUE, "%s: Parser error", file_name);
                sb->rs = stopped;
                sb->error = TRUE;
                g_idle_add((gboolean (*)(gpointer))update_gui, sb);
                return;
               }

            if(fclose(bp))
              {
                print_error(sb, TRUE, file_name);
                sb->rs = stopped;
                sb->error = TRUE;
                g_idle_add((gboolean (*)(gpointer))update_gui, sb);
                return;
              }

            if(numqueues >= MAX_QUEUES)
              {
                print_error(sb, TRUE, "%s: Too many IPC message queues", file_name);
                sb->rs = stopped;
                sb->error = TRUE;
                g_idle_add((gboolean (*)(gpointer))update_gui, sb);
                return;
              }

            sb->fn = fn;
            sb->gn = gn;

            end_cfgcache(&sb->cfg, &cfgcc, cfn, cgn);

            clean_io(sb);

            if(!sb->cp_batch_in)
              {
                for(i = 0; i < sb->fn; i++)
                  {
                    if(sb->f[i].file_io)
                      {
                        sb->f[i].fp = open_output_file(sb->f[i].name_full);
                        if(!is_file_open(sb->f[i].fp))
                          {
                            print_error(sb, TRUE, sb->f[i].name_full);
                            clean_io(sb);
                            sb->rs = stopped;
                            sb->error = TRUE;
                            g_idle_add((gboolean (*)(gpointer))update_gui, sb);
                            return;
                          }
                      }
                    else
                      if(!sb->cp_sys5)
                        {
                          sb->f[i].cp = add_queue_posix(sb->f[i].name_full, output_stream);
                          if(failed_queue_posix(sb->f[i].cp))
                            {
                              print_error(sb, TRUE, sb->f[i].name_full);
                              clean_io(sb);
                              sb->rs = stopped;
                              sb->error = TRUE;
                              g_idle_add((gboolean (*)(gpointer))update_gui, sb);
                              return;
                            }
                        }
                      else
                        {
                          sb->f[i].cp5 = add_queue_sys5(sb->f[i].name_full, input_stream);
                          if(failed_queue_sys5(sb->f[i].cp5))
                            {
                              print_error(sb, TRUE, sb->f[i].name_full);
                              clean_io(sb);
                              sb->rs = stopped;
                              sb->error = TRUE;
                              g_idle_add((gboolean (*)(gpointer))update_gui, sb);
                              return;
                            }
                        }
                  }
              }
          }

        tot_rows = max(1, sb->fn + sb->gn);
        page_rows = min(tot_rows, sb->cfg.display_rows);
        gtk_adjustment_configure(sb->area_adj, 0, 0, tot_rows, 1, page_rows, page_rows);

        if(sb->spectrum_window)
          gtk_adjustment_configure(sb->spectrum_adj, 0, 0, tot_rows, 1, page_rows, page_rows);

        if(*sb->cfg.preext_name)
          {
            strcpy(cmd, CMD_PATH);
            strcat(cmd, sb->cfg.preext_name);
            strcat(cmd, " &");

            runsystem(sb, cmd);
          }

        sb->time_base = get_time();

        if(sb->cfg.num_threads > 1)
          {
            if(sb->cfg.hp_io)
              sb->version = 3;
            else
              sb->version = 2;
          }        
        else
          {
            if(sb->cfg.hp_io)
              sb->version = 1;
            else
              sb->version = 0;
          }
        
        strcpy(cmd, CMD_PATH);
        strcat(cmd, sb->cfg.engine_name[sb->version]);
        strcat(cmd, " 2>&1");

        sprintf(arg, " -n %d", sb->cfg.num_threads);
        strcat(cmd, arg);

        sprintf(arg, " -r %d", sb->cfg.bsbt);
        strcat(cmd, arg);

        sprintf(arg, " -t "REAL_OUT_FMT, sb->cfg.step);
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

        sprintf(arg, " -g "REAL_OUT_FMT, sb->time_base);
        strcat(cmd, arg);

        strcpy(sb->cmd, cmd);

        sb->term = FALSE;

        pthread_attr_init(&attributes);

#if defined BUGGED_PTHREADS
        leave_pipe(sb);
#else
        if(pthread_create(&sb->tinxpipe, &attributes, (void *)&tinxpipe, (void *)sb))
          {
            print_error(sb, TRUE, "POSIX thread error");
            sb->rs = stopped;
            sb->error = TRUE;
            g_idle_add((gboolean (*)(gpointer))update_gui, sb);
            return;
          }
#endif

        if(!sb->cp_quiet)
          {
            if(!sb->cp_batch_in)
              for(i = 0; i < sb->fn; i++)
                for(k = 0; k < sb->cp_horizon_size; k++)
                  {
                    sb->f[i].memory[k] = DISPLAY_UNKNOWN_CODE;
                    sb->f[i].realmem[k] = 0;
                  }

            if(!sb->cp_batch_out)
              {
                for(i = 0; i < sb->gn; i++)
                  for(k = 0; k < sb->cp_horizon_size; k++)
                    {
                      sb->g[i].memory[k] = DISPLAY_UNKNOWN_CODE;
                      sb->g[i].realmem[k] = 0;
                    }
                    
                for(i = 0; i < sb->gn; i++)
                  {
                    if(sb->g[i].file_io)
                      do
                        sb->g[i].fp = open_input_file(sb->g[i].name_full);
                      while(!sb->term && !is_file_open(sb->g[i].fp));
                    else
                      if(!sb->cp_sys5)
                        {
                          sb->g[i].cp = add_queue_posix(sb->g[i].name_full, input_stream);
                          if(failed_queue_posix(sb->g[i].cp))
                            {
                              print_error(sb, TRUE, sb->g[i].name_full);
                              clean_io(sb);
                              sb->rs = stopped;
                              sb->error = TRUE;
                              g_idle_add((gboolean (*)(gpointer))update_gui, sb);
                              return;
                            }
                        }
                      else
                        {
                          sb->g[i].cp5 = add_queue_sys5(sb->g[i].name_full, output_stream);
                          if(failed_queue_sys5(sb->g[i].cp5))
                            {
                              print_error(sb, TRUE, sb->g[i].name_full);
                              clean_io(sb);
                              sb->rs = stopped;
                              sb->error = TRUE;
                              g_idle_add((gboolean (*)(gpointer))update_gui, sb);
                              return;
                            }
                        }
                  }
              }

            if(!sb->term && pthread_create(&sb->tintloop, &attributes, (void *)&tintloop, (void *)sb))
              {
                print_error(sb, TRUE, "POSIX thread error");
                sb->rs = stopped;
                sb->error = TRUE;
                g_idle_add((gboolean (*)(gpointer))update_gui, sb);
                return;
              }
          }

        sb->rs = started;
        g_idle_add((gboolean (*)(gpointer))update_gui, sb);
      break;

      case started:
      case frozen:
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

                    if(sb->f[i].file_io)
                      {
                        if(sb->f[i].fp)
                          {
                            if(!sb->f[i].packed && put_file(sb->f[i].fp, &oc))
                              {
                                print_error(sb, TRUE, sb->f[i].name_full);
                                clean_io(sb);
                                sb->error = TRUE;
                                break;
                              }

                            if(close_file(sb->f[i].fp))
                              {
                                print_error(sb, TRUE, sb->f[i].name_full);
                                clean_io(sb);
                                sb->error = TRUE;
                                break;
                              }
                          }
                      }
                    else
                      if(!sb->cp_sys5)
                        {
                          if(!failed_queue_posix(sb->f[i].cp))
                            {
                              if(!sb->f[i].packed)
                                send_message_posix(sb->f[i].cp, &oc);

                              if(commit_queue_posix(sb->f[i].cp))
                                {
                                  print_error(sb, TRUE, sb->f[i].name_full);
                                  clean_io(sb);
                                  sb->error = TRUE;
                                  break;
                                }
                            }
                        }
                      else
                        if(!failed_queue_sys5(sb->f[i].cp5) && !sb->f[i].packed)
                          send_message_sys5(sb->f[i].cp5, &oc);
                  }
              }

            if(!sb->cp_batch_out)
              {
                if(sb->waitio)
                  {
                    count = 0;
                    halt = get_time();
                    do
                      {
                        for(i = 0; i < sb->gn; i++)
                          {
                            if(sb->g[i].file_io)
                              {
                                if(sb->g[i].fp)
                                  {
                                    if(get_file(sb->g[i].fp, &ic) && file_error(sb->g[i].fp))
                                      {
                                        print_error(sb, TRUE, sb->g[i].name_full);
                                        clean_io(sb);
                                        sb->error = TRUE;
                                        break;
                                      }

                                    if(ic == EOF)
                                      reset_file(sb->g[i].fp);
                                  }
                              }
                            else
                              if(!sb->cp_sys5)
                                read_message_posix(sb->g[i].cp, &ic);
                              else
                                read_message_sys5(sb->g[i].cp5, &ic);

                            if(!sb->g[i].packed && ic == sb->cfg.alpha[end_symbol])
                              count += TAIL_LEN;
                            else
                              if(ic != EOF)
                                count++;
                          }
                      }
                    while(!sb->term && count < sb->gn * TAIL_LEN && get_time() - halt < MAX_SEC_HALT);
                  }

                for(i = 0; i < sb->gn; i++)
                  if(sb->g[i].file_io)
                    {
                      if(sb->g[i].fp && close_file(sb->g[i].fp))
                        {
                          print_error(sb, TRUE, sb->g[i].name_full);
                          clean_io(sb);
                          sb->error = TRUE;
                          break;
                        }
                    }
                  else
                    if(!sb->cp_sys5 && !failed_queue_posix(sb->g[i].cp) && commit_queue_posix(sb->g[i].cp))
                      {
                        print_error(sb, TRUE, sb->g[i].name_full);
                        clean_io(sb);
                        sb->error = TRUE;
                        break;
                      }
              }
          }

        pid = 0;

        for(k = 0; k < MAX_WAIT; k++)
          {
            pid = pidof(sb, sb->cfg.engine_name[sb->version]);

            if(!pid)
              break;

            usleep(DELAY);
          }

        if(pid)
          {
            kill(pid, SIGINT);
            waitpid(pid);
          }

#if !defined BUGGED_PTHREADS
        pthread_join(sb->tinxpipe, NULL);
#endif

        if(*sb->cfg.postext_name)
          {
            strcpy(cmd, CMD_PATH);
            strcat(cmd, sb->cfg.postext_name);
            strcat(cmd, " &");

            runsystem(sb, cmd);
          }

        sb->rs = stopped;
        g_idle_add((gboolean (*)(gpointer))update_gui, sb);
      break;

      default:
      break;
    }
}

void runfreeze(s_base *sb)
{
  switch(sb->rs)
    {
      case started:
        switch_freeze(sb);

        sb->rs = frozen;
        g_idle_add((gboolean (*)(gpointer))update_gui, sb);
      break;

      case frozen:
        switch_freeze(sb);

        sb->rs = started;
        sb->time_base = get_time() - sb->t * sb->cp_step;
        g_idle_add((gboolean (*)(gpointer))update_gui, sb);
      break;

      default:
      break;
    }
}

bool exec_request(s_base *sb, runstate rs)
{
  bool rv;

  pthread_mutex_lock(&sb->mutex_request);

  if(sb->error)
    {
      sb->rsreq[0] = invalid;
      sb->startreq = 0;
      sb->endreq = 0;
      sb->error = FALSE;
    }

  if(sb->rsreq[sb->endreq] != rs)
    {
      sb->endreq = (sb->endreq + 1) % MAXREQUESTS;
      sb->rsreq[sb->endreq] = rs;
    }

  if(sb->processing)
    rv = FALSE;
  else
    {
      sb->processing = TRUE;
      rv = TRUE;
    }

  pthread_mutex_unlock(&sb->mutex_request);
  
  return rv;
}

runstate begin_request(s_base *sb)
{
  runstate rs;

  pthread_mutex_lock(&sb->mutex_request);

  if(sb->startreq == sb->endreq)
    rs = invalid;
  else
    {
      sb->startreq = (sb->startreq + 1) % MAXREQUESTS;
      rs = sb->rsreq[sb->startreq];

      if(sb->rs == rs || sb->error)
        rs = invalid;
    }

  pthread_mutex_unlock(&sb->mutex_request);

  return rs;
}

bool end_request(s_base *sb)
{
  pthread_mutex_lock(&sb->mutex_request);

  if(sb->startreq == sb->endreq)
    sb->processing = FALSE;

  pthread_mutex_unlock(&sb->mutex_request);
  
  return sb->processing;
}

void request_runstate(s_base *sb, runstate rs)
{
  if(exec_request(sb, rs))
    {
      do
        {
          rs = begin_request(sb);

          switch(sb->rs)
            {
              case stopped:
                if(rs == started)
                  runstop(sb);
              break;

              case started:
                if(rs == stopped)
                  runstop(sb);
                else
                  if(rs == frozen)
                    runfreeze(sb);
                else

              case frozen:
                if(rs == stopped)
                  runstop(sb);
                else
                  if(rs == started)
                    runfreeze(sb);
              break;

              case starting:
              case stopping:
              case invalid:
              break;
            }
        }
      while(end_request(sb));
    }
}

void run_button_clicked(GtkWidget *widget, s_base *sb)
{
  pthread_mutex_lock(&sb->mutex_button);

  if(sb->rs == stopped)
    request_runstate(sb, started);
  else
    {
      sb->waitio = FALSE;
      request_runstate(sb, stopped);
    }

  pthread_mutex_unlock(&sb->mutex_button);
}

void freeze_button_clicked(GtkWidget *widget, s_base *sb)
{
  pthread_mutex_lock(&sb->mutex_button);

  switch(sb->rs)
    {
      case started:
        request_runstate(sb, frozen);
      break;

      case frozen:
        request_runstate(sb, started);
      break;

      default:
      break;
    }

  pthread_mutex_unlock(&sb->mutex_button);
}

pid_t pidof(s_base *sb, char *name)
{
  FILE *fp;
  char cmd[MAX_STRLEN_IF], buffer[MAX_STRLEN_IF];
  pid_t pid;

  strcpy(cmd, "pidof ");
  strcat(cmd, name);

  pid = 0;

  fp = popen(cmd, "r");
  if(!fp)
    print_error(sb, TRUE, "Error retrieving PID");
  else
    {
      fread(buffer, sizeof(char), MAX_STRLEN_IF, fp);
      if(ferror(fp))
        print_error(sb, TRUE, "Broken pipe");
      else
        pid = atoi(buffer);

      pclose(fp);
    }

  return pid;
}

void gen_button_clicked(GtkWidget *widget, s_base *sb)
{
  FILE *fp;
  char cmd[MAX_STRLEN_IF], ch[LONG_STRLEN_IF];
  char *d, *e, *f;
  char a;
  bool got;

  pthread_mutex_lock(&sb->mutex_button);

  strcpy(cmd, CMD_PATH);
  strcat(cmd, sb->cfg.compiler_name);
  strcat(cmd, " 2>&1 -o '");
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

  if(sb->cfg.postopt)
    strcat(cmd, " -O");

  if(sb->cfg.constout)
    strcat(cmd, " -k");

  if(sb->cfg.constout_sugg)
    strcat(cmd, " -K");

  if(sb->cfg.constout_user)
    strcat(cmd, " -m");

  if(sb->cfg.outaux)
    strcat(cmd, " -b");

  if(sb->cfg.outint)
    strcat(cmd, " -B");

  sprintf(ch, " -M%d", sb->fancymsg);
  strcat(cmd, ch);

  strcat(cmd, " '");
  strcat(cmd, sb->cfg.source_name);
  strcat(cmd, "'");

  d = cmd;
  e = ch;

  *e = '\0';

  while(*d)
    {
      f = filterchar(*d);

      strcat(e, f);

      d++;
      e += strlen(f);
    }

  fancyprint(sb, "<b><i>%s</i></b>\n", ch);

  fp = popen(cmd, "r");

  if(!fp)
    print_error(sb, TRUE, "Error launching the compiler");
  else
    {
      d = ch;

      do
        {
          a = '\0';

          got = fread(&a, sizeof(char), 1, fp);
          if(ferror(fp))
            {
              print_error(sb, TRUE, "Broken pipe");
              break;
            }

          if(got)
            {
              if(a == '%')
                {
                  *d = a;
                  d++;
                }
                
              *d = a;
              d++;
            }

          if(d != ch && (a == '\r' || a == '\n' || feof(fp)))
            {
              *d = '\0';
              fancyprint_add(sb, ch);
              d = ch;
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

  sb->donotscroll = TRUE;
  fancyprint(sb, BANNER);

  flexible_dialog(sb, "Ok", NULL, HEADING_HTML);

  pthread_mutex_unlock(&sb->mutex_button);
}

gboolean exit_button_clicked_if(GtkWidget *widget, GdkEvent *event, s_base *sb)
{
  exit_button_clicked(widget, sb);

  return TRUE;
}

void exit_button_clicked(GtkWidget *widget, s_base *sb)
{
  pthread_mutex_lock(&sb->mutex_button);

  if(sb->donotquit)
    sb->donotquit = FALSE;
  else
    if(sb->rs == stopped)
      gtk_main_quit();
    else
      if(flexible_dialog(sb, "Yes", "No", "The executor is running\n\nDo you want to exit anyway?\n") == GTK_RESPONSE_ACCEPT)
        {
          sb->waitio = FALSE;
          request_runstate(sb, stopped);

          gtk_main_quit();
        }

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

      runsystem(sb, cmd);
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

      runsystem(sb, cmd);
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
      if(fwrite(CONFIG_HEADER""CONFIG_VERSION, sizeof(char), 8, fp) != 8 || fwrite(&sb->cfg, sizeof(sb->cfg), 1, fp) != 1)
        print_error(sb, TRUE, CONFIG_FILENAME);
      else
        print_message(sb, FALSE, "Default configuration saved");

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
  else
    print_error(sb, TRUE, CONFIG_FILENAME);

  pthread_mutex_unlock(&sb->mutex_button);
}

void erase_button_clicked(GtkWidget *widget, s_base *sb)
{
  pthread_mutex_lock(&sb->mutex_button);

  if(remove(CONFIG_FILENAME))
    print_error(sb, TRUE, CONFIG_FILENAME);
  else
    print_message(sb, FALSE, "Default configuration cleared");

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

void postopt_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->cfg.postopt = TRUE;
  else
    sb->cfg.postopt = FALSE;

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

void constout_sugg_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->cfg.constout_sugg = TRUE;
  else
    sb->cfg.constout_sugg = FALSE;

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

void constout_user_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->cfg.constout_user = TRUE;
  else
    sb->cfg.constout_user = FALSE;

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

void hp_io_box(GtkWidget *widget, s_base *sb)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    sb->cfg.hp_io = TRUE;
  else
    sb->cfg.hp_io = FALSE;

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

void compiler_fname(GtkWidget *widget, s_base *sb)
{
  strcpy(sb->cfg.compiler_name, gtk_entry_get_text(GTK_ENTRY(widget)));

  if(*sb->cfg.compiler_name)
    {
      gtk_widget_set_sensitive(GTK_WIDGET(sb->gen_button), TRUE);
      gtk_widget_set_sensitive(GTK_WIDGET(sb->gen_menu), TRUE);
    }
  else
    {
      gtk_widget_set_sensitive(GTK_WIDGET(sb->gen_button), FALSE);
      gtk_widget_set_sensitive(GTK_WIDGET(sb->gen_menu), FALSE);
    }

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

void monoengine_fname(GtkWidget *widget, s_base *sb)
{
  strcpy(sb->cfg.engine_name[0], gtk_entry_get_text(GTK_ENTRY(widget)));

  if(*sb->cfg.engine_name[0] && *sb->cfg.engine_name[1] && *sb->cfg.engine_name[2] && *sb->cfg.engine_name[3])
    {
      gtk_widget_set_sensitive(GTK_WIDGET(sb->run_button), TRUE);
      gtk_widget_set_sensitive(GTK_WIDGET(sb->run_menu), TRUE);
    }
  else
    {
      gtk_widget_set_sensitive(GTK_WIDGET(sb->run_button), FALSE);
      gtk_widget_set_sensitive(GTK_WIDGET(sb->run_menu), FALSE);
    }

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void dualengine_fname(GtkWidget *widget, s_base *sb)
{
  strcpy(sb->cfg.engine_name[1], gtk_entry_get_text(GTK_ENTRY(widget)));

  if(*sb->cfg.engine_name[0] && *sb->cfg.engine_name[1] && *sb->cfg.engine_name[2] && *sb->cfg.engine_name[3])
    {
      gtk_widget_set_sensitive(GTK_WIDGET(sb->run_button), TRUE);
      gtk_widget_set_sensitive(GTK_WIDGET(sb->run_menu), TRUE);
    }
  else
    {
      gtk_widget_set_sensitive(GTK_WIDGET(sb->run_button), FALSE);
      gtk_widget_set_sensitive(GTK_WIDGET(sb->run_menu), FALSE);
    }

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void multiengine_fname(GtkWidget *widget, s_base *sb)
{
  strcpy(sb->cfg.engine_name[2], gtk_entry_get_text(GTK_ENTRY(widget)));

  if(*sb->cfg.engine_name[0] && *sb->cfg.engine_name[1] && *sb->cfg.engine_name[2] && *sb->cfg.engine_name[3])
    {
      gtk_widget_set_sensitive(GTK_WIDGET(sb->run_button), TRUE);
      gtk_widget_set_sensitive(GTK_WIDGET(sb->run_menu), TRUE);
    }
  else
    {
      gtk_widget_set_sensitive(GTK_WIDGET(sb->run_button), FALSE);
      gtk_widget_set_sensitive(GTK_WIDGET(sb->run_menu), FALSE);
    }

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void largeengine_fname(GtkWidget *widget, s_base *sb)
{
  strcpy(sb->cfg.engine_name[3], gtk_entry_get_text(GTK_ENTRY(widget)));

  if(*sb->cfg.engine_name[0] && *sb->cfg.engine_name[1] && *sb->cfg.engine_name[2] && *sb->cfg.engine_name[3])
    {
      gtk_widget_set_sensitive(GTK_WIDGET(sb->run_button), TRUE);
      gtk_widget_set_sensitive(GTK_WIDGET(sb->run_menu), TRUE);
    }
  else
    {
      gtk_widget_set_sensitive(GTK_WIDGET(sb->run_button), FALSE);
      gtk_widget_set_sensitive(GTK_WIDGET(sb->run_menu), FALSE);
    }

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void preext_fname(GtkWidget *widget, s_base *sb)
{
  strcpy(sb->cfg.preext_name, gtk_entry_get_text(GTK_ENTRY(widget)));

  if(!sb->changed)
    {
      sb->changed = TRUE;

      gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
      if(sb->save_button)
        gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
    }
}

void postext_fname(GtkWidget *widget, s_base *sb)
{
  strcpy(sb->cfg.postext_name, gtk_entry_get_text(GTK_ENTRY(widget)));

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

gboolean compiler_default(GtkWidget *widget, cairo_t *cr, s_base *sb)
{
  gtk_entry_set_text(GTK_ENTRY(widget), sb->cfg.compiler_name);

  return FALSE;
}

gboolean monoengine_default(GtkWidget *widget, cairo_t *cr, s_base *sb)
{
  gtk_entry_set_text(GTK_ENTRY(widget), sb->cfg.engine_name[0]);

  return FALSE;
}

gboolean dualengine_default(GtkWidget *widget, cairo_t *cr, s_base *sb)
{
  gtk_entry_set_text(GTK_ENTRY(widget), sb->cfg.engine_name[1]);

  return FALSE;
}

gboolean multiengine_default(GtkWidget *widget, cairo_t *cr, s_base *sb)
{
  gtk_entry_set_text(GTK_ENTRY(widget), sb->cfg.engine_name[2]);

  return FALSE;
}

gboolean largeengine_default(GtkWidget *widget, cairo_t *cr, s_base *sb)
{
  gtk_entry_set_text(GTK_ENTRY(widget), sb->cfg.engine_name[3]);

  return FALSE;
}

gboolean preext_default(GtkWidget *widget, cairo_t *cr, s_base *sb)
{
  gtk_entry_set_text(GTK_ENTRY(widget), sb->cfg.preext_name);

  return FALSE;
}

gboolean postext_default(GtkWidget *widget, cairo_t *cr, s_base *sb)
{
  gtk_entry_set_text(GTK_ENTRY(widget), sb->cfg.postext_name);

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

int load_config(FILE *fp, config *cp)
{
  char buffer[MAX_STRLEN];

  if(fread(buffer, sizeof(char), 8, fp) != 8 || memcmp(CONFIG_HEADER, buffer, 4) || memcmp(CONFIG_VERSION, buffer + 4, 4) < 0)
    return 1;

  fread(cp, sizeof(*cp), 1, fp);

  if(ferror(fp))
    return 1;
  else
    return 0;
}

void cfg_dialog(GtkEntry *widget, GtkEntryIconPosition pos, GdkEvent *event, s_base *sb)
{
  FILE *fp;
  GtkDialog *dialog;
  char fname[MAX_STRLEN];
  config cfg;

  dialog = GTK_DIALOG(gtk_file_chooser_dialog_new("Select configuration file", sb->window, GTK_FILE_CHOOSER_ACTION_OPEN, "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL));

  if(gtk_dialog_run(dialog) == GTK_RESPONSE_ACCEPT)
    {
      strcpy(fname, gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog)));
      fp = fopen(fname, "r");
      if(fp)
        {
          if(load_config(fp, &cfg))
            print_error(sb, TRUE, "%s: Wrong file format", fname);
          else
            {
              sb->donotquit = TRUE;

              if(sb->config_window)
                {
                  gtk_widget_destroy(GTK_WIDGET(sb->config_window));
                  sb->config_window = NULL;
                }

              if(sb->prob_window)
                {
                  gtk_widget_destroy(GTK_WIDGET(sb->prob_window));
                  sb->prob_window = NULL;
                }

              if(sb->filter_window)
                {
                  gtk_widget_destroy(GTK_WIDGET(sb->filter_window));
                  sb->filter_window = NULL;
                }

              if(sb->pplan_window)
                {
                  gtk_widget_destroy(GTK_WIDGET(sb->pplan_window));
                  sb->pplan_window = NULL;
                }

              if(sb->spectrum_window)
                {
                  gtk_widget_destroy(GTK_WIDGET(sb->spectrum_window));
                  sb->spectrum_window = NULL;
                }

              if(sb->plan_window)
                {
                  gtk_widget_destroy(GTK_WIDGET(sb->plan_window));
                  sb->plan_window = NULL;
                }

              sb->waitio = FALSE;
              request_runstate(sb, stopped);

              gtk_widget_destroy(GTK_WIDGET(sb->window));

              sb->cfg = cfg;

              main_window(sb);

              print_message(sb, FALSE, "Configuration loaded from %s", fname);

              if(!sb->changed)
                {
                  sb->changed = TRUE;

                  gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
                  if(sb->save_button)
                    gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
                }
            }

          fclose(fp);
        }
      else
        print_error(sb, TRUE, fname);
    }

  gtk_widget_destroy(GTK_WIDGET(dialog));
}

void cfg_dialog_if(GtkEntry *widget, s_base *sb)
{
  cfg_dialog(widget, 0, NULL, sb);
}

void saveas_dialog(GtkEntry *widget, GtkEntryIconPosition pos, GdkEvent *event, s_base *sb)
{
  FILE *fp;
  GtkDialog *dialog;
  char fname[MAX_STRLEN];

  dialog = GTK_DIALOG(gtk_file_chooser_dialog_new("Select configuration file", sb->window, GTK_FILE_CHOOSER_ACTION_SAVE, "Cancel", GTK_RESPONSE_CANCEL, "Save", GTK_RESPONSE_ACCEPT, NULL));

  if(gtk_dialog_run(dialog) == GTK_RESPONSE_ACCEPT)
    {
      strcpy(fname, gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog)));
      fp = fopen(fname, "r");
      if(fp)
        {
          fclose(fp);

          if(flexible_dialog(sb, "Yes", "No", "Selected file already exists\n\nDo you want to replace it?\n") == GTK_RESPONSE_ACCEPT)
            fp = NULL;
        }

      if(!fp)
        {
          fp = fopen(fname, "w");

          if(fp)
            {
              if(fwrite(CONFIG_HEADER""CONFIG_VERSION, sizeof(char), 8, fp) != 8 || fwrite(&sb->cfg, sizeof(sb->cfg), 1, fp) != 1)
                print_error(sb, TRUE, fname);
              else
                print_message(sb, FALSE, "Configuration saved to %s", fname);

              fclose(fp);
            }
          else
            print_error(sb, TRUE, fname);
        }
    }

  gtk_widget_destroy(GTK_WIDGET(dialog));
}

void saveas_dialog_if(GtkEntry *widget, s_base *sb)
{
  saveas_dialog(widget, 0, NULL, sb);
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

  if(sb->rs == stopped || sb->rs == frozen)
    update_drawing(sb);
}

void spectrum_value(GtkWidget *widget, s_base *sb)
{
  sb->specpos = gtk_adjustment_get_value(sb->spectrum_adj);

  if(sb->rs == stopped || sb->rs == frozen)
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

gboolean goto_start(s_base *sb)
{
  GtkTextIter iter;

  gtk_text_buffer_get_start_iter(gtk_text_view_get_buffer(sb->textarea), &iter);
  gtk_text_view_scroll_to_iter(sb->textarea, &iter, 0, FALSE, 0, 0);

  return G_SOURCE_REMOVE;
}

gboolean goto_end(s_base *sb)
{
  GtkTextIter iter;

  gtk_text_buffer_get_end_iter(gtk_text_view_get_buffer(sb->textarea), &iter);
  gtk_text_view_scroll_to_iter(sb->textarea, &iter, 0, FALSE, 0, 0);

  return G_SOURCE_REMOVE;
}

gboolean update_view(s_base *sb)
{
  char buffer[XBUFSIZE];
  char ch;
  int i, j, n;
  bool xcat, fancymsg, backmsg;
  GtkTextBuffer *textbuffer;
  GtkTextIter iter;

  pthread_mutex_lock(&sb->mutex_xbuffer);

  fancymsg = backmsg = FALSE;
  n = 0;

  while(sb->xstart != sb->xend)
    {
      n = (XBUFSIZE + sb->xend - sb->xstart) % XBUFSIZE;
      j = 0;

      for(i = 0; i < n; i++)
        {
          ch = sb->xbuffer[(sb->xstart + i) % XBUFSIZE];

          if(ch == XON)
            {
              if(!fancymsg)
                {
                  backmsg = TRUE;
                  
                  if(j > 0)
                    {
                      i++;
                      break;
                    }
                  else
                    fancymsg = TRUE;
                }
            }
          else
            if(ch == XOFF)
              {
                if(fancymsg)
                  {
                    backmsg = FALSE;

                  if(j > 0)
                    {
                      i++;
                      break;
                    }
                  else
                    fancymsg = FALSE;
                  }
              }
            else
              {
                buffer[j] = ch;
                j++;
              }
        }

      sb->xstart = (sb->xstart + i) % XBUFSIZE;

      buffer[j] = '\0';

      xcat = sb->xcat;
      if(!xcat)
        sb->xcat = TRUE;

      textbuffer = gtk_text_view_get_buffer(sb->textarea);

      if(!xcat)
        {
          gtk_text_buffer_set_text(textbuffer, "", 0);
          gtk_text_buffer_get_start_iter(gtk_text_view_get_buffer(sb->textarea), &iter);
        }
      else
        gtk_text_buffer_get_end_iter(gtk_text_view_get_buffer(sb->textarea), &iter);

      if(fancymsg)
        gtk_text_buffer_insert_markup(textbuffer, &iter, buffer, -1);
      else
        gtk_text_buffer_insert(textbuffer, &iter, buffer, -1);
        
      fancymsg = backmsg;
    }

  pthread_mutex_unlock(&sb->mutex_xbuffer);

  if(n)
    {
      if(sb->donotscroll)
        {
          sb->donotscroll = FALSE;
          g_idle_add((gboolean (*)(gpointer))goto_start, sb);
        }
      else
        g_idle_add((gboolean (*)(gpointer))goto_end, sb);
    }

  return G_SOURCE_REMOVE;
}

bool insert_text(s_base *sb, char *buffer, bool fancymsg)
{
  int i, n;
  bool flush;

  flush = FALSE;
  n = strlen(buffer);
  if(n < XBUFSIZE - 2)
    {
      if(fancymsg)
        {
          sb->xbuffer[sb->xend] = XON;

          sb->xend = (sb->xend + 1) % XBUFSIZE;
          if(sb->xstart == sb->xend)
            sb->xstart = (sb->xstart + 1) % XBUFSIZE;
        }

      for(i = 0; i < n; i++)
        {
          sb->xbuffer[sb->xend] = buffer[i];
          if(buffer[i] == '\r' || buffer[i] == '\n')
            flush = TRUE;

          sb->xend = (sb->xend + 1) % XBUFSIZE;
          if(sb->xstart == sb->xend)
            sb->xstart = (sb->xstart + 1) % XBUFSIZE;
        }

      if(fancymsg)
        {
          sb->xbuffer[sb->xend] = XOFF;

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

  return flush;
}

void print(s_base *sb, char *string, ...)
{
  char buffer[XBUFSIZE];
  va_list arglist;
  bool flush;

  va_start(arglist, string);
  vsnprintf(buffer, XBUFSIZE, string, arglist);
  va_end(arglist);

  pthread_mutex_lock(&sb->mutex_xbuffer);

  sb->xstart = 0;
  sb->xend = 0;

  flush = insert_text(sb, buffer, FALSE);

  sb->xcat = FALSE;
  
  pthread_mutex_unlock(&sb->mutex_xbuffer);

  if(!sb->cp_echo_stdout && flush)
    g_idle_add((gboolean (*)(gpointer))update_view, sb);
}

void print_add(s_base *sb, char *string, ...)
{
  char buffer[XBUFSIZE];
  va_list arglist;
  bool flush;

  va_start(arglist, string);
  vsnprintf(buffer, XBUFSIZE, string, arglist);
  va_end(arglist);

  pthread_mutex_lock(&sb->mutex_xbuffer);

  flush = insert_text(sb, buffer, FALSE);

  pthread_mutex_unlock(&sb->mutex_xbuffer);

  if(!sb->cp_echo_stdout && flush)
    g_idle_add((gboolean (*)(gpointer))update_view, sb);
}

void fancyprint(s_base *sb, char *string, ...)
{
  char buffer[XBUFSIZE];
  va_list arglist;
  bool flush;

  va_start(arglist, string);
  vsnprintf(buffer, XBUFSIZE, string, arglist);
  va_end(arglist);

  pthread_mutex_lock(&sb->mutex_xbuffer);

  sb->xstart = 0;
  sb->xend = 0;

  flush = insert_text(sb, buffer, TRUE);

  sb->xcat = FALSE;

  pthread_mutex_unlock(&sb->mutex_xbuffer);

  if(!sb->cp_echo_stdout && flush)
    g_idle_add((gboolean (*)(gpointer))update_view, sb);
}

void fancyprint_add(s_base *sb, char *string, ...)
{
  char buffer[XBUFSIZE];
  va_list arglist;
  bool flush;

  va_start(arglist, string);
  vsnprintf(buffer, XBUFSIZE, string, arglist);
  va_end(arglist);

  pthread_mutex_lock(&sb->mutex_xbuffer);

  flush = insert_text(sb, buffer, TRUE);

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
  int fn, i, n, packed;
  io_type stype;
  bool known[MAX_FILES];
  char c, d, str[3];
  cfgcache cfgcc;
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
      print_error(sb, TRUE, file_name);
      pthread_mutex_unlock(&sb->mutex_button);
      return;
    }

  fn = 0;

  stype = io_any;
  packed = 0;

  for(i = 0; i < MAX_FILES; i++)
    known[i] = FALSE;

  begin_cfgcache(&sb->cfg, &cfgcc);

  while(fscanf(bp, " "SKIP_FMT" ", name) >= 1);

  if(ferror(bp))
    {
      print_error(sb, TRUE, file_name);
      pthread_mutex_unlock(&sb->mutex_button);
      return;
    }

  while(fscanf(bp, " "FUN_FMT" "FUN_FMT" ( %*[^"SEPARATORS"] , %*[^"SEPARATORS"] ) # %*d / %u , %d , %*d , %*g, %*u @ %*ld ", str, name, &stype, &packed) >= 2)
    {
      d = '\0';
      sscanf(str, OP_FMT""OP_FMT, &c, &d);

      if(stype != io_socket && (packed <= 0 || !known[packed]))
        {
          if(packed > 0)
            known[packed] = TRUE;

          switch(c)
            {
              case '!':
                if(fn >= MAX_FILES)
                  {
                    print_error(sb, TRUE, "%s: Too many input signals", file_name);
                    pthread_mutex_unlock(&sb->mutex_button);
                    return;
                  }

                remap_cfgcache(&sb->cfg, &cfgcc, fn, -1, name);

                fn++;
              break;

              case '?':
              case '.':
              case '_':
              break;

              default:
                print_error(sb, TRUE, "%s, "OP_FMT": Invalid stream class", file_name, c);
                pthread_mutex_unlock(&sb->mutex_button);
                return;
              break;
            }
        }

      stype = io_any;
      packed = 0;
    }

  if(ferror(bp))
    {
      print_error(sb, TRUE, file_name);
      pthread_mutex_unlock(&sb->mutex_button);
      return;
    }

  if(!feof(bp))
    {
      print_error(sb, TRUE, "%s: Parser error", file_name);
      pthread_mutex_unlock(&sb->mutex_button);
      return;
    }

  if(fclose(bp))
    {
      print_error(sb, TRUE, file_name);
      pthread_mutex_unlock(&sb->mutex_button);
      return;
    }

  if(fn > MAX_EXT_SIGNALS)
    {
      print_error(sb, TRUE, "Too many input signals to edit");
      pthread_mutex_unlock(&sb->mutex_button);
      return;
    }

  end_cfgcache(&sb->cfg, &cfgcc, fn, -1);
            
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), PROB_WINDOW_WIDTH, PROB_WINDOW_HEIGHT);
  gtk_container_set_border_width(GTK_CONTAINER(window), 10);
  gtk_window_set_title(GTK_WINDOW(window), PROB_TITLE);
  gtk_window_set_icon_name(GTK_WINDOW(window), "tinx");
  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(end_prob), (gpointer)sb);

  sb->prob_window = GTK_WINDOW(window);

  extbox = gtk_vbox_new(FALSE, 15);

  sb->changeprob = FALSE;

  if(fn > 0)
    {
      n = ceil(sqrt(fn / 2.0));

      tabdyn = gtk_table_new(2 * n, n, FALSE);

      for(i = 0; i < fn; i++)
        {
          sprintf(name, " %s ", sb->cfg.fname[i]);
          
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
    }
  else
    {
      lbl = gtk_label_new("Current project has no inputs");
      gtk_box_pack_start(GTK_BOX(extbox), lbl, TRUE, TRUE, 0);
    }

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
          {
            sb->cfg.fexcl[i] = TRUE;

            if(!sb->changefilter)
              {
                sb->changefilter = TRUE;
                gtk_widget_set_sensitive(GTK_WIDGET(sb->filclear_button), TRUE);
              }
          }
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
          {
            sb->cfg.gexcl[i] = TRUE;

            if(!sb->changefilter)
              {
                sb->changefilter = TRUE;
                gtk_widget_set_sensitive(GTK_WIDGET(sb->filclear_button), TRUE);
              }
          }
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
  sb->filclear_button = NULL;
}

void filter_clear(GtkWidget *widget, s_base *sb)
{
  int i;

  pthread_mutex_lock(&sb->mutex_button);

  if(sb->changefilter)
    {
      for(i = 0; i < MAX_FILES; i++)
        {
          sb->cfg.fexcl[i] = FALSE;
          sb->cfg.gexcl[i] = FALSE;
        }

      for(i = 0; i < sb->cfg.fn; i++)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(sb->infilter_widget[i]), FALSE);

      for(i = 0; i < sb->cfg.gn; i++)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(sb->outfilter_widget[i]), FALSE);

      sb->changefilter = FALSE;
      gtk_widget_set_sensitive(GTK_WIDGET(sb->filclear_button), FALSE);

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

void filter_button_clicked(GtkWidget *widget, s_base *sb)
{
  FILE *bp;
  char file_name[MAX_STRLEN], name[MAX_STRLEN];
  int fn, gn, i, n, packed;
  io_type stype;
  bool known[MAX_FILES];
  char c, d, str[3];
  cfgcache cfgcc;
  GtkWidget *window;
  GtkWidget *fr1, *fr2;
  GtkWidget *tabdyn, *tabdyn2;
  GtkWidget *cb, *lbl, *lbl2;
  GtkWidget *btn;
  GtkWidget *extbox, *inbox, *outbox, *bbox;

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
      print_error(sb, TRUE, file_name);
      pthread_mutex_unlock(&sb->mutex_button);
      return;
    }

  fn = 0;
  gn = 0;

  stype = io_any;
  packed = 0;

  for(i = 0; i < MAX_FILES; i++)
    known[i] = FALSE;

  begin_cfgcache(&sb->cfg, &cfgcc);

  while(fscanf(bp, " "SKIP_FMT" ", name) >= 1);

  if(ferror(bp))
    {
      print_error(sb, TRUE, file_name);
      pthread_mutex_unlock(&sb->mutex_button);
      return;
    }

  while(fscanf(bp, " "FUN_FMT" "FUN_FMT" ( %*[^"SEPARATORS"] , %*[^"SEPARATORS"] ) # %*d / %u , %d , %*d , %*g, %*u @ %*ld ", str, name, &stype, &packed) >= 2)
    {
      d = '\0';
      sscanf(str, OP_FMT""OP_FMT, &c, &d);

      if(stype != io_socket && (packed <= 0 || !known[packed]))
        {
          if(packed > 0)
            known[packed] = TRUE;

          switch(c)
            {
              case '!':
                if(fn >= MAX_FILES)
                  {
                    print_error(sb, TRUE, "%s: Too many input signals", file_name);
                    pthread_mutex_unlock(&sb->mutex_button);
                    return;
                  }

                remap_cfgcache(&sb->cfg, &cfgcc, fn, -1, name);

                fn++;
              break;

              case '?':
              case '.':
                if(gn >= MAX_FILES)
                  {
                    print_error(sb, TRUE, "%s: Too many output signals", file_name);
                    pthread_mutex_unlock(&sb->mutex_button);
                    return;
                  }

                remap_cfgcache(&sb->cfg, &cfgcc, -1, gn, name);

                gn++;
              break;

              case '_':
              break;

              default:
                print_error(sb, TRUE, "%s, "OP_FMT": Invalid stream class", file_name, c);
                pthread_mutex_unlock(&sb->mutex_button);
                return;
              break;
            }
        }

      stype = io_any;
      packed = 0;
    }

  if(ferror(bp))
    {
      print_error(sb, TRUE, file_name);
      pthread_mutex_unlock(&sb->mutex_button);
      return;
    }

  if(!feof(bp))
    {
      print_error(sb, TRUE, "%s: Parser error", file_name);
      pthread_mutex_unlock(&sb->mutex_button);
      return;
    }

  if(fclose(bp))
    {
      print_error(sb, TRUE, file_name);
      pthread_mutex_unlock(&sb->mutex_button);
      return;
    }

  if(fn + gn > MAX_EXT_SIGNALS)
    {
      print_error(sb, TRUE, "Too many input or output signals to edit");
      pthread_mutex_unlock(&sb->mutex_button);
      return;
    }

  end_cfgcache(&sb->cfg, &cfgcc, fn, gn);

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

  sb->changefilter = FALSE;

  if(fn > 0)
    {
      n = ceil(sqrt(fn / 2.0));

      tabdyn = gtk_table_new(2 * n, n, FALSE);

      for(i = 0; i < fn; i++)
        {
          cb = gtk_check_button_new_with_label(sb->cfg.fname[i]);
          gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb), sb->cfg.fexcl[i]);
          gtk_table_attach_defaults(GTK_TABLE(tabdyn), cb, i % n, i % n + 1, i / n, i / n + 1);
          g_signal_connect(G_OBJECT(cb), "clicked", G_CALLBACK(infilter_box), (gpointer)sb);

          sb->infilter_widget[i] = cb;

          if(sb->cfg.fexcl[i])
            sb->changefilter = TRUE;
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
          cb = gtk_check_button_new_with_label(sb->cfg.gname[i]);
          gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb), sb->cfg.gexcl[i]);
          gtk_table_attach_defaults(GTK_TABLE(tabdyn2), cb, i % n, i % n + 1, i / n, i / n + 1);
          g_signal_connect(G_OBJECT(cb), "clicked", G_CALLBACK(outfilter_box), (gpointer)sb);

          sb->outfilter_widget[i] = cb;

          if(sb->cfg.gexcl[i])
            sb->changefilter = TRUE;
        }

      gtk_box_pack_start(GTK_BOX(outbox), tabdyn2, TRUE, TRUE, 0);
    }
  else
    {
      lbl2 = gtk_label_new("Current project has no outputs");
      gtk_box_pack_start(GTK_BOX(outbox), lbl2, TRUE, TRUE, 0);
    }

  gtk_box_pack_start(GTK_BOX(extbox), fr1, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(extbox), fr2, TRUE, TRUE, 0);

  bbox = gtk_hbox_new(FALSE, 15);

  btn = gtk_button_new_with_label("Clear");
  gtk_widget_set_size_request(btn, 70, 30);
  gtk_box_pack_start(GTK_BOX(bbox), btn, FALSE, FALSE, 0);
  gtk_box_pack_end(GTK_BOX(extbox), bbox, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(btn), "clicked", G_CALLBACK(filter_clear), (gpointer)sb);

  sb->filclear_button = GTK_BUTTON(btn);

  if(sb->changefilter)
    gtk_widget_set_sensitive(GTK_WIDGET(btn), TRUE);
  else
    gtk_widget_set_sensitive(GTK_WIDGET(btn), FALSE);

  gtk_container_add(GTK_CONTAINER(window), extbox);

  gtk_widget_show_all(window);

  pthread_mutex_unlock(&sb->mutex_button);
}

void pplan_box(GtkWidget *widget, s_base *sb)
{
  int i, j;

  for(i = 0; i < sb->cfg.gn; i++)
    for(j = 0; j < sb->cfg.gn; j++)
      if(widget == sb->pplan_widget[i][j])
        {
          if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
            {
              sb->cfg.pplan[i][j] = TRUE;

              if(!sb->changepp)
                {
                  sb->changepp = TRUE;
                  gtk_widget_set_sensitive(GTK_WIDGET(sb->ppclear_button), TRUE);
                }
            }
          else
            sb->cfg.pplan[i][j] = FALSE;

          if(!sb->changed)
            {
              sb->changed = TRUE;

              gtk_widget_set_sensitive(GTK_WIDGET(sb->save_menu), TRUE);
              if(sb->save_button)
                gtk_widget_set_sensitive(GTK_WIDGET(sb->save_button), TRUE);
            }

          i = sb->cfg.gn;
          break;
        }
}

void end_pplan(GtkWidget *widget, s_base *sb)
{
  sb->pplan_window = NULL;
  sb->ppclear_button = NULL;
}

void pplan_clear(GtkWidget *widget, s_base *sb)
{
  int i, j;

  pthread_mutex_lock(&sb->mutex_button);

  if(sb->changepp)
    {
      for(i = 0; i < MAX_FILES; i++)
        for(j = 0; j < MAX_FILES; j++)
          sb->cfg.pplan[i][j] = FALSE;

      for(i = 0; i < sb->cfg.gn; i++)
        for(j = 0; j < sb->cfg.gn; j++)
          if(i != j)
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(sb->pplan_widget[i][j]), FALSE);

      sb->changepp = FALSE;
      gtk_widget_set_sensitive(GTK_WIDGET(sb->ppclear_button), FALSE);

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

void pplan_button_clicked(GtkWidget *widget, s_base *sb)
{
  FILE *bp;
  char file_name[MAX_STRLEN], name[MAX_STRLEN];
  int gn, i, j, n, packed;
  io_type stype;
  bool known[MAX_FILES];
  char c, d, str[3], buffer[MAX_STRLEN];
  cfgcache cfgcc;
  GtkWidget *window;
  GtkWidget *fr;
  GtkWidget *tabdyn;
  GtkWidget *cb, *lbl;
  GtkWidget *btn;
  GtkWidget *extbox, *outbox, *bbox;

  pthread_mutex_lock(&sb->mutex_button);

  if(sb->pplan_window)
    {
      gtk_window_present(sb->pplan_window);
      pthread_mutex_unlock(&sb->mutex_button);
      return;
    }

  strcpy(file_name, sb->cfg.base_name);
  strcat(file_name, NETWORK_EXT);

  bp = fopen(file_name, "r");
  if(!bp)
    {
      print_error(sb, TRUE, file_name);
      pthread_mutex_unlock(&sb->mutex_button);
      return;
    }

  gn = 0;

  stype = io_any;
  packed = 0;

  for(i = 0; i < MAX_FILES; i++)
    known[i] = FALSE;

  begin_cfgcache(&sb->cfg, &cfgcc);

  while(fscanf(bp, " "SKIP_FMT" ", name) >= 1);

  if(ferror(bp))
    {
      print_error(sb, TRUE, file_name);
      pthread_mutex_unlock(&sb->mutex_button);
      return;
    }

  while(fscanf(bp, " "FUN_FMT" "FUN_FMT" ( %*[^"SEPARATORS"] , %*[^"SEPARATORS"] ) # %*d / %u , %d , %*d , %*g, %*u @ %*ld ", str, name, &stype, &packed) >= 2)
    {
      d = '\0';
      sscanf(str, OP_FMT""OP_FMT, &c, &d);

      if(stype != io_socket && (packed <= 0 || !known[packed]))
        {
          if(packed > 0)
            known[packed] = TRUE;

          switch(c)
            {
              case '!':
              break;

              case '?':
              case '.':
                if(gn >= MAX_FILES)
                  {
                    print_error(sb, TRUE, "%s: Too many output signals", file_name);
                    pthread_mutex_unlock(&sb->mutex_button);
                    return;
                  }

                remap_cfgcache(&sb->cfg, &cfgcc, -1, gn, name);

                gn++;
              break;

              case '_':
              break;

              default:
                print_error(sb, TRUE, "%s, "OP_FMT": Invalid stream class", file_name, c);
                pthread_mutex_unlock(&sb->mutex_button);
                return;
              break;
            }
        }

      stype = io_any;
      packed = 0;
    }

  if(ferror(bp))
    {
      print_error(sb, TRUE, file_name);
      pthread_mutex_unlock(&sb->mutex_button);
      return;
    }

  if(!feof(bp))
    {
      print_error(sb, TRUE, "%s: Parser error", file_name);
      pthread_mutex_unlock(&sb->mutex_button);
      return;
    }

  if(fclose(bp))
    {
      print_error(sb, TRUE, file_name);
      pthread_mutex_unlock(&sb->mutex_button);
      return;
    }

  if(gn * gn > MAX_EXT_SIGNALS)
    {
      print_error(sb, TRUE, "Too many output signals to edit");
      pthread_mutex_unlock(&sb->mutex_button);
      return;
    }

  end_cfgcache(&sb->cfg, &cfgcc, -1, gn);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), PLANSETUP_WINDOW_WIDTH, PLANSETUP_WINDOW_HEIGHT);
  gtk_container_set_border_width(GTK_CONTAINER(window), 10);
  gtk_window_set_title(GTK_WINDOW(window), PLANSETUP_TITLE);
  gtk_window_set_icon_name(GTK_WINDOW(window), "tinx");
  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(end_pplan), (gpointer)sb);

  sb->pplan_window = GTK_WINDOW(window);

  fr = gtk_frame_new("Pairs");
  gtk_label_set_markup(GTK_LABEL(gtk_frame_get_label_widget(GTK_FRAME(fr))), "<b>Pairs</b>");

  outbox = gtk_vbox_new(FALSE, 15);
  gtk_container_add(GTK_CONTAINER(fr), outbox);
  gtk_container_set_border_width(GTK_CONTAINER(fr), 5);
  gtk_container_set_border_width(GTK_CONTAINER(outbox), 5);

  extbox = gtk_vbox_new(FALSE, 0);

  sb->changepp = FALSE;

  if(gn > 0)
    {
      tabdyn = gtk_table_new(gn + 1, gn + 1, FALSE);

      for(j = 0; j < gn; j++)
        {
          sprintf(buffer, " %s ", sb->cfg.gname[j]);
          lbl = gtk_label_new(buffer);
          gtk_table_attach_defaults(GTK_TABLE(tabdyn), lbl, j + 1, j + 2, 0, 1);
        }

      n = 0;
      for(i = 0; i < gn; i++)
        {
          lbl = gtk_label_new(sb->cfg.gname[i]);
          gtk_table_attach_defaults(GTK_TABLE(tabdyn), lbl, 0, 1, i + 1, i + 2);

          for(j = 0; j < gn; j++)
            if(i != j)
              {
                sprintf(buffer, "%03d", n);
                cb = gtk_check_button_new_with_label(buffer);
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb), sb->cfg.pplan[i][j]);
                gtk_table_attach_defaults(GTK_TABLE(tabdyn), cb, j + 1, j + 2, i + 1, i + 2);
                g_signal_connect(G_OBJECT(cb), "clicked", G_CALLBACK(pplan_box), (gpointer)sb);

                sb->pplan_widget[i][j] = cb;

                if(sb->cfg.pplan[i][j])
                  sb->changepp = TRUE;
                  
                n++;
              }
        }

      gtk_box_pack_start(GTK_BOX(outbox), tabdyn, TRUE, TRUE, 0);
    }
  else
    {
      lbl = gtk_label_new("Current project has no outputs");
      gtk_box_pack_start(GTK_BOX(outbox), lbl, TRUE, TRUE, 0);
    }

  gtk_box_pack_start(GTK_BOX(extbox), fr, TRUE, TRUE, 0);

  bbox = gtk_hbox_new(FALSE, 15);

  btn = gtk_button_new_with_label("Clear");
  gtk_widget_set_size_request(btn, 70, 30);
  gtk_box_pack_start(GTK_BOX(bbox), btn, FALSE, FALSE, 0);
  gtk_box_pack_end(GTK_BOX(extbox), bbox, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(btn), "clicked", G_CALLBACK(pplan_clear), (gpointer)sb);

  sb->ppclear_button = GTK_BUTTON(btn);

  if(sb->changepp)
    gtk_widget_set_sensitive(GTK_WIDGET(btn), TRUE);
  else
    gtk_widget_set_sensitive(GTK_WIDGET(btn), FALSE);

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
  GtkWidget *tabf, *tabk, *tabh, *tabg, *tabs, *tabw, *tabz, *tabi;
  GtkWidget *cb8, *cb9, *cb10, *cb11, *cb12, *cb13, *cb14, *cb15, *cb16, *cb17, *cb18, *cb19, *cb20, *cb21, *cb22, *cb23, *cb24;
  GtkWidget *lbl2, *ent2, *lbl3, *ent3, *lbl4, *ent4, *lbl7, *ent7, *lbl10, *ent10, *lbl11, *ent11, *lbl12, *ent12, *lbl13, *ent13, *lbl14, *ent14, *lbl15, *ent15, *lbl16, *ent16, *lbl17, *ent17,
            *lbl18, *ent18, *lbl19, *ent19, *lbl20, *ent20, *lbl21, *ent21, *lbl22, *ent22, *lbl23, *ent23, *lbl24, *ent24, *lbl25, *ent25, *lbl26, *ent26, *lbl27, *ent27, *lbl28, *ent28, *lbl29, *ent29;
  GtkWidget *fr1, *fr2, *fr3, *fr4, *fr5, *fr6;
  GtkWidget *vbox1, *vbox2, *vbox3, *vbox6, *vbox7, *vbox8, *vbox9, *vbox10, *hbox1, *hbox2, *hbox4, *extbox, *winbox;
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

  tabf = gtk_table_new(4, 7, FALSE);

  lbl2 = gtk_label_new("Network object file name ");
  gtk_misc_set_alignment(GTK_MISC(lbl2), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabf), lbl2, 0, 1, 0, 1);
  ent2 = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(ent2), 0);
  gtk_entry_set_width_chars(GTK_ENTRY(ent2), 30);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent2), 30);
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
  gtk_entry_set_width_chars(GTK_ENTRY(ent3), 30);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent3), 30);
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
  gtk_entry_set_width_chars(GTK_ENTRY(ent4), 30);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent4), 30);
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
  gtk_entry_set_width_chars(GTK_ENTRY(ent10), 30);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent10), 30);
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
  gtk_entry_set_width_chars(GTK_ENTRY(ent20), 30);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent20), 30);
  gtk_entry_set_icon_from_stock(GTK_ENTRY(ent20), GTK_ENTRY_ICON_SECONDARY, GTK_STOCK_FILE);
  gtk_entry_set_text(GTK_ENTRY(ent20), sb->cfg.include_path);
  gtk_table_attach_defaults(GTK_TABLE(tabf), ent20, 1, 2, 4, 5);
  g_signal_connect(G_OBJECT(ent20), "changed", G_CALLBACK(include_path_fname), (gpointer)sb);
  g_signal_connect(G_OBJECT(ent20), "draw", G_CALLBACK(include_path_default), (gpointer)sb);
  g_signal_connect(G_OBJECT(ent20), "icon-press", G_CALLBACK(include_path_dialog), (gpointer)sb);

  lbl21 = gtk_label_new("Text editor ");
  gtk_misc_set_alignment(GTK_MISC(lbl21), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabf), lbl21, 0, 1, 5, 6);
  ent21 = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(ent21), 0);
  gtk_entry_set_width_chars(GTK_ENTRY(ent21), 30);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent21), 30);
  gtk_entry_set_text(GTK_ENTRY(ent21), sb->cfg.editor_name);
  gtk_table_attach_defaults(GTK_TABLE(tabf), ent21, 1, 2, 5, 6);
  g_signal_connect(G_OBJECT(ent21), "changed", G_CALLBACK(editor_fname), (gpointer)sb);
  g_signal_connect(G_OBJECT(ent21), "draw", G_CALLBACK(editor_default), (gpointer)sb);

  lbl22 = gtk_label_new("Help viewer ");
  gtk_misc_set_alignment(GTK_MISC(lbl22), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabf), lbl22, 0, 1, 6, 7);
  ent22 = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(ent22), 0);
  gtk_entry_set_width_chars(GTK_ENTRY(ent22), 30);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent22), 30);
  gtk_entry_set_text(GTK_ENTRY(ent22), sb->cfg.viewer_name);
  gtk_table_attach_defaults(GTK_TABLE(tabf), ent22, 1, 2, 6, 7);
  g_signal_connect(G_OBJECT(ent22), "changed", G_CALLBACK(viewer_fname), (gpointer)sb);
  g_signal_connect(G_OBJECT(ent22), "draw", G_CALLBACK(viewer_default), (gpointer)sb);

  lbl23 = gtk_label_new("Logic compiler ");
  gtk_misc_set_alignment(GTK_MISC(lbl23), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabf), lbl23, 2, 3, 0, 1);
  ent23 = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(ent23), 0);
  gtk_entry_set_width_chars(GTK_ENTRY(ent23), 30);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent23), 30);
  gtk_entry_set_text(GTK_ENTRY(ent23), sb->cfg.compiler_name);
  gtk_table_attach_defaults(GTK_TABLE(tabf), ent23, 3, 4, 0, 1);
  g_signal_connect(G_OBJECT(ent23), "changed", G_CALLBACK(compiler_fname), (gpointer)sb);
  g_signal_connect(G_OBJECT(ent23), "draw", G_CALLBACK(compiler_default), (gpointer)sb);

  lbl24 = gtk_label_new("Monoprocess executor ");
  gtk_misc_set_alignment(GTK_MISC(lbl24), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabf), lbl24, 2, 3, 1, 2);
  ent24 = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(ent24), 0);
  gtk_entry_set_width_chars(GTK_ENTRY(ent24), 30);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent24), 30);
  gtk_entry_set_text(GTK_ENTRY(ent24), sb->cfg.engine_name[0]);
  gtk_table_attach_defaults(GTK_TABLE(tabf), ent24, 3, 4, 1, 2);
  g_signal_connect(G_OBJECT(ent24), "changed", G_CALLBACK(monoengine_fname), (gpointer)sb);
  g_signal_connect(G_OBJECT(ent24), "draw", G_CALLBACK(monoengine_default), (gpointer)sb);

  lbl28 = gtk_label_new("High throughput monoprocess executor ");
  gtk_misc_set_alignment(GTK_MISC(lbl28), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabf), lbl28, 2, 3, 2, 3);
  ent28 = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(ent28), 0);
  gtk_entry_set_width_chars(GTK_ENTRY(ent28), 30);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent28), 30);
  gtk_entry_set_text(GTK_ENTRY(ent28), sb->cfg.engine_name[1]);
  gtk_table_attach_defaults(GTK_TABLE(tabf), ent28, 3, 4, 2, 3);
  g_signal_connect(G_OBJECT(ent28), "changed", G_CALLBACK(dualengine_fname), (gpointer)sb);
  g_signal_connect(G_OBJECT(ent28), "draw", G_CALLBACK(dualengine_default), (gpointer)sb);

  lbl25 = gtk_label_new("Multiprocess executor ");
  gtk_misc_set_alignment(GTK_MISC(lbl25), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabf), lbl25, 2, 3, 3, 4);
  ent25 = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(ent25), 0);
  gtk_entry_set_width_chars(GTK_ENTRY(ent25), 30);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent25), 30);
  gtk_entry_set_text(GTK_ENTRY(ent25), sb->cfg.engine_name[2]);
  gtk_table_attach_defaults(GTK_TABLE(tabf), ent25, 3, 4, 3, 4);
  g_signal_connect(G_OBJECT(ent25), "changed", G_CALLBACK(multiengine_fname), (gpointer)sb);
  g_signal_connect(G_OBJECT(ent25), "draw", G_CALLBACK(multiengine_default), (gpointer)sb);

  lbl29 = gtk_label_new("High throughput multiprocess executor ");
  gtk_misc_set_alignment(GTK_MISC(lbl29), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabf), lbl29, 2, 3, 4, 5);
  ent29 = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(ent29), 0);
  gtk_entry_set_width_chars(GTK_ENTRY(ent29), 30);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent29), 30);
  gtk_entry_set_text(GTK_ENTRY(ent29), sb->cfg.engine_name[3]);
  gtk_table_attach_defaults(GTK_TABLE(tabf), ent29, 3, 4, 4, 5);
  g_signal_connect(G_OBJECT(ent29), "changed", G_CALLBACK(largeengine_fname), (gpointer)sb);
  g_signal_connect(G_OBJECT(ent29), "draw", G_CALLBACK(largeengine_default), (gpointer)sb);

  lbl26 = gtk_label_new("External startup program ");
  gtk_misc_set_alignment(GTK_MISC(lbl26), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabf), lbl26, 2, 3, 5, 6);
  ent26 = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(ent26), 0);
  gtk_entry_set_width_chars(GTK_ENTRY(ent26), 30);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent26), 30);
  gtk_entry_set_text(GTK_ENTRY(ent26), sb->cfg.preext_name);
  gtk_table_attach_defaults(GTK_TABLE(tabf), ent26, 3, 4, 5, 6);
  g_signal_connect(G_OBJECT(ent26), "changed", G_CALLBACK(preext_fname), (gpointer)sb);
  g_signal_connect(G_OBJECT(ent26), "draw", G_CALLBACK(preext_default), (gpointer)sb);

  lbl27 = gtk_label_new("External shutdown program ");
  gtk_misc_set_alignment(GTK_MISC(lbl27), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabf), lbl27, 2, 3, 6, 7);
  ent27 = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(ent27), 0);
  gtk_entry_set_width_chars(GTK_ENTRY(ent27), 30);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent27), 30);
  gtk_entry_set_text(GTK_ENTRY(ent27), sb->cfg.postext_name);
  gtk_table_attach_defaults(GTK_TABLE(tabf), ent27, 3, 4, 6, 7);
  g_signal_connect(G_OBJECT(ent27), "changed", G_CALLBACK(postext_fname), (gpointer)sb);
  g_signal_connect(G_OBJECT(ent27), "draw", G_CALLBACK(postext_default), (gpointer)sb);

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
  gtk_entry_set_width_chars(GTK_ENTRY(ent13), 30);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent13), 30);
  gtk_entry_set_text(GTK_ENTRY(ent13), sb->cfg.prefix);
  gtk_table_attach_defaults(GTK_TABLE(tabg), ent13, 1, 2, 0, 1);
  g_signal_connect(G_OBJECT(ent13), "changed", G_CALLBACK(prefix_fname), (gpointer)sb);
  g_signal_connect(G_OBJECT(ent13), "draw", G_CALLBACK(prefix_default), (gpointer)sb);

  lbl12 = gtk_label_new("I/O file path ");
  gtk_misc_set_alignment(GTK_MISC(lbl12), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabg), lbl12, 0, 1, 1, 2);
  ent12 = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(ent12), 0);
  gtk_entry_set_width_chars(GTK_ENTRY(ent12), 30);
  gtk_entry_set_max_width_chars(GTK_ENTRY(ent12), 30);
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

  tabw = gtk_table_new(3, 3, FALSE);

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

  cb20 = gtk_check_button_new_with_label("Display full names");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb20), sb->cfg.full_names);
  gtk_table_attach_defaults(GTK_TABLE(tabw), cb20, 1, 2, 2, 3);
  g_signal_connect(G_OBJECT(cb20), "clicked", G_CALLBACK(full_names_box), (gpointer)sb);

  cb15 = gtk_check_button_new_with_label("Display unknowns as dots");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb15), sb->cfg.draw_undef);
  gtk_table_attach_defaults(GTK_TABLE(tabw), cb15, 2, 3, 0, 1);
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

  fr3 = gtk_frame_new("Optimizations");
  gtk_label_set_markup(GTK_LABEL(gtk_frame_get_label_widget(GTK_FRAME(fr3))), "<b>Optimizations</b>");
  vbox3 = gtk_vbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(fr3), 5);
  gtk_container_set_border_width(GTK_CONTAINER(vbox3), 5);

  tabh = gtk_table_new(1, 4, FALSE);

  cb16 = gtk_check_button_new_with_label("Optimize joints in intervals");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb16), sb->cfg.seplit_fe);
  gtk_table_attach_defaults(GTK_TABLE(tabh), cb16, 0, 1, 0, 1);
  g_signal_connect(G_OBJECT(cb16), "clicked", G_CALLBACK(seplit_fe_box), (gpointer)sb);

  cb17 = gtk_check_button_new_with_label("Optimize joints in recursions");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb17), sb->cfg.seplit_su);
  gtk_table_attach_defaults(GTK_TABLE(tabh), cb17, 0, 1, 1, 2);
  g_signal_connect(G_OBJECT(cb17), "clicked", G_CALLBACK(seplit_su_box), (gpointer)sb);

  cb13 = gtk_check_button_new_with_label("Optimize delays");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb13), sb->cfg.merge);
  gtk_table_attach_defaults(GTK_TABLE(tabh), cb13, 0, 1, 2, 3);
  g_signal_connect(G_OBJECT(cb13), "clicked", G_CALLBACK(merge_box), (gpointer)sb);

  cb21 = gtk_check_button_new_with_label("Post optimization");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb21), sb->cfg.postopt);
  gtk_table_attach_defaults(GTK_TABLE(tabh), cb21, 0, 1, 3, 4);
  g_signal_connect(G_OBJECT(cb21), "clicked", G_CALLBACK(postopt_box), (gpointer)sb);

  gtk_container_add(GTK_CONTAINER(vbox3), tabh);
  gtk_container_add(GTK_CONTAINER(fr3), vbox3);
  gtk_box_pack_start(GTK_BOX(hbox1), fr3, TRUE, TRUE, 0);

  fr6 = gtk_frame_new("Defaults");
  gtk_label_set_markup(GTK_LABEL(gtk_frame_get_label_widget(GTK_FRAME(fr6))), "<b>Defaults</b>");
  vbox10 = gtk_vbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(fr6), 5);
  gtk_container_set_border_width(GTK_CONTAINER(vbox10), 5);

  tabi = gtk_table_new(1, 3, FALSE);

  cb19 = gtk_check_button_new_with_label("Generate constant signals");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb19), sb->cfg.constout);
  gtk_table_attach_defaults(GTK_TABLE(tabi), cb19, 0, 1, 0, 1);
  g_signal_connect(G_OBJECT(cb19), "clicked", G_CALLBACK(constout_box), (gpointer)sb);

  cb23 = gtk_check_button_new_with_label("Generate suggested defaults");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb23), sb->cfg.constout_sugg);
  gtk_table_attach_defaults(GTK_TABLE(tabi), cb23, 0, 1, 1, 2);
  g_signal_connect(G_OBJECT(cb23), "clicked", G_CALLBACK(constout_sugg_box), (gpointer)sb);

  cb8 = gtk_check_button_new_with_label("Generate user defaults");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb8), sb->cfg.constout_user);
  gtk_table_attach_defaults(GTK_TABLE(tabi), cb8, 0, 1, 2, 3);
  g_signal_connect(G_OBJECT(cb8), "clicked", G_CALLBACK(constout_user_box), (gpointer)sb);

  gtk_container_add(GTK_CONTAINER(vbox10), tabi);
  gtk_container_add(GTK_CONTAINER(fr6), vbox10);
  gtk_box_pack_start(GTK_BOX(hbox1), fr6, TRUE, TRUE, 0);

  fr5 = gtk_frame_new("Miscellaneous");
  gtk_label_set_markup(GTK_LABEL(gtk_frame_get_label_widget(GTK_FRAME(fr5))), "<b>Miscellaneous</b>");
  vbox9 = gtk_vbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(fr5), 5);
  gtk_container_set_border_width(GTK_CONTAINER(vbox9), 5);

  tabz = gtk_table_new(1, 3, FALSE);

  cb22 = gtk_check_button_new_with_label("High throughput");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb22), sb->cfg.hp_io);
  gtk_table_attach_defaults(GTK_TABLE(tabz), cb22, 0, 1, 0, 1);
  g_signal_connect(G_OBJECT(cb22), "clicked", G_CALLBACK(hp_io_box), (gpointer)sb);

  cb12 = gtk_check_button_new_with_label("Wait running");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb12), sb->cfg.busywait);
  gtk_table_attach_defaults(GTK_TABLE(tabz), cb12, 0, 1, 1, 2);
  g_signal_connect(G_OBJECT(cb12), "clicked", G_CALLBACK(busywait_box), (gpointer)sb);

  cb24 = gtk_check_button_new_with_label("Hard real time (root access only)");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb24), sb->cfg.hard);
  gtk_table_attach_defaults(GTK_TABLE(tabz), cb24, 0, 1, 2, 3);
  g_signal_connect(G_OBJECT(cb24), "clicked", G_CALLBACK(hard_box), (gpointer)sb);

  gtk_container_add(GTK_CONTAINER(vbox9), tabz);
  gtk_container_add(GTK_CONTAINER(fr5), vbox9);
  gtk_box_pack_end(GTK_BOX(hbox1), fr5, TRUE, TRUE, 0);

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

void end_plan(GtkWidget *widget, s_base *sb)
{
  sb->plan_window = NULL;
}

void show_plan(GtkWidget *widget, s_base *sb)
{
  GtkWidget *window;
  GtkWidget *drawingarea;
  GtkWidget *extbox, *winbox;
  GtkWidget *frgfx;

  pthread_mutex_lock(&sb->mutex_button);

  if(sb->plan_window)
    {
      gtk_window_present(sb->plan_window);
      pthread_mutex_unlock(&sb->mutex_button);
      return;
    }

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), PLAN_WINDOW_WIDTH, PLAN_WINDOW_HEIGHT);
  gtk_container_set_border_width(GTK_CONTAINER(window), 0);
  gtk_window_set_title(GTK_WINDOW(window), PLAN_TITLE);
  gtk_window_set_icon_name(GTK_WINDOW(window), "tinx");
  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(end_plan), (gpointer)sb);

  sb->plan_window = GTK_WINDOW(window);

  winbox = gtk_vbox_new(FALSE, 0);

  extbox = gtk_vbox_new(FALSE, 0);

  gtk_container_set_border_width(GTK_CONTAINER(extbox), 10);

  frgfx = gtk_frame_new("Trajectories of input and output signals");
  gtk_label_set_markup(GTK_LABEL(gtk_frame_get_label_widget(GTK_FRAME(frgfx))), "<b>Trajectories of input and output signals</b>");
  gtk_frame_set_label_align(GTK_FRAME(frgfx), 0.5, 0.5);

  gtk_container_set_border_width(GTK_CONTAINER(frgfx), 5);

  drawingarea = gtk_drawing_area_new();
  gtk_widget_set_size_request(drawingarea, PLAN_WINDOW_WIDTH, PLAN_WINDOW_HEIGHT);
  g_signal_connect(G_OBJECT(drawingarea), "draw", G_CALLBACK(plan_callback), sb);

  sb->planarea = GTK_DRAWING_AREA(drawingarea);

  gtk_container_add(GTK_CONTAINER(frgfx), drawingarea);

  gtk_box_pack_start(GTK_BOX(extbox), frgfx, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(winbox), extbox, TRUE, TRUE, 0);
  gtk_container_add(GTK_CONTAINER(window), winbox);

  gtk_widget_show_all(window);

  pthread_mutex_unlock(&sb->mutex_button);
}

void end_spectrum(GtkWidget *widget, s_base *sb)
{
  sb->spectrum_window = NULL;
}

void show_spectrum(GtkWidget *widget, s_base *sb)
{
  int tot_rows, page_rows;
  GtkWidget *window;
  GtkWidget *drawingarea;
  GtkWidget *extbox, *winbox;
  GtkWidget *ent0;
  GtkWidget *frgfx, *hboxgfx;
  GtkAdjustment *adj;

  pthread_mutex_lock(&sb->mutex_button);

  if(sb->spectrum_window)
    {
      gtk_window_present(sb->spectrum_window);
      pthread_mutex_unlock(&sb->mutex_button);
      return;
    }

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), SPECTRUM_WINDOW_WIDTH, SPECTRUM_WINDOW_HEIGHT);
  gtk_container_set_border_width(GTK_CONTAINER(window), 0);
  gtk_window_set_title(GTK_WINDOW(window), SPECTRUM_TITLE);
  gtk_window_set_icon_name(GTK_WINDOW(window), "tinx");
  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(end_spectrum), (gpointer)sb);

  sb->spectrum_window = GTK_WINDOW(window);

  winbox = gtk_vbox_new(FALSE, 0);

  extbox = gtk_vbox_new(FALSE, 0);

  gtk_container_set_border_width(GTK_CONTAINER(extbox), 10);

  frgfx = gtk_frame_new("Spectrum of input and output signals");
  gtk_label_set_markup(GTK_LABEL(gtk_frame_get_label_widget(GTK_FRAME(frgfx))), "<b>Spectrum of input and output signals</b>");
  gtk_frame_set_label_align(GTK_FRAME(frgfx), 0.5, 0.5);

  hboxgfx = gtk_hbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(frgfx), 5);
  gtk_container_set_border_width(GTK_CONTAINER(hboxgfx), 5);

  drawingarea = gtk_drawing_area_new();
  gtk_widget_set_size_request(drawingarea, SPECTRUM_WINDOW_WIDTH, SPECTRUM_WINDOW_HEIGHT);
  g_signal_connect(G_OBJECT(drawingarea), "draw", G_CALLBACK(spectrum_callback), sb);

  sb->spectrumarea = GTK_DRAWING_AREA(drawingarea);

  adj = gtk_adjustment_new(0, 0, 1, 1, 1, 1);
  ent0 = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, adj);
  g_signal_connect(G_OBJECT(ent0), "value-changed", G_CALLBACK(spectrum_value), (gpointer)sb);

  tot_rows = max(1, sb->fn + sb->gn);
  page_rows = min(tot_rows, sb->cfg.display_rows);
  gtk_adjustment_configure(adj, 0, 0, tot_rows, 1, page_rows, page_rows);

  sb->spectrum_adj = adj;

  gtk_box_pack_start(GTK_BOX(hboxgfx), drawingarea, TRUE, TRUE, 0);
  gtk_box_pack_end(GTK_BOX(hboxgfx), ent0, FALSE, FALSE, 0);
  gtk_container_add(GTK_CONTAINER(frgfx), hboxgfx);

  gtk_box_pack_start(GTK_BOX(extbox), frgfx, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(winbox), extbox, TRUE, TRUE, 0);
  gtk_container_add(GTK_CONTAINER(window), winbox);

  gtk_widget_show_all(window);

  pthread_mutex_unlock(&sb->mutex_button);
}

void main_window(s_base *sb)
{
  GtkWidget *window;
  GtkWidget *drawingarea;
  GtkWidget *textarea, *scrollarea;
  GtkWidget *btn1, *btn2, *btn3, *btn4, *btn6, *btn7, *btn8;
  GtkWidget *cb0, *cb1, *cb2, *cb3, *cb4, *cb5, *cb6, *cb7, *cb10, *cb14;
  GtkWidget *tabf, *tabi, *tabt, *tabh, *tabx;
  GtkWidget *extbox, *winbox, *hboxctl1, *hboxctl2;
  GtkWidget *ent0, *lbl1, *ent1, *lbl5, *ent5, *lbl6, *ent6, *lbl8, *ent8, *lbl9, *ent9;
  GtkWidget *frgfx, *hboxgfx, *frtxt, *vboxtxt, *fr1, *vbox1, *fr2, *vbox2, *fr3, *vbox3, *fr4, *vbox4, *fr6, *vbox6;
  GtkWidget *vbox5, *hbox3, *hbox5, *hbox6, *lbl0, *lblt, *lblg, *icng;
  GtkWidget *menubar, *menu_items, *menu_items_2, *menu_items_3, *project_menu, *setup_menu, *windows_menu, *open_menu, *gen_menu, *run_menu, *freeze_menu, *config_menu,
            *prob_menu, *filter_menu, *ppsetup_menu, *spectrum_menu, *plan_menu, *opencfg_menu, *save_menu, *saveas_menu, *erase_menu, *help_menu, *about_menu, *quit_menu, *sep1, *sep2;
  GtkTextBuffer *textbuffer;
  GtkAdjustment *adj;
  GdkRGBA color;

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_WIDTH, WINDOW_HEIGHT);
  gtk_container_set_border_width(GTK_CONTAINER(window), 0);
  gtk_window_set_title(GTK_WINDOW(window), TITLE);
  gtk_window_set_icon_name(GTK_WINDOW(window), "tinx");
  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(exit_button_clicked), (gpointer)sb);
  g_signal_connect(G_OBJECT(window), "delete-event", G_CALLBACK(exit_button_clicked_if), (gpointer)sb);

  sb->window = GTK_WINDOW(window);
  sb->config_window = NULL;
  sb->prob_window = NULL;
  sb->filter_window = NULL;
  sb->pplan_window = NULL;
  sb->spectrum_window = NULL;
  sb->plan_window = NULL;

  sb->save_button = NULL;
  sb->erase_button = NULL;
  sb->clear_button = NULL;
  sb->filclear_button = NULL;
  sb->edit_button = NULL;
  sb->help_button = NULL;

  winbox = gtk_vbox_new(FALSE, 0);

  menubar = gtk_menu_bar_new();

  project_menu = gtk_menu_item_new_with_label("Project");
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), project_menu);

  menu_items = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(project_menu), menu_items);

  open_menu = gtk_menu_item_new_with_label("Open...");
  gen_menu = gtk_menu_item_new_with_label("Generate network");
  run_menu = gtk_menu_item_new_with_label("Execute network");
  freeze_menu = gtk_menu_item_new_with_label("Pause execution");

  sb->gen_menu = GTK_MENU_ITEM(gen_menu);

  if(*sb->cfg.compiler_name)
    gtk_widget_set_sensitive(GTK_WIDGET(sb->gen_menu), TRUE);
  else
    gtk_widget_set_sensitive(GTK_WIDGET(sb->gen_menu), FALSE);

  sb->run_menu = GTK_MENU_ITEM(run_menu);
  
  if(*sb->cfg.engine_name[0] && *sb->cfg.engine_name[1] && *sb->cfg.engine_name[2] && *sb->cfg.engine_name[3])
    gtk_widget_set_sensitive(GTK_WIDGET(sb->run_menu), TRUE);
  else
    gtk_widget_set_sensitive(GTK_WIDGET(sb->run_menu), FALSE);
  
  sb->freeze_menu = GTK_MENU_ITEM(freeze_menu);

  gtk_widget_set_sensitive(freeze_menu, FALSE);

  sep1 = gtk_separator_menu_item_new();

  help_menu = gtk_menu_item_new_with_label("Help...");

  sb->help_menu = GTK_MENU_ITEM(help_menu);

  if(*sb->cfg.viewer_name)
    gtk_widget_set_sensitive(help_menu, TRUE);
  else
    gtk_widget_set_sensitive(help_menu, FALSE);

  about_menu = gtk_menu_item_new_with_label("About...");
  quit_menu = gtk_menu_item_new_with_label("Exit");

  setup_menu = gtk_menu_item_new_with_label("Configuration");
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), setup_menu);

  menu_items_2 = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(setup_menu), menu_items_2);

  config_menu = gtk_menu_item_new_with_label("Configure...");
  prob_menu = gtk_menu_item_new_with_label("Probabilities...");
  filter_menu = gtk_menu_item_new_with_label("External signals...");
  ppsetup_menu = gtk_menu_item_new_with_label("Phase plan setup...");

  sep2 = gtk_separator_menu_item_new();

  save_menu = gtk_menu_item_new_with_label("Save configuration");
  opencfg_menu = gtk_menu_item_new_with_label("Open configuration...");
  saveas_menu = gtk_menu_item_new_with_label("Save configuration as...");
  erase_menu = gtk_menu_item_new_with_label("Clear configuration");

  sb->save_menu = GTK_MENU_ITEM(save_menu);
  sb->erase_menu = GTK_MENU_ITEM(erase_menu);

  windows_menu = gtk_menu_item_new_with_label("Display");
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), windows_menu);

  menu_items_3 = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(windows_menu), menu_items_3);

  spectrum_menu = gtk_menu_item_new_with_label("Spectrum analyzer...");
  plan_menu = gtk_menu_item_new_with_label("Phase plan...");
  
  gtk_widget_set_sensitive(save_menu, FALSE);

  if(sb->configured)
    gtk_widget_set_sensitive(erase_menu, TRUE);
  else
    gtk_widget_set_sensitive(erase_menu, FALSE);

  gtk_menu_shell_append(GTK_MENU_SHELL(menu_items), open_menu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_items), gen_menu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_items), run_menu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_items), freeze_menu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_items), sep1);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_items), help_menu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_items), about_menu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_items), quit_menu);

  gtk_menu_shell_append(GTK_MENU_SHELL(menu_items_2), config_menu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_items_2), prob_menu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_items_2), filter_menu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_items_2), ppsetup_menu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_items_2), sep2);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_items_2), opencfg_menu);  
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_items_2), save_menu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_items_2), saveas_menu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_items_2), erase_menu);

  gtk_menu_shell_append(GTK_MENU_SHELL(menu_items_3), spectrum_menu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_items_3), plan_menu);
  
  g_signal_connect(open_menu, "activate", G_CALLBACK(source_dialog_if), (gpointer)sb);
  g_signal_connect(gen_menu, "activate", G_CALLBACK(gen_button_clicked), (gpointer)sb);
  g_signal_connect(run_menu, "activate", G_CALLBACK(run_button_clicked), (gpointer)sb);
  g_signal_connect(freeze_menu, "activate", G_CALLBACK(freeze_button_clicked), (gpointer)sb);
  g_signal_connect(help_menu, "activate", G_CALLBACK(help_button_clicked), (gpointer)sb);
  g_signal_connect(about_menu, "activate", G_CALLBACK(about_button_clicked), (gpointer)sb);
  g_signal_connect(quit_menu, "activate", G_CALLBACK(exit_button_clicked), (gpointer)sb);

  g_signal_connect(config_menu, "activate", G_CALLBACK(configure), (gpointer)sb);
  g_signal_connect(prob_menu, "activate", G_CALLBACK(prob_button_clicked), (gpointer)sb);
  g_signal_connect(filter_menu, "activate", G_CALLBACK(filter_button_clicked), (gpointer)sb);
  g_signal_connect(ppsetup_menu, "activate", G_CALLBACK(pplan_button_clicked), (gpointer)sb);
  g_signal_connect(opencfg_menu, "activate", G_CALLBACK(cfg_dialog_if), (gpointer)sb);
  g_signal_connect(save_menu, "activate", G_CALLBACK(save_button_clicked), (gpointer)sb);
  g_signal_connect(saveas_menu, "activate", G_CALLBACK(saveas_dialog_if), (gpointer)sb);
  g_signal_connect(erase_menu, "activate", G_CALLBACK(erase_button_clicked), (gpointer)sb);

  g_signal_connect(spectrum_menu, "activate", G_CALLBACK(show_spectrum), (gpointer)sb);
  g_signal_connect(plan_menu, "activate", G_CALLBACK(show_plan), (gpointer)sb);

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
  g_signal_connect(G_OBJECT(drawingarea), "draw", G_CALLBACK(draw_callback), sb);
  gtk_widget_add_tick_callback(drawingarea, (gboolean (*)(GtkWidget *, GdkFrameClock *, gpointer))tick_callback, sb, NULL);

  sb->drawingarea = GTK_DRAWING_AREA(drawingarea);

  adj = gtk_adjustment_new(0, 0, 1, 1, 1, 1);
  ent0 = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, adj);
  g_signal_connect(G_OBJECT(ent0), "value-changed", G_CALLBACK(pos_value), (gpointer)sb);

  sb->area_adj = adj;

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
  textarea = gtk_text_view_new();
  gtk_text_view_set_editable(GTK_TEXT_VIEW(textarea), FALSE);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(textarea), FALSE);
  gtk_text_view_set_buffer(GTK_TEXT_VIEW(textarea), textbuffer);
  scrollarea = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_set_size_request(scrollarea, WINDOW_WIDTH, TEXT_HEIGHT);
  gtk_container_add(GTK_CONTAINER(scrollarea), textarea);

  sb->textarea = GTK_TEXT_VIEW(textarea);

  gtk_style_context_get_background_color(gtk_widget_get_style_context(textarea), GTK_STATE_FLAG_NORMAL, &color);

  sb->fancymsg = (color.red + color.green + color.blue < 1.5) ? 2 : 1;

  sb->donotscroll = TRUE;
  fancyprint(sb, BANNER);

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
  gtk_entry_set_text(GTK_ENTRY(ent1), sb->cfg.source_name);
  gtk_table_attach_defaults(GTK_TABLE(tabf), ent1, 1, 2, 0, 1);

  hbox6 = gtk_hbox_new(FALSE, 15);
  btn6 = gtk_button_new_with_label("Edit...");
  gtk_box_pack_end(GTK_BOX(hbox6), btn6, FALSE, FALSE, 5);
  gtk_table_attach_defaults(GTK_TABLE(tabf), hbox6, 2, 3, 0, 1);
  g_signal_connect(G_OBJECT(btn6), "clicked", G_CALLBACK(edit_button_clicked), (gpointer)sb);

  sb->edit_button = GTK_BUTTON(btn6);

  if(*sb->cfg.editor_name)
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
  g_signal_connect(G_OBJECT(ent1), "changed", G_CALLBACK(source_fname), (gpointer)sb);
  g_signal_connect(G_OBJECT(ent1), "draw", G_CALLBACK(source_default), (gpointer)sb);
  g_signal_connect(G_OBJECT(ent1), "icon-press", G_CALLBACK(source_dialog), (gpointer)sb);

  sb->reg_warning = GTK_LABEL(lblg);
  sb->reg_warning_icon = GTK_IMAGE(icng);

  cb1 = gtk_check_button_new_with_label("Load initial conditions");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb1), sb->cfg.load_state);
  gtk_table_attach_defaults(GTK_TABLE(tabf), cb1, 1, 2, 1, 2);
  g_signal_connect(G_OBJECT(cb1), "clicked", G_CALLBACK(load_state_box), (gpointer)sb);

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
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(ent5), sb->cfg.step);
  gtk_table_attach_defaults(GTK_TABLE(tabt), ent5, 1, 2, 0, 1);
  g_signal_connect(G_OBJECT(ent5), "value-changed", G_CALLBACK(step_value), (gpointer)sb);

  lbl6 = gtk_label_new("Horizon length (0 = none) ");
  gtk_misc_set_alignment(GTK_MISC(lbl6), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabt), lbl6, 0, 1, 1, 2);
  ent6 = gtk_spin_button_new_with_range(0, MAX_RUN_LEN, 1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(ent6), sb->cfg.max_time);
  gtk_table_attach_defaults(GTK_TABLE(tabt), ent6, 1, 2, 1, 2);
  g_signal_connect(G_OBJECT(ent6), "value-changed", G_CALLBACK(max_time_value), (gpointer)sb);

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
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb2), sb->cfg.soundness_check);
  gtk_table_attach_defaults(GTK_TABLE(tabi), cb2, 0, 1, 0, 1);
  g_signal_connect(G_OBJECT(cb2), "clicked", G_CALLBACK(soundness_check_box), (gpointer)sb);

  cb3 = gtk_check_button_new_with_label("Causal inference only");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb3), sb->cfg.strictly_causal);
  gtk_table_attach_defaults(GTK_TABLE(tabi), cb3, 0, 1, 1, 2);
  g_signal_connect(G_OBJECT(cb3), "clicked", G_CALLBACK(strictly_causal_box), (gpointer)sb);

  cb10 = gtk_check_button_new_with_label("Use symbol table");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb10), sb->cfg.use_xref);
  gtk_table_attach_defaults(GTK_TABLE(tabi), cb10, 0, 1, 2, 3);
  g_signal_connect(G_OBJECT(cb10), "clicked", G_CALLBACK(use_xref_box), (gpointer)sb);

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
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb0), !sb->cfg.quiet && !sb->cfg.file_io);
  gtk_table_attach_defaults(GTK_TABLE(tabh), cb0, 0, 1, 0, 1);

  cb7 = gtk_radio_button_new_with_label(gtk_radio_button_get_group(GTK_RADIO_BUTTON(cb0)), "File input and output");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb7), sb->cfg.file_io);
  gtk_table_attach_defaults(GTK_TABLE(tabh), cb7, 0, 1, 1, 2);
  g_signal_connect(G_OBJECT(cb7), "clicked", G_CALLBACK(file_io_box), (gpointer)sb);

  cb6 = gtk_radio_button_new_with_label(gtk_radio_button_get_group(GTK_RADIO_BUTTON(cb7)), "Quiet mode");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb6), sb->cfg.quiet);
  gtk_table_attach_defaults(GTK_TABLE(tabh), cb6, 0, 1, 2, 3);
  g_signal_connect(G_OBJECT(cb6), "clicked", G_CALLBACK(quiet_box), (gpointer)sb);

  cb4 = gtk_check_button_new_with_label("Trace inference");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb4), sb->cfg.echo_stdout);
  gtk_table_attach_defaults(GTK_TABLE(tabh), cb4, 1, 2, 0, 1);
  g_signal_connect(G_OBJECT(cb4), "clicked", G_CALLBACK(echo_stdout_box), (gpointer)sb);

  cb5 = gtk_check_button_new_with_label("Log inference to file");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb5), sb->cfg.echo_logfile);
  gtk_table_attach_defaults(GTK_TABLE(tabh), cb5, 1, 2, 1, 2);
  g_signal_connect(G_OBJECT(cb5), "clicked", G_CALLBACK(echo_logfile_box), (gpointer)sb);

  cb14 = gtk_check_button_new_with_label("Display auxiliary signals");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb14), sb->cfg.outaux);
  gtk_table_attach_defaults(GTK_TABLE(tabh), cb14, 1, 2, 2, 3);
  g_signal_connect(G_OBJECT(cb14), "clicked", G_CALLBACK(outaux_box), (gpointer)sb);

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
  gtk_range_set_value(GTK_RANGE(ent8), sb->cfg.prob);
  gtk_table_attach_defaults(GTK_TABLE(tabx), ent8, 1, 2, 0, 1);
  g_object_set(G_OBJECT(ent8), "width-request", BAR_WIDTH, NULL);
  g_signal_connect(G_OBJECT(ent8), "value-changed", G_CALLBACK(prob_value), (gpointer)sb);

  lbl9 = gtk_label_new("Time correction ");
  gtk_misc_set_alignment(GTK_MISC(lbl9), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabx), lbl9, 0, 1, 1, 2);
  ent9 = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, -1, 1, 0.01);
  gtk_scale_set_value_pos(GTK_SCALE(ent9), GTK_POS_LEFT);
  gtk_scale_add_mark(GTK_SCALE(ent9), 0,  GTK_POS_TOP, NULL);
  gtk_scale_add_mark(GTK_SCALE(ent9), 0,  GTK_POS_BOTTOM, NULL);
  gtk_range_set_value(GTK_RANGE(ent9), sb->cfg.correction);
  gtk_table_attach_defaults(GTK_TABLE(tabx), ent9, 1, 2, 1, 2);
  g_object_set(G_OBJECT(ent9), "width-request", BAR_WIDTH, NULL);
  g_signal_connect(G_OBJECT(ent9), "value-changed", G_CALLBACK(correction_value), (gpointer)sb);

  lbl0 = gtk_label_new("Elapsed time ");
  gtk_misc_set_alignment(GTK_MISC(lbl0), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabx), lbl0, 0, 1, 2, 3);
  lblt = gtk_label_new("0 (0.000000 s) ~ 0.000000 s");
  gtk_misc_set_alignment(GTK_MISC(lblt), 1, 0.5);
  gtk_table_attach_defaults(GTK_TABLE(tabx), lblt, 1, 2, 2, 3);

  sb->timer = GTK_LABEL(lblt);

  gtk_container_add(GTK_CONTAINER(vbox6), tabx);
  gtk_container_add(GTK_CONTAINER(fr6), vbox6);
  gtk_box_pack_end(GTK_BOX(hboxctl2), fr6, TRUE, TRUE, 0);

  vbox5 = gtk_vbox_new(FALSE, 15);
  hbox5 = gtk_hbox_new(FALSE, 15);

  btn1 = gtk_button_new_with_label("Generate network");
  gtk_widget_set_size_request(btn1, 70, 30);
  gtk_box_pack_start(GTK_BOX(hbox5), btn1, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(btn1), "clicked", G_CALLBACK(gen_button_clicked), (gpointer)sb);

  sb->gen_button = GTK_BUTTON(btn1);

  if(*sb->cfg.compiler_name)
    gtk_widget_set_sensitive(GTK_WIDGET(sb->gen_button), TRUE);
  else
    gtk_widget_set_sensitive(GTK_WIDGET(sb->gen_button), FALSE);

  btn2 = gtk_button_new_with_label("Execute network");
  gtk_widget_set_size_request(btn2, 70, 30);
  gtk_box_pack_start(GTK_BOX(hbox5), btn2, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(btn2), "clicked", G_CALLBACK(run_button_clicked), (gpointer)sb);

  sb->run_button = GTK_BUTTON(btn2);

  if(*sb->cfg.engine_name[0] && *sb->cfg.engine_name[1] && *sb->cfg.engine_name[2])
    gtk_widget_set_sensitive(GTK_WIDGET(sb->run_button), TRUE);
  else
    gtk_widget_set_sensitive(GTK_WIDGET(sb->run_button), FALSE);

  btn8 = gtk_button_new_with_label("Pause execution");
  gtk_widget_set_size_request(btn8, 70, 30);
  gtk_box_pack_start(GTK_BOX(hbox5), btn8, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(btn8), "clicked", G_CALLBACK(freeze_button_clicked), (gpointer)sb);

  sb->freeze_button = GTK_BUTTON(btn8);

  gtk_widget_set_sensitive(btn8, FALSE);

  btn3 = gtk_button_new_with_label("Exit");
  gtk_widget_set_size_request(btn3, 70, 30);
  gtk_box_pack_end(GTK_BOX(hbox5), btn3, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(btn3), "clicked", G_CALLBACK(exit_button_clicked), (gpointer)sb);

  btn4 = gtk_button_new_with_label("Configure...");
  gtk_widget_set_size_request(btn4, 70, 30);
  gtk_box_pack_end(GTK_BOX(hbox5), btn4, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(btn4), "clicked", G_CALLBACK(configure), (gpointer)sb);

  btn7 = gtk_button_new_with_label("Help...");
  gtk_widget_set_size_request(btn7, 70, 30);
  gtk_box_pack_end(GTK_BOX(hbox5), btn7, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(btn7), "clicked", G_CALLBACK(help_button_clicked), (gpointer)sb);

  sb->help_button = GTK_BUTTON(btn7);

  if(*sb->cfg.viewer_name)
    gtk_widget_set_sensitive(btn7, TRUE);
  else
    gtk_widget_set_sensitive(btn7, FALSE);

  gtk_box_pack_end(GTK_BOX(vbox5), hbox5, FALSE, FALSE, 0);

  gtk_box_pack_start(GTK_BOX(extbox), frgfx, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(extbox), frtxt, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(extbox), hboxctl1, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(extbox), hboxctl2, FALSE, FALSE, 0);
  gtk_box_pack_end(GTK_BOX(extbox), vbox5, FALSE, FALSE, 0);

  gtk_box_pack_end(GTK_BOX(winbox), extbox, TRUE, TRUE, 0);

  gtk_container_add(GTK_CONTAINER(window), winbox);

  gtk_widget_show_all(window);
}

int execute(char *source_name, char *base_name, char *state_name, char *logfile_name, char *xref_name,
         bool strictly_causal, bool soundness_check, bool echo_stdout, bool file_io, bool quiet, bool hard, bool sys5, bool sturdy, bool busywait, bool seplit_fe, bool seplit_su, bool merge, bool postopt,
         bool outaux, bool outint, int bufexp, d_time max_time, m_time step, char *prefix, char *path, char *include_path, char *alpha, int num_threads, double prob,
         bool batch_in, bool batch_out, bool draw_undef)
{
  s_base *sb;
  FILE *fp;
  int i, j;
#if defined BUGGED_PTHREADS
  pthread_attr_t attributes;
#endif

  sb = malloc(sizeof(s_base));
  memset(sb, 0, sizeof(s_base));

  fp = fopen(CONFIG_FILENAME, "r");
  if(fp)
    {
      if(load_config(fp, &sb->cfg))
        sb->configured = FALSE;
      else
        sb->configured = TRUE;

      fclose(fp);
    }
  else
    sb->configured = FALSE;

  if(!sb->configured)
    {
      strcpy(sb->cfg.source_name, source_name);

      if(!base_name)
        base_name = source_name;

      strcpy(sb->cfg.base_name, base_name);

      if(state_name)
        {
          strcpy(sb->cfg.state_name, state_name);
          sb->cfg.load_state = TRUE;
        }
      else
        {
          strcpy(sb->cfg.state_name, base_name);
          strcat(sb->cfg.state_name, STATE_SUFFIX);
          sb->cfg.load_state = FALSE;
        }

      if(logfile_name)
        {
          strcpy(sb->cfg.logfile_name, logfile_name);
          sb->cfg.echo_logfile = TRUE;
        }
      else
        {
          strcpy(sb->cfg.logfile_name, base_name);
          strcat(sb->cfg.logfile_name, LOG_SUFFIX);
          sb->cfg.echo_logfile = FALSE;
        }

      if(xref_name)
        {
          strcpy(sb->cfg.xref_name, xref_name);
          sb->cfg.use_xref = TRUE;
        }
      else
        {
          strcpy(sb->cfg.xref_name, base_name);
          sb->cfg.use_xref = FALSE;
        }

      sb->cfg.bsbt = bufexp;
      sb->cfg.step = step;
      sb->cfg.max_time = max_time;
      sb->cfg.num_threads = num_threads;
      sb->cfg.hp_io = FALSE;
      sb->cfg.prob = prob;
      sb->cfg.correction = 0;
      strcpy(sb->cfg.prefix, prefix);
      strcpy(sb->cfg.path, path);
      strcpy(sb->cfg.include_path, include_path);
      strncpy(sb->cfg.alpha, alpha, end_symbol - false_symbol + 1);
      sb->cfg.strictly_causal = strictly_causal;
      sb->cfg.soundness_check = soundness_check;
      sb->cfg.echo_stdout = echo_stdout;
      sb->cfg.file_io = file_io;
      sb->cfg.quiet = quiet;
      sb->cfg.hard = hard;
      sb->cfg.sys5 = sys5;
      sb->cfg.sturdy = sturdy;
      sb->cfg.busywait = busywait;
      sb->cfg.seplit_fe = seplit_fe;
      sb->cfg.seplit_su = seplit_su;
      sb->cfg.merge = merge;
      sb->cfg.postopt = postopt;
      sb->cfg.outaux = outaux;
      sb->cfg.outint = outint;
      sb->cfg.batch_in = batch_in;
      sb->cfg.batch_out = batch_out;
      sb->cfg.draw_undef = draw_undef;
      sb->cfg.full_names = FALSE;
      sb->cfg.horizon_size = DEFAULT_HORIZON_SIZE;
      sb->cfg.display_rows = DEFAULT_DISPLAY_ROWS;
      strcpy(sb->cfg.editor_name, EDITOR_FILENAME);
      strcpy(sb->cfg.viewer_name, VIEWER_FILENAME);
      strcpy(sb->cfg.compiler_name, COMPILER_FILENAME);
      strcpy(sb->cfg.engine_name[0], MONOENGINE_FILENAME);
      strcpy(sb->cfg.engine_name[1], DUALENGINE_FILENAME);
      strcpy(sb->cfg.engine_name[2], MULTIENGINE_FILENAME);
      strcpy(sb->cfg.engine_name[3], LARGEENGINE_FILENAME);
      for(i = 0; i < MAX_FILES; i++)
        {
          sb->cfg.inprob[i] = 1;
          sb->cfg.fexcl[i] = FALSE;
          sb->cfg.gexcl[i] = FALSE;
          *sb->cfg.fname[i] = '\0';
          *sb->cfg.gname[i] = '\0';

          for(j = 0; j < MAX_FILES; j++)
            sb->cfg.pplan[i][j] = FALSE;
        }

      sb->cfg.fn = 0;
      sb->cfg.gn = 0;
    }

  sb->cp_step = step;
  sb->cp_max_time = max_time;
  sb->cp_echo_stdout = echo_stdout;
  sb->cp_file_io = file_io;
  sb->cp_quiet = quiet;
  sb->cp_sys5 = sys5;
  sb->cp_batch_in = batch_in;
  sb->cp_batch_out = batch_out;
  sb->cp_full_names = FALSE;
  sb->cp_horizon_size = DEFAULT_HORIZON_SIZE;
  sb->cp_display_rows = DEFAULT_DISPLAY_ROWS;

  sb->fn = 0;
  sb->gn = 0;
  sb->pos = 0;
  sb->t = 0;
  sb->time = 0;
  sb->time_base = 0;
  sb->maxlen = 0;
  sb->xstart = 0;
  sb->xend = 0;
  sb->xcat = FALSE;
  sb->version = 0;
  sb->rs = stopped;
  sb->rsbu = stopped;
  sb->rsreq[0] = invalid;
  sb->startreq = 0;
  sb->endreq = 0;
  sb->processing = FALSE;
  sb->error = FALSE;
  sb->term = FALSE;
  sb->waitio = FALSE;
  sb->regenerate = FALSE;
  sb->changed = FALSE;
  sb->changeprob = FALSE;
  sb->changefilter = FALSE;
  sb->changepp = FALSE;
  sb->donotquit = FALSE;
  sb->donotscroll = FALSE;

#if defined BUGGED_PTHREADS
  sb->done = FALSE;

  pthread_attr_init(&attributes);

  if(pthread_create(&sb->tinxpipe, &attributes, (void *)&tinxpipe, (void *)sb))
      {
        fprintf(stderr, "POSIX thread error");
        return EXIT_FAILURE;
      }
#endif

  gtk_disable_setlocale();

  gtk_init(NULL, NULL);

  main_window(sb);

  pthread_mutex_init(&sb->mutex_xbuffer, NULL);
  pthread_mutex_init(&sb->mutex_button, NULL);
  pthread_mutex_init(&sb->mutex_request, NULL);
  
#if defined BUGGED_PTHREADS
  pthread_mutex_init(&sb->mutex_pipe, NULL);

/*  pthread_cond_init(&sb->cond_pipe, NULL); */
#endif

  gtk_main();

#if defined BUGGED_PTHREADS
/*  pthread_cond_destroy(&sb->cond_pipe); */

  pthread_mutex_destroy(&sb->mutex_pipe);
#endif

  pthread_mutex_destroy(&sb->mutex_request);
  pthread_mutex_destroy(&sb->mutex_button);
  pthread_mutex_destroy(&sb->mutex_xbuffer);

  free(sb);

  return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
  char *source_name, *base_name, *state_name, *logfile_name, *xref_name, *option, *ext, *prefix, *path, *include_path;
  char default_state_name[MAX_STRLEN], default_logfile_name[MAX_STRLEN], default_xref_name[MAX_STRLEN], alpha[SYMBOL_NUMBER + 1];
  bool strictly_causal, soundness_check, echo_stdout, file_io, quiet, hard, sys5, sturdy, busywait, seplit_fe, seplit_su, merge, postopt, outaux, outint, batch_in, batch_out, draw_undef;
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
  strictly_causal = soundness_check = echo_stdout = file_io = quiet = hard = sys5 = sturdy = busywait = seplit_fe = seplit_su = merge = postopt =
                    outaux = outint = batch_in = batch_out = draw_undef = FALSE;
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
         strictly_causal, soundness_check, echo_stdout, file_io, quiet, hard, sys5, sturdy, busywait, seplit_fe, seplit_su, merge, postopt, outaux, outint,
         bufexp, max_time, step, prefix, path, include_path, alpha, num_threads, prob, batch_in, batch_out, draw_undef);
}


