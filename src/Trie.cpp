#include "../include/Trie.h"
#include "../include/Util.h"

Trie buildTrie(std::unordered_set<std::string> strings) {
    Trie trie;
    for (auto s: strings) {
        trie.insertIntoTrie(s);
    }
    return trie;
}

int countWordsInTrie(const Trie& trie) {
    return countWordsInTrieNode(*trie.rootNode);
}

int countWordsInTrieNode(const TrieNode& node) {
    auto res = 0;
    if (node.isWord) res++;
    for (auto i = 0; i < 4; ++i) {
        if (node.children[i] != nullptr) {
            res += countWordsInTrieNode(*(node.children[i]));
        }
    }
    return res;
}

Trie::Trie() {
    rootNode = getNewNode();
}

std::shared_ptr<TrieNode> Trie::getNewNode() {
    auto newNode = std::make_shared<TrieNode>();
    allNodes.push_back(std::move(newNode));
    return allNodes[allNodes.size()-1];
}

std::shared_ptr<TrieNode> Trie::findNode(const std::string &str) const {
    auto node = rootNode;
    for (auto c: str) {
        auto i = static_cast<int>(charToDirection(c));
        node = node->children[i];
        if (node == nullptr) return node;
    }
    return node;
}

void Trie::insertIntoTrie(std::string s) {
    auto node = rootNode;
    for (auto c: s) {
        auto direction = static_cast<int>(charToDirection(c));
        if (node->children[direction] == nullptr) node->children[direction] = getNewNode();
        node = node->children[direction];
    }
    node->isWord = true;
}

