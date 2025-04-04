
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
#define COMPILER_FILENAME "ting"
#define MONOENGINE_FILENAME "tinx"
#define DUALENGINE_FILENAME "tinx_dt"
#define MULTIENGINE_FILENAME "tinx_mt"
#define LARGEENGINE_FILENAME "tinx_zt"
#define HELP_FILENAME "/usr/share/doc/tinx/reference.pdf"

#define CONFIG_HEADER "TINX"
#define CONFIG_VERSION "0006"

#define MAX_STRLEN_IF 512
#define MAX_STRLEN_IF_C "511"
#define LONG_STRLEN_IF DEBUG_STRLEN

#define MAX_FILES 1024
#define MAX_QUEUES 256
#define MAX_EXT_SIGNALS 300
#define DEFAULT_HORIZON_SIZE (100 + 1)
#define MAX_HORIZON_SIZE (255 + 1)
#define MAX_RUN_LEN 999999999999
#define MIN_BSBT 2
#define MAX_BSBT 30

#define SKIP_FMT "%"MAX_NAMEBUF_C"[^ ?!._]%*[^\n]"
#define NUM_COLORS 5

#define DISPLAY_LO_CODE -1
#define DISPLAY_HI_CODE 1
#define DISPLAY_UNKNOWN_CODE 0

#define DEFAULT_DISPLAY_ROWS 15
#define MAX_DISPLAY_ROWS 255

#define DEFAULT_PROB ((double) 0.5)

#define TITLE "Temporal Inference Network eXecutor Suite"
#define CONFIG_TITLE "Configuration"
#define PROB_TITLE "Probabilities"
#define FILTER_TITLE "External signals"
#define PLANSETUP_TITLE "Phase plan setup"
#define SPECTRUM_TITLE "Spectrum analyzer"
#define PLAN_TITLE "Phase plan analyzer"

#define HEADING "Temporal Inference Network eXecutor Suite "PACK_VER", graphical shell "VER"\n" \
                "Design &amp; coding by Andrea Giotti, 1998-1999, 2016-2025\n"

#define BANNER HEADING "\n" \
        "TINX is a real time inference engine for system specification in an executable temporal logic. It is able to acquire, process and generate any binary, <i>n</i>-ary or real signal through POSIX\n" \
        "IPC, files or UNIX sockets. Specifications of signals and dynamic systems are represented as special graphs named <i>temporal inference networks</i> and executed in real time, with a\n" \
        "predictable sampling time which varies from few microseconds to some milliseconds, depending on the complexity of the specification.\n\n" \
        "Real time signal processing, dynamic system control, modeling of state machines, logical and mathematical property verification, realization of reactive systems are some fields of\n" \
        "application of this inference engine, which is deterministic but fully relational. It adopts driven forward reasoning in a three-valued logic, the clauses of which are assumed unknown\n" \
        "as default, to satisfy relations (2SAT) and it is able to run on an unlimited temporal horizon.\n\n" \
        "The accepted language is named <i>Basic Temporal Logic</i> and provides logical and mathematical operators, temporal operators on instants and intervals, parametrization of signals by\n" \
        "multidimensional arrays and bounded quantifiers on them.\n\n" \
        "This software runs on Linux operating system and it is distributed under GNU Public License.\n"

#define HEADING_HTML "Temporal Inference Network eXecutor Suite\n\n" \
                     "\tPackage version "PACK_VER"\n" \
                     "\tGraphical shell "VER"\n\n" \
                     "\tDesign &amp; coding by Andrea Giotti\n" \
                     "\t1998-1999, 2016-2025\n\n" \
                     "\tThis software is licensed under the\n" \
                     "\tGNU Public License\n"

#define REAL_FMT_IF "%.6f"
#define ORIGIN_FMT "%.9f"

