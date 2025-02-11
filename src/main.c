#include "sodium/crypto_auth_hmacsha256.h"
#include "sodium/crypto_pwhash.h"
#include "sodium/randombytes.h"
#include <dirent.h>
#include <hash_table.h>
#include <sodium.h>
#include <stdio.h>
#include <string.h>

#define KEY_LEN crypto_box_SEEDBYTES
#define VAULT_FILE "vault1.txt"

typedef struct {
  unsigned char salt[crypto_pwhash_SALTBYTES];
  unsigned char master_hash[crypto_pwhash_STRBYTES];
  unsigned char key[crypto_box_SEEDBYTES];
  unsigned char hmac[crypto_auth_hmacsha256_KEYBYTES];
  unsigned char *vault_file;
} Vault;

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

Vault *create_vault() {
  Vault *vault = malloc(sizeof(Vault));

  char pw[100];
  printf("Please Enter Master Password for Vault: ");
  fgets(pw, sizeof(pw), stdin);
  pw[strcspn(pw, "\n")] = '\0';

  randombytes_buf(vault->salt, sizeof(vault->salt));

  char hashed_password[crypto_pwhash_STRBYTES];
  if (crypto_pwhash_str(hashed_password, pw, strlen(pw),
                        crypto_pwhash_OPSLIMIT_SENSITIVE,
                        crypto_pwhash_MEMLIMIT_SENSITIVE) != 0) {
    perror("Error hashing password");
  }

  if (crypto_pwhash(vault->key, sizeof(vault->key), pw, strlen(pw), vault->salt,
                    crypto_pwhash_OPSLIMIT_INTERACTIVE,
                    crypto_pwhash_MEMLIMIT_INTERACTIVE,
                    crypto_pwhash_ALG_DEFAULT) != 0) {
    perror("Error Out of memory");
  }

  if (crypto_auth_hmacsha256(vault->hmac, (unsigned char *)hashed_password,
                             strlen(hashed_password), vault->key) != 0) {
    perror("Error generating hmac for vault\n");
  }

  char vn[100];
  printf("Please Enter Vault Name: ");
  fgets(vn, sizeof(vn), stdin);
  vn[strcspn(vn, "\n")] = '\0';
  strcat(vn, ".vault");

  FILE *file = fopen(vn, "wb");
  if (file) {
    fwrite(hashed_password, 1, sizeof(hashed_password), file);
    fwrite(vault->hmac, 1, sizeof(vault->hmac), file);
    fclose(file);
    printf("Vault %s created\n\n", vn);
  } else {
    perror("Failed to create vault\n");
    return NULL;
  }

  return vault;
}

Vault *read_vault() {
  Vault *vault = malloc(sizeof(Vault));

  if (!vault) {
    perror("Error allocating memory for vault");
    return NULL;
  }

  char vn[100];
  printf("Please Enter Vault to Read from Existing Vaults:\n");
  list_files(".", ".vault");
  fgets(vn, sizeof(vn), stdin);
  vn[strcspn(vn, "\n")] = '\0'; // Remove the newline
  strcat(vn, ".vault");

  char hashed_password[crypto_pwhash_STRBYTES];
  FILE *file = fopen(vn, "rb");
  if (file) {
    fread(hashed_password, 1, crypto_pwhash_STRBYTES, file);
    hashed_password[crypto_pwhash_STRBYTES - 1] =
        '\0'; // Ensure null-termination

    printf("Hashed password: %s\n", hashed_password);

    fclose(file);
  } else {
    perror("Error Opening Vault");
    free(vault);
    return NULL;
  }

  return vault;
}

int main() {
  HashTable *table = ht_create(DJB2, 0);
  if (sodium_init() < 0) {
    perror("Error initializing sodium");
    exit(1);
  }

  Vault *vault = create_vault();
  vault = read_vault();

  // char pw1[100];
  // fgets(pw1, sizeof(pw1), stdin);
  // char hashed_password[crypto_pwhash_STRBYTES];
  // if (crypto_pwhash_str(hashed_password, pw1, strlen(pw1),
  //                       crypto_pwhash_OPSLIMIT_SENSITIVE,
  //                       crypto_pwhash_MEMLIMIT_SENSITIVE) != 0) {
  //   perror("Error hashing password");
  // }
  //
  // if (crypto_pwhash_str_verify(hashed_password, pw1, strlen(pw1)) != 0) {
  //   perror("Error wrong password");
  // }
  //
  // printf("%s, %s\n", pw1, hashed_password);

  return 0;
}
