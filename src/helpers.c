#include "helpers.h"
#include "sodium/crypto_pwhash.h"
#include "sodium/utils.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int list_files(const char *path, const char *ext) {
  struct dirent *entry;
  DIR *dir = opendir(path);

  if (!dir) {
    perror("Unable to open directory");
    return -1;
  }

  while ((entry = readdir(dir)) != NULL) {
    if (strstr(entry->d_name, ext)) {
      printf("%s\n", entry->d_name);
    }
  }
  return 0;
}

int file_exists(const char *filename) { return access(filename, F_OK) == 0; }

void prompt(const char *prompt, char *output, size_t output_len) {
  printf("%s", prompt);
  fgets(output, output_len, stdin);
  output[strcspn(output, "\n")] = '\0';
}

int decode_base64url(const char *base64_str, unsigned char *output,
                     size_t *output_len) {
  if (sodium_base642bin(output, *output_len, base64_str, strlen(base64_str),
                        NULL, output_len, NULL,
                        sodium_base64_VARIANT_ORIGINAL_NO_PADDING) != 0) {
    perror("Decoding base64 string failed");
    return -1;
  }
  return 0;
}

int extract_salt(const char *hashed_password, unsigned char *salt) {
  char *salt_start = strchr(hashed_password, '$');
  salt_start = strchr(salt_start + 1, '$');
  salt_start = strchr(salt_start + 1, '$');
  salt_start = strchr(salt_start + 1, '$');
  char *salt_end = strchr(salt_start + 1, '$');

  if (salt_start != NULL && salt_end != NULL) {
    size_t salt_length = salt_end - salt_start - 1;
    char salt_b64url[salt_length + 1];
    strncpy(salt_b64url, salt_start + 1, salt_length);
    salt_b64url[salt_length] = '\0';

    size_t decoded_len = crypto_pwhash_SALTBYTES;
    decode_base64url(salt_b64url, salt, &decoded_len);
  } else {
    perror("Error extracting salt form hashed string");
    return -1;
  }
  return 0;
}
