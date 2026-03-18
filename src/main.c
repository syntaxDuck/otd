#include "vault.h"
#include "vault/vault_interaction.h"
#include <sodium.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_menu() {
  printf("\n=== OTD Password Vault ===\n");
  printf("1. Create new vault\n");
  printf("2. Open existing vault\n");
  printf("3. Quit\n");
  printf("=========================\n");
  printf("Enter choice: ");
}

int main() {
  if (sodium_init() < 0) {
    perror("Error initializing sodium");
    exit(1);
  }

  while (1) {
    print_menu();

    char choice[10];
    if (!fgets(choice, sizeof(choice), stdin)) {
      break;
    }
    choice[strcspn(choice, "\n")] = '\0';

    switch (choice[0]) {
    case '1':
      if (create_vault()) {
        printf("Vault created successfully.\n");
      }
      break;

    case '2': {
      Vault *vault = open_vault();
      if (vault) {
        vault_interaction_loop(vault);
        free(vault);
      }
      break;
    }

    case '3':
      printf("Goodbye!\n");
      return 0;

    default:
      printf("Invalid choice.\n");
      break;
    }
  }

  return 0;
}
