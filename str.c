#include "str.h"

/* Count the number of whitespace, delete the space at the beginning of the line */
int trim(char *string) {
  int length;           /* The length of string */         
  int counter = 0;      /* Number of blanks */        
  int i = 0;
  char *pos;
  
  /*字符串结束的标志，字符串换行\r*/
  /* Delete line breaks */
  if ((pos = strchr(string, '\r')) != NULL) {
    *pos = '\0';
  }
  if ((pos = strchr(string, '\n')) != NULL) {
    *pos = '\0';
  }
  
  /* Count the number of spaces */
  length = strlen(string);
  for (i = 0; i < length; i++) {
    if (' ' == string[i]) {
      string[i] = '\0';
      counter++;
    }
  }  
  return counter;
}

//根据空格来split字符串
/* Separate input with spaces */
char **split(char *str, int *number) {
  char **string;           /* Store character strings separated by spaces */
  char *input = malloc(sizeof(char) * (strlen(str) + 1));
  int space_number;        /* Number of blanks */    
  int i;
  strcpy(input, str);
  space_number = trim(input);
  *number = (space_number + 1);
  string = malloc(sizeof(char *) * *number);
  for(i = 0; i < *number; i++) {
    string[i] = input;
    input += strlen(input) + 1;
  }

  return string;
}
