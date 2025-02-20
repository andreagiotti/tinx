
/* Linux IPC interface */

#if !defined UNIX_FILE_IO
  #define ANSI_FILE_IO
#endif

#include "tinx.h"

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

int msend_message_posix(channel_posix c, char *a, int n)
{
  char buffer[MSG_SIZE];

  memcpy(buffer, a, n);

  return mq_send(c, buffer, MSG_SIZE, 0);
}

int mread_message_posix(channel_posix c, char *a, int n)
{
  char buffer[MSG_SIZE];

  if(mq_receive(c, buffer, MSG_SIZE, NULL) != MSG_SIZE)
    {
      *a = EOF;

      return -1;
    }

  memcpy(a, buffer, n);

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

int msend_message_sys5(channel_sys5 c, char *a, int n)
{
  message qbuf;

  qbuf.mtype = c.saddr;
  memcpy(qbuf.mtext, a, n);

  return msgsnd(c.paddr, &qbuf, MSG_SIZE, IPC_NOWAIT);
}

int mread_message_sys5(channel_sys5 c, char *a, int n)
{
  message qbuf;

  qbuf.mtype = c.saddr;

  if(msgrcv(c.paddr, &qbuf, MSG_SIZE, c.saddr, IPC_NOWAIT) != MSG_SIZE)
    {
      *a = EOF;

      return -1;
    }

  memcpy(a, qbuf.mtext, n);

  return 0;
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

safesocket add_socket(char *name, stream_class sclass)
{
  char basename[MAX_NAMELEN];
  safesocket sock;
  int port;
  static struct sockaddr_in address, address2;
  static socklen_t length;
  struct hostent *server;
  void (*prevhand)(int);

  sock.asid = -1;

  if(sscanf(name, "%[^)(] ( %d )", basename, &port) < 2)
    port = PORTBASE;
  else
    if(port < 1 || port > 65535)
      return sock;

  server = gethostbyname(basename);
  if(!server)
    return sock;

  address.sin_family = AF_INET;
  address.sin_port = htons(port);

  if(sclass == output_stream)
    {
      sock.lsid = socket(AF_INET, SOCK_STREAM, 0);
      if(sock.lsid < 0)
        return sock;

      address.sin_addr.s_addr = INADDR_ANY;

      if(bind(sock.lsid, (struct sockaddr *)&address, sizeof(address)) < 0)
        return sock;

      if(listen(sock.lsid, 5) < 0)
        return sock;

      length = sizeof(address2);
      
      prevhand = signal(SIGINT, SIG_DFL);

      for(;;)
        {
          sock.asid = accept(sock.lsid, (struct sockaddr *)&address2, &length);

          if(memcmp(&address2.sin_addr.s_addr, server->h_addr, server->h_length))
            close(sock.asid);
          else
            break;
        }

      signal(SIGINT, prevhand);
    }
  else
    {
      sock.lsid = -1;

      sock.asid = socket(AF_INET, SOCK_STREAM, 0);
      if(sock.asid < 0)
        return sock;

      memcpy(&address.sin_addr.s_addr, server->h_addr, server->h_length);

      prevhand = signal(SIGINT, SIG_DFL);

      while(connect(sock.asid, (struct sockaddr *)&address, sizeof(address)) < 0)
        usleep(1000);

      signal(SIGINT, prevhand);
    }

  return sock;
}

int close_socket(safesocket sock)
{
  int rv;

  rv = close(sock.asid);
  
  if(rv < 0)
    return rv;

  if(sock.lsid >= 0)
    rv = close(sock.lsid);

  return rv;
}


