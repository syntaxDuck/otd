#include "sodium/crypto_auth_hmacsha256.h"
#include "sodium/crypto_pwhash.h"
#include "sodium/randombytes.h"
#include <hash_table.h>
#include <sodium.h>
#include <stdio.h>
#include <string.h>

#define KEY_LEN crypto_box_SEEDBYTES
#define VAULT_FILE "vault1.txt"
int create_vault() {
  unsigned char salt[crypto_pwhash_SALTBYTES];
  unsigned char key[KEY_LEN];
  unsigned char hmac[crypto_auth_hmacsha256_KEYBYTES];

  char pw[100];
  fgets(pw, sizeof(pw), stdin);
  pw[strcspn(pw, "\n")] = '\0';

  randombytes_buf(salt, sizeof(salt));

  char hashed_password[crypto_pwhash_STRBYTES];
  if (crypto_pwhash_str(hashed_password, pw, strlen(pw),
                        crypto_pwhash_OPSLIMIT_SENSITIVE,
                        crypto_pwhash_MEMLIMIT_SENSITIVE) != 0) {
    perror("Error hashing password");
  }

  if (crypto_pwhash(key, sizeof(key), pw, strlen(pw), salt,
                    crypto_pwhash_OPSLIMIT_INTERACTIVE,
                    crypto_pwhash_MEMLIMIT_INTERACTIVE,
                    crypto_pwhash_ALG_DEFAULT) != 0) {
    perror("Out of memory");
  }

  if (crypto_auth_hmacsha256(hmac, (unsigned char *)hashed_password,
                             strlen(hashed_password), key) != 0) {
    perror("Error generating hmac for vault\n");
  }

  FILE *file = fopen(VAULT_FILE, "wb");
  if (file) {
    fwrite(hashed_password, 1, sizeof(hashed_password), file);
    fwrite(hmac, 1, sizeof(hashed_password), file);
    fclose(file);
    printf("Vault %s created\n", VAULT_FILE);
  } else {
    perror("Failed to create valut\n");
    return -1;
  }

  return 0;
}

int main() {
  HashTable *table = ht_create(DJB2, 0);
  if (sodium_init() < 0) {
    perror("Error initializing sodium");
    exit(1);
  }

  create_vault();

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
