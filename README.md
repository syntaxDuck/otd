# OTD - Password Vault Manager

A secure password vault manager written in C using libsodium for cryptographic operations.

## Features

### Hash Table
- Dynamic resizing with automatic rehashing
- Custom hash function support (DJB2 or custom)
- Load factor monitoring
- Key existence checking
- Clear table functionality
- Copy/clone support

### Vault
- Create new password vaults with master password
- Open and authenticate existing vaults
- Secure password hashing using `crypto_pwhash`
- HMAC-based key derivation using `crypto_auth_hmacsha256`

## Requirements

- C compiler (clang)
- CMake 3.19+
- libsodium

## Building

```bash
mkdir build && cd build
cmake ..
make
```

## Usage

Run the compiled binary:
```bash
./build/main
```

This will prompt you to either create a new vault or open an existing one.

## Project Structure

```
otd/
├── include/          # Header files
│   ├── hash_table.h
│   ├── vault.h
│   ├── generics.h
│   └── vault/
├── src/              # Source files
│   ├── main.c
│   ├── vault.c
│   ├── hash_table.c
│   ├── generics.c
│   └── vault/
├── build/            # Build output
├── CMakeLists.txt
└── README.md
```

## Security

- Master passwords are hashed using libsodium's `crypto_pwhash` with sensitive resource limits
- Keys are derived using HMAC-SHA256
- All cryptographic operations use libsodium's secure implementations
