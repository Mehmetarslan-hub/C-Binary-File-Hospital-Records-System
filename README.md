# C Binary File Hospital Records System

A high-performance C database application using binary file I/O (`fread`, `fwrite`, `fseek`) with direct record offsets, input bounds checking, and custom validation.

## Key Features
- **Direct Offset File Access**: Leverages $O(1)$ random position reading and updating via binary files (`.dat`).
- **Memory Safety & Initialization**: Zero-initialized structures (`{0}`) preventing stack garbage data leakage.
- **Strict Input Validation**: Validates ID bounds ($1 - 1000$) to prevent sparse file expansion, handles `fgets` buffer overflow, and validates HH:MM time syntax.

## How to Run

1. Compile the program:
   ```bash
   gcc main.c -o hospital_system