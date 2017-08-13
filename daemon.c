#include <syslog.h>
#include "wrapunix.h"

/* Process becomes daemon 
守护进程的创建*/
void daemonize() {
  pid_t pid;
  
  /* End parent process */
  if ((pid = fork()) != 0) {
    exit(0);
  }
  /* Promoted to session leader */
  setsid();
  /*调用进程不能是进程首进程，也就是说想setsid调用成功，那么调用者就不能是进程组长*/
  signal(SIGHUP, SIG_IGN);
  return;
}
