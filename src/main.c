#include "hash_table.h"
#include <stdio.h>

int main() {
  HashTable table = ht_create(DJB2, 10);

  printf("Hash Type: %d, Size: %lu\n", table.hash_type, table.size);

  ht_insert(table, "apple", "red");
  ht_insert(table, "banana", "yellow");
  ht_insert(table, "grape", "purple");
  ht_insert(table, "orange", "orange");
  ht_insert(table, "strawberry", "red");
  ht_insert(table, "blueberry", "blue");
  ht_insert(table, "cherry", "red");
  ht_insert(table, "watermelon", "green");
  ht_insert(table, "mango", "orange");
  ht_insert(table, "peach", "pink");
  ht_insert(table, "pear", "green");
  ht_insert(table, "pineapple", "yellow");
  ht_insert(table, "plum", "purple");
  ht_insert(table, "kiwi", "brown");
  ht_insert(table, "coconut", "brown");
  ht_insert(table, "avocado", "green");
  ht_insert(table, "lemon", "yellow");
  ht_insert(table, "lime", "green");
  ht_insert(table, "pomegranate", "red");
  ht_insert(table, "cranberry", "red");
  ht_insert(table, "fig", "purple");
  ht_insert(table, "apricot", "orange");
  ht_insert(table, "blackberry", "black");
  ht_insert(table, "raspberry", "red");
  // ht_insert(table, "papaya", "orange");
  // ht_insert(table, "dragonfruit", "pink");
  // ht_insert(table, "passionfruit", "purple");
  // ht_insert(table, "cantaloupe", "orange");
  // ht_insert(table, "honeydew", "green");
  // ht_insert(table, "persimmon", "orange");
  // ht_insert(table, "date", "brown");
  // ht_insert(table, "olive", "green");
  // ht_insert(table, "gooseberry", "green");
  // ht_insert(table, "lychee", "red");
  // ht_insert(table, "mulberry", "black");
  // ht_insert(table, "tangerine", "orange");
  // ht_insert(table, "boysenberry", "purple");
  // ht_insert(table, "currant", "red");
  // ht_insert(table, "elderberry", "black");
  // ht_insert(table, "jackfruit", "yellow");
  // ht_insert(table, "kumquat", "orange");
  // ht_insert(table, "longan", "brown");
  // ht_insert(table, "starfruit", "yellow");
  // ht_insert(table, "soursop", "green");
  // ht_insert(table, "quince", "yellow");
  // ht_insert(table, "guava", "green");
  // ht_insert(table, "cherimoya", "green");
  // ht_insert(table, "sapodilla", "brown");
  // ht_insert(table, "tamarind", "brown");
  // ht_insert(table, "marionberry", "black");
  // ht_insert(table, "ackee", "red");

  // ht_print(table);

  ht_remove(table, "peach");

  printf("\n\n");
  ht_print(table);

  return 0;
}
