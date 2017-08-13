#ifndef __READLINE_H
#define __READLINE_H

#include "wrapunix.h"

#define READLINE_BUFF 4096
extern ssize_t readline(int fd, void *vptr, size_t maxlen);

#endif // __READLINE_H