#define XBUFSIZE 1048576

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800
#define CONFIG_WINDOW_WIDTH 1000
#define CONFIG_WINDOW_HEIGHT 600
#define SPECTRUM_WINDOW_WIDTH 600
#define SPECTRUM_WINDOW_HEIGHT 300
#define PLAN_WINDOW_WIDTH 400
#define PLAN_WINDOW_HEIGHT 400
#define PROB_WINDOW_WIDTH 256
#define PROB_WINDOW_HEIGHT 128
#define FILTER_WINDOW_WIDTH 256
#define FILTER_WINDOW_HEIGHT 128
#define PLANSETUP_WINDOW_WIDTH 256
#define PLANSETUP_WINDOW_HEIGHT 256
#define POPUP_WINDOW_WIDTH 400
#define POPUP_WINDOW_HEIGHT 200

#define GRAPHICS_HEIGHT (WINDOW_HEIGHT / 3)
#define TEXT_HEIGHT (WINDOW_HEIGHT / 5)
#define BAR_WIDTH (WINDOW_WIDTH / 6)

#define BORDER_TRUE 0.2
#define BORDER_FALSE 0.4
#define FONT_SIZE 0.8
#define BALL_RATIO 0.7
#define RING_RATIO 0.5
#define ASPECT 0.6
#define MAX_FONT_PIXELS 16
#define GRAPH_SCALE 0.8

#define MAX_SEC_HALT 0.25
#define DELAY 1000
#define MAX_WAIT 25
#define TAIL_LEN 8

#define round(x) floor((x) + 0.5)

#define XON '\x17'
#define XOFF '\x19'

#define MAX_DFT MAX_HORIZON_SIZE
#define MAX_PLAN MAX_HORIZON_SIZE

typedef enum runstate
  {
    invalid,
    stopped,
    starting,
    started,
    stopping,
    frozen
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
    bool hp_io;
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
    bool constout_sugg;
    bool constout_user;
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
    char compiler_name[MAX_STRLEN];
    char engine_name[4][MAX_STRLEN];
    char preext_name[MAX_STRLEN];
    char postext_name[MAX_STRLEN];
    double inprob[MAX_FILES];
    bool fexcl[MAX_FILES];
    bool gexcl[MAX_FILES];
    int fn;
    int gn;
    /* V6 */
    char fname[MAX_FILES][MAX_STRLEN];
    char gname[MAX_FILES][MAX_STRLEN];
    bool pplan[MAX_FILES][MAX_FILES];
  } config;

typedef struct cfgcache
  {
    char fname[MAX_FILES][MAX_STRLEN];
    char gname[MAX_FILES][MAX_STRLEN];
    double inprob[MAX_FILES];
    bool fexcl[MAX_FILES];
    bool gexcl[MAX_FILES];
    bool pplan[MAX_FILES][MAX_FILES];
    int indir[MAX_FILES];
  } cfgcache;

#if defined BUGGED_PTHREADS
	#define lock_pipe(SB) pthread_mutex_lock(&(SB)->mutex_pipe)
	#define unlock_pipe(SB) pthread_mutex_unlock(&(SB)->mutex_pipe)

	#define wait_pipe(SB) pthread_cond_wait(&(SB)->cond_pipe, &(SB)->mutex_pipe)
	#define signal_pipe(SB) pthread_cond_signal(&(SB)->cond_pipe)
#endif

typedef struct channel
  {
    char memory[MAX_HORIZON_SIZE];
    real realmem[MAX_HORIZON_SIZE];
    char name[MAX_STRLEN];
    char name_full[MAX_STRLEN];
    bool file_io;
    bool packed;
    bool omit;
    bool aux;
    bool realbuf;
    bool excl;
    int sync;
    file fp;
    channel_posix cp;
    channel_sys5 cp5;       
  } channel;

#define MAXREQUESTS 16

