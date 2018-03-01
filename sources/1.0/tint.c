/*
  TINT - Temporal Inference Network Tester
  Design & coding by Andrea Giotti, 1998-1999
*/

#include <time.h>

#if !defined CLOCKS_PER_SEC
#define CLOCKS_PER_SEC CLK_TCK
#endif

#include "tinx.h"

#define VER "0.50"

#define MAX_FILES 32
#define HORIZON_SIZE 64

#define SKIP_FMT "%*[^?!]"

#define DISPLAY_LO_CHAR '-'
#define DISPLAY_HI_CHAR '#'

/* ANSI control sequences */
#define ESC                     "\x1b"
#define CSI                     ESC "["
#define HOME                    CSI "H"
#define ERASE2EOS               CSI "J"
#define ERASE2EOL               CSI "K"
#define CLS                     HOME ERASE2EOS
#define LOCATE(i, j)            CSI i ";" j "H"
#define SET_COLORS(fg, bg)      CSI "1;3" fg ";4" bg "m"
#define RESET_COLORS            CSI "m"

#define DEFAULT_STEP .05

int main(int argc, char *argv[])
{
  FILE *bp, *fp[MAX_FILES], *gp[MAX_FILES];
  int fn, gn, i;
  float step;
  char name[MAX_STRLEN],
       fnames[MAX_FILES][MAX_STRLEN], gnames[MAX_FILES][MAX_STRLEN];
  d_time t, k, tau[MAX_FILES];
  char memory_f[MAX_FILES][HORIZON_SIZE], memory_g[MAX_FILES][HORIZON_SIZE];
  char c, ic, oc;
  clock_t deadline;
  bool quiet;

  *name = '\0';
  quiet = FALSE;
  step = DEFAULT_STEP;

  for(i = 1; i < argc; i++)
    {
      if(*argv[i] == '-')
        {
          switch(*(argv[i] + 1))
            {
            case 'h':
              fprintf(stderr, "Usage: %s [-q] [-t step] [base]\n", argv[0]);
              exit(EXIT_SUCCESS);
              break;

            case 'q':
              quiet = TRUE;
              break;

            case 't':
              if(++i < argc)
                {
                  step = atof(argv[i]);
                  if(step < 0)
                    {
                      fprintf(stderr, "%f: Argument out of range\n", step);
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
              fprintf(stderr,
                      "%s: Invalid command line option (%s -h for help)\n",
                      argv[i], argv[0]);
              exit(EXIT_FAILURE);
            }
        }
      else
        {
          if(!*name)
            strcpy(name, argv[i]);
          else
            fprintf(stderr, "%s: Extra argument ignored\n", argv[i]);
        }
    }

  if(!*name)
    strcpy(name, DEFAULT_NAME);

  printf("\nTINT "VER" - Temporal Inference Network Tester\n"
         "Design & coding by Andrea Giotti, 1998-1999\n\n");

  strcat(name, NETWORK_EXT);
  bp = fopen(name, "r");

  if(!bp)
    {
      perror(name);
      exit(EXIT_FAILURE);
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
              exit(EXIT_FAILURE);
            }
          strcpy(fnames[fn], name);
          strcat(fnames[fn], STREAM_EXT);

          fp[fn] = fopen(fnames[fn], "w");
          if(!fp[fn])
            {
              perror(fnames[fn]);
              exit(EXIT_FAILURE);
            }
          fn++;
          break;

        case '?':
          if(!quiet)
            {
              if(gn >= MAX_FILES)
                {
                  fprintf(stderr, "Too many output files\n");
                  exit(EXIT_FAILURE);
                }
              strcpy(gnames[gn], name);
              strcat(gnames[gn], STREAM_EXT);

              remove(gnames[gn]);
              gn++;
            }
          break;

        default:
          assert(FALSE);
        }
    }

  if(ferror(bp))
    {
      perror(name);
      exit(EXIT_FAILURE);
    }

  if(fclose(bp))
    {
      perror(name);
      exit(EXIT_FAILURE);
    }

  for(i = 0; i < fn; i++)
    for(k = 0; k < HORIZON_SIZE; k++)
      memory_f[i][k] = ' ';

  if(!quiet)
    {
      printf("Waiting for TINX host...\n");

      for(i = 0; i < gn; i++)
        {
          do
            gp[i] = fopen(gnames[i], "r");
          while(!gp[i]);

          for(k = 0; k < HORIZON_SIZE; k++)
            memory_g[i][k] = ' ';

          tau[i] = 0;
        }

      printf(CLS);
    }

  t = 0;
  deadline = 0;
  for(;;)
    {
      if(clock() >= deadline)
        {
          deadline = clock() + CLOCKS_PER_SEC * step;

          if(!quiet)
            {
              printf(HOME""SET_COLORS("1","0"));

              for(i = 0; i < fn; i++)
                {
                  memory_f[i][t % HORIZON_SIZE] = ' ';

                  for(k = t; k < t + HORIZON_SIZE; k++)
                    putchar(memory_f[i][k % HORIZON_SIZE]);

                  printf(" [I] %s\n", fnames[i]);
                }

              printf(SET_COLORS("2","0"));

              for(i = 0; i < gn; i++)
                {
                  while(tau[i] < t)
                    {
                      ic = fgetc(gp[i]);

                      if(ic == EOF)
                        {
                          if(ferror(gp[i]))
                            {
                              perror(gnames[i]);
                              exit(EXIT_FAILURE);
                            }
                          else
                            {
                              clearerr(gp[i]);
                              break;
                            }
                        }

                      switch(ic)
                        {
                        case LO_CHAR:
                          memory_g[i][tau[i] % HORIZON_SIZE] = DISPLAY_LO_CHAR;
                          break;

                        case HI_CHAR:
                          memory_g[i][tau[i] % HORIZON_SIZE] = DISPLAY_HI_CHAR;
                          break;

                        default:
                          memory_g[i][tau[i] % HORIZON_SIZE] = ' ';
                          break;
                        }

                      tau[i]++;
                    }

                  memory_g[i][t % HORIZON_SIZE] = ' ';

                  for(k = t; k < t + HORIZON_SIZE; k++)
                    putchar(memory_g[i][k % HORIZON_SIZE]);

                  printf(" [O] %s\n", gnames[i]);
                }

              printf(RESET_COLORS);
              fflush(stdout);
            }

          if(t >= TIME_LIMIT)
            break;

          for(i = 0; i < fn; i++)
            {
              oc = (rand() <= RAND_MAX / 2)? LO_CHAR : HI_CHAR;

              if(fputc(oc, fp[i]) == EOF)
                {
                  perror(fnames[i]);
                  exit(EXIT_FAILURE);
                }

              if(fflush(fp[i]))
                {
                  perror(fnames[i]);
                  exit(EXIT_FAILURE);
                }

              memory_f[i][t % HORIZON_SIZE] = (oc == LO_CHAR)?
                                              DISPLAY_LO_CHAR : DISPLAY_HI_CHAR;
            }

          t++;
        }
    }

  for(i = 0; i < fn; i++)
    {
      if(fputc(END_CHAR, fp[i]) == EOF)
        {
          perror(fnames[i]);
          exit(EXIT_FAILURE);
        }

      if(fclose(fp[i]))
        {
          perror(fnames[i]);
          exit(EXIT_FAILURE);
        }
    }

  if(!quiet)
    for(i = 0; i < gn; i++)
      if(fclose(gp[i]))
        {
          perror(gnames[i]);
          exit(EXIT_FAILURE);
        }

  printf("\nEnd of generation\n");

  return EXIT_SUCCESS;
}

