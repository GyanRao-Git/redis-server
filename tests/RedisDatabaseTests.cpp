#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>
#include <sstream>
#include "RedisDatabase.h"

// Helper function to read file contents into a vector of strings
std::vector<std::string> readFileLines(const std::string& filename) {
    std::vector<std::string> lines;
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
}

// Helper function to normalize file content for comparison
std::vector<std::string> getNormalizedLines(const std::string& filename) {
    std::vector<std::string> lines = readFileLines(filename);
    for (auto& line : lines) {
        std::istringstream iss(line);
        char type;
        iss >> type;
        if (type == 'H') {
            std::string key;
            iss >> key;
            std::vector<std::string> parts;
            std::string part;
            while (iss >> part) {
                parts.push_back(part);
            }
            std::sort(parts.begin(), parts.end());
            std::ostringstream oss;
            oss << type << " " << key;
            for (const auto& p : parts) {
                oss << " " << p;
            }
            line = oss.str();
        }
    }
    std::sort(lines.begin(), lines.end());
    return lines;
}


// Test function for RedisDatabase::dump and RedisDatabase::load
void testDumpAndLoad() {
    std::cout << "Running test: testDumpAndLoad..." << std::endl;

    // 1. Create a dummy dump file
    const std::string test_dump_filename = "test_dump.rdb";
    std::ofstream outfile(test_dump_filename);
    outfile << "K name Alice" << std::endl;
    outfile << "K city Berlin" << std::endl;
    outfile << "L fruits apple banana orange" << std::endl;
    outfile << "H user:100 name:Bob age:30" << std::endl;
    outfile.close();

    // 2. Load the database from the dummy file
    RedisDatabase& db = RedisDatabase::getInstance();
    bool load_success = db.load(test_dump_filename);
    assert(load_success && "Failed to load test_dump.rdb");
    std::cout << "  - Loaded data from " << test_dump_filename << std::endl;

    // 3. Dump the database to a new file
    const std::string test_dump_output_filename = "test_dump_output.rdb";
    bool dump_success = db.dump(test_dump_output_filename);
    assert(dump_success && "Failed to dump to test_dump_output.rdb");
    std::cout << "  - Dumped data to " << test_dump_output_filename << std::endl;

    // 4. Compare the contents of the two files
    std::vector<std::string> original_lines = getNormalizedLines(test_dump_filename);
    std::vector<std::string> dumped_lines = getNormalizedLines(test_dump_output_filename);

    assert(original_lines == dumped_lines && "Original and dumped files do not match!");

    std::cout << "  - Verification successful: " << test_dump_filename << " and " << test_dump_output_filename << " are equivalent." << std::endl;
    std::cout << "Test testDumpAndLoad PASSED." << std::endl;

    // Clean up created files
    remove(test_dump_filename.c_str());
    remove(test_dump_output_filename.c_str());
}

int main() {
    int failures = 0;
    try {
        testDumpAndLoad();
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        failures++;
    }

    if (failures == 0) {
        std::cout << "All RedisDatabase tests passed!" << std::endl;
    } else {
        std::cerr << failures << " test(s) failed." << std::endl;
    }

    return failures;
}
