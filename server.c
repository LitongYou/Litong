#include "server.h"
#include "readline.h"
#include "wrapunix.h"
#include "hash.h"
#include "str.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;//线程初始化
int line_num = 0;

typedef struct {
  int connfd;
  char *recvline;
  char *sendline;
  int argc;
  char **argv;
  bool fin_flg;
} client_t;

/* Command statistics */
struct {
  unsigned int get;
  unsigned int set;
  unsigned int delete;
  unsigned int find;
  unsigned int mem;
} hash_state = {
  0, 0, 0, 0, 0,
};

/* Overall setting */
struct {
  bool do_daemon;
  int port;
  char *inter;
  double uptime;
} settings;

/* Function prototype declaration
协议的声明 */
void cmd_set(client_t *client);
void cmd_get(client_t *client);
void cmd_find(client_t *client);
void cmd_delete(client_t *client);
void cmd_status(client_t *client);
void cmd_error(client_t *client);
void cmd_help(client_t *client);
void cmd_quit(client_t *client);
void cmd_purge(client_t *client);

/* Command table */
struct {
  char *name;
  void (*func)(client_t *client);
} cmd_tb[] = {
  { "set", cmd_set },
  { "get", cmd_get },
  { "find", cmd_find },
  { "delete", cmd_delete },
  { "status", cmd_status },
  { "error", cmd_error },
  { "purge", cmd_purge },
  { "help", cmd_help },
  { "quit", cmd_quit },
};
int cmd_tb_size = sizeof(cmd_tb) / sizeof(cmd_tb[0]);/*定义命令行table的大小*/

/* Set the current time */
double get_time() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + tv.tv_usec * 1e-6;
}

/* Set up a message*/
void msg_set(client_t *client, char *fmt, ...) {
  va_list ap;
  char line[MAXLINE] = "";
  va_start(ap, fmt);
  vsnprintf(line, MAXLINE - 2, fmt, ap);
  strcat(line, "\r\n");
  client->sendline = malloc(sizeof(char) * (strlen(line) + 1));
  strcpy(client->sendline, line);
  va_end(ap);
}

/* Set up (key,value) 
插入键值对*/
void cmd_set(client_t *client) {
  char *key, *value;
  if (client->argc < 3) {
    msg_set(client, "set the format is invalid. [set <key> <value>]");
    return;
  }
  key = client->argv[1];
  value = client->argv[2];
  Pthread_mutex_lock(&mutex);
  arr_insert(key, value);
  hash_state.set++;
  hash_state.mem +=
    sizeof(hash_record_t) + strlen(key) + strlen(value) + 2;
  Pthread_mutex_unlock(&mutex);
  msg_set(client, "Success. (key, value): (%s, %s)", key, value);
}

/* Get key value */
void cmd_get(client_t *client) {
  char *value = NULL, *key = NULL;
  if (client->argc < 2) {
    msg_set(client, "get the format is invalid [get <key>]");
  }
  key = client->argv[1];
  Pthread_mutex_lock(&mutex);
  value = arr_get(key);
  hash_state.get++;
  Pthread_mutex_unlock(&mutex);
  if (value == NULL) {
    msg_set(client, "%s has no value", key);
  }
  else {
    msg_set(client, "The (key, value) is: (%s, %s)", key, value);
  }
}

/* Confirm wheter or not it exists the key */
void cmd_find(client_t *client) {
  bool check;
  if (client->argc < 2) {
    msg_set(client, "find the format is invalid [find <key>]");
  }
  check = arr_find(client->argv[1]);
  Pthread_mutex_lock(&mutex);
  hash_state.find++;
  Pthread_mutex_unlock(&mutex);
  if (check) {
    msg_set(client, "True. The Key exists in the Server.");
  }
  else {
    msg_set(client,  "False. The Key does not exist in the Server.");
  }
}

/* Delete value associated with key */
void cmd_delete(client_t *client) {
  int check;
  char *value = NULL, *key = NULL;
  if (client->argc < 2) {
    msg_set(client, "delete the format is invalid [delete <key>]");
  }
  key = client->argv[1];
  value = arr_get(key);
  check = arr_delete(client->argv[1]);
  if (check) {
  Pthread_mutex_lock(&mutex);
  hash_state.delete++;
  hash_state.mem -=
    sizeof(hash_record_t) + strlen(key) + strlen(value) + 2;
  Pthread_mutex_unlock(&mutex);
    msg_set(client, "Deleted (key, value): (%s, %s)", key, value);
  }
  else {
    msg_set(client, "This operation was failed.");
  }
}

/* Obtain the current status */
void cmd_status(client_t *client) {
  int number = arr_get_num();
  msg_set(client,
          "Uptime        ... %9.2lf sec\r\n" \
          "Port number   ... %9d\r\n" \
          "Key numbers   ... %9d\r\n" \
          "get cmd       ... %9d\r\n" \
          "set cmd       ... %9d\r\n" \
          "delete cmd    ... %9d\r\n" \
          "find cmd      ... %9d\r\n" \
          "Memory alloc  ... %9d",
          (get_time() - settings.uptime), settings.port,
          number, hash_state.get, hash_state.set,
          hash_state.delete, hash_state.find,
          hash_state.mem);
}

