#include "vault/vault_interaction.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void vault_interaction_loop(Vault *vault) {
  char command[10];
  char service[MAX_SERVICE_NAME_LENGTH];
  char username[MAX_USERNAME_LENGTH];
  char password[MAX_PASSWORD_LENGTH];

  while (1) {
    printf("\n=== Vault Commands ===\n");
    printf("1. Add password\n");
    printf("2. Get password\n");
    printf("3. List entries\n");
    printf("4. Delete entry\n");
    printf("5. Quit\n");
    printf("======================\n");
    printf("Enter choice: ");

    if (!fgets(command, sizeof(command), stdin)) {
      break;
    }
    command[strcspn(command, "\n")] = '\0';

    switch (command[0]) {
    case '1':
      printf("Service: ");
      fgets(service, sizeof(service), stdin);
      service[strcspn(service, "\n")] = '\0';

      printf("Username: ");
      fgets(username, sizeof(username), stdin);
      username[strcspn(username, "\n")] = '\0';

      printf("Password: ");
      fgets(password, sizeof(password), stdin);
      password[strcspn(password, "\n")] = '\0';

      add_password_entry(vault, service, username, password);
      break;

    case '2':
      printf("Service: ");
      fgets(service, sizeof(service), stdin);
      service[strcspn(service, "\n")] = '\0';

      char *retrieved = get_password_entry(vault, service);
      if (retrieved) {
        printf("Password: %s\n", retrieved);
        free(retrieved);
      }
      break;

    case '3':
      list_password_entries(vault);
      break;

    case '4':
      printf("Service to delete: ");
      fgets(service, sizeof(service), stdin);
      service[strcspn(service, "\n")] = '\0';

      delete_password_entry(vault, service);
      break;

    case '5':
      printf("Exiting vault.\n");
      return;

    default:
      printf("Invalid choice.\n");
      break;
    }
  }
}
