/*
  TING - Temporal Inference Network Generator
  Design & coding by Andrea Giotti, 2017-2025
*/

#define NDEBUG

#include "ting_core.h"
#include "ting_parser.h"
#include "ting_lexer.h"

#define VER "11.0.0"

const char class_symbol[NODE_CLASSES_NUMBER] = CLASS_SYMBOLS;
const char *signal_class[IO_CLASSES_NUMBER] = { "internal", "auxiliary", "input", "output" };
const char *color_name_light[NUM_COLORS] = { "green4", "yellow4", "orange4", "red4", "purple4" };
const char *color_name_dark[NUM_COLORS] = { "green1", "yellow1", "orange1", "red1", "purple1" };

int yyparse(btl_specification **spec, yyscan_t scanner);

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

void printmsg(bool fancymsg, int color, char *format, ...)
{
  char buffer[DEBUG_STRLEN], buffer2[DEBUG_STRLEN];
  char *d, *e, *f;
  va_list arglist;

  va_start(arglist, format);
  vsprintf(buffer, format, arglist);
  va_end(arglist);

  if(fancymsg)
    {
      d = buffer;
      e = buffer2;

      *e = '\0';

      while(*d)
        {
          f = filterchar(*d);

          strcat(e, f);

          d++;
          e += strlen(f);
        }

      if(e > buffer2 && *(e - 1) == '\n')
        *(e - 1) = '\0';

      if(!color || color > NUM_COLORS)
        printf("%s\n", buffer2);
      else
        printf("<span foreground='%s'>%s</span>\n", fancymsg > 1? color_name_dark[color - 1] : color_name_light[color - 1], buffer2);
    }
  else
    fprintf(stderr, "%s", buffer);
}

void printerr(bool fancymsg, char *format, ...)
{
  char buffer[MAX_STRLEN], buffer2[MAX_STRLEN];
  va_list arglist;

  va_start(arglist, format);
  vsprintf(buffer, format, arglist);
  va_end(arglist);

  if(errno && *buffer)
    sprintf(buffer2, "%s: %s\n", buffer, strerror(errno));
  else
    sprintf(buffer2, "%s\n", errno? strerror(errno) : buffer);

  printmsg(fancymsg, 4, buffer2);
}

void exit_failure()
{
  printmsg(0, 0, "Network not generated\n");
  exit(EXIT_FAILURE);
}

char *hilt(bool fancymsg, char *s)
{
  char *buffer;

  if(fancymsg)
    {
      buffer = malloc(MAX_STRLEN);
      if(!buffer)
        {
          printerr(fancymsg, NULL);
          exit_failure();
        }

      sprintf(buffer, "<b>%s</b>", s);

      return buffer;
    }
  else
    return s;
}

btl_specification *alloc_syntnode()
{
  btl_specification *sp;

  sp = malloc(sizeof(btl_specification));
  if(!sp)
    return NULL;

  sp->ot = op_invalid;
  sp->symbol[0] = '\0';
  sp->value = NULL_TIME;
  sp->realval = REAL_MAX;

  sp->left = NULL;
  sp->right = NULL;

  sp->debug[0] = '\0';

  return sp;
}

btl_specification *create_ground(op_type ot, char *symbol, d_time value, real realval)
{
  char name[MAX_NAMELEN], args[MAX_NAMELEN];
  btl_specification *sp;

  if(strlen(symbol) >= MAX_STRLEN)
    {
      printmsg(0, 0, "%s: Error, string too long\n", symbol);
      exit_failure();
    }

  sp = alloc_syntnode();
  if(!sp)
    {
      perror(NULL);
      exit_failure();
    }

  sp->ot = ot;
  sp->value = value;
  sp->realval = realval;

  if(symbol[0])
    {
      *args = '\0';
      if(!sscanf(symbol, REALRICH_FMT"%s", name, args))
        if(!sscanf(symbol, RICHNAME_FMT"%s", name, args))
          strcpy(sp->symbol, symbol);
        else
          sprintf(sp->symbol, "%s%s", name, args);
      else
        sprintf(sp->symbol, "%s%s", name, args);
      
      strcpy(sp->debug, symbol);
    }
  else
    {
      sp->symbol[0] = '\0';

      if(value != NULL_TIME)
        sprintf(sp->debug, TIME_FMT, value);
      else
        if(realval != REAL_MAX)
          sprintf(sp->debug, REAL_OUT_FMT, realval);
        else
          *sp->debug = '\0';
    }

  return sp;
}

btl_specification *create_operation(op_type ot, btl_specification *left, btl_specification *right, char *debug)
{
  btl_specification *sp;

  sp = alloc_syntnode();
  if(!sp)
    {
      perror(NULL);
      exit_failure();
    }

  sp->ot = ot;
  sp->left = left;
  sp->right = right;

  assert(left);

  if(right)
    snprintf(sp->debug, DEBUG_STRLEN, debug, left->debug, right->debug);
  else
    snprintf(sp->debug, DEBUG_STRLEN, debug, left->debug);

  if(strlen(sp->debug) == DEBUG_STRLEN - 1)
    {
      strcpy(sp->debug, "<symbol>");
      printmsg(0, 0, "Warning, symbol table overflow in first phase\n");
    }

  return sp;
}

char *richwrap(char *string)
{
  char buffer[MAX_NAMELEN];

  sprintf(buffer, "`%s`", string);
  strcpy(string, buffer);

  return string;
}

char *richwrap2(char *string)
{
  char buffer[MAX_NAMELEN];

  sprintf(buffer, "``%s``", string);
  strcpy(string, buffer);

  return string;
}

void delete_specification(btl_specification *sp)
{
  if(!sp)
    return;

  delete_specification(sp->left);
  delete_specification(sp->right);

  free(sp);
}

btl_specification *copy_specification(btl_specification *sp)
{
  btl_specification *rp;

  if(!sp)
    return NULL;

  rp = alloc_syntnode();
  if(!rp)
    {
      perror(NULL);
      exit_failure();
    }

  rp->ot = sp->ot;

  strcpy(rp->symbol, sp->symbol);
  rp->value = sp->value;
  rp->realval = sp->realval;

  rp->left = copy_specification(sp->left);
  rp->right = copy_specification(sp->right);

  strcpy(rp->debug, sp->debug);

  return rp;
}

smallnode *create_smallnode(c_base *cb, node_class nclass)
{
  smallnode *vp;

  vp = (smallnode *)malloc(sizeof(smallnode));
  if(!vp || vp == SPECIAL)
    {
      printerr(cb->fancymsg, "Error, memory manager failure\n");
      return NULL;
    }

  sprintf(vp->name, "%c%06d", class_symbol[nclass], cb->part_nodes[nclass]);
  cb->part_nodes[nclass]++;

  vp->root[0] = '\0';
  vp->nclass = nclass;
  vp->k = NULL_TIME;
  vp->realval = REAL_MAX;
  vp->neg = FALSE;
  vp->zombie = FALSE;
  vp->turn = 0;

  vp->up = NULL;
  vp->up_2 = NULL;
  vp->left = NULL;
  vp->right = NULL;

  vp->up_dir = no_link;
  vp->up_2_dir = no_link;
  vp->left_dir = no_link;
  vp->right_dir = no_link;

  vp->debug[0] = '\0';

  vp->vp = cb->network;
  cb->network = vp;

  return vp;
}

void delete_smallnode(c_base *cb, smallnode *vp)
{
  smallnode *wp, *zp;

  assert(vp);

  wp = cb->network;
  zp = NULL;

  while(wp && wp != vp)
    {
      zp = wp;
      wp = wp->vp;
    }

  if(zp)
    zp->vp = vp->vp;
  else
    cb->network = vp->vp;

  cb->part_nodes[vp->nclass]--;

  free(vp);
}

void delete_zombies(c_base *cb)
{
  smallnode *vp, *wp, *zp;
  io_signal *sp;

  vp = cb->network;
  zp = NULL;

  while(vp)
    {
      wp = vp->vp;

      if(vp->zombie)
        {
          if((vp->up && vp->up != SPECIAL) || (vp->left && vp->left != SPECIAL) || (vp->right && vp->right != SPECIAL))
            printmsg(cb->fancymsg, 5, "%s: Warning, abnormal node removed from network: %s\n", vp->name, vp->debug);

          if(zp)
            zp->vp = wp;
          else
            cb->network = wp;

          cb->part_nodes[vp->nclass]--;

          sp = cb->sigtab;
          while(sp)
            {
              if(sp->from == vp || sp->to == vp)
                {
                  sp->from = NULL;
                  sp->to = NULL;

                  sp->removed = TRUE;

                  if(sp->sclass != internal_class)
                    printmsg(cb->fancymsg, sp->sclass == input_class || sp->sclass == output_class? 2 : 1, "%s: Warning, %s signal removed\n", sp->name, signal_class[sp->sclass]);
                }

              sp = sp->nextsig;
            }
            
          free(vp);
        }
      else
        zp = vp;

      vp = wp;
    }
}

void delete_smalltree(c_base *cb)
{
  smallnode *vp, *wp;

  vp = cb->network;

  while(vp)
    {
      wp = vp->vp;

      free(vp);

      vp = wp;
    }
}

btl_specification *parse(char *expr)
{
  btl_specification *spec;
  yyscan_t scanner;
  YY_BUFFER_STATE state;

  if(yylex_init(&scanner))
    {
      printmsg(0, 0, "Error, internal lexical analyzer failure\n");
      return NULL;
    }

  state = yy_scan_string(expr, scanner);

  if(yyparse(&spec, scanner))
    return NULL;

  yy_delete_buffer(state, scanner);

  yylex_destroy(scanner);

  return spec;
}

unsigned long int hashsymbol(char *name)
{
  unsigned long int k;

  k = 0;

  while(*name)
    {
      k = k * 36;

      if(isdigit(*name))
        k += *name - '0';
      else
        if(isalpha(*name))
          k += toupper(*name) - 'A' + 10;

      name++;
    }

  return k;
}

smallnode *name2smallnode(c_base *cb, char *name, bool create)
{
  smallnode *vp, *wp;
  symlist *si, *sl;
  int h, i;

  if(*name)
    {
      if(strlen(name) >= MAX_NAMELEN)
        {
          printmsg(cb->fancymsg, 4, "%s: Error, name too long\n", name);
          exit_failure();
        }

      h = hashsymbol(name) % SYMTAB_SIZE;
      i = 0;

      while((vp = cb->symptr[h][i]))
        {
          if(strcmp(vp->name, name))
            {
              i++;

              if(i == SYMTAB_DEPTH)
                {
                  printmsg(cb->fancymsg, 4, "%s, %s: Error, node names generate duplicate hashes\n", vp->name, name);
                  exit_failure();
                }
            }
          else
            {
              if(create)
                {
                  wp = create_smallnode(cb, literal);
                  if(!wp)
                   {
                     printerr(cb->fancymsg, NULL);
                     exit_failure();
                   }

                  si = vp->literal_symtab;
                  sl = malloc(sizeof(symlist));
                  if(!sl)
                    {
                      printerr(cb->fancymsg, NULL);
                      exit_failure();
                    }

                  sl->vp = vp;
                  sl->nextocc = si->nextocc;
                  sl->nextlit = si->nextlit;

                  vp->literal_symtab = sl;

                  si->vp = wp;
                  si->nextocc = sl;

                  strcpy(wp->name, name);
                  wp->literal_symtab = si;
                }
              else
                wp = vp;

              return wp;
            }
        }

      if(create)
        {
          vp = create_smallnode(cb, literal);
          if(!vp)
            {
              printerr(cb->fancymsg, NULL);
              exit_failure();
            }

          si = malloc(sizeof(symlist));
          if(!si)
            {
              printerr(cb->fancymsg, NULL);
              exit_failure();
            }

          si->vp = vp;
          si->nextocc = NULL;
          si->nextlit = cb->symtab;

          cb->symtab = si;

          strcpy(vp->name, name);
          vp->literal_symtab = si;

          cb->symptr[h][i] = vp;

          return vp;
        }
      else
        return NULL;
    }
  else
    return NULL;
}

io_signal *name2signal(c_base *cb, char *name, bool create)
{
  io_signal *sp;
  int h, i;

  if(*name)
    {
      if(strlen(name) >= MAX_NAMELEN)
        {
          printmsg(cb->fancymsg, 4, "%s: Error, name too long\n", name);
          exit_failure();
        }

      h = hashsymbol(name) % SYMTAB_SIZE;
      i = 0;

      while((sp = cb->sigptr[h][i]))
        {
          if(strcmp(sp->name, name))
            {
              i++;

              if(i == SYMTAB_DEPTH)
                {
                  printmsg(cb->fancymsg, 4, "%s, %s: Error, signal names generate duplicate hashes\n",
                      sp->name, name);
                  exit_failure();
                }
            }
          else
            {
              if(create && sp->shared)
                break;
              else
                return sp;
            }
        }

      if(create)
        {
          sp = malloc(sizeof(io_signal));
          if(!sp)
            {
              printerr(cb->fancymsg, NULL);
              exit_failure();
            }

          strcpy(sp->name, name);
          strcpy(sp->root, name);
          sp->from = NULL;
          sp->to = NULL;
          sp->tofrom = NULL;
          sp->fromto = NULL;
          sp->sclass = internal_class;
          sp->stype = io_any;
          sp->packed = 0;
          sp->packedbit = 0;
          sp->defaultval = io_unknown;
          sp->omissions = io_raw;
          sp->defaultreal = REAL_MAX;
          sp->val = undefined;
          sp->shared = FALSE;
          sp->removed = FALSE;
          sp->nextsig = cb->sigtab;
          
          cb->sigtab = sp;
          cb->num_signals++;

          cb->part_signals[internal_class]++;
          cb->sigptr[h][i] = sp;

          return sp;
        }
      else
        return NULL;
    }
  else
    return NULL;
}

constant *name2constant(c_base *cb, char *name, bool create)
{
  constant *tp;
  int h, i;

  if(*name)
    {
      if(strlen(name) >= MAX_NAMELEN)
        {
          printmsg(cb->fancymsg, 4, "%s: Error, name too long\n", name);
          exit_failure();
        }

      h = hashsymbol(name) % SYMTAB_SIZE;
      i = 0;

      while((tp = cb->intptr[h][i]))
        {
          if(strcmp(tp->name, name))
            {
              i++;

              if(i == SYMTAB_DEPTH)
                {
                  printmsg(cb->fancymsg, 4, "%s, %s: Error, variable names generate duplicate hashes\n",
                      tp->name, name);
                  exit_failure();
                }
            }
          else
            return tp;
        }

      if(create)
        {
          tp = malloc(sizeof(constant));
          if(!tp)
            {
              printerr(cb->fancymsg, NULL);
              exit_failure();
            }

          strcpy(tp->name, name);
          tp->scope_unit = 0;
          tp->scope_level = 0;
          tp->error = FALSE;
          tp->nextcon = cb->inttab;
          
          cb->inttab = tp;

          cb->intptr[h][i] = tp;

          return tp;
        }
      else
        return NULL;
    }
  else
    return NULL;
}

group *name2group(c_base *cb, char *name, bool create)
{
  group *grpp;
  int h, i;

  if(*name)
    {
      if(strlen(name) >= MAX_NAMELEN)
        {
          printmsg(cb->fancymsg, 4, "%s: Error, name too long\n", name);
          exit_failure();
        }

      h = hashsymbol(name) % SYMTAB_SIZE;
      i = 0;

      while((grpp = cb->grpptr[h][i]))
        {
          if(strcmp(grpp->name, name))
            {
              i++;

              if(i == SYMTAB_DEPTH)
                {
                  printmsg(cb->fancymsg, 4, "%s, %s: Error, packed signal names generate duplicate hashes\n",
                      grpp->name, name);
                  exit_failure();
                }
            }
          else
            return grpp;
        }

      if(create)
        {
          grpp = malloc(sizeof(group));
          if(!grpp)
            {
              printerr(cb->fancymsg, NULL);
              exit_failure();
            }

          strcpy(grpp->name, name);
          grpp->group_id = cb->num_groups;
          grpp->nextgrp = cb->grptab;
          
          cb->grptab = grpp;
          cb->num_groups++;

          cb->grpptr[h][i] = grpp;

          return grpp;
        }
      else
        return NULL;
    }
  else
    return NULL;
}

void add_ic(c_base *cb, char *name, bool neg, d_time t, real realval)
{
  initial_condition *icp;

  if(strlen(name) >= MAX_NAMELEN)
    {
      printmsg(cb->fancymsg, 4, "%s: Error, name too long\n", name);
      exit_failure();
    }

  icp = malloc(sizeof(initial_condition));
  if(!icp)
    {
      printerr(cb->fancymsg, NULL);
      exit_failure();
    }

  strcpy(icp->name, name);
  icp->neg = neg;
  icp->t = t;
  icp->realval = realval;
  icp->nextic = cb->ictab;
          
  cb->ictab = icp;
  cb->num_ics++;
}

void gensym(c_base *cb, char *symbol, char *type, litval val, bool incr)
{
  char c;

  switch(val)
    {
      case negated:
        c = '-';
      break;

      case asserted:
        c = '+';
      break;

      case undefined:
        c = '_';
      break;

      case continuos:
        c = '*';
      break;

      default:
        c = '_';
      break;
    }

  sprintf(symbol, "%c%s%d", c, type, cb->num_vargen);
  assert(strlen(symbol) <= MAX_NAMELEN);

  if(incr)
    cb->num_vargen++;
}

char *opname(op_type ot)
{
  switch(ot)
    {
      case op_not:
        return "~";
      break;

      case op_internal:
        return "internal";
      break;

      case op_aux:
        return "aux";
      break;

      case op_input:
        return "input";
      break;

      case op_output:
        return "output";
      break;

      case op_init:
        return "init";
      break;

      case op_join:
        return ";";
      break;

      case op_join_qual:
        return ",";
      break;

      case op_and:
        return "&";
      break;

      case op_or:
        return "|";
      break;

      case op_delay:
      case op_math_delay:
        return "@";
      break;

      case op_plus:
        return "+";
      break;

      case op_minus:
        return "-";
      break;

      case op_mul:
        return "*";
      break;

      case op_div:
        return "/";
      break;

      case op_equal:
      case op_assign:
        return "=";
      break;

      case op_neq:
        return "~=";
      break;

      case op_lt:
        return "<";
      break;

      case op_gt:
        return ">";
      break;

      case op_lteq:
        return "<=";
      break;

      case op_gteq:
        return ">=";
      break;

      default:
        assert(FALSE);
      break;
    }

  return "";
}

void print_specification(btl_specification *sp)
{
  if(!sp)
    return;

  printf("(");

  print_specification(sp->left);

  printf("[%d=%s/%s|%ld]", sp->ot, opname(sp->ot), sp->symbol, sp->value);

  print_specification(sp->right);

  printf(")");
}

d_time extract_root(char *root, char *fullname)
{
  char name[MAX_NAMELEN], args[MAX_NAMELEN];
  d_time idx, idx_2, idx_3, idx_4;

  *args = '\0';
  sscanf(fullname, VARNAME_FMT"%s", name, args);
 
  idx = LONG_MIN;
  idx_2 = LONG_MIN;
  idx_3 = LONG_MIN;
  idx_4 = LONG_MIN;

  sscanf(args, "( "TIME_FMT" , "TIME_FMT" , "TIME_FMT" , "TIME_FMT" )", &idx_4, &idx_3, &idx_2, &idx);

  if(idx == LONG_MIN)
    {
      sscanf(args, "( "TIME_FMT" , "TIME_FMT" , "TIME_FMT" )", &idx_3, &idx_2, &idx);
                
      if(idx == LONG_MIN)
        {
          sscanf(args, "( "TIME_FMT" , "TIME_FMT" )", &idx_2, &idx);

          if(idx == LONG_MIN)
            sscanf(args, "( "TIME_FMT" )", &idx);
          else
            idx = 8 * idx_2 + idx;

          strcpy(root, name);
        }
      else
        {
          idx = 8 * idx_2 + idx;
          sprintf(root, "%s("TIME_FMT")", name, idx_3);
        }
    }
   else
    {
      idx = 8 * idx_2 + idx;
      sprintf(root, "%s("TIME_FMT","TIME_FMT")", name, idx_4, idx_3);
    }

  return idx;
}

void compose_root(char *root, char *fullname, d_time k)
{
  char name[MAX_NAMELEN], args[MAX_NAMELEN];
  d_time idx, idx_2, idx_3, idx_4;

  *args = '\0';
  sscanf(fullname, VARNAME_FMT"%s", name, args);
 
  idx = LONG_MIN;
  idx_2 = LONG_MIN;
  idx_3 = LONG_MIN;
  idx_4 = LONG_MIN;

  sscanf(args, "( "TIME_FMT" , "TIME_FMT" , "TIME_FMT" , "TIME_FMT" )", &idx_4, &idx_3, &idx_2, &idx);

  if(idx == LONG_MIN)
    {
      sscanf(args, "( "TIME_FMT" , "TIME_FMT" , "TIME_FMT" )", &idx_3, &idx_2, &idx);
                
      if(idx == LONG_MIN)
        {
          sscanf(args, "( "TIME_FMT" , "TIME_FMT" )", &idx_2, &idx);

          if(idx == LONG_MIN)
            sprintf(root, "%s("TIME_FMT")", name, k);
          else
            sprintf(root, "%s("TIME_FMT","TIME_FMT")", name, k / 8, k % 8);
        }
       else
        sprintf(root, "%s("TIME_FMT","TIME_FMT","TIME_FMT")", name, idx_3, k / 8, k % 8);
    }
   else
     sprintf(root, "%s("TIME_FMT","TIME_FMT","TIME_FMT","TIME_FMT")", name, idx_4, idx_3, k / 8, k % 8);
}

int check_root(char *fullname)
{
  char name[MAX_NAMELEN], args[MAX_NAMELEN];
  d_time idx, idx_2, idx_3, idx_4;

  *args = '\0';
  sscanf(fullname, VARNAME_FMT"%s", name, args);
 
  idx = LONG_MIN;
  idx_2 = LONG_MIN;
  idx_3 = LONG_MIN;
  idx_4 = LONG_MIN;

  sscanf(args, "( "TIME_FMT" , "TIME_FMT" , "TIME_FMT" , "TIME_FMT" )", &idx_4, &idx_3, &idx_2, &idx);

  if(idx == LONG_MIN)
    {
      sscanf(args, "( "TIME_FMT" , "TIME_FMT" , "TIME_FMT" )", &idx_3, &idx_2, &idx);
                
      if(idx == LONG_MIN)
        {
          sscanf(args, "( "TIME_FMT" , "TIME_FMT" )", &idx_2, &idx);

          if(idx == LONG_MIN)
            {
              sscanf(args, "( "TIME_FMT" )", &idx);

              if(idx == LONG_MIN)
                return 0;
              else
                return 1; 
            }
          else
            return 2;
        }
      else
        return 3;
    }
   else
     return 4;
}

int get_depth(btl_specification *spec)
{
  btl_specification *spec2;
  int dlevel;

  dlevel = 0;

  if(spec->ot == op_range)
     dlevel++;
  else
    if(spec->ot == op_interval_1)
      {
        if(spec->left->ot == op_range)
          dlevel++;

        if(spec->right->ot == op_range)
          dlevel++;
      }
    else
      if(spec->ot == op_fourdim)
        {
          spec2 = spec->left;

          if(spec2->left->ot == op_range)
            dlevel++;

          if(spec2->right->ot == op_range)
            dlevel++;

          spec2 = spec->right;

          if(spec2->left->ot == op_range)
            dlevel++;

          if(spec2->right->ot == op_range)
            dlevel++;
        }

  return dlevel;
}

btl_specification *create_equal(btl_specification *p, btl_specification *q)
{
  if(q)
    {
      if(p)
        p = create_operation(op_plus, p, create_operation(op_chs, q, NULL, "(- %s)"), "(%s + %s)");
      else
        p = q;
    }

  p = create_operation(op_math_eqv0, p, NULL, "(%s = 0)");

  return p;
}

