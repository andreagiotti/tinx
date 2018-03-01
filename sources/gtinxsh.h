
/*
  GTINXSH - Temporal Inference Network eXecutor Suite
  Design & coding by Andrea Giotti, 1998-1999
  Revised 2016-2018
*/

#include "ting_core.h"
#include <gtk/gtk.h>

#pragma GCC diagnostic ignored "-Wdeprecated"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#define CMD_PATH ""

#define MAX_STRLEN_IF 512
#define MAX_STRLEN_IF_C "511"

#define MAX_FILES 256
#define HORIZON_SIZE 81
#define MAX_RUN_LEN 1000000
#define MIN_BSBT 2
#define MAX_BSBT 30

#define SKIP_FMT "%*[^?!]"

#define DISPLAY_LO_CHAR '-'
#define DISPLAY_HI_CHAR '#'
#define DISPLAY_UNKNOWN_CHAR ' '

#define DEFAULT_PROB ((float) 0.5)

#define TITLE "Temporal Inference Network eXecutor Suite"
#define BANNER "Temporal Inference Network eXecutor Suite "PACK_VER", graphical shell "VER"\n" \
               "Design & coding by Andrea Giotti, 1998-1999, 2016-2018\n\n" \
               "A real time inference engine for temporal logic specifications, which is able to process and generate any binary signal through POSIX IPC or files.\n" \
               "Specifications of signals are represented as special graphs and executed in real time, with a sampling time of few milliseconds.\n" \
               "The accepted language provides timed logic operators, conditional operators, interval operators, bounded quantifiers and parametrization of signals.\n\n" \
               "This software is licensed under the GNU Public License."

#define REAL_FMT "%.3f"

#define XBUFSIZE 262144

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 750
#define GRAPHICS_HEIGHT (WINDOW_HEIGHT / 3)
#define TEXT_HEIGHT (WINDOW_HEIGHT / 5)
#define BAR_WIDTH (WINDOW_WIDTH / 6)

#define BORDER_TRUE 0.2
#define BORDER_FALSE 0.4
#define FONT_SIZE 0.8
#define BALL_RATIO 0.5
#define LABEL_LEN 24
#define ASPECT 0.6
#define MAX_FONT_PIXELS 16

#define DELAY 1

#define round(x) floor(x + 0.5)
#define print_error(SB, A) print(SB, "%s: %s\n", A, strerror(errno))

typedef enum runstate
  {
    stopped,
    starting,
    started,
    stopping
  } runstate;

typedef struct s_base
  {
    char memory_f[MAX_FILES][HORIZON_SIZE];
    char memory_g[MAX_FILES][HORIZON_SIZE];
    int fn;
    int gn;
    d_time t;
    m_time time;
    char fnames[MAX_FILES][MAX_STRLEN];
    char gnames[MAX_FILES][MAX_STRLEN];
    int maxlen;
    file fp[MAX_FILES];
    file gp[MAX_FILES];
    channel cp[MAX_FILES];
    channel dp[MAX_FILES];
    char xbuffer[XBUFSIZE];
    int xstart;
    int xend;
    bool xcat;
    bool mt;
    char cmd[MAX_STRLEN_IF];
    char source_name[MAX_STRLEN];
    char base_name[MAX_STRLEN];
    char state_name[MAX_STRLEN];
    char logfile_name[MAX_STRLEN];
    char xref_name[MAX_STRLEN];
    int bsbt;
    m_time step;
    m_time cp_step;
    d_time max_time;
    d_time cp_max_time;
    int num_threads;
    float prob;
    float correction;
    bool load_state;
    bool strictly_causal;
    bool soundness_check;
    bool echo_stdout;
    bool cp_echo_stdout;
    bool echo_logfile;
    bool file_io;
    bool cp_file_io;
    bool quiet;
    bool cp_quiet;
    bool hard;
    bool sturdy;
    bool use_xref;
    bool batch;
    runstate rs;
    bool term;
    pthread_t tintloop;
    pthread_t tinxpipe;
    pthread_mutex_t mutex;
    GtkWindow *window;
    GtkTextView *textarea;
    GtkButton *run_button;
    GtkLabel *timer;
  } s_base;

/* Protos */

INLINE m_time get_time();
void plot(GtkWidget *widget, cairo_t *cr, s_base *sb, int x, int y, char truth);
gboolean draw_callback(GtkWidget *widget, cairo_t *cr, s_base *sb);
gboolean tick_callback(GtkWidget *widget, GdkFrameClock *frame_clock, s_base *sb);
void tintloop(s_base *sb);
void tinxpipe(s_base *sb);
void run_button_clicked(GtkWidget *widget, s_base *sb);
void gen_button_clicked(GtkWidget *widget, s_base *sb);
void about_button_clicked(GtkWidget *widget, s_base *sb);
void exit_button_clicked(GtkWidget *widget, s_base *sb);
void load_state_box(GtkWidget *widget, s_base *sb);
void soundness_check_box(GtkWidget *widget, s_base *sb);
void strictly_causal_box(GtkWidget *widget, s_base *sb);
void echo_stdout_box(GtkWidget *widget, s_base *sb);
void echo_logfile_box(GtkWidget *widget, s_base *sb);
void file_io_box(GtkWidget *widget, s_base *sb);
void quiet_box(GtkWidget *widget, s_base *sb);
void hard_box(GtkWidget *widget, s_base *sb);
void sturdy_box(GtkWidget *widget, s_base *sb);
void source_fname(GtkWidget *widget, s_base *sb);
void base_fname(GtkWidget *widget, s_base *sb);
void state_fname(GtkWidget *widget, s_base *sb);
void logfile_fname(GtkWidget *widget, s_base *sb);
void xref_fname(GtkWidget *widget, s_base *sb);
gboolean source_default(GtkWidget *widget, cairo_t *cr, s_base *sb);
gboolean base_default(GtkWidget *widget, cairo_t *cr, s_base *sb);
gboolean state_default(GtkWidget *widget, cairo_t *cr, s_base *sb);
gboolean logfile_default(GtkWidget *widget, cairo_t *cr, s_base *sb);
gboolean xref_default(GtkWidget *widget, cairo_t *cr, s_base *sb);
void source_dialog(GtkEntry *widget, GtkEntryIconPosition pos, GdkEvent *event, s_base *sb);
void base_dialog(GtkEntry *widget, GtkEntryIconPosition pos, GdkEvent *event, s_base *sb);
void state_dialog(GtkEntry *widget, GtkEntryIconPosition pos, GdkEvent *event, s_base *sb);
void logfile_dialog(GtkEntry *widget, GtkEntryIconPosition pos, GdkEvent *event, s_base *sb);
void xref_dialog(GtkEntry *widget, GtkEntryIconPosition pos, GdkEvent *event, s_base *sb);
void step_value(GtkWidget *widget, s_base *sb);
void max_time_value(GtkWidget *widget, s_base *sb);
void num_threads_value(GtkWidget *widget, s_base *sb);
void prob_value(GtkWidget *widget, s_base *sb);
void correction_value(GtkWidget *widget, s_base *sb);
gboolean update_view(s_base *sb);
gboolean reset_view(s_base *sb);
void print(s_base *sb, char *string, ...);
void print_add(s_base *sb, char *string, ...);
pid_t pidof(s_base *sb, char *name);
int execute(char *source_name, char *base_name, char *state_name, char *logfile_name, char *xref_name,
    bool strictly_causal, bool soundness_check, bool echo_stdout, bool file_io, bool quiet, bool hard, bool sturdy, int bufexp, int max_time, m_time step, int num_threads, float prob, bool batch);


