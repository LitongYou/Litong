#ifndef __WRAPUNIX_H
#define __WRAPUNIX_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <assert.h>
#include <stdbool.h>
#include <stdarg.h>
#include <pthread.h>

#define MAXLINE 1024
#define SERV_PORT 9877
#define LISTENQ 1024
#define SERV_PORT_STR "9877"
#define SERV_ADDR "127.0.0.1"
#define BUFFSIZE 8192
#define MAXSOCKADDR 128
#define SA struct sockaddr

#define min(a, b) ( (a) < (b) ? (a) : (b) )
#define max(a, b) ( (a) > (b)) ? (a) : (b) )

extern int Socket(int family, int type, int protocol);
extern void Bind(int listenfd, void *servaddr, int size);
extern void Listen(int listenfd, int backlog);
extern int Accept(int sockfd, void *servaddr, socklen_t *size);
extern void Write(int sockfd, char *buff, int size);
extern int Read(int sockfd, char *buff, int size);
extern void Close(int fd);
extern const char *Inet_ntop(int family, const void *addr, char *line, size_t line_size);
extern void Inet_pton(int family, char *ip_str, void *s_addr);
extern int Recv(int sockfd, char *line, int *len);
extern int Connect(int sockfd, void *servaddr, socklen_t len);
extern int Select(int nfds, fd_set *rfds, fd_set *wrds, fd_set *efds, struct timeval *timeout);
extern int Fcntl(int fd, int cmd, int arg);
extern pid_t Fork();
extern void *Malloc(size_t size);
extern void Pthread_create(pthread_t *tid, const pthread_attr_t *attr, void *(*func)(void *), void *arg);
extern void Pthread_detach(pthread_t tid);
extern void Pthread_mutex_lock(pthread_mutex_t *mptr);
extern void Pthread_mutex_unlock(pthread_mutex_t *mptr);

/* Functions related to errors */
extern void err_ret(const char *fmt, ...);
extern void err_msg(const char *fmt, ...);
extern void err_quit(const char *fmt, ...);

#endif /* __WRAPUNIX_H */

