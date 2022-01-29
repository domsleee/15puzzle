#include <iostream>
#include <unordered_set>
#include <vector>
#include <string>
#include <fstream>

int main() {
    std::ifstream fin("../../databases/fsm-80mb-MAX");
    std::vector<std::string> words;
    std::string s;
    while (fin >> s) words.push_back(s);
    for (auto i = 0; i < words.size(); ++i) {
        for (auto j = 0; j < words.size(); ++j) {
            if (i == j) continue;
            auto w1 = words[i];
            auto w2 = words[j];
            if (w1.find(w2) != std::string::npos) {
                std::cout << w1 << " has substring " << w2 << '\n';
            }
        }
    }

    exit(0);

}