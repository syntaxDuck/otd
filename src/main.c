#include "generics.h"
#include "hash_table.h"
#include "sodium.h"
#include <stdio.h>
#include <string.h>

int main() {
  HashTable *table = ht_create(DJB2, 0);
  if (sodium_init() < 0) {
    perror("Error initializing sodium");
    exit(1);
  }
  printf("We did it!");
  return 0;
}