subtreeval preval(c_base *cb, btl_specification *spec, int level, d_time param, real realval)
{
  subtreeval stv, stv_2, stv_3, stv_4;
  char symbol[MAX_STRLEN], symbol1[MAX_NAMELEN], symbol2[MAX_NAMELEN], debug[MAX_STRLEN], filename[MAX_STRLEN];
  btl_specification *e, *p, *q, *r, *p1, *np, *np1;
  constant *tp;
  d_time val, i, j, h, k, n;
  real result;
  int l, start, end, arity;
  op_type ot;
  FILE *fp;
  char c;
  char *endp;

  stv.btl = NULL;
  stv.btltwo = NULL;
  stv.btldef = NULL;
  stv.vp = NULL;
  stv.a = NULL_TIME;
  stv.b = NULL_TIME;
  stv.c = NULL_TIME;
  stv.d = NULL_TIME;
  stv.xtra = NULL;
  stv.ytra = NULL;
  stv.ztra = NULL;
  stv.wtra = NULL;
  stv.realval = REAL_MAX;
  stv.neg = FALSE;

  if(!spec)
    return stv;

  if(level >= NUM_LEVELS - 1)
    {
      printmsg(cb->fancymsg, 4, "Error, too many nested quantifiers: %s\n", spec->debug);
      exit_failure();
    }

  switch(spec->ot)
    {
      case op_fname:
        strcpy(filename, cb->path);
        strcat(filename, spec->symbol);

        fp = fopen(filename, "r");
        if(!fp)
          {
            printerr(cb->fancymsg, filename);
            exit_failure();
          }

        memset(cb->source, 0, SOURCE_BUFSIZE);

        if(fread(cb->source, sizeof(char), SOURCE_BUFSIZE, fp) == SOURCE_BUFSIZE)
          {
            printmsg(cb->fancymsg, 4, "%s: Error, source file too large\n", filename);
            exit_failure();
          }

        if(ferror(fp))
          {
            printerr(cb->fancymsg, filename);
            exit_failure();
          }

        fclose(fp);

        printf("\tCompiling module %s ...\n", hilt(cb->fancymsg, spec->symbol));

        e = parse(cb->source);
        if(!e)
          exit_failure();

        stv = preval(cb, e, 0, NULL_TIME, REAL_MAX);

        delete_specification(e);

        printf("\t... %s syntax ok\n", hilt(cb->fancymsg, spec->symbol));
      break;

      case op_define:
        stv_2 = preval(cb, spec->right, level, param, realval);
        stv = preval(cb, spec->left, level, stv_2.a, stv_2.realval);

        delete_specification(stv_2.btl);
      break;

      case op_defarray:
        if(spec->right->ot == op_join_array || spec->right->ot == op_down)
          {
            stv_2 = preval(cb, spec->right, level, -4, realval);
            delete_specification(stv_2.btl);
          }
        else
          {
            stv_2.a = NULL_TIME;
            stv_2.b = NULL_TIME;
            stv_2.c = NULL_TIME;
            stv_2.d = 0;
          }

        n = 0;

        if(stv_2.d == NULL_TIME)
          stv_2.d = 0;
        else
          n++;

        if(stv_2.c == NULL_TIME)
          {
            stv_2.c = stv_2.d;
            stv_2.d = 0;
          }
        else
          n++;

        if(stv_2.b == NULL_TIME)
          {
            stv_2.b = stv_2.c;
            stv_2.c = stv_2.d;
            stv_2.d = 0;
          }
        else
          n++;

        if(stv_2.a == NULL_TIME)
          {
            stv_2.a = stv_2.b;
            stv_2.b = stv_2.c;            
            stv_2.c = stv_2.d;
            stv_2.d = 0;
          }
        else
          n++;

        if(!stv_2.a || (n > 1 && !stv_2.b) || (n > 2 && !stv_2.c) || (n > 3 && !stv_2.d))
          printmsg(cb->fancymsg, 2, "Warning, array contains a single row or column: %s\n", spec->debug);

        p = spec->left;

        if(p->right)
          {
            val = get_depth(p->right);

            if(n != val)
              {
                printmsg(cb->fancymsg, 4, "Error, array free arguments mismatch dimension: %s\n", spec->debug);
                exit_failure();
              }
          }
        else
          {
            printmsg(cb->fancymsg, 4, "Error, array has no free arguments: %s\n", spec->debug);
            exit_failure();
          }

        for(i = stv_2.d; i >= 0; i--)
          {
            if(n > 3)
              {
                cb->iterator[level] = stv_2.d - i;
                level++;
              }

            for(j = stv_2.c; j >= 0; j--)
              {
                if(n > 2)
                  {
                    cb->iterator[level] = stv_2.c - j;
                    level++;
                  }

                for(h = stv_2.b; h >= 0; h--)
                  {
                    if(n > 1)
                      {
                        cb->iterator[level] = stv_2.b - h;
                        level++;
                      }

                    for(k = stv_2.a; k >= 0; k--)
                      {
                        cb->iterator[level] = stv_2.a - k;

                        stv_3 = preval(cb, spec->right, level, (stv_2.a + 1) * ((stv_2.b + 1) * ((stv_2.c + 1) * i + j) + h) + k, realval);
                        stv = preval(cb, spec->left, level, stv_3.a, stv_3.realval);
                      }

                    if(n > 1)
                      level--;
                  }

                if(n > 2)
                  level--;
              }

            if(n > 3)
              level--;
          }
      break;

      case op_cname:
        if(level < 0)
          stv.btl = create_ground(spec->ot, spec->symbol, param, realval);
        else
          {
            tp = name2constant(cb, spec->symbol, FALSE);
        
            if(!tp)
              {
                tp = name2constant(cb, spec->symbol, TRUE);

                tp->value = param;
                tp->realval = realval;

                tp->scope_level = level;
                tp->scope_unit = cb->scope_unit[level];
              }
            else
              {
                tp->value = param;
                tp->realval = realval;

                if(tp->scope_level > level)
                  {
                    if(tp->scope_unit == cb->scope_unit[tp->scope_level] && !tp->error)
                      {
                        tp->error = TRUE;
                        printmsg(cb->fancymsg, 2, "Warning, redefinition of variable defined in inner scope: %s\n", spec->debug);
                      }

                    tp->scope_level = level;
                  }
                else
                  if(tp->scope_level < level)
                    {
                      if(tp->scope_unit == cb->scope_unit[tp->scope_level])
                        {
                          if(!tp->error)
                            {
                              tp->error = TRUE;
                              printmsg(cb->fancymsg, 2, "Warning, redefinition of variable defined in outer scope: %s\n", spec->debug);
                            }
                        }
                      else
                        tp->scope_level = level;
                    }
          
                tp->scope_unit = cb->scope_unit[tp->scope_level];
              }
          }
      break;

      case op_constant:
        if(level < 0)
          stv.btl = create_ground(spec->ot, spec->symbol, param, realval);
        else
          {
            tp = name2constant(cb, spec->symbol, FALSE);
        
            if(!tp)
              {
                printmsg(cb->fancymsg, 4, "Error, reference to undefined variable: %s\n", spec->debug);
                exit_failure();
              }

            if(tp->scope_level <= level && tp->scope_unit == cb->scope_unit[tp->scope_level])
              {
                stv.btl = create_ground(tp->value == NULL_TIME? op_real : op_number, "", tp->value, tp->realval);
                stv.a = tp->value;
                stv.b = tp->value;
                stv.realval = tp->realval;
            
                stv.wtra = (d_time *)TRUE;
              }
            else
              {
                printmsg(cb->fancymsg, 4, "Error, reference to variable out of scope: %s\n", spec->debug);
                exit_failure();
              }
          }
      break;

      case op_name:
      case op_dname:
      case op_iname:
      case op_string:
      case op_variable:
      case op_dvariable:
      case op_ivariable:
        stv.btl = create_ground(spec->ot, spec->symbol, NULL_TIME, REAL_MAX);
      break;

      case op_number:
        stv.btl = create_ground(op_number, "", spec->value, REAL_MAX);
        stv.a = spec->value;
        stv.b = spec->value;
        stv.realval = REAL_MAX;

        stv.wtra = (d_time *)TRUE;
      break;

      case op_ioqual1:
      case op_ioqual2:
      case op_ioqual3:
      case op_ioqual4:
      case op_ioqual3b:
        if(spec->left)
          {
            stv_2 = preval(cb, spec->left, level, param, realval);

            stv.btl = create_ground(spec->ot, "", stv_2.a, stv_2.realval);
            stv.a = stv_2.a;
            stv.b = stv_2.b;
            stv.realval = stv_2.realval;

            delete_specification(stv_2.btl);
          }
        else
          {
            stv.btl = create_ground(spec->ot, "", spec->value, spec->realval);
            stv.a = spec->value;
            stv.b = spec->value;
            stv.realval = spec->realval;
          }

        stv.wtra = (d_time *)TRUE;
      break;

      case op_real:
        stv.btl = create_ground(op_real, "", NULL_TIME, spec->realval);
        stv.a = NULL_TIME;
        stv.b = NULL_TIME;
        stv.realval = spec->realval;

        stv.wtra = (d_time *)TRUE;
      break;

      case op_iterator:
        l = level - strlen(spec->symbol);
        if(l < 0)
          {
            printmsg(cb->fancymsg, 4, "Error, iterator symbol out of scope: %s\n", spec->debug);
            exit_failure();
          }

        stv.btl = create_ground(op_number, "", cb->iterator[l], REAL_MAX);
        stv.a = cb->iterator[l];
        stv.b = cb->iterator[l];

        stv.wtra = (d_time *)TRUE;
      break;

      case op_range:
        l = level - param;
        if(l < 0)
          {
            printmsg(cb->fancymsg, 4, "Error, too many free array arguments: %s\n", spec->debug);
            exit_failure();
          }

        stv.btl = create_ground(op_number, "", cb->iterator[l], REAL_MAX);
        stv.a = cb->iterator[l];
        stv.b = cb->iterator[l];

        stv.wtra = (d_time *)TRUE;
      break;

      case op_vector:
        stv = preval(cb, spec->left, -1, param, realval);
        stv_2 = preval(cb, spec->right, level, 0, realval);

        sprintf(symbol, "%s("TIME_FMT")", stv.btl->symbol, stv_2.a);
        ot = stv.btl->ot;
 
        delete_specification(stv.btl);

        stv.btl = create_ground(ot, symbol, NULL_TIME, REAL_MAX);

        if(ot == op_cname || ot == op_constant)
          {
            n = get_depth(spec->right);
            if(n > 0)
              n--;

            stv = preval(cb, stv.btl, level - n, param, realval);

            if(ot == op_cname)
              {
                delete_specification(stv.btl);
                stv.btl = NULL;
              }
          }

        delete_specification(stv_2.btl);
      break;

      case op_matrix:
        stv = preval(cb, spec->left, -1, param, realval);
        stv_2 = preval(cb, spec->right, level, 0, realval);

        sprintf(symbol, "%s("TIME_FMT","TIME_FMT")", stv.btl->symbol, stv_2.a, stv_2.b);
        ot = stv.btl->ot;
 
        delete_specification(stv.btl);

        stv.btl = create_ground(ot, symbol, NULL_TIME, REAL_MAX);

        if(ot == op_cname || ot == op_constant)
          {
            n = get_depth(spec->right);
            if(n > 0)
              n--;

            stv = preval(cb, stv.btl, level - n, param, realval);

            if(ot == op_cname)
              {
                delete_specification(stv.btl);
                stv.btl = NULL;
              }
          }

        delete_specification(stv_2.btl);
      break;

      case op_array3:
        stv = preval(cb, spec->left, -1, param, realval);
        stv_2 = preval(cb, spec->right, level, 2, realval);

        sprintf(symbol, "%s("TIME_FMT","TIME_FMT","TIME_FMT")", stv.btl->symbol, stv_2.a, stv_2.b, stv_2.c);
        ot = stv.btl->ot;
 
        delete_specification(stv.btl);

        stv.btl = create_ground(ot, symbol, NULL_TIME, REAL_MAX);

        if(ot == op_cname || ot == op_constant)
          {
            n = get_depth(spec->right);
            if(n > 0)
              n--;

            stv = preval(cb, stv.btl, level - n, param, realval);

            if(ot == op_cname)
              {
                delete_specification(stv.btl);
                stv.btl = NULL;
              }
          }

        delete_specification(stv_2.btl);
      break;

      case op_array4:
        stv = preval(cb, spec->left, -1, param, realval);
        stv_2 = preval(cb, spec->right, level, 2, realval);

        sprintf(symbol, "%s("TIME_FMT","TIME_FMT","TIME_FMT","TIME_FMT")", stv.btl->symbol, stv_2.a, stv_2.b, stv_2.c, stv_2.d);
        ot = stv.btl->ot;
 
        delete_specification(stv.btl);

        stv.btl = create_ground(ot, symbol, NULL_TIME, REAL_MAX);

        if(ot == op_cname || ot == op_constant)
          {
            n = get_depth(spec->right);
            if(n > 0)
              n--;

            stv = preval(cb, stv.btl, level - n, param, realval);

            if(ot == op_cname)
              {
                delete_specification(stv.btl);
                stv.btl = NULL;
              }
          }

        delete_specification(stv_2.btl);
      break;

      case op_fourdim:
        stv_2 = preval(cb, spec->right, level, 0, realval);
        stv = preval(cb, spec->left, level, stv_2.d, realval);

        stv.c = stv_2.a;
        stv.d = stv_2.b;

        delete_specification(stv_2.btl);
      break;

      case op_and:
      case op_or:
      case op_delay:
      case op_math_delay:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level, param, realval);

        if(stv.btl)
          {
            if(stv_2.btl)
                {
                  sprintf(debug, "(%%s %s %%s)", opname(spec->ot));
                  stv.btl = create_operation(spec->ot, stv.btl, stv_2.btl, debug);
                }
          }
        else
          stv.btl = stv_2.btl;

        if(stv.btldef)
          {
            if(stv_2.btldef)
              stv.btldef = create_operation(op_and, stv.btldef, stv_2.btldef, "%s ; %s");
          }
        else
          stv.btldef = stv_2.btldef;
      break;

      case op_join:
      case op_join_qual:
      case op_assign:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level, param, realval);

        if(stv.btl)
          {
            if(stv_2.btl)
                {
                  sprintf(debug, "%%s %s %%s", opname(spec->ot));
                  stv.btl = create_operation(spec->ot, stv.btl, stv_2.btl, debug);
                }
          }
        else
          stv.btl = stv_2.btl;

        if(stv.btldef)
          {
            if(stv_2.btldef)
              stv.btldef = create_operation(op_and, stv.btldef, stv_2.btldef, "%s ; %s");
          }
        else
          stv.btldef = stv_2.btldef;
      break;

      case op_join_array:
        p = spec->left;
        q = spec->right;

        if(param < 0)
          {
            switch(param)
              {
                case -1:
                  if(p->ot == op_join_array || p->ot == op_down)
                    {
                      stv = preval(cb, p, level, param, realval);
                      
                      if(stv.a == NULL_TIME)
                        stv.a = 0;
                     }
                  else
                    stv.a = 0;

                  if(q->ot == op_down)
                    {
                      stv_2 = preval(cb, q, level, param, realval);

                      stv.a += stv_2.a + 1;

                      printmsg(cb->fancymsg, 4, "Error, ill-formed array: %s\n", spec->debug);
                      exit_failure();
                    }
                  else
                    stv.a++;
                break;

                case -2:
                  if(p->ot == op_join_array || p->ot == op_down)
                    {
                      stv = preval(cb, p, level, param, realval);
                      
                      if(stv.b == NULL_TIME)
                        stv.b = 0;
                     }
                  else
                    stv.b = 0;

                  if(q->ot == op_down)
                    {
                      stv_2 = preval(cb, q, level, param, realval);

                      stv.b += stv_2.b + 1;

                      if(stv.a != stv_2.a)
                        {
                          printmsg(cb->fancymsg, 4, "Error, ill-formed array: %s\n", spec->debug);
                          exit_failure();
                        }
                    }
                  else
                    stv.b++;
                break;

                case -3:
                  if(p->ot == op_join_array || p->ot == op_down)
                    {
                      stv = preval(cb, p, level, param, realval);
                      
                      if(stv.c == NULL_TIME)
                        stv.c = 0;
                     }
                  else
                    stv.c = 0;

                  if(q->ot == op_down)
                    {
                      stv_2 = preval(cb, q, level, param, realval);

                      stv.c += stv_2.c + 1;

                      if(stv.a != stv_2.a || stv.b != stv_2.b)
                        {
                          printmsg(cb->fancymsg, 4, "Error, ill-formed array: %s\n", spec->debug);
                          exit_failure();
                        }
                    }
                  else
                    stv.c++;
                break;

                case -4:
                  if(p->ot == op_join_array || p->ot == op_down)
                    {
                      stv = preval(cb, p, level, param, realval);
                      
                      if(stv.d == NULL_TIME)
                        stv.d = 0;
                     }
                  else
                    stv.d = 0;

                  if(q->ot == op_down)
                    {
                      stv_2 = preval(cb, q, level, param, realval);

                      stv.d += stv_2.d + 1;

                      if(stv.a != stv_2.a || stv.b != stv_2.b || stv.c != stv_2.c)
                        {
                          printmsg(cb->fancymsg, 4, "Error, ill-formed array: %s\n", spec->debug);
                          exit_failure();
                        }
                    }
                  else
                    stv.d++;
                break;
              }
          }
        else
          {
            if(!param)
              {
                stv = preval(cb, q, level, param, realval);
                stv.d = 0;
              }
            else
              {
                if(q->ot == op_down)
                  {
                    stv_2 = preval(cb, q, level, param, realval);

                    if(param > stv_2.d)
                      {
                        stv = preval(cb, p, level, param - stv_2.d - 1, realval);

                        if(stv.d != NULL_TIME)
                          stv.d += stv_2.d + 1;
                        else
                          stv.d = stv_2.d + 1;
                      }
                    else
                      stv = stv_2;
                  } 
                else
                  {
                    stv = preval(cb, p, level, param - 1, realval);

                    if(stv.d != NULL_TIME)
                      stv.d++;
                    else
                      stv.d = 1;
                  }
              }
         }
      break;

      case op_down:
        if(param >= 0)
          stv = preval(cb, spec->left, level + 1, param, realval);
        else
          {
            if(param == -1)
              {
                printmsg(cb->fancymsg, 5, "Error, array too deep: %s\n", spec->debug);
                exit_failure();
              }

            if(spec->left->ot == op_join_array || spec->left->ot == op_down)
              stv = preval(cb, spec->left, level + 1, param + 1, realval);

            switch(param)
              {
                case -2:
                  if(stv.a == NULL_TIME)
                    stv.a = 0;

                case -3:
                  if(stv.b == NULL_TIME)
                    stv.b = 0;

                case -4:
                  if(stv.c == NULL_TIME)
                    stv.c = 0;

                  if(stv.d == NULL_TIME)
                    stv.d = 0;
                break;
              }
          }
      break;

      case op_input:
      case op_output:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level, param, realval);

        sprintf(debug, "%s [%%2$s] %%1$s", opname(spec->ot));
        stv.btl = create_operation(spec->ot, stv.btl, stv_2.btl, debug);
      break;

      case op_var_at:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level, param, realval);

        if(stv_2.a > stv_2.b)
          {
            printmsg(cb->fancymsg, 4, TIME_FMT", "TIME_FMT": Error, empty interval in initial conditions: %s\n", stv_2.a, stv_2.b, spec->debug);
            exit_failure();
          }

        q = create_operation(op_var_at, stv.btl, create_ground(op_number, "", stv_2.a, REAL_MAX), "(%s @ %s)");

        for(k = stv_2.a + 1; k <= stv_2.b; k++)
          q = create_operation(op_join, q,
                                         create_operation(op_var_at, copy_specification(stv.btl),
                                                                     create_ground(op_number, "", k, REAL_MAX), "(%s @ %s)"), "%s , %s");
        stv.btl = q;

        delete_specification(stv_2.btl);
      break;

      case op_not:
        stv = preval(cb, spec->left, level, param, realval);

        stv.btl = create_operation(op_not, stv.btl, NULL, "(~ %s)");
      break;

      case op_internal:
      case op_aux:
      case op_init:
        stv = preval(cb, spec->left, level, param, realval);

        sprintf(debug, "%s %%s", opname(spec->ot));
        stv.btl = create_operation(spec->ot, stv.btl, NULL, debug);
      break;

      case op_plus:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level, param, realval);

        if(!stv.wtra || !stv_2.wtra)
          {
            if(stv.btl)
              {
                if(stv_2.btl)
                  stv.btl = create_operation(op_plus, stv.btl, stv_2.btl, "(%s + %s)");
              }
            else
              stv.btl = stv_2.btl;

            if(stv.btldef)
              {
                if(stv_2.btldef)
                  stv.btldef = create_operation(op_and, stv.btldef, stv_2.btldef, "%s ; %s");
              }
            else
              stv.btldef = stv_2.btldef;

            stv.wtra = (d_time *)FALSE;
          }
        else
          if(stv.a == NULL_TIME || stv_2.a == NULL_TIME)
            {
              if(stv.a != NULL_TIME)
                stv.realval = stv.a;

              if(stv_2.a != NULL_TIME)
                stv_2.realval = stv_2.a;

              result = stv.realval + stv_2.realval;

              delete_specification(stv.btl);
              delete_specification(stv_2.btl);

              stv.btl = create_ground(op_real, "", NULL_TIME, result);
              stv.a = NULL_TIME;
              stv.realval = result;

              stv.wtra = (d_time *)TRUE;
            }
          else
            {
              val = stv.a + stv_2.a;

              delete_specification(stv.btl);
              delete_specification(stv_2.btl);

              stv.btl = create_ground(op_number, "", val, REAL_MAX);
              stv.a = val;
              stv.b = val;

              stv.wtra = (d_time *)TRUE;
            }
      break;

      case op_minus:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level, param, realval);

        if(!stv.wtra || !stv_2.wtra)
          {
            stv_2 = preval(cb, create_operation(op_chs, spec->right, NULL, "(- %s)"), level, param, realval);

            if(stv.btl)
              {
                if(stv_2.btl)
                  stv.btl = create_operation(op_plus, stv.btl, stv_2.btl, "(%s + %s)");
              }
            else
              stv.btl = stv_2.btl;

            if(stv.btldef)
              {
                if(stv_2.btldef)
                  stv.btldef = create_operation(op_and, stv.btldef, stv_2.btldef, "%s ; %s");
              }
            else
              stv.btldef = stv_2.btldef;

            stv.wtra = (d_time *)FALSE;
          }
        else
          if(stv.a == NULL_TIME || stv_2.a == NULL_TIME)
            {
              if(stv.a != NULL_TIME)
                stv.realval = stv.a;

              if(stv_2.a != NULL_TIME)
                stv_2.realval = stv_2.a;

              result = stv.realval - stv_2.realval;

              delete_specification(stv.btl);
              delete_specification(stv_2.btl);

              stv.btl = create_ground(op_real, "", NULL_TIME, result);
              stv.a = NULL_TIME;
              stv.realval = result;

              stv.wtra = (d_time *)TRUE;
            }
          else
            {
              val = stv.a - stv_2.a;

              delete_specification(stv.btl);
              delete_specification(stv_2.btl);

              stv.btl = create_ground(op_number, "", val, REAL_MAX);
              stv.a = val;
              stv.b = val;

              stv.wtra = (d_time *)TRUE;
            }
      break;

      case op_mul:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level, param, realval);

        if(!stv.wtra || !stv_2.wtra)
          {
            if(stv.btl)
              {
                if(stv_2.btl)
                  stv.btl = create_operation(op_mul, stv.btl, stv_2.btl, "(%s * %s)");
              }
            else
              stv.btl = stv_2.btl;

            if(stv.btldef)
              {
                if(stv_2.btldef)
                  stv.btldef = create_operation(op_and, stv.btldef, stv_2.btldef, "%s ; %s");
              }
            else
              stv.btldef = stv_2.btldef;

            stv.wtra = (d_time *)FALSE;
          }
        else
          if(stv.a == NULL_TIME || stv_2.a == NULL_TIME)
            {
              if(stv.a != NULL_TIME)
                stv.realval = stv.a;

              if(stv_2.a != NULL_TIME)
                stv_2.realval = stv_2.a;

              result = stv.realval * stv_2.realval;

              delete_specification(stv.btl);
              delete_specification(stv_2.btl);

              stv.btl = create_ground(op_real, "", NULL_TIME, result);
              stv.a = NULL_TIME;
              stv.realval = result;

              stv.wtra = (d_time *)TRUE;
            }
          else
            {
              val = stv.a * stv_2.a;

              delete_specification(stv.btl);
              delete_specification(stv_2.btl);

              stv.btl = create_ground(op_number, "", val, REAL_MAX);
              stv.a = val;
              stv.b = val;

              stv.wtra = (d_time *)TRUE;
            }
      break;

      case op_div:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level, param, realval);

        if(!stv.wtra || !stv_2.wtra)
          {
            stv_2 = preval(cb, create_operation(op_inv, spec->right, NULL, "(1 / %s)"), level, param, realval);

            if(stv.btl)
              {
                if(stv_2.btl)
                  stv.btl = create_operation(op_mul, stv.btl, stv_2.btl, "(%s * %s)");
              }
            else
              stv.btl = stv_2.btl;

            if(stv.btldef)
              {
                if(stv_2.btldef)
                  stv.btldef = create_operation(op_and, stv.btldef, stv_2.btldef, "%s ; %s");
              }
            else
              stv.btldef = stv_2.btldef;

            stv.wtra = (d_time *)FALSE;
          }
        else
          if(stv.a == NULL_TIME || stv_2.a == NULL_TIME)
            {
              if(stv.a != NULL_TIME)
                stv.realval = stv.a;

              if(stv_2.a != NULL_TIME)
                stv_2.realval = stv_2.a;

              if(!stv_2.realval)
                {
                  printmsg(cb->fancymsg, 4, "Error, division by zero: %s\n", spec->debug);
                  exit_failure();
                }

              result = stv.realval / stv_2.realval;

              delete_specification(stv.btl);
              delete_specification(stv_2.btl);

              stv.btl = create_ground(op_real, "", NULL_TIME, result);
              stv.a = NULL_TIME;
              stv.realval = result;

              stv.wtra = (d_time *)TRUE;
            }
          else
            {
              if(!stv_2.a)
                {
                  printmsg(cb->fancymsg, 4, "Error, division by zero: %s\n", spec->debug);
                  exit_failure();
                }

              val = stv.a / stv_2.a;

              delete_specification(stv.btl);
              delete_specification(stv_2.btl);

              stv.btl = create_ground(op_number, "", val, REAL_MAX);
              stv.a = val;
              stv.b = val;

              stv.wtra = (d_time *)TRUE;
            }
      break;

      case op_mod:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level, param, realval);

        if(stv.a == NULL_TIME || stv_2.a == NULL_TIME)
          {
            printmsg(cb->fancymsg, 4, "Error, integer type required: %s\n", spec->debug);
            exit_failure();
          }

        if(!stv_2.a)
          {
            printmsg(cb->fancymsg, 4, "Error, division by zero: %s\n", spec->debug);
            exit_failure();
          }

        val = stv.a % stv_2.a;

        delete_specification(stv.btl);
        delete_specification(stv_2.btl);

        stv.btl = create_ground(op_number, "", val, REAL_MAX);
        stv.a = val;
        stv.b = val;

        stv.wtra = (d_time *)TRUE;
      break;

      case op_pow:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level, param, realval);

        if(!stv.wtra || !stv_2.wtra)
          {
            if(stv.btl)
              {
                if(stv_2.btl)
                  stv.btl = create_operation(op_pow, stv.btl, stv_2.btl, "(%s ^ %s)");
              }
            else
              stv.btl = stv_2.btl;

            if(stv.btldef)
              {
                if(stv_2.btldef)
                  stv.btldef = create_operation(op_and, stv.btldef, stv_2.btldef, "%s ; %s");
              }
            else
              stv.btldef = stv_2.btldef;

            stv.wtra = (d_time *)FALSE;
          }
        else
          if(stv.a == NULL_TIME || stv_2.a == NULL_TIME)
            {
              if(stv.a != NULL_TIME)
                stv.realval = stv.a;

              if(stv_2.a != NULL_TIME)
                stv_2.realval = stv_2.a;

              result = pow(stv.realval, stv_2.realval);

              delete_specification(stv.btl);
              delete_specification(stv_2.btl);

              stv.btl = create_ground(op_real, "", NULL_TIME, result);
              stv.a = NULL_TIME;
              stv.realval = result;

              stv.wtra = (d_time *)TRUE;
            }
          else
            {
              val = floor(pow(stv.a, stv_2.a));

              delete_specification(stv.btl);
              delete_specification(stv_2.btl);

              stv.btl = create_ground(op_number, "", val, REAL_MAX);
              stv.a = val;
              stv.b = val;

              stv.wtra = (d_time *)TRUE;
            }
      break;

      case op_root:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level, param, realval);

        if(!stv.wtra || !stv_2.wtra)
          {
            if(stv.btl)
              {
                if(stv_2.btl)
                  stv.btl = create_operation(op_root, stv.btl, stv_2.btl, "root(%s, %s)");
              }
            else
              stv.btl = stv_2.btl;

            if(stv.btldef)
              {
                if(stv_2.btldef)
                  stv.btldef = create_operation(op_and, stv.btldef, stv_2.btldef, "%s ; %s");
              }
            else
              stv.btldef = stv_2.btldef;

            stv.wtra = (d_time *)FALSE;
          }
        else
          if(stv.a == NULL_TIME || stv_2.a == NULL_TIME)
            {
              if(stv.a != NULL_TIME)
                stv.realval = stv.a;

              if(stv_2.a != NULL_TIME)
                stv_2.realval = stv_2.a;

              result = pow(stv.realval, 1.0 / stv_2.realval);

              delete_specification(stv.btl);
              delete_specification(stv_2.btl);

              stv.btl = create_ground(op_real, "", NULL_TIME, result);
              stv.a = NULL_TIME;
              stv.realval = result;

              stv.wtra = (d_time *)TRUE;
            }
          else
            {
              val = floor(pow(stv.a, 1.0 / stv_2.a));

              delete_specification(stv.btl);
              delete_specification(stv_2.btl);

              stv.btl = create_ground(op_number, "", val, REAL_MAX);
              stv.a = val;
              stv.b = val;

              stv.wtra = (d_time *)TRUE;
            }
      break;

      case op_log:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level, param, realval);

        if(!stv.wtra || !stv_2.wtra)
          {
            if(stv.btl)
              {
                if(stv_2.btl)
                  stv.btl = create_operation(op_log, stv.btl, stv_2.btl, "log(%s, %s)");
              }
            else
              stv.btl = stv_2.btl;

            if(stv.btldef)
              {
                if(stv_2.btldef)
                  stv.btldef = create_operation(op_and, stv.btldef, stv_2.btldef, "%s ; %s");
              }
            else
              stv.btldef = stv_2.btldef;

            stv.wtra = (d_time *)FALSE;
          }
        else
          if(stv.a == NULL_TIME || stv_2.a == NULL_TIME)
            {
              if(stv.a != NULL_TIME)
                stv.realval = stv.a;

              if(stv_2.a != NULL_TIME)
                stv_2.realval = stv_2.a;

              result = log(stv_2.realval) / log(stv.realval);

              delete_specification(stv.btl);
              delete_specification(stv_2.btl);

              stv.btl = create_ground(op_real, "", NULL_TIME, result);
              stv.a = NULL_TIME;
              stv.realval = result;

              stv.wtra = (d_time *)TRUE;
            }
          else
            {
              val = floor(log(stv_2.a) / log(stv.a));

              delete_specification(stv.btl);
              delete_specification(stv_2.btl);

              stv.btl = create_ground(op_number, "", val, REAL_MAX);
              stv.a = val;
              stv.b = val;

              stv.wtra = (d_time *)TRUE;
            }
      break;

      case op_math_eqv0:
        stv = preval(cb, spec->left, level, param, realval);

        if(!stv.wtra)
          {
            if(stv.btl)
              stv.btl = create_operation(op_math_eqv0, stv.btl, NULL, "(%s = 0)");
          }
        else
          if(stv.a == NULL_TIME)
            {
              result = (stv.realval == 0);

              delete_specification(stv.btl);

              stv.btl = create_ground(op_real, "", NULL_TIME, result);
              stv.a = NULL_TIME;
              stv.realval = result;
            }
          else
            {
              val = (stv.a == 0);

              delete_specification(stv.btl);

              stv.btl = create_ground(op_number, "", val, REAL_MAX);
              stv.a = val;
              stv.b = val;
            }
      break;

      case op_chs:
        stv = preval(cb, spec->left, level, param, realval);

        if(!stv.wtra)
          {
            if(stv.btl)
              stv.btl = create_operation(op_chs, stv.btl, NULL, "(- %s)");
          }
        else
          if(stv.a == NULL_TIME)
            {
              result = - stv.realval;

              delete_specification(stv.btl);

              stv.btl = create_ground(op_real, "", NULL_TIME, result);
              stv.a = NULL_TIME;
              stv.realval = result;
            }
          else
            {
              val = - stv.a;

              delete_specification(stv.btl);

              stv.btl = create_ground(op_number, "", val, REAL_MAX);
              stv.a = val;
              stv.b = val;
            }
      break;

      case op_inv:
        stv = preval(cb, spec->left, level, param, realval);

        if(!stv.wtra)
          {
            if(stv.btl)
              stv.btl = create_operation(op_inv, stv.btl, NULL, "(1 / %s)");
          }
        else
          if(stv.a == NULL_TIME)
            {
              if(!stv.realval)
                {
                  printmsg(cb->fancymsg, 4, "Error, division by zero: %s\n", spec->debug);
                  exit_failure();
                }

              result = 1.0 / stv.realval;

              delete_specification(stv.btl);

              stv.btl = create_ground(op_real, "", NULL_TIME, result);
              stv.a = NULL_TIME;
              stv.realval = result;
            }
          else
            {
              if(!stv.a)
                {
                  printmsg(cb->fancymsg, 4, "Error, division by zero: %s\n", spec->debug);
                  exit_failure();
                }

              result = 1.0 / stv.a;

              delete_specification(stv.btl);

              stv.btl = create_ground(op_real, "", NULL_TIME, result);
              stv.a = NULL_TIME;
              stv.realval = result;
            }
      break;

      case op_sin:
        stv = preval(cb, spec->left, level, param, realval);

        if(!stv.wtra)
          {
            if(stv.btl)
              stv.btl = create_operation(op_sin, stv.btl, NULL, "sin(%s)");
          }
        else
          if(stv.a == NULL_TIME)
            {
              result = sin(stv.realval);

              delete_specification(stv.btl);

              stv.btl = create_ground(op_real, "", NULL_TIME, result);
              stv.a = NULL_TIME;
              stv.realval = result;
            }
          else
            {
              val = floor(sin(stv.a));

              delete_specification(stv.btl);

              stv.btl = create_ground(op_number, "", val, REAL_MAX);
              stv.a = val;
              stv.b = val;
            }
      break;

      case op_asin:
        stv = preval(cb, spec->left, level, param, realval);

        if(!stv.wtra)
          {
            if(stv.btl)
              stv.btl = create_operation(op_asin, stv.btl, NULL, "asin(%s)");
          }
        else
          if(stv.a == NULL_TIME)
            {
              result = asin(stv.realval);

              delete_specification(stv.btl);

              stv.btl = create_ground(op_real, "", NULL_TIME, result);
              stv.a = NULL_TIME;
              stv.realval = result;
            }
          else
            {
              val = floor(asin(stv.a));

              delete_specification(stv.btl);

              stv.btl = create_ground(op_number, "", val, REAL_MAX);
              stv.a = val;
              stv.b = val;
            }
      break;

      case op_cos:
        stv = preval(cb, spec->left, level, param, realval);

        if(!stv.wtra)
          {
            if(stv.btl)
              stv.btl = create_operation(op_sin, create_operation(op_plus, stv.btl, create_ground(op_real, "", NULL_TIME, M_PI / 2), "(%s + %s)"), NULL, "sin(%s)");
          }
        else
          if(stv.a == NULL_TIME)
            {
              result = cos(stv.realval);

              delete_specification(stv.btl);

              stv.btl = create_ground(op_real, "", NULL_TIME, result);
              stv.a = NULL_TIME;
              stv.realval = result;
            }
          else
            {
              val = floor(cos(stv.a));

              delete_specification(stv.btl);

              stv.btl = create_ground(op_number, "", val, REAL_MAX);
              stv.a = val;
              stv.b = val;
            }
      break;

      case op_acos:
        stv = preval(cb, spec->left, level, param, realval);

        if(!stv.wtra)
          {
            if(stv.btl)
              stv.btl = create_operation(op_chs, create_operation(op_plus,
                        create_operation(op_asin, stv.btl, NULL, "asin(%s)"), create_ground(op_real, "", NULL_TIME, - M_PI / 2), "(%s + %s)"), NULL, "(- %s)");
          }
        else
          if(stv.a == NULL_TIME)
            {
              result = acos(stv.realval);

              delete_specification(stv.btl);

              stv.btl = create_ground(op_real, "", NULL_TIME, result);
              stv.a = NULL_TIME;
              stv.realval = result;
            }
          else
            {
              val = floor(acos(stv.a));

              delete_specification(stv.btl);

              stv.btl = create_ground(op_number, "", val, REAL_MAX);
              stv.a = val;
              stv.b = val;
            }
      break;

      case op_tan:
        stv = preval(cb, spec->left, level, param, realval);

        if(!stv.wtra)
          {
            if(stv.btl)
              stv.btl = create_operation(op_mul,
                        create_operation(op_sin, stv.btl, NULL, "sin(%s)"),
                        create_operation(op_inv,
                        create_operation(op_sin, create_operation(op_plus, copy_specification(stv.btl), create_ground(op_real, "", NULL_TIME, M_PI / 2), "(%s + %s)"), NULL, "sin(%s)"),
                        NULL, "(1 / %s)"),
                        "(%s * %s)");
          }
        else
          if(stv.a == NULL_TIME)
            {
              result = tan(stv.realval);

              delete_specification(stv.btl);

              stv.btl = create_ground(op_real, "", NULL_TIME, result);
              stv.a = NULL_TIME;
              stv.realval = result;
            }
          else
            {
              val = floor(tan(stv.a));

              delete_specification(stv.btl);

              stv.btl = create_ground(op_number, "", val, REAL_MAX);
              stv.a = val;
              stv.b = val;
            }
      break;

      case op_atan:
        stv = preval(cb, spec->left, level, param, realval);

        if(!stv.wtra)
          {
            if(stv.btl)
              stv.btl = create_operation(op_asin,
                        create_operation(op_mul, stv.btl,
                        create_operation(op_pow,
                        create_operation(op_plus,
                        create_operation(op_pow, copy_specification(stv.btl), create_ground(op_real, "", NULL_TIME, 2), "(%s ^ %s)"),
                        create_ground(op_real, "", NULL_TIME, 1), "(%s + %s)"),
                        create_ground(op_real, "", NULL_TIME, -0.5), "(%s ^ %s)"),
                        "(%s * %s)"),
                        NULL, "asin(%s)");
          }
        else
          if(stv.a == NULL_TIME)
            {
              result = atan(stv.realval);

              delete_specification(stv.btl);

              stv.btl = create_ground(op_real, "", NULL_TIME, result);
              stv.a = NULL_TIME;
              stv.realval = result;
            }
          else
            {
              val = floor(atan(stv.a));

              delete_specification(stv.btl);

              stv.btl = create_ground(op_number, "", val, REAL_MAX);
              stv.a = val;
              stv.b = val;
            }
      break;

      case op_equal:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level, param, realval);

        if(!stv.wtra || !stv_2.wtra)
          {
            if((!stv.wtra || (stv.a && (stv.a != NULL_TIME || stv.realval))) && (!stv_2.wtra || (stv_2.a && (stv_2.a != NULL_TIME || stv_2.realval))))
              stv.btl = preval(cb, create_equal(stv.btl, stv_2.btl), level, param, realval).btl;
            else
              if(stv.wtra && (!stv.a || (stv.a == NULL_TIME && !stv.realval)))
                stv.btl = create_equal(NULL, stv_2.btl);
              else
                stv.btl = create_equal(stv.btl, NULL);

            if(stv.btldef)
              {
                if(stv_2.btldef)
                  stv.btldef = create_operation(op_and, stv.btldef, stv_2.btldef, "%s ; %s");
              }
            else
              stv.btldef = stv_2.btldef;
          }
        else
          if(stv.a == NULL_TIME || stv_2.a == NULL_TIME)
            {
              if(stv.a != NULL_TIME)
                stv.realval = stv.a;

              if(stv_2.a != NULL_TIME)
                stv_2.realval = stv_2.a;

              result = (stv.realval == stv_2.realval);

              delete_specification(stv.btl);
              delete_specification(stv_2.btl);

              stv.btl = create_ground(op_real, "", NULL_TIME, result);
              stv.a = NULL_TIME;
              stv.realval = result;

              printmsg(cb->fancymsg, 2, "%s: Warning, ignoring math relation always %s\n", spec->debug, result? "true" : "false");
            }
          else
            {
              val = (stv.a == stv_2.a);

              delete_specification(stv.btl);
              delete_specification(stv_2.btl);

              stv.btl = create_ground(op_number, "", val, REAL_MAX);
              stv.a = val;
              stv.b = val;

              printmsg(cb->fancymsg, 2, "%s: Warning, ignoring math relation always %s\n", spec->debug, val? "true" : "false");
            }
      break;

      case op_neq:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level, param, realval);

        if(!stv.wtra || !stv_2.wtra)
          {
            if((!stv.wtra || (stv.a && (stv.a != NULL_TIME || stv.realval))) && (!stv_2.wtra || (stv_2.a && (stv_2.a != NULL_TIME || stv_2.realval))))
              {
                stv_2 = preval(cb, create_operation(op_chs, spec->right, NULL, "(- %s)"), level, param, realval);

                if(stv.btl)
                  {
                    if(stv_2.btl)
                      stv.btl = create_operation(op_plus, stv.btl, stv_2.btl, "(%s + %s)");
                  }
                else
                  stv.btl = stv_2.btl;
              }
            else
              if(stv.wtra && (!stv.a || (stv.a == NULL_TIME && !stv.realval)))
                stv.btl = stv_2.btl;

            stv.btl = create_operation(op_math_neq0, stv.btl, NULL, "(%s ~= 0)");

            if(stv.btldef)
              {
                if(stv_2.btldef)
                  stv.btldef = create_operation(op_and, stv.btldef, stv_2.btldef, "%s ; %s");
              }
            else
              stv.btldef = stv_2.btldef;
          }
        else
          if(stv.a == NULL_TIME || stv_2.a == NULL_TIME)
            {
              if(stv.a != NULL_TIME)
                stv.realval = stv.a;

              if(stv_2.a != NULL_TIME)
                stv_2.realval = stv_2.a;

              result = (stv.realval != stv_2.realval);

              delete_specification(stv.btl);
              delete_specification(stv_2.btl);

              stv.btl = create_ground(op_real, "", NULL_TIME, result);
              stv.a = NULL_TIME;
              stv.realval = result;

              printmsg(cb->fancymsg, 2, "%s: Warning, ignoring math relation always %s\n", spec->debug, result? "true" : "false");
            }
          else
            {
              val = (stv.a != stv_2.a);

              delete_specification(stv.btl);
              delete_specification(stv_2.btl);

              stv.btl = create_ground(op_number, "", val, REAL_MAX);
              stv.a = val;
              stv.b = val;

              printmsg(cb->fancymsg, 2, "%s: Warning, ignoring math relation always %s\n", spec->debug, val? "true" : "false");
            }
      break;

      case op_gteq:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level, param, realval);

        if(!stv.wtra || !stv_2.wtra)
          {
            if((!stv.wtra || (stv.a && (stv.a != NULL_TIME || stv.realval))) && (!stv_2.wtra || (stv_2.a && (stv_2.a != NULL_TIME || stv_2.realval))))
              {
                stv_2 = preval(cb, create_operation(op_chs, spec->right, NULL, "(- %s)"), level, param, realval);

                if(stv.btl)
                  {
                    if(stv_2.btl)
                      stv.btl = create_operation(op_plus, stv.btl, stv_2.btl, "(%s + %s)");
                  }
                else
                  stv.btl = stv_2.btl;
              }
            else
              if(stv.wtra && (!stv.a || (stv.a == NULL_TIME && !stv.realval)))
                stv.btl = create_operation(op_chs, stv_2.btl, NULL, "(- %s)");

            stv.btl = create_operation(op_math_gteq0, stv.btl, NULL, "(%s >= 0)");

            if(stv.btldef)
              {
                if(stv_2.btldef)
                  stv.btldef = create_operation(op_and, stv.btldef, stv_2.btldef, "%s ; %s");
              }
            else
              stv.btldef = stv_2.btldef;
          }
        else
          if(stv.a == NULL_TIME || stv_2.a == NULL_TIME)
            {
              if(stv.a != NULL_TIME)
                stv.realval = stv.a;

              if(stv_2.a != NULL_TIME)
                stv_2.realval = stv_2.a;

              result = (stv.realval >= stv_2.realval);

              delete_specification(stv.btl);
              delete_specification(stv_2.btl);

              stv.btl = create_ground(op_real, "", NULL_TIME, result);
              stv.a = NULL_TIME;
              stv.realval = result;

              printmsg(cb->fancymsg, 2, "%s: Warning, ignoring math relation always %s\n", spec->debug, result? "true" : "false");
            }
          else
            {
              val = (stv.a >= stv_2.a);

              delete_specification(stv.btl);
              delete_specification(stv_2.btl);

              stv.btl = create_ground(op_number, "", val, REAL_MAX);
              stv.a = val;
              stv.b = val;

              printmsg(cb->fancymsg, 2, "%s: Warning, ignoring math relation always %s\n", spec->debug, val? "true" : "false");
            }
      break;

      case op_lt:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level, param, realval);

        if(!stv.wtra || !stv_2.wtra)
          {
            if((!stv.wtra || (stv.a && (stv.a != NULL_TIME || stv.realval))) && (!stv_2.wtra || (stv_2.a && (stv_2.a != NULL_TIME || stv_2.realval))))
              {
                stv_2 = preval(cb, create_operation(op_chs, spec->right, NULL, "(- %s)"), level, param, realval);

                if(stv.btl)
                  {
                    if(stv_2.btl)
                      stv.btl = create_operation(op_plus, stv.btl, stv_2.btl, "(%s + %s)");
                  }
                else
                  stv.btl = stv_2.btl;
              }
            else
              if(stv.wtra && (!stv.a || (stv.a == NULL_TIME && !stv.realval)))
                stv.btl = create_operation(op_chs, stv_2.btl, NULL, "(- %s)");

            stv.btl = create_operation(op_math_lt0, stv.btl, NULL, "(%s < 0)");

            if(stv.btldef)
              {
                if(stv_2.btldef)
                  stv.btldef = create_operation(op_and, stv.btldef, stv_2.btldef, "%s ; %s");
              }
            else
              stv.btldef = stv_2.btldef;
          }
        else
          if(stv.a == NULL_TIME || stv_2.a == NULL_TIME)
            {
              if(stv.a != NULL_TIME)
                stv.realval = stv.a;

              if(stv_2.a != NULL_TIME)
                stv_2.realval = stv_2.a;

              result = (stv.realval < stv_2.realval);

              delete_specification(stv.btl);
              delete_specification(stv_2.btl);

              stv.btl = create_ground(op_real, "", NULL_TIME, result);
              stv.a = NULL_TIME;
              stv.realval = result;

              printmsg(cb->fancymsg, 2, "%s: Warning, ignoring math relation always %s\n", spec->debug, result? "true" : "false");
            }
          else
            {
              val = (stv.a < stv_2.a);

              delete_specification(stv.btl);
              delete_specification(stv_2.btl);

              stv.btl = create_ground(op_number, "", val, REAL_MAX);
              stv.a = val;
              stv.b = val;

              printmsg(cb->fancymsg, 2, "%s: Warning, ignoring math relation always %s\n", spec->debug, val? "true" : "false");
            }
      break;

      case op_lteq:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level, param, realval);

        if(!stv.wtra || !stv_2.wtra)
          {
            if((!stv.wtra || (stv.a && (stv.a != NULL_TIME || stv.realval))) && (!stv_2.wtra || (stv_2.a && (stv_2.a != NULL_TIME || stv_2.realval))))
              {
                stv = preval(cb, create_operation(op_chs, spec->left, NULL, "(- %s)"), level, param, realval);

                if(stv.btl)
                  {
                    if(stv_2.btl)
                      stv.btl = create_operation(op_plus, stv.btl, stv_2.btl, "(%s + %s)");
                  }
                else
                  stv.btl = stv_2.btl;
              }
            else
              if(stv.wtra && (!stv.a || (stv.a == NULL_TIME && !stv.realval)))
                stv.btl = stv_2.btl;
              else
                stv.btl = create_operation(op_chs, stv.btl, NULL, "(- %s)");

            stv.btl = create_operation(op_math_gteq0, stv.btl, NULL, "(%s >= 0)");

            if(stv.btldef)
              {
                if(stv_2.btldef)
                  stv.btldef = create_operation(op_and, stv.btldef, stv_2.btldef, "%s ; %s");
              }
            else
              stv.btldef = stv_2.btldef;
          }
        else
          if(stv.a == NULL_TIME || stv_2.a == NULL_TIME)
            {
              if(stv.a != NULL_TIME)
                stv.realval = stv.a;

              if(stv_2.a != NULL_TIME)
                stv_2.realval = stv_2.a;

              result = (stv.realval <= stv_2.realval);

              delete_specification(stv.btl);
              delete_specification(stv_2.btl);

              stv.btl = create_ground(op_real, "", NULL_TIME, result);
              stv.a = NULL_TIME;
              stv.realval = result;

              printmsg(cb->fancymsg, 2, "%s: Warning, ignoring math relation always %s\n", spec->debug, result? "true" : "false");
            }
          else
            {
              val = (stv.a <= stv_2.a);

              delete_specification(stv.btl);
              delete_specification(stv_2.btl);

              stv.btl = create_ground(op_number, "", val, REAL_MAX);
              stv.a = val;
              stv.b = val;

              printmsg(cb->fancymsg, 2, "%s: Warning, ignoring math relation always %s\n", spec->debug, val? "true" : "false");
            }
      break;

      case op_gt:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level, param, realval);

        if(!stv.wtra || !stv_2.wtra)
          {
            if((!stv.wtra || (stv.a && (stv.a != NULL_TIME || stv.realval))) && (!stv_2.wtra || (stv_2.a && (stv_2.a != NULL_TIME || stv_2.realval))))
              {
                stv = preval(cb, create_operation(op_chs, spec->left, NULL, "(- %s)"), level, param, realval);

                if(stv.btl)
                  {
                    if(stv_2.btl)
                      stv.btl = create_operation(op_plus, stv.btl, stv_2.btl, "(%s + %s)");
                  }
                else
                  stv.btl = stv_2.btl;
              }
            else
              if(stv.wtra && (!stv.a || (stv.a == NULL_TIME && !stv.realval)))
                stv.btl = stv_2.btl;
              else
                stv.btl = create_operation(op_chs, stv.btl, NULL, "(- %s)");

            stv.btl = create_operation(op_math_lt0, stv.btl, NULL, "(%s < 0)");

            if(stv.btldef)
              {
                if(stv_2.btldef)
                  stv.btldef = create_operation(op_and, stv.btldef, stv_2.btldef, "%s ; %s");
              }
            else
              stv.btldef = stv_2.btldef;
          }
        else
          if(stv.a == NULL_TIME || stv_2.a == NULL_TIME)
            {
              if(stv.a != NULL_TIME)
                stv.realval = stv.a;

              if(stv_2.a != NULL_TIME)
                stv_2.realval = stv_2.a;

              result = (stv.realval > stv_2.realval);

              delete_specification(stv.btl);
              delete_specification(stv_2.btl);

              stv.btl = create_ground(op_real, "", NULL_TIME, result);
              stv.a = NULL_TIME;
              stv.realval = result;

              printmsg(cb->fancymsg, 2, "%s: Warning, ignoring math relation always %s\n", spec->debug, result? "true" : "false");
            }
          else
            {
              val = (stv.a > stv_2.a);

              delete_specification(stv.btl);
              delete_specification(stv_2.btl);

              stv.btl = create_ground(op_number, "", val, REAL_MAX);
              stv.a = val;
              stv.b = val;

              printmsg(cb->fancymsg, 2, "%s: Warning, ignoring math relation always %s\n", spec->debug, val? "true" : "false");
            }
      break;

      case op_interval_1:
        if(spec->left->ot == op_range && spec->right->ot == op_range)
          stv = preval(cb, spec->left, level, param + 1, realval);
        else
          stv = preval(cb, spec->left, level, param, realval);

        stv_2 = preval(cb, spec->right, level, param, realval);

        if((stv.a == NULL_TIME && stv.realval != REAL_MAX) || (stv_2.a == NULL_TIME && stv_2.realval != REAL_MAX))
          {
            printmsg(cb->fancymsg, 4, "Error, integer type required: %s\n", spec->debug);
            exit_failure();
          }

        stv.b = stv_2.a;

        stv.d = (spec->left->ot == op_range) + (spec->right->ot == op_range);

        delete_specification(stv_2.btl);
      break;

      case op_interval_2:
        if(spec->left->ot == op_range && spec->right->ot == op_range)
          stv = preval(cb, spec->left, level, param + 1, realval);
        else
          stv = preval(cb, spec->left, level, param, realval);

        stv_2 = preval(cb, spec->right, level, param, realval);

        if((stv.a == NULL_TIME && stv.realval != REAL_MAX) || (stv_2.a == NULL_TIME && stv_2.realval != REAL_MAX))
          {
            printmsg(cb->fancymsg, 4, "Error, integer type required: %s\n", spec->debug);
            exit_failure();
          }

        stv.a++;
        stv.b = stv_2.a;

        stv.d = (spec->left->ot == op_range) + (spec->right->ot == op_range);

        delete_specification(stv_2.btl);
      break;

      case op_interval_3:
        if(spec->left->ot == op_range && spec->right->ot == op_range)
          stv = preval(cb, spec->left, level, param + 1, realval);
        else
          stv = preval(cb, spec->left, level, param, realval);

        stv_2 = preval(cb, spec->right, level, param, realval);

        if((stv.a == NULL_TIME && stv.realval != REAL_MAX) || (stv_2.a == NULL_TIME && stv_2.realval != REAL_MAX))
          {
            printmsg(cb->fancymsg, 4, "Error, integer type required: %s\n", spec->debug);
            exit_failure();
          }

        stv.b = stv_2.a - 1;

        stv.d = (spec->left->ot == op_range) + (spec->right->ot == op_range);

        delete_specification(stv_2.btl);
      break;

      case op_interval_4:
        if(spec->left->ot == op_range && spec->right->ot == op_range)
          stv = preval(cb, spec->left, level, param + 1, realval);
        else
          stv = preval(cb, spec->left, level, param, realval);

        stv_2 = preval(cb, spec->right, level, param, realval);

        if((stv.a == NULL_TIME && stv.realval != REAL_MAX) || (stv_2.a == NULL_TIME && stv_2.realval != REAL_MAX))
          {
            printmsg(cb->fancymsg, 4, "Error, integer type required: %s\n", spec->debug);
            exit_failure();
          }

        stv.a++;
        stv.b = stv_2.a - 1;

        stv.d = (spec->left->ot == op_range) + (spec->right->ot == op_range);

        delete_specification(stv_2.btl);
      break;

      case op_at:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level, param, realval);

        delete_specification(stv_2.btl);

        stv = at_happen(cb, stv.btl, stv.btldef, stv_2.a, stv_2.b, level, param, realval, FALSE);
      break;

      case op_happen:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level, param, realval);

        delete_specification(stv_2.btl);

        stv = at_happen(cb, stv.btl, stv.btldef, stv_2.a, stv_2.b, level, param, realval, TRUE);
      break;

      case op_pplus:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level, param, realval);

        delete_specification(stv_2.btl);

        stv = pplus_mmul(cb, stv.btl, stv.btldef, stv_2.a, stv_2.b, level, param, realval, FALSE);
      break;

      case op_mmul:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level, param, realval);

        delete_specification(stv_2.btl);

        stv = pplus_mmul(cb, stv.btl, stv.btldef, stv_2.a, stv_2.b, level, param, realval, TRUE);
      break;

      case op_only:
        stv = subset_only(cb, spec, level, param, realval);
      break;

      case op_since:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level, param, realval);

        stv = since_until(cb, stv.btl, stv.btldef, stv_2.btl, stv_2.btldef, level, param, realval, FALSE);
      break;

      case op_until:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level, param, realval);

        stv = since_until(cb, stv.btl, stv.btldef, stv_2.btl, stv_2.btldef, level, param, realval, TRUE);
      break;

      case op_xor:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level, param, realval);

        stv.btl = CREATE_XOR(stv.btl, stv_2.btl);

        if(stv.btldef)
          {
            if(stv_2.btldef)
              stv.btldef = create_operation(op_and, stv.btldef, stv_2.btldef, "%s ; %s");
          }
        else
         stv.btldef = stv_2.btldef;
      break;

      case op_imply:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level, param, realval);

        stv.btl = CREATE_IMPLY(stv.btl, stv_2.btl);

        if(stv.btldef)
          {
            if(stv_2.btldef)
              stv.btldef = create_operation(op_and, stv.btldef, stv_2.btldef, "%s ; %s");
          }
        else
         stv.btldef = stv_2.btldef;
      break;

      case op_eqv:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level, param, realval);

        stv.btl = CREATE_EQV(stv.btl, stv_2.btl);

        if(stv.btldef)
          {
            if(stv_2.btldef)
              stv.btldef = create_operation(op_and, stv.btldef, stv_2.btldef, "%s ; %s");
          }
        else
         stv.btldef = stv_2.btldef;
      break;

      case op_forall:
        stv = forall(cb, spec, level, param, realval);
      break;

      case op_exists:
        stv = exists(cb, spec, level, param, realval);
      break;

      case op_one:
        stv_2 = preval(cb, spec->right, level, param, realval);

        start = stv_2.a;
        end = stv_2.b;

        cb->iterator[level] = start;

        stv_2 = preval(cb, spec->right, level, stv_2.a, realval);
        stv = preval(cb, spec->left, level + 1, param, realval);

        if(stv.btl)
          {
            if(!stv_2.neg)
              p = stv.btl;
            else
              p = create_operation(op_not, stv.btl, NULL, "(~ %s)");

            q = stv.btldef;
          }
        else
          {
            p = NULL;
            q = NULL;
          }

        for(k = start + 1; k <= end; k++)
          {
            cb->iterator[level] = k;

            stv_2 = preval(cb, spec->right, level, k, realval);
            stv = preval(cb, spec->left, level + 1, param, realval);

            if(stv.btl)
              {
                if(!p)
                  {
                    p = stv.btl;
                    q = stv.btldef;
                  }
                else
                  {
                    if(!stv_2.neg)
                      p = create_operation(op_and, p, copy_specification(stv.btl), "(%s & %s)");
                    else
                      p = create_operation(op_and, p, create_operation(op_not, copy_specification(stv.btl), NULL, "(~ %s)"), "(%s & %s)");

                    if(stv.btldef)
                      q = create_operation(op_and, q, copy_specification(stv.btldef), "%s ; %s");
                  }
              }
          }

        stv.btl = p;
        stv.btldef = q;

        delete_specification(stv_2.btl);
        
        cb->scope_unit[level + 1]++;
      break;

      case op_one_check:
        stv_2 = preval(cb, spec->right, level, param, realval);

        if(stv_2.b < stv_2.a)
          {
            printmsg(cb->fancymsg, 4, TIME_FMT" : "TIME_FMT": Error, iteration parameter out of range in <one> construct: %s\n", stv_2.a, stv_2.b, spec->debug);
            exit_failure();
          }

        stv = preval(cb, spec->left, level, param, realval);

        if(stv.b < stv.a)
          {
            printmsg(cb->fancymsg, 4, TIME_FMT" : "TIME_FMT": Error, selection parameter out of range in <one> construct: %s\n", stv.a, stv.b, spec->debug);
            exit_failure();
          }

        if(!stv.neg)
          {
            if(stv.xtra)
              stv.neg = !(stv.a <= *stv.xtra && *stv.xtra <= stv.b);
            else
              stv.neg = !(stv.a <= param && param <= stv.b);
          }
        else
          {
            if(stv.xtra)
              stv.neg = (stv.a <= *stv.xtra && *stv.xtra <= stv.b);
            else
              stv.neg = (stv.a <= param && param <= stv.b);
          }

        stv.a = stv_2.a;
        stv.b = stv_2.b;

        delete_specification(stv_2.btl);
      break;

      case op_unique:
        stv = subset_unique(cb, spec, level, param, realval);
      break;

      case op_code:
        stv = preval(cb, spec->left, level + 2, param, realval);

        arity = check_root(stv.btl->symbol);

        if(!arity)
          {
            spec->left = create_operation(op_matrix, spec->left, create_operation(op_interval_1,
                         create_ground(op_iterator, "##", NULL_TIME, REAL_MAX), create_ground(op_iterator, "#", NULL_TIME, REAL_MAX), "%s , %s"), "%s(%s)");
            arity = 2;
          }

        stv_2 = preval(cb, spec->right, level, param, realval);

        l = strlen(stv_2.btl->symbol);

        if(!l)
          {
            printmsg(cb->fancymsg, 4, "Error, empty string in <code> construct: %s\n", spec->debug);
            exit_failure();
          }

        p = NULL;
        n = 0;
        for(h = 0; h < l; h++)
          {
            if(arity > 1)
              {
                cb->iterator[level] = h;
                cb->iterator[level + 1] = 0;                    
              }
            else
              cb->iterator[level] = 8 * n;

            stv = preval(cb, spec->left, level + ((arity > 1)? 2 : 1), param, realval);

            if(strlen(stv.btl->symbol) >= MAX_NAMELEN)
              {
                printmsg(cb->fancymsg, 4, "%s: Error, symbol name too long in <code> construct: %s\n", stv.btl->symbol, spec->debug);
                exit_failure();
              }

            c = stv_2.btl->symbol[h];

            if(c == '\\' && h < l - 1)
              {
                h++;

                switch(stv_2.btl->symbol[h])
                  {
                    case '\\':
                      c = '\\';
                    break;

                    case '\"':
                      c = '\"';
                    break;

                    case 'n':
                      c = '\n';
                    break;

                    case 'r':
                      c = '\r';
                    break;

                    case 'f':
                      c = '\f';
                    break;

                    case 't':
                      c = '\t';
                    break;

                    case 'v':
                      c = '\v';
                    break;

                    case 'a':
                      c = '\a';
                    break;

                    case 'b':
                      c = '\b';
                    break;

                    case 'x':
                      if(h < l - 1)
                        {
                          h++;

                          c = strtoul(&stv_2.btl->symbol[h], &endp, 16);
                          h = endp - stv_2.btl->symbol - 1;
                        }
                    break;
                  }
              }

            if(c & 1)
              r = stv.btl;
            else
              r = create_operation(op_not, copy_specification(stv.btl), NULL, "(~ %s)");

            for(k = 1; k < 8; k++)
              {
                if(arity > 1)
                  cb->iterator[level + 1] = k;                    
                else
                  cb->iterator[level] = 8 * n + k;

                stv = preval(cb, spec->left, level + ((arity > 1)? 2 : 1), param, realval);

                if(c & (1 << k))
                  r = create_operation(op_and, r, copy_specification(stv.btl), "(%s & %s)");
                else
                  r = create_operation(op_and, r, create_operation(op_not, copy_specification(stv.btl), NULL, "(~ %s)"), "(%s & %s)");
              }

            if(n == 0)
              p = r;
            else
              p = create_operation(op_and, p, r, "(%s & %s)");

            n++;
          }

        stv.btl = p;

        delete_specification(stv_2.btl);
      break;

      case op_code_num:      
        stv = preval(cb, spec->left, level + 2, param, realval);

        arity = check_root(stv.btl->symbol);

        if(!arity)
          {
            spec->left = create_operation(op_matrix, spec->left, create_operation(op_interval_1,
                         create_ground(op_iterator, "##", NULL_TIME, REAL_MAX), create_ground(op_iterator, "#", NULL_TIME, REAL_MAX), "%s , %s"), "%s(%s)");
            arity = 2;
          }

        stv_2 = preval(cb, spec->right, level, param, realval);

        if(stv_2.btl->value < 0)
          {
            printmsg(cb->fancymsg, 4, TIME_FMT": Error, number out of range in <code> construct: %s\n", stv_2.btl->value, spec->debug);
            exit_failure();
          }

        endp = (char *)&stv_2.btl->value;

        l = 0;
        for(h = sizeof(d_time) - 1; h >= 0; h--)
          if(endp[h])
            {
              for(l = 0; l <= h; l++)
                stv_2.btl->symbol[l] = endp[l];

              break;
            }

        if(!l)
          l = 1;

        p = NULL;
        for(h = 0; h < l; h++)
          {
            if(arity > 1)
              {
                cb->iterator[level] = h;
                cb->iterator[level + 1] = 0;                    
              }
            else
              cb->iterator[level] = 8 * h;

            stv = preval(cb, spec->left, level + ((arity > 1)? 2 : 1), param, realval);

            if(strlen(stv.btl->symbol) >= MAX_NAMELEN)
              {
                printmsg(cb->fancymsg, 4, "%s: Error, symbol name too long in <code> construct: %s\n", stv.btl->symbol, spec->debug);
                exit_failure();
              }

            if(stv_2.btl->symbol[h] & 1)
              r = stv.btl;
            else
              r = create_operation(op_not, copy_specification(stv.btl), NULL, "(~ %s)");

            for(k = 1; k < 8; k++)
              {
                if(arity > 1)
                  cb->iterator[level + 1] = k;
                else
                  cb->iterator[level] = 8 * h + k;

                stv = preval(cb, spec->left, level + ((arity > 1)? 2 : 1), param, realval);

                if(stv_2.btl->symbol[h] & (1 << k))
                  r = create_operation(op_and, r, copy_specification(stv.btl), "(%s & %s)");
                else
                  r = create_operation(op_and, r, create_operation(op_not, copy_specification(stv.btl), NULL, "(~ %s)"), "(%s & %s)");
              }

            if(h == 0)
              p = r;
            else
              p = create_operation(op_and, p, r, "(%s & %s)");
          }

        stv.btl = p;

        delete_specification(stv_2.btl);
      break;

      case op_combine:
        stv = exec_comb(cb, spec, level, param, realval);
      break;

      case op_com_delay:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level + 2, param, realval);

        arity = check_root(stv_2.btl->symbol);

        if(cb->seplit_fe)
          {
            gensym(cb, symbol, "VD", asserted, FALSE);
            gensym(cb, symbol1, "VD", negated, FALSE);
            gensym(cb, symbol2, "VD", undefined, TRUE);

            p = create_ground(op_name, symbol, NULL_TIME, REAL_MAX);
            np = create_operation(op_not, create_ground(op_name, symbol1, NULL_TIME, REAL_MAX), NULL, "(~ %s)");

            if(stv.btldef)
              stv.btldef = create_operation(op_and, stv.btldef, create_operation(op_and, CREATE_IMPLY(p, stv.btl), CREATE_IMPLY(copy_specification(stv.btl), np), "%s ; %s"), "%s ; %s");
            else
              stv.btldef = create_operation(op_and, CREATE_IMPLY(p, stv.btl), CREATE_IMPLY(copy_specification(stv.btl), np), "%s ; %s");

            n = 1;
            for(h = 0; h < (8 * sizeof(d_time)) && n <= abs(stv_2.a); h++)
              {
                if(arity > 1)
                  {
                    cb->iterator[level] = h / 8;
                    cb->iterator[level + 1] = h % 8;                    
                  }
                else
                  cb->iterator[level] = h;

                gensym(cb, symbol, "VD", asserted, FALSE);
                gensym(cb, symbol1, "VD", negated, FALSE);
                gensym(cb, symbol2, "VD", undefined, TRUE);

                p1 = create_ground(op_name, symbol, NULL_TIME, REAL_MAX);
                np1 = create_operation(op_not, create_ground(op_name, symbol1, NULL_TIME, REAL_MAX), NULL, "(~ %s)");

                stv_2 = preval(cb, spec->right, level + ((arity > 1)? 2 : 1), param, realval);

                q = copy_specification(stv_2.btl);

                stv.btldef = create_operation(op_and, stv.btldef, create_operation(op_and,
                             CREATE_IMPLY(q, create_operation(op_and,
                                             CREATE_IMPLY(p1, create_operation(op_delay, copy_specification(p), create_ground(op_number, "", (stv_2.a > 0)? n : - n, REAL_MAX), "(%s @ %s)")),
                                             CREATE_IMPLY(create_operation(op_delay, copy_specification(np), create_ground(op_number, "", (stv_2.a > 0)? n : - n, REAL_MAX), "(%s @ %s)"), np1), "%s ; %s")),
                             CREATE_IMPLY(create_operation(op_not, copy_specification(q), NULL, "(~ %s)"), create_operation(op_and,
                                             CREATE_IMPLY(copy_specification(p1), copy_specification(p)),
                                             CREATE_IMPLY(copy_specification(np), copy_specification(np1)), "%s ; %s")), "%s ; %s"), "%s ; %s");
                p = p1;
                np = np1;
                n *= 2;
              }

            stv.btl = create_ground(op_name, symbol2, NULL_TIME, REAL_MAX);
          }
        else
          {
            gensym(cb, symbol, "VD", asserted, TRUE);

            p = create_ground(op_name, symbol, NULL_TIME, REAL_MAX);

            if(stv.btldef)
              stv.btldef = create_operation(op_and, stv.btldef, CREATE_EQV(p, stv.btl), "%s ; %s");
            else
              stv.btldef = CREATE_EQV(p, stv.btl);

            n = 1;
            for(h = 0; h < (8 * sizeof(d_time)) && n <= abs(stv_2.a); h++)
              {
                if(arity > 1)
                  {
                    cb->iterator[level] = h / 8;
                    cb->iterator[level + 1] = h % 8;                    
                  }
                else
                  cb->iterator[level] = h;

                gensym(cb, symbol, "VD", asserted, TRUE);

                p1 = create_ground(op_name, symbol, NULL_TIME, REAL_MAX);

                stv_2 = preval(cb, spec->right, level + ((arity > 1)? 2 : 1), param, realval);

                q = copy_specification(stv_2.btl);

                stv.btldef = create_operation(op_and, stv.btldef, create_operation(op_and,
                                          CREATE_IMPLY(q, CREATE_EQV(p1, create_operation(op_delay, copy_specification(p), create_ground(op_number, "", (stv_2.a > 0)? n : - n, REAL_MAX), "(%s @ %s)"))),
                                          CREATE_IMPLY(create_operation(op_not, copy_specification(q), NULL, "(~ %s)"), CREATE_EQV(copy_specification(p1), copy_specification(p))), "%s ; %s"),
                                          "%s ; %s");
                p = p1;
                n *= 2;
              }

            stv.btl = copy_specification(p);
          }

        delete_specification(stv_2.btl);
      break;

      case op_sum:
        stv_2 = preval(cb, spec->right, level, param, realval);

        start = stv_2.a;
        end = stv_2.b;

        cb->iterator[level] = start;

        stv_2 = preval(cb, spec->right, level, start, realval);
        stv = preval(cb, spec->left, level + 1, param, realval);

        if(!stv.wtra)
          {
            stv = sum(cb, spec, level, param, realval);

            stv.wtra = (d_time *)FALSE;
          }
        else
          {
            if(stv.a == NULL_TIME)
              {
                val = 0;
                result = stv.realval;
              }
            else
              {
                val = stv.a;
                result = 0;
              }

            for(k = start + 1; k <= end; k++)
              {
                cb->iterator[level] = k;

                stv_2 = preval(cb, spec->right, level, k, realval);
                stv = preval(cb, spec->left, level + 1, param, realval);

                if(stv.a == NULL_TIME)
                  result += stv.realval;
                else
                  val += stv.a;

                delete_specification(stv.btl);
                delete_specification(stv_2.btl);
              }

            if(result)
              {
                stv.btl = create_ground(op_real, "", NULL_TIME, result + val);
                stv.a = NULL_TIME;
                stv.realval = result + val;
              }
            else
              {
                stv.btl = create_ground(op_number, "", val, REAL_MAX);
                stv.a = val;
                stv.b = val;
              }

            cb->scope_unit[level + 1]++;

            stv.wtra = (d_time *)TRUE;
          }
      break;

      case op_prod:
        stv_2 = preval(cb, spec->right, level, param, realval);

        start = stv_2.a;
        end = stv_2.b;

        cb->iterator[level] = start;

        stv_2 = preval(cb, spec->right, level, start, realval);
        stv = preval(cb, spec->left, level + 1, param, realval);

        if(!stv.wtra)
          {
            stv = prod(cb, spec, level, param, realval);

            stv.wtra = (d_time *)FALSE;
          }
        else
          {
            if(stv.a == NULL_TIME)
              {
                val = 1;
                result = stv.realval;
              }
            else
              {
                val = stv.a;
                result = 1;
              }

            for(k = start + 1; k <= end; k++)
              {
                cb->iterator[level] = k;

                stv_2 = preval(cb, spec->right, level, k, realval);
                stv = preval(cb, spec->left, level + 1, param, realval);

                if(stv.a == NULL_TIME)
                  result *= stv.realval;
                else
                  val *= stv.a;

                delete_specification(stv.btl);
                delete_specification(stv_2.btl);
              }

            if(result != 1)
              {
                stv.btl = create_ground(op_real, "", NULL_TIME, result * val);
                stv.a = NULL_TIME;
                stv.realval = result * val;
              }
            else
              {
                stv.btl = create_ground(op_number, "", val, REAL_MAX);
                stv.a = val;
                stv.b = val;
              }

            cb->scope_unit[level + 1]++;

            stv.wtra = (d_time *)TRUE;
          }
      break;

      case op_varinterval_1:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level, param, realval);

        stv.b = stv_2.a;
        stv.ytra = (d_time *)TRUE;

        stv.btltwo = stv_2.btl;
      break;

      case op_varinterval_2:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level, param, realval);

        stv.a++;
        stv.b = stv_2.a;
        stv.ytra = (d_time *)TRUE;

        stv.btltwo = stv_2.btl;
      break;

      case op_varinterval_3:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level, param, realval);

        stv.b = stv_2.a;
        stv.xtra = (d_time *)TRUE;

        delete_specification(stv_2.btl);
      break;

      case op_varinterval_4:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level, param, realval);

        stv.b = stv_2.a - 1;
        stv.xtra = (d_time *)TRUE;

        delete_specification(stv_2.btl);
      break;

      case op_varinterval_5:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level, param, realval);

        stv.b = stv_2.a;
        stv.xtra = (d_time *)TRUE;
        stv.ytra = (d_time *)TRUE;

        stv.btltwo = stv_2.btl;
      break;

      case op_com_at:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level + 2, param, realval);

        if(stv_2.xtra)
          {
            arity = check_root(stv_2.btl->symbol);
            stv_3 = com_at_happen(cb, stv.btl, stv.btldef, spec->right->left, arity, stv_2.a, level, param, realval, FALSE);
          }
        else
          {
            if(stv_2.a < 0)
              stv_3 = at_happen(cb, stv.btl, stv.btldef, stv_2.a, 0, level, param, realval, FALSE);
            else
              if(stv_2.a > 0)
                {
                  printmsg(cb->fancymsg, 4, TIME_FMT": Error, possibly empty interval after <@>: %s\n", stv_2.a, stv_2.btl->debug);
                  exit_failure();
                }
              else
                {
                  stv_3.btl = NULL;
                  stv_3.btldef = NULL;
                }
          }

        if(stv_2.ytra)
          {
            arity = check_root(stv_2.btltwo->symbol);
            stv_4 = com_at_happen(cb, stv.btl, stv.btldef, spec->right->right, arity, stv_2.b, level, param, realval, FALSE);
          }
        else
          {
            if(stv_2.b > 0)
              stv_4 = at_happen(cb, stv.btl, stv.btldef, 0, stv_2.b, level, param, realval, FALSE);
            else
              if(stv_2.b < 0)
                {
                  printmsg(cb->fancymsg, 4, TIME_FMT": Error, possibly empty interval after <@>: %s\n", stv_2.b, stv_2.btl->debug);
                  exit_failure();
                }
              else
                {
                  stv_4.btl = NULL;
                  stv_4.btldef = NULL;
                }
          }

        if(stv_3.btl)
          {
            if(stv_4.btl)
              stv.btl = create_operation(op_and, stv_3.btl, copy_specification(stv_4.btl), "(%s & %s)");
            else
              stv.btl = stv_3.btl;
          }
        else
          stv.btl = copy_specification(stv_4.btl);        

        if(stv_3.btldef)
          {
            if(stv_4.btldef)
              stv.btldef = create_operation(op_and, stv_3.btldef, copy_specification(stv_4.btldef), "%s ; %s");
            else
              stv.btldef = stv_3.btldef;
          }
        else
          stv.btldef = copy_specification(stv_4.btldef);

        delete_specification(stv_2.btl);
      break;

      case op_com_happen:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level + 2, param, realval);

        if(stv_2.xtra)
          {
            arity = check_root(stv_2.btl->symbol);
            stv_3 = com_at_happen(cb, stv.btl, stv.btldef, spec->right->left, arity, stv_2.a, level, param, realval, TRUE);
          }
        else
          {
            if(stv_2.a < 0)
              stv_3 = at_happen(cb, stv.btl, stv.btldef, stv_2.a, 0, level, param, realval, TRUE);
            else
              if(stv_2.a > 0)
                {
                  printmsg(cb->fancymsg, 4, TIME_FMT": Error, possibly empty interval after <?>: %s\n", stv_2.a, stv_2.btl->debug);
                  exit_failure();
                }
              else
                {
                  stv_3.btl = NULL;
                  stv_3.btldef = NULL;
                }
          }

        if(stv_2.ytra)
          {
            arity = check_root(stv_2.btltwo->symbol);
            stv_4 = com_at_happen(cb, stv.btl, stv.btldef, spec->right->right, arity, stv_2.b, level, param, realval, TRUE);
          }
        else
          {
            if(stv_2.b > 0)
              stv_4 = at_happen(cb, stv.btl, stv.btldef, 0, stv_2.b, level, param, realval, TRUE);
            else
              if(stv_2.b < 0)
                {
                  printmsg(cb->fancymsg, 4, TIME_FMT": Error, possibly empty interval after <?>: %s\n", stv_2.b, stv_2.btl->debug);
                  exit_failure();
                }
              else
                {
                  stv_4.btl = NULL;
                  stv_4.btldef = NULL;
                }
          }

        if(stv_3.btl)
          {
            if(stv_4.btl)
              stv.btl = create_operation(op_or, stv_3.btl, copy_specification(stv_4.btl), "(%s | %s)");
            else
              stv.btl = stv_3.btl;
          }
        else
          stv.btl = copy_specification(stv_4.btl);        

        if(stv_3.btldef)
          {
            if(stv_4.btldef)
              stv.btldef = create_operation(op_and, stv_3.btldef, copy_specification(stv_4.btldef), "%s ; %s");
            else
              stv.btldef = stv_3.btldef;
          }
        else
          stv.btldef = copy_specification(stv_4.btldef);

        delete_specification(stv_2.btl);
      break;

      case op_com_only:
        printmsg(cb->fancymsg, 4, "Error, operator <!> does not allow dynamic intervals (by now)\n");
        exit_failure();
      break;

      case op_pvalue:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level, param, realval);

        if(strlen(stv.btl->symbol) >= MAX_NAMELEN)
          {
            printmsg(cb->fancymsg, 4, "%s: Error, symbol name too long in <+$> construct: %s\n", stv.btl->symbol, spec->debug);
            exit_failure();
          }

        if(!check_root(stv.btl->symbol))
          {
            spec->left = create_operation(op_matrix, spec->left, create_operation(op_interval_1,
                         create_ground(op_iterator, "##", NULL_TIME, REAL_MAX), create_ground(op_iterator, "#", NULL_TIME, REAL_MAX), "%s , %s"), "%s(%s)");

            stv = preval(cb, spec->left, level, param, realval);
          }

        stv.a = stv_2.a;
      break;

      case op_nvalue:
        stv = preval(cb, spec->left, level, param, realval);
        stv_2 = preval(cb, spec->right, level, param, realval);

        if(strlen(stv.btl->symbol) >= MAX_NAMELEN)
          {
            printmsg(cb->fancymsg, 4, "%s: Error, symbol name too long in <-$> construct: %s\n", stv.btl->symbol, spec->debug);
            exit_failure();
          }

        if(!check_root(stv.btl->symbol))
          {
            spec->left = create_operation(op_matrix, spec->left, create_operation(op_interval_1,
                         create_ground(op_iterator, "##", NULL_TIME, REAL_MAX), create_ground(op_iterator, "#", NULL_TIME, REAL_MAX), "%s , %s"), "%s(%s)");

            stv = preval(cb, spec->left, level, param, realval);
          }

        stv.a = - stv_2.a;
      break;

      case op_iter:
        stv_2 = preval(cb, spec->right, level, param, realval);

        start = stv_2.a;
        end = stv_2.b;

        if(end < start)
          {
            printmsg(cb->fancymsg, 4, TIME_FMT" : "TIME_FMT": Error, iteration parameter out of range in <iter> construct: %s\n", stv_2.a, stv_2.b, spec->debug);
            exit_failure();
          }

        cb->iterator[level] = start;

        stv_2 = preval(cb, spec->right, level, start, realval);
        stv = preval(cb, spec->left, level + 1, param, realval);

        p = stv.btl;
        q = stv.btldef;

        for(k = start + 1; k <= end; k++)
          {
            cb->iterator[level] = k;

            stv_2 = preval(cb, spec->right, level, k, realval);
            stv = preval(cb, spec->left, level + 1, param, realval);

            if(stv.btl)
              {
                if(!p)
                  {
                    p = stv.btl;
                    q = stv.btldef;
                  }
                else
                  {
                    p = create_operation(op_join, p, copy_specification(stv.btl), "%s ; %s");

                    if(stv.btldef)
                      q = create_operation(op_and, q, copy_specification(stv.btldef), "%s ; %s");
                  }
              }
          }

        stv.btl = p;
        stv.btldef = q;

        delete_specification(stv_2.btl);
        
        cb->scope_unit[level + 1]++;
      break;

      case op_when:
        if(level == 0)
          {
            printmsg(cb->fancymsg, 4, "Error, <when> construct out of scope: %s\n", spec->debug);
            exit_failure();
          }

        stv_2 = preval(cb, spec->right, level, param, realval);

        if(stv_2.b < stv_2.a)
          {
            printmsg(cb->fancymsg, 4, TIME_FMT" : "TIME_FMT": Error, selection parameter out of range in <when> construct: %s\n", stv_2.a, stv_2.b, spec->debug);
            exit_failure();
          }

        if(!stv_2.neg)
          {
            if(stv_2.xtra)
              stv_2.neg = !(stv_2.a <= *stv_2.xtra && *stv_2.xtra <= stv_2.b);
            else
              stv_2.neg = !(stv_2.a <= cb->iterator[level - 1] && cb->iterator[level - 1] <= stv_2.b);
          }
        else
          {
            if(stv_2.xtra)
              stv_2.neg = (stv_2.a <= *stv_2.xtra && *stv_2.xtra <= stv_2.b);
            else
              stv_2.neg = (stv_2.a <= cb->iterator[level - 1] && cb->iterator[level - 1] <= stv_2.b);
          }

        if(!stv_2.neg)
          stv = preval(cb, spec->left, level, param, realval);

        delete_specification(stv_2.btl);
      break;

      case op_in:
        stv_2 = preval(cb, spec->right, level, param, realval);
        stv = preval(cb, spec->left, level + 1, param, realval);
        
        stv.a = stv_2.a;
        stv.b = stv_2.b;
        stv.xtra = &stv.btl->value;

        delete_specification(stv_2.btl);
      break;

      case op_neg_range:
        stv = preval(cb, spec->left, level, param, realval);
        stv.neg = !stv.neg;
      break;

      default:
        printmsg(cb->fancymsg, 5, "Internal error, unmanaged operator in phase 1 (%d)\n", spec->ot);
        assert(FALSE);
      break;
    }

  return stv;
}

