#pragma once
#include <vector>
#include <string>

template <typename T>
class TrieNode
{
public:
    TrieNode<T> *children[26];
    std::vector<T *> values;
    bool isEnd;

    TrieNode() : isEnd(false)
    {
        for (int i = 0; i < 26; i++)
            children[i] = nullptr;
    }

    ~TrieNode()
    {
        for (int i = 0; i < 26; i++)
            delete children[i];
    }
};

template <typename T>
class ITrie
{
public:
    virtual ~ITrie() = default;

    virtual void insert(const std::string &key, T *value) = 0;
    virtual std::vector<T *> searchPrefix(const std::string &prefix) const = 0;
};

template <typename T>
class Trie : public ITrie<T>
{
private:
    TrieNode<T> *root;

    void collect(
        TrieNode<T> *node,
        std::vector<T *> &result) const
    {
        if (!node)
            return;

        if (node->isEnd)
            for (T *v : node->values)
                result.push_back(v);

        for (int i = 0; i < 26; i++)
            collect(node->children[i], result);
    }

public:
    Trie() : root(new TrieNode<T>()) {}

    ~Trie()
    {
        delete root;
    }

    void insert(const std::string &key, T *value) override
    {
        TrieNode<T> *current = root;

        for (char c : key)
        {
            if (c < 'a' || c > 'z')
                continue;

            int idx = c - 'a';

            if (!current->children[idx])
                current->children[idx] = new TrieNode<T>();

            current = current->children[idx];
        }

        current->isEnd = true;
        current->values.push_back(value);
    }

    std::vector<T *> searchPrefix(const std::string &prefix) const override
    {
        TrieNode<T> *current = root;

        for (char c : prefix)
        {
            if (c < 'a' || c > 'z')
                continue;

            int idx = c - 'a';

            if (!current->children[idx])
                return {};

            current = current->children[idx];
        }

        std::vector<T *> result;
        collect(current, result);
        return result;
    }
};
