/*
  TING - Temporal Inference Network Generator
  Design & coding by Andrea Giotti, 2017
*/

#define NDEBUG

#include "ting_core.h"
#include "ting_parser.h"
#include "ting_lexer.h"

#define VER "2.3.0"

int yyparse(btl_specification **spec, yyscan_t scanner);

const char class_symbol[NODE_CLASSES_NUMBER] = CLASS_SYMBOLS;

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

  sp = alloc_syntnode();
  if(!sp)
    {
      perror(NULL);
      exit(EXIT_FAILURE);
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
      exit(EXIT_FAILURE);
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
      exit(EXIT_FAILURE);
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
                  exit(EXIT_FAILURE);
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
                     exit(EXIT_FAILURE);
                   }

                  cb->symtab[vp->literal_id][cb->symcount[vp->literal_id]] = wp;

                  strcpy(wp->name, name);
                  wp->literal_id = vp->literal_id;

                  cb->symcount[vp->literal_id]++;

                  if(cb->symcount[vp->literal_id] >= NUM_OCCURRENCES)
                    {
                      fprintf(stderr, "%s: Error, too many occurrences for literal\n", name);
                      exit(EXIT_FAILURE);
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
              exit(EXIT_FAILURE);
            }

          cb->symtab[cb->num_literals][0] = vp;

          strcpy(vp->name, name);
          vp->literal_id = cb->num_literals;

          cb->symcount[cb->num_literals] = 1;
          cb->num_literals++;

          if(cb->num_literals >= NUM_LITERALS)
            {
              fprintf(stderr, "Error, too many literals in formula\n");
              exit(EXIT_FAILURE);
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
                  exit(EXIT_FAILURE);
                }
            }
          else
            return &cb->sigtab[sp->signal_id];
        }

      if(create)
        {
          sp = &cb->sigtab[cb->num_signals];

          strcpy(sp->name, name);
          sp->sclass = internal_class;
          sp->signal_id = cb->num_signals;

          cb->num_signals++;

          if(cb->num_signals >= NUM_LITERALS)
            {
              fprintf(stderr, "Error, too many signals declared\n");
              exit(EXIT_FAILURE);
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
                  exit(EXIT_FAILURE);
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
              exit(EXIT_FAILURE);
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

void add_ic(c_base *cb, char *name, bool neg, d_time t)
{
  strcpy(cb->ictab[cb->num_ics].name, name);
  cb->ictab[cb->num_ics].neg = neg;
  cb->ictab[cb->num_ics].t = t;

  cb->num_ics++;

  if(cb->num_ics >= NUM_ICS)
    {
      fprintf(stderr, "Error, too many initial conditions\n");
      exit(EXIT_FAILURE);
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
  char symbol[MAX_NAMELEN], symbol1[MAX_NAMELEN], symbol2[MAX_NAMELEN], symbol_h[BTL_HISTORY_LEN][MAX_NAMELEN];
  char debug[MAX_STRLEN], filename[MAX_STRLEN];
  btl_specification *e, *newbtl, *p, *q, *p1, *q1, *r, *btl_history[BTL_HISTORY_LEN], *btl_history1[BTL_HISTORY_LEN];
  constant *tp;
  d_time val;
  int h, k, l, n, tail;
  op_type ot;
  FILE *fp;

  stv.btl = NULL;
  stv.btldef = NULL;
  stv.a = 0;
  stv.b = 0;

  if(!spec)
    return stv;

  if(level >= NUM_LEVELS)
    {
      fprintf(stderr, "Error, too many nested quantifiers: %s\n", spec->debug);
      exit(EXIT_FAILURE);
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
            exit(EXIT_FAILURE);
          }

        fread(cb->source, SOURCE_BUFSIZE, sizeof(char), fp);
        if(ferror(fp))
          {
            perror(filename);
            exit(EXIT_FAILURE);
          }

        fclose(fp);

        printf("\tCompiling module %s\n", spec->symbol);

        e = parse(cb->source);
        if(!e)
          exit(EXIT_FAILURE);

        stv = preval(cb, e, 0, 0);

        delete_specification(e);

        printf("\t\t%s ok\n", spec->symbol);
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
            exit(EXIT_FAILURE);
          }
      break;

      case op_name:
      case op_dname:
      case op_iname:
        stv.btl = create_ground(spec->ot, spec->symbol, 0);
      break;

      case op_number:
        stv.btl = create_ground(op_number, "", spec->value);
        stv.a = spec->value;
        stv.b = spec->value;
      break;

      case op_iterator:
        l = level - strlen(spec->symbol);
        if(l < 0)
          {
            fprintf(stderr, "%s: Error, iterator symbol out of scope: %s\n", spec->symbol, spec->debug);
            exit(EXIT_FAILURE);
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

      case op_join:
        stv = preval(cb, spec->left, level, param);
        stv_2 = preval(cb, spec->right, level, param);

        if(stv.btl)
          {
            if(stv_2.btl)
              stv.btl = create_operation(op_join, stv.btl, stv_2.btl, "%s ; %s");
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

      case op_and:
      case op_or:
      case op_delay:
        stv = preval(cb, spec->left, level, param);
        stv_2 = preval(cb, spec->right, level, param);

        sprintf(debug, "%%s %s %%s", opname(spec->ot));
        stv.btl = create_operation(spec->ot, stv.btl, stv_2.btl, debug);

        if(stv.btldef)
          {
            if(stv_2.btldef)
              stv.btldef = create_operation(op_and, stv.btldef, stv_2.btldef, "%s ; %s");
          }
        else
          stv.btldef = stv_2.btldef;
      break;

      case op_var_at:
        stv = preval(cb, spec->left, level, param);
        stv_2 = preval(cb, spec->right, level, param);

        if(stv_2.a > stv_2.b)
          {
            fprintf(stderr, TIME_FMT", "TIME_FMT": Error, empty interval in initial conditions: %s\n", stv_2.a, stv_2.b, spec->debug);
            exit(EXIT_FAILURE);
          }

        q = create_operation(op_var_at, stv.btl, create_ground(op_number, "", stv_2.a), "%s @ %s");

        for(k = stv_2.a + 1; k <= stv_2.b; k++)
          q = create_operation(op_join, q,
                                         create_operation(op_var_at, copy_specification(stv.btl),
                                                                     create_ground(op_number, "", k), "%s @ %s"), "%s , %s");
        stv.btl = q;

        delete_specification(stv_2.btl);
      break;

      case op_not:
      case op_aux:
      case op_input:
      case op_output:
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
            exit(EXIT_FAILURE);
          }

        val = stv.a / stv_2.a;

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
        for(h = 0; h < BTL_HISTORY_LEN; h++)
          {
            btl_history[h] = NULL;
            btl_history1[h] = NULL;
          }

        stv = preval(cb, spec->left, level, param);
        stv_2 = preval(cb, spec->right, level, param);

        if(stv_2.a > stv_2.b)
          {
            fprintf(stderr, TIME_FMT", "TIME_FMT": Error, empty interval after <@>: %s\n", stv_2.a, stv_2.b, spec->debug);
            exit(EXIT_FAILURE);
          }

        if(cb->seplit)
          {
            tail = stv_2.b - stv_2.a;
            r = NULL;

            gensym(cb, symbol, "AT", asserted, FALSE);
            gensym(cb, symbol1, "AT", negated, FALSE);
            gensym(cb, symbol2, "AT", undefined, TRUE);

            btl_history[0] = create_ground(op_name, symbol, 0);
            btl_history1[0] = create_operation(op_not, create_ground(op_name, symbol1, 0), NULL, "~ %s");
            strcpy(symbol_h[0], symbol2);

            newbtl = create_operation(op_and, CREATE_IMPLY(btl_history[0], stv.btl), CREATE_IMPLY(copy_specification(stv.btl), btl_history1[0]), "%s & %s");

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
                        exit(EXIT_FAILURE);
                      }

                    if(btl_history[h])
                      {
                        p = btl_history[h];
                        p1 = btl_history[h];
                      }
                    else
                      {
                        q = create_operation(op_and, copy_specification(p),
                                                     create_operation(op_delay, copy_specification(p),
                                                                                create_ground(op_number, "", - n), "%s @ %s"), "%s & %s");

                        q1 = create_operation(op_and, copy_specification(p1),
                                                      create_operation(op_delay, copy_specification(p1),
                                                                                 create_ground(op_number, "", - n), "%s @ %s"), "%s & %s");

                        gensym(cb, symbol, "AT", asserted, FALSE);
                        gensym(cb, symbol1, "AT", negated, FALSE);
                        gensym(cb, symbol2, "AT", undefined, TRUE);

                        p = create_ground(op_name, symbol, 0);
                        p1 = create_operation(op_not, create_ground(op_name, symbol1, 0), NULL, "~ %s");
                        btl_history[h] = p;
                        btl_history1[h] = p1;
                        strcpy(symbol_h[h], symbol2);

                        newbtl = create_operation(op_and, newbtl, create_operation(op_and, CREATE_IMPLY(p, q), CREATE_IMPLY(q1, p1), "%s & %s"), "%s ; %s");
                      }

                    tail -= n;
                    n *= 2;
                  }

                q = create_operation(op_delay, create_ground(op_name, symbol_h[h], 0), create_ground(op_number, "", stv_2.b), "%s @ %s");

                if(r)
                  r = create_operation(op_and, q, r, "%s & %s");
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

            gensym(cb, symbol, "AT", asserted, TRUE);

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
                        exit(EXIT_FAILURE);
                      }

                    if(btl_history[h])
                      p = btl_history[h];
                    else
                      {
                        q = create_operation(op_and, copy_specification(p),
                                             create_operation(op_delay, copy_specification(p),
                                                                        create_ground(op_number, "", - n), "%s @ %s"), "%s & %s");

                        gensym(cb, symbol, "AT", asserted, TRUE);

                        p = create_ground(op_name, symbol, 0);
                        btl_history[h] = p;

                        newbtl = create_operation(op_and, newbtl, CREATE_EQV(p, q), "%s ; %s");
                      }

                    tail -= n;
                    n *= 2;
                  }

                q = create_operation(op_delay, copy_specification(p), create_ground(op_number, "", stv_2.b), "%s @ %s");

                if(r)
                  r = create_operation(op_and, q, r, "%s & %s");
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
      break;

      case op_happen:
        for(h = 0; h < BTL_HISTORY_LEN; h++)
          {
            btl_history[h] = NULL;
            btl_history1[h] = NULL;
          }

        stv = preval(cb, spec->left, level, param);
        stv_2 = preval(cb, spec->right, level, param);

        if(stv_2.a > stv_2.b)
          {
            fprintf(stderr, TIME_FMT", "TIME_FMT": Error, empty interval after <?>: %s\n", stv_2.a, stv_2.b, spec->debug);
            exit(EXIT_FAILURE);
          }

        if(cb->seplit)
          {
            tail = stv_2.b - stv_2.a;
            r = NULL;

            gensym(cb, symbol, "HP", asserted, FALSE);
            gensym(cb, symbol1, "HP", negated, FALSE);
            gensym(cb, symbol2, "HP", undefined, TRUE);

            btl_history[0] = create_ground(op_name, symbol, 0);
            btl_history1[0] = create_operation(op_not, create_ground(op_name, symbol1, 0), NULL, "~ %s");
            strcpy(symbol_h[0], symbol2);

            newbtl = create_operation(op_and, CREATE_IMPLY(btl_history[0], stv.btl), CREATE_IMPLY(copy_specification(stv.btl), btl_history1[0]), "%s & %s");

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
                        exit(EXIT_FAILURE);
                      }

                    if(btl_history[h])
                      {
                        p = btl_history[h];
                        p1 = btl_history[h];
                      }
                    else
                      {
                        q = create_operation(op_or, copy_specification(p),
                                                    create_operation(op_delay, copy_specification(p),
                                                                               create_ground(op_number, "", - n), "%s @ %s"), "%s | %s");

                        q1 = create_operation(op_or, copy_specification(p1),
                                                     create_operation(op_delay, copy_specification(p1),
                                                                                create_ground(op_number, "", - n), "%s @ %s"), "%s | %s");

                        gensym(cb, symbol, "HP", asserted, FALSE);
                        gensym(cb, symbol1, "HP", negated, FALSE);
                        gensym(cb, symbol2, "HP", undefined, TRUE);

                        p = create_ground(op_name, symbol, 0);
                        p1 = create_operation(op_not, create_ground(op_name, symbol1, 0), NULL, "~ %s");
                        btl_history[h] = p;
                        btl_history1[h] = p1;
                        strcpy(symbol_h[h], symbol2);

                        newbtl = create_operation(op_and, newbtl, create_operation(op_and, CREATE_IMPLY(p, q), CREATE_IMPLY(q1, p1), "%s & %s"), "%s ; %s");
                      }

                    tail -= n;
                    n *= 2;
                  }

                q = create_operation(op_delay, create_ground(op_name, symbol_h[h], 0), create_ground(op_number, "", stv_2.b), "%s @ %s");

                if(r)
                  r = create_operation(op_or, q, r, "%s | %s");
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

            gensym(cb, symbol, "HP", asserted, TRUE);

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
                        exit(EXIT_FAILURE);
                      }

                    if(btl_history[h])
                      p = btl_history[h];
                    else
                      {
                        q = create_operation(op_or, copy_specification(p),
                                             create_operation(op_delay, copy_specification(p),
                                                                        create_ground(op_number, "", - n), "%s @ %s"), "%s | %s");

                        gensym(cb, symbol, "HP", asserted, TRUE);

                        p = create_ground(op_name, symbol, 0);
                        btl_history[h] = p;

                        newbtl = create_operation(op_and, newbtl, CREATE_EQV(p, q), "%s ; %s");
                      }

                    tail -= n;
                    n *= 2;
                  }

                q = create_operation(op_delay, copy_specification(p), create_ground(op_number, "", stv_2.b), "%s @ %s");

                if(r)
                  r = create_operation(op_or, q, r, "%s | %s");
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
      break;

      case op_since:
        stv = preval(cb, spec->left, level, param);
        stv_2 = preval(cb, spec->right, level, param);

        if(cb->seplit)
          {
            gensym(cb, symbol, "SN", asserted, FALSE);
            gensym(cb, symbol1, "SN", negated, FALSE);

            p = create_ground(op_name, symbol, 0);
            p1 = create_operation(op_not, create_ground(op_name, symbol1, 0), NULL, "~ %s");

            q = create_operation(op_or, stv.btl,
                                        create_operation(op_and, stv_2.btl,
                                                                 create_operation(op_delay, copy_specification(p),
                                                                                            create_ground(op_number, "", -1), "%s @ %s"), "%s & %s"), "%s | %s");
            q1 = create_operation(op_or, copy_specification(stv.btl),
                                         create_operation(op_and, copy_specification(stv_2.btl),
                                                                  create_operation(op_delay, copy_specification(p1),
                                                                                             create_ground(op_number, "", -1), "%s @ %s"), "%s & %s"), "%s | %s");

            newbtl = create_operation(op_and, CREATE_IMPLY(p, q), CREATE_IMPLY(q1, p1), "%s & %s");

            gensym(cb, symbol2, "SN", undefined, TRUE);

            stv.btl = create_ground(op_name, symbol2, 0);
          }
        else
          {
            gensym(cb, symbol, "SN", asserted, TRUE);

            p = create_ground(op_name, symbol, 0);
            q = create_operation(op_or, stv.btl,
                                        create_operation(op_and, stv_2.btl,
                                                                 create_operation(op_delay, create_ground(op_name, symbol, 0),
                                                                                            create_ground(op_number, "", -1), "%s @ %s"), "%s & %s"), "%s | %s");
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
      break;

      case op_until:
        stv = preval(cb, spec->left, level, param);
        stv_2 = preval(cb, spec->right, level, param);

        if(cb->seplit)
          {
            gensym(cb, symbol, "UT", asserted, FALSE);
            gensym(cb, symbol1, "UT", negated, FALSE);

            p = create_ground(op_name, symbol, 0);
            p1 = create_operation(op_not, create_ground(op_name, symbol1, 0), NULL, "~ %s");

            q = create_operation(op_or, stv.btl,
                                        create_operation(op_and, stv_2.btl,
                                                                 create_operation(op_delay, copy_specification(p),
                                                                                            create_ground(op_number, "", 1), "%s @ %s"), "%s & %s"), "%s | %s");
            q1 = create_operation(op_or, copy_specification(stv.btl),
                                         create_operation(op_and, copy_specification(stv_2.btl),
                                                                  create_operation(op_delay, copy_specification(p1),
                                                                                             create_ground(op_number, "", 1), "%s @ %s"), "%s & %s"), "%s | %s");

            newbtl = create_operation(op_and, CREATE_IMPLY(p, q), CREATE_IMPLY(q1, p1), "%s & %s");

            gensym(cb, symbol2, "UT", undefined, TRUE);

            stv.btl = create_ground(op_name, symbol2, 0);
          }
        else
          {
            gensym(cb, symbol, "UT", asserted, TRUE);

            p = create_ground(op_name, symbol, 0);
            q = create_operation(op_or, stv.btl,
                                        create_operation(op_and, stv_2.btl,
                                                                 create_operation(op_delay, create_ground(op_name, symbol, 0),
                                                                                            create_ground(op_number, "", 1), "%s @ %s"), "%s & %s"), "%s | %s");
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
        cb->iterator[level] = 0;

        stv = preval(cb, spec->left, level + 1, param);
        stv_2 = preval(cb, spec->right, level, param);

        if(stv_2.a < 1)
          {
            fprintf(stderr, TIME_FMT": Error, iteration parameter out of range in <forall> construct: %s\n", stv_2.a, spec->debug);
            exit(EXIT_FAILURE);
          }

        p = stv.btl;
        q = stv.btldef;

        for(k = 1; k < stv_2.a; k++)
          {
            cb->iterator[level] = k;

            stv = preval(cb, spec->left, level + 1, param);

            p = create_operation(op_and, p, copy_specification(stv.btl), "%s & %s");

            if(stv.btldef)
              q = create_operation(op_and, q, copy_specification(stv.btldef), "%s ; %s");
          }

        stv.btl = p;
        stv.btldef = q;

        delete_specification(stv_2.btl);
      break;

      case op_exists:
        cb->iterator[level] = 0;

        stv = preval(cb, spec->left, level + 1, param);
        stv_2 = preval(cb, spec->right, level, param);

        if(stv_2.a < 1)
          {
            fprintf(stderr, TIME_FMT": Error, iteration parameter out of range in <exists> construct: %s\n", stv_2.a, spec->debug);
            exit(EXIT_FAILURE);
          }

        p = stv.btl;
        q = stv.btldef;

        for(k = 1; k < stv_2.a; k++)
          {
            cb->iterator[level] = k;

            stv = preval(cb, spec->left, level + 1, param);

            p = create_operation(op_or, p, copy_specification(stv.btl), "%s | %s");

            if(stv.btldef)
              q = create_operation(op_and, q, copy_specification(stv.btldef), "%s ; %s");
          }

        stv.btl = p;
        stv.btldef = q;

        delete_specification(stv_2.btl);
      break;

      case op_one:
        cb->iterator[level] = 0;

        stv = preval(cb, spec->left, level + 1, param);
        stv_2 = preval(cb, spec->right, level, param);

        if(stv_2.b < 1)
          {
            fprintf(stderr, TIME_FMT": Error, iteration parameter out of range in <one> construct: %s\n", stv_2.b, spec->debug);
            exit(EXIT_FAILURE);
          }

        if(stv_2.a < 0 || stv_2.a >= stv_2.b)
          {
            fprintf(stderr, TIME_FMT": Error, selection parameter out of range in <one> construct: %s\n", stv_2.a, spec->debug);
            exit(EXIT_FAILURE);
          }

        if(!stv_2.a)
          p = stv.btl;
        else
          p = create_operation(op_not, stv.btl, NULL, "~ %s");

        q = stv.btldef;

        for(k = 1; k < stv_2.b; k++)
          {
            cb->iterator[level] = k;

            stv = preval(cb, spec->left, level + 1, param);

            if(stv_2.a == k)
              p = create_operation(op_and, p, copy_specification(stv.btl), "%s & %s");
            else
              p = create_operation(op_and, p, create_operation(op_not, copy_specification(stv.btl), NULL, "~ %s"), "%s & %s");

            if(stv.btldef)
              q = create_operation(op_and, q, copy_specification(stv.btldef), "%s ; %s");
          }

        stv.btl = p;
        stv.btldef = q;

        delete_specification(stv_2.btl);
      break;

      case op_unique:
        stv_2 = preval(cb, spec->right, level, param);

        if(stv_2.a < 1)
          {
            fprintf(stderr, TIME_FMT": Error, iteration parameter out of range in <unique> construct: %s\n", stv_2.a, spec->debug);
            exit(EXIT_FAILURE);
          }

        p = NULL;
        q = NULL;
        for(h = 0; h < stv_2.a; h++)
          {
            cb->iterator[level] = 0;

            stv = preval(cb, spec->left, level + 1, param);

            if(!h)
              {
                r = stv.btl;
                q = stv.btldef;
              }
            else
              {
                r = create_operation(op_not, copy_specification(stv.btl), NULL, "~ %s");

                if(stv.btldef)
                  q = create_operation(op_and, q, copy_specification(stv.btldef), "%s ; %s");
              }

            for(k = 1; k < stv_2.a; k++)
              {
                cb->iterator[level] = k;

                stv = preval(cb, spec->left, level + 1, param);

                if(h == k)
                  r = create_operation(op_and, r, copy_specification(stv.btl), "%s & %s");
                else
                  r = create_operation(op_and, r, create_operation(op_not, copy_specification(stv.btl), NULL, "~ %s"), "%s & %s");

                if(stv.btldef)
                  q = create_operation(op_and, q, copy_specification(stv.btldef), "%s ; %s");
              }

            if(!h)
              p = r;
            else
              p = create_operation(op_or, p, r, "%s | %s");
          }

        stv.btl = p;
        stv.btldef = q;

        delete_specification(stv_2.btl);
      break;

      case op_iter:
        cb->iterator[level] = 0;

        stv = preval(cb, spec->left, level + 1, param);
        stv_2 = preval(cb, spec->right, level, param);

        if(stv_2.a < 1)
          {
            fprintf(stderr, TIME_FMT": Error, iteration parameter out of range in <iter> construct: %s\n", stv_2.a, spec->debug);
            exit(EXIT_FAILURE);
          }

        p = stv.btl;
        q = stv.btldef;

        for(k = 1; k < stv_2.a; k++)
          {
            cb->iterator[level] = k;

            stv = preval(cb, spec->left, level + 1, param);

            p = create_operation(op_join, p, copy_specification(stv.btl), "%s ; %s");

            if(stv.btldef)
              q = create_operation(op_and, q, copy_specification(stv.btldef), "%s ; %s");
          }

        stv.btl = p;
        stv.btldef = q;

        delete_specification(stv_2.btl);
      break;

      default:
        assert(FALSE);
      break;
    }

  return stv;
}

