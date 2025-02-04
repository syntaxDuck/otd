#include "hash_table.h"
#include <stdio.h>
#include <string.h>

void ht_set_hash_function(HashTable *table,
                          unsigned long (*hash_functon)(const char *, size_t)) {
  table->hash_function = hash_functon;
}

unsigned long ht_get_index(HashTable *table, const char *str) {
  return table->hash_function(str, table->size);
};

unsigned long djb2_hash(const char *str, size_t size) {
  unsigned long hash = 5381;
  int c;
  while ((c = *str++)) {
    hash = ((hash << 5) + hash) + c;
  }
  return hash % size;
};

// TODO: Currently we have a hashtype member of the struct I'm not a huge fan of
// this but we will just keep it around for now
HashTable *ht_create(int hash_type, size_t size) {
  HashTable *table = malloc(sizeof(HashTable));
  if (!table)
    return NULL;
  table->size = size != 0 ? size : DEFAULT_TABLE_SIZE;
  table->buckets = calloc(table->size, sizeof(Entry *));
  if (!table->buckets) {
    free(table);
    return NULL;
  }
  table->max_load_factor = DEFAULT_LOAD_FACTOR;
  table->hash_type = hash_type;

  switch (hash_type) {
  case DJB2:
  default:
    table->hash_function = djb2_hash;
    break;
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
  unsigned long index = ht_get_index(table, key);
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

  // BUG: This currently cant be called here becuase the loacal address storing
  // our pointer is not the same as the address it is stored from the caller. So
  // in resize the table pointer gets freed but the new pointer is never
  // assinged to the original address. We can fix this by passing a double
  // pointer **table to this ht_set_hash_function but currently I'm not sure how
  // I feel about that.
  //  if (ht_get_load_factor(table) > table->max_load_factor)
  //    ht_resize(&table, table->size * 2);
  return 0;
}

bool ht_contains(HashTable *table, char *key) {
  Entry *entry = ht_get_entry(table, key);
  return entry != NULL;
}

int ht_remove(HashTable *table, char *key) {
  unsigned long index = ht_get_index(table, key);
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
  unsigned long index = ht_get_index(table, key);
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
  printf("\n");
}

void entry_print(const Entry *entry) {
  if (entry)
    printf("Key: %s, Val: %s, Next: %p\n", entry->key, entry->val, entry->next);
  else
    printf("Entry is Null\n");
}

HashTable *ht_copy_table(HashTable *table, size_t size) {
  HashTable *new_table = ht_create(table->hash_type, size);
  new_table->hash_type = table->hash_type;
  new_table->hash_function = table->hash_function;
  new_table->max_load_factor = table->max_load_factor;
  new_table->num_entries = 0;

  for (size_t i = 0; i < table->size; i++) {
    Entry *entry = table->buckets[i];
    while (entry) {
      ht_insert(new_table, entry->key, entry->val);
      entry = entry->next;
    }
  }

  return new_table;
}

void ht_resize(HashTable **table, size_t new_size) {
  HashTable *new_table = ht_copy_table(*table, new_size);
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

float ht_get_load_factor(HashTable *table) {
  return (float)table->num_entries / table->size;
}

void ht_print_load_factor(HashTable *table) {
  printf("Max Load Facotr: %f, Current Load Factor: %f\n",
         table->max_load_factor, ht_get_load_factor(table));
}
