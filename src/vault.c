#include "vault.h"
#include "vault/vault_helpers.h"
#include "vault/vault_io.h"
#include "sodium/crypto_auth_hmacsha256.h"
#include "sodium/crypto_pwhash.h"
#include "sodium/crypto_secretbox.h"
#include "sodium/randombytes.h"
#include <ctype.h>
#include <sodium.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// -- Helper Functions -- //

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
  vault->entries_head = NULL;
  vault->entries_count = 0;
  return vault;
}

Vault *create_vault() {
  Vault *vault = init_vault();
  if (get_vault_name_input(vault->file_name, MAX_VAULT_FILE_LENGTH)) {
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

  if (crypto_pwhash(vault->key, sizeof(vault->key), password, strlen(password),
                   vault->salt, crypto_pwhash_OPSLIMIT_INTERACTIVE,
                   crypto_pwhash_MEMLIMIT_INTERACTIVE,
                   crypto_pwhash_ALG_DEFAULT) != 0) {
    perror("Error deriving key");
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

int validate_vault_password(const char *hash) {
  char password[100];
  prompt("Please Enter Vault Password: ", password, sizeof(password));
  return crypto_pwhash_str_verify(hash, password, strlen(password));
}

int verify_vault_password(const char *hash) {
  return validate_vault_password(hash);
}

Vault *open_vault() {
  Vault *vault = init_vault();

  if (get_existing_vault_name(vault->file_name, MAX_VAULT_FILE_LENGTH)) {
    free(vault);
    return NULL;
  }

  if (read_vault(vault) != 0) {
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

// -- Password Entry Functions -- //

void encrypt_password(const char *password, size_t password_len, unsigned char *encrypted_out,
                     unsigned char *nonce, const unsigned char *key) {
  randombytes_buf(nonce, crypto_secretbox_NONCEBYTES);
  crypto_secretbox_easy(encrypted_out, (const unsigned char *)password, password_len,
                        nonce, key);
}

int decrypt_password(const unsigned char *encrypted, size_t encrypted_len,
                    unsigned char *decrypted_out, const unsigned char *nonce,
                    const unsigned char *key) {
  return crypto_secretbox_open_easy(decrypted_out, encrypted, encrypted_len,
                                    nonce, key);
}

void add_password_entry(Vault *vault, const char *service, const char *username,
                       const char *password) {
  PasswordEntryNode *node = malloc(sizeof(PasswordEntryNode));
  if (!node) {
    perror("Error allocating memory for password entry");
    return;
  }

  strncpy(node->entry.service, service, MAX_SERVICE_NAME_LENGTH - 1);
  node->entry.service[MAX_SERVICE_NAME_LENGTH - 1] = '\0';
  strncpy(node->entry.username, username, MAX_USERNAME_LENGTH - 1);
  node->entry.username[MAX_USERNAME_LENGTH - 1] = '\0';

  size_t password_len = strlen(password);
  node->entry.password_len = password_len + crypto_secretbox_MACBYTES;

  encrypt_password(password, password_len, node->entry.encrypted_password,
                   node->entry.nonce, vault->key);

  node->next = vault->entries_head;
  vault->entries_head = node;
  vault->entries_count++;

  printf("Password for %s saved.\n", service);
}

char *get_password_entry(Vault *vault, const char *service) {
  PasswordEntryNode *node = vault->entries_head;
  while (node) {
    if (strncmp(node->entry.service, service, MAX_SERVICE_NAME_LENGTH) == 0) {
      unsigned char *decrypted = malloc(MAX_PASSWORD_LENGTH);
      if (decrypt_password(node->entry.encrypted_password,
                           node->entry.password_len,
                           decrypted,
                           node->entry.nonce,
                           vault->key) == 0) {
        decrypted[node->entry.password_len - crypto_secretbox_MACBYTES] = '\0';
        return (char *)decrypted;
      } else {
        printf("Decryption failed.\n");
        return NULL;
      }
    }
    node = node->next;
  }
  printf("Service %s not found.\n", service);
  return NULL;
}

void list_password_entries(Vault *vault) {
  printf("\n=== Password Entries ===\n");
  PasswordEntryNode *node = vault->entries_head;
  if (!node) {
    printf("(No entries)\n");
  }
  while (node) {
    printf("Service: %s | Username: %s\n", node->entry.service, node->entry.username);
    node = node->next;
  }
  printf("========================\n\n");
}

void delete_password_entry(Vault *vault, const char *service) {
  PasswordEntryNode *node = vault->entries_head;
  PasswordEntryNode *prev = NULL;
  while (node) {
    if (strncmp(node->entry.service, service, MAX_SERVICE_NAME_LENGTH) == 0) {
      if (prev) {
        prev->next = node->next;
      } else {
        vault->entries_head = node->next;
      }
      free(node);
      vault->entries_count--;
      printf("Deleted entry for %s.\n", service);
      return;
    }
    prev = node;
    node = node->next;
  }
  printf("Service %s not found.\n", service);
}
