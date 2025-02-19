#ifndef VAULT_HELPERS
#define VAULT_HELPERS
#include <stdlib.h>

int list_files(const char *path, const char *ext);
int file_exists(const char *filename);
void prompt(const char *prompt, char *output, size_t output_len);
int decode_base64url(const char *base64_str, unsigned char *output,
                     size_t *output_len);
int extract_salt(const char *hashed_password, unsigned char *salt);

#endif // !VAULT_HELPRERS
