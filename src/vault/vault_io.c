#include "vault.h"
#include "vault/vault_io.h"
#include <stdio.h>

int write_vault(Vault *vault) {
  FILE *file = fopen(vault->file_name, "wb");
  if (!file) {
    perror("Failed to create vault file");
    return -1;
  }
  fwrite(vault->salt, 1, sizeof(vault->salt), file);
  fwrite(vault->master_hash, 1, sizeof(vault->master_hash), file);
  fwrite(vault->key, 1, sizeof(vault->key), file);
  fwrite(vault->hmac, 1, sizeof(vault->hmac), file);
  fclose(file);
  return 0;
}

int read_vault(Vault *vault) {
  FILE *file = fopen(vault->file_name, "rb");
  if (!file) {
    perror("Error opening vault file");
    return -1;
  }
  fread(vault->salt, 1, sizeof(vault->salt), file);
  fread(vault->master_hash, 1, crypto_pwhash_STRBYTES, file);
  fread(vault->key, 1, sizeof(vault->key), file);
  fread(vault->hmac, 1, sizeof(vault->hmac), file);
  fclose(file);
  return 0;
}
