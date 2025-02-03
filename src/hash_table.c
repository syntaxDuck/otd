#include "hash_table.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned long ht_get_index(int hash_type, const char *str, size_t size) {

  unsigned long index;
  switch (hash_type) {
  case DJB2:
  default:
    index = djb2_hash(str, size);
    break;
  }

  return index;
};

unsigned long djb2_hash(const char *str, size_t size) {
  unsigned long hash = 5381;
  int c;
  while ((c = *str++)) {
    hash = ((hash << 5) + hash) + c;
  }
  return hash % size;
};

HashTable *ht_create(int hash_type, size_t size) {
  HashTable *table = malloc(sizeof(HashTable));
  if (!table)
    return NULL;
  table->hash_type = hash_type;
  table->size = size;
  table->buckets = calloc(size, sizeof(Entry *));
  if (!table->buckets) {
    free(table);
    return NULL;
  }
  return table;
}

void free_ht(HashTable *table) {
  for (int i = 0; i < table->size; i++) {
    if (table->buckets[i]) {
      free_entries(table->buckets[i]);
    }
  }
  free(table->buckets);
  free(table);
}

void free_entry(Entry *entry) {
  free(entry->key);
  free(entry->val);
  free(entry);
}

void free_entries(Entry *entry) {
  while (entry) {
    Entry *next = entry->next;
    free(entry->key);
    free(entry->val);
    free(entry);
    entry = next;
  }
}

int ht_insert(HashTable *table, char *key, char *val) {
  unsigned long index = ht_get_index(table->hash_type, key, table->size);
  bool first = false;

  Entry *existing_entry = table->buckets[index];
  if (!existing_entry) {
    table->buckets[index] = malloc(sizeof(Entry));
    existing_entry = table->buckets[index];
    first = true;
  }

  while (existing_entry->next) {
    // If key already exists update value
    if (strcmp(existing_entry->key, key) == 0) {
      free(existing_entry->val);
      existing_entry->val = strdup(val);
      return 0;
    }
    existing_entry = existing_entry->next;
  }

  Entry *new_entry = malloc(sizeof(Entry));
  if (!new_entry)
    return 1;
  new_entry->key = strdup(key);
  new_entry->val = strdup(val);

  if (!first)
    new_entry->next = table->buckets[index];
  table->buckets[index] = new_entry;
  table->num_entries++;
  return 0;
}

int ht_remove(HashTable *table, char *key) {
  unsigned long index = ht_get_index(table->hash_type, key, table->size);
  Entry *prev_entry = NULL;
  Entry *entry = table->buckets[index];

  while (entry) {
    if (strcmp(entry->key, key) == 0) {
      if (prev_entry) {
        prev_entry->next = entry->next;
      } else {
        table->buckets[index] = entry->next;
      }
      free_entry(entry);
      table->num_entries--;
      return 0; // Successfully removed
    }
    prev_entry = entry;
    entry = entry->next;
  }

  return 1; // Key not found
}

Entry *ht_get_entry(HashTable *table, char *key) {
  unsigned long index = ht_get_index(table->hash_type, key, table->size);
  Entry *entry = table->buckets[index];

  while (entry) {
    if (strcmp(entry->key, key) == 0) {
      return entry; // Found the entry
    }
    entry = entry->next;
  }

  return NULL; // Entry not found
}

void ht_print(HashTable *table) {
  for (unsigned long i = 0; i < table->size; i++) {
    if (table->buckets[i]) {
      printf("Index: %lu\n", i);
      Entry *entry = table->buckets[i];
      while (entry) {
        printf("  Key: %s, Val: %s\n", entry->key, entry->val);
        entry = entry->next;
      }
    } else {
      printf("Index: %lu is NULL\n", i);
    }
  }
}

void entry_print(const Entry *entry) {
  printf("Key: %s, Val: %s, Next: %p\n", entry->key, entry->val, entry->next);
}

void ht_resize(HashTable **table, size_t new_size) {
  HashTable *new_table = ht_create((*table)->hash_type, new_size);
  for (size_t i = 0; i < (*table)->size; i++) {
    Entry *entry = (*table)->buckets[i];
    while (entry) {
      ht_insert(new_table, entry->key, entry->val);
      entry = entry->next;
    }
  }
  free_ht(*table);
  *table = new_table;
}

void ht_clear(HashTable *table) {
  for (size_t i = 0; i < table->size; i++) {
    free_entries(table->buckets[i]);
    table->buckets[i] = NULL;
  }
  table->num_entries = 0;
}
