#include "hash_table.h"
#include <_string.h>
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

HashTable ht_create(int hash_type, size_t size) {
  HashTable table;
  table.hash_type = hash_type;
  table.size = size;
  table.buckets = calloc(size, sizeof(Entry));
  return table;
}

void ht_free(HashTable table) {
  for (int i = 0; i < table.size; i++) {
    if (table.buckets[i]) {
      free_entry(table.buckets[i]);
    }
  }
}

// TODO: Need to make function that clear the linked list and a function that
// only frees that singular entry
void free_entry(Entry *entry) {
  free(entry->key);
  free(entry->val);
  if (entry->next)
    free_entry(entry->next);
  free(entry);
}

void ht_insert(HashTable table, char *key, char *val) {
  static int count = 0;
  unsigned long index = ht_get_index(table.hash_type, key, table.size);

  if (table.buckets[index]) {
    Entry *existing_entry = table.buckets[index];
    while (existing_entry->next) {
      if (strcmp(existing_entry->key, key) == 0) {
        free(existing_entry->val);
        existing_entry->val = strdup(val);
        return;
      }
      existing_entry = existing_entry->next;
    }

    existing_entry->next = malloc(sizeof(Entry));
    existing_entry->next->key = strdup(key);
    existing_entry->next->val = strdup(val);
    existing_entry->next->next = NULL;
  } else {
    Entry *new_entry = malloc(sizeof(Entry));
    new_entry->key = strdup(key);
    new_entry->val = strdup(val);
    new_entry->next = NULL;
    table.buckets[index] = new_entry;
  }
}

// TODO: Finish testing this I think the only, last, and inbetween cases are
// working
int ht_remove(HashTable table, char *key) {
  Entry **entry_stack = malloc(sizeof(Entry) * table.size);
  unsigned int stack_len = 0;

  Entry *entry = table.buckets[ht_get_index(table.hash_type, key, table.size)];
  while (true) {
    if (strcmp(entry->key, key) == 0) {
      stack_len--;
      break;
    }

    entry_stack[stack_len] = entry;
    stack_len++;
    entry = entry->next;

    if (!entry)
      return 1;
  }

  if (!entry->next && !stack_len) {
    free_entry(entry);
    table.buckets[ht_get_index(table.hash_type, key, table.size)] = NULL;
    return 0;
  }

  // Has following nodes
  if (entry->next) {

    // Is first node in ll
    if (!stack_len) {
      table.buckets[ht_get_index(table.hash_type, key, table.size)] =
          entry->next;
      memcpy(entry, entry->next, sizeof(Entry));
      free(entry->next);
    }

    // Inbetween node
    else {
      entry_stack[stack_len]->next = entry->next;
      free_entry(entry);
    }

  }

  // Is last node in ll
  else {
    free_entry(entry_stack[stack_len]->next);
    entry_stack[stack_len]->next = NULL;
  }

  free(entry_stack);
  return 0;
}

Entry ht_get_entry(HashTable table, char *key) {
  unsigned long index = ht_get_index(table.hash_type, key, table.size);
  Entry entry = *table.buckets[index];
  return entry;
}

void ht_print(HashTable table) {
  for (unsigned long i = 0; i < table.size; i++) {
    if (table.buckets[i]) {
      printf("Index: %lu Key: %s Val: %s\n", i, table.buckets[i]->key,
             table.buckets[i]->val);
      Entry *next = table.buckets[i]->next;
      int node = 1;
      while (next) {
        for (int j = 0; j < node; j++)
          printf("⎯⎯⎯⎯");

        printf("Node: %d Key: %s Val: %s\n", node, next->key, next->val);
        next = next->next;
        node++;
      }
    } else
      printf("Index: %lu is NULL\n", i);
  }
}

void entry_print(Entry entry) {
  printf("Key: %s, Val: %s, Next: %p\n", entry.key, entry.val, entry.next);
}