subtreeval eval(c_base *cb, btl_specification *spec, smallnode *vp, bool neg, io_class sclass, d_time t)
{
  subtreeval stv, stv_2;
  smallnode *wp;
  io_signal *sp;

  stv.vp = NULL;
  stv.a = 0;

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
              sprintf(wp->debug, "~ %s", spec->debug);
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
            exit(EXIT_FAILURE);
          }
      break;

      case op_dname:
        sp = name2signal(cb, spec->symbol, TRUE);

        if(sp->sclass != internal_class)
          {
            fprintf(stderr, "%s: Error, duplicate declaration of signal: %s\n", spec->symbol, spec->debug);
            exit(EXIT_FAILURE);
          }

        sp->sclass = sclass;
      break;

      case op_iname:
        add_ic(cb, spec->symbol, neg, t);
      break;

      case op_number:
        stv.a = spec->value;
      break;

      case op_join:
        stv = eval(cb, spec->left, vp, neg, sclass, t);

        if(stv.vp)
          eval(cb, spec->right, vp, neg, sclass, t);
        else
          stv = eval(cb, spec->right, vp, neg, sclass, t);
      break;

      case op_not:
        stv = eval(cb, spec->left, vp, !neg, sclass, t);
      break;

      case op_and:
        wp = create_smallnode(cb, neg? gate : joint);
        if(!wp)
          {
            perror(NULL);
            exit(EXIT_FAILURE);
          }

        wp->up = vp;
        wp->left = eval(cb, spec->left, wp, neg, sclass, t).vp;
        wp->right = eval(cb, spec->right, wp, neg, sclass, t).vp;

        if(neg)
          sprintf(wp->debug, "~ (%s)", spec->debug);
        else
          strcpy(wp->debug, spec->debug);

        stv.vp = wp;
      break;

      case op_or:
        wp = create_smallnode(cb, neg? joint : gate);
        if(!wp)
          {
            perror(NULL);
            exit(EXIT_FAILURE);
          }

        wp->up = vp;
        wp->left = eval(cb, spec->left, wp, neg, sclass, t).vp;
        wp->right = eval(cb, spec->right, wp, neg, sclass, t).vp;

        if(neg)
          sprintf(wp->debug, "~ (%s)", spec->debug);
        else
          strcpy(wp->debug, spec->debug);

        stv.vp = wp;
      break;

      case op_delay:
        if(cb->merge && vp && vp->nclass == delay)
          {
            stv = eval(cb, spec->left, vp, neg, sclass, t);
            stv.a += eval(cb, spec->right, vp, neg, sclass, t).a;
          }
        else
          {
            wp = create_smallnode(cb, delay);
            if(!wp)
              {
                perror(NULL);
                exit(EXIT_FAILURE);
              }

            stv_2 = eval(cb, spec->left, wp, neg, sclass, t);

            wp->up = vp;
            wp->left = stv_2.vp;
            wp->k = - (stv_2.a + eval(cb, spec->right, wp, neg, sclass, t).a);

            if(cb->merge && !wp->k)
              {
                stv.vp = stv_2.vp;
                stv.vp->up = vp;

                purge_smallnode(cb, wp, vp);
              }
            else
              {
                strcpy(wp->debug, spec->debug);

                stv.vp = wp;
              }
          }
      break;

      case op_var_at:
        stv = eval(cb, spec->left, vp, neg, sclass, eval(cb, spec->right, vp, neg, sclass, t).a);
      break;

      case op_aux:
        stv = eval(cb, spec->left, vp, neg, aux_class, t);
      break;

      case op_input:
        stv = eval(cb, spec->left, vp, neg, input_class, t);
      break;

      case op_output:
        stv = eval(cb, spec->left, vp, neg, output_class, t);
      break;

      case op_init:
        stv = eval(cb, spec->left, vp, neg, sclass, t);
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

  return zpp;
}

