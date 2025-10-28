# tempDB Client

A cli client for [tempDB](https://github.com/3l-d1abl0/tempDB). A client for connecting to tempDB servers using TCP sockets, written in modem C++.

### Core Modules

- **`Cli`** - Command line argument parsing and validation
- **`Network`** - Socket connection management with RAII principles
- **`RespProtocol`** - RESP protocol encoding and decoding
- **`Client`** - Main client logic coordinating all components


## Building

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, etc.)
- Make build system
- Linux/Unix environment with socket support

### Build Commands

```bash
# Show all available targets
make help

# Build the project
make

# Install the binary to /usr/local/bin/
make install

# Clean build files
make clean

```

## Usage

### Command Line Options

```
Usage: tempDB-client -h <host-ip> -p <port>
   OR: tempDB-client -host <host-ip> -port <port>

Options:
  -h, -host <host>    Server hostname or IP address
  -p, -port <port>    Server port number (1-65535)

Examples:
  tempDB-client -h localhost -p 6379
  tempDB-client -host 127.0.0.1 -port 9000
```

### Commands

- Type any commnad supported by [tempDB](https://github.com/3l-d1abl0/tempDB) command at the prompt
- Use `exit` or Ctrl+D to quit

## Future Enhancements


- [ ] Unit tests for each module
- [ ] Connection pooling
- [ ] SSL/TLS support
- [ ] Command history and completion
- [ ] Multi-threaded operations
