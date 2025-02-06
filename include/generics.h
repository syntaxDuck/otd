#ifndef GENERICS_H
#define GENERICS_H

#include <stdlib.h>

typedef enum {
  // Fundamental Integer Types
  TYPE_CHAR,
  TYPE_SIGNED_CHAR,
  TYPE_UNSIGNED_CHAR,
  TYPE_SHORT,
  TYPE_UNSIGNED_SHORT,
  TYPE_INT,
  TYPE_UNSIGNED_INT,
  TYPE_LONG,
  TYPE_UNSIGNED_LONG,
  TYPE_LONG_LONG,
  TYPE_UNSIGNED_LONG_LONG,

  // Floating Point Types
  TYPE_FLOAT,
  TYPE_DOUBLE,
  TYPE_LONG_DOUBLE,

  // Fixed-Width Integer Types (<stdint.h>)
  TYPE_INT8,
  TYPE_UINT8,
  TYPE_INT16,
  TYPE_UINT16,
  TYPE_INT32,
  TYPE_UINT32,
  TYPE_INT64,
  TYPE_UINT64,

  // Standard Library Types
  TYPE_SIZE_T,
  TYPE_SSIZE_T,
  TYPE_PTRDIFF_T,
  TYPE_INTPTR_T,
  TYPE_UINTPTR_T,

  // Boolean Type
  TYPE_BOOL,

  // Pointer Type (Generic)
  TYPE_CHAR_POINTER,
  TYPE_POINTER

} DataType;

typedef struct GenericValue {
  void *val;
  size_t size;
  DataType type;
} GenericValue;

GenericValue create_generic(void *val, size_t size, DataType type);
void set_generic(GenericValue *gv, void *val, size_t size, DataType type);
void print_generic(const GenericValue gv);

#endif
