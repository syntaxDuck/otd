#include "vault.h"
#include "helpers.h"
#include "sodium/crypto_auth_hmacsha256.h"
#include "sodium/crypto_pwhash.h"
#include "sodium/randombytes.h"
#include <ctype.h>
#include <sodium.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---- Helper Functions ---- */

int confirm_overwrite() {
  char response[10];
  while (true) {
    prompt("Vault already exists, would you like to overwrite? (Y/n): ",
           response, sizeof(response));
    switch (tolower(response[0])) {
    case 'y':
      return 1;
    case 'n':
      return 0;
    default:
      printf("Invalid, type Y/n: ");
    }
  }
}

int get_vault_name_input(char *vault_name, size_t buff_size) {
  while (true) {
    prompt("Please Enter Vault Name (q: Quit): ", vault_name, buff_size);
    if (tolower(vault_name[0]) == 'q' && vault_name[1] == '\0') {
      printf("Aborting vault creation...\n");
      return 1;
    }
    strcat(vault_name, VAULT_EXTENSION);
    if (!file_exists(vault_name) || confirm_overwrite()) {
      return 0;
    }
  }
}

int hash_master_password(const char *password, char *hashed_output) {
  return crypto_pwhash_str(hashed_output, password, strlen(password),
                           crypto_pwhash_OPSLIMIT_SENSITIVE,
                           crypto_pwhash_MEMLIMIT_SENSITIVE);
}

int write_vault(Vault *vault) {
  FILE *file = fopen(vault->file_name, "wb");
  if (!file) {
    perror("Failed to create vault file");
    return -1;
  }
  fwrite(vault->master_hash, 1, sizeof(vault->master_hash), file);
  fwrite(vault->hmac, 1, sizeof(vault->hmac), file);
  fclose(file);
  return 0;
}

Vault *init_vault() {
  Vault *vault = malloc(sizeof(Vault));
  if (!vault) {
    perror("Error allocating memory for vault");
    return NULL;
  }
  vault->file_name = malloc(MAX_VAULT_FILE_LENGTH);
  if (!vault->file_name) {
    perror("Error allocating memory for vault file");
    free(vault);
    return NULL;
  }
  return vault;
}

/* ---- Vault Creation ---- */

Vault *create_vault() {
  Vault *vault = init_vault();
  if (get_vault_name_input(vault->file_name, MAX_VAULT_FILE_LENGTH != 0)) {
    free(vault);
    return NULL;
  }

  char password[MAX_VAULT_PASSWORD_LENGTH];
  prompt("Please Enter Master Password for Vault: ", password,
         sizeof(password));

  randombytes_buf(vault->salt, sizeof(vault->salt));

  if (hash_master_password(password, (char *)vault->master_hash) != 0) {
    perror("Error hashing password");
    free(vault);
    return NULL;
  }

  if (crypto_auth_hmacsha256(vault->hmac, vault->master_hash,
                             sizeof(vault->master_hash), vault->key) != 0) {
    perror("Error generating HMAC for vault\n");
    free(vault);
    return NULL;
  }

  if (write_vault(vault) != 0) {
    free(vault);
    return NULL;
  }

  printf("Vault %s created successfully.\n\n", vault->file_name);
  return vault;
}

int get_existing_vault_name(char *vault_name, size_t size) {
  printf("Please Choose an Existing Vault:\n");
  list_files(".", VAULT_EXTENSION);
  fgets(vault_name, size, stdin);
  vault_name[strcspn(vault_name, "\n")] = '\0';
  strcat(vault_name, VAULT_EXTENSION);
  return file_exists(vault_name) ? 0 : -1;
}

int read_vault_data(Vault *vault) {
  FILE *file = fopen(vault->file_name, "rb");
  if (!file) {
    perror("Error opening vault file");
    return -1;
  }
  fread(vault->master_hash, 1, crypto_pwhash_STRBYTES, file);
  fclose(file);
  return 0;
}

int validate_vault_password(const char *hash) {
  char password[100];
  prompt("Please Enter Vault Password: ", password, sizeof(password));
  return crypto_pwhash_str_verify(hash, password, strlen(password));
}

Vault *open_vault() {
  Vault *vault = init_vault();

  if (get_existing_vault_name(vault->file_name, MAX_VAULT_FILE_LENGTH != 0)) {
    free(vault);
    return NULL;
  }

  if (read_vault_data(vault) != 0) {
    free(vault);
    return NULL;
  }

  if (validate_vault_password((char *)vault->master_hash) != 0) {
    printf("Incorrect Password!\n");
    free(vault);
    return NULL;
  }

  printf("Vault %s opened successfully.\n", vault->file_name);
  return vault;
}

void create_key() {
  // if (crypto_pwhash(vault->key, sizeof(vault->key), pw, strlen(pw),
  // vault->salt,
  //                     crypto_pwhash_OPSLIMIT_INTERACTIVE,
  //                     crypto_pwhash_MEMLIMIT_INTERACTIVE,
  //                     crypto_pwhash_ALG_DEFAULT) != 0) {
  //     perror("Error Out of memory");
  //   }
}
