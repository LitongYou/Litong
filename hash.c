#include "hash.h"
#include "server.h"

typedef struct {
  unsigned int num_bins;
  unsigned int num_records;
  hash_record_t **bin;
} hash_tb_t;
hash_tb_t tb;

#define MINSIZE 5  /* The smallest size of hash */

/* Size of Hash table */
unsigned int primes[] = {
  8 + 3,
  16 + 1,
  32 + 5,
  64 + 3,
  128 + 1,
  256 + 5,
  512 + 1,
  1024 + 5,
  2048 + 3,
  4096 + 1,
  8192 + 1,
  16384 + 3,
  32768 + 5,
  65536 + 1,
  131072 + 1,
  262144 + 3,
  524288 + 5,
  1048576 + 1,
  2097152 + 1,
  4194304 + 3,
  8388608 + 3,
  16777216 + 1,
  33554432 + 1,
  67108864 + 3,
  134217728 + 5,
  268435456 + 1,
  1073741824 + 3,
  0,
};

/* Return the number of entries in the new hash table */
static unsigned int new_size(unsigned int size) {
  unsigned int new_size = MINSIZE;
  int i;
  for (i = 0; i < sizeof(primes) / sizeof(primes[0]); i++) {
    if (new_size > size) return primes[i];
    new_size *= 2;
  }
  /* Too large size */
  return -1;
}

/* Initialize hash table */
void arr_init(void) {
  int i;
  tb.num_records = 0;
  tb.num_bins = new_size(DEFAULT_SIZE);
  tb.bin = (hash_record_t **)malloc(sizeof(hash_record_t *) * tb.num_bins);  

  /* Initialized with NULL pointer */
  for (i = 0; i < tb.num_bins; i++) {
    tb.bin[i] = NULL;
  }
}

/* Delete the hash table */
void arr_free(void) {
  hash_record_t *ptr, *next;
  int i;
  for (i = 0; i < tb.num_bins; i++) {
    ptr = tb.bin[i];
    while (ptr != NULL) {
      next = ptr->next;
      free(ptr);
      ptr = next;
    }
  }
  free(tb.bin);
  tb.num_records = 0;
  tb.num_bins = 0;
}

/* Perform hash processing */
static unsigned int
do_hash(char *key) {
  int length = strlen(key);
  int i;
  unsigned int hash_val = 1;
  for (i = 0; i < length; i++) {
    hash_val += key[i];
    hash_val *= 5;
  }
  return hash_val;
}

/* return bin_pos  */
static unsigned int
do_hash_bin(char *key) {
  int hash_val;
  hash_val = do_hash(key);
  return hash_val % tb.num_bins;
}

/* Search for the existence of key in the hash table */
static hash_record_t *
arr_find_record(char *key) {
  unsigned int  bin_pos;
  hash_record_t *ptr = NULL;
  assert(strlen(key) > 0);
  bin_pos = do_hash_bin(key);
  ptr = tb.bin[bin_pos];

  while (1) {
    if (ptr == NULL || strcmp(key, ptr->key) == 0) break;
    ptr = ptr->next;
  }
  return ptr;
}

/* Regenerate the hash table */
static void tb_resize(void) {
  hash_record_t *ptr, *next;
  hash_tb_t new_tb;
  unsigned int old_num_bins = tb.num_bins;
  int i;
  unsigned int hash_val;
  new_tb.num_bins = new_size(tb.num_bins + 1);
  new_tb.num_records = tb.num_records;
  new_tb.bin = (hash_record_t **)malloc(sizeof(hash_record_t *) * new_tb.num_bins);

  for (i = 0; i < old_num_bins; i++) {
    ptr = tb.bin[i];
    while (ptr != NULL) {
      next = ptr->next;
      hash_val = ptr->hash % new_tb.num_bins;
      ptr->next = new_tb.bin[hash_val];
      new_tb.bin[hash_val] = ptr;
      ptr = next;
    }
  }
  /* Generate a new hash table(Direct assignment of structures) */
  free(tb.bin);
  tb = new_tb;
}

/* Insert in hash table */
int arr_insert(char *key, char *value) {
  hash_record_t *record = NULL, *ptr = NULL;
  unsigned int  bin_pos;
  ptr = arr_find_record(key);

  if (ptr == NULL) {

    /* When a certain density is exceeded, the re-hash procedure is performed. */
    if ((double)tb.num_records / (double)tb.num_bins >= HASH_DENSITY) {
      tb_resize();
    }
    record = malloc(sizeof(hash_record_t));
    record->key = malloc(sizeof(char) * (strlen(key) + 1));
    record->value = malloc(sizeof(char) * (strlen(value) + 1));
    strcpy(record->key, key);
    strcpy(record->value, value);
    record->hash = do_hash(key);
    bin_pos = do_hash_bin(key);
    record->next = tb.bin[bin_pos];
    tb.bin[bin_pos] = record;
    tb.num_records++;
    return 1;
  }
  else {    
    ptr->value = malloc(sizeof(char) * (strlen(value) + 1));
    ptr->value = value;
    return 0;
  }
}

/* Get value from hash table */
char *arr_get(char *key) {
  hash_record_t *ptr = NULL;
  ptr = arr_find_record(key);
  if (ptr == NULL) {
    return NULL;
  }
  else {
    return ptr->value;
  }
}

/* Remove value from hash table */
int arr_delete(char *key) {
  hash_record_t *ptr, *next;
  unsigned int bin_pos;
  bin_pos = do_hash_bin(key);
  ptr = tb.bin[bin_pos];

  if (ptr == NULL) {
    return 0;
  }

  if (strcmp(key, ptr->key) == 0) {
    tb.bin[bin_pos] = ptr->next;
    free(ptr);
    tb.num_records--;
    return 1;
  }
  
  for (; ptr->next != NULL; ptr = ptr->next) {
    next = ptr->next;
    if (strcmp(next->key, key) == 0) {
      ptr->next = next->next;
      free(next);
      tb.num_records--;
      return 1;
    }
  }
  return 0;
}

/* Check if there is a value from hash table */
bool arr_find(char *key) {
  hash_record_t *ptr = NULL;
  ptr = arr_find_record(key);
  if (ptr == NULL) {
    return false;
  }
  else {
    return true;
  }
}

/* Output the number of stored keys. */
int arr_get_num(void) {
  return tb.num_records;
}
