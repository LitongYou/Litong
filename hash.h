#ifndef __HASH_H
#define __HASH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#define HASH_DENSITY 1.3       /* Density of hash table */ 
#define DEFAULT_SIZE 1024      /* Size of default hash table */
/* record */
typedef struct hash_record_tag {
  struct hash_record_tag *next;
  unsigned int hash;
  char *key;
  char *value;
} hash_record_t;

/* Create hash table */
extern void arr_init(void);

/* Initialize hash table */
extern void arr_free(void);

/* Insert the pair of (key, value)  */
extern int arr_insert(char *key, char *value);

/* Get key */
extern char *arr_get(char *key);

/* Delete key */
extern int arr_delete(char *key);

/* Find key */
extern bool arr_find(char *key);

/* Output the number of stored keys. */
extern int arr_get_num(void);

#endif // __HASH_H
