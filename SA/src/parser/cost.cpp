#include "cost.h"

string exec(const string& cmd, const string& args) {
    array<char, 128> buffer;
    string result;
    string fullCommand = cmd + " " + args; // Combine command and arguments
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(fullCommand.c_str(), "r"), pclose);
    if (!pipe) {
        throw runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

float extractCost(const string& output) {
    istringstream iss(output);
    string token;
    while (iss >> token) {
        if (token == "=") {
            if (iss >> token) {
                return stof(token);
            }
        }
    }
    throw runtime_error("Cost not found in output");
}