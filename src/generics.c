#include "generics.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GenericValue create_generic(void *val, size_t size, DataType type) {
  GenericValue gv;
  gv.val = NULL;
  set_generic(&gv, val, size, type);
  return gv;
}

void set_generic(GenericValue *gv, void *val, size_t size, DataType type) {
  if (gv->type != type || gv->size != size) {
    if (gv->val != NULL)
      free(gv->val);
    gv->val = malloc(size);
    if (gv->val == NULL) {
      perror("Failed to allocate memory for generic value");
      exit(1);
    }
    gv->size = size;
    gv->type = type;
  }
  memcpy(gv->val, val, size);
}

void print_generic(const GenericValue gv) {
  switch (gv.type) {
  case TYPE_CHAR:
    printf("%c", *(char *)(gv.val));
  case TYPE_INT:
    printf("%d", *(int *)(gv.val));
  case TYPE_CHAR_POINTER:
    printf("%s", (char *)gv.val);
    break;
  default:
    break;
  }
}
