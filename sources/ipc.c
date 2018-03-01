
/* Linux IPC interface */

#define ANSI_FILE_IO
/* #define UNIX_FILE_IO */
#define POSIX_IPC_IO
/* #define UNIX_IPC_IO */

#include "tinx.h"

#if defined POSIX_IPC_IO

channel add_queue(char *name, stream_class sclass)
{
  channel c;
  struct mq_attr attr;

  attr.mq_flags = O_NONBLOCK;
  attr.mq_maxmsg = MAX_MESSAGES;
  attr.mq_msgsize = MSG_SIZE;
  attr.mq_curmsgs = 0;

  c = mq_open(name, (sclass == input_stream? O_RDONLY : O_WRONLY) | O_CREAT | O_NONBLOCK, PERMS, &attr);

  return c;
}

int send_message(channel c, char *a)
{
  char buffer[MSG_SIZE];

  *buffer = *a;

  return mq_send(c, buffer, MSG_SIZE, 0);
}

int read_message(channel c, char *a)
{
  char buffer[MSG_SIZE];

  if(mq_receive(c, buffer, MSG_SIZE, NULL) != MSG_SIZE)
    {
      *a = EOF;

      return -1;
    }

  *a = *buffer;

  return 0;
}

#elif defined UNIX_IPC_IO

long int queue_key(char *name)
  {
    long int k;

    k = 0;

    while(*name)
      {
        k <<= FAKE_BASE_BITS;

        if(isdigit(*name))
          k += *name - '0';
        else
          if(isalpha(*name))
            k += toupper(*name) - 'A' + 10;

        name++;
      }

    return k;
  }

channel add_queue(char *name, stream_class sclass)
{
  channel c;

  c.paddr = msgget(ftok(".", MAGIC_TOKEN), IPC_CREAT | PERMS);
  c.saddr = queue_key(name) % LONG_MAX;

  return c;
}

int send_message(channel c, char *a)
{
  message qbuf;

  qbuf.mtype = c.saddr;
  *qbuf.mtext = *a;

  return msgsnd(c.paddr, (message *)&qbuf, 1, 0);
}

int read_message(channel c, char *a)
{
  message qbuf;
  int rv;

  qbuf.mtype = c.saddr;

  rv = msgrcv(c.paddr, (message *)&qbuf, 1, c.saddr, IPC_NOWAIT);

  if(rv == 1)
    {
      *a = *qbuf.mtext;

      return 0;
    }

  *a = EOF;

  return rv;
}

int remove_queue(char *name)
{
  int paddr;

  paddr = msgget(ftok(".", MAGIC_TOKEN), IPC_CREAT | PERMS);
  if(paddr < 0)
    return paddr;

  return msgctl(paddr, IPC_RMID, 0);
}

#endif

