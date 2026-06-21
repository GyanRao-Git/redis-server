## Key Insight
The code builds a TCP server by combining class state, constructor initialization, socket creation, and socket options.

## Mental Model
The class stores server state, the constructor prepares it, `socket()` opens the network endpoint, and options like `SO_REUSEADDR` tell the OS how to treat that endpoint.

## Example
```cpp
RedisServer::RedisServer(int port) : port(port), server_socket(-1), running(true) {}
```

## Common Mistakes
- Using `RedisServer::` as a namespace instead of class scope.
- Leaving out the return type on an out-of-class member function.
- Mixing Linux socket headers with Windows builds.

## Revision
- `this` points to the current object.
- `std::atomic` is for shared values across threads.
- `AF_INET + SOCK_STREAM + 0` means IPv4 TCP with the default protocol.
- `SO_REUSEADDR` helps reuse the port sooner after restart.
- `htons(port)` converts the port to network byte order before storing it in `sin_port`.
- `INADDR_ANY` binds the server to all local network interfaces.
