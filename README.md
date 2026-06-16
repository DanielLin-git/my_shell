## Directory Structure

```text
.
├── build/                  # Build artifacts and binaries
├── include/                # Header files (.h)
├── src/                    # Source files (.c)
│   ├── my_shell_v1.c       # Version 1 implementation
│   └── my_shell_v2.c       # Version 2 implementation
├── .gitignore
├── CMakeLists.txt          # CMake configuration file
└── README.md               # Project documentation
```

Prerequisites
Before building the project, ensure you have the following installed on your system:

- GCC (or any C compiler)
- CMake (version 3.10 or higher)
- Make

On Ubuntu/Debian, you can install them using:
```
sudo apt update
sudo apt install build-essential cmake
```

## How to Build and Run
1. Create a Build Directory
```
mkdir -p build && cd build
```
2. Generate Build Files
```
cmake ..
```
3. Compile the Source Code
```
make
```
4. Run the Executable
```
./my_shell_v1
```

(Note: If you want to run my_shell_v2, make sure it is added to your CMakeLists.txt via add_executable(my_shell_v2 src/my_shell_v2.c))

- my_shell_v1.c control structer
```
   [ Start ]
       │
       ▼
┌──────────────┐
│  Read Line   │  <─── (Gets raw input via getchar)
└──────┬───────┘
       │
       ▼
┌──────────────┐
│  Split Line  │  <─── (Tokenizes string into arguments)
└──────┬───────┘
       │
       ▼
┌──────────────┐
│   Execute    │  <─── (Runs builtin command or forks subpros)
└──────┬───────┘
       │
       ▼
┌──────────────┐
│   Clean Up   │  <─── (Frees memory allocated for this loop)
└──────┬───────┘
       │
       └─ (Loop continues until 'exit' command returns 0) ──> [ End ]
```
- my_shell_v2.c control structer
```
         ┌─────────────────── Empty Input / Error ──────────────────┐
         ▼                                                          │
┌─────────────────┐          Success         ┌────────────────┐     │
│  WAITING_INPUT  │─────────────────────────>│   SPLIT_LINE   │     │
└────────┬────────┘                          └───────┬────────┘     │
         ▲                                           │              │
         │                                   Success │              │
         │                                           ▼              │
         │                                   ┌────────────────┐     │
         ├─────────── Execution Done ────────│    EXECUTE     │     │
         │                                   └───────┬────────┘     │
         │                                           │              │
         │                                      If 'exit'           │
         │                                           ▼              │
         │                                   ┌────────────────┐     │
         └─────────── Reset & Clean Up ──────│      EXIT      │<────┘
                                             └───────┬────────┘
                                                     │
                                                     ▼
                                                  [ End ]
```