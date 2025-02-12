#include "vault.h"
#include <sodium.h>
#include <stdio.h>

int main() {
  if (sodium_init() < 0) {
    perror("Error initializing sodium");
    exit(1);
  }

  open_vault();
  return 0;
}
