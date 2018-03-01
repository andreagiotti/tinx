
/* TINXSH by Andrea Giotti */

#define ANSI_FILE_IO
/* #define UNIX_FILE_IO */
#define POSIX_IPC_IO
/* #define UNIX_IPC_IO */

#include "tinxsh.h"

#define VER "0.3.3"

volatile sig_atomic_t irq = FALSE;

INLINE m_time get_time()
{
  struct timespec ts;

  clock_gettime(CLOCK_TYPE, &ts);

  return ts.tv_sec + 0.000000001 * ts.tv_nsec;
}

int main(int argc, char *argv[])
{
  char *base_name, *state_name, *logfile_name, *option;
  char default_state_name[MAX_STRLEN], default_logfile_name[MAX_STRLEN];
  bool strictly_causal, soundness_check, echo_stdout, file_io, quiet, hard, sturdy, batch;
  int i;
  float default_step, prob;
  m_time step;
  int num_threads, horizon_len;

  base_name = state_name = logfile_name = NULL;
  strictly_causal = soundness_check = echo_stdout = file_io = quiet = hard = sturdy = batch = FALSE;
  default_step = -1;
  num_threads = DEFAULT_THREADS;
  prob = DEFAULT_PROB;
  horizon_len = 0;

  for(i = 1; i < argc; i++)
    {
      if(*argv[i] == '-')
        {
          option = argv[i] + 1;
          switch(*option)
            {
            case 'h':
              fprintf(stderr, "Usage: %s [-bcdfilqsvy] [-I state] [-L log] [-n processes] [-t step] [-p probability] [-z horizon] [base]\n",
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
                  horizon_len = atoi(argv[i]);
                  if(horizon_len < 0 || horizon_len > MAX_RUN_LEN)
                    {
                      fprintf(stderr, "%d: Argument out of range\n", horizon_len);
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
          if(!base_name)
            base_name = argv[i];
          else
            fprintf(stderr, "%s: Extra argument ignored\n", argv[i]);
        }
    }

  if(!base_name)
    base_name = DEFAULT_NAME;

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

  if(default_step < 0)
    step = quiet? 0 : DEFAULT_STEP_SEC;
  else
    step = default_step;

  return execute(base_name, state_name, logfile_name,
      strictly_causal, soundness_check, echo_stdout, file_io, quiet, hard, sturdy, step, num_threads, horizon_len, prob, batch);
}

void trap()
{
  irq = TRUE;
}

int tint(char *netname, bool file_io, m_time step, int horizon_len, float prob, bool batch)
{
  FILE *bp;
  file fp[MAX_FILES], gp[MAX_FILES];
  channel cp[MAX_FILES], dp[MAX_FILES];
  int fn, gn, i, row;
  char name[MAX_STRLEN];
  char fnames[MAX_FILES][MAX_STRLEN], gnames[MAX_FILES][MAX_STRLEN];
  d_time t, k, tau[MAX_FILES];
  char memory_f[MAX_FILES][HORIZON_SIZE], memory_g[MAX_FILES][HORIZON_SIZE];
  char c, ic, oc;
  m_time deadline, gt;

  assert(netname);

  strcpy(name, netname);
  strcat(name, NETWORK_EXT);

  bp = fopen(name, "r");
  if(!bp)
    {
      perror(name);
      return EXIT_FAILURE;
    }

  fn = gn = 0;
  while(fscanf(bp, SKIP_FMT" "OP_FMT" "NAME_FMT, &c, name) == 2)
    {
      switch(c)
        {
        case '!':
          if(fn >= MAX_FILES)
            {
              fprintf(stderr, "Too many input files\n");
              return EXIT_FAILURE;
            }

          if(file_io)
            {
              strcpy(fnames[fn], name);
              strcat(fnames[fn], STREAM_EXT);

              fp[fn] = open_output_file(fnames[fn]);
              if(!is_file_open(fp[fn]))
                {
                  perror(fnames[fn]);
                  return EXIT_FAILURE;
                }
            }
          else
            {
              strcpy(fnames[fn], MAGIC_PREFIX);
              strcat(fnames[fn], name);

              cp[fn] = add_queue(fnames[fn], output_stream);
              if(failed_queue(cp[fn]))
                {
                  perror(fnames[fn]);
                  return EXIT_FAILURE;
                }
            }

          fn++;

        break;

        case '?':
          if(gn >= MAX_FILES)
            {
              fprintf(stderr, "Too many output files\n");
              return EXIT_FAILURE;
            }

          if(file_io)
            {
              strcpy(gnames[gn], name);
              strcat(gnames[gn], STREAM_EXT);

              clean_file(gnames[gn]);
            }
          else
            {
              strcpy(gnames[gn], MAGIC_PREFIX);
              strcat(gnames[gn], name);
            }

          gn++;

        break;

        default:
          assert(FALSE);
        }
    }

  if(ferror(bp))
    {
      perror(name);
      return EXIT_FAILURE;
    }

  if(fclose(bp))
    {
      perror(name);
      return EXIT_FAILURE;
    }

  if(!batch)
    {
      for(i = 0; i < fn; i++)
        for(k = 0; k < HORIZON_SIZE; k++)
          memory_f[i][k] = DISPLAY_UNKNOWN_CHAR;

      for(i = 0; i < gn; i++)
        for(k = 0; k < HORIZON_SIZE; k++)
          memory_g[i][k] = DISPLAY_UNKNOWN_CHAR;
    }

  for(i = 0; i < gn; i++)
    {
      if(file_io)
        do
          gp[i] = open_input_file(gnames[i]);
        while(!is_file_open(gp[i]));
      else
        {
          dp[i] = add_queue(gnames[i], input_stream);
          if(failed_queue(dp[i]))
            {
              perror(gnames[i]);
              return EXIT_FAILURE;
            }
         }

      tau[i] = 0;
    }

  signal(SIGINT, (void (*)())&trap);

  t = 0;
  deadline = 0;
  for(;;)
    {
      gt = get_time();
      if(gt >= deadline)
        {
          deadline = gt + step;

          if(!batch)
            {
              row = 0;

              for(i = 0; i < fn; i++)
                {
                  printf(SET_COLORS("1","0"));

                  locate(OFFSET_I + 4 + row, OFFSET_J);
                  row++;

                  memory_f[i][t % HORIZON_SIZE] = DISPLAY_UNKNOWN_CHAR;

                  for(k = t + 1; k < t + HORIZON_SIZE; k++)
                    putchar(memory_f[i][k % HORIZON_SIZE]);

                  printf(SET_COLORS("4","9")" [I] %s\n", fnames[i]);
                }


              for(i = 0; i < gn; i++)
                {
                  printf(SET_COLORS("2","0"));

                  while(tau[i] < t)
                    {
                      if(file_io)
                        {
                          if(get_file(gp[i], &ic) && file_error(gp[i]))
                            {
                              perror(gnames[i]);
                              return EXIT_FAILURE;
                            }

                          if(ic == EOF)
                            {
                              reset_file(gp[i]);
                              break;
                            }
                        }
                      else
                        {
                          read_message(dp[i], &ic);

                          if(ic == EOF)
                            break;
                        }

                      switch(ic)
                        {
                          case UNKNOWN_CHAR:
                            memory_g[i][tau[i] % HORIZON_SIZE] = DISPLAY_UNKNOWN_CHAR;
                          break;

                          case LO_CHAR:
                            memory_g[i][tau[i] % HORIZON_SIZE] = DISPLAY_LO_CHAR;
                          break;

                          case HI_CHAR:
                            memory_g[i][tau[i] % HORIZON_SIZE] = DISPLAY_HI_CHAR;
                          break;

                          case END_CHAR:
                            memory_g[i][tau[i] % HORIZON_SIZE] = DISPLAY_UNKNOWN_CHAR;
                          break;

                          default:
                            fprintf(stderr, "%s, %c (dec %d): Invalid character in stream\n", gnames[i], ic, ic);
                            return EXIT_FAILURE;
                          break;
                        }

                      tau[i]++;
                    }

                  locate(OFFSET_I + 4 + row, OFFSET_J);
                  row++;

                  memory_g[i][t % HORIZON_SIZE] = DISPLAY_UNKNOWN_CHAR;

                  for(k = t + 1; k < t + HORIZON_SIZE; k++)
                    putchar(memory_g[i][k % HORIZON_SIZE]);

                  if(memory_g[i][(t + (HORIZON_SIZE - 1)) % HORIZON_SIZE] == DISPLAY_UNKNOWN_CHAR)
                    printf(SET_COLORS("3","9"));
                  else
                    printf(SET_COLORS("4","9"));

                  printf(" [O] %s\n", gnames[i]);
                }

              printf(RESET_COLORS);
              fflush(stdout);
            }

          if(irq || (horizon_len && t >= horizon_len))
            break;

          for(i = 0; i < fn; i++)
            {
              oc = (rand() <= prob * RAND_MAX)? HI_CHAR : LO_CHAR;

              if(file_io)
                {
                  if(put_file(fp[i], &oc))
                    {
                      perror(fnames[i]);
                      return EXIT_FAILURE;
                    }

                  if(sync_file(fp[i]))
                    {
                      perror(fnames[i]);
                      return EXIT_FAILURE;
                    }
                  }
                else
                  send_message(cp[i], &oc);

              if(!batch)
                memory_f[i][t % HORIZON_SIZE] = (oc == LO_CHAR)? DISPLAY_LO_CHAR : DISPLAY_HI_CHAR;
            }

          t++;
        }
    }

  signal(SIGINT, SIG_DFL);

  irq = FALSE;

  for(i = 0; i < fn; i++)
    {
      oc = END_CHAR;

      if(file_io)
        {
          if(put_file(fp[i], &oc))
            {
              perror(fnames[i]);
              return EXIT_FAILURE;
            }

          if(close_file(fp[i]))
            {
              perror(fnames[i]);
              return EXIT_FAILURE;
            }
        }
      else
        {
          send_message(cp[i], &oc);

          if(commit_queue(cp[i]))
            {
              perror(fnames[i]);
              return EXIT_FAILURE;
            }
        }
    }

  for(i = 0; i < gn; i++)
    if(file_io)
      {
        if(close_file(gp[i]))
          {
            perror(gnames[i]);
            return EXIT_FAILURE;
          }
      }
    else
      {
        if(commit_queue(dp[i]))
          {
            perror(gnames[i]);
            return EXIT_FAILURE;
          }
      }

  return EXIT_SUCCESS;
}

void display_field(field *f)
{
  locate(f->i, f->j);

  if(*(f->content))
    printf("[%c] %s: "SET_COLORS("3","9")"%-*s"RESET_COLORS, f->key, f->name, f->length, f->content);
  else
    printf("[%c] %s: "SET_COLORS("4","9")"%-*s"RESET_COLORS, f->key, f->name, f->length, f->defval);
}

char *input_field(field *f)
{
  *(f->rv) = '\0';
  do
    {
      locate(f->i, f->j + strlen(f->name) + 6);

      while(scanf("%"MAX_STRLEN_IF_C"s", f->content) < 1);
    }
  while(!eval_field(f));

  return f->rv;
}

bool eval_field(field *f)
{
  bool checked;
  int i, j;
  float r, s;
  char b, c;

  checked = FALSE;

  if(*(f->content) == '\x1b')
    *(f->content) = '\0';

  if(*(f->content) == '\0')
    checked = TRUE;
  else
    switch(f->ft)
      {
        case text:
          checked = TRUE;

          if(!strcmp(f->content, f->defval))
            *(f->content) = '\0';
          else
            if(!f->internal && *(f->content))
              {
                strcpy(f->rv, f->prefix);
                strcat(f->rv, f->content);
              }
        break;

        case integer:
          i = atoi(f->content);

          if(i >= f->min && i <= f->max)
            {
              checked = TRUE;

              j = atoi(f->defval);
              if(i == j)
                *(f->content) = '\0';
              else
                {
                  sprintf(f->content, "%d", i);

                  if(!f->internal)
                    {
                      strcpy(f->rv, f->prefix);
                      strcat(f->rv, f->content);
                    }
                }
            }
        break;

        case floating:
          r = atof(f->content);

          if(r >= f->min && r <= f->max)
            {
               checked = TRUE;

               s = atof(f->defval);
               if(r == s)
                 *(f->content) = '\0';
               else
                 {
                   sprintf(f->content, "%f", r);

                   if(!f->internal)
                     {
                       strcpy(f->rv, f->prefix);
                       strcat(f->rv, f->content);
                     }
                 }
             }
         break;

         case boolean:
           b = toupper(*(f->content));

           if(b == 'Y' || b == 'N')
             {
               checked = TRUE;

               c = toupper(*(f->defval));
               if(b == c)
                 *(f->content) = '\0';
               else
                   {
                     *(f->content) = b;
                     *(f->content + 1) = '\0';

                     if(!f->internal)
                       strcpy(f->rv, f->prefix);
                   }
             }
         break;

         default:
           exit(EXIT_FAILURE);
         break;
      }

  return checked;
}

int execute(char *base_name, char *state_name, char *logfile_name,
         bool strictly_causal, bool soundness_check, bool echo_stdout, bool file_io, bool quiet, bool hard, bool sturdy, m_time step, int num_threads, int horizon_len, float prob, bool batch)
{
  field f[FNAME_NUMBER];
  char cmd[MAX_STRLEN_IF];
  fname fn;
  char *execname;
  char c;
  FILE *fp;
  int i;
  struct sched_param spar;

  f[b_name].ft = text;
  f[b_name].internal = FALSE;
  strcpy(f[b_name].name, "Network file name");
  strcpy(f[b_name].defval, DEFAULT_NAME);
  strcpy(f[b_name].content, strcmp(base_name, DEFAULT_NAME)? base_name : "");
  f[b_name].length = 16;
  f[b_name].i = OFFSET_I + 4;
  f[b_name].j = OFFSET_J;
  f[b_name].key = 'B';
  strcpy(f[b_name].prefix, " ");
  strcpy(f[b_name].rv, "");

  f[ic_name].ft = text;
  f[ic_name].internal = FALSE;
  strcpy(f[ic_name].name, "Initial state file name");
  strcpy(f[ic_name].defval, get_field(&f[b_name]));
  strcat(f[ic_name].defval, STATE_SUFFIX);
  strcpy(f[ic_name].content, state_name? state_name : "");
  f[ic_name].length = 16;
  f[ic_name].i = OFFSET_I + 4;
  f[ic_name].j = OFFSET_J + 45;
  f[ic_name].key = 'E';
  strcpy(f[ic_name].prefix, " -I ");
  strcpy(f[ic_name].rv, "");

  f[log_name].ft = text;
  f[log_name].internal = FALSE;
  strcpy(f[log_name].name, "Log file name");
  strcpy(f[log_name].defval, get_field(&f[b_name]));
  strcat(f[log_name].defval, LOG_SUFFIX);
  strcpy(f[log_name].content, logfile_name? logfile_name : "");
  f[log_name].length = 16;
  f[log_name].i = OFFSET_I + 4;
  f[log_name].j = OFFSET_J + 90;
  f[log_name].key = 'M';
  strcpy(f[log_name].prefix, " -L ");
  strcpy(f[log_name].rv, "");

  f[sampling_time].ft = floating;
  f[sampling_time].internal = FALSE;
  strcpy(f[sampling_time].name, "Sampling time (s)");
  sprintf(f[sampling_time].defval, "%f", DEFAULT_STEP_SEC);
  if(step != DEFAULT_STEP_SEC)
    sprintf(f[sampling_time].content, "%f", step);
  else
    strcpy(f[sampling_time].content, "");
  f[sampling_time].length = 10;
  f[sampling_time].i = OFFSET_I + 6;
  f[sampling_time].j = OFFSET_J;
  f[sampling_time].min = 0;
  f[sampling_time].max = 3600;
  f[sampling_time].key = 'T';
  strcpy(f[sampling_time].prefix, " -t ");
  strcpy(f[sampling_time].rv, "");

  f[process_number].ft = integer;
  f[process_number].internal = FALSE;
  strcpy(f[process_number].name, "Number of processes");
  sprintf(f[process_number].defval, "%d", DEFAULT_THREADS);
  if(num_threads != DEFAULT_THREADS)
    sprintf(f[process_number].content, "%d", num_threads);
  else
    strcpy(f[process_number].content, "");
  f[process_number].length = 3;
  f[process_number].i = OFFSET_I + 6;
  f[process_number].j = OFFSET_J + 45;
  f[process_number].min = 1;
  f[process_number].max = 256;
  f[process_number].key = 'N';
  strcpy(f[process_number].prefix, " -n ");
  strcpy(f[process_number].rv, "");

  f[causal_switch].ft = boolean;
  f[causal_switch].internal = FALSE;
  strcpy(f[causal_switch].name, "Causal inference");
  strcpy(f[causal_switch].defval, "N");
  strcpy(f[causal_switch].content, strictly_causal? "Y" : "");
  f[causal_switch].length = 1;
  f[causal_switch].i = OFFSET_I + 6;
  f[causal_switch].j = OFFSET_J + 90;
  f[causal_switch].key = 'C';
  strcpy(f[causal_switch].prefix, " -c ");
  strcpy(f[causal_switch].rv, "");

  f[display_switch].ft = boolean;
  f[display_switch].internal = FALSE;
  strcpy(f[display_switch].name, "Trace inference");
  strcpy(f[display_switch].defval, "N");
  strcpy(f[display_switch].content, echo_stdout? "Y" : "");
  f[display_switch].length = 1;
  f[display_switch].i = OFFSET_I + 8;
  f[display_switch].j = OFFSET_J;
  f[display_switch].key = 'D';
  strcpy(f[display_switch].prefix, " -d ");
  strcpy(f[display_switch].rv, "");

  f[file_switch].ft = boolean;
  f[file_switch].internal = FALSE;
  strcpy(f[file_switch].name, "File input/output");
  strcpy(f[file_switch].defval, "N");
  strcpy(f[file_switch].content, file_io? "Y" : "");
  f[file_switch].length = 1;
  f[file_switch].i = OFFSET_I + 8;
  f[file_switch].j = OFFSET_J + 45;
  f[file_switch].key = 'F';
  strcpy(f[file_switch].prefix, " -f ");
  strcpy(f[file_switch].rv, "");

  f[ic_switch].ft = boolean;
  f[ic_switch].internal = FALSE;
  strcpy(f[ic_switch].name, "Load initial state");
  strcpy(f[ic_switch].defval, "N");
  strcpy(f[ic_switch].content, state_name? "Y" : "");
  f[ic_switch].length = 1;
  f[ic_switch].i = OFFSET_I + 8;
  f[ic_switch].j = OFFSET_J + 90;
  f[ic_switch].key = 'I';
  strcpy(f[ic_switch].prefix, " -i ");
  strcpy(f[ic_switch].rv, "");

  f[log_switch].ft = boolean;
  f[log_switch].internal = FALSE;
  strcpy(f[log_switch].name, "Log inference");
  strcpy(f[log_switch].defval, "N");
  strcpy(f[log_switch].content, logfile_name? "Y" : "");
  f[log_switch].length = 1;
  f[log_switch].i = OFFSET_I + 10;
  f[log_switch].j = OFFSET_J;
  f[log_switch].key = 'L';
  strcpy(f[log_switch].prefix, " -l ");
  strcpy(f[log_switch].rv, "");

  f[quiet_switch].ft = boolean;
  f[quiet_switch].internal = FALSE;
  strcpy(f[quiet_switch].name, "Quiet mode");
  strcpy(f[quiet_switch].defval, "N");
  strcpy(f[quiet_switch].content, quiet? "Y" : "");
  f[quiet_switch].length = 1;
  f[quiet_switch].i = OFFSET_I + 10;
  f[quiet_switch].j = OFFSET_J + 45;
  f[quiet_switch].key = 'Q';
  strcpy(f[quiet_switch].prefix, " -q ");
  strcpy(f[quiet_switch].rv, "");

  f[verify_switch].ft = boolean;
  f[verify_switch].internal = FALSE;
  strcpy(f[verify_switch].name, "Soundness verify");
  strcpy(f[verify_switch].defval, "N");
  strcpy(f[verify_switch].content, soundness_check? "Y" : "");
  f[verify_switch].length = 1;
  f[verify_switch].i = OFFSET_I + 10;
  f[verify_switch].j = OFFSET_J + 90;
  f[verify_switch].key = 'V';
  strcpy(f[verify_switch].prefix, " -v ");
  strcpy(f[verify_switch].rv, "");

  f[truth_prob].ft = floating;
  f[truth_prob].internal = TRUE;
  strcpy(f[truth_prob].name, "Truth probability");
  sprintf(f[truth_prob].defval, "%f", DEFAULT_PROB);
  if(prob != DEFAULT_PROB)
    sprintf(f[truth_prob].content, "%f", prob);
  else
    strcpy(f[truth_prob].content, "");
  f[truth_prob].length = 10;
  f[truth_prob].i = OFFSET_I + 12;
  f[truth_prob].j = OFFSET_J;
  f[truth_prob].min = 0;
  f[truth_prob].max = 1;
  f[truth_prob].key = 'P';
  strcpy(f[truth_prob].rv, "");

  f[horizon].ft = integer;
  f[horizon].internal = FALSE;
  strcpy(f[horizon].name, "Horizon length (0 = none)");
  sprintf(f[horizon].defval, "%d", 0);
  if(horizon_len)
    sprintf(f[horizon].content, "%d", horizon_len);
  else
    strcpy(f[horizon].content, "");
  f[horizon].length = 10;
  f[horizon].i = OFFSET_I + 12;
  f[horizon].j = OFFSET_J + 45;
  f[horizon].min = 0;
  f[horizon].max = MAX_RUN_LEN;
  f[horizon].key = 'Z';
  strcpy(f[horizon].prefix, " -z ");
  strcpy(f[horizon].rv, "");

  f[src_name].ft = text;
  f[src_name].internal = TRUE;
  strcpy(f[src_name].name, "Source file name");
  strcpy(f[src_name].defval, get_field(&f[b_name]));
  strcpy(f[src_name].content, "");
  f[src_name].length = 16;
  f[src_name].i = OFFSET_I + 12;
  f[src_name].j = OFFSET_J + 90;
  f[src_name].key = 'O';
  strcpy(f[src_name].rv, "");

  f[hard_switch].ft = boolean;
  f[hard_switch].internal = FALSE;
  strcpy(f[hard_switch].name, "Real time scheduling (root only)");
  strcpy(f[hard_switch].defval, "N");
  strcpy(f[hard_switch].content, hard? "Y" : "");
  f[hard_switch].length = 1;
  f[hard_switch].i = OFFSET_I + 14;
  f[hard_switch].j = OFFSET_J;
  f[hard_switch].key = 'S';
  strcpy(f[hard_switch].prefix, " -s ");
  strcpy(f[hard_switch].rv, "");

  f[sturdy_switch].ft = boolean;
  f[sturdy_switch].internal = FALSE;
  strcpy(f[sturdy_switch].name, "Sturdy IPC");
  strcpy(f[sturdy_switch].defval, "N");
  strcpy(f[sturdy_switch].content, sturdy? "Y" : "");
  f[sturdy_switch].length = 1;
  f[sturdy_switch].i = OFFSET_I + 14;
  f[sturdy_switch].j = OFFSET_J + 45;
  f[sturdy_switch].key = 'Y';
  strcpy(f[sturdy_switch].prefix, " -y ");
  strcpy(f[sturdy_switch].rv, "");

  for(i = 0; i < FNAME_NUMBER; i++)
    if(!eval_field(&f[i]))
      exit(EXIT_FAILURE);
/*
  spar.sched_priority = sched_get_priority_max(SCHED_RR);
  if(sched_setscheduler(0, SCHED_RR, &spar))
    {
      perror("Scheduler error");
      exit(EXIT_FAILURE);
    }
*/
  do
    {
      if(!batch)
        {
          for(;;)
            {
              clear_screen();

              locate(OFFSET_I, OFFSET_J);
              printf("*** Temporal Inference Network eXecutor Shell "VER" ***");
              locate(OFFSET_I + 2, OFFSET_J);
              printf("    Main Menu - ESC to reset field content");

              for(fn = 0; fn < FNAME_NUMBER; fn++)
                display_field(&f[fn]);

              locate(OFFSET_I + 14, OFFSET_J + 90);
              printf("[G] Generate");

              locate(OFFSET_I + 16, OFFSET_J);
              printf("[R] Run");

              locate(OFFSET_I + 16, OFFSET_J + 45);
              printf("[X] Exit");

              locate(OFFSET_I + 18, OFFSET_J);
              printf("> ");

              c = toupper(getchar());

              if(c == 'R' || c == 'X')
                break;
              else
                if(c == 'G')
                  {
                    clear_screen();
                    locate(OFFSET_I, OFFSET_J);
                    printf("*** Temporal Inference Network eXecutor Shell "VER" ***");
                    locate(OFFSET_I + 2, OFFSET_J);
                    printf("    Network generation in progress...");
                    locate(OFFSET_I + 4, OFFSET_J);
                    fflush(stdout);

                    strcpy(cmd, CMD_PATH"/ting -o ");
                    strcat(cmd, get_field(&f[b_name]));
                    strcat(cmd, " ");
                    strcat(cmd, get_field(&f[src_name]));

                    if(system(cmd) < 0)
                      perror("Error launching TING\n");

                    locate(OFFSET_I + 2, OFFSET_J);
                    printf("    Network generation completed, RETURN to resume...");

                    getchar();
                    getchar(); /* for a bug in libraries */
                  }
                else
                  for(fn = 0; fn < FNAME_NUMBER; fn++)
                    if(c == f[fn].key)
                      {
                        input_field(&f[fn]);

                        if(fn == b_name)
                          {
                            strcpy(f[ic_name].defval, get_field(&f[b_name]));
                            strcat(f[ic_name].defval, STATE_SUFFIX);

                            strcpy(f[log_name].defval, get_field(&f[b_name]));
                            strcat(f[log_name].defval, LOG_SUFFIX);

                            strcpy(f[src_name].defval, get_field(&f[b_name]));
                          }

                        break;
                      }
            }

          if(c == 'X')
            break;
        }

      base_name = get_field(&f[b_name]);
      echo_stdout = *get_field(&f[display_switch]) == 'Y';
      file_io = *get_field(&f[file_switch]) == 'Y';
      quiet = *get_field(&f[quiet_switch]) == 'Y';
      step = atof(get_field(&f[sampling_time]));
      num_threads = atoi(get_field(&f[process_number]));
      prob = atof(get_field(&f[truth_prob]));
      horizon_len = atof(get_field(&f[horizon]));

      if(num_threads == 1)
        execname = "tinx";
      else
        execname = "tinx_mt";

      strcpy(cmd, CMD_PATH"/");
      strcat(cmd, execname);

      for(fn = 0; fn < FNAME_NUMBER; fn++)
        strcat(cmd, f[fn].rv);

      if(!quiet)
        {
          if(echo_stdout)
            strcat(cmd, " &");
          else
            strcat(cmd, " >tinx_output.txt &");
        }

      if(!batch)
        {
          clear_screen();
          locate(OFFSET_I, OFFSET_J);
          printf("*** Temporal Inference Network eXecutor Shell "VER" ***");
          locate(OFFSET_I + 2, OFFSET_J);
          printf("    Execution running, Ctrl+C to stop...");
          locate(OFFSET_I + 4, OFFSET_J);
          fflush(stdout);
        }

      if(system(cmd) < 0)
        perror("Error launching TINX\n");
      else
        if(!quiet)
          {
            tint(base_name, file_io, step, horizon_len, prob, batch);

            strcpy(cmd, "ps >tinx_ps.txt h -C ");
            strcat(cmd, execname);
            do
              {
                system(cmd);

                fp = fopen("tinx_ps.txt", "r");
                c = fgetc(fp);
                fclose(fp);
              }
            while(c != EOF);

            if(!batch && !echo_stdout)
              {
                printf("\n      Output follows...\n");

                fp = fopen("tinx_output.txt", "r");

                for(;;)
                  {
                    c = fgetc(fp);
                    if(c == EOF)
                      break;

                    putchar(c);
                  }

                fclose(fp);
              }
          }

      if(!batch)
          {
            locate(OFFSET_I + 2, OFFSET_J);
            printf("    Execution halted, RETURN to resume...");

            getchar();
            getchar(); /* for a bug in libraries */
          }
    }
  while(!batch);

  if(!batch)
    clear_screen();

  return EXIT_SUCCESS;
}