subtreeval forall(c_base *cb, btl_specification *spec, int level, d_time param, real realval)
{
  subtreeval stv, stv_2;
  btl_specification *p, *q;
  int k, start, end;

  stv_2 = preval(cb, spec->right, level, param, realval);

  start = stv_2.a;
  end = stv_2.b;

  if(end < start)
    {
      printmsg(cb->fancymsg, 4, TIME_FMT" : "TIME_FMT": Error, iteration parameter out of range in <forall> construct: %s\n", stv_2.a, stv_2.b, spec->debug);
      exit_failure();
    }

  cb->iterator[level] = start;

  stv_2 = preval(cb, spec->right, level, start, realval);
  stv = preval(cb, spec->left, level + 1, param, realval);
  p = stv.btl;
  q = stv.btldef;

  for(k = start + 1; k <= end; k++)
    {
      cb->iterator[level] = k;

      stv_2 = preval(cb, spec->right, level, k, realval);
      stv = preval(cb, spec->left, level + 1, param, realval);

      if(stv.btl)
        {
          if(!p)
            {
              p = stv.btl;
              q = stv.btldef;
            }
          else
            {
              p = create_operation(op_and, p, copy_specification(stv.btl), "(%s & %s)");

             if(stv.btldef)
               q = create_operation(op_and, q, copy_specification(stv.btldef), "%s ; %s");
            }
        }
    }

  stv.btl = p;
  stv.btldef = q;

  delete_specification(stv_2.btl);

  cb->scope_unit[level + 1]++;
  
  return stv;
}

