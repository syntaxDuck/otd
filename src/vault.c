#include "vault.h"
#include "helpers.h"
#include "sodium/crypto_pwhash.h"
#include "sodium/randombytes.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// TODO: Create a write a read vault function
int handle_create_vault_user_input(char *vault_name, size_t buff_size) {
  while (true) {
    prompt("Please Enter Vault Name (q: Quit): ", vault_name, buff_size);
    if (tolower(vault_name[0]) == 'q' && vault_name[1] == '\0') {
      printf("Aborting vault creation...\n");
      return 1;
    }
    strcat(vault_name, ".vault");
    if (file_exists(vault_name)) {
      printf("Vault already exists, would you like to overwrite? (Y/n): ");
      char buffer[10] = {}; // Buffer to store input
      bool is_done = false;
      while (!is_done) {
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
          continue;
        }
        switch (tolower(buffer[0])) {
        case 'y':
          printf("Overwriting vault...\n");
          return 0;
        case 'n':
          is_done = true;
          break;
        default:
          printf("Invalid, type Y/n: ");
          break;
        }
      }
    } else
      return 0;
  }
  return -1;
}
int write_vault(const char *vault_name, Vault *vault) {
  FILE *file = fopen(vault_name, "wb");
  if (file) {
    fwrite(vault->master_hash, 1, sizeof(vault->master_hash), file);
    fwrite(vault->hmac, 1, sizeof(vault->hmac), file);
    fclose(file);
    return 0;
  } else {
    return -1;
  }
}

int verify_vault_password(const char *hash) {
  char pw[100];
  prompt("Please Enter Vault Password: ", pw, sizeof(pw));

  return crypto_pwhash_str_verify(hash, pw, strlen(pw));
}

Vault *create_vault() {
  Vault *vault = malloc(sizeof(Vault));

  char vn[100];
  if (handle_create_vault_user_input(vn, sizeof(vn)) != 0)
    return NULL;

  char pw[100];
  prompt("Please Enter Master Password for Vault: ", pw, sizeof(pw));

  // Create Sault for Vault
  randombytes_buf(vault->salt, sizeof(vault->salt));

  // Hash Master Password
  if (crypto_pwhash_str((char *)vault->master_hash, pw, strlen(pw),
                        crypto_pwhash_OPSLIMIT_SENSITIVE,
                        crypto_pwhash_MEMLIMIT_SENSITIVE) != 0) {
    perror("Error hashing password");
  }

  // Generate HMAC for file
  if (crypto_auth_hmacsha256(vault->hmac, (unsigned char *)vault->master_hash,
                             sizeof(vault->master_hash), vault->key) != 0) {
    perror("Error generating hmac for vault\n");
  }

  if (write_vault(vn, vault) != 0) {
    perror("Failed to create vault\n");
    return NULL;
  }
  printf("Vault %s created\n\n", vn);

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

void create_key() {
  // if (crypto_pwhash(vault->key, sizeof(vault->key), pw, strlen(pw),
  // vault->salt,
  //                     crypto_pwhash_OPSLIMIT_INTERACTIVE,
  //                     crypto_pwhash_MEMLIMIT_INTERACTIVE,
  //                     crypto_pwhash_ALG_DEFAULT) != 0) {
  //     perror("Error Out of memory");
  //   }
}
