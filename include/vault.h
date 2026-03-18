
#ifndef VAULT_H
#define VAULT_H

#include "hash_table.h"
#include "sodium/crypto_auth_hmacsha256.h"
#include "sodium/crypto_box.h"
#include "sodium/crypto_pwhash.h"
#include "sodium/crypto_secretbox.h"

#define VAULT_EXTENSION ".vault"
#define MAX_VAULT_FILE_LENGTH 100
#define MAX_VAULT_PASSWORD_LENGTH 100
#define MAX_SERVICE_NAME_LENGTH 100
#define MAX_USERNAME_LENGTH 100
#define MAX_PASSWORD_LENGTH 256

typedef struct {
  char service[MAX_SERVICE_NAME_LENGTH];
  char username[MAX_USERNAME_LENGTH];
  unsigned char encrypted_password[MAX_PASSWORD_LENGTH + crypto_secretbox_MACBYTES];
  size_t password_len;
  unsigned char nonce[crypto_secretbox_NONCEBYTES];
} PasswordEntry;

typedef struct PasswordEntryNode {
  PasswordEntry entry;
  struct PasswordEntryNode *next;
} PasswordEntryNode;

typedef struct {
  unsigned char salt[crypto_pwhash_SALTBYTES];
  unsigned char master_hash[crypto_pwhash_STRBYTES];
  unsigned char key[crypto_box_SEEDBYTES];
  unsigned char hmac[crypto_auth_hmacsha256_KEYBYTES];
  char *file_name;
  PasswordEntryNode *entries_head;
  size_t entries_count;
} Vault;

int verify_vault_password(const char *);
Vault *create_vault();
Vault *open_vault();

void encrypt_password(const char *password, size_t password_len, unsigned char *encrypted_out,
                     unsigned char *nonce, const unsigned char *key);
int decrypt_password(const unsigned char *encrypted, size_t encrypted_len,
                    unsigned char *decrypted_out, const unsigned char *nonce,
                    const unsigned char *key);
void add_password_entry(Vault *vault, const char *service, const char *username,
                       const char *password);
char *get_password_entry(Vault *vault, const char *service);
void list_password_entries(Vault *vault);
void delete_password_entry(Vault *vault, const char *service);

#endif // !VAULT_H