typedef struct s_base
  {
    channel f[MAX_FILES];
    channel g[MAX_FILES];
    int fn;
    int gn;
    int pos;
    int specpos;
    d_time t;
    m_time time;
    m_time time_base;
    int maxlen;
    char xbuffer[XBUFSIZE];
    int xstart;
    int xend;
    bool xcat;
    int version;
    char cmd[MAX_STRLEN_IF];
    runstate rs;
    runstate rsbu;
    runstate rsreq[MAXREQUESTS];
    int startreq;
    int endreq;
    bool processing;
    bool error;
    bool term;
    bool waitio;
    bool regenerate;
    bool changed;
    bool configured;
    bool changeprob;
    bool changefilter;
    bool changepp;
    bool donotquit;
    bool donotscroll;
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
    int fancymsg;
    real re[MAX_DFT][MAX_DFT];
    real im[MAX_DFT][MAX_DFT];
    pthread_t tintloop;
    pthread_t tinxpipe;
    pthread_mutex_t mutex_xbuffer;
    pthread_mutex_t mutex_button;
    pthread_mutex_t mutex_request;
#if defined BUGGED_PTHREADS
    pthread_mutex_t mutex_pipe;
    /* pthread_cond_t cond_pipe; */
    bool done;
#endif
    GtkWindow *window;
    GtkWindow *config_window;
    GtkWindow *prob_window;
    GtkWindow *filter_window;
    GtkWindow *pplan_window;
    GtkWindow *spectrum_window;
    GtkWindow *plan_window;
    GtkDrawingArea *drawingarea;
    GtkDrawingArea *spectrumarea;
    GtkDrawingArea *planarea;
    GtkTextView *textarea;
    GtkButton *gen_button;
    GtkButton *run_button;
    GtkButton *freeze_button;
    GtkButton *save_button;
    GtkButton *erase_button;
    GtkButton *edit_button;
    GtkButton *help_button;
    GtkButton *clear_button;
    GtkButton *filclear_button;
    GtkButton *ppclear_button;
    GtkMenuItem *gen_menu;
    GtkMenuItem *run_menu;
    GtkMenuItem *freeze_menu;
    GtkMenuItem *help_menu;
    GtkMenuItem *save_menu;
    GtkMenuItem *erase_menu;
    GtkAdjustment *area_adj;
    GtkAdjustment *spectrum_adj;
    GtkLabel *timer;
    GtkLabel *reg_warning;
    GtkImage *reg_warning_icon;
    GtkWidget *inprob_widget[MAX_FILES];
    GtkWidget *infilter_widget[MAX_FILES];
    GtkWidget *outfilter_widget[MAX_FILES];
    GtkWidget *pplan_widget[MAX_FILES][MAX_FILES];
  } s_base;

/* Protos */

INLINE m_time get_time();
void plot(cairo_t *cr, s_base *sb, int x, int y, double offset_x, double offset_y, double width, double height, char truth, bool text);
gboolean draw_callback(GtkWidget *widget, cairo_t *cr, s_base *sb);
gboolean tick_callback(GtkWidget *widget, GdkFrameClock *frame_clock, s_base *sb);
void tintloop(s_base *sb);
void tinxpipe(s_base *sb);
void request_runstate(s_base *sb, runstate rs);
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
gboolean goto_start(s_base *sb);
gboolean goto_end(s_base *sb);
void print(s_base *sb, char *string, ...);
void print_add(s_base *sb, char *string, ...);
void fancyprint(s_base *sb, char *string, ...);
void fancyprint_add(s_base *sb, char *string, ...);
pid_t pidof(s_base *sb, char *name);
void configure(GtkWidget *widget, s_base *sb);
void main_window(s_base *sb);
int execute(char *source_name, char *base_name, char *state_name, char *logfile_name, char *xref_name,
         bool strictly_causal, bool soundness_check, bool echo_stdout, bool file_io, bool quiet, bool hard, bool sys5, bool sturdy, bool busywait, bool seplit_fe, bool seplit_su, bool merge, bool postopt,
         bool outaux, bool outint, int bufexp, d_time max_time, m_time step, char *prefix, char *path, char *include_path, char *alpha, int num_threads, double prob,
         bool batch_in, bool batch_out, bool draw_undef);


