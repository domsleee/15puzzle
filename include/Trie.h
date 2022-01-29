#include <vector>
#include <unordered_set>
#include <string>
#include <memory>

struct TrieNode {
    std::shared_ptr<TrieNode> children[4];
    bool isWord;

    TrieNode() {
        isWord = false;
        for (auto i = 0; i < 4; ++i) children[i] = nullptr;
    }
};

struct Trie {
    Trie();
    std::shared_ptr<TrieNode> rootNode;
    std::vector<std::shared_ptr<TrieNode>> allNodes;

    void insertIntoTrie(std::string);
    void insertReverseIntoTrie(std::string);

    std::shared_ptr<TrieNode> findNode(const std::string&) const;
    bool hasAnySuffix(const std::string&) const;

private:
    std::shared_ptr<TrieNode> getNewNode();
};

Trie buildTrie(std::unordered_set<std::string> strings); 
int countWordsInTrie(const Trie& trie);
int countWordsInTrieNode(const TrieNode& node);
