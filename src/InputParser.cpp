#include "../include/InputParser.h"
#include "../include/Util.h"

#include <string>
#include <unordered_map>
#include <vector>

std::unordered_map<std::string, std::vector<std::string>> InputParser::tokens{};

void InputParser::parse(int argc, const char** argv) {
    if (argc > 1 && argv[1][0] == '-') {
        std::string option;
        for (int i = 1; i < argc; i++) {
            if (argv[i][0] == '-') {  // new option
                option = argv[i];
                tokens[option] = std::vector<std::string>();
            } else {  // option argument
                tokens[option].push_back(std::string(argv[i]));
            }
        }
    }
}

bool InputParser::showHelp() {
    return optionExists("-h") || optionExists("--help");
}

bool InputParser::databaseExists() {
    return optionExists("-d") || optionExists("--database");
}

bool InputParser::boardExists() {
    return optionExists("-b") || optionExists("--board");
}

bool InputParser::fsmDepthExists() {
    return optionExists("-f") || optionExists("--fsmDepth");
}

bool InputParser::fsmFileExists() {
    return optionExists("--fsmFile");
}

bool InputParser::showInteractive() {
    return optionExists("-i") || optionExists("--interactive");
}

bool InputParser::runParallel() {
    return optionExists("-p") || optionExists("--parallel");
}

std::string InputParser::getDatabase() {
    auto args = getMultipleArgs({"-d", "--database"});
    if (args.empty()) {
        return "";
    }
    return args[0];
}

std::string InputParser::getBoard() {
    auto args = getMultipleArgs({"-b", "--board"});
    if (args.empty()) {
        return "";
    }
    return args[0];
}

int InputParser::getFSMDepth() {
    auto args = getMultipleArgs({"-f", "--fsmDepth"});
    return std::stoi(args[0]);
}

long long InputParser::getFSMMemLimit() {
    auto args = getMultipleArgs({"--fsmMemLimit"});
    if (args.empty()) {
        return MAX_LL;
    }
    return std::stoll(args[0]) * (long long)1000000;
}

long long InputParser::getFSMItLimit() {
    auto args = getMultipleArgs({"--fsmItLimit"});
    if (args.empty()) {
        return MAX_LL;
    }
    return std::stoll(args[0]);
}

std::string InputParser::getFSMFile() {
    auto args = getMultipleArgs({"--fsmFile"});
    return args[0];
}

bool InputParser::optionExists(const std::string& option) {
    return tokens.find(option) != tokens.end();
}

std::vector<std::string> InputParser::getArgs(const std::string& option) {
    return tokens[option];
}

std::vector<std::string> InputParser::getMultipleArgs(
    const std::vector<std::string>& options) {
    std::vector<std::string> args;
    for (auto& option : options) {
        auto nextArgs = getArgs(option);
        args.insert(args.end(), nextArgs.begin(), nextArgs.end());
    }
    return args;
}
