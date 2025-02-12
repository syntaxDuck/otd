#ifndef HELPERS_H
#define HELPERS_H

#include <stdlib.h>

int list_files(const char *, const char *);
int decode_base64url(const char *, unsigned char *, size_t *);
int extract_salt(const char *, unsigned char *);
#endif