subtreeval exists(c_base *cb, btl_specification *spec, int level, d_time param, real realval)
{
  subtreeval stv, stv_2;
  btl_specification *p, *q;
  int k, start, end;

  stv_2 = preval(cb, spec->right, level, param, realval);

  start = stv_2.a;
  end = stv_2.b;

  if(end < start)
    {
      printmsg(cb->fancymsg, 4, TIME_FMT" : "TIME_FMT": Error, iteration parameter out of range in <exists> construct: %s\n", stv_2.a, stv_2.b, spec->debug);
      exit_failure();
    }

  cb->iterator[level] = start;

  stv_2 = preval(cb, spec->right, level, start, realval);
  stv = preval(cb, spec->left, level + 1, param, realval);

  p = stv.btl;
  q = stv.btldef;

  for(k = start + 1; k <= end; k++)
    {
      cb->iterator[level] = k;

      stv_2 = preval(cb, spec->right, level, k, realval);
      stv = preval(cb, spec->left, level + 1, param, realval);

      if(stv.btl)
        {
          if(!p)
            {
              p = stv.btl;
              q = stv.btldef;
            }
          else
            {
              p = create_operation(op_or, p, copy_specification(stv.btl), "(%s | %s)");

              if(stv.btldef)
                q = create_operation(op_and, q, copy_specification(stv.btldef), "%s ; %s");
            }
        }
    }

  stv.btl = p;
  stv.btldef = q;

  delete_specification(stv_2.btl);
        
  cb->scope_unit[level + 1]++;

  return stv;
}

subtreeval unique(c_base *cb, btl_specification *spec, int level, d_time param, real realval)
{
  subtreeval stv, stv_2;
  btl_specification *p, *q, *r;
  int h, k, start, end;

  stv_2 = preval(cb, spec->right, level, param, realval);

  start = stv_2.a;
  end = stv_2.b;

  if(end < start)
    {
      printmsg(cb->fancymsg, 4, TIME_FMT" : "TIME_FMT": Error, iteration parameter out of range in <unique> construct: %s\n", stv_2.a, stv_2.b, spec->debug);
      exit_failure();
    }

  p = NULL;
  q = NULL;
  r = NULL;
  for(h = start; h <= end; h++)
    {
      cb->iterator[level] = start;

      stv_2 = preval(cb, spec->right, level, start, realval);
      stv = preval(cb, spec->left, level + 1, param, realval);

      if(stv.btl)
        {
          if(h == start)
            {
              r = stv.btl;
              q = stv.btldef;
            }
          else
            {
              r = create_operation(op_not, copy_specification(stv.btl), NULL, "(~ %s)");

              if(stv.btldef)
                q = create_operation(op_and, q, copy_specification(stv.btldef), "%s ; %s");
            }
        }

      for(k = start + 1; k <= end; k++)
        {
          cb->iterator[level] = k;

          stv_2 = preval(cb, spec->right, level, k, realval);
          stv = preval(cb, spec->left, level + 1, param, realval);

          if(stv.btl)
            {
              if(!r)
                {
                  if(h == k)
                    r = stv.btl;
                  else
                    r = create_operation(op_not, copy_specification(stv.btl), NULL, "(~ %s)");

                  q = stv.btldef;
                }
              else
                {
                  if(h == k)
                    r = create_operation(op_and, r, copy_specification(stv.btl), "(%s & %s)");
                  else
                    r = create_operation(op_and, r, create_operation(op_not, copy_specification(stv.btl), NULL, "(~ %s)"), "(%s & %s)");

                  if(stv.btldef)
                    q = create_operation(op_and, q, copy_specification(stv.btldef), "%s ; %s");
                }
            }
        }

      if(h == start)
        p = r;
      else
        p = create_operation(op_or, p, r, "(%s | %s)");
    }

  stv.btl = p;
  stv.btldef = q;

  delete_specification(stv_2.btl);
        
  cb->scope_unit[level + 1]++;

  return stv;
}

btl_specification *dynamic_offset(int step, int offset)
{
  return create_operation(op_plus, create_operation(op_mul, create_ground(op_number, "", step, REAL_MAX), create_ground(op_iterator, "#", NULL_TIME, REAL_MAX), "%s * %s"),
         create_ground(op_number, "", offset, REAL_MAX), "%s + %s");
}

void fix_iterators(btl_specification *spec, int num)
{
  switch(spec->ot)
    {
    case op_iterator:
      if(strlen(spec->symbol) > num)
        strcat(spec->symbol, "#");
    break;
    
    case op_forall:
    case op_exists:
    case op_unique:
    case op_one:
      fix_iterators(spec->left, num + 1);
      fix_iterators(spec->right, num);
    break;
    
    default:
      if(spec->left)
        fix_iterators(spec->left, num);

      if(spec->right)
        fix_iterators(spec->right, num);
    break;
    }
}

subtreeval subset_unique(c_base *cb, btl_specification *spec, int level, d_time param, real realval)
{
  subtreeval stv, stv_2, stv_3;
  btl_specification *o, *p, *q, *r, *s, *x, *z, *h, *h1, *i, *i1, *j, *j1, *k, *l, *m, *fixed;
  int start, end, root, ratio, len, delta, offset_x, offset_y;
  char symbol[MAX_NAMELEN];

  if(spec->right->ot == op_in)
    stv_2 = preval(cb, spec->right->right, level, param, realval);
  else
    stv_2 = preval(cb, spec->right, level, param, realval);

  start = stv_2.a;
  end = stv_2.b;

  if(end < start)
    {
      printmsg(cb->fancymsg, 4, TIME_FMT" : "TIME_FMT": Error, iteration parameter out of range in <unique> construct: %s\n", stv_2.a, stv_2.b, spec->debug);
      exit_failure();
    }

  len = end - start + 1;

  root = floor(sqrt(len));

  if(root < 2)
    return unique(cb, spec, level, param, realval);

  ratio = ceil((real)len / root);

  offset_x = start % root;
  offset_y = start / root;

  delta = root * ratio - len;

  gensym(cb, symbol, "UQ", asserted, TRUE);

  o = create_operation(op_vector, create_ground(op_name, symbol, NULL_TIME, REAL_MAX), create_ground(op_iterator, "#", NULL_TIME, REAL_MAX), "%s(%s)");

  h = create_operation(op_interval_3, dynamic_offset(root, offset_x), dynamic_offset(root, offset_x + root), "%s : %s - 1");

  k = create_operation(op_interval_3, create_ground(op_number, "", offset_y, REAL_MAX), create_ground(op_number, "", offset_y + ratio, REAL_MAX), "%s : %s - 1");

  fixed = copy_specification(spec->left);

  if(spec->right->ot == op_in)
    h1 = create_operation(op_in, spec->right->left, h, "%s in %s");
  else
    {
      h1 = h;

      fix_iterators(fixed, 1);
    }

  if(delta > 0)
    {
      i = create_operation(op_interval_3, dynamic_offset(root, offset_x), dynamic_offset(root, offset_x + root - delta), "%s : %s - 1");
      j = create_operation(op_interval_3, dynamic_offset(root, offset_x + root - delta), dynamic_offset(root, offset_x + root), "%s : %s - 1");

      l = create_operation(op_interval_3, create_ground(op_number, "", offset_y, REAL_MAX), create_ground(op_number, "", offset_y + ratio - 1, REAL_MAX), "%s : %s - 1");
      m = create_operation(op_interval_1, create_ground(op_number, "", offset_y + ratio - 1, REAL_MAX), create_ground(op_number, "", offset_y + ratio - 1, REAL_MAX), "%s : %s");

      if(spec->right->ot == op_in)
        {
          i1 = create_operation(op_in, spec->right->left, i, "%s in %s");
          j1 = create_operation(op_in, spec->right->left, j, "%s in %s");
        }
      else
        {
          i1 = i;
          j1 = j;
          
          fix_iterators(fixed, 1);
        }

      x = create_operation(op_forall, create_operation(op_forall,
                           CREATE_EQV(fixed, o),
                           h1, "forall(%s , %s)"), l, "forall(%s , %s)");
                           
      x = create_operation(op_and, x, create_operation(op_forall, create_operation(op_forall,
                           CREATE_EQV(copy_specification(fixed), copy_specification(o)),
                           i1, "forall(%s , %s)"), m, "forall(%s , %s)"), "%s & %s");
                           
      x = create_operation(op_and, x, create_operation(op_forall, create_operation(op_forall,
                           create_operation(op_not, copy_specification(o), NULL, "(~ %s)"),
                           j1, "forall(%s , %s)"), m, "forall(%s , %s)"), "%s & %s");
    }
  else
    x = create_operation(op_forall, create_operation(op_forall, CREATE_EQV(fixed, o), h1, "forall(%s , %s)"), k, "forall(%s , %s)");
  
  p = create_operation(op_exists, copy_specification(o), copy_specification(h), "exists(%s , %s)");  
  r = create_operation(op_unique, copy_specification(o), copy_specification(h), "unique(%s , %s)");

  q = create_operation(op_unique, p, copy_specification(k), "unique(%s , %s)");
  s = create_operation(op_exists, r, copy_specification(k), "exists(%s , %s)");

  z = create_operation(op_and, q, s, "%s & %s");
  
  stv = preval(cb, z, level + 1, param, realval);

  do
    {
      stv_3 = preval(cb, x, level + 1, param, realval);

      if(stv.btldef)
        stv.btldef = create_operation(op_and, stv_3.btl, stv.btldef, "%s ; %s");
      else
        stv.btldef = stv_3.btl;
       
      x = stv_3.btldef;
    }
  while(x);

  delete_specification(stv_2.btl);

  cb->scope_unit[level + 1]++;

  return stv;
}

btl_specification *combine(c_base *cb, btl_specification *ap[], btl_specification *spec, int h, int k, int level, d_time param, real realval)
  {
    subtreeval stv;
    btl_specification *btldef, *newbtl;
    int i, n;

    if(h <= k)
      {
        n = pow(2, k - h);
        btldef = NULL;

        cb->iterator[level] = k;

        preval(cb, spec->right, level, k, realval);
        stv = preval(cb, spec->left->left, level + 1, param, realval);

        for(i = 0; i < n; i++)
          {
            if(ap[i])
              {
                ap[i] = create_operation(op_and, ap[i], create_operation(op_not, copy_specification(stv.btl), NULL, "(~ %s)"), "(%s & %s)");
                ap[i + n] = create_operation(op_and, ap[i + n], copy_specification(stv.btl), "(%s & %s)");
              }
            else
              {
                ap[i] = create_operation(op_not, copy_specification(stv.btl), NULL, "(~ %s)");
                ap[i + n] = copy_specification(stv.btl);
              }

            if(stv.btldef)
              {
                if(btldef)
                  btldef = create_operation(op_and, stv.btldef, btldef, "%s ; %s");
                else
                  btldef = stv.btldef;
              }
          }

        newbtl = combine(cb, ap, spec, h, k - 1, level, param, realval);

        if(newbtl)
          {
            if(btldef)
              btldef = create_operation(op_and, newbtl, btldef, "%s ; %s");
            else
              btldef = newbtl;
          }

        newbtl = combine(cb, ap + n, spec, h, k - 1, level, param + n, realval);

        if(newbtl)
          {
            if(btldef)
              btldef = create_operation(op_and, newbtl, btldef, "%s ; %s");
            else
              btldef = newbtl;
          }
      }
    else
      {
        cb->iterator[level] = param;

        preval(cb, spec->right, level, param, realval);
        stv = preval(cb, spec->left->right, level + 1, param, realval);
        btldef = stv.btldef;

        ap[0] = CREATE_EQV(ap[0], stv.btl);
      }

  return btldef;
}

