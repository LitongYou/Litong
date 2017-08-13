#include "wrapunix.h"

#define HOST_NAME "localhost"

static int sockfd;
void *send_msg(void *arg);

/* Global settings */
struct {
  int port;
  char *host;
} settings;

/* The process */
void process(int connfd) {
  pthread_t tid;
  int n;
  sockfd = connfd;
  Pthread_create(&tid, NULL, send_msg, NULL);
  //线程创建
  /* Server==>Standard output */
  while (1) {
    char recvline[MAXLINE] = "";
    if ((n = read(sockfd, recvline, MAXLINE)) == 0) break;
    fputs(recvline, stdout);
    printf("KVStore > "); fflush(stdout);
  }
}

/* Server<==Standard input */
void *send_msg(void *arg) {
  printf("KVStore > ");
  while (1) {
    char sendline[MAXLINE] = "";
    fflush(stdout);
    if (fgets(sendline, MAXLINE - 2, stdin) == NULL) break;
    sendline[strlen(sendline) - 1] = '\0';
    strcat(sendline, "\r\n");
    Write(sockfd, sendline, strlen(sendline));
  }
  shutdown(sockfd, SHUT_WR);
  return NULL;
}

/* New connection */
int conn_new() {
  struct sockaddr_in servaddr;
  struct hostent *hptr;
  struct in_addr **pptr;
  int connfd;
  connfd = Socket(AF_INET, SOCK_STREAM, 0);
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(settings.port);
  if ((hptr = gethostbyname(settings.host)) == NULL) {
    err_quit("Invalid host name");
  }
  pptr = (struct in_addr **)hptr->h_addr_list;
  memcpy(&servaddr.sin_addr, *pptr, sizeof(struct in_addr));
  printf("Connecting to server %s\n", hptr->h_name);
  Connect(connfd, &servaddr, sizeof(servaddr));
  return connfd;
}

/* Initialize settings */
void settings_init(void) {
  settings.port = 9877;
  settings.host = HOST_NAME;
}

/* Instruction of using */
void usage(void) {
  printf("-h ... Designate the host to connect.\n" \
         "-p ... Specify the port number of the host to be connected.\n");
  exit(1);
}

int main(int argc, char **argv) {
  int connfd;
  char ch;

  settings_init();
  while (-1 != (ch = getopt(argc, argv, "p:h:"))) {
    switch (ch) {
    case 'p':
      settings.port = atoi(optarg);
      break;
    case 'h':
      settings.host = optarg;
      break;
    default:
      fprintf(stderr, "Illigal option \"%c\"\n", ch);
      usage();
      return 1;
    }
  }
  
  connfd = conn_new();
  process(connfd);
  return 0;
}