smallnode *gendir(smallnode *vp, smallnode *bp, direction dir)
{
  smallnode *zp;

  assert(vp && !vp->zombie);

  zp = NULL;

  switch(dir)
    {
      case dir_left:
        if(bp == *genup(vp))
          zp = vp->left;
        else
          if(bp == vp->left)
            zp = vp->right;
          else
            if(bp == vp->right)
              zp = *genup(vp);
            else
              assert(FALSE);
      break;

      case dir_right:
        if(bp == *genup(vp))
          zp = vp->right;
        else
          if(bp == vp->right)
            zp = vp->left;
          else
            if(bp == vp->left)
              zp = *genup(vp);
            else
              assert(FALSE);
      break;

      case dir_back:
        zp = bp;
      break;

      default:
        assert(FALSE);
      break;
    }

  if(zp && zp->zombie)
    return NULL;
  else
    return zp;
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

void purge_smallnode(c_base *cb, smallnode *vp, smallnode *bp)
{
  io_signal *sp;
  smallnode *lp, *rp;
  int i;

  assert(vp);
  assert(!vp->zombie);

  lp = gendir(vp, bp, dir_left);
  rp = gendir(vp, bp, dir_right);

  for(i = 0; i < cb->num_signals; i++)
    {
      sp = &cb->sigtab[i];
      assert(sp);

      if(sp->from == vp || sp->to == vp)
        {
          if(sp->from == vp)
            {
              if(sp->to == lp)
                {
                  sp->from = rp;
                  sp->occurr = occurrence(vp, lp);
                }
              else
                if(sp->to == rp)
                  {
                    sp->from = lp;
                    sp->occurr = occurrence(vp, rp);
                  }
            }

          if(sp->to == vp)
            {
              if(sp->from == lp)
                {
                  sp->to = rp;
                  sp->occurr = occurrence(vp, rp);
                }
              else
                if(sp->from == rp)
                  {
                    sp->to = lp;
                    sp->occurr = occurrence(vp, lp);
                  }
            }

          if(!sp->from || !sp->to || sp->from == vp || sp->to == vp)
            {
              sp->from = NULL;
              sp->to = NULL;

              fprintf(stderr, "%s: Warning, signal removed\n", sp->name);
            }
        }
    }

  vp->zombie = TRUE;
}

void close_smallbranches(c_base *cb, smallnode *xp, smallnode *yp, smallnode *bp)
{
  smallnode **left, **right;

  if(!xp || xp->zombie)
    {
      erase_smalltree(cb, yp, bp);
      return;
    }
  else
    if(!yp || yp->zombie)
      {
        erase_smalltree(cb, xp, bp);
        return;
      }

  left = get_neighbor_handle(bp, xp);
  right = get_neighbor_handle(bp, yp);

  *left = yp;
  *right = xp;
}

void erase_smalltree(c_base *cb, smallnode *vp, smallnode *bp)
{
  smallnode *lp, *rp;

  if(!vp || vp->zombie)
    return;

  switch(vp->nclass)
    {
      case gate:
        lp = gendir(vp, bp, dir_left);
        rp = gendir(vp, bp, dir_right);

        purge_smallnode(cb, vp, bp);

        erase_smalltree(cb, lp, vp);
        erase_smalltree(cb, rp, vp);
      break;

      case joint:
        lp = gendir(vp, bp, dir_left);
        rp = gendir(vp, bp, dir_right);

        purge_smallnode(cb, vp, bp);

        if(bp == *genup(vp))
          {
            erase_smalltree(cb, lp, vp);
            erase_smalltree(cb, rp, vp);
          }
        else
          close_smallbranches(cb, lp, rp, vp);
      break;

      case delay:
        lp = gendir(vp, bp, dir_left);
        if(!lp)
          lp = gendir(vp, bp, dir_right);

        purge_smallnode(cb, vp, bp);

        erase_smalltree(cb, lp, vp);
      break;

      default:
        assert(FALSE);
      break;
    }
}

void purge_smalltree(c_base *cb, smallnode *vp, smallnode *bp)
{
  smallnode *lp, *rp;

  if(!vp || vp->zombie)
    return;

  switch(vp->nclass)
    {
      case gate:
        lp = gendir(vp, bp, dir_left);
        rp = gendir(vp, bp, dir_right);

        purge_smallnode(cb, vp, bp);

        close_smallbranches(cb, lp, rp, vp);
      break;

      case joint:
        lp = gendir(vp, bp, dir_left);
        rp = gendir(vp, bp, dir_right);

        purge_smallnode(cb, vp, bp);

        if(bp == *genup(vp))
          {
            purge_smalltree(cb, lp, vp);
            purge_smalltree(cb, rp, vp);
          }
        else
          if(lp == *genup(vp))
            {
              purge_smalltree(cb, lp, vp);
              erase_smalltree(cb, rp, vp);
            }
          else
            if(rp == *genup(vp))
              {
                erase_smalltree(cb, lp, vp);
                purge_smalltree(cb, rp, vp);
              }
      break;

      case delay:
        lp = gendir(vp, bp, dir_left);
        if(!lp)
          lp = gendir(vp, bp, dir_right);

        purge_smallnode(cb, vp, bp);

        purge_smalltree(cb, lp, vp);
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
                rv = fprintf(fp, "! %s (%s, %s) # %d\n", sp->name, sp->from? sp->from->name : "*", sp->to? sp->to->name : "*", sp->occurr);
              break;

              case output_class:
                rv = fprintf(fp, "? %s (%s, %s) # %d\n", sp->name, sp->from? sp->from->name : "*", sp->to? sp->to->name : "*", sp->occurr);
              break;

              case aux_class:
                if(cb->outaux)
                  rv = fprintf(fp, ". %s (%s, %s) # %d\n", sp->name, sp->from? sp->from->name : "*", sp->to? sp->to->name : "*", sp->occurr);
                else
                  rv = 0;
              break;

              case internal_class:
                if(cb->outint)
                  rv = fprintf(fp, ". %s (%s, %s) # %d\n", sp->name, sp->from? sp->from->name : "*", sp->to? sp->to->name : "*", sp->occurr);
                else
                  rv = 0;
              break;

              default:
                assert(FALSE);
              break;
            }
        }
      else
        fprintf(stderr, "%s: Warning, signal not generated\n", sp->name);
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
            rv = fprintf(fp, "(%s, %s) # %d @ "TIME_FMT"\n", sp->from? sp->from->name : "*", sp->to? sp->to->name : "*", sp->occurr, icp->t);
          else
            rv = fprintf(fp, "(%s, %s) # %d @ "TIME_FMT"\n", sp->to? sp->to->name : "*", sp->from? sp->from->name : "*", sp->occurr, icp->t);
        }
      else
        {
          fprintf(stderr, "%s: Error, initial condition refers to undeclared signal\n", icp->name);
          exit(EXIT_FAILURE);
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

  if(!vp)
    return;

  for(branch = FALSE; branch <= TRUE; branch++)
    {
      if(!branch)
        wp = vp->left;
      else
        wp = vp->right;

      if(vp == wp->up_2)
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
                  exit(EXIT_FAILURE);
                }

              xp->left = vp;
              xp->right = yp;

              vp->up_2 = xp;
              yp->up_2 = xp;

              snprintf(xp->debug, DEBUG_STRLEN, "%s & %s", vp->debug, yp->debug);

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

  vp = build_smalltree(cb, i, FALSE);

  if(!vp)
    {
      fprintf(stderr, "%s: Error, missing asserted literal for signal\n", cb->symtab[i][0]->name);
      exit(EXIT_FAILURE);
    }

  wp = build_smalltree(cb, i, TRUE);

  if(!wp)
    {
      fprintf(stderr, "%s: Error, missing negated literal for signal\n", cb->symtab[i][0]->name);
      exit(EXIT_FAILURE);
    }

  xp = create_smallnode(cb, gate);
  if(!xp)
    {
      perror(NULL);
      exit(EXIT_FAILURE);
    }

  xp->left = vp;
  xp->right = wp;

  vp->up_2 = xp;
  wp->up_2 = xp;

  snprintf(xp->debug, DEBUG_STRLEN, "%s | %s", vp->debug, wp->debug);

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
              exit(EXIT_FAILURE);
            }

          xp->left = vp;
          xp->right = yp;

          vp->up_2 = xp;
          yp->up_2 = xp;

          snprintf(xp->debug, DEBUG_STRLEN, "%s & %s", vp->debug, yp->debug);

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