subtreeval exec_comb(c_base *cb, btl_specification *spec, int level, d_time param, real realval)
  {
    btl_specification *ap[NUM_COMBS];
    subtreeval stv;
    int i, n;

    stv = preval(cb, spec->right, level, param, realval);
    if(stv.b < stv.a)
      {
        printmsg(cb->fancymsg, 4, TIME_FMT": Error, iteration parameter out of range in <combine> construct: %s\n", stv.btl->value, spec->debug);
        exit_failure();
      }

    n = pow(2, stv.b - stv.a);
    if(n >= NUM_COMBS)
      {
        printmsg(cb->fancymsg, 4, TIME_FMT": Error, too many combinations generated by <combine> construct: %s\n", stv.btl->value, spec->debug);
        exit_failure();
      }

    for(i = 0; i < n; i++)
      ap[i] = NULL;

    stv.btldef = combine(cb, ap, spec, stv.a, stv.b, level, 0, realval);

    stv.btl = ap[0];
    for(i = 1; i < 2 * n; i++)
      stv.btl = create_operation(op_and, stv.btl, ap[i], "(%s & %s)");

    cb->scope_unit[level + 1]++;
      
    return stv;
  }

subtreeval sum(c_base *cb, btl_specification *spec, int level, d_time param, real realval)
{
  subtreeval stv, stv_2;
  btl_specification *p, *q;
  int k, start, end;

  stv_2 = preval(cb, spec->right, level, param, realval);

  start = stv_2.a;
  end = stv_2.b;

  if(end < start)
    {
      printmsg(cb->fancymsg, 4, TIME_FMT" : "TIME_FMT": Error, iteration parameter out of range in <sum> construct: %s\n", stv_2.a, stv_2.b, spec->debug);
      exit_failure();
    }

  cb->iterator[level] = start;

  stv_2 = preval(cb, spec->right, level, start, realval);
  stv = preval(cb, spec->left, level + 1, param, realval);
  p = stv.btl;
  q = stv.btldef;

  for(k = start + 1; k <= end; k++)
    {
      cb->iterator[level] = k;

      stv_2 = preval(cb, spec->right, level, k, realval);
      stv = preval(cb, spec->left, level + 1, param, realval);

      if(stv.btl)
        {
          if(!p)
            {
              p = stv.btl;
              q = stv.btldef;
            }
          else
            {
              p = create_operation(op_plus, p, copy_specification(stv.btl), "(%s + %s)");

             if(stv.btldef)
               q = create_operation(op_and, q, copy_specification(stv.btldef), "%s ; %s");
            }
        }
    }

  stv.btl = p;
  stv.btldef = q;

  delete_specification(stv_2.btl);

  cb->scope_unit[level + 1]++;
  
  return stv;
}

subtreeval prod(c_base *cb, btl_specification *spec, int level, d_time param, real realval)
{
  subtreeval stv, stv_2;
  btl_specification *p, *q;
  int k, start, end;

  stv_2 = preval(cb, spec->right, level, param, realval);

  start = stv_2.a;
  end = stv_2.b;

  if(end < start)
    {
      printmsg(cb->fancymsg, 4, TIME_FMT" : "TIME_FMT": Error, iteration parameter out of range in <prod> construct: %s\n", stv_2.a, stv_2.b, spec->debug);
      exit_failure();
    }

  cb->iterator[level] = start;

  stv_2 = preval(cb, spec->right, level, start, realval);
  stv = preval(cb, spec->left, level + 1, param, realval);
  p = stv.btl;
  q = stv.btldef;

  for(k = start + 1; k <= end; k++)
    {
      cb->iterator[level] = k;

      stv_2 = preval(cb, spec->right, level, k, realval);
      stv = preval(cb, spec->left, level + 1, param, realval);

      if(stv.btl)
        {
          if(!p)
            {
              p = stv.btl;
              q = stv.btldef;
            }
          else
            {
              p = create_operation(op_mul, p, copy_specification(stv.btl), "(%s * %s)");

             if(stv.btldef)
               q = create_operation(op_and, q, copy_specification(stv.btldef), "%s ; %s");
            }
        }
    }

  stv.btl = p;
  stv.btldef = q;

  delete_specification(stv_2.btl);

  cb->scope_unit[level + 1]++;
  
  return stv;
}

subtreeval at_happen(c_base *cb, btl_specification *btl, btl_specification *btldef, d_time a, d_time b, int level, d_time param, real realval, bool dual)
{
  subtreeval stv;
  char symbol[MAX_NAMELEN], symbol1[MAX_NAMELEN], symbol2[MAX_NAMELEN], symbol_h[BTL_HISTORY_LEN][MAX_NAMELEN];
  btl_specification *newbtl, *p, *q, *p1, *q1, *r, *btl_history[BTL_HISTORY_LEN], *btl_history1[BTL_HISTORY_LEN];
  d_time h, n, tail;
  bool sign;

  if(a > b)
    {
      printmsg(cb->fancymsg, 4, TIME_FMT", "TIME_FMT": Error, empty interval after <%c>: %s\n", a, b, dual? '?' : '@', btl->debug);
      exit_failure();
    }

  for(h = 0; h < BTL_HISTORY_LEN; h++)
    {
      btl_history[h] = NULL;
      btl_history1[h] = NULL;
    }

  sign = abs(a) < abs(b);

  if(cb->seplit_fe)
    {
      tail = b - a + 1;
      r = NULL;

      gensym(cb, symbol, dual? "HP" : "AT", asserted, FALSE);
      gensym(cb, symbol1, dual? "HP" : "AT", negated, FALSE);
      gensym(cb, symbol2, dual? "HP" : "AT", undefined, TRUE);

      btl_history[0] = create_ground(op_name, symbol, NULL_TIME, REAL_MAX);
      btl_history1[0] = create_operation(op_not, create_ground(op_name, symbol1, NULL_TIME, REAL_MAX), NULL, "(~ %s)");
      strcpy(symbol_h[0], symbol2);

      newbtl = create_operation(op_and, CREATE_IMPLY(btl_history[0], btl), CREATE_IMPLY(copy_specification(btl), btl_history1[0]), "(%s & %s)");

      do
        {
          p = btl_history[0];
          p1 = btl_history1[0];

          n = 2;
          h = 1;

          while(tail >= n)
            {
              if(h >= BTL_HISTORY_LEN)
                {
                  printmsg(cb->fancymsg, 4, "Error, interval too large: %s\n", btl->debug);
                  exit_failure();
                }

              if(btl_history[h])
                {
                  p = btl_history[h];
                  p1 = btl_history1[h];
                }
              else
                {
                  q = create_operation(dual? op_or : op_and, copy_specification(p),
                                                             create_operation(op_delay, copy_specification(p),
                                                                                        create_ground(op_number, "", sign? n / 2 : - n / 2, REAL_MAX), "(%s @ %s)"), dual? "(%s | %s)" : "(%s & %s)");

                  q1 = create_operation(dual? op_or : op_and, copy_specification(p1),
                                                              create_operation(op_delay, copy_specification(p1),
                                                                                         create_ground(op_number, "", sign? n / 2 : - n / 2, REAL_MAX), "(%s @ %s)"), dual? "(%s | %s)" : "(%s & %s)");

                  gensym(cb, symbol, dual? "HP" : "AT", asserted, FALSE);
                  gensym(cb, symbol1, dual? "HP" : "AT", negated, FALSE);
                  gensym(cb, symbol2, dual? "HP" : "AT", undefined, TRUE);

                  p = create_ground(op_name, symbol, NULL_TIME, REAL_MAX);
                  p1 = create_operation(op_not, create_ground(op_name, symbol1, NULL_TIME, REAL_MAX), NULL, "(~ %s)");
                  btl_history[h] = p;
                  btl_history1[h] = p1;
                  strcpy(symbol_h[h], symbol2);

                  newbtl = create_operation(op_and, newbtl, create_operation(op_and, CREATE_IMPLY(p, q), CREATE_IMPLY(q1, p1), "(%s & %s)"), "%s ; %s");
                }

              n *= 2;
              h++;
            }

          q = create_operation(op_delay, create_ground(op_name, symbol_h[h - 1], NULL_TIME, REAL_MAX), create_ground(op_number, "", sign? a : b, REAL_MAX), "(%s @ %s)");

          if(r)
            r = create_operation(dual? op_or : op_and, q, r, dual? "(%s | %s)" : "(%s & %s)");
          else
            r = q;

          tail -= n / 2;

          if(sign)
            a = b - tail + 1;
          else
            b = a + tail - 1;
        }
      while(tail > 0);

      stv.btl = r;
    }
  else
    {
      tail = b - a + 1;
      r = NULL;

      gensym(cb, symbol, dual? "HP" : "AT", asserted, TRUE);

      btl_history[0] = create_ground(op_name, symbol, NULL_TIME, REAL_MAX);

      newbtl = CREATE_EQV(btl_history[0], btl);

      do
        {
          p = btl_history[0];

          n = 2;
          h = 1;

          while(tail >= n)
            {
              if(h >= BTL_HISTORY_LEN)
                {
                  printmsg(cb->fancymsg, 4, "Error, interval too large: %s\n", btl->debug);
                  exit_failure();
                }

              if(btl_history[h])
                p = btl_history[h];
              else
                {
                  q = create_operation(dual? op_or : op_and, copy_specification(p),
                                                             create_operation(op_delay, copy_specification(p),
                                                                                        create_ground(op_number, "", sign? n / 2 : - n / 2, REAL_MAX), "(%s @ %s)"), dual? "(%s | %s)" : "(%s & %s)");

                  gensym(cb, symbol, dual? "HP" : "AT", asserted, TRUE);

                  p = create_ground(op_name, symbol, NULL_TIME, REAL_MAX);
                  btl_history[h] = p;

                  newbtl = create_operation(op_and, newbtl, CREATE_EQV(p, q), "%s ; %s");
                }

              n *= 2;
              h++;
            }

          q = create_operation(op_delay, copy_specification(p), create_ground(op_number, "", sign? a : b, REAL_MAX), "(%s @ %s)");

          if(r)
            r = create_operation(dual? op_or : op_and, q, r, dual? "(%s | %s)" : "(%s & %s)");
          else
            r = q;

          tail -= n / 2;

          if(sign)
            a = b - tail + 1;
          else
            b = a + tail - 1;
        }
      while(tail > 0);

      stv.btl = r;
    }

  if(btldef)
    stv.btldef = create_operation(op_and, newbtl, btldef, "%s ; %s");
  else
    stv.btldef = newbtl;

  return stv;
}

subtreeval pplus_mmul(c_base *cb, btl_specification *btl, btl_specification *btldef, d_time a, d_time b, int level, d_time param, real realval, bool dual)
{
  subtreeval stv;
  char symbol[MAX_NAMELEN];
  btl_specification *newbtl, *p, *q, *r, *btl_history[BTL_HISTORY_LEN];
  d_time h, n, tail;
  bool sign;

  if(a > b)
    {
      printmsg(cb->fancymsg, 4, TIME_FMT", "TIME_FMT": Error, empty interval after <%s>: %s\n", a, b, dual? "**" : "++", btl->debug);
      exit_failure();
    }

  for(h = 0; h < BTL_HISTORY_LEN; h++)
    btl_history[h] = NULL;

  sign = abs(a) < abs(b);

  tail = b - a + 1;
  r = NULL;

  gensym(cb, symbol, dual? "TS" : "TP", continuos, TRUE);

  btl_history[0] = create_ground(op_variable, symbol, NULL_TIME, REAL_MAX);

  newbtl = CREATE_REQV(btl_history[0], btl);

  do
    {
      p = btl_history[0];

      n = 2;
      h = 1;

      while(tail >= n)
        {
          if(h >= BTL_HISTORY_LEN)
            {
              printmsg(cb->fancymsg, 4, "Error, interval too large: %s\n", btl->debug);
              exit_failure();
            }

          if(btl_history[h])
            p = btl_history[h];
          else
            {
              q = create_operation(dual? op_mul : op_plus, copy_specification(p),
                                                             create_operation(op_math_delay, copy_specification(p),
                                                                                        create_ground(op_number, "", sign? n / 2 : - n / 2, REAL_MAX), "(%s @ %s)"), dual? "(%s * %s)" : "(%s + %s)");

              gensym(cb, symbol, dual? "TS" : "TP", continuos, TRUE);

              p = create_ground(op_variable, symbol, NULL_TIME, REAL_MAX);
              btl_history[h] = p;

              newbtl = create_operation(op_and, newbtl, CREATE_REQV(p, q), "%s ; %s");
            }

          n *= 2;
          h++;
        }

      q = create_operation(op_math_delay, copy_specification(p), create_ground(op_number, "", sign? a : b, REAL_MAX), "(%s @ %s)");

      if(r)
        r = create_operation(dual? op_mul : op_plus, q, r, dual? "(%s * %s)" : "(%s + %s)");
      else
        r = q;

      tail -= n / 2;

      if(sign)
        a = b - tail + 1;
      else
        b = a + tail - 1;
    }
  while(tail > 0);

  stv.btl = r;

  if(btldef)
    stv.btldef = create_operation(op_and, newbtl, btldef, "%s ; %s");
  else
    stv.btldef = newbtl;

  return stv;
}

subtreeval since_until(c_base *cb, btl_specification *btl, btl_specification *btldef, btl_specification *btl_2, btl_specification *btldef_2, int level, d_time param, real realval, bool sign)
{
  subtreeval stv;
  char symbol[MAX_NAMELEN], symbol1[MAX_NAMELEN], symbol2[MAX_NAMELEN];
  btl_specification *newbtl, *p, *q, *p1, *q1;

  if(cb->seplit_su)
    {
      gensym(cb, symbol, sign? "UT" : "SN", asserted, FALSE);
      gensym(cb, symbol1, sign? "UT" : "SN", negated, FALSE);

      p = create_ground(op_name, symbol, NULL_TIME, REAL_MAX);
      p1 = create_operation(op_not, create_ground(op_name, symbol1, NULL_TIME, REAL_MAX), NULL, "(~ %s)");

      q = create_operation(op_or, btl,
                                  create_operation(op_and, btl_2,
                                                           create_operation(op_delay, copy_specification(p),
                                                                                      create_ground(op_number, "", sign? 1 : -1, REAL_MAX), "(%s @ %s)"), "(%s & %s)"), "(%s | %s)");
      q1 = create_operation(op_or, copy_specification(btl),
                                   create_operation(op_and, copy_specification(btl_2),
                                                            create_operation(op_delay, copy_specification(p1),
                                                                                       create_ground(op_number, "", sign? 1 : -1, REAL_MAX), "(%s @ %s)"), "(%s & %s)"), "(%s | %s)");

      newbtl = create_operation(op_and, CREATE_IMPLY(p, q), CREATE_IMPLY(q1, p1), "(%s & %s)");

      gensym(cb, symbol2, sign? "UT" : "SN", undefined, TRUE);

      stv.btl = create_ground(op_name, symbol2, NULL_TIME, REAL_MAX);
    }
  else
    {
      gensym(cb, symbol, sign? "UT" : "SN", asserted, TRUE);

      p = create_ground(op_name, symbol, NULL_TIME, REAL_MAX);
      q = create_operation(op_or, btl,
                           create_operation(op_and, btl_2,
                                                    create_operation(op_delay, create_ground(op_name, symbol, NULL_TIME, REAL_MAX),
                                                                               create_ground(op_number, "", sign? 1 : -1, REAL_MAX), "(%s @ %s)"), "(%s & %s)"), "(%s | %s)");
      newbtl = CREATE_EQV(p, q);

      stv.btl = create_ground(op_name, symbol, NULL_TIME, REAL_MAX);
    }

  if(btldef)
    {
      if(btldef_2)
        stv.btldef = create_operation(op_and, newbtl, create_operation(op_and, btldef, btldef_2, "%s ; %s"), "%s ; %s");
      else
        stv.btldef = create_operation(op_and, newbtl, btldef, "%s ; %s");
    }
  else
    {
      if(btldef_2)
        stv.btldef = create_operation(op_and, newbtl, btldef_2, "%s ; %s");
      else
        stv.btldef = newbtl;
    }

  return stv;
}

subtreeval com_at_happen(c_base *cb, btl_specification *btl, btl_specification *btldef, btl_specification *varextr, int arity, d_time maxtime, int level, d_time param, real realval, bool dual)
{
  subtreeval stv, stv_2, stv_3;
  char symbol[MAX_NAMELEN], symbol1[MAX_NAMELEN], symbol2[MAX_NAMELEN];
  btl_specification *p, *q, *p1, *np, *np1;
  d_time h, n;

  if(cb->seplit_fe)
    {
      gensym(cb, symbol, dual? "VHP" : "VAT", asserted, FALSE);
      gensym(cb, symbol1, dual? "VHP" : "VAT", negated, FALSE);
      gensym(cb, symbol2, dual? "VHP" : "VAT", undefined, TRUE);

      p = create_ground(op_name, symbol, NULL_TIME, REAL_MAX);
      np = create_operation(op_not, create_ground(op_name, symbol1, NULL_TIME, REAL_MAX), NULL, "(~ %s)");

      if(btldef)
        stv.btldef = create_operation(op_and, btldef, create_operation(op_and, CREATE_IMPLY(p, btl), CREATE_IMPLY(copy_specification(btl), np), "%s ; %s"), "%s ; %s");
      else
        stv.btldef = create_operation(op_and, CREATE_IMPLY(p, btl), CREATE_IMPLY(copy_specification(btl), np), "%s ; %s");

      n = 1;
      for(h = 0; h < (8 * sizeof(d_time)) && n <= abs(maxtime); h++)
        {
          if(arity > 1)
            {
              cb->iterator[level] = h / 8;
              cb->iterator[level + 1] = h % 8;                    
            }
          else
            cb->iterator[level] = h;

          gensym(cb, symbol, dual? "VHP" : "VAT", asserted, FALSE);
          gensym(cb, symbol1, dual? "VHP" : "VAT", negated, FALSE);
          gensym(cb, symbol2, dual? "VHP" : "VAT", undefined, TRUE);

          p1 = create_ground(op_name, symbol, NULL_TIME, REAL_MAX);
          np1 = create_operation(op_not, create_ground(op_name, symbol1, NULL_TIME, REAL_MAX), NULL, "(~ %s)");

          stv_2 = preval(cb, varextr, level + ((arity > 1)? 2 : 1), param, realval);

          if(stv_2.btltwo)
            q = stv_2.btltwo;
          else
            q = stv_2.btl;

          if(maxtime >= 0)
            {
              stv_2 = at_happen(cb, copy_specification(p), stv.btldef, 0, n, level + 1, param, realval, dual);
              stv_3 = at_happen(cb, copy_specification(np), stv_2.btldef, 0, n, level + 1, param, realval, dual);
            }
          else
            {
              stv_2 = at_happen(cb, copy_specification(p), stv.btldef, - n, 0, level + 1, param, realval, dual);
              stv_3 = at_happen(cb, copy_specification(np), stv_2.btldef, - n, 0, level + 1, param, realval, dual);
            }

          stv.btldef = create_operation(op_and, stv_3.btldef, create_operation(op_and,
                       CREATE_IMPLY(q, create_operation(op_and,
                                       CREATE_IMPLY(p1, stv_2.btl),
                                       CREATE_IMPLY(copy_specification(stv_3.btl), np1), "%s ; %s")),
                       CREATE_IMPLY(create_operation(op_not, copy_specification(q), NULL, "(~ %s)"), create_operation(op_and,
                                    CREATE_IMPLY(copy_specification(p1), copy_specification(p)),
                                    CREATE_IMPLY(copy_specification(np), copy_specification(np1)), "%s ; %s")), "%s ; %s"), "%s ; %s");
          p = p1;
          np = np1;
          n *= 2;
        }

      stv.btl = create_ground(op_name, symbol2, NULL_TIME, REAL_MAX);
    }
  else
    {
      gensym(cb, symbol, dual? "VHP" : "VAT", asserted, TRUE);

      p = create_ground(op_name, symbol, NULL_TIME, REAL_MAX);

      if(btldef)
        stv.btldef = create_operation(op_and, btldef, CREATE_EQV(p, btl), "%s ; %s");
      else
        stv.btldef = CREATE_EQV(p, btl);

      n = 1;
      for(h = 0; h < (8 * sizeof(d_time)) && n <= abs(maxtime); h++)
        {
          if(arity > 1)
            {
              cb->iterator[level] = h / 8;
              cb->iterator[level + 1] = h % 8;                    
            }
          else
            cb->iterator[level] = h;

          gensym(cb, symbol, dual? "VHP" : "VAT", asserted, TRUE);

          p1 = create_ground(op_name, symbol, NULL_TIME, REAL_MAX);

          stv_2 = preval(cb, varextr, level + ((arity > 1)? 2 : 1), param, realval);

          if(stv_2.btltwo)
            q = stv_2.btltwo;
          else
            q = stv_2.btl;

          if(maxtime >= 0)
            stv_2 = at_happen(cb, copy_specification(p), stv.btldef, 0, n, level + 1, param, realval, dual);
          else
            stv_2 = at_happen(cb, copy_specification(p), stv.btldef, - n, 0, level + 1, param, realval, dual);

          stv.btldef = create_operation(op_and, stv_2.btldef, create_operation(op_and,
                       CREATE_IMPLY(q, CREATE_EQV(p1, stv_2.btl)),
                       CREATE_IMPLY(create_operation(op_not, copy_specification(q), NULL, "(~ %s)"), CREATE_EQV(copy_specification(p1), copy_specification(p))), "%s ; %s"), "%s ; %s");
          p = p1;
          n *= 2;
        }

      stv.btl = copy_specification(p);
    }

  return stv;
}

subtreeval only(c_base *cb, btl_specification *btl, btl_specification *btldef, d_time a, d_time b, int level, d_time param, real realval)
{
  subtreeval stv, stv_1, stv_2;
  d_time tau;

  if(a > b)
    {
      printmsg(cb->fancymsg, 4, TIME_FMT", "TIME_FMT": Error, empty interval after <!>: %s\n", a, b, btl->debug);
      exit_failure();
    }

  if(a == b)
    stv.btl = create_operation(op_delay, btl, create_ground(op_number, "", a, REAL_MAX), "(%s @ %s)");
  else
    {
      stv = at_happen(cb, create_operation(op_not, btl, NULL, "(~ %s)"), btldef, a + 1, b, level + 1, param, realval, FALSE);       

      stv.btl = create_operation(op_and, stv.btl, create_operation(op_delay, copy_specification(btl), create_ground(op_number, "", a, REAL_MAX), "(%s @ %s)"), "(%s & %s)");

      for(tau = a + 1; tau < b; tau++)
        {
          stv_1 = at_happen(cb, create_operation(op_not, copy_specification(btl), NULL, "(~ %s)"), NULL, a, tau - 1, level + 1, param, realval, FALSE);
          stv_2 = at_happen(cb, create_operation(op_not, copy_specification(btl), NULL, "(~ %s)"), stv_1.btldef, tau + 1, b, level + 1, param, realval, FALSE);

          stv.btl = create_operation(op_or, stv.btl, create_operation(op_and, stv_1.btl, create_operation(op_and, stv_2.btl,
                    create_operation(op_delay, copy_specification(btl), create_ground(op_number, "", tau, REAL_MAX), "(%s @ %s)"), "(%s & %s)"), "(%s & %s)"), "(%s | %s)");

          if(stv.btldef)
            {
              if(stv_2.btldef)
                stv.btldef = create_operation(op_and, stv.btldef, stv_2.btldef, "%s ; %s");
            }
          else
            stv.btldef = stv_2.btldef;
        }

      stv_1 = at_happen(cb, create_operation(op_not, copy_specification(btl), NULL, "(~ %s)"), copy_specification(btldef), a, b - 1, level + 1, param, realval, FALSE);

      stv.btl = create_operation(op_or, stv.btl, create_operation(op_and, stv_1.btl,
                create_operation(op_delay, copy_specification(btl), create_ground(op_number, "", b, REAL_MAX), "(%s @ %s)"), "(%s & %s)"), "(%s | %s)");

      if(stv.btldef)
        {
          if(stv_1.btldef)
            stv.btldef = create_operation(op_and, stv.btldef, stv_1.btldef, "%s ; %s");
        }
      else
        stv.btldef = stv_1.btldef;
    }

  return stv;
}

subtreeval subset_only(c_base *cb, btl_specification *spec, int level, d_time param, real realval)
{
  subtreeval stv, stv_2, stv_3;
  btl_specification *o1, *o2, *o3, *o4, *p, *q, *r, *s, *x, *z, *h, *i, *j, *k, *l, *m, *n;
  d_time start, end, root, ratio, len, delta, offset_x, offset_y;
  char symbol[MAX_NAMELEN];

  stv_2 = preval(cb, spec->right, level, param, realval);

  start = stv_2.a;
  end = stv_2.b;

  if(end < start)
    {
      printmsg(cb->fancymsg, 4, TIME_FMT" : "TIME_FMT": Error, iteration parameter out of range in <!> construct: %s\n", stv_2.a, stv_2.b, spec->debug);
      exit_failure();
    }

  len = end - start + 1;

  root = floor(sqrt(len));

  if(root < 2)
    {
      stv = preval(cb, spec->left, level, param, realval);

      delete_specification(stv_2.btl);

      return only(cb, stv.btl, stv.btldef, start, end, level, param, realval);
    }

  ratio = ceil((real)len / root);

  offset_x = start % root;
  offset_y = start / root;

  delta = root * ratio - len;

  gensym(cb, symbol, "OY", asserted, TRUE);

  o1 = create_ground(op_name, symbol, NULL_TIME, REAL_MAX);
  o2 = create_operation(op_vector, copy_specification(o1), create_ground(op_iterator, "#", NULL_TIME, REAL_MAX), "%s(%s)");

  gensym(cb, symbol, "OY", asserted, TRUE);

  o3 = create_ground(op_name, symbol, NULL_TIME, REAL_MAX);
  o4 = create_operation(op_vector, copy_specification(o3), create_ground(op_iterator, "#", NULL_TIME, REAL_MAX), "%s(%s)");

  h = create_operation(op_interval_3, dynamic_offset(root, offset_x), dynamic_offset(root, offset_x + root), "%s : %s - 1");

  k = create_operation(op_interval_3, create_ground(op_number, "", offset_y, REAL_MAX), create_ground(op_number, "", offset_y + ratio, REAL_MAX), "%s : %s - 1");

  n = create_operation(op_interval_1, create_ground(op_number, "", start, REAL_MAX), create_ground(op_number, "", end, REAL_MAX), "%s : %s");

  if(delta > 0)
    {
      i = create_operation(op_interval_3, dynamic_offset(root, offset_x), dynamic_offset(root, offset_x + root - delta), "%s : %s - 1");
      j = create_operation(op_interval_3, dynamic_offset(root, offset_x + root - delta), dynamic_offset(root, offset_x + root), "%s : %s - 1");

      l = create_operation(op_interval_3, create_ground(op_number, "", offset_y, REAL_MAX), create_ground(op_number, "", offset_y + ratio - 1, REAL_MAX), "%s : %s - 1");
      m = create_operation(op_interval_1, create_ground(op_number, "", offset_y + ratio - 1, REAL_MAX), create_ground(op_number, "", offset_y + ratio - 1, REAL_MAX), "%s : %s");

      x = create_operation(op_forall, create_operation(op_forall,
                           CREATE_EQV(o2, o4),
                           h, "forall(%s , %s)"), l, "forall(%s , %s)");
                           
      x = create_operation(op_and, x, create_operation(op_forall, create_operation(op_forall,
                           CREATE_EQV(copy_specification(o2), copy_specification(o4)),
                           i, "forall(%s , %s)"), m, "forall(%s , %s)"), "%s & %s");
                           
      x = create_operation(op_and, x, create_operation(op_forall, create_operation(op_forall,
                           create_operation(op_not, copy_specification(o4), NULL, "(~ %s)"),
                           j, "forall(%s , %s)"), m, "forall(%s , %s)"), "%s & %s");

      x = create_operation(op_and, x, create_operation(op_forall, CREATE_EQV(create_operation(op_delay, spec->left, create_ground(op_iterator, "#", NULL_TIME, REAL_MAX), "%s @ %s"),
                                                                             copy_specification(o2)), n, "forall(%s , %s)"), "%s & %s");

      p = create_operation(op_exists, copy_specification(o4), copy_specification(h), "exists(%s , %s)");  
      r = create_operation(op_unique, copy_specification(o4), copy_specification(h), "unique(%s , %s)");
    }
  else
    {
      x = CREATE_EQV(spec->left, o3);

      p = create_operation(op_happen, copy_specification(o3), copy_specification(h), "%s ? %s");  
      r = create_operation(op_only, copy_specification(o3), copy_specification(h), "%s ! %s");
    }

  q = create_operation(op_unique, p, copy_specification(k), "unique(%s , %s)");
  s = create_operation(op_exists, r, copy_specification(k), "exists(%s , %s)");

  z = create_operation(op_and, q, s, "%s & %s");
  
  stv = preval(cb, z, level + 1, param, realval);

  do
    {
      stv_3 = preval(cb, x, level + 1, param, realval);

      if(stv.btldef)
        stv.btldef = create_operation(op_and, stv_3.btl, stv.btldef, "%s ; %s");
      else
        stv.btldef = stv_3.btl;
       
      x = stv_3.btldef;
    }
  while(x);

  delete_specification(stv_2.btl);

  return stv;
}

