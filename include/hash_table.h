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
  size_t num_entries;
  int hash_type;
} HashTable;

enum hash_function {
  DJB2,
};

// Hash-related functions
unsigned long ht_get_index(int hash_type, const char *str, size_t size);
unsigned long djb2_hash(const char *str, size_t size);

// Core functionality
HashTable *ht_create(int hash_type, size_t size);
int ht_insert(HashTable *htable, char *key, char *val);
Entry *ht_get_entry(HashTable *table, char *key);
int ht_remove(HashTable *table, char *key);
void ht_resize(HashTable **table, size_t new_size);
void ht_clear(HashTable *table);

// Utility functions
void ht_print(HashTable *table);
void entry_print(const Entry *entry);

// Memory management
void free_ht(HashTable *table);
void free_entries(Entry *entry);
void free_entry(Entry *entry);

#endif