compinfo compile(char *source_name, char *base_name, char *state_name, char *xref_name, char *path, bool seplit, bool merge, bool outaux, bool outint)
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

  cb->seplit = seplit;
  cb->merge = merge;
  cb->outaux = outaux;
  cb->outint = outint;

  fp = fopen(source_filename, "r");
  if(!fp)
    {
      perror(source_filename);
      free(cb);
      return cperf;
    }

  fread(cb->source, SOURCE_BUFSIZE, sizeof(char), fp);
  if(ferror(fp))
    {
      perror(source_filename);
      free(cb);
      return cperf;
    }

  fclose(fp);

  printf("Compiling master %s to %s\n", source_filename, base_filename);

  e = parse(cb->source);
  if(!e)
    {
      free(cb);
      return cperf;
    }

  stv = preval(cb, e, 0, 0);

  delete_specification(e);

  printf("\t%s ok\n", source_filename);

  if(stv.btldef)
    f = create_operation(op_and, stv.btl, stv.btldef, "%s ; %s");
  else
    f = stv.btl;

  stv = eval(cb, f, NULL, FALSE, internal_class, 0);

  delete_specification(f);

  printf("Generating overall network\n");

  cvp = build_cotree(cb);

  purge_smalltree(cb, stv.vp, NULL);
  purge_smalltree(cb, cvp, NULL);

  delete_zombies(cb);

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
  bool seplit, merge, outaux, outint;
  compinfo cperf;
  int i;

  source_name = base_name = state_name = xref_name = NULL;
  path = "";
  seplit = merge = outaux = outint = FALSE;

  for(i = 1; i < argc; i++)
    {
      if(*argv[i] == '-')
        {
          option = argv[i] + 1;
          switch(*option)
            {
            case 'h':
              fprintf(stderr, "Usage: %s [-bBuwx] [-I state] [-o base] [-P path] [-X symbols] [source]\n",
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
                        outaux = TRUE;
                        outint = TRUE;
                      break;

                      case 'u':
                        merge = TRUE;
                      break;

                      case 'w':
                        seplit = TRUE;
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
         "Design & coding by Andrea Giotti, 2017-2018\n\n");

  cperf = compile(source_name, base_name, state_name, xref_name, path, seplit, merge, outaux, outint);

  if(cperf.ok)
    printf("Network generated -- %d edges, %d nodes (%d gates + %d joints + %d delays), %d signals, %d initial conditions\n",
            cperf.edges, cperf.tot_nodes, cperf.num_nodes[gate], cperf.num_nodes[joint], cperf.num_nodes[delay], cperf.num_signals, cperf.num_ics);
  else
    printf("Network not generated\n");

  return cperf.ok? EXIT_SUCCESS : EXIT_FAILURE;
}