subtreeval eval(c_base *cb, btl_specification *spec, smallnode *vp, link_code ext_dir, bool neg, io_class sclass, io_type stype, io_type_2 packed, io_type_3b defaultval, io_type_4 omissions,
                d_time t, real realval)
{
  subtreeval stv, stv_2;
  smallnode *wp;
  io_signal *sp;
  d_time idx, k;

  stv.btl = NULL;
  stv.btltwo = NULL;
  stv.btldef = NULL;
  stv.vp = NULL;
  stv.a = NULL_TIME;
  stv.b = NULL_TIME;
  stv.c = NULL_TIME;
  stv.d = NULL_TIME;
  stv.xtra = NULL;
  stv.ytra = NULL;
  stv.ztra = NULL;
  stv.wtra = NULL;
  stv.realval = REAL_MAX;
  stv.neg = FALSE;

  if(!spec)
    return stv;

  switch(spec->ot)
    {
      case op_name:
        if(spec->symbol[0] == '_')
          {
            if(neg)
              {
                neg = FALSE;
                spec->symbol[0] = '-';
              }
            else
              spec->symbol[0] = '+';
          }

        sp = name2signal(cb, spec->symbol, spec->symbol[0] == '+' || spec->symbol[0] == '-');
        if(sp)
          {
            wp = name2smallnode(cb, spec->symbol, TRUE);

            wp->up = vp;

            wp->up_dir = ext_dir;

            wp->neg = neg;

            if(neg)
              snprintf(wp->debug, DEBUG_STRLEN, "(~ %s)", spec->debug);
            else
              strcpy(wp->debug, spec->debug);

            if(!sp->from && !sp->to)
              {
                if(!neg)
                  sp->from = wp;
                else
                  sp->to = wp;
              }

            stv.vp = wp;
          }
        else
          {
            printmsg(cb->fancymsg, 4, "Error, reference to undeclared signal: %s\n", spec->debug);
            exit_failure();
          }
      break;

      case op_dname:
        sp = name2signal(cb, spec->symbol, TRUE);
        if(!sp)
          {
            printerr(cb->fancymsg, NULL);
            exit_failure();
          }

        if(sclass != internal_class)
          {
            if(sp->sclass != internal_class)
              {
                printmsg(cb->fancymsg, 4, "Error, duplicate declaration of signal: %s\n", spec->debug);
                exit_failure();
              }

            cb->part_signals[internal_class]--;
            cb->part_signals[sclass]++;

            sp->sclass = sclass;
            sp->stype = stype;

            if(!packed)
              {
                strcpy(sp->root, sp->name);

                sp->packed = 0;
                sp->packedbit = 0;
              }
            else
              {
                idx = extract_root(sp->root, sp->name);

                if(idx < 0)
                  {
                    printmsg(cb->fancymsg, 4, "Error, packed I/O signal should be at least a vector: %s\n", spec->debug);
                    exit_failure();
                  }

                sp->packed = 1 + name2group(cb, sp->root, TRUE)->group_id;
                sp->packedbit = idx;
              }

            sp->defaultval = defaultval;
            sp->omissions = omissions;
            sp->defaultreal = realval;
          }
      break;

      case op_iname:
        add_ic(cb, spec->symbol, neg, t, REAL_MAX);
      break;

      case op_ioqual1:
        stv.xtra = &spec->value;
      break;

      case op_ioqual2:
        stv.ytra = &spec->value;
      break;

      case op_ioqual3:
        stv.ztra = &spec->value;
      break;

      case op_ioqual4:
        stv.wtra = &spec->value;
      break;

      case op_ioqual3b:
        stv.ztra = &spec->value;
        stv.realval = spec->realval;
      break;

      case op_join:
        stv = eval(cb, spec->left, vp, left_son, neg, sclass, stype, packed, defaultval, omissions, t, realval);
        stv_2 = eval(cb, spec->right, vp, right_son, neg, sclass, stype, packed, defaultval, omissions, t, realval);

        if(stv.vp && stv_2.vp)
          {
            wp = create_smallnode(cb, neg? gate : joint);
            if(!wp)
              {
                printerr(cb->fancymsg, NULL);
                exit_failure();
              }

            wp->up = vp;
            wp->left = stv.vp;
            wp->right = stv_2.vp;

            wp->up_dir = ext_dir;
            wp->left_dir = parent;
            wp->right_dir = parent;

            stv.vp->up = wp;
            stv_2.vp->up = wp;

            stv.vp->up_dir = left_son;
            stv_2.vp->up_dir = right_son;

            if(neg)
              snprintf(wp->debug, DEBUG_STRLEN, "(~ %s)", spec->debug);
            else
              strcpy(wp->debug, spec->debug);

            stv.vp = wp;
          }
        else
          if(stv_2.vp)
            stv = stv_2;
      break;

      case op_join_qual:
        stv = eval(cb, spec->left, vp, left_son, neg, sclass, stype, packed, defaultval, omissions, t, realval);
        stv_2 = eval(cb, spec->right, vp, right_son, neg, sclass, stype, packed, defaultval, omissions, t, realval);

        if(stv_2.xtra)
          {
            if(stv.xtra)
              {
                printmsg(cb->fancymsg, 4, "Error, repeated or conflicting any/ipc/file/remote qualifiers: %s\n", spec->debug);
                exit_failure();
              }

            stv.xtra = stv_2.xtra;
          }

        if(stv_2.ytra)
          {
            if(stv.ytra)
              {
                printmsg(cb->fancymsg, 4, "Error, repeated or conflicting binary/packed qualifiers: %s\n", spec->debug);
                exit_failure();
              }

            stv.ytra = stv_2.ytra;
          }

        if(stv_2.ztra)
          {
            if(stv.ztra)
              {
                printmsg(cb->fancymsg, 4, "Error, repeated or conflicting true/false/unknown/default qualifiers: %s\n", spec->debug);
                exit_failure();
              }

            stv.ztra = stv_2.ztra;
          }

        if(stv_2.wtra)
          {
            if(stv.wtra)
              {
                printmsg(cb->fancymsg, 4, "Error, repeated or conflicting raw/filter/omit qualifiers: %s\n", spec->debug);
                exit_failure();
              }

            stv.wtra = stv_2.wtra;
          }

        if(stv_2.realval != REAL_MAX)
          {
            if(stv.realval != REAL_MAX)
              {
                printmsg(cb->fancymsg, 4, "Error, repeated or conflicting default values: %s\n", spec->debug);
                exit_failure();
              }

            stv.realval = stv_2.realval;
          }
      break;

      case op_not:
        stv = eval(cb, spec->left, vp, ext_dir, !neg, sclass, stype, packed, defaultval, omissions, t, realval);
      break;

      case op_and:
        wp = create_smallnode(cb, neg? gate : joint);
        if(!wp)
          {
            printerr(cb->fancymsg, NULL);
            exit_failure();
          }

        wp->up = vp;
        wp->left = eval(cb, spec->left, wp, left_son, neg, sclass, stype, packed, defaultval, omissions, t, realval).vp;
        wp->right = eval(cb, spec->right, wp, right_son, neg, sclass, stype, packed, defaultval, omissions, t, realval).vp;

        wp->up_dir = ext_dir;
        wp->left_dir = parent;
        wp->right_dir = parent;

        if(neg)
          snprintf(wp->debug, DEBUG_STRLEN, "(~ %s)", spec->debug);
        else
          strcpy(wp->debug, spec->debug);

        stv.vp = wp;
      break;

      case op_or:
        wp = create_smallnode(cb, neg? joint : gate);
        if(!wp)
          {
            printerr(cb->fancymsg, NULL);
            exit_failure();
          }

        wp->up = vp;
        wp->left = eval(cb, spec->left, wp, left_son, neg, sclass, stype, packed, defaultval, omissions, t, realval).vp;
        wp->right = eval(cb, spec->right, wp, right_son, neg, sclass, stype, packed, defaultval, omissions, t, realval).vp;

        wp->up_dir = ext_dir;
        wp->left_dir = parent;
        wp->right_dir = parent;

        if(neg)
          snprintf(wp->debug, DEBUG_STRLEN, "(~ %s)", spec->debug);
        else
          strcpy(wp->debug, spec->debug);

        stv.vp = wp;
      break;

      case op_delay:
      case op_math_delay:
        if(cb->merge && vp && ((spec->ot == op_delay && vp->nclass == delay) || (spec->ot == op_math_delay && vp->nclass == math_delay)))
          {
            stv_2 = eval(cb, spec->left, vp, left_son, neg, sclass, stype, packed, defaultval, omissions, t, realval);
            if(stv_2.b == NULL_TIME)
              stv_2.b = 0;

            k = eval(cb, spec->right, NULL, right_son, neg, sclass, stype, packed, defaultval, omissions, t, realval).a;
            if(k == NULL_TIME)
              {
                printmsg(cb->fancymsg, 4, "Error, integer type required: %s\n", spec->debug);
                exit_failure();
              }

            stv.b = stv_2.b + k;

            stv.vp = stv_2.vp;
            stv.vp->up = vp;

            stv.vp->up_dir = ext_dir;
          }
        else
          {
            wp = create_smallnode(cb, spec->ot == op_delay? delay : math_delay);
            if(!wp)
              {
                printerr(cb->fancymsg, NULL);
                exit_failure();
              }

            stv_2 = eval(cb, spec->left, wp, left_son, neg, sclass, stype, packed, defaultval, omissions, t, realval);
            if(stv_2.b == NULL_TIME)
              stv_2.b = 0;

            k = eval(cb, spec->right, NULL, right_son, neg, sclass, stype, packed, defaultval, omissions, t, realval).a;
            if(k == NULL_TIME)
              {
                printmsg(cb->fancymsg, 4, "Error, integer type required: %s\n", spec->debug);
                exit_failure();
              }

            wp->up = vp;
            wp->left = stv_2.vp;

            wp->up_dir = ext_dir;
            wp->left_dir = parent;

            wp->k = - (stv_2.b + k);

            if(cb->merge && !wp->k)
              {
                purge_smallnode(cb, wp, &wp->right, NULL, undefined, FALSE);

                wp->up = NULL;
                wp->left = NULL;

                stv.vp = stv_2.vp;
                stv.vp->up = vp;

                stv.vp->up_dir = ext_dir;
              }
            else
              {
                strcpy(wp->debug, spec->debug);

                stv.vp = wp;
              }
          }
      break;

      case op_number:
        if(vp)
          {
            wp = create_smallnode(cb, literal);
            if(!wp)
              {
                printerr(cb->fancymsg, NULL);
                exit_failure();
              }

            wp->up = vp;

            wp->up_dir = ext_dir;

            wp->k = spec->value;
            wp->neg = neg;

            strcpy(wp->debug, spec->debug);

            stv.vp = wp;
          }
      
        stv.a = spec->value;
      break;

      case op_real:
        if(vp)
          {
            wp = create_smallnode(cb, literal);
            if(!wp)
              {
                printerr(cb->fancymsg, NULL);
                exit_failure();
              }

            wp->up = vp;

            wp->up_dir = ext_dir;

            wp->k = NULL_TIME;
            wp->realval = spec->realval;
            wp->neg = neg;

            strcpy(wp->debug, spec->debug);

            stv.vp = wp;
          }
              
        stv.a = NULL_TIME;
        stv.realval = spec->realval;
      break;

      case op_variable:
        sp = name2signal(cb, spec->symbol, FALSE);
        if(!sp && spec->symbol[0] == '*')
          {
            sp = name2signal(cb, spec->symbol, TRUE);
            if(!sp)
              {
                printerr(cb->fancymsg, NULL);
                exit_failure();
              }

            sp->packed = -1;
            sp->packedbit = 0;
            sp->shared = TRUE;
          }

        if(sp)
          {
            wp = create_smallnode(cb, literal);
            if(!wp)
              {
                printerr(cb->fancymsg, NULL);
                exit_failure();
              }

            wp->up = vp;

            wp->up_dir = ext_dir;

            strcpy(wp->root, spec->symbol);
            strcpy(wp->debug, spec->debug);

            if(sp->shared && (sp->from || sp->to))
              {
                sp = name2signal(cb, spec->symbol, TRUE);

                sp->sclass = hidden_class;
                sp->packed = -1;
                sp->packedbit = 0;
                sp->shared = TRUE;
              }

            if(!sp->from && !sp->to)
              {
                sp->from = vp;
                sp->to = wp;

                sp->tofrom = (ext_dir == left_son)? &vp->left : &vp->right;
                sp->fromto = &wp->up;
              }

            stv.vp = wp;
          }
        else
          {
            printmsg(cb->fancymsg, 4, "Error, reference to undeclared signal: %s\n", spec->debug);
            exit_failure();
          }
      break;

      case op_dvariable:
        sp = name2signal(cb, spec->symbol, TRUE);
        if(!sp)
          {
            printerr(cb->fancymsg, NULL);
            exit_failure();
          }

        if(sp->sclass != internal_class)
          {
            printmsg(cb->fancymsg, 4, "Error, duplicate declaration of signal: %s\n", spec->debug);
            exit_failure();
          }

        cb->part_signals[internal_class]--;
        cb->part_signals[sclass]++;

        sp->sclass = sclass;
        sp->stype = stype;

        strcpy(sp->root, sp->name);

        sp->packed = -1;
        sp->packedbit = 0;

        sp->defaultval = defaultval;
        sp->omissions = omissions;
        sp->defaultreal = realval;

        sp->shared = TRUE;
      break;

      case op_ivariable:
        add_ic(cb, spec->symbol, neg, t, realval);
      break;

      case op_plus:
        wp = create_smallnode(cb, math_add);
        if(!wp)
          {
            printerr(cb->fancymsg, NULL);
            exit_failure();
          }

        wp->up = vp;
        wp->left = eval(cb, spec->left, wp, left_son, neg, sclass, stype, packed, defaultval, omissions, t, realval).vp;
        wp->right = eval(cb, spec->right, wp, right_son, neg, sclass, stype, packed, defaultval, omissions, t, realval).vp;

        wp->up_dir = ext_dir;
        wp->left_dir = parent;
        wp->right_dir = parent;

        strcpy(wp->debug, spec->debug);

        stv.vp = wp;
      break;

      case op_mul:
        wp = create_smallnode(cb, math_mul);
        if(!wp)
          {
            printerr(cb->fancymsg, NULL);
            exit_failure();
          }

        wp->up = vp;
        wp->left = eval(cb, spec->left, wp, left_son, neg, sclass, stype, packed, defaultval, omissions, t, realval).vp;
        wp->right = eval(cb, spec->right, wp, right_son, neg, sclass, stype, packed, defaultval, omissions, t, realval).vp;

        wp->up_dir = ext_dir;
        wp->left_dir = parent;
        wp->right_dir = parent;

        strcpy(wp->debug, spec->debug);

        stv.vp = wp;
      break;

      case op_pow:
        wp = create_smallnode(cb, math_pow);
        if(!wp)
          {
            printerr(cb->fancymsg, NULL);
            exit_failure();
          }

        wp->up = vp;
        wp->left = eval(cb, spec->left, wp, left_son, neg, sclass, stype, packed, defaultval, omissions, t, realval).vp;
        wp->right = eval(cb, spec->right, wp, right_son, neg, sclass, stype, packed, defaultval, omissions, t, realval).vp;

        wp->up_dir = ext_dir;
        wp->left_dir = parent;
        wp->right_dir = parent;

        strcpy(wp->debug, spec->debug);

        stv.vp = wp;
      break;

      case op_root:
        wp = create_smallnode(cb, math_pow);
        if(!wp)
          {
            printerr(cb->fancymsg, NULL);
            exit_failure();
          }

        wp->up = vp;
        wp->left = eval(cb, spec->left, wp, left_son, neg, sclass, stype, packed, defaultval, omissions, t, realval).vp;
        wp->right = eval(cb, spec->right, wp, right_son, neg, sclass, stype, packed, defaultval, omissions, t, realval).vp;

        wp->up_dir = ext_dir;
        wp->left_dir = parent;
        wp->right_dir = parent;

        strcpy(wp->debug, spec->debug);

        wp->turn = 1;

        stv.vp = wp;
      break;

      case op_log:
        wp = create_smallnode(cb, math_pow);
        if(!wp)
          {
            printerr(cb->fancymsg, NULL);
            exit_failure();
          }

        wp->up = vp;
        wp->left = eval(cb, spec->left, wp, left_son, neg, sclass, stype, packed, defaultval, omissions, t, realval).vp;
        wp->right = eval(cb, spec->right, wp, right_son, neg, sclass, stype, packed, defaultval, omissions, t, realval).vp;

        wp->up_dir = ext_dir;
        wp->left_dir = parent;
        wp->right_dir = parent;

        strcpy(wp->debug, spec->debug);

        wp->turn = -1;

        stv.vp = wp;
      break;

      case op_chs:
        wp = create_smallnode(cb, math_chs);
        if(!wp)
          {
            printerr(cb->fancymsg, NULL);
            exit_failure();
          }

        wp->up = vp;
        wp->left = eval(cb, spec->left, wp, left_son, neg, sclass, stype, packed, defaultval, omissions, t, realval).vp;

        wp->up_dir = ext_dir;
        wp->left_dir = parent;

        strcpy(wp->debug, spec->debug);

        stv.vp = wp;
      break;

      case op_inv:
        wp = create_smallnode(cb, math_inv);
        if(!wp)
          {
            printerr(cb->fancymsg, NULL);
            exit_failure();
          }

        wp->up = vp;
        wp->left = eval(cb, spec->left, wp, left_son, neg, sclass, stype, packed, defaultval, omissions, t, realval).vp;

        wp->up_dir = ext_dir;
        wp->left_dir = parent;

        strcpy(wp->debug, spec->debug);

        stv.vp = wp;
      break;

      case op_sin:
        wp = create_smallnode(cb, math_sin);
        if(!wp)
          {
            printerr(cb->fancymsg, NULL);
            exit_failure();
          }

        wp->up = vp;
        wp->left = eval(cb, spec->left, wp, left_son, neg, sclass, stype, packed, defaultval, omissions, t, realval).vp;

        wp->up_dir = ext_dir;
        wp->left_dir = parent;

        strcpy(wp->debug, spec->debug);

        stv.vp = wp;
      break;

      case op_asin:
        wp = create_smallnode(cb, math_sin);
        if(!wp)
          {
            printerr(cb->fancymsg, NULL);
            exit_failure();
          }

        wp->up = vp;
        wp->left = eval(cb, spec->left, wp, left_son, neg, sclass, stype, packed, defaultval, omissions, t, realval).vp;

        wp->up_dir = ext_dir;
        wp->left_dir = parent;

        strcpy(wp->debug, spec->debug);

        wp->turn = 1;

        stv.vp = wp;
      break;

      case op_math_eqv0:
        wp = create_smallnode(cb, neg? math_neq0 : math_eqv0);
        if(!wp)
          {
            printerr(cb->fancymsg, NULL);
            exit_failure();
          }

        wp->up = vp;
        wp->left = eval(cb, spec->left, wp, left_son, neg, sclass, stype, packed, defaultval, omissions, t, realval).vp;

        wp->up_dir = ext_dir;
        wp->left_dir = parent;

        if(neg)
          snprintf(wp->debug, DEBUG_STRLEN, "(~ %s)", spec->debug);
        else
          strcpy(wp->debug, spec->debug);

        stv.vp = wp;
      break;

      case op_math_neq0:
        wp = create_smallnode(cb, neg? math_eqv0 : math_neq0);
        if(!wp)
          {
            printerr(cb->fancymsg, NULL);
            exit_failure();
          }

        wp->up = vp;
        wp->left = eval(cb, spec->left, wp, left_son, neg, sclass, stype, packed, defaultval, omissions, t, realval).vp;

        wp->up_dir = ext_dir;
        wp->left_dir = parent;

        if(neg)
          snprintf(wp->debug, DEBUG_STRLEN, "(~ %s)", spec->debug);
        else
          strcpy(wp->debug, spec->debug);

        stv.vp = wp;
      break;

      case op_math_gteq0:
        wp = create_smallnode(cb, neg? math_lt0 : math_gteq0);
        if(!wp)
          {
            printerr(cb->fancymsg, NULL);
            exit_failure();
          }

        wp->up = vp;
        wp->left = eval(cb, spec->left, wp, left_son, neg, sclass, stype, packed, defaultval, omissions, t, realval).vp;

        wp->up_dir = ext_dir;
        wp->left_dir = parent;

        if(neg)
          snprintf(wp->debug, DEBUG_STRLEN, "(~ %s)", spec->debug);
        else
          strcpy(wp->debug, spec->debug);

        stv.vp = wp;
      break;

      case op_math_lt0:
        wp = create_smallnode(cb, neg? math_gteq0 : math_lt0);
        if(!wp)
          {
            printerr(cb->fancymsg, NULL);
            exit_failure();
          }

        wp->up = vp;
        wp->left = eval(cb, spec->left, wp, left_son, neg, sclass, stype, packed, defaultval, omissions, t, realval).vp;

        wp->up_dir = ext_dir;
        wp->left_dir = parent;

        if(neg)
          snprintf(wp->debug, DEBUG_STRLEN, "(~ %s)", spec->debug);
        else
          strcpy(wp->debug, spec->debug);

        stv.vp = wp;
      break;

      case op_var_at:
        stv_2 = eval(cb, spec->right, NULL, right_son, neg, sclass, stype, packed, defaultval, omissions, t, realval);
        stv = eval(cb, spec->left, vp, left_son, neg, sclass, stype, packed, defaultval, omissions, stv_2.a, realval);
      break;

      case op_assign:
        stv_2 = eval(cb, spec->right, NULL, right_son, neg, sclass, stype, packed, defaultval, omissions, t, realval);

        if(stv_2.a != NULL_TIME)
          stv_2.realval = stv_2.a;

        stv = eval(cb, spec->left, vp, left_son, neg, sclass, stype, packed, defaultval, omissions, t, stv_2.realval);
      break;

      case op_internal:
        stv = eval(cb, spec->left, vp, left_son, neg, internal_class, stype, packed, defaultval, omissions, t, realval);
      break;

      case op_aux:
        stv = eval(cb, spec->left, vp, left_son, neg, aux_class, stype, packed, defaultval, omissions, t, realval);
      break;

      case op_input:
        stv_2 = eval(cb, spec->right, vp, right_son, neg, sclass, stype, packed, defaultval, omissions, t, realval);
        stv = eval(cb, spec->left, vp, left_son, neg, input_class, stv_2.xtra? *stv_2.xtra : io_any, stv_2.ytra? *stv_2.ytra : io_binary,
                   stv_2.ztra? *stv_2.ztra : io_unknown, stv_2.wtra? *stv_2.wtra : io_raw, t, stv_2.realval);
      break;

      case op_output:
        stv_2 = eval(cb, spec->right, vp, right_son, neg, sclass, stype, packed, defaultval, omissions, t, realval);
        stv = eval(cb, spec->left, vp, left_son, neg, output_class, stv_2.xtra? *stv_2.xtra : io_any, stv_2.ytra? *stv_2.ytra : io_binary,
                   stv_2.ztra? *stv_2.ztra : io_unknown, stv_2.wtra? *stv_2.wtra : io_raw, t, stv_2.realval);
      break;

      case op_init:
        stv = eval(cb, spec->left, vp, left_son, neg, sclass, stype, packed, defaultval, omissions, t, realval);
      break;

      default:
        printmsg(cb->fancymsg, 5, "Internal error, unmanaged operator in phase 2 (%d)\n", spec->ot);
        assert(FALSE);
      break;
    }

  return stv;
}

smallnode **gendir(smallnode *vp, smallnode **bpp, link_code dir)
{
  smallnode **zpp;

  assert(vp);

  zpp = NULL;

  switch(dir)
    {
      case parent:
        zpp = bpp;
      break;

      case left_son:
        if(bpp == &vp->up)
          zpp = &vp->right;
        else
          if(bpp == &vp->left)
             zpp = &vp->up;
          else
            if(bpp == &vp->right)
              zpp = &vp->left;
            else
              assert(FALSE);
      break;

      case right_son:
        if(bpp == &vp->up)
          zpp = &vp->left;
        else
          if(bpp == &vp->left)
            zpp = &vp->right;
          else
            if(bpp == &vp->right)
              zpp = &vp->up;
            else
              assert(FALSE);
      break;

      default:
        assert(FALSE);
      break;
    }

  if(!zpp)
    {
      printmsg(0, 0, "%s: Internal error in pruning network (E1): %s\n", vp->name, vp->debug);
      exit_failure();
    }

  return zpp;
}

link_code *dirdir(smallnode *vp, smallnode **bpp, link_code dir)
{
  link_code *lcp;

  assert(vp);

  lcp = NULL;

  switch(dir)
    {
      case parent:
        if(bpp == &vp->up)
          lcp = &vp->up_dir;
        else
          if(bpp == &vp->left)
             lcp = &vp->left_dir;
          else
            if(bpp == &vp->right)
              lcp = &vp->right_dir;
            else
              assert(FALSE);
      break;

      case left_son:
        if(bpp == &vp->up)
          lcp = &vp->right_dir;
        else
          if(bpp == &vp->left)
             lcp = &vp->up_dir;
          else
            if(bpp == &vp->right)
              lcp = &vp->left_dir;
            else
              assert(FALSE);
      break;

      case right_son:
        if(bpp == &vp->up)
          lcp = &vp->left_dir;
        else
          if(bpp == &vp->left)
            lcp = &vp->right_dir;
          else
            if(bpp == &vp->right)
              lcp = &vp->up_dir;
            else
              assert(FALSE);
      break;

      default:
        assert(FALSE);
      break;
    }

  if(!lcp)
    {
      printmsg(0, 0, "%s: Internal error in pruning network (E2): %s\n", vp->name, vp->debug);
      exit_failure();
    }

  return lcp;
}

smallnode **get_neighbor_handle(smallnode *vp, smallnode **bpp, link_code dir)
{
  smallnode *wp;
  smallnode **zpp;
  link_code lc;

  assert(vp);

  zpp = NULL;

  wp = *gendir(vp, bpp, dir);
  lc = *dirdir(vp, bpp, dir);

  if(wp && wp != SPECIAL)
    {
      switch(lc)
        {
          case parent:
            zpp = &wp->up;
          break;

          case left_son:
            zpp = &wp->left;
          break;

          case right_son:
            zpp = &wp->right;
          break;

          default:
            return NULL;
          break;
        }
    }
  else
    return NULL;

  if(!zpp || (*zpp != NULL && *zpp != SPECIAL && *zpp != vp))
    {
      printmsg(0, 0, "%s: Internal error in pruning network (E3): %s\n", vp->name, vp->debug);
      exit_failure();
    }

  return zpp;
}

link_code *get_neighbor_dir(smallnode *vp, smallnode **bpp, link_code dir)
{
  smallnode *wp;
  link_code *zlcp;
  link_code lc;

  assert(vp);

  zlcp = NULL;

  wp = *gendir(vp, bpp, dir);
  lc = *dirdir(vp, bpp, dir);

  if(wp && wp != SPECIAL)
    {
      switch(lc)
        {
          case parent:
            zlcp = &wp->up_dir;
          break;

          case left_son:
            zlcp = &wp->left_dir;
          break;

          case right_son:
            zlcp = &wp->right_dir;
          break;

          default:
            return NULL;
          break;
        }
    }
  else
    return NULL;

  if(!zlcp)
    {
      printmsg(0, 0, "%s: Internal error in pruning network (E4): %s\n", vp->name, vp->debug);
      exit_failure();
    }

  return zlcp;
}

void validate(c_base *cb)
{
  smallnode *vp;

  vp = cb->network;

  while(vp)
    {
      if(vp->nclass != literal)
        {
          get_neighbor_handle(vp, &vp->up, left_son);
          get_neighbor_handle(vp, &vp->left, left_son);
          get_neighbor_handle(vp, &vp->right, left_son);
        }

      vp = vp->vp;
    }
}

void turn_math_ops(c_base *cb)
{
  smallnode *vp, *pp, *lp, *rp;
  link_code plc, llc, rlc, slc;
  link_code *lcp1, *lcp2;

  vp = cb->network;

  while(vp)
    {
      if(vp->nclass != literal && vp->turn)
        {
          pp = vp->up;

          plc = vp->up_dir;

          lcp1 = NULL;
          lcp2 = NULL;

          if(pp->up == vp)
            lcp1 = &pp->up_dir;
          else
            if(pp->left == vp)
              lcp1 = &pp->left_dir;
            else
              if(pp->right == vp)
                lcp1 = &pp->right_dir;

          if(vp->turn > 0)
            {
              lp = vp->left;
              llc = vp->left_dir;

              vp->up = lp;
              vp->left = pp;

              vp->up_dir = llc;
              vp->left_dir = plc;

              if(lp->up == vp)
                 lcp2 = &lp->up_dir;
              else
                if(lp->left == vp)
                  lcp2 = &lp->left_dir;
                else
                  if(lp->right == vp)
                    lcp2 = &lp->right_dir;

              if(!lcp1 || !lcp2)
                {
                  printmsg(0, 0, "%s: Internal error in pruning network (E5): %s\n", vp->name, vp->debug);
                  exit_failure();
                }

              slc = *lcp1;
              *lcp1 = *lcp2;
              *lcp2 = slc;
            }
          else
            if(vp->turn < 0)
              {
                rp = vp->right;
                rlc = vp->right_dir;

                vp->up = rp;
                vp->right = pp;

                vp->up_dir = rlc;
                vp->right_dir = plc;

                if(rp->up == vp)
                   lcp2 = &rp->up_dir;
                else
                  if(rp->left == vp)
                    lcp2 = &rp->left_dir;
                  else
                    if(rp->right == vp)
                      lcp2 = &rp->right_dir;

                if(!lcp1 || !lcp2)
                  {
                    printmsg(0, 0, "%s: Internal error in pruning network (E5): %s\n", vp->name, vp->debug);
                    exit_failure();
                  }

                slc = *lcp1;
                *lcp1 = *lcp2;
                *lcp2 = slc;
              }
        }

      vp = vp->vp;
    }
}

link_code occurrence(smallnode **fromto, smallnode *to)
{
  if(!to || to == SPECIAL)
    return no_link;
  else
    {
      if(fromto == &to->up)
        return parent;
      else
        if(fromto == &to->left)
          return left_son;
        else
          if(fromto == &to->right)
            return right_son;
          else
            return no_link;
    }
}

