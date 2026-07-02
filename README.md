# Redis Server Clone

This is a learning project focused on building a Redis server clone in C++.

The goal is to understand the pieces behind a simple networked key-value server, including sockets, server setup, and command handling, while gradually growing the implementation.

This project is for learning and experimentation, so the code may evolve as new concepts are explored.



nodemon --watch src --ext cpp,h --exec "mingw32-make && .\my_redis_server.exe"


nodemon --watch src --watch redis-cli/src --ext cpp,h --exec "cd redis-cli && mingw32-make && .\redis-cli.exe"