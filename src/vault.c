#include "vault.h"
#include "helpers.h"
#include "sodium/crypto_pwhash.h"
#include "sodium/randombytes.h"
#include <stdio.h>
#include <string.h>

int verify_vault_password(const char *hash) {
  char pw[100];
  printf("Please Enter Vault Password: ");
  fgets(pw, sizeof(pw), stdin);
  pw[strcspn(pw, "\n")] = '\0';

  return crypto_pwhash_str_verify(hash, pw, strlen(pw));
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

Vault *open_vault() {
  Vault *vault = malloc(sizeof(Vault));

  if (!vault) {
    perror("Error allocating memory for vault");
    return NULL;
  }

  char vn[100];
  printf("Please Choose an Existing Vault:\n");
  list_files(".", ".vault");
  fgets(vn, sizeof(vn), stdin);
  vn[strcspn(vn, "\n")] = '\0';
  strcat(vn, ".vault");

  char hashed_password[crypto_pwhash_STRBYTES];
  FILE *file = fopen(vn, "rb");
  if (file) {
    fread(hashed_password, 1, crypto_pwhash_STRBYTES, file);

    if (verify_vault_password(hashed_password) != 0) {
      printf("Incorrect Password!\n");
      fclose(file);
      return NULL;
    }
    strcpy((char *)vault->master_hash, hashed_password);
    extract_salt(hashed_password, vault->salt);
    fclose(file);
  } else {
    perror("Error Opening Vault");
    free(vault);
    return NULL;
  }

  return vault;
}