bool assign_signal(c_base *cb, io_signal *sp, smallnode *vp, smallnode **bpp, litval val)
{
  smallnode **lvpp, **rvpp;
  io_type_3b def;
  bool neg, rv;

  assert(vp);

  rv = FALSE;
      
  if(val != undefined)
    {
      def = io_unknown;
      neg = (val == negated);

      if(sp->to == vp)
        {
          if(sp->fromto == bpp)
            neg = !neg;

          def = neg? io_true : io_false;
        }
      else
        if(sp->from == vp)
          {
            if(sp->tofrom == bpp)
              neg = !neg;

            def = neg? io_false : io_true;
          }

      if(sp->defaultval != io_unknown && sp->defaultval != def)
        {
          sp->omissions = io_raw;

          printmsg(cb->fancymsg, 2, "%s: Warning, incompatible default on signal\n", sp->name);
        }

      sp->defaultval = def;
      sp->val = val;
    }
  else
    {
      lvpp = get_neighbor_handle(vp, bpp, left_son);
      rvpp = get_neighbor_handle(vp, bpp, right_son);

      if(sp->from == vp)
        {
          if(sp->fromto == lvpp)
            {
              sp->from = *gendir(vp, bpp, right_son);
              sp->tofrom = rvpp;

              rv = TRUE;
            }
          else
            if(sp->fromto == rvpp)
              {
                sp->from = *gendir(vp, bpp, left_son);
                sp->tofrom = lvpp;
                
                rv = TRUE;
              }
        }

      if(sp->to == vp)
        {
          if(sp->tofrom == lvpp)
            {
              sp->to = *gendir(vp, bpp, right_son);
              sp->fromto = rvpp;
              
              rv = TRUE;
            }
          else
            if(sp->tofrom == rvpp)
              {
                sp->to = *gendir(vp, bpp, left_son);
                sp->fromto = lvpp;
                
                rv = TRUE;
              }
        }
    }

  return rv;
}

void purge_smallnode(c_base *cb, smallnode *vp, smallnode **bpp, smallnode **dpp, litval val, bool flip)
{
  io_signal *sp;
  smallnode *wp;
  smallnode **wpp;
  bool rv1, rv2;

  assert(vp);
  assert(!vp->zombie);

  sp = cb->sigtab;
  while(sp)
    {
      if(sp->from == vp || sp->to == vp)
        {
          rv1 = assign_signal(cb, sp, vp, bpp, val);
          rv2 = FALSE;

          if(dpp && !rv1)
            {
              rv2 = assign_signal(cb, sp, vp, dpp, val);
              
              if(rv2 && flip)
                {
                  wp = sp->from;
                  sp->from = sp->to;
                  sp->to = wp;

                  wpp = sp->fromto;
                  sp->fromto = sp->tofrom;
                  sp->tofrom = wpp;
                }
            }

          if(!sp->from || sp->from == SPECIAL || sp->from == vp || !sp->to || sp->to == SPECIAL || sp->to == vp)
            {
              sp->from = NULL;
              sp->to = NULL;

              sp->removed = TRUE;

              if(sp->sclass != internal_class)
                printmsg(cb->fancymsg, sp->sclass == input_class || sp->sclass == output_class? 2 : 1, "%s: Warning, %s signal removed\n", sp->name, signal_class[sp->sclass]);
            }
          else
            if(sp->sclass == input_class && dpp && (rv1 || rv2))
              printmsg(cb->fancymsg, 3, "%s: Warning, input signal displacement may conflict with other inputs\n", sp->name);
        }

      sp = sp->nextsig;
    }

  vp->zombie = TRUE;
}

void add_erase_vector(c_base *cb, smallnode *vp, smallnode **bpp, smallnode *bp)
{
  if(!vp)
    return;

  if(vp == SPECIAL)
    {
      printmsg(cb->fancymsg, 3, "%s: Warning, logical contradiction or incompleteness between clauses (B1): %s\n",
               bp && bp != SPECIAL? bp->name : "<blank>", bp && bp != SPECIAL? bp->debug : "<blank>");
      return;
    }

  cb->erasearrow[cb->end_erasearrows].vp = vp;
  cb->erasearrow[cb->end_erasearrows].bp = *bpp;
  cb->erasearrow[cb->end_erasearrows].bpp = bpp;

  cb->end_erasearrows = (cb->end_erasearrows + 1) % NUM_VECTORS;

  if(cb->start_erasearrows == cb->end_erasearrows)
    {
      printmsg(cb->fancymsg, 4, "Error, network too large for pruning\n");
      exit_failure();
    }
}

void erase_vectors(c_base *cb)
{
  smallnode *vp;

  while(cb->start_erasearrows != cb->end_erasearrows)
    {
      vp = cb->erasearrow[cb->start_erasearrows].vp;

      erase_smalltree(cb, vp, cb->erasearrow[cb->start_erasearrows].bpp);

      cb->start_erasearrows = (cb->start_erasearrows + 1) % NUM_VECTORS;
    }
}

void add_purge_vector(c_base *cb, smallnode *vp, smallnode **bpp, smallnode *bp)
{
  if(vp == SPECIAL)
    return;

  if(!vp)
    {
      printmsg(cb->fancymsg, 4, "%s: Error, logical contradiction between clauses (A1): %s\n",
               bp && bp != SPECIAL? bp->name : "<blank>", bp && bp != SPECIAL? bp->debug : "<blank>");
      exit_failure();
    }

  cb->purgearrow[cb->end_purgearrows].vp = vp;
  cb->purgearrow[cb->end_purgearrows].bp = *bpp;
  cb->purgearrow[cb->end_purgearrows].bpp = bpp;

  cb->end_purgearrows = (cb->end_purgearrows + 1) % NUM_VECTORS;

  if(cb->start_purgearrows == cb->end_purgearrows)
    {
      printmsg(cb->fancymsg, 4, "Error, network too large for pruning\n");
      exit_failure();
    }
}

void purge_vectors(c_base *cb)
{
  smallnode *vp;

  while(cb->start_purgearrows != cb->end_purgearrows)
    {
      vp = cb->purgearrow[cb->start_purgearrows].vp;

      purge_smalltree(cb, vp, cb->purgearrow[cb->start_purgearrows].bpp);

      cb->start_purgearrows = (cb->start_purgearrows + 1) % NUM_VECTORS;
    }
}

void link_directions(smallnode *vp, smallnode **bpp)
{
  link_code *xlcp, *ylcp;

  xlcp = get_neighbor_dir(vp, bpp, left_son);
  ylcp = get_neighbor_dir(vp, bpp, right_son);

  if(xlcp)
    *xlcp = *dirdir(vp, bpp, right_son);

  if(ylcp)
    *ylcp = *dirdir(vp, bpp, left_son);
}

bool clean_smallbranches(c_base *cb, smallnode *xp, smallnode *yp, smallnode **xbpp, smallnode **ybpp)
{
  if(!xp)
    {
      if(yp && yp != SPECIAL)
        {
          *ybpp = NULL;

          add_purge_vector(cb, yp, ybpp, xp);
        }

      return FALSE;
    }

  if(xp == SPECIAL)
    {
      if(yp && yp != SPECIAL)
        {
          *ybpp = SPECIAL;

          add_erase_vector(cb, yp, ybpp, xp);
        }

      return FALSE;
    }

  if(!yp)
    {
      if(xp && xp != SPECIAL)
        {
          *xbpp = NULL;

          add_purge_vector(cb, xp, xbpp, yp);
        }

      return FALSE;
    }

  if(yp == SPECIAL)
    {
      if(xp && xp != SPECIAL)
        {
          *xbpp = SPECIAL;

          add_erase_vector(cb, xp, xbpp, yp);
        }

      return FALSE;
    }

  return TRUE;
}

void close_smallbranches(c_base *cb, smallnode *xp, smallnode *yp, smallnode **xbpp, smallnode **ybpp, smallnode *vp)
{
  int i;

  if(*xbpp && *xbpp != SPECIAL)
    *xbpp = yp;

  if(*ybpp && *ybpp != SPECIAL)
    *ybpp = xp;

  for(i = cb->start_purgearrows; i != cb->end_purgearrows; i = (i + 1) % NUM_VECTORS)
    {
      if(cb->purgearrow[i].vp == vp)
        {
          if(cb->purgearrow[i].bp == xp)
            {
              cb->purgearrow[i].vp = yp;
              cb->purgearrow[i].bp = *ybpp;
              cb->purgearrow[i].bpp = ybpp;
            }
          else
            if(cb->purgearrow[i].bp == yp)
              {
                cb->purgearrow[i].vp = xp;
                cb->purgearrow[i].bp = *xbpp;
                cb->purgearrow[i].bpp = xbpp;
              }
        }
    }

  for(i = cb->start_erasearrows; i != cb->end_erasearrows; i = (i + 1) % NUM_VECTORS)
    {
      if(cb->erasearrow[i].vp == vp)
        {
          if(cb->erasearrow[i].bp == xp)
            {
              cb->erasearrow[i].vp = yp;
              cb->erasearrow[i].bp = *ybpp;
              cb->erasearrow[i].bpp = ybpp;
            }
          else
            if(cb->erasearrow[i].bp == yp)
              {
                cb->erasearrow[i].vp = xp;
                cb->erasearrow[i].bp = *xbpp;
                cb->erasearrow[i].bpp = xbpp;
              }
        }
    }
}

void zero_on_smalltree(c_base *cb, smallnode *vp, smallnode **bpp)
{
  smallnode **lpp, **rpp, **lvpp, **rvpp, **dpp;
  smallnode *lp, *rp, *wp;

  switch(vp->nclass)
    {
      case math_add:
        if(!vp->zombie)
          {
            wp = *gendir(vp, bpp, left_son);
            dpp = &wp->right;

            if(wp->nclass == math_chs)
              {
                lpp = gendir(wp, dpp, left_son);
                rpp = gendir(wp, dpp, right_son);

                lp = *lpp;
                rp = *rpp;

                if(lp && lp != SPECIAL)
                  lvpp = get_neighbor_handle(wp, dpp, left_son);
                else
                 {
                   if(lp)
                     lvpp = &cb->specnode;
                   else
                     lvpp = &cb->nullnode;
                 }

                if(rp && rp != SPECIAL)
                  rvpp = get_neighbor_handle(wp, dpp, right_son);
                else
                  {
                    if(rp)
                      rvpp = &cb->specnode;
                    else
                      rvpp = &cb->nullnode;
                  }

                purge_smallnode(cb, wp, dpp, NULL, undefined, FALSE);

                if(lp && lp != SPECIAL && rp && rp != SPECIAL)
                  {
                    link_directions(wp, dpp);
                    close_smallbranches(cb, lp, rp, lvpp, rvpp, wp);
                  }
                else
                  clean_smallbranches(cb, lp, rp, lvpp, rvpp);

                *dpp = NULL;
                *lpp = SPECIAL;
                *rpp = SPECIAL;
              }
            else
             if(wp->nclass == literal)
               {
                 printmsg(cb->fancymsg, 4, "%s: Error, real signal has a constant value and must be replaced by defining a parameter: %s\n", vp->name, vp->debug);
                 exit_failure();
               } 
             else
              {
                printmsg(cb->fancymsg, 4, "%s: Error, ill-formed identity: %s\n", vp->name, vp->debug);
                exit_failure();
              }

            lpp = gendir(vp, bpp, left_son);
            rpp = gendir(vp, bpp, right_son);

            lp = *lpp;
            rp = *rpp;

            if(lp && lp != SPECIAL)
              lvpp = get_neighbor_handle(vp, bpp, left_son);
            else
              {
                if(lp)
                  lvpp = &cb->specnode;
                else
                  lvpp = &cb->nullnode;
              }

            if(rp && rp != SPECIAL)
              rvpp = get_neighbor_handle(vp, bpp, right_son);
            else
              {
                if(rp)
                  rvpp = &cb->specnode;
                else
                  rvpp = &cb->nullnode;
              }

            purge_smallnode(cb, vp, bpp, NULL, undefined, FALSE);

            if(lp && lp != SPECIAL && rp && rp != SPECIAL)
              {
                link_directions(vp, bpp);
                close_smallbranches(cb, lp, rp, lvpp, rvpp, vp);
              }
            else
              clean_smallbranches(cb, lp, rp, lvpp, rvpp);

            *bpp = NULL;
            *lpp = SPECIAL;
            *rpp = SPECIAL;
          }
      break;

      case math_delay:
      case math_chs:
      case math_sin:
        lpp = gendir(vp, bpp, left_son);

        lp = *lpp;

        if(lp && lp != SPECIAL)
          lvpp = get_neighbor_handle(vp, bpp, left_son);
        else
          {
            if(lp)
              lvpp = &cb->specnode;
            else
              lvpp = &cb->nullnode;
          }

        if(!lp)
          {
            lpp = gendir(vp, bpp, right_son);

            lp = *lpp;

            if(lp && lp != SPECIAL)
              lvpp = get_neighbor_handle(vp, bpp, right_son);
            else
              {
                if(lp)
                  lvpp = &cb->specnode;
                else
                  lvpp = &cb->nullnode;
              }
          }

        zero_on_smalltree(cb, lp, lvpp);

        if(!vp->zombie)
          {
            purge_smallnode(cb, vp, bpp, NULL, asserted, FALSE);

            *bpp = NULL;
            *lpp = SPECIAL;
          }
      break;

      case math_mul:
      case math_inv:
      case math_pow:
        printmsg(cb->fancymsg, 4, "%s: Error, cannot propagate a result equal to zero through products, powers or inverse: %s\n", vp->name, vp->debug);
        exit_failure();
      break;

      case literal:
        printmsg(cb->fancymsg, 4, "%s: Error, numeric variable has a constant value equal to zero: %s\n", vp->name, vp->debug);
        exit_failure();
      break;

      default:
      break;
    }
}

void erase_smalltree(c_base *cb, smallnode *vp, smallnode **bpp)
{
  smallnode **lpp, **rpp, **lvpp, **rvpp;
  smallnode *lp, *rp;
  bool neg;

  switch(vp->nclass)
    {
      case gate:
        lpp = gendir(vp, bpp, left_son);
        rpp = gendir(vp, bpp, right_son);

        lp = *lpp;
        rp = *rpp;

        if(lp && lp != SPECIAL)
          lvpp = get_neighbor_handle(vp, bpp, left_son);
        else
          {
            if(lp)
              lvpp = &cb->specnode;
            else
              lvpp = &cb->nullnode;
          }

        if(rp && rp != SPECIAL)
          rvpp = get_neighbor_handle(vp, bpp, right_son);
        else
          {
            if(rp)
              rvpp = &cb->specnode;
            else
              rvpp = &cb->nullnode;
          }

        if(lp == vp && rp == vp)
          printmsg(cb->fancymsg, 3, "%s: Warning, logical contradiction or incompleteness between clauses (B2): %s\n", vp->name, vp->debug);

        add_erase_vector(cb, lp, lvpp, vp);
        add_erase_vector(cb, rp, rvpp, vp);

        if(!vp->zombie)
          {
            purge_smallnode(cb, vp, bpp, NULL, negated, FALSE);

            *bpp = SPECIAL;
            *lpp = NULL;
            *rpp = NULL;
          }
      break;

      case joint:
        lpp = gendir(vp, bpp, left_son);
        rpp = gendir(vp, bpp, right_son);

        lp = *lpp;
        rp = *rpp;

        if(lp && lp != SPECIAL)
          lvpp = get_neighbor_handle(vp, bpp, left_son);
        else
          {
            if(lp)
              lvpp = &cb->specnode;
            else
              lvpp = &cb->nullnode;
          }

        if(rp && rp != SPECIAL)
          rvpp = get_neighbor_handle(vp, bpp, right_son);
        else
          {
            if(rp)
              rvpp = &cb->specnode;
            else
              rvpp = &cb->nullnode;
          }

        if(bpp == &vp->up)
          {
            if(lp == vp && rp == vp)
              printmsg(cb->fancymsg, 3, "%s: Warning, logical contradiction or incompleteness between clauses (B3): %s\n", vp->name, vp->debug);

            add_erase_vector(cb, lp, lvpp, vp);
            add_erase_vector(cb, rp, rvpp, vp);

            if(!vp->zombie)
              {
                purge_smallnode(cb, vp, bpp, NULL, negated, FALSE);

                *bpp = SPECIAL;
                *lpp = NULL;
                *rpp = NULL;
              }
          }
        else
          if(!vp->zombie)
            {
              purge_smallnode(cb, vp, bpp, NULL, undefined, FALSE);

              if(lp && lp != SPECIAL && rp && rp != SPECIAL)
                {
                  link_directions(vp, bpp);
                  close_smallbranches(cb, lp, rp, lvpp, rvpp, vp);
                }
              else
                clean_smallbranches(cb, lp, rp, lvpp, rvpp);

              *bpp = SPECIAL;
              *lpp = NULL;
              *rpp = NULL;
            }
      break;

      case delay:
        lpp = gendir(vp, bpp, left_son);

        lp = *lpp;

        if(lp && lp != SPECIAL)
          lvpp = get_neighbor_handle(vp, bpp, left_son);
        else
          {
            if(lp)
              lvpp = &cb->specnode;
            else
              lvpp = &cb->nullnode;
          }

        if(!lp)
          {
            lpp = gendir(vp, bpp, right_son);

            lp = *lpp;

            if(lp && lp != SPECIAL)
              lvpp = get_neighbor_handle(vp, bpp, right_son);
            else
              {
                if(lp)
                  lvpp = &cb->specnode;
                else
                  lvpp = &cb->nullnode;
              }
          }

        add_erase_vector(cb, lp, lvpp, vp);

        if(!vp->zombie)
          {
            purge_smallnode(cb, vp, bpp, NULL, negated, FALSE);

            *bpp = SPECIAL;
            *lpp = NULL;
          }
      break;

      case math_neq0:
        lpp = gendir(vp, bpp, left_son);

        lp = *lpp;

        if(lp && lp != SPECIAL)
          lvpp = get_neighbor_handle(vp, bpp, left_son);
        else
          {
            if(lp)
              lvpp = &cb->specnode;
            else
              lvpp = &cb->nullnode;
          }

        if(!lp)
          {
            lpp = gendir(vp, bpp, right_son);

            lp = *lpp;

            if(lp && lp != SPECIAL)
              lvpp = get_neighbor_handle(vp, bpp, right_son);
            else
              {
                if(lp)
                  lvpp = &cb->specnode;
                else
                  lvpp = &cb->nullnode;
              }
          }

        zero_on_smalltree(cb, lp, lvpp);

        if(!vp->zombie)
          {
            purge_smallnode(cb, vp, bpp, NULL, asserted, FALSE);

            *bpp = NULL;
            *lpp = SPECIAL;
          }
      break;

      case math_eqv0:
      case math_gteq0:
      case math_lt0:
        printmsg(cb->fancymsg, 4, "%s: Error, cannot negate a math relation different from diversity: %s\n", vp->name, vp->debug);
        exit_failure();
      break;

      case literal:
        neg = !vp->k || (vp->k == NULL_TIME && !vp->realval);
        if(vp->neg)
          neg = !neg;

        if(neg)
          printmsg(cb->fancymsg, 3, "%s: Warning, asserted math relation is always false: %s\n", vp->name, vp->debug);
        else
          {
            printmsg(cb->fancymsg, 4, "%s: Error, asserted math relation is always true: %s\n", vp->name, vp->debug);
            exit_failure();
          }
          
        if(!vp->zombie)
          {
            purge_smallnode(cb, vp, bpp, NULL, asserted, FALSE);

            *bpp = NULL;
          }
      break;

      default:
      break;
    }
}

void purge_smalltree(c_base *cb, smallnode *vp, smallnode **bpp)
{
  smallnode **lpp, **rpp, **lvpp, **rvpp;
  smallnode *lp, *rp;
  bool neg;

  switch(vp->nclass)
    {
      case gate:
        if(!vp->zombie)
          {
            lpp = gendir(vp, bpp, left_son);
            rpp = gendir(vp, bpp, right_son);

            lp = *lpp;
            rp = *rpp;

            if(lp && lp != SPECIAL)
              lvpp = get_neighbor_handle(vp, bpp, left_son);
            else
              {
                if(lp)
                  lvpp = &cb->specnode;
                else
                  lvpp = &cb->nullnode;
              }

            if(rp && rp != SPECIAL)
              rvpp = get_neighbor_handle(vp, bpp, right_son);
            else
              {
                if(rp)
                  rvpp = &cb->specnode;
                else
                  rvpp = &cb->nullnode;
              }

            purge_smallnode(cb, vp, bpp, NULL, undefined, FALSE);

            if(lp && lp != SPECIAL && rp && rp != SPECIAL)
              {
                link_directions(vp, bpp);
                close_smallbranches(cb, lp, rp, lvpp, rvpp, vp);
              }
            else
              clean_smallbranches(cb, lp, rp, lvpp, rvpp);

            *bpp = NULL;
            *lpp = SPECIAL;
            *rpp = SPECIAL;
          }
      break;

      case joint:
        lpp = gendir(vp, bpp, left_son);
        rpp = gendir(vp, bpp, right_son);

        lp = *lpp;
        rp = *rpp;

        if(lp && lp != SPECIAL)
          lvpp = get_neighbor_handle(vp, bpp, left_son);
        else
          {
            if(lp)
              lvpp = &cb->specnode;
            else
              lvpp = &cb->nullnode;
          }

        if(rp && rp != SPECIAL)
          rvpp = get_neighbor_handle(vp, bpp, right_son);
        else
          {
            if(rp)
              rvpp = &cb->specnode;
            else
              rvpp = &cb->nullnode;
          }

        if(bpp == &vp->up)
          {
            if(lp == vp && rp == vp)
              {
                printmsg(cb->fancymsg, 4, "%s: Error, logical contradiction between clauses (A2): %s\n", vp->name, vp->debug);
                exit_failure();
              }

            add_purge_vector(cb, lp, lvpp, vp);
            add_purge_vector(cb, rp, rvpp, vp);
          }
        else
          if(lpp == &vp->up)
            {
              add_purge_vector(cb, lp, lvpp, vp);
              add_erase_vector(cb, rp, rvpp, vp);
            }
          else
            if(rpp == &vp->up)
              {
                add_erase_vector(cb, lp, lvpp, vp);
                add_purge_vector(cb, rp, rvpp, vp);
              }

        if(!vp->zombie)
          {
            purge_smallnode(cb, vp, bpp, NULL, asserted, FALSE);

            *bpp = NULL;
            *lpp = SPECIAL;
            *rpp = SPECIAL;
          }
      break;

      case delay:
        lpp = gendir(vp, bpp, left_son);

        lp = *lpp;

        if(lp && lp != SPECIAL)
          lvpp = get_neighbor_handle(vp, bpp, left_son);
        else
          {
            if(lp)
              lvpp = &cb->specnode;
            else
              lvpp = &cb->nullnode;
          }

        if(!lp)
          {
            lpp = gendir(vp, bpp, right_son);

            lp = *lpp;

            if(lp && lp != SPECIAL)
              lvpp = get_neighbor_handle(vp, bpp, right_son);
            else
              {
                if(lp)
                  lvpp = &cb->specnode;
                else
                  lvpp = &cb->nullnode;
              }
          }

        add_purge_vector(cb, lp, lvpp, vp);

        if(!vp->zombie)
          {
            purge_smallnode(cb, vp, bpp, NULL, asserted, FALSE);

            *bpp = NULL;
            *lpp = SPECIAL;
          }
      break;

      case math_eqv0:
        lpp = gendir(vp, bpp, left_son);

        lp = *lpp;

        if(lp && lp != SPECIAL)
          lvpp = get_neighbor_handle(vp, bpp, left_son);
        else
          {
            if(lp)
              lvpp = &cb->specnode;
            else
              lvpp = &cb->nullnode;
          }

        if(!lp)
          {
            lpp = gendir(vp, bpp, right_son);

            lp = *lpp;

            if(lp && lp != SPECIAL)
              lvpp = get_neighbor_handle(vp, bpp, right_son);
            else
              {
                if(lp)
                  lvpp = &cb->specnode;
                else
                  lvpp = &cb->nullnode;
              }
          }

        zero_on_smalltree(cb, lp, lvpp);

        if(!vp->zombie)
          {
            purge_smallnode(cb, vp, bpp, NULL, asserted, FALSE);

            *bpp = NULL;
            *lpp = SPECIAL;
          }
      break;

      case math_neq0:
      case math_gteq0:
      case math_lt0:
        printmsg(cb->fancymsg, 4, "%s: Error, cannot assert a math relation different from identity: %s\n", vp->name, vp->debug);
        exit_failure();
      break;

      case literal:
        neg = !vp->k || (vp->k == NULL_TIME && !vp->realval);
        if(vp->neg)
          neg = !neg;

        if(neg)
          {
            printmsg(cb->fancymsg, 4, "%s: Error, asserted math relation is always false: %s\n", vp->name, vp->debug);
            exit_failure();
          }
        else
          printmsg(cb->fancymsg, 3, "%s: Warning, asserted math relation is always true: %s\n", vp->name, vp->debug);

        if(!vp->zombie)
          {
            purge_smallnode(cb, vp, bpp, NULL, asserted, FALSE);

            *bpp = NULL;
          }
      break;

      default:
      break;
    }
}

int save_smalltree(c_base *cb, FILE *fp)
{
  smallnode *vp;
  int rv;

  vp = cb->network;
  rv = 0;

  while(vp)
    {
      assert(!vp->zombie);

      if(!vp->up || vp->up == SPECIAL || (vp->nclass != literal && (!vp->left || vp->left == SPECIAL)) ||
         ((vp->nclass == gate || vp->nclass == joint || vp->nclass == math_add || vp->nclass == math_mul || vp->nclass == math_pow) && (!vp->right || vp->right == SPECIAL)))
        printmsg(cb->fancymsg, 5, "%s: Warning, network not closed: %s\n", vp->name, vp->debug);

      if(vp == vp->up || vp == vp->left || vp == vp->right)
        printmsg(cb->fancymsg, 2, "%s: Warning, tight loop on node: %s\n", vp->name, vp->debug);

      switch(vp->nclass)
        {
          case gate:
          case joint:
          case math_add:
          case math_mul:
          case math_pow:
            rv = fprintf(fp, "%s: %c ; %s # %d, %s # %d, %s # %d\n",
                    vp->name, class_symbol[vp->nclass],
                    vp->up && vp->up != SPECIAL? vp->up->name : "*", vp->up_dir,
                    vp->left && vp->left != SPECIAL? vp->left->name : "*", vp->left_dir,
                    vp->right && vp->right != SPECIAL? vp->right->name : "*", vp->right_dir);
          break;

          case delay:
          case math_delay:
            rv = fprintf(fp, "%s: %c"TIME_FMT" ; %s # %d, %s # %d\n",
                    vp->name, class_symbol[vp->nclass], vp->k,
                    vp->up && vp->up != SPECIAL? vp->up->name : "*", vp->up_dir,
                    vp->left && vp->left != SPECIAL? vp->left->name : "*", vp->left_dir);
          break;

          case math_chs:
          case math_inv:
          case math_sin:
          case math_eqv0:
          case math_neq0:
          case math_gteq0:
          case math_lt0:
            rv = fprintf(fp, "%s: %c ; %s # %d, %s # %d\n",
                    vp->name, class_symbol[vp->nclass],
                    vp->up && vp->up != SPECIAL? vp->up->name : "*", vp->up_dir,
                    vp->left && vp->left != SPECIAL? vp->left->name : "*", vp->left_dir);
          break;

          case literal:
            if(*vp->root)
              rv = fprintf(fp, "%s: %c%s ; %s # %d\n",
                           vp->name, class_symbol[vp->nclass], vp->root,
                           vp->up && vp->up != SPECIAL? vp->up->name : "*", vp->up_dir);
            else
              if(vp->k != NULL_TIME)
                rv = fprintf(fp, "%s: %c"TIME_FMT" ; %s # %d\n",
                             vp->name, class_symbol[vp->nclass], vp->k,
                             vp->up && vp->up != SPECIAL? vp->up->name : "*", vp->up_dir);

              else
                rv = fprintf(fp, "%s: %c"REAL_OUT_FMT" ; %s # %d\n",
                             vp->name, class_symbol[vp->nclass], vp->realval,
                             vp->up && vp->up != SPECIAL? vp->up->name : "*", vp->up_dir);
          break;

          default:
            assert(FALSE);
          break;
        }

      if(rv < 0)
        {
          printerr(cb->fancymsg, "Error while generating network file");
          exit_failure();
        }

      vp = vp->vp;
    }

  return rv;
}

