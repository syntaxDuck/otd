#ifndef HELPERS_H
#define HELPERS_H

#include <stdlib.h>
#include <unistd.h>

int list_files(const char *, const char *);
int decode_base64url(const char *, unsigned char *, size_t *);
int extract_salt(const char *, unsigned char *);
void prompt(const char *prompt, char *output, size_t output_len);
int file_exists(const char *);
#endif
