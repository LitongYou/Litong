#include "wrapunix.h"

int Socket (int family, int type, int protocol) {
  int n;
  if ((n = socket(family, type, protocol)) < 0)
    err_ret("Socket error");
  return n;
}
/*listenfd，用于表示一个已捆绑未连接的套接口的描述字
backlog等待连接队列的最大长度
无错误发生，listen()返回值为0；否则返回-1*/
void Bind(int listenfd, void *servaddr, int size) {
  if (bind(listenfd, servaddr, size) < 0)
    err_ret("Bind error");
}

void Listen(int listenfd, int backlog) {
  char *ptr;
  if ((ptr = getenv("LISTENQ")) != NULL)
    backlog = atoi(ptr);
  
  if (listen(listenfd, backlog) < 0)
    err_ret("Listen error");
}

int Accept(int sockfd, void *servaddr, socklen_t *size) {
  int connfd;
  if ((connfd = accept(sockfd, servaddr, size)) < 0) {
    err_ret("Connect error");
  }
  return connfd;
}

void Write(int sockfd, char *buff, int size) {
  if (write(sockfd, buff, size) < 0) {
    err_ret("Write error");
  }
}

int Read(int sockfd, char *buff, int size) {
  int n;
  if ((n = read(sockfd, buff, size)) < 0) {
    err_ret("Read error");
  }
  return n;
}

void Close(int fd) {
  if (close(fd) < 0) {
    err_ret("Close error");
  }
}

const char *
Inet_ntop(int family, const void *addr, char *line, size_t line_size) {
  const char *string;
  if ((string = inet_ntop(family, addr, line, line_size)) == NULL)
    err_ret("Inet_ntop error");
  printf("String = %s", string);
  return string;
}

void Inet_pton(int family, char *ip_str, void *s_addr) {
  if (inet_pton(family, ip_str, s_addr) < 0)
    err_ret("Inet_pton error");
}

int Recv(int sockfd, char *line, int *len) {
  if (recv(sockfd, line, *len, MSG_WAITALL) <= 0)
    err_ret("Recv error: ");
  return *len;
}

int Connect(int sockfd, void *servaddr, socklen_t len) {
  if (connect(sockfd, servaddr, len) < 0)
    err_quit("Connection error: ");
  return len;
}

int Select(int nfds, fd_set *rfds, fd_set *wrds, fd_set *efds, struct timeval *timeout) {
  int flg;
  flg = select(nfds, rfds, wrds, efds, timeout);
  if (0 > flg) {
    err_ret("Select error");
  }
  else if (0 == flg) {
    err_ret("Timeout");
  }
  return flg;
}

int Fcntl(int fd, int cmd, int arg) {
  int n;
  if ((n = fcntl(fd, cmd, arg)) == -1) {
    err_ret("Fcntl");
  }
  return n;
}

pid_t Fork() {
  pid_t pid;
  if ((pid = fork()) < 0) {
    err_ret("Fork error");
  }
  return pid;
}

void *Malloc(size_t size) {
  void *ptr;
  if ((ptr = malloc(size)) == NULL) {
    err_ret("Malloc error");
  }
  return ptr;
}

/*计算机创建一个线程默认状态是joinable
*/
void Pthread_create(pthread_t *tid, const pthread_attr_t *attr, void *(*func)(void *), void *arg) {
  if (pthread_create(tid, attr, func, arg) != 0) {
    err_ret("Pthread create error");
  }
}

void Pthread_detach(pthread_t tid) {
  if (pthread_detach(tid) != 0) {
    err_ret("Pthread detach error");
  }
}

void Pthread_mutex_lock(pthread_mutex_t *mptr) {
  if (pthread_mutex_lock(mptr) != 0) {
    err_ret("Pthread mutex lock error");
  }
}

void Pthread_mutex_unlock(pthread_mutex_t *mptr) {
  if (pthread_mutex_unlock(mptr) != 0) {
    err_ret("Pthread mutex unlock error");
  }
}

/*******************************************/
/* error.c - Define error handling         */
/* 目的 - Define generic error handling    */
/*******************************************/
/*错误机制*/
static void err_doit(int errnoflg, const char *fmt, va_list ap);

/* Output error statement (errno ant） */
void err_ret(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  err_doit(1, fmt, ap);
  va_end(ap);
  return;
}

/* Output error statement (errno none） */
void err_msg(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  err_doit(0, fmt, ap);
  va_end(ap);
  return ;
}

/* Output an error statement and kill it */
void err_quit(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  err_doit(0, fmt, ap);
  va_end(ap);
  exit(1);
}

static void err_doit(int errnoflg, const char *fmt, va_list ap) {
  int errno_save, n;
  char buf[MAXLINE + 1];
  errno_save = errno;
  vsprintf(buf, fmt, ap);
  if (errnoflg) {
    n = strlen(buf);
    snprintf(&buf[n], MAXLINE - n, ": %s", strerror(errno_save));
  }
  strcat(buf, "\n");
  fflush(stdout);
  fputs(buf, stderr);
  fflush(stderr);
  return;
}