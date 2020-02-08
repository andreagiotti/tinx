/*
  TING - Temporal Inference Network Generator
  Design & coding by Andrea Giotti, 2017
*/

#define NDEBUG

#include "ting_core.h"
#include "ting_parser.h"
#include "ting_lexer.h"

#define VER "4.1.0"

const char class_symbol[NODE_CLASSES_NUMBER] = CLASS_SYMBOLS;
const char *signal_class[IO_CLASSES_NUMBER] = { "internal", "auxiliary", "input", "output" };

int yyparse(btl_specification **spec, yyscan_t scanner);

void exit_failure()
{
  printf("Network not generated\n");
  exit(EXIT_FAILURE);
}

btl_specification *alloc_syntnode()
{
  btl_specification *sp;

  sp = malloc(sizeof(btl_specification));
  if(!sp)
    return NULL;

  sp->ot = op_invalid;
  sp->symbol[0] = '\0';
  sp->value = 0;

  sp->left = NULL;
  sp->right = NULL;

  sp->debug[0] = '\0';

  return sp;
}

btl_specification *create_ground(op_type ot, char *symbol, d_time value)
{
  btl_specification *sp;

  if(strlen(symbol) >= MAX_STRLEN)
    {
      fprintf(stderr, "%s: Error, string too long\n", symbol);
      exit_failure();
    }

  sp = alloc_syntnode();
  if(!sp)
    {
      perror(NULL);
      exit_failure();
    }

  sp->ot = ot;
  strcpy(sp->symbol, symbol);
  sp->value = value;

  if(symbol[0])
    strcpy(sp->debug, symbol);
  else
    sprintf(sp->debug, TIME_FMT, value);

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
      fprintf(stderr, "Warning, symbol table overflow in first phase\n");
    }

  return sp;
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

  rp->left = copy_specification(sp->left);
  rp->right = copy_specification(sp->right);

  strcpy(rp->debug, sp->debug);

  return rp;
}

void print_specification(btl_specification *sp)
{
  if(!sp)
    return;

  print_specification(sp->left);

  printf("(%d/%s/%ld)", sp->ot, sp->symbol, sp->value);

  print_specification(sp->right);
}

smallnode *create_smallnode(c_base *cb, node_class nclass)
{
  smallnode *vp;

  vp = (smallnode *)malloc(sizeof(smallnode));
  if(!vp)
    return NULL;

  sprintf(vp->name, "%c%06d", class_symbol[nclass], cb->num_nodes[nclass]);
  cb->num_nodes[nclass]++;

  vp->nclass = nclass;
  vp->k = 0;
  vp->literal_id = 0;
  vp->neg = FALSE;
  vp->zombie = FALSE;
  vp->open = 0;

  vp->up = NULL;
  vp->up_2 = NULL;
  vp->left = NULL;
  vp->right = NULL;

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

  cb->num_nodes[vp->nclass]--;

  free(vp);
}

void delete_zombies(c_base *cb)
{
  smallnode *vp, *wp, *zp;

  vp = cb->network;
  zp = NULL;

  while(vp)
    {
      wp = vp->vp;

      if(vp->zombie)
        {
          if(zp)
            zp->vp = wp;
          else
            cb->network = wp;

          cb->num_nodes[vp->nclass]--;

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
      fprintf(stderr, "Error, internal lexical analyzer failure\n");
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
  int h, i;

  if(*name)
    {
      if(strlen(name) >= MAX_NAMELEN)
        {
          fprintf(stderr, "%s: Error, name too long\n", name);
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
                  fprintf(stderr, "%s, %s: Error, node names generate duplicate hashes\n", vp->name, name);
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
                     perror(NULL);
                     exit_failure();
                   }

                  cb->symtab[vp->literal_id][cb->symcount[vp->literal_id]] = wp;

                  strcpy(wp->name, name);
                  wp->literal_id = vp->literal_id;

                  cb->symcount[vp->literal_id]++;

                  if(cb->symcount[vp->literal_id] >= NUM_OCCURRENCES)
                    {
                      fprintf(stderr, "%s: Error, too many occurrences for literal\n", name);
                      exit_failure();
                    }
                }
              else
                wp = cb->symtab[vp->literal_id][0];

              return wp;
            }
        }

      if(create)
        {
          vp = create_smallnode(cb, literal);
          if(!vp)
            {
              perror(NULL);
              exit_failure();
            }

          cb->symtab[cb->num_literals][0] = vp;

          strcpy(vp->name, name);
          vp->literal_id = cb->num_literals;

          cb->symcount[cb->num_literals] = 1;
          cb->num_literals++;

          if(cb->num_literals >= NUM_LITERALS)
            {
              fprintf(stderr, "Error, too many literals in formula\n");
              exit_failure();
            }

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
          fprintf(stderr, "%s: Error, name too long\n", name);
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
                  fprintf(stderr, "%s, %s: Error, signal names generate duplicate hashes\n",
                      sp->name, name);
                  exit_failure();
                }
            }
          else
            return &cb->sigtab[sp->signal_id];
        }

      if(create)
        {
          sp = &cb->sigtab[cb->num_signals];

          strcpy(sp->name, name);
          *sp->root = '\0';
          sp->from = NULL;
          sp->to = NULL;
          sp->occurr = no_link;
          sp->occurr_neg = no_link;
          sp->sclass = internal_class;
          sp->stype = io_any;
          sp->packed = 0;
          sp->packedbit = 0;
          sp->defaultval = io_unknown;
          sp->omissions = io_raw;
          sp->val = undefined;
          sp->removed = FALSE;
          sp->signal_id = cb->num_signals;

          cb->num_signals++;

          if(cb->num_signals >= NUM_LITERALS)
            {
              fprintf(stderr, "Error, too many signals declared\n");
              exit_failure();
            }

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
          fprintf(stderr, "%s: Error, name too long\n", name);
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
                  fprintf(stderr, "%s, %s: Error, constant names generate duplicate hashes\n",
                      tp->name, name);
                  exit_failure();
                }
            }
          else
            return &cb->inttab[tp->integer_id];
        }

      if(create)
        {
          tp = &cb->inttab[cb->num_integers];

          strcpy(tp->name, name);
          tp->integer_id = cb->num_integers;

          cb->num_integers++;

          if(cb->num_integers >= NUM_INTEGERS)
            {
              fprintf(stderr, "Error, too many constants declared\n");
              exit_failure();
            }

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
          fprintf(stderr, "%s: Error, name too long\n", name);
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
                  fprintf(stderr, "%s, %s: Error, packed signal names generate duplicate hashes\n",
                      grpp->name, name);
                  exit_failure();
                }
            }
          else
            return &cb->grptab[grpp->group_id];
        }

      if(create)
        {
          grpp = &cb->grptab[cb->num_groups];

          strcpy(grpp->name, name);
          grpp->group_id = cb->num_groups;

          cb->num_groups++;

          if(cb->num_groups >= NUM_LITERALS)
            {
              fprintf(stderr, "Error, too many packed signals declared\n");
              exit_failure();
            }

          cb->grpptr[h][i] = grpp;

          return grpp;
        }
      else
        return NULL;
    }
  else
    return NULL;
}

void add_ic(c_base *cb, char *name, bool neg, d_time t)
{
  if(strlen(name) >= MAX_NAMELEN)
    {
      fprintf(stderr, "%s: Error, name too long\n", name);
      exit_failure();
    }

  strcpy(cb->ictab[cb->num_ics].name, name);
  cb->ictab[cb->num_ics].neg = neg;
  cb->ictab[cb->num_ics].t = t;

  cb->num_ics++;

  if(cb->num_ics >= NUM_ICS)
    {
      fprintf(stderr, "Error, too many initial conditions\n");
      exit_failure();
    }
}

void gensym(c_base *cb, char *symbol, char *type, litval val, bool incr)
{
  sprintf(symbol, "%c%s%d", val == negated? '-' : (val == asserted? '+' : '_'), type, cb->num_vargen);
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
        return "@";
      break;

      default:
        assert(FALSE);
      break;
    }

  return "";
}