/* Display of help */
void cmd_help(client_t *client) {
  msg_set(client,
	  "set <key> <value>  ...  <key> <value>save the pair of (key,value).\r\n"  \
          "get <key>          ...  <key> Get the value corresponding to the key.\r\n" \
          "delete <key>       ...  Delete the value corresponding to <key>.\r\n" \
          "find <key>         ...  Check if <key> exists.\r\n" \
          "purge              ...  Delete cache.\r\n" \
          "help               ...  Output this message.\r\n" \
          "status             ...  Show the system's status.\r\n" \
          "quit               ...  Disconnect from the server." \          
          );
}

/* Delete cache or called delete the whole hash table*/
void cmd_purge(client_t *client) {  
  Pthread_mutex_lock(&mutex);
  arr_free();
  arr_init();
  Pthread_mutex_unlock(&mutex);
}

/* Disconnect */
void cmd_quit(client_t *client) {
  client->fin_flg = true;
}

/* Error message */
void cmd_error(client_t *client) {
  msg_set(client, "---- System Error ! ----");
}

//client and server之间的通信
/* Message initialization */
void message_init(client_t *client, int connfd) {
  client->recvline = NULL;
  client->sendline = NULL;
  client->argv = NULL;
  client->argc = 0;
  client->connfd = connfd;
  client->fin_flg = false;
}

/* Get message */
void message_get(client_t *client) {
  int n;
  client->recvline = malloc(sizeof(char) * MAXLINE);
  if ((n = readline(client->connfd, client->recvline, MAXLINE)) <= 0) {    
    
    client->fin_flg = true;
  }
  else {
    if (client->recvline != NULL) {
      Pthread_mutex_lock(&mutex);
      line_num++;
      //printf("## total = %d, line = %s\n", line_num, client->recvline);
      Pthread_mutex_unlock(&mutex);
    }
  }
}

/* Analyze message 
分析获取的message包含的内容信息*/
void message_parse(client_t *client) {
  char **recvline, *cmd;
  int recv_size;
  int i;
  if (client->fin_flg == true || client->recvline == NULL) return;
  recvline = split(client->recvline, &recv_size);
  client->argc = recv_size;
  client->argv = recvline;
  cmd = recvline[0];

  /* Execute command*/
  for (i = 0; i < cmd_tb_size; i++) {
    if (strcmp(cmd_tb[i].name, cmd) == 0) {
      cmd_tb[i].func(client);
      break;
    }
  }
  if (i == cmd_tb_size) {
    cmd_error(client);
  }
}

/* Send message */
void message_send(client_t *client) {
  if (client->sendline != NULL) {
    write(client->connfd, client->sendline, strlen(client->sendline));
  }
}

/* Release pointer 
释放通信时的指针*/
void message_free(client_t *client) {
  free(client->recvline);
  free(client->sendline);
  if (client->argv != NULL) {    
    free(client->argv);
  }
  client->argc = 0;
}

/* Dialogue with clients */
void process_client(int connfd) {
  client_t *client = malloc(sizeof(client_t));

  while (1) {
    message_init(client, connfd);
    message_get(client);
    message_parse(client);
    message_send(client);
    message_free(client);
    if (client->fin_flg) break;
  }
  
  free(client);
}
/* Client individual processing(This process) */
void *process(void *arg) {
  int connfd;
  connfd = *((int *)arg);
  free(arg);
  Pthread_detach(pthread_self());
  process_client(connfd);
  Close(connfd);
  return NULL;
}

/* Prepare listenfd */
/*Listen file descriptor定义*/
int conn_listen(void) {
  struct sockaddr_in servaddr;
  int listenfd;
  listenfd = Socket(AF_INET, SOCK_STREAM, 0);
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(settings.port);
  if (settings.inter != NULL) {
    Inet_pton(AF_INET, settings.inter, &servaddr.sin_addr.s_addr);
  }
  else {
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  }
  Bind(listenfd, &servaddr, sizeof(servaddr));
  Listen(listenfd, LISTENQ);
  return listenfd;
}

/* Initialize setting */
void settings_init() {
  settings.do_daemon = false;
  settings.port = 9877;
  settings.uptime = get_time();
  settings.inter = NULL;
}

/* Display usage */
void usage() {
  printf("-p ... Specify the port number to listen to.\n" \
         "-l ... Specify the port number to listen to.\n" \
         "-h ... Output a help message.\n" \
         "-d ... Operate in daemon mode\n");
}

int main(int argc, char *argv[]) {
  struct sockaddr_in *cliaddr = NULL;
  int listenfd;
  int *connfd = NULL;
  socklen_t len;
  pthread_t tid;
  char ch;

  settings_init();
  while (-1 != (ch = getopt(argc, argv, "dhp:l:"))) {
    switch (ch) {
    case 'p' :
      settings.port = atoi(optarg);
      break;
    case 'h':
      usage();
      exit(0);
      break;
    case 'd':
      settings.do_daemon = true;
      break;
    case  'l':
      settings.inter = optarg;
      break;
    default:
      fprintf(stderr, "Illegal argument \"%c\"", ch);
      return 1;
    }
  }

  if (settings.do_daemon) {
    daemonize();
  }
  
  signal(SIGPIPE, SIG_IGN); 
  listenfd = conn_listen();
  arr_init();
  while (1) {
    len = sizeof(*cliaddr);
    connfd = malloc(sizeof(int));
    *connfd = Accept(listenfd, NULL, NULL);
    Pthread_create(&tid, NULL, &process, connfd);
  }
  arr_free();
  return 0;
}
