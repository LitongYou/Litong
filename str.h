#ifndef __STRING_H
#define __STRING_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Count the number of newlines, delete leading spaces at the end of the line */
extern int trim(char *string);

/* Separate input with spaces */
extern char **split(char *input, int *number);

#endif // __STRING_H
