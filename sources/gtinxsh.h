
/*
  GTINXSH - Temporal Inference Network eXecutor Suite
  Design & coding by Andrea Giotti, 1998-1999
  Revised 2016-2019
*/

#include "ting_core.h"
#include <gtk/gtk.h>

#pragma GCC diagnostic ignored "-Wdeprecated"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#define CMD_PATH ""
#define CONFIG_FILENAME ".gtinxshrc"
#define EDITOR_FILENAME "gedit"
#define VIEWER_FILENAME "evince"
#define HELP_FILENAME "/usr/share/doc/tinx/reference.pdf"

#define MAX_STRLEN_IF 512
#define MAX_STRLEN_IF_C "511"

#define MAX_FILES 1024
#define DEFAULT_HORIZON_SIZE (100 + 1)
#define MAX_HORIZON_SIZE (255 + 1)
#define MAX_RUN_LEN 999999999999
#define MIN_BSBT 2
#define MAX_BSBT 30

#define SKIP_FMT "%*[^.?!]"

#define DISPLAY_LO_CHAR '0'
#define DISPLAY_HI_CHAR '1'
#define DISPLAY_UNKNOWN_CHAR '?'
#define DISPLAY_EMPTY_CHAR ' '

#define DEFAULT_DISPLAY_ROWS 15
#define MAX_DISPLAY_ROWS 255

#define DEFAULT_PROB ((double) 0.5)

#define TITLE "Temporal Inference Network eXecutor Suite"
#define CONFIG_TITLE "Configuration"
#define PROB_TITLE "Probabilities"
#define FILTER_TITLE "External signals"
#define BANNER "Temporal Inference Network eXecutor Suite "PACK_VER", graphical shell "VER"\n" \
               "Design & coding by Andrea Giotti, 1998-1999, 2016-2020\n\n" \
               "A real time inference engine for temporal logic specifications, which is able to process and generate any binary signal through POSIX IPC or files.\n" \
               "Specifications of signals are represented as special graphs and executed in real time, with a sampling time of few milliseconds.\n" \
               "The accepted language provides timed logic operators, conditional operators, interval operators, bounded quantifiers and parametrization of signals.\n\n" \
               "This software is licensed under the GNU Public License.\n"

#define REAL_FMT "%.6f"
#define ORIGIN_FMT "%.9f"

#define XBUFSIZE 262144

#define WINDOW_WIDTH 768
#define WINDOW_HEIGHT 512
#define CONFIG_WINDOW_WIDTH 512
#define CONFIG_WINDOW_HEIGHT 512
#define PROB_WINDOW_WIDTH 128
#define PROB_WINDOW_HEIGHT 64
#define FILTER_WINDOW_WIDTH 128
#define FILTER_WINDOW_HEIGHT 64

#define GRAPHICS_HEIGHT (WINDOW_HEIGHT / 2)
#define TEXT_HEIGHT (WINDOW_HEIGHT / 3)
#define BAR_WIDTH (WINDOW_WIDTH / 6)

#define BORDER_TRUE 0.2
#define BORDER_FALSE 0.4
#define FONT_SIZE 0.8
#define BALL_RATIO 0.7
#define RING_RATIO 0.5
#define ASPECT 0.6
#define MAX_FONT_PIXELS 16

#define MAX_SEC_HALT 0.25
#define DELAY 10000
#define MAX_WAIT 25
#define TAIL_LEN 8

#define round(x) floor((x) + 0.5)
#define print_error(SB, A) print(SB, "%s: %s\n", A, strerror(errno))

typedef enum runstate
  {
    stopped,
    starting,
    started,
    stopping
  } runstate;

typedef struct config
  {
    char source_name[MAX_STRLEN];
    char base_name[MAX_STRLEN];
    char state_name[MAX_STRLEN];
    char logfile_name[MAX_STRLEN];
    char xref_name[MAX_STRLEN];
    int bsbt;
    m_time step;
    d_time max_time;
    int num_threads;
    double prob;
    double correction;
    char prefix[MAX_STRLEN];
    char path[MAX_STRLEN];
    char include_path[MAX_STRLEN];
    char alpha[SYMBOL_NUMBER + 1];
    bool load_state;
    bool strictly_causal;
    bool soundness_check;
    bool echo_stdout;
    bool echo_logfile;
    bool file_io;
    bool quiet;
    bool hard;
    bool sys5;
    bool sturdy;
    bool busywait;
    bool use_xref;
    bool seplit_fe;
    bool seplit_su;
    bool merge;
    bool postopt;
    bool constout;
    bool outaux;
    bool outint;
    bool batch_in;
    bool batch_out;
    bool draw_undef;
    bool full_names;
    int horizon_size;
    int display_rows;
    char editor_name[MAX_STRLEN];
    char viewer_name[MAX_STRLEN];
    double inprob[MAX_FILES];
    bool fexcl[MAX_FILES];
    bool gexcl[MAX_FILES];
    int fn;
    int gn;
  } config;