subtreeval preval(c_base *cb, btl_specification *spec, int level, int param)
{
  subtreeval stv, stv_2;
  char symbol[MAX_STRLEN], debug[MAX_STRLEN], filename[MAX_STRLEN];
  btl_specification *e, *p, *q, *r;
  constant *tp;
  d_time val, h, k, n;
  int l, start, end;
  op_type ot;
  FILE *fp;
  char c;
  char *endp;

  stv.btl = NULL;
  stv.btldef = NULL;
  stv.a = 0;
  stv.b = 0;
  stv.xtra = NULL;
  stv.ytra = NULL;
  stv.ztra = NULL;
  stv.wtra = NULL;
  stv.neg = FALSE;

  if(!spec)
    return stv;

  if(level >= NUM_LEVELS)
    {
      fprintf(stderr, "Error, too many nested quantifiers: %s\n", spec->debug);
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
            perror(filename);
            exit_failure();
          }

        memset(cb->source, 0, SOURCE_BUFSIZE);

        if(fread(cb->source, SOURCE_BUFSIZE, sizeof(char), fp) == SOURCE_BUFSIZE)
          {
            fprintf(stderr, "%s: Error, source file too long\n", filename);
            exit_failure();
          }

        if(ferror(fp))
          {
            perror(filename);
            exit_failure();
          }

        fclose(fp);

        printf("\tCompiling module %s ...\n", spec->symbol);

        e = parse(cb->source);
        if(!e)
          exit_failure();

        stv = preval(cb, e, 0, 0);

        delete_specification(e);

        printf("\t... %s ok\n", spec->symbol);
      break;

      case op_define:
        stv_2 = preval(cb, spec->right, level, param);
        stv = preval(cb, spec->left, level, stv_2.a);

        delete_specification(stv_2.btl);
      break;

      case op_cname:
        tp = name2constant(cb, spec->symbol, TRUE);
        tp->value = param;
      break;

      case op_constant:
        tp = name2constant(cb, spec->symbol, FALSE);
        if(tp)
          {
            stv.btl = create_ground(op_number, "", tp->value);
            stv.a = tp->value;
            stv.b = tp->value;
          }
        else
          {
            fprintf(stderr, "%s: Error, reference to undefined constant: %s\n", spec->symbol, spec->debug);
            exit_failure();
          }
      break;

      case op_name:
      case op_dname:
      case op_iname:
      case op_string:
        stv.btl = create_ground(spec->ot, spec->symbol, 0);
      break;

      case op_number:
      case op_ioqual1:
      case op_ioqual2:
      case op_ioqual3:
      case op_ioqual4:
        stv.btl = create_ground(spec->ot, "", spec->value);
        stv.a = spec->value;
        stv.b = spec->value;
      break;

      case op_iterator:
        l = level - strlen(spec->symbol);
        if(l < 0)
          {
            fprintf(stderr, "%s: Error, iterator symbol out of scope: %s\n", spec->symbol, spec->debug);
            exit_failure();
          }

        stv.btl = create_ground(op_number, "", cb->iterator[l]);
        stv.a = cb->iterator[l];
        stv.b = cb->iterator[l];
      break;

      case op_vector:
        stv = preval(cb, spec->left, level, param);
        stv_2 = preval(cb, spec->right, level, param);

        sprintf(symbol, "%s("TIME_FMT")", stv.btl->symbol, stv_2.a);
        ot = stv.btl->ot;
 
        delete_specification(stv.btl);
        delete_specification(stv_2.btl);

        stv.btl = create_ground(ot, symbol, 0);
      break;

      case op_matrix:
        stv = preval(cb, spec->left, level, param);
        stv_2 = preval(cb, spec->right, level, param);

        sprintf(symbol, "%s("TIME_FMT","TIME_FMT")", stv.btl->symbol, stv_2.a, stv_2.b);
        ot = stv.btl->ot;
 
        delete_specification(stv.btl);
        delete_specification(stv_2.btl);

        stv.btl = create_ground(ot, symbol, 0);
      break;

      case op_and:
      case op_or:
      case op_delay:
        stv = preval(cb, spec->left, level, param);
        stv_2 = preval(cb, spec->right, level, param);

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
        stv = preval(cb, spec->left, level, param);
        stv_2 = preval(cb, spec->right, level, param);

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

      case op_input:
      case op_output:
        stv = preval(cb, spec->left, level, param);
        stv_2 = preval(cb, spec->right, level, param);

        sprintf(debug, "%s[%%2$s] %%1$s", opname(spec->ot));
        stv.btl = create_operation(spec->ot, stv.btl, stv_2.btl, debug);
      break;

      case op_var_at:
        stv = preval(cb, spec->left, level, param);
        stv_2 = preval(cb, spec->right, level, param);

        if(stv_2.a > stv_2.b)
          {
            fprintf(stderr, TIME_FMT", "TIME_FMT": Error, empty interval in initial conditions: %s\n", stv_2.a, stv_2.b, spec->debug);
            exit_failure();
          }

        q = create_operation(op_var_at, stv.btl, create_ground(op_number, "", stv_2.a), "(%s @ %s)");

        for(k = stv_2.a + 1; k <= stv_2.b; k++)
          q = create_operation(op_join, q,
                                         create_operation(op_var_at, copy_specification(stv.btl),
                                                                     create_ground(op_number, "", k), "(%s @ %s)"), "%s , %s");
        stv.btl = q;

        delete_specification(stv_2.btl);
      break;

      case op_not:
        stv = preval(cb, spec->left, level, param);

        stv.btl = create_operation(op_not, stv.btl, NULL, "(~ %s)");
      break;

      case op_aux:
      case op_init:
        stv = preval(cb, spec->left, level, param);

        sprintf(debug, "%s %%s", opname(spec->ot));
        stv.btl = create_operation(spec->ot, stv.btl, NULL, debug);
      break;

      case op_plus:
        stv = preval(cb, spec->left, level, param);
        stv_2 = preval(cb, spec->right, level, param);

        val = stv.a + stv_2.a;

        delete_specification(stv.btl);
        delete_specification(stv_2.btl);

        stv.btl = create_ground(op_number, "", val);
        stv.a = val;
        stv.b = val;
      break;

      case op_minus:
        stv = preval(cb, spec->left, level, param);
        stv_2 = preval(cb, spec->right, level, param);

        val = stv.a - stv_2.a;

        delete_specification(stv.btl);
        delete_specification(stv_2.btl);

        stv.btl = create_ground(op_number, "", val);
        stv.a = val;
        stv.b = val;
      break;

      case op_mul:
        stv = preval(cb, spec->left, level, param);
        stv_2 = preval(cb, spec->right, level, param);

        val = stv.a * stv_2.a;

        delete_specification(stv.btl);
        delete_specification(stv_2.btl);

        stv.btl = create_ground(op_number, "", val);
        stv.a = val;
        stv.b = val;
      break;

      case op_div:
        stv = preval(cb, spec->left, level, param);
        stv_2 = preval(cb, spec->right, level, param);

        if(!stv_2.a)
          {
            fprintf(stderr, "Error, division by zero: %s\n", spec->debug);
            exit_failure();
          }

        val = stv.a / stv_2.a;

        delete_specification(stv.btl);
        delete_specification(stv_2.btl);

        stv.btl = create_ground(op_number, "", val);
        stv.a = val;
        stv.b = val;
      break;

      case op_mod:
        stv = preval(cb, spec->left, level, param);
        stv_2 = preval(cb, spec->right, level, param);

        if(!stv_2.a)
          {
            fprintf(stderr, "Error, division by zero: %s\n", spec->debug);
            exit_failure();
          }

        val = stv.a % stv_2.a;

        delete_specification(stv.btl);
        delete_specification(stv_2.btl);

        stv.btl = create_ground(op_number, "", val);
        stv.a = val;
        stv.b = val;
      break;

      case op_pow:
        stv = preval(cb, spec->left, level, param);
        stv_2 = preval(cb, spec->right, level, param);

        val = pow(stv.a, stv_2.a);

        delete_specification(stv.btl);
        delete_specification(stv_2.btl);

        stv.btl = create_ground(op_number, "", val);
        stv.a = val;
        stv.b = val;
      break;

      case op_chs:
        stv = preval(cb, spec->left, level, param);

        val = - stv.a;

        delete_specification(stv.btl);

        stv.btl = create_ground(op_number, "", val);
        stv.a = val;
        stv.b = val;
      break;

      case op_interval_1:
        stv = preval(cb, spec->left, level, param);
        stv_2 = preval(cb, spec->right, level, param);

        stv.b = stv_2.a;

        delete_specification(stv_2.btl);
      break;

      case op_interval_2:
        stv = preval(cb, spec->left, level, param);
        stv_2 = preval(cb, spec->right, level, param);

        stv.a++;
        stv.b = stv_2.a;

        delete_specification(stv_2.btl);
      break;

      case op_interval_3:
        stv = preval(cb, spec->left, level, param);
        stv_2 = preval(cb, spec->right, level, param);

        stv.b = stv_2.a - 1;

        delete_specification(stv_2.btl);
      break;

      case op_interval_4:
        stv = preval(cb, spec->left, level, param);
        stv_2 = preval(cb, spec->right, level, param);

        stv.a++;
        stv.b = stv_2.a - 1;

        delete_specification(stv_2.btl);
      break;

      case op_at:
        stv = at_happen(cb, spec, level, param, FALSE);
      break;

      case op_happen:
        stv = at_happen(cb, spec, level, param, TRUE);
      break;

      case op_since:
        stv = since_until(cb, spec, level, param, FALSE);
      break;

      case op_until:
        stv = since_until(cb, spec, level, param, TRUE);
      break;

      case op_imply:
        stv = preval(cb, spec->left, level, param);
        stv_2 = preval(cb, spec->right, level, param);

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
        stv = preval(cb, spec->left, level, param);
        stv_2 = preval(cb, spec->right, level, param);

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
        stv_2 = preval(cb, spec->right, level, param);

        start = stv_2.a;
        end = stv_2.b;

        if(start < 0 || end < start)
          {
            fprintf(stderr, TIME_FMT" : "TIME_FMT": Error, iteration parameter out of range in <forall> construct: %s\n", stv_2.a, stv_2.b, spec->debug);
            exit_failure();
          }

        cb->iterator[level] = start;

        stv_2 = preval(cb, spec->right, level, stv_2.a);
        stv = preval(cb, spec->left, level + 1, param);

        p = stv.btl;
        q = stv.btldef;

        for(k = start + 1; k <= end; k++)
          {
            cb->iterator[level] = k;

            stv_2 = preval(cb, spec->right, level, k);
            stv = preval(cb, spec->left, level + 1, param);

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
      break;

      case op_exists:
        stv_2 = preval(cb, spec->right, level, param);

        start = stv_2.a;
        end = stv_2.b;

        if(start < 0 || end < start)
          {
            fprintf(stderr, TIME_FMT" : "TIME_FMT": Error, iteration parameter out of range in <exists> construct: %s\n", stv_2.a, stv_2.b, spec->debug);
            exit_failure();
          }

        cb->iterator[level] = start;

        stv_2 = preval(cb, spec->right, level, stv_2.a);
        stv = preval(cb, spec->left, level + 1, param);

        p = stv.btl;
        q = stv.btldef;

        for(k = start + 1; k <= end; k++)
          {
            cb->iterator[level] = k;

            stv_2 = preval(cb, spec->right, level, k);
            stv = preval(cb, spec->left, level + 1, param);

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
      break;

      case op_one:
        stv_2 = preval(cb, spec->right, level, param);

        start = stv_2.a;
        end = stv_2.b;

        cb->iterator[level] = start;

        stv_2 = preval(cb, spec->right, level, stv_2.a);
        stv = preval(cb, spec->left, level + 1, param);

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

            stv_2 = preval(cb, spec->right, level, k);
            stv = preval(cb, spec->left, level + 1, param);

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
      break;

      case op_one_check:
        stv_2 = preval(cb, spec->right, level, param);

        if(stv_2.a < 0 || stv_2.b < stv_2.a)
          {
            fprintf(stderr, TIME_FMT" : "TIME_FMT": Error, iteration parameter out of range in <one> construct: %s\n", stv_2.a, stv_2.b, spec->debug);
            exit_failure();
          }

        stv = preval(cb, spec->left, level, param);

        if(stv.a < 0 || stv.b < stv.a)
          {
            fprintf(stderr, TIME_FMT" : "TIME_FMT": Error, selection parameter out of range in <one> construct: %s\n", stv.a, stv.b, spec->debug);
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
        stv_2 = preval(cb, spec->right, level, param);

        start = stv_2.a;
        end = stv_2.b;

        if(start < 0 || end < start)
          {
            fprintf(stderr, TIME_FMT" : "TIME_FMT": Error, iteration parameter out of range in <unique> construct: %s\n", stv_2.a, stv_2.b, spec->debug);
            exit_failure();
          }

        p = NULL;
        q = NULL;
        r = NULL;
        for(h = start; h <= end; h++)
          {
            cb->iterator[level] = start;

            stv_2 = preval(cb, spec->right, level, stv_2.a);
            stv = preval(cb, spec->left, level + 1, param);

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

                stv_2 = preval(cb, spec->right, level, k);
                stv = preval(cb, spec->left, level + 1, param);

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
      break;

      case op_code:
        stv_2 = preval(cb, spec->right, level, param);
        l = strlen(stv_2.btl->symbol);

        p = NULL;
        n = 0;
        for(h = 0; h < l; h++)
          {
            stv = preval(cb, spec->left, level + 1, param);

            if(strlen(stv.btl->symbol) >= MAX_NAMELEN)
              {
                fprintf(stderr, "%s: Error, symbol name too long in <code> construct: %s\n", stv.btl->symbol, spec->debug);
                exit_failure();
              }

            sprintf(symbol, "%s("TIME_FMT",0)", stv.btl->symbol, n);
            ot = stv.btl->ot;

            delete_specification(stv.btl);

            stv.btl = create_ground(ot, symbol, 0);

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
                stv = preval(cb, spec->left, level + 1, param);

                sprintf(symbol, "%s("TIME_FMT","TIME_FMT")", stv.btl->symbol, n, k);
                ot = stv.btl->ot;

                delete_specification(stv.btl);

                stv.btl = create_ground(ot, symbol, 0);

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
        stv_2 = preval(cb, spec->right, level, param);

        if(stv_2.btl->value < 0)
          {
            fprintf(stderr, TIME_FMT": Error, number out of range in <code> construct: %s\n", stv_2.btl->value, spec->debug);
            exit_failure();
          }

        l = 0;
        for(h = sizeof(d_time); h >= 0; h--)
          if(((char *)&(stv_2.btl->value))[h])
            {
              for(l = 0; l <= h; l++)
                stv_2.btl->symbol[l] = ((char *)&(stv_2.btl->value))[l];

              break;
            }

        p = NULL;
        for(h = 0; h < l; h++)
          {
            stv = preval(cb, spec->left, level + 1, param);

            if(strlen(stv.btl->symbol) >= MAX_NAMELEN)
              {
                fprintf(stderr, "%s: Error, symbol name too long in <code> construct: %s\n", stv.btl->symbol, spec->debug);
                exit_failure();
              }

            sprintf(symbol, "%s("TIME_FMT",0)", stv.btl->symbol, h);
            ot = stv.btl->ot;

            delete_specification(stv.btl);

            stv.btl = create_ground(ot, symbol, 0);

            if(stv_2.btl->symbol[h] & 1)
              r = stv.btl;
            else
              r = create_operation(op_not, copy_specification(stv.btl), NULL, "(~ %s)");

            for(k = 1; k < 8; k++)
              {
                stv = preval(cb, spec->left, level + 1, param);

                sprintf(symbol, "%s("TIME_FMT","TIME_FMT")", stv.btl->symbol, h, k);
                ot = stv.btl->ot;

                delete_specification(stv.btl);

                stv.btl = create_ground(ot, symbol, 0);

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

      case op_iter:
        stv_2 = preval(cb, spec->right, level, param);

        start = stv_2.a;
        end = stv_2.b;

        if(start < 0 || end < start)
          {
            fprintf(stderr, TIME_FMT" : "TIME_FMT": Error, iteration parameter out of range in <iter> construct: %s\n", stv_2.a, stv_2.b, spec->debug);
            exit_failure();
          }

        cb->iterator[level] = start;

        stv_2 = preval(cb, spec->right, level, stv_2.a);
        stv = preval(cb, spec->left, level + 1, param);

        p = stv.btl;
        q = stv.btldef;

        for(k = start + 1; k <= end; k++)
          {
            cb->iterator[level] = k;

            stv_2 = preval(cb, spec->right, level, k);
            stv = preval(cb, spec->left, level + 1, param);

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
      break;

      case op_when:
        if(level == 0)
          {
            fprintf(stderr, "Error, <when> construct out of scope: %s\n", spec->debug);
            exit_failure();
          }

        stv_2 = preval(cb, spec->right, level, param);

        if(stv_2.a < 0 || stv_2.b < stv_2.a)
          {
            fprintf(stderr, TIME_FMT" : "TIME_FMT": Error, selection parameter out of range in <when> construct: %s\n", stv_2.a, stv_2.b, spec->debug);
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
          stv = preval(cb, spec->left, level, param);

        delete_specification(stv_2.btl);
      break;

      case op_in:
        stv = preval(cb, spec->left, level, param);
        stv_2 = preval(cb, spec->right, level, param);

        stv.a = stv_2.a;
        stv.b = stv_2.b;
        stv.xtra = &stv.btl->value;

        delete_specification(stv_2.btl);
      break;

      case op_neg_range:
        stv = preval(cb, spec->left, level, param);
        stv.neg = !stv.neg;
      break;

      default:
        assert(FALSE);
      break;
    }

  return stv;
}

subtreeval at_happen(c_base *cb, btl_specification *spec, int level, int param, bool dual)
{
  subtreeval stv, stv_2;
  char symbol[MAX_NAMELEN], symbol1[MAX_NAMELEN], symbol2[MAX_NAMELEN], symbol_h[BTL_HISTORY_LEN][MAX_NAMELEN];
  btl_specification *newbtl, *p, *q, *p1, *q1, *r, *btl_history[BTL_HISTORY_LEN], *btl_history1[BTL_HISTORY_LEN];
  int h, n, tail;

  for(h = 0; h < BTL_HISTORY_LEN; h++)
    {
      btl_history[h] = NULL;
      btl_history1[h] = NULL;
    }

  stv = preval(cb, spec->left, level, param);
  stv_2 = preval(cb, spec->right, level, param);

  if(stv_2.a > stv_2.b)
    {
      fprintf(stderr, TIME_FMT", "TIME_FMT": Error, empty interval after <%c>: %s\n", stv_2.a, stv_2.b, dual? '?' : '@', spec->debug);
      exit_failure();
    }

  if(cb->seplit_fe)
    {
      tail = stv_2.b - stv_2.a;
      r = NULL;

      gensym(cb, symbol, dual? "HP" : "AT", asserted, FALSE);
      gensym(cb, symbol1, dual? "HP" : "AT", negated, FALSE);
      gensym(cb, symbol2, dual? "HP" : "AT", undefined, TRUE);

      btl_history[0] = create_ground(op_name, symbol, 0);
      btl_history1[0] = create_operation(op_not, create_ground(op_name, symbol1, 0), NULL, "(~ %s)");
      strcpy(symbol_h[0], symbol2);

      newbtl = create_operation(op_and, CREATE_IMPLY(btl_history[0], stv.btl), CREATE_IMPLY(copy_specification(stv.btl), btl_history1[0]), "(%s & %s)");

      do
        {
          p = btl_history[0];
          p1 = btl_history1[0];

          n = 1;
          h = 0;

          while(tail >= n)
            {
              h++;
              if(h >= BTL_HISTORY_LEN)
                {
                  fprintf(stderr, "Error, interval too large: %s\n", spec->debug);
                  exit_failure();
                }

              if(btl_history[h])
                {
                  p = btl_history[h];
                  p1 = btl_history[h];
                }
              else
                {
                  q = create_operation(dual? op_or : op_and, copy_specification(p),
                                                             create_operation(op_delay, copy_specification(p),
                                                                                        create_ground(op_number, "", - n), "(%s @ %s)"), dual? "(%s | %s)" : "(%s & %s)");

                  q1 = create_operation(dual? op_or : op_and, copy_specification(p1),
                                                              create_operation(op_delay, copy_specification(p1),
                                                                                         create_ground(op_number, "", - n), "(%s @ %s)"), dual? "(%s | %s)" : "(%s & %s)");

                  gensym(cb, symbol, dual? "HP" : "AT", asserted, FALSE);
                  gensym(cb, symbol1, dual? "HP" : "AT", negated, FALSE);
                  gensym(cb, symbol2, dual? "HP" : "AT", undefined, TRUE);

                  p = create_ground(op_name, symbol, 0);
                  p1 = create_operation(op_not, create_ground(op_name, symbol1, 0), NULL, "(~ %s)");
                  btl_history[h] = p;
                  btl_history1[h] = p1;
                  strcpy(symbol_h[h], symbol2);

                  newbtl = create_operation(op_and, newbtl, create_operation(op_and, CREATE_IMPLY(p, q), CREATE_IMPLY(q1, p1), "(%s & %s)"), "%s ; %s");
                }

              tail -= n;
              n *= 2;
            }

          q = create_operation(op_delay, create_ground(op_name, symbol_h[h], 0), create_ground(op_number, "", stv_2.b), "(%s @ %s)");

          if(r)
            r = create_operation(dual? op_or : op_and, q, r, dual? "(%s | %s)" : "(%s & %s)");
          else
            r = q;

          stv_2.b = stv_2.a + tail;
        }
      while(tail > 0);

      stv.btl = r;
    }
  else
    {
      tail = stv_2.b - stv_2.a;
      r = NULL;

      gensym(cb, symbol, dual? "HP" : "AT", asserted, TRUE);

      btl_history[0] = create_ground(op_name, symbol, 0);

      newbtl = CREATE_EQV(btl_history[0], stv.btl);

      do
        {
          p = btl_history[0];

          n = 1;
          h = 0;

          while(tail >= n)
            {
              h++;
              if(h >= BTL_HISTORY_LEN)
                {
                  fprintf(stderr, "Error, interval too large: %s\n", spec->debug);
                  exit_failure();
                }

              if(btl_history[h])
                p = btl_history[h];
              else
                {
                  q = create_operation(dual? op_or : op_and, copy_specification(p),
                                                             create_operation(op_delay, copy_specification(p),
                                                                                        create_ground(op_number, "", - n), "(%s @ %s)"), dual? "(%s | %s)" : "(%s & %s)");

                  gensym(cb, symbol, dual? "HP" : "AT", asserted, TRUE);

                  p = create_ground(op_name, symbol, 0);
                  btl_history[h] = p;

                  newbtl = create_operation(op_and, newbtl, CREATE_EQV(p, q), "%s ; %s");
                }

              tail -= n;
              n *= 2;
            }

          q = create_operation(op_delay, copy_specification(p), create_ground(op_number, "", stv_2.b), "(%s @ %s)");

          if(r)
            r = create_operation(dual? op_or : op_and, q, r, dual? "(%s | %s)" : "(%s & %s)");
          else
            r = q;

          stv_2.b = stv_2.a + tail;
        }
      while(tail > 0);

      stv.btl = r;
    }

  if(stv.btldef)
    stv.btldef = create_operation(op_and, newbtl, stv.btldef, "%s ; %s");
  else
    stv.btldef = newbtl;

  delete_specification(stv_2.btl);

  return stv;
}

subtreeval since_until(c_base *cb, btl_specification *spec, int level, int param, bool dual)
{
  subtreeval stv, stv_2;
  char symbol[MAX_NAMELEN], symbol1[MAX_NAMELEN], symbol2[MAX_NAMELEN];
  btl_specification *newbtl, *p, *q, *p1, *q1;

  stv = preval(cb, spec->left, level, param);
  stv_2 = preval(cb, spec->right, level, param);

  if(cb->seplit_su)
    {
      gensym(cb, symbol, dual? "UT" : "SN", asserted, FALSE);
      gensym(cb, symbol1, dual? "UT" : "SN", negated, FALSE);

      p = create_ground(op_name, symbol, 0);
      p1 = create_operation(op_not, create_ground(op_name, symbol1, 0), NULL, "(~ %s)");

      q = create_operation(op_or, stv.btl,
                                  create_operation(op_and, stv_2.btl,
                                                           create_operation(op_delay, copy_specification(p),
                                                                                      create_ground(op_number, "", dual? 1 : -1), "(%s @ %s)"), "(%s & %s)"), "(%s | %s)");
      q1 = create_operation(op_or, copy_specification(stv.btl),
                                   create_operation(op_and, copy_specification(stv_2.btl),
                                                            create_operation(op_delay, copy_specification(p1),
                                                                                       create_ground(op_number, "", dual? 1 : -1), "(%s @ %s)"), "(%s & %s)"), "(%s | %s)");

      newbtl = create_operation(op_and, CREATE_IMPLY(p, q), CREATE_IMPLY(q1, p1), "(%s & %s)");

      gensym(cb, symbol2, dual? "UT" : "SN", undefined, TRUE);

      stv.btl = create_ground(op_name, symbol2, 0);
    }
  else
    {
      gensym(cb, symbol, dual? "UT" : "SN", asserted, TRUE);

      p = create_ground(op_name, symbol, 0);
      q = create_operation(op_or, stv.btl,
                           create_operation(op_and, stv_2.btl,
                                                    create_operation(op_delay, create_ground(op_name, symbol, 0),
                                                                               create_ground(op_number, "", dual? 1 : -1), "(%s @ %s)"), "(%s & %s)"), "(%s | %s)");
      newbtl = CREATE_EQV(p, q);

      stv.btl = create_ground(op_name, symbol, 0);
    }

  if(stv.btldef)
    {
      if(stv_2.btldef)
        stv.btldef = create_operation(op_and, newbtl, create_operation(op_and, stv.btldef, stv_2.btldef, "%s ; %s"), "%s ; %s");
      else
        stv.btldef = create_operation(op_and, newbtl, stv.btldef, "%s ; %s");
    }
  else
    {
      if(stv_2.btldef)
        stv.btldef = create_operation(op_and, newbtl, stv_2.btldef, "%s ; %s");
      else
        stv.btldef = newbtl;
    }

  return stv;
}

subtreeval eval(c_base *cb, btl_specification *spec, smallnode *vp, bool neg, io_class sclass, io_type stype, io_type_2 packed, io_type_3 defaultval, io_type_4 omissions, d_time t)
{
  subtreeval stv, stv_2;
  smallnode *wp;
  io_signal *sp;
  char name[MAX_NAMELEN];
  int idx, idx_2;

  stv.vp = NULL;
  stv.a = 0;
  stv.b = 0;
  stv.xtra = NULL;
  stv.ytra = NULL;
  stv.ztra = NULL;
  stv.wtra = NULL;

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
            fprintf(stderr, "%s: Error, reference to undeclared signal: %s\n", spec->symbol, spec->debug);
            exit_failure();
          }
      break;

      case op_dname:
        sp = name2signal(cb, spec->symbol, TRUE);

        if(sp->sclass != internal_class)
          {
            fprintf(stderr, "%s: Error, duplicate declaration of signal: %s\n", spec->symbol, spec->debug);
            exit_failure();
          }

        sp->sclass = sclass;
        sp->stype = stype;

        if(!packed)
          {
            sp->packed = 0;
            sp->packedbit = 0;
          }
        else
          {
            idx = -1;
            idx_2 = -1;

            sscanf(sp->name, VARNAME_FMT" ( %d , %d )", name, &idx, &idx_2);

            if(idx_2 < 0)
              sscanf(sp->name, VARNAME_FMT" ( %d )", name, &idx);
            else
              idx = 8 * idx + idx_2;

            if(idx < 0)
              {
                fprintf(stderr, "%s: Error, packed I/O signal should be a vector or matrix: %s\n", spec->symbol, spec->debug);
                exit_failure();
              }

            strcpy(sp->root, name);

            sp->packed = 1 + name2group(cb, name, TRUE)->group_id;
            sp->packedbit = idx;
          }

        sp->defaultval = defaultval;
        sp->omissions = omissions;
      break;

      case op_iname:
        add_ic(cb, spec->symbol, neg, t);
      break;

      case op_number:
        stv.a = spec->value;
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

      case op_join:
        stv = eval(cb, spec->left, vp, neg, sclass, stype, packed, defaultval, omissions, t);
        stv_2 = eval(cb, spec->right, vp, neg, sclass, stype, packed, defaultval, omissions, t);

        if(stv.vp && stv_2.vp)
          {
            wp = create_smallnode(cb, neg? gate : joint);
            if(!wp)
              {
                perror(NULL);
                exit_failure();
              }

            wp->up = vp;
            wp->left = stv.vp;
            wp->right = stv_2.vp;

            stv.vp->up = wp;
            stv_2.vp->up = wp;

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
        stv = eval(cb, spec->left, vp, neg, sclass, stype, packed, defaultval, omissions, t);
        stv_2 = eval(cb, spec->right, vp, neg, sclass, stype, packed, defaultval, omissions, t);

        if(stv_2.xtra)
          {
            if(stv.xtra)
              {
                fprintf(stderr, "Error, repeated or conflicting any/ipc/file qualifiers: %s\n", spec->debug);
                exit_failure();
              }

            stv.xtra = stv_2.xtra;
          }

        if(stv_2.ytra)
          {
            if(stv.ytra)
              {
                fprintf(stderr, "Error, repeated or conflicting binary/packed qualifiers: %s\n", spec->debug);
                exit_failure();
              }

            stv.ytra = stv_2.ytra;
          }

        if(stv_2.ztra)
          {
            if(stv.ztra)
              {
                fprintf(stderr, "Error, repeated or conflicting true/false/unknown qualifiers: %s\n", spec->debug);
                exit_failure();
              }

            stv.ztra = stv_2.ztra;
          }

        if(stv_2.wtra)
          {
            if(stv.wtra)
              {
                fprintf(stderr, "Error, repeated or conflicting raw/filter/omit qualifiers: %s\n", spec->debug);
                exit_failure();
              }

            stv.wtra = stv_2.wtra;
          }
      break;

      case op_not:
        stv = eval(cb, spec->left, vp, !neg, sclass, stype, packed, defaultval, omissions, t);
      break;

      case op_and:
        wp = create_smallnode(cb, neg? gate : joint);
        if(!wp)
          {
            perror(NULL);
            exit_failure();
          }

        wp->up = vp;
        wp->left = eval(cb, spec->left, wp, neg, sclass, stype, packed, defaultval, omissions, t).vp;
        wp->right = eval(cb, spec->right, wp, neg, sclass, stype, packed, defaultval, omissions, t).vp;

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
            perror(NULL);
            exit_failure();
          }

        wp->up = vp;
        wp->left = eval(cb, spec->left, wp, neg, sclass, stype, packed, defaultval, omissions, t).vp;
        wp->right = eval(cb, spec->right, wp, neg, sclass, stype, packed, defaultval, omissions, t).vp;

        if(neg)
          snprintf(wp->debug, DEBUG_STRLEN, "(~ %s)", spec->debug);
        else
          strcpy(wp->debug, spec->debug);

        stv.vp = wp;
      break;

      case op_delay:
        if(cb->merge && vp && vp->nclass == delay)
          {
            stv = eval(cb, spec->left, vp, neg, sclass, stype, packed, defaultval, omissions, t);
            stv.a += eval(cb, spec->right, vp, neg, sclass, stype, packed, defaultval, omissions, t).a;
          }
        else
          {
            wp = create_smallnode(cb, delay);
            if(!wp)
              {
                perror(NULL);
                exit_failure();
              }

            stv_2 = eval(cb, spec->left, wp, neg, sclass, stype, packed, defaultval, omissions, t);

            wp->up = vp;
            wp->left = stv_2.vp;
            wp->k = - (stv_2.a + eval(cb, spec->right, wp, neg, sclass, stype, packed, defaultval, omissions, t).a);

            if(cb->merge && !wp->k)
              {
                stv.vp = stv_2.vp;
                stv.vp->up = vp;

                purge_smallnode(cb, wp, vp, undefined);
              }
            else
              {
                strcpy(wp->debug, spec->debug);

                stv.vp = wp;
              }
          }
      break;

      case op_var_at:
        stv = eval(cb, spec->left, vp, neg, sclass, stype, packed, defaultval, omissions, eval(cb, spec->right, vp, neg, sclass, stype, packed, defaultval, omissions, t).a);
      break;

      case op_aux:
        stv = eval(cb, spec->left, vp, neg, aux_class, stype, packed, defaultval, omissions, t);
      break;

      case op_input:
        stv_2 = eval(cb, spec->right, vp, neg, sclass, stype, packed, defaultval, omissions, t);
        stv = eval(cb, spec->left, vp, neg, input_class, stv_2.xtra? *stv_2.xtra : io_any, stv_2.ytra? *stv_2.ytra : io_binary,
                   stv_2.ztra? *stv_2.ztra : io_unknown, stv_2.wtra? *stv_2.wtra : io_raw, t);
      break;

      case op_output:
        stv_2 = eval(cb, spec->right, vp, neg, sclass, stype, packed, defaultval, omissions, t);
        stv = eval(cb, spec->left, vp, neg, output_class, stv_2.xtra? *stv_2.xtra : io_any, stv_2.ytra? *stv_2.ytra : io_binary,
                   stv_2.ztra? *stv_2.ztra : io_unknown, stv_2.wtra? *stv_2.wtra : io_raw, t);

      break;

      case op_init:
        stv = eval(cb, spec->left, vp, neg, sclass, stype, packed, defaultval, omissions, t);
      break;

      default:
        assert(FALSE);
      break;
    }

  return stv;
}

smallnode **get_neighbor_handle(smallnode *vp, smallnode *wp)
{
  smallnode **zpp;

  assert(vp && wp);

  zpp = NULL;

  if(vp == *genup(wp))
    zpp = genup(wp);
  else
    if(vp == wp->left)
      zpp = &wp->left;
    else
      if(vp == wp->right)
        zpp = &wp->right;
      else
        assert(FALSE);

  if(!zpp)
    {
      fprintf(stderr, "%s: Internal error in pruning network (2): %s\n", vp->name, vp->debug);
      exit_failure();
    }

  return zpp;
}

smallnode **gendir(smallnode *vp, smallnode *bp, direction dir)
{
  smallnode **zpp;

  assert(vp && !vp->zombie);

  zpp = NULL;

  switch(dir)
    {
      case dir_left:
        if(bp == *genup(vp))
          {
            if(bp == vp->left)
              zpp = &vp->right;
            else
              zpp = &vp->left;
          }
        else
          if(bp == vp->right)
             zpp = genup(vp);
          else
            if(bp == vp->left)
              zpp = &vp->right;
            else
              assert(FALSE);
      break;

      case dir_right:
        if(bp == *genup(vp))
          {
            if(bp == vp->left)
              zpp = &vp->left;
            else
              zpp = &vp->right;
          }
        else
          if(bp == vp->right)
            zpp = &vp->left;
          else
            if(bp == vp->left)
              zpp = genup(vp);
            else
              assert(FALSE);
      break;

      default:
        assert(FALSE);
      break;
    }

  if(!zpp)
    {
      fprintf(stderr, "%s: Internal error in pruning network (1): %s\n", vp->name, vp->debug);
      exit_failure();
    }

  return zpp;
}

link_code occurrence(smallnode *from, smallnode *to)
{
  if(!from || !to)
    return no_link;
  else
    {
      if(from == *genup(to))
        return parent;
      else
        if(from == to->left)
          return left_son;
        else
          if(from == to->right)
            return right_son;
          else
            return no_link;
    }
}

link_code occurrence_neg(smallnode *from, smallnode *to)
{
  if(!from || !to)
    return no_link;
  else
    {
      if(from == to->right)
        return right_son;
      else
        if(from == to->left)
          return left_son;
        else
          if(from == *genup(to))
            return parent;
          else
            return no_link;
    }
}

void purge_smallnode(c_base *cb, smallnode *vp, smallnode *bp, litval val)
{
  io_signal *sp;
  smallnode *lp, *rp;
  int i;
  io_type_3 def;

  assert(vp);
  assert(!vp->zombie);

  lp = *gendir(vp, bp, dir_left);
  rp = *gendir(vp, bp, dir_right);

  for(i = 0; i < cb->num_signals; i++)
    {
      sp = &cb->sigtab[i];
      assert(sp);

      if(sp->from == vp || sp->to == vp)
        {
          if(val != undefined)
            {
              if(sp->to == vp && (sp->from != vp || sp->occurr == parent))
                def = (val == negated)? io_true : io_false;
              else
                def = (val == negated)? io_false : io_true;

              if(sp->defaultval != io_unknown && sp->defaultval != def)
                {
                  sp->omissions = io_raw;

                  fprintf(stderr, "%s: Warning, incompatible default on signal\n", sp->name);
                }

              sp->defaultval = def;
              sp->val = val;
            }
          else
            {
              if(sp->from == vp)
                {
                  if(sp->to == lp)
                    {
                      sp->from = rp;

                      if(sp->occurr == parent)
                        {
                          sp->occurr = occurrence(vp, lp);
                          sp->occurr_neg = occurrence_neg(vp, rp);
                        }
                      else
                        {
                          sp->occurr = occurrence_neg(vp, lp);
                          sp->occurr_neg = occurrence(vp, rp);
                        }
                    }
                  else
                    if(sp->to == rp)
                      {
                        sp->from = lp;

                        if(sp->occurr == parent)
                          {
                            sp->occurr = occurrence(vp, rp);
                            sp->occurr_neg = occurrence_neg(vp, lp);
                          }
                        else
                          {
                            sp->occurr = occurrence_neg(vp, rp);
                            sp->occurr_neg = occurrence(vp, lp);
                          }
                      }
                }

              if(sp->to == vp)
                {
                  if(sp->from == lp)
                    {
                      sp->to = rp;

                      if(sp->occurr != parent)
                        {
                          sp->occurr = occurrence(vp, rp);
                          sp->occurr_neg = occurrence_neg(vp, lp);
                        }
                      else
                        {
                          sp->occurr = occurrence_neg(vp, rp);
                          sp->occurr_neg = occurrence(vp, lp);
                        }
                    }
                  else
                    if(sp->from == rp)
                      {
                        sp->to = lp;

                        if(sp->occurr != parent)
                          {
                            sp->occurr = occurrence(vp, lp);
                            sp->occurr_neg = occurrence_neg(vp, rp);
                          }
                        else
                          {
                            sp->occurr = occurrence_neg(vp, lp);
                            sp->occurr_neg = occurrence(vp, rp);
                          }
                      }
                }
            }
    
          if(sp->from == vp || sp->to == vp)
            {
              sp->from = NULL;
              sp->to = NULL;

              sp->removed = TRUE;

              if(sp->sclass != internal_class)
                fprintf(stderr, "%s: Warning, %s signal removed\n", sp->name, signal_class[sp->sclass]);
            }
        }
    }

  vp->zombie = TRUE;
}

void close_smallbranches(c_base *cb, smallnode *xp, smallnode *yp, smallnode *bp)
{
  smallnode **zpp;

  if(!xp)
    {
      if(bp->open)
        erase_smalltree(cb, yp, bp);
      else
        purge_smalltree(cb, yp, bp);

      return;
    }

  if(!yp)
    {
      if(bp->open)
        erase_smalltree(cb, xp, bp);
      else
        purge_smalltree(cb, xp, bp);

      return;
    }

  if(xp != bp)
    *get_neighbor_handle(xp, bp) = NULL;

  if(yp != bp)
    *get_neighbor_handle(yp, bp) = NULL;

  zpp = get_neighbor_handle(bp, xp);
  *zpp = NULL;

  *get_neighbor_handle(bp, yp) = xp;
  *zpp = yp;
}

void erase_smalltree(c_base *cb, smallnode *vp, smallnode *bp)
{
  smallnode **lpp, **rpp;

  if(bp)
    *get_neighbor_handle(vp, bp) = NULL;

  if(!vp)
    {
      if(bp && bp->open)
        {
          bp->open--;
          return;
        }
      else
        {
          fprintf(stderr, "%s: Error, logical contradiction between clauses: %s\n", bp->name, bp->debug);
          exit_failure();
        }
    }

  if(vp->zombie)
    return;

  switch(vp->nclass)
    {
      case gate:
        lpp = gendir(vp, bp, dir_left);
        rpp = gendir(vp, bp, dir_right);

        if(*lpp == vp && *rpp == vp)
          {
            fprintf(stderr, "%s: Error, logical contradiction between clauses: %s\n", vp->name, vp->debug);
            exit_failure();
          }

        purge_smallnode(cb, vp, bp, negated);

        erase_smalltree(cb, *lpp, vp);
        erase_smalltree(cb, *rpp, vp);
      break;

      case joint:
        lpp = gendir(vp, bp, dir_left);
        rpp = gendir(vp, bp, dir_right);

        if(bp == *genup(vp))
          {
            if(*lpp == vp && *rpp == vp)
              {
                fprintf(stderr, "%s: Error, logical contradiction between clauses: %s\n", vp->name, vp->debug);
                exit_failure();
              }

            purge_smallnode(cb, vp, bp, negated);

            erase_smalltree(cb, *lpp, vp);
            erase_smalltree(cb, *rpp, vp);
          }
        else
          {
            purge_smallnode(cb, vp, bp, undefined);

            close_smallbranches(cb, *lpp, *rpp, vp);
          }
      break;

      case delay:
        lpp = gendir(vp, bp, dir_left);
        if(!*lpp)
          lpp = gendir(vp, bp, dir_right);

        purge_smallnode(cb, vp, bp, negated);

        erase_smalltree(cb, *lpp, vp);
      break;

      default:
        assert(FALSE);
      break;
    }
}

void purge_smalltree(c_base *cb, smallnode *vp, smallnode *bp)
{
  smallnode **lpp, **rpp;

  if(bp)
    *get_neighbor_handle(vp, bp) = NULL;

  if(!vp)
    {
      if(bp && bp->open)
        {
          bp->open--;
          return;
        }
      else
        {
          fprintf(stderr, "%s: Error, logical contradiction between clauses: %s\n", bp->name, bp->debug);
          exit_failure();
        }
    }

  if(vp->zombie)
    return;

  switch(vp->nclass)
    {
      case gate:
        lpp = gendir(vp, bp, dir_left);
        rpp = gendir(vp, bp, dir_right);

        purge_smallnode(cb, vp, bp, undefined);

        close_smallbranches(cb, *lpp, *rpp, vp);
      break;

      case joint:
        lpp = gendir(vp, bp, dir_left);
        rpp = gendir(vp, bp, dir_right);

        if(bp == *genup(vp))
          {
            if(*lpp == vp && *rpp == vp)
              {
                fprintf(stderr, "%s: Error, logical contradiction between clauses: %s\n", vp->name, vp->debug);
                exit_failure();
              }

            purge_smallnode(cb, vp, bp, asserted);

            purge_smalltree(cb, *lpp, vp);
            purge_smalltree(cb, *rpp, vp);
          }
        else
          if(*lpp == *genup(vp))
            {
              purge_smallnode(cb, vp, bp, asserted);

              purge_smalltree(cb, *lpp, vp);
              erase_smalltree(cb, *rpp, vp);
            }
          else
            if(*rpp == *genup(vp))
              {
                purge_smallnode(cb, vp, bp, asserted);

                erase_smalltree(cb, *lpp, vp);
                purge_smalltree(cb, *rpp, vp);
              }
      break;

      case delay:
        lpp = gendir(vp, bp, dir_left);
        if(!*lpp)
          lpp = gendir(vp, bp, dir_right);

        purge_smallnode(cb, vp, bp, asserted);

        purge_smalltree(cb, *lpp, vp);
      break;

      default:
        assert(FALSE);
      break;
    }
}

int save_smalltree(c_base *cb, FILE *fp)
{
  smallnode *vp, *up;
  int rv;

  vp = cb->network;
  rv = 0;

  while(vp)
    {
      assert(!vp->zombie);

      up = *genup(vp);

      if(!up || !vp->left || (!vp->right && vp->nclass != delay))
        fprintf(stderr, "%s: Warning, network not closed: %s\n", vp->name, vp->debug);

      if(vp == up || vp == vp->left || vp == vp->right)
        fprintf(stderr, "%s: Warning, tight loop on node: %s\n", vp->name, vp->debug);

      switch(vp->nclass)
        {
          case gate:
          case joint:
            rv = fprintf(fp, "%s: %c ; %s, %s, %s\n",
                    vp->name, class_symbol[vp->nclass], up? up->name : "*", vp->left? vp->left->name : "*", vp->right? vp->right->name : "*");
          break;

          case delay:
            rv = fprintf(fp, "%s: %c"TIME_FMT" ; %s, %s\n",
                    vp->name, class_symbol[vp->nclass], vp->k, up? up->name : "*", vp->left? vp->left->name : "*");
          break;

          default:
            assert(FALSE);
          break;
        }

      vp = vp->vp;
    }

  return rv;
}

int save_signals(c_base *cb, FILE *fp)
{
  io_signal *sp;
  int i;
  int rv;

  rv = 0;

  for(i = 0; i < cb->num_signals; i++)
    {
      sp = &cb->sigtab[i];

      if(sp->from && sp->to)
        {
          switch(sp->sclass)
            {
              case input_class:
                rv = fprintf(fp, "! %s (%s, %s) # %d / %d, %d, %d, %d, %d\n",
                             *sp->root? sp->root : sp->name, sp->from->name, sp->to->name, sp->occurr, sp->stype, sp->packed, sp->packedbit, sp->defaultval, sp->omissions);
              break;

              case output_class:
                rv = fprintf(fp, "? %s (%s, %s) # %d / %d, %d, %d, %d, %d\n",
                             *sp->root? sp->root : sp->name, sp->from->name, sp->to->name, sp->occurr, sp->stype, sp->packed, sp->packedbit, sp->defaultval, sp->omissions);
              break;

              case aux_class:
                if(cb->outaux)
                  rv = fprintf(fp, ". %s (%s, %s) # %d / %d, %d, %d, %d, %d\n",
                             *sp->root? sp->root : sp->name, sp->from->name, sp->to->name, sp->occurr, sp->stype, sp->packed, sp->packedbit, sp->defaultval, sp->omissions);
                else
                  rv = 0;
              break;

              case internal_class:
                if(cb->outint)
                  rv = fprintf(fp, ". %s (%s, %s) # %d / %d, %d, %d, %d, %d\n",
                             *sp->root? sp->root : sp->name, sp->from->name, sp->to->name, sp->occurr, sp->stype, sp->packed, sp->packedbit, sp->defaultval, sp->omissions);
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
              rv = fprintf(fp, "? %s (*, *) # 0 / %d, %d, %d, %d, %d\n", *sp->root? sp->root : sp->name, sp->stype, sp->packed, sp->packedbit, sp->defaultval, sp->omissions);

              fprintf(stderr, "%s: Warning, constant output signal generated as %s by default (%s deduction)\n", sp->name, sp->defaultval == io_false? "false" : "true", sp->val? "strong" : "weak");
            }
          else
            if(sp->sclass != internal_class && !sp->removed)
              fprintf(stderr, "%s: Warning, %s signal not generated\n", sp->name, signal_class[sp->sclass]);
        }
    }

  return rv;
}

int save_ics(c_base *cb, FILE *fp)
{
  initial_condition *icp;
  io_signal *sp;
  int i;
  int rv;

  rv = 0;

  for(i = 0; i < cb->num_ics; i++)
    {
      icp = &cb->ictab[i];
      assert(icp);

      sp = name2signal(cb, icp->name, FALSE);

      if(sp && sp->from && sp->to)
        {
          if(!icp->neg)
            rv = fprintf(fp, "(%s, %s) # %d @ "TIME_FMT"\n", sp->from->name, sp->to->name, sp->occurr, icp->t);
          else
            rv = fprintf(fp, "(%s, %s) # %d @ "TIME_FMT"\n", sp->to->name, sp->from->name, sp->occurr_neg, icp->t);
        }
      else
        {
          fprintf(stderr, "%s: Error, initial condition refers to undeclared or removed signal\n", icp->name);
          exit_failure();
        }
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

      vp = vp->vp;
    }

  return rv;
}

void link_cotree(c_base *cb)
{
  smallnode *vp, *up, *up_2;
  io_signal *sp;
  int i, j, k;

  for(i = 0; i < cb->num_literals; i++)
    {
      for(j = 0; j < cb->symcount[i]; j++)
        {
          vp = cb->symtab[i][j];
          assert(vp);

          up = vp->up;
          up_2 = vp->up_2;
          assert(up && up_2);
          assert(up != up_2);

          assert(vp == up->left || vp == up->right);

          if(vp == up->left)
            up->left = up_2;
          else
            up->right = up_2;

          assert(vp == up_2->left || vp == up_2->right);

          if(vp == up_2->left)
            up_2->left = up;
          else
            up_2->right = up;

          for(k = 0; k < cb->num_signals; k++)
            {
              sp = &cb->sigtab[k];
              assert(sp);

              if(sp->from == vp)
                {
                  assert(sp->to == NULL);
                  assert(!vp->neg);

                  sp->from = up;
                  sp->to = up_2;
                }
              else
                if(sp->to == vp)
                  {
                    assert(sp->from == NULL);
                    assert(vp->neg);

                    sp->from = up_2;
                    sp->to = up;
                  }
            }

          vp->up = NULL;
          vp->up_2 = NULL;

          vp->zombie = TRUE;
        }
    }
}

void raise_signals(c_base *cb, smallnode *vp)
{
  smallnode *wp;
  io_signal *sp;
  bool branch;
  int k;

  assert(vp);

  for(branch = FALSE; branch <= TRUE; branch++)
    {
      if(!branch)
        wp = vp->left;
      else
        wp = vp->right;

      if(wp && vp == wp->up_2)
        {
          raise_signals(cb, wp);

          if(wp->nclass == joint)
            {
              for(k = 0; k < cb->num_signals; k++)
                {
                  sp = &cb->sigtab[k];
                  assert(sp);

                  if(sp->from == wp)
                    {
                       sp->from = vp;
                       sp->to = wp;
                    }
                  else
                    if(sp->to == wp)
                      {
                         sp->from = wp;
                         sp->to = vp;
                      }
                }
            }
        }
    }
}

smallnode *build_smalltree(c_base *cb, int i, bool neg)
{
  smallnode *vp, *xp, *yp;
  int j;

  xp = NULL;
  yp = NULL;

  for(j = 0; j < cb->symcount[i]; j++)
    {
      vp = cb->symtab[i][j];
      assert(vp);

      if(vp->neg == neg)
        {
          if(yp)
            {
              xp = create_smallnode(cb, joint);
              if(!xp)
                {
                  perror(NULL);
                  exit_failure();
                }

              xp->left = vp;
              xp->right = yp;

              vp->up_2 = xp;
              yp->up_2 = xp;

              snprintf(xp->debug, DEBUG_STRLEN, "(%s & %s)", vp->debug, yp->debug);

              if(strlen(xp->debug) == DEBUG_STRLEN - 1)
                {
                  strcpy(xp->debug, "<symbol>");
                  fprintf(stderr, "Warning, symbol table overflow in second phase\n");
                }

              yp = xp;
            }
          else
            {
              xp = vp;
              yp = xp;
            }
        }
    }

  return xp;
}

smallnode *build_twotrees(c_base *cb, int i)
{
  smallnode *vp, *wp, *xp;

  xp = create_smallnode(cb, gate);
  if(!xp)
    {
      perror(NULL);
      exit_failure();
    }

  vp = build_smalltree(cb, i, FALSE);

  if(!vp)
    {
      if(cb->symtab[i][0]->name[0] != '+' && cb->symtab[i][0]->name[0] != '-')
        fprintf(stderr, "%s: Warning, missing asserted literal for signal\n", cb->symtab[i][0]->name);

      xp->open++;
    }
  else
    vp->up_2 = xp;

  wp = build_smalltree(cb, i, TRUE);

  if(!wp)
    {
      if(cb->symtab[i][0]->name[0] != '+' && cb->symtab[i][0]->name[0] != '-')
        fprintf(stderr, "%s: Warning, missing negated literal for signal\n", cb->symtab[i][0]->name);

      xp->open++;
    }
  else
    wp->up_2 = xp;

  xp->left = vp;
  xp->right = wp;

  snprintf(xp->debug, DEBUG_STRLEN, "(%s | %s)", vp? vp->debug : "<blank>", wp? wp->debug : "<blank>");

  if(strlen(xp->debug) == DEBUG_STRLEN - 1)
    {
      strcpy(xp->debug, "<symbol>");
      fprintf(stderr, "Warning, symbol table overflow in second phase\n");
    }

  return xp;
}

smallnode *build_cotree(c_base *cb)
{
  smallnode *vp, *xp, *yp;
  int i;

  xp = NULL;
  yp = NULL;

  for(i = 0; i < cb->num_literals; i++)
    {
      vp = build_twotrees(cb, i);

      if(yp)
        {
          xp = create_smallnode(cb, joint);
          if(!xp)
            {
              perror(NULL);
              exit_failure();
            }

          xp->left = vp;
          xp->right = yp;

          vp->up_2 = xp;
          yp->up_2 = xp;

          snprintf(xp->debug, DEBUG_STRLEN, "(%s & %s)", vp->debug, yp->debug);

          if(strlen(xp->debug) == DEBUG_STRLEN - 1)
            {
              strcpy(xp->debug, "<symbol>");
              fprintf(stderr, "Warning, symbol table overflow in second phase\n");
            }

          yp = xp;
        }
      else
        {
          xp = vp;
          yp = vp;
        }
    }

  link_cotree(cb);

  raise_signals(cb, xp);

  return xp;
}

compinfo compile(char *source_name, char *base_name, char *state_name, char *xref_name, char *path, bool seplit_fe, bool seplit_su, bool merge, bool constout, bool outaux, bool outint)
{
  c_base *cb;
  btl_specification *e, *f;
  subtreeval stv;
  smallnode *cvp;
  FILE *fp, *gp, *hp, *ip;
  char source_filename[MAX_STRLEN], base_filename[MAX_STRLEN], state_filename[MAX_STRLEN], xref_filename[MAX_STRLEN];
  compinfo cperf;
  node_class nclass;

  cperf.ok = FALSE;

  strcpy(source_filename, source_name);
  strcat(source_filename, SOURCE_EXT);

  strcpy(base_filename, base_name);
  strcat(base_filename, NETWORK_EXT);

  cb = malloc(sizeof(c_base));
  if(!cb)
    {
      perror(NULL);
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
  cb->outaux = outaux;
  cb->outint = outint;

  fp = fopen(source_filename, "r");
  if(!fp)
    {
      perror(source_filename);
      free(cb);
      return cperf;
    }

  if(fread(cb->source, SOURCE_BUFSIZE, sizeof(char), fp) == SOURCE_BUFSIZE)
    {
      fprintf(stderr, "%s: Error, source file too long\n", source_filename);
      free(cb);
      return cperf;
    }

  if(ferror(fp))
    {
      perror(source_filename);
      free(cb);
      return cperf;
    }

  fclose(fp);

  printf("Compiling master %s to %s ...\n", source_filename, base_filename);

  e = parse(cb->source);
  if(!e)
    {
      free(cb);
      return cperf;
    }

  stv = preval(cb, e, 0, 0);

  delete_specification(e);

  if(stv.btldef)
    f = create_operation(op_and, stv.btl, stv.btldef, "%s ; %s");
  else
    f = stv.btl;

  printf("... %s ok\n", source_filename);

  printf("Generating network\n");

  stv = eval(cb, f, NULL, FALSE, internal_class, io_any, io_binary, io_unknown, io_raw, 0);

  delete_specification(f);

  cvp = build_cotree(cb);

  printf("Pruning network\n");

  purge_smalltree(cb, stv.vp, NULL);
  purge_smalltree(cb, cvp, NULL);

  delete_zombies(cb);

  if(!cb->network)
    fprintf(stderr, "Warning, network empty\n");

  printf("Generating object files\n");

  gp = fopen(base_filename, "w");
  if(!gp)
    {
      perror(base_filename);
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
          perror(state_filename);
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
          perror(xref_filename);
          free(cb);
          return cperf;
        }

      save_xref(cb, ip);

      fclose(ip);
    }

  delete_smalltree(cb);

  for(nclass = 0; nclass < NODE_CLASSES_NUMBER; nclass++)
    cperf.num_nodes[nclass] = cb->num_nodes[nclass];

  cperf.num_signals = cb->num_signals;
  cperf.num_ics = cb->num_ics;

  free(cb);

  cperf.edges = (3 * (cperf.num_nodes[gate] + cperf.num_nodes[joint]) + 2 * cperf.num_nodes[delay]) / 2;
  cperf.tot_nodes = cperf.num_nodes[gate] + cperf.num_nodes[joint] + cperf.num_nodes[delay];
  cperf.ok = TRUE;

  return cperf;
}

int main(int argc, char *argv[])
{
  char *source_name, *base_name, *state_name, *xref_name, *path, *option, *ext;
  char default_state_name[MAX_STRLEN], default_xref_name[MAX_STRLEN];
  bool seplit_fe, seplit_su, merge, constout, outaux, outint;
  compinfo cperf;
  int i;

  source_name = base_name = state_name = xref_name = NULL;
  path = "";
  seplit_fe = seplit_su = merge = constout = outaux = outint = FALSE;

  for(i = 1; i < argc; i++)
    {
      if(*argv[i] == '-')
        {
          option = argv[i] + 1;
          switch(*option)
            {
            case 'h':
              fprintf(stderr, "Usage: %s [-bBkuwWx] [-I state] [-o base] [-P path] [-X symbols] [source]\n",
                      argv[0]);
              exit(EXIT_SUCCESS);
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

            case 'P':
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

  if(!state_name)
    {
      state_name = default_state_name;

      strcpy(state_name, base_name);
      strcat(state_name, STATE_SUFFIX);
    }

  if(xref_name == default_xref_name)
    strcpy(xref_name, base_name);

  printf("\nTING "VER" - Temporal Inference Network Generator\n"
         "Design & coding by Andrea Giotti, 2017-2020\n\n");

  cperf = compile(source_name, base_name, state_name, xref_name, path, seplit_fe, seplit_su, merge, constout, outaux, outint);

  if(cperf.ok)
    printf("Network generated -- %d edges, %d nodes (%d gates + %d joints + %d delays), %d signals, %d initial conditions\n",
            cperf.edges, cperf.tot_nodes, cperf.num_nodes[gate], cperf.num_nodes[joint], cperf.num_nodes[delay], cperf.num_signals, cperf.num_ics);
  else
    printf("Network not generated\n");

  return cperf.ok? EXIT_SUCCESS : EXIT_FAILURE;
}


