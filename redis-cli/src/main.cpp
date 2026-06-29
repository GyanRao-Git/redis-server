#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <sstream>
#include <algorithm>

// For Windows sockets
#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
typedef int socklen_t;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket close
#endif

class RedisCli
{
private:
    std::string host;
    int port;
    SOCKET sockfd;
    bool connected;

public:
    RedisCli(const std::string &h, int p) : host(h), port(p), sockfd(INVALID_SOCKET), connected(false)
    {
#ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            std::cerr << "WSAStartup failed" << std::endl;
        }
#endif
    }

    ~RedisCli()
    {
        disconnect();
#ifdef _WIN32
        WSACleanup();
#endif
    }

    bool connectToServer()
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == INVALID_SOCKET)
        {
            std::cerr << "Error creating socket" << std::endl;
            return false;
        }

        struct sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        serverAddr.sin_addr.s_addr = inet_addr(host.c_str());

        if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        {
            std::cerr << "Error connecting to server" << std::endl;
            closesocket(sockfd);
            sockfd = INVALID_SOCKET;
            return false;
        }

        connected = true;
        std::cout << "Connected to " << host << ":" << port << std::endl;
        return true;
    }

    void disconnect()
    {
        if (sockfd != INVALID_SOCKET)
        {
            closesocket(sockfd);
            sockfd = INVALID_SOCKET;
            connected = false;
        }
    }

    // Convert command to RESP format
    std::string toRESP(const std::vector<std::string> &tokens)
    {
        if (tokens.empty())
            return "";

        std::ostringstream oss;
        oss << "*" << tokens.size() << "\r\n";

        for (const auto &token : tokens)
        {
            oss << "$" << token.length() << "\r\n"
                << token << "\r\n";
        }

        return oss.str();
    }

    // Parse user input into tokens
    std::vector<std::string> parseInput(const std::string &input)
    {
        std::vector<std::string> tokens;
        std::istringstream iss(input);
        std::string token;
        while (iss >> token)
        {
            tokens.push_back(token);
        }
        return tokens;
    }

    bool sendCommand(const std::string &command)
    {
        if (!connected)
        {
            std::cerr << "Not connected to server" << std::endl;
            return false;
        }

        int sent = send(sockfd, command.c_str(), command.length(), 0);
        if (sent == SOCKET_ERROR)
        {
            std::cerr << "Error sending command" << std::endl;
            return false;
        }
        return true;
    }

    std::string receiveResponse()
    {
        if (!connected)
        {
            return "-ERR Not connected\r\n";
        }

        char buffer[4096] = {0};
        int received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);

        if (received == SOCKET_ERROR || received == 0)
        {
            return "-ERR Connection lost\r\n";
        }

        buffer[received] = '\0';
        return std::string(buffer);
    }

    // Parse RESP response for display
    std::string formatResponse(const std::string &response)
    {
        if (response.empty())
            return "(empty)";

        // Simple RESP type indicator
        char type = response[0];
        std::string content = response.substr(1);

        // Remove trailing \r\n for display
        if (content.length() >= 2 && content.substr(content.length() - 2) == "\r\n")
        {
            content = content.substr(0, content.length() - 2);
        }

        switch (type)
        {
        case '+':
            return "(simple string) " + content;
        case '-':
            return "(error) " + content;
        case ':':
            return "(integer) " + content;
        case '$':
            return "(bulk string) " + content;
        case '*':
            return "(array) " + content;
        default:
            return response;
        }
    }

    void run()
    {
        if (!connectToServer())
        {
            return;
        }

        std::string line;
        std::cout << "redis-cli> ";

        while (std::getline(std::cin, line))
        {
            // Trim whitespace
            line.erase(0, line.find_first_not_of(" \t\n\r"));
            line.erase(line.find_last_not_of(" \t\n\r") + 1);

            if (line.empty())
            {
                std::cout << "redis-cli> ";
                continue;
            }

            // Handle built-in commands
            std::vector<std::string> tokens = parseInput(line);
            if (tokens.empty())
            {
                std::cout << "redis-cli> ";
                continue;
            }

            std::string cmd = tokens[0];
            std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

            // Built-in commands (not sent to server)
            if (cmd == "QUIT" || cmd == "EXIT")
            {
                std::cout << "Goodbye!" << std::endl;
                break;
            }

            if (cmd == "HELP")
            {
                printHelp();
                std::cout << "redis-cli> ";
                continue;
            }

            if (cmd == "CLEAR")
            {
#ifdef _WIN32
                system("cls");
#else
                system("clear");
#endif
                std::cout << "redis-cli> ";
                continue;
            }

            // Send to server
            std::string respCommand = toRESP(tokens);
            if (sendCommand(respCommand))
            {
                std::string response = receiveResponse();
                std::cout << formatResponse(response) << std::endl;
            }

            std::cout << "redis-cli> ";
        }

        disconnect();
    }

    void printHelp()
    {
        std::cout << "\n=== Redis CLI Help ===\n";
        std::cout << "Commands:\n";
        std::cout << "  PING          - Test server connection\n";
        std::cout << "  ECHO <msg>    - Echo a message\n";
        std::cout << "  SET <k> <v>   - Set a key-value pair\n";
        std::cout << "  GET <key>     - Get value by key\n";
        std::cout << "  DEL <key>     - Delete a key\n";
        std::cout << "  EXISTS <key>  - Check if key exists\n";
        std::cout << "  KEYS <pat>    - List keys matching pattern\n";
        std::cout << "  FLUSHALL      - Clear all data\n";
        std::cout << "\nBuilt-in:\n";
        std::cout << "  HELP          - Show this help\n";
        std::cout << "  CLEAR         - Clear screen\n";
        std::cout << "  QUIT/EXIT     - Exit CLI\n";
        std::cout << "======================\n\n";
    }
};

void printBanner()
{
    std::cout << "\n";
    std::cout << "+======================================+\n";
    std::cout << "|        Redis CLI v1.0                |\n";
    std::cout << "|  Type HELP for available commands    |\n";
    std::cout << "+======================================+\n\n";
}

int main(int argc, char *argv[])
{
    std::string host = "127.0.0.1";
    int port = 6973;

    // Parse command line arguments
    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];
        if ((arg == "-h" || arg == "--host") && i + 1 < argc)
        {
            host = argv[++i];
        }
        else if ((arg == "-p" || arg == "--port") && i + 1 < argc)
        {
            port = std::stoi(argv[++i]);
        }
    }

    printBanner();
    std::cout << "Connecting to " << host << ":" << port << "...\n";

    RedisCli cli(host, port);
    cli.run();

    return 0;
}