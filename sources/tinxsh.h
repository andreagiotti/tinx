
/* TINXSH by Andrea Giotti */

#include "tinx_mt.h"

#define CMD_PATH "."

/* Control sequences */
#define ESC				"\x1b"
#define CSI				ESC "["
#define HOME				CSI "H"
#define ERASE2EOS			CSI "J"
#define ERASE2EOL			CSI "K"
#define CLS				HOME ERASE2EOS
#define LOCATE(i, j)			CSI i ";" j "H"
#define SET_COLORS(fg, bg)		CSI "1;3" fg ";4" bg "m"
#define RESET_COLORS			CSI "m"

/* Dummy functions */
#define locate(i, j)			printf(LOCATE("%d", "%d"), (int)(i) + 1, (int)(j) + 1)
#define	clear_screen()			puts(CLS)
#define clear_to_eos()			puts(ERASE2EOS)
#define clear_to_eol()			puts(ERASE2EOL)

#define MAX_STRLEN_IF 256
#define MAX_STRLEN_IF_C "255"

#define OFFSET_I 1
#define OFFSET_J 2

#define TIME_LIMIT LONG_MAX

#define MAX_FILES 256
#define HORIZON_SIZE 81
#define MAX_RUN_LEN 65536

#define SKIP_FMT "%*[^?!]"

#define LO_CHAR '0'
#define HI_CHAR '1'
#define UNKNOWN_CHAR '?'
#define END_CHAR '.'
#define TERM_CHAR '\x1b' /* Escape */

#define DISPLAY_LO_CHAR '-'
#define DISPLAY_HI_CHAR '#'
#define DISPLAY_UNKNOWN_CHAR ' '

#define DEFAULT_PROB ((float) 0.5)

typedef enum ftype
{
  text,
  integer,
  floating,
  boolean,
  FTYPE_NUMBER
} ftype;

typedef enum fname
{
  b_name,
  ic_name,
  log_name,
  sampling_time,
  process_number,
  causal_switch,
  display_switch,
  file_switch,
  ic_switch,
  log_switch,
  quiet_switch,
  verify_switch,
  truth_prob,
  horizon,
  src_name,
  hard_switch,
  sturdy_switch,
  FNAME_NUMBER
} fname;

#define get_field(F) (*((F)->content) ? (F)->content : (F)->defval)

typedef struct field
{
  ftype ft;
  bool internal;
  char name[MAX_STRLEN_IF];
  char content[MAX_STRLEN_IF];
  char defval[MAX_STRLEN_IF];
  int length;
  int i;
  int j;
  float min;
  float max;
  char key;
  char prefix[MAX_STRLEN_IF];
  char rv[MAX_STRLEN_IF];
} field;

/* Protos */

void display_field(field *f);
char *input_field(field *f);
bool eval_field(field *f);
int tint(char *netname, bool file_io, m_time step, int horizon_len, float prob, bool batch);
int execute(char *base_name, char *state_name, char *logfile_name,
         bool strictly_causal, bool soundness_check, bool echo_stdout, bool file_io, bool quiet, bool hard, bool sturdy,
         m_time step, int num_threads, int horizon_len, float prob, bool batch);


