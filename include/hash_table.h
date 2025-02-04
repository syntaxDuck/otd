#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#define DEFAULT_TABLE_SIZE 16
#define DEFAULT_LOAD_FACTOR 0.75

typedef struct Entry {
  char *key;
  char *val;
  struct Entry *next;
} Entry;

typedef struct {
  Entry **buckets;
  size_t size;
  size_t num_entries;
  short hash_type;
  unsigned long (*hash_function)(const char *, size_t);
  float max_load_factor;
} HashTable;

enum hash_function { DJB2, CUSTOM };

// Hash-related functions
unsigned long ht_get_index(HashTable *table, const char *str);
unsigned long djb2_hash(const char *str, size_t size);

// Core functionality
HashTable *ht_create(int hash_type, size_t size);
int ht_insert(HashTable *htable, char *key, char *val);
Entry *ht_get_entry(HashTable *table, char *key);
int ht_remove(HashTable *table, char *key);
void ht_resize(HashTable **table, size_t new_size);
void ht_clear(HashTable *table);
bool ht_contains(HashTable *table, char *key);
float ht_get_load_factor(HashTable *table);
HashTable *ht_copy_table(HashTable *table, size_t size);

// Utility functions
void ht_print(HashTable *table);
void ht_print_load_factor(HashTable *table);
void entry_print(const Entry *entry);

// Memory management
void free_ht(HashTable *table);
void free_entries(Entry *entry);
void free_entry(Entry *entry);

#endif
