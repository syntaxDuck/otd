
#ifndef VAULT_H
#define VAULT_H

#include "sodium/crypto_auth_hmacsha256.h"
#include "sodium/crypto_box.h"
#include "sodium/crypto_pwhash.h"

#define VAULT_EXTENSION ".vault"

typedef struct {
  unsigned char salt[crypto_pwhash_SALTBYTES];
  unsigned char master_hash[crypto_pwhash_STRBYTES];
  unsigned char key[crypto_box_SEEDBYTES];
  unsigned char hmac[crypto_auth_hmacsha256_KEYBYTES];
  unsigned char *vault_file;
} Vault;

int verify_vault_password(const char *);
Vault *create_vault();
Vault *open_vault();

#endif // !VAULT_H
