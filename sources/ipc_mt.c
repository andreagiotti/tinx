
/* Linux IPC interface */

#if !defined UNIX_FILE_IO
  #define ANSI_FILE_IO
#endif

#include "tinx_mt.h"

channel_posix add_queue_posix(char *name, stream_class sclass)
{
  channel_posix c;
  struct mq_attr attr;

  attr.mq_flags = O_NONBLOCK;
  attr.mq_maxmsg = MAX_MESSAGES;
  attr.mq_msgsize = MSG_SIZE;
  attr.mq_curmsgs = 0;

  c = mq_open(name, (sclass == input_stream? O_RDONLY : O_WRONLY) | O_CREAT | O_NONBLOCK, PERMS, &attr);

  return c;
}

int send_message_posix(channel_posix c, char *a)
{
  char buffer[MSG_SIZE];

  *buffer = *a;

  return mq_send(c, buffer, MSG_SIZE, 0);
}

int read_message_posix(channel_posix c, char *a)
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

long int queue_key(char *name)
  {
    long int k;

    k = 0;

    while(*name)
      {
        k <<= FAKE_BASE_BITS;

        if(isdigit(*name))
          k ^= *name - '0';
        else
          if(isalpha(*name))
            k ^= toupper(*name) - 'A' + 10;

        name++;
      }

    return k;
  }

channel_sys5 add_queue_sys5(char *name, stream_class sclass)
{
  channel_sys5 c;

  c.paddr = msgget(ftok(".", sclass == input_stream? MAGIC_TOKEN_I : MAGIC_TOKEN_O), IPC_CREAT | PERMS);
  c.saddr = queue_key(name) % LONG_MAX;

  return c;
}

int send_message_sys5(channel_sys5 c, char *a)
{
  message qbuf;

  qbuf.mtype = c.saddr;
  qbuf.mtext = *a;

  return msgsnd(c.paddr, &qbuf, 1, IPC_NOWAIT);
}

int read_message_sys5(channel_sys5 c, char *a)
{
  message qbuf;
  int rv;

  qbuf.mtype = c.saddr;

  rv = msgrcv(c.paddr, &qbuf, 1, c.saddr, IPC_NOWAIT);

  if(rv == 1)
    {
      *a = qbuf.mtext;

      return 0;
    }

  *a = EOF;

  return rv;
}

int delete_queues_sys5()
{
  int paddr, rv;

  paddr = msgget(ftok(".", MAGIC_TOKEN_I), IPC_CREAT | PERMS);
  if(paddr < 0)
    return paddr;

  rv = msgctl(paddr, IPC_RMID, NULL);
  if(rv)
    return rv;

  paddr = msgget(ftok(".", MAGIC_TOKEN_O), IPC_CREAT | PERMS);
  if(paddr < 0)
    return paddr;

  rv = msgctl(paddr, IPC_RMID, NULL);
  if(rv)
    return rv;

  return 0;
}

