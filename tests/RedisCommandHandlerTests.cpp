#include <iostream>
#include <string>
#include <vector>

std::vector<std::string> parseResp(const std::string input);

namespace {

int failures = 0;

void expectTokens(
    const std::vector<std::string>& actual,
    const std::vector<std::string>& expected,
    const std::string& testName
){
    if(actual == expected) return;

    std::cerr << "FAIL: " << testName << "\n";
    std::cerr << "  expected:";
    for(const auto& token : expected) std::cerr << " [" << token << "]";
    std::cerr << "\n  actual:  ";
    for(const auto& token : actual) std::cerr << " [" << token << "]";
    std::cerr << "\n";
    failures++;
}

}

int main(){
    expectTokens(parseResp("*1\r\n$4\r\nPING\r\n"), {"PING"}, "parses single RESP bulk string");
    expectTokens(parseResp("*2\r\n$4\r\nECHO\r\n$5\r\nhello\r\n"), {"ECHO", "hello"}, "parses multiple RESP bulk strings");
    expectTokens(parseResp("*3\r\n$3\r\nSET\r\n$3\r\nkey\r\n$5\r\nvalue\r\n"), {"SET", "key", "value"}, "parses three-element command");
    expectTokens(parseResp("PING hello"), {"PING", "hello"}, "falls back to whitespace parsing for non-RESP input");
    expectTokens(parseResp("*x\r\n$4\r\nPING\r\n"), {}, "returns empty tokens for invalid array length");
    expectTokens(parseResp("*1\r\n$4\r\nPIN\r\n"), {}, "returns empty tokens for truncated bulk string");

    if(failures == 0){
        std::cout << "All RedisCommandHandler tests passed\n";
    }

    return failures == 0 ? 0 : 1;
}