int save_signals(c_base *cb, FILE *fp)
{
  io_signal *rp, *rp1, *sp, *sp1;
  int rv, h, i;
  real defval;

  sp = cb->sigtab;
  rp = NULL;

  while(sp)
    {
      sp1 = sp;
      sp = sp1->nextsig;

      rp1 = malloc(sizeof(io_signal));
      if(!rp1)
        {
          printerr(cb->fancymsg, NULL);
          exit_failure();
        }

      memcpy(rp1, sp1, sizeof(io_signal));

      rp1->nextsig = rp;
      rp = rp1;

      free(sp1);

      h = hashsymbol(rp1->name) % SYMTAB_SIZE;
      i = 0;

      while((sp1 = cb->sigptr[h][i]))
        {
          if(strcmp(sp1->name, rp1->name))
            {
              i++;

              if(i == SYMTAB_DEPTH)
                {
                  printmsg(cb->fancymsg, 4, "%s, %s: Error, signal names generate duplicate hashes\n",
                      sp1->name, rp1->name);
                  exit_failure();
                }
            }
          else
            break;
        }

      cb->sigptr[h][i] = rp1;
    }

  cb->sigtab = rp;

  rv = 0;
  sp = cb->sigtab;

  while(sp)
    {
      if(sp->defaultval != NULL_TIME)
        defval = sp->defaultval;
      else
        defval = sp->defaultreal;

      if(sp->from && sp->from != SPECIAL && sp->to && sp->to != SPECIAL)
        {
          switch(sp->sclass)
            {
              case input_class:
                rv = fprintf(fp, "!%s %s (%s, %s) # %d / %d, %d, %d, "REAL_OUT_FMT", %d\n", sp->shared? "^" : "",
                             sp->root, sp->from->name, sp->to->name, occurrence(sp->fromto, sp->to), sp->stype, sp->packed, sp->packedbit, defval, sp->omissions);
              break;

              case output_class:
                rv = fprintf(fp, "?%s %s (%s, %s) # %d / %d, %d, %d, "REAL_OUT_FMT", %d\n", sp->shared? "^" : "",
                             sp->root, sp->from->name, sp->to->name, occurrence(sp->fromto, sp->to), sp->stype, sp->packed, sp->packedbit, defval, sp->omissions);
              break;

              case aux_class:
                if(cb->outaux || sp->shared)
                  rv = fprintf(fp, "%s%s %s (%s, %s) # %d / %d, %d, %d, "REAL_OUT_FMT", %d\n", cb->outaux? "." : "_", sp->shared? "^" : "",
                               sp->root, sp->from->name, sp->to->name, occurrence(sp->fromto, sp->to), sp->stype, sp->packed, sp->packedbit, defval, sp->omissions);
                else
                  rv = 0;
              break;

              case internal_class:
                if(cb->outint || sp->shared)
                  rv = fprintf(fp, "%s%s %s (%s, %s) # %d / %d, %d, %d, "REAL_OUT_FMT", %d\n", cb->outint? "." : "_", sp->shared? "^" : "",
                               sp->root, sp->from->name, sp->to->name, occurrence(sp->fromto, sp->to), sp->stype, sp->packed, sp->packedbit, defval, sp->omissions);
                else
                  rv = 0;
              break;

              case hidden_class:
                if(sp->shared)
                  rv = fprintf(fp, "_^ %s (%s, %s) # %d / %d, %d, %d, "REAL_OUT_FMT", %d\n",
                               sp->root, sp->from->name, sp->to->name, occurrence(sp->fromto, sp->to), sp->stype, sp->packed, sp->packedbit, defval, sp->omissions);
                else
                  rv = 0;
              break;

              default:
                assert(FALSE);
              break;
            }
        }
      else
        {
          if(cb->constout && sp->sclass == output_class && sp->defaultval != io_unknown)
            {
              if(sp->val == asserted || (cb->constout_sugg && sp->val == negated) || (cb->constout_user && sp->val == undefined))
                {
                  rv = fprintf(fp, "? %s (*, *) # 0 / %d, %d, %d, "REAL_OUT_FMT", %d\n", sp->root, sp->stype, sp->packed, sp->packedbit, defval, sp->omissions);

                  printmsg(cb->fancymsg, 1, "%s: Warning, constant output signal generated as %s by default\n", sp->name, sp->defaultval == io_false? "false" : "true");
                }
              else
                printmsg(cb->fancymsg, 1, "%s: Warning, constant or unknown output signal suggested as %s by default\n", sp->name, sp->defaultval == io_false? "false" : "true");
            }
          else
            if(sp->sclass != internal_class && !sp->removed)
              printmsg(cb->fancymsg, 2, "%s: Warning, %s signal not generated\n", sp->name, signal_class[sp->sclass]);
        }

      if(rv < 0)
        {
          printerr(cb->fancymsg, "Error while generating network file");
          exit_failure();
        }

      sp = sp->nextsig;
    }

  return rv;
}

int save_ics(c_base *cb, FILE *fp)
{
  initial_condition *icp;
  io_signal *sp;
  char buffer[MAX_NAMELEN];
  int rv;

  rv = 0;
  icp = cb->ictab;

  while(icp)
    {
      sp = name2signal(cb, icp->name, FALSE);

      if(sp && sp->from && sp->from != SPECIAL && sp->to && sp->to != SPECIAL)
        {
          if(icp->realval == REAL_MAX)
            *buffer = '\0';
          else
            sprintf(buffer, " = "REAL_OUT_FMT, icp->realval);

          if(!icp->neg)
            rv = fprintf(fp, "(%s, %s) # %d @ "TIME_FMT"%s\n", sp->from->name, sp->to->name, occurrence(sp->fromto, sp->to), icp->t, buffer);
          else
            rv = fprintf(fp, "(%s, %s) # %d @ "TIME_FMT"%s\n", sp->to->name, sp->from->name, occurrence(sp->tofrom, sp->from), icp->t, buffer);
        }
      else
        {
          printmsg(cb->fancymsg, 4, "%s: Error, initial condition refers to undeclared or removed signal\n", icp->name);
          exit_failure();
        }
        
      if(rv < 0)
        {
          printerr(cb->fancymsg, "Error while generating initial conditions file");
          exit_failure();
        }

      icp = icp->nextic;
    }

  return rv;
}

int save_xref(c_base *cb, FILE *fp)
{
  smallnode *vp;
  int rv;

  vp = cb->network;
  rv = 0;

  while(vp)
    {
      assert(!vp->zombie);
      assert(strlen(vp->debug) <= DEBUG_STRLEN);

      rv = fprintf(fp, "%s: %s\n", vp->name, vp->debug);
      if(rv < 0)
        {
          printerr(cb->fancymsg, "Error while generating symbol table file");
          exit_failure();
        }

      vp = vp->vp;
    }

  return rv;
}

void link_cotree(c_base *cb)
{
  smallnode *vp, *up, *up_2, *up_left, *up_2_left;
  io_signal *sp;
  symlist *si, *sl;

  si = cb->symtab;
  
  while(si)
    {
      sl = si;
    
      while(sl)
        {
          vp = sl->vp;

          assert(vp);
      
          up = vp->up;
          up_2 = vp->up_2;
          up_left = up->left;
          up_2_left = up_2->left;

          assert(up && up_2);
          assert(up != up_2);

          assert(vp == up->left || vp == up->right);

          if(vp == up_left)
            {
              up->left = up_2;
              up->left_dir = (vp == up_2_left? left_son : right_son);
            }
          else
            {
              up->right = up_2;
              up->right_dir = (vp == up_2_left? left_son : right_son);
            }

          assert(vp == up_2->left || vp == up_2->right);

          if(vp == up_2_left)
            {
              up_2->left = up;
              up_2->left_dir = (vp == up_left? left_son : right_son);
            }
          else
            {
              up_2->right = up;
              up_2->right_dir = (vp == up_left? left_son : right_son);
            }

          sp = cb->sigtab;

          while(sp)
            {
              if(sp->from == vp)
                {
                  assert(sp->to == NULL);
                  assert(!vp->neg);

                  sp->from = up;
                  sp->to = up_2;

                  sp->tofrom = (vp == up_left? &up->left : &up->right);
                  sp->fromto = (vp == up_2_left? &up_2->left : &up_2->right);
                }
              else
                if(sp->to == vp)
                  {
                    assert(sp->from == NULL);
                    assert(vp->neg);

                    sp->from = up_2;
                    sp->to = up;

                    sp->tofrom = (vp == up_2_left)? &up_2->left : &up_2->right;
                    sp->fromto = (vp == up_left)? &up->left : &up->right;
                  }
                  
              sp = sp->nextsig;
            }

          vp->up = NULL;
          vp->up_2 = NULL;

          vp->zombie = TRUE;
          
          sl = sl->nextocc;
        }
      
      si = si->nextlit;
    }
}

void raise_signals(c_base *cb, smallnode *vp)
{
  smallnode *wp;
  io_signal *sp;
  bool branch;

  assert(vp);

  for(branch = FALSE; branch <= TRUE; branch++)
    {
      if(!branch)
        wp = vp->left;
      else
        wp = vp->right;

      if(wp != SPECIAL && vp == wp->up)
        {
          raise_signals(cb, wp);

          if(wp->nclass == joint)
            {
              sp = cb->sigtab;

              while(sp)
                {
                  if(sp->from == wp)
                    {
                       sp->from = vp;
                       sp->to = wp;

                       sp->tofrom = !branch? &vp->left : &vp->right;
                       sp->fromto = &wp->up;
                    }
                  else
                    if(sp->to == wp)
                      {
                         sp->from = wp;
                         sp->to = vp;

                         sp->tofrom = &wp->up;
                         sp->fromto = !branch? &vp->left : &vp->right;
                      }
                   
                   sp = sp->nextsig;
                }
            }
        }
    }
}

smallnode *build_smalltree(c_base *cb, symlist *sl, bool neg)
{
  smallnode *vp, *xp, *yp;

  xp = NULL;
  yp = NULL;

  while(sl)
   {
      vp = sl->vp;

      assert(vp);

      if(vp->neg == neg)
        {
          if(yp)
            {
              xp = create_smallnode(cb, joint);
              if(!xp)
                {
                  printerr(cb->fancymsg, NULL);
                  exit_failure();
                }

              xp->left = vp;
              xp->right = yp;

              xp->left_dir = parent;
              xp->right_dir = parent;

              vp->up_2 = xp;
              yp->up_2 = xp;

              vp->up_2_dir = left_son;
              yp->up_2_dir = right_son;

              snprintf(xp->debug, DEBUG_STRLEN, "{%s & %s}", vp->debug, yp->debug);

              if(strlen(xp->debug) == DEBUG_STRLEN - 1)
                {
                  strcpy(xp->debug, "<symbol>");
                  printmsg(cb->fancymsg, 2, "Warning, symbol table overflow in second phase\n");
                }

              yp = xp;
            }
          else
            {
              xp = vp;
              yp = xp;
            }
        }

      sl = sl->nextocc;
    }

  return xp;
}

smallnode *build_twotrees(c_base *cb, symlist *si)
{
  smallnode *vp, *wp, *xp;

  xp = create_smallnode(cb, gate);
  if(!xp)
    {
      printerr(cb->fancymsg, NULL);
      exit_failure();
    }

  vp = build_smalltree(cb, si, FALSE);

  if(!vp)
    {
      if(si->vp->name[0] != '+' && si->vp->name[0] != '-')
        printmsg(cb->fancymsg, 2, "%s: Warning, missing asserted literal for signal\n", si->vp->name);

      vp = SPECIAL;
    }
  else
   {
     vp->up_2 = xp;
     vp->up_2_dir = left_son;
   }

  wp = build_smalltree(cb, si, TRUE);

  if(!wp)
    {
      if(si->vp->name[0] != '+' && si->vp->name[0] != '-')
        printmsg(cb->fancymsg, 2, "%s: Warning, missing negated literal for signal\n", si->vp->name);

      wp = SPECIAL;
    }
  else
    {
      wp->up_2 = xp;
      wp->up_2_dir = right_son;
    }

  xp->left = vp;
  xp->right = wp;

  xp->left_dir = parent;
  xp->right_dir = parent;

  snprintf(xp->debug, DEBUG_STRLEN, "{%s | %s}", vp != SPECIAL? vp->debug : "<blank>", wp != SPECIAL? wp->debug : "<blank>");

  if(strlen(xp->debug) == DEBUG_STRLEN - 1)
    {
      strcpy(xp->debug, "<symbol>");
      printmsg(cb->fancymsg, 2, "Warning, symbol table overflow in second phase\n");
    }

  return xp;
}

smallnode *build_cotree(c_base *cb)
{
  smallnode *vp, *xp, *yp;
  symlist *si;

  xp = NULL;
  yp = NULL;

  si = cb->symtab;

  while(si)
    {
      vp = build_twotrees(cb, si);

      if(yp)
        {
          xp = create_smallnode(cb, joint);
          if(!xp)
            {
              printerr(cb->fancymsg, NULL);
              exit_failure();
            }

          xp->left = vp;
          xp->right = yp;

          xp->left_dir = parent;
          xp->right_dir = parent;

          vp->up_2 = xp;
          yp->up_2 = xp;

          vp->up_2_dir = left_son;
          yp->up_2_dir = right_son;

          snprintf(xp->debug, DEBUG_STRLEN, "{%s & %s}", vp->debug, yp->debug);

          if(strlen(xp->debug) == DEBUG_STRLEN - 1)
            {
              strcpy(xp->debug, "<symbol>");
              printmsg(cb->fancymsg, 2, "Warning, symbol table overflow in second phase\n");
            }

          yp = xp;
        }
      else
        {
          xp = vp;
          yp = xp;
        }
        
      si = si->nextlit;
    }

  link_cotree(cb);

  vp = cb->network;

  while(vp)
    {
      if((!vp->up || vp->up == SPECIAL) && vp->up_2 && vp->up_2 != SPECIAL)
        {
          vp->up = vp->up_2;
          vp->up_dir = vp->up_2_dir;
        }

      vp = vp->vp;
    }

  if(xp)
    raise_signals(cb, xp);

  return xp;
}

void free_tabs(c_base *cb)
{
  symlist *si, *sl, *si1, *sl1;
  io_signal *sp, *sp1;
  group *grpp, *grpp1;
  constant *tp, *tp1;
  initial_condition *icp, *icp1;

  si = cb->symtab;
  
  while(si)
    {
      si1 = si;
      si = si1->nextlit;

      sl = si1;

      while(sl)
        {
          sl1 = sl;
          sl = sl1->nextocc;

          free(sl1);
        }
    }

  sp = cb->sigtab;
  
  while(sp)
    {
      sp1 = sp;
      sp = sp1->nextsig;

      free(sp1);
    }

  grpp = cb->grptab;
  
  while(grpp)
    {
      grpp1 = grpp;
      grpp = grpp1->nextgrp;

      free(grpp1);
    }

  tp = cb->inttab;
  
  while(tp)
    {
      tp1 = tp;
      tp = tp1->nextcon;

      free(tp1);
    }

  icp = cb->ictab;
  
  while(icp)
    {
      icp1 = icp;
      icp = icp1->nextic;

      free(icp1);
    }
}

bool remove_pair(c_base *cb, smallnode *vp, smallnode *wp)
{
  smallnode **ubpp, **vbpp, **fbpp, **gbpp, **hbpp, **kbpp, **xbpp, **ybpp, **w1bpp, **w2bpp;
  link_code xlc;
  link_code *w2lcp;
  bool flip_v, flip_w, swap_v, swap_w;

  flip_v = flip_w = swap_v = swap_w = FALSE;

  ubpp = &vp->left;

  if(wp == vp->up)
    {
      if(wp != vp->left)
        ubpp = &vp->right;
      else
        if(wp != vp->right)
          {
            ubpp = &vp->up;
            swap_v = TRUE;
          }
        else
          {
            printmsg(cb->fancymsg, 2, "%s: Warning, tight loop on pair prevents optimization: %s\n", vp->name, vp->debug);
            return FALSE;
          }

      flip_v = TRUE;
    }

  vbpp = &wp->left;

  if(vp == wp->up)
    {
      if(vp != wp->left)
        vbpp = &wp->right;
      else
        if(vp != wp->right)
          {
            vbpp = &wp->up;
            swap_w = TRUE;
          }
        else
          {
            printmsg(cb->fancymsg, 2, "%s: Warning, tight loop on pair prevents optimization: %s\n", wp->name, wp->debug);
            return FALSE;
          }

      flip_w = !flip_v;
    }

  fbpp = gendir(vp, ubpp, left_son);
  gbpp = gendir(vp, ubpp, right_son);

  hbpp = gendir(wp, vbpp, left_son);
  kbpp = gendir(wp, vbpp, right_son);

  xbpp = get_neighbor_handle(vp, ubpp, left_son);
  ybpp = get_neighbor_handle(wp, vbpp, left_son);

  w1bpp = get_neighbor_handle(vp, fbpp, left_son);
  w2bpp = get_neighbor_handle(vp, fbpp, right_son);

  xlc = *dirdir(vp, ubpp, left_son);
  w2lcp = get_neighbor_dir(vp, fbpp, right_son);

  if(!swap_v)
    purge_smallnode(cb, vp, ubpp, gbpp, undefined, flip_v);
  else
    purge_smallnode(cb, vp, gbpp, ubpp, undefined, flip_v);

  link_directions(vp, ubpp);
  close_smallbranches(cb, *fbpp, wp, xbpp, w1bpp, vp);
  close_smallbranches(cb, *fbpp, wp, xbpp, w2bpp, vp);

  /* *xlcp = w2lc; */
  *w2lcp = xlc;

  if(!swap_w)
    purge_smallnode(cb, wp, vbpp, kbpp, undefined, flip_w);
  else
    purge_smallnode(cb, wp, kbpp, vbpp, undefined, flip_w);

  link_directions(wp, vbpp);
  close_smallbranches(cb, *fbpp, *hbpp, xbpp, ybpp, wp);

  vp->up = NULL;
  vp->left = NULL;
  vp->right = NULL;

  wp->up = NULL;
  wp->left = NULL;
  wp->right = NULL;

  return TRUE;
}

void postoptimize(c_base *cb)
{
  smallnode *vp, *wp;
  bool changed;
  int num_nodes;

  num_nodes = 0;

  do
    {
      vp = cb->network;
      changed = FALSE;

      while(vp)
        {
          if(!vp->zombie && vp->nclass == joint)
            {
              wp = vp->left;

              if(vp != wp && wp == vp->right && (wp->nclass == gate || (vp == wp->left && vp == wp->right)))
                {
                  if(remove_pair(cb, vp, wp))
                    {
                      num_nodes += 2;
                      changed = TRUE;

                      if(wp->nclass == gate)
                        printmsg(cb->fancymsg, 1, "%s, %s: Inference increment during removal of pair: %s, %s\n", vp->name, wp->name, vp->debug, wp->debug);
                    }
                }
              else
                {
                  wp = vp->up;

                  if(vp != wp && vp->up_dir == parent && (wp == vp->right || wp == vp->left) &&
                    (wp->nclass == joint && vp == wp->up && wp->up_dir == parent && (vp == wp->left || vp == wp->right)))
                    {
                      if(remove_pair(cb, vp, wp))
                        {
                          num_nodes += 2;
                          changed = TRUE;
                        }
                    }
                }
            }

          vp = vp->vp;
       }
    }
  while(changed);
  
  if(num_nodes)
    printf("%d nodes have been removed\n", num_nodes);
}

compinfo compile(char *source_name, char *base_name, char *state_name, char *xref_name, char *path,
                 bool seplit_fe, bool seplit_su, bool merge, bool constout, bool constout_sugg, bool constout_user, bool outaux, bool outint, bool postopt, bool fancymsg)
{
  c_base *cb;
  btl_specification *e, *f;
  subtreeval stv;
  smallnode *cvp;
  FILE *fp, *gp, *hp, *ip;
  char source_filename[MAX_STRLEN], base_filename[MAX_STRLEN], state_filename[MAX_STRLEN], xref_filename[MAX_STRLEN];
  compinfo cperf;
  node_class nclass, lclass;
  io_class sclass;

  cperf.ok = FALSE;

  strcpy(source_filename, source_name);
  strcat(source_filename, SOURCE_EXT);

  strcpy(base_filename, base_name);
  strcat(base_filename, NETWORK_EXT);

  cb = malloc(sizeof(c_base));
  if(!cb)
    {
      printerr(fancymsg, NULL);
      return cperf;
    }

  memset(cb, 0, sizeof(c_base));

  if(*path)
    {
      strcpy(cb->path, path);
      strcat(cb->path, "/");
    }
  else
    *(cb->path) = '\0';

  cb->seplit_fe = seplit_fe;
  cb->seplit_su = seplit_su;
  cb->merge = merge;
  cb->constout = constout;
  cb->constout_sugg = constout_sugg;
  cb->constout_user = constout_user;
  cb->outaux = outaux;
  cb->outint = outint;
  cb->fancymsg = fancymsg;
  cb->nullnode = NULL;
  cb->specnode = SPECIAL;

  fp = fopen(source_filename, "r");
  if(!fp)
    {
      printerr(cb->fancymsg, source_filename);
      free(cb);
      return cperf;
    }

  if(fread(cb->source, sizeof(char), SOURCE_BUFSIZE, fp) == SOURCE_BUFSIZE)
    {
      printmsg(cb->fancymsg, 4, "%s: Error, source file too large\n", source_filename);
      free(cb);
      return cperf;
    }

  if(ferror(fp))
    {
      printerr(cb->fancymsg, source_filename);
      free(cb);
      return cperf;
    }

  fclose(fp);

  printf("%s: compiling master %s to %s ...\n", hilt(cb->fancymsg, "Phase 1"), hilt(cb->fancymsg, source_filename), hilt(cb->fancymsg, base_filename));

  e = parse(cb->source);
  if(!e)
    {
      free(cb);
      return cperf;
    }

  stv = preval(cb, e, 0, NULL_TIME, REAL_MAX);

  delete_specification(e);

  if(stv.btldef)
    f = create_operation(op_and, stv.btl, stv.btldef, "%s ; %s");
  else
    f = stv.btl;

  printf("... %s syntax ok\n", hilt(cb->fancymsg, source_filename));

  printf("%s: generating network\n", hilt(cb->fancymsg, "Phase 2"));

  stv = eval(cb, f, NULL, left_son, FALSE, internal_class, io_any, io_binary, io_unknown, io_raw, NULL_TIME, REAL_MAX);

  delete_specification(f);

  cvp = build_cotree(cb);

  if(cb->network)
    {
      printf("%s: pruning network\n", hilt(cb->fancymsg, "Phase 3"));

      add_purge_vector(cb, stv.vp, &stv.vp->up, stv.vp->up);

      purge_vectors(cb);

      if(cvp)
        add_purge_vector(cb, cvp, &cvp->up, cvp->up);

      do
        {
          purge_vectors(cb);
          erase_vectors(cb);
        }
      while(cb->start_purgearrows != cb->end_purgearrows || cb->start_erasearrows != cb->end_erasearrows);

      if(postopt)
        {
          printf("%s: post optimizing network\n", hilt(cb->fancymsg, "Phase 3.5"));
          postoptimize(cb);
        }

      delete_zombies(cb);
      turn_math_ops(cb);
    }

  if(!cb->network)
    printmsg(cb->fancymsg, 2, "Warning, network empty\n");

  printf("%s: generating object files\n", hilt(cb->fancymsg, "Phase 4"));

  gp = fopen(base_filename, "w");
  if(!gp)
    {
      printerr(cb->fancymsg, base_filename);
      free(cb);
      return cperf;
    }

  save_smalltree(cb, gp);
  save_signals(cb, gp);

  fclose(gp);

  if(cb->num_ics > 0)
    {
      strcpy(state_filename, state_name);
      strcat(state_filename, EVENT_LIST_EXT);

      hp = fopen(state_filename, "w");
      if(!hp)
        {
          printerr(cb->fancymsg, state_filename);
          free(cb);
          return cperf;
        }

      save_ics(cb, hp);

      fclose(hp);
    }

  if(xref_name)
    {
      strcpy(xref_filename, xref_name);
      strcat(xref_filename, XREF_EXT);

      ip = fopen(xref_filename, "w");
      if(!ip)
        {
          printerr(cb->fancymsg, xref_filename);
          free(cb);
          return cperf;
        }

      save_xref(cb, ip);

      fclose(ip);
    }

  delete_smalltree(cb);

  cperf.num_nodes = 0;
  cperf.edges = 0;

  for(lclass = 0; lclass < NODE_LARGE_CLASSES; lclass++)
    cperf.part_nodes[lclass] = 0;

  for(nclass = 0; nclass < NODE_CLASSES_NUMBER; nclass++)
    {
      if(nclass < NODE_MATH_BASE)
        lclass = nclass;
      else
        if(nclass < NODE_REL_BASE)
          lclass = NODE_LARGE_MATH;
        else
          lclass = NODE_LARGE_REL;

      cperf.num_nodes += cb->part_nodes[nclass];
      cperf.part_nodes[lclass] += cb->part_nodes[nclass];

      if(nclass == literal)
        cperf.edges += cb->part_nodes[nclass];
      else
        if(nclass == gate || nclass == joint || nclass == math_add || nclass == math_mul || nclass == math_pow)
          cperf.edges += 3 * cb->part_nodes[nclass];
        else
          cperf.edges += 2 * cb->part_nodes[nclass];
    }

  cperf.edges /= 2;

  for(sclass = 0; sclass < IO_CLASSES_NUMBER; sclass++)
    cperf.part_signals[sclass] = cb->part_signals[sclass];

  cperf.num_signals = cb->num_signals;
  cperf.num_ics = cb->num_ics;
  cperf.ok = TRUE;

  free_tabs(cb);
  free(cb);

  return cperf;
}

int main(int argc, char *argv[])
{
  char *source_name, *base_name, *state_name, *xref_name, *path, *option, *ext;
  char default_state_name[MAX_STRLEN], default_xref_name[MAX_STRLEN];
  bool seplit_fe, seplit_su, merge, constout, constout_sugg, constout_user, outaux, outint, postopt, fancymsg;
  compinfo cperf;
  int i;

  source_name = base_name = state_name = xref_name = NULL;
  path = "";
  seplit_fe = seplit_su = merge = constout = constout_sugg = constout_user = outaux = outint = postopt = fancymsg = FALSE;

  for(i = 1; i < argc; i++)
    {
      if(*argv[i] == '-')
        {
          option = argv[i] + 1;
          switch(*option)
            {
            case 'h':
              printmsg(0, 0, "Usage: %s [-bBkKmMOuwWx] [-I state] [-o base] [-P path] [-X symbols] [source]\n",
                      argv[0]);
              exit(EXIT_SUCCESS);
            break;

            case 'o':
              if(*(++option))
                {
                  printmsg(0, 0, "%s, %c: Invalid command line option"
                                  " (%s -h for help)\n",
                          argv[i], *option, argv[0]);
                  exit(EXIT_FAILURE);
                }

              if(base_name)
                {
                  printmsg(0, 0, "%s: Duplicate option\n", argv[i]);
                  exit(EXIT_FAILURE);
                }

              if(++i < argc)
                base_name = argv[i];
              else
                {
                  printmsg(0, 0, "%s: Missing argument\n", argv[--i]);
                  exit(EXIT_FAILURE);
                }

              ext = strstr(base_name, NETWORK_EXT);
              if(ext && !strcmp(ext, NETWORK_EXT))
                *ext = '\0';
            break;

            case 'I':
              if(*(++option))
                {
                  printmsg(0, 0, "%s, %c: Invalid command line option"
                                  " (%s -h for help)\n",
                          argv[i], *option, argv[0]);
                  exit(EXIT_FAILURE);
                }

              if(state_name && state_name != default_state_name)
                {
                  printmsg(0, 0, "%s: Duplicate option\n", argv[i]);
                  exit(EXIT_FAILURE);
                }

              if(++i < argc)
                state_name = argv[i];
              else
                {
                  printmsg(0, 0, "%s: Missing argument\n", argv[--i]);
                  exit(EXIT_FAILURE);
                }

              ext = strstr(state_name, EVENT_LIST_EXT);
              if(ext && !strcmp(ext, EVENT_LIST_EXT))
                *ext = '\0';
            break;

            case 'P':
              if(*(++option))
                {
                  printmsg(0, 0, "%s, %c: Invalid command line option"
                                  " (%s -h for help)\n",
                          argv[i], *option, argv[0]);
                  exit(EXIT_FAILURE);
                }

              if(++i < argc)
                path = argv[i]; 
              else
                {
                  printmsg(0, 0, "%s: Missing argument\n", argv[--i]);
                  exit(EXIT_FAILURE);
                }
            break;

            case 'X':
              if(*(++option))
                {
                  printmsg(0, 0, "%s, %c: Invalid command line option"
                                  " (%s -h for help)\n",
                          argv[i], *option, argv[0]);
                  exit(EXIT_FAILURE);
                }

              if(xref_name && xref_name != default_xref_name)
                {
                  printmsg(0, 0, "%s: Duplicate option\n", argv[i]);
                  exit(EXIT_FAILURE);
                }

              if(++i < argc)
                xref_name = argv[i];
              else
                {
                  printmsg(0, 0, "%s: Missing argument\n", argv[--i]);
                  exit(EXIT_FAILURE);
                }

              ext = strstr(xref_name, XREF_EXT);
              if(ext && !strcmp(ext, XREF_EXT))
                *ext = '\0';
            break;

            default:
              do
                {
                  switch(*option)
                    {
                      case 'b':
                        outaux = TRUE;
                      break;

                      case 'B':
                        outint = TRUE;
                      break;

                      case 'k':
                        constout = TRUE;
                      break;

                      case 'K':
                        constout_sugg = TRUE;
                      break;

                      case 'm':
                        constout_user = TRUE;
                      break;

                      case 'M':
                        fancymsg = isdigit(*(option + 1))? atoi(++option) : TRUE;
                      break;

                      case 'O':
                        postopt = TRUE;
                      break;

                      case 'u':
                        merge = TRUE;
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

                      default:
                        printmsg(0, 0, "%s, %c: Invalid command line option"
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
            printmsg(0, 0, "%s: Extra argument ignored\n", argv[i]);
        }
    }

  if(!source_name)
    source_name = DEFAULT_NAME;

  if(!base_name)
    base_name = source_name;

  if(!state_name)
    {
      state_name = default_state_name;

      strcpy(state_name, base_name);
      strcat(state_name, STATE_SUFFIX);
    }

  if(xref_name == default_xref_name)
    strcpy(xref_name, base_name);

  printmsg(fancymsg, 0, "\nTING "VER" - Temporal Inference Network Generator\n"
           "Design & coding by Andrea Giotti, 2017-2025\n\n");

  cperf = compile(source_name, base_name, state_name, xref_name, path, seplit_fe, seplit_su, merge, constout, constout_sugg, constout_user, outaux, outint, postopt, fancymsg);

  if(cperf.ok)
    printmsg(fancymsg, 0,
    "Network generated -- %d edges, %d nodes (%d gates + %d joints + %d delays + %d literals + %d math ops + %d math rels), %d signals (%d inputs + %d outputs + %d auxiliaries + %d internals), %d initial conditions\n",
             cperf.edges, cperf.num_nodes,
             cperf.part_nodes[gate], cperf.part_nodes[joint], cperf.part_nodes[delay], cperf.part_nodes[literal], cperf.part_nodes[NODE_LARGE_MATH], cperf.part_nodes[NODE_LARGE_REL],
             cperf.num_signals, cperf.part_signals[input_class], cperf.part_signals[output_class], cperf.part_signals[aux_class], cperf.part_signals[internal_class], cperf.num_ics);
  else
    printmsg(fancymsg, 0, "Network not generated\n");

  return cperf.ok? EXIT_SUCCESS : EXIT_FAILURE;
}


