# Redis CLI

A simple command-line client for testing the Redis server.

## Building

### Windows (MinGW)
```bash
mingw32-make
```

### Linux/macOS
```bash
make
```

## Running

Connect to default server (localhost:6379):
```bash
./redis-cli
```

Connect to custom host/port:
```bash
./redis-cli -h 192.168.1.100 -p 6380
```

## Available Commands

### Server Commands
| Command | Description |
|---------|-------------|
| `PING` | Test server connection |
| `ECHO <msg>` | Echo a message back |
| `SET <key> <value>` | Set a key-value pair |
| `GET <key>` | Get value by key |
| `DEL <key>` | Delete a key |
| `EXISTS <key>` | Check if key exists |
| `KEYS <pattern>` | List keys matching pattern |
| `FLUSHALL` | Clear all data |

### Built-in Commands
| Command | Description |
|---------|-------------|
| `HELP` | Show available commands |
| `CLEAR` | Clear the screen |
| `QUIT` / `EXIT` | Exit the CLI |

## Protocol

The client uses RESP (Redis Serialization Protocol) format:
- Arrays start with `*<count>`
- Bulk strings start with `$<length>`

Example: `PING` becomes `*1\r\n$4\r\nPING\r\n`

## Make Targets

| Target | Description |
|--------|-------------|
| `make` | Build the CLI |
| `make clean` | Remove build artifacts |
| `make rebuild` | Clean and rebuild |
| `make run` | Build and run |
