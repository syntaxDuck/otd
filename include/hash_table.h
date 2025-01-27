#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stddef.h>
#include <stdlib.h>

typedef struct Entry {
  char *key;
  char *val;
  struct Entry *next;
} Entry;

typedef struct {
  Entry **buckets;
  size_t size;
  int hash_type;

} HashTable;

enum hash_function {
  DJB2,
};

unsigned long ht_get_index(int hash_type, const char *str, size_t size);
unsigned long djb2_hash(const char *str, size_t size);
HashTable ht_create(int hash_type, size_t size);
void ht_free(HashTable table);
void free_entry(Entry *entry);
void ht_insert(HashTable htable, char *key, char *val);
Entry ht_get_entry(HashTable table, char *key);
void ht_print(HashTable table);
#endif