typedef struct s_base
  {
    char memory_f[MAX_FILES][MAX_HORIZON_SIZE];
    char memory_g[MAX_FILES][MAX_HORIZON_SIZE];
    int fn;
    int gn;
    int pos;
    d_time t;
    m_time time;
    m_time time_base;
    char fnames[MAX_FILES][MAX_STRLEN];
    char fnames_full[MAX_FILES][MAX_STRLEN];
    bool ffile_io[MAX_FILES];
    bool fpacked[MAX_FILES];
    char gnames[MAX_FILES][MAX_STRLEN];
    char gnames_full[MAX_FILES][MAX_STRLEN];
    bool gfile_io[MAX_FILES];
    bool gpacked[MAX_FILES];
    bool gomit[MAX_FILES];
    bool gaux[MAX_FILES];
    bool gsync[MAX_FILES];
    int maxlen;
    file fp[MAX_FILES];
    file gp[MAX_FILES];
    channel_posix cp[MAX_FILES];
    channel_posix dp[MAX_FILES];
    channel_sys5 cp5[MAX_FILES];
    channel_sys5 dp5[MAX_FILES];
    char xbuffer[XBUFSIZE];
    int xstart;
    int xend;
    bool xcat;
    bool mt;
    char cmd[MAX_STRLEN_IF];
    runstate rs;
    bool term;
    int sent;
    bool regenerate;
    bool changed;
    bool configured;
    bool changeprob;
    config cfg;
    m_time cp_step;
    d_time cp_max_time;
    bool cp_echo_stdout;
    bool cp_file_io;
    bool cp_quiet;
    bool cp_sys5;
    bool cp_batch_in;
    bool cp_batch_out;
    bool cp_full_names;
    int cp_horizon_size;
    int cp_display_rows;
    pthread_t tintloop;
    pthread_t tinxpipe;
    pthread_mutex_t mutex_xbuffer;
    pthread_mutex_t mutex_button;
    pthread_mutex_t mutex_sent;
    GtkWindow *window;
    GtkWindow *config_window;
    GtkWindow *prob_window;
    GtkWindow *filter_window;
    GtkDrawingArea *drawingarea;
    GtkTextView *textarea;
    GtkButton *run_button;
    GtkButton *save_button;
    GtkButton *erase_button;
    GtkButton *dummy_button;
    GtkButton *edit_button;
    GtkButton *help_button;
    GtkButton *clear_button;
    GtkMenuItem *run_menu;
    GtkMenuItem *help_menu;
    GtkMenuItem *save_menu;
    GtkMenuItem *erase_menu;
    GtkAdjustment *area_adj;
    GtkLabel *timer;
    GtkLabel *reg_warning;
    GtkImage *reg_warning_icon;
    GtkWidget *inprob_widget[MAX_FILES];
    GtkWidget *infilter_widget[MAX_FILES];
    GtkWidget *outfilter_widget[MAX_FILES];
  } s_base;

/* Protos */

INLINE m_time get_time();
void plot(cairo_t *cr, s_base *sb, int x, int y, double offset_x, double offset_y, double width, double height, char truth, bool text);
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
void update_drawing(s_base *sb);
gboolean update_view(s_base *sb);
gboolean reset_view(s_base *sb);
void print(s_base *sb, char *string, ...);
void print_add(s_base *sb, char *string, ...);
pid_t pidof(s_base *sb, char *name);
int execute(char *source_name, char *base_name, char *state_name, char *logfile_name, char *xref_name,
         bool strictly_causal, bool soundness_check, bool echo_stdout, bool file_io, bool quiet, bool hard, bool sys5, bool sturdy, bool busywait, bool seplit_fe, bool seplit_su, bool merge, bool postopt,
         bool outaux, bool outint, int bufexp, d_time max_time, m_time step, char *prefix, char *path, char *include_path, char *alpha, int num_threads, double prob,
         bool batch_in, bool batch_out, bool draw_undef);


