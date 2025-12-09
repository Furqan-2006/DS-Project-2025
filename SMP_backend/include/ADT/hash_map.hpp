#pragma once

#include <vector>
#include <string>

template <typename T>
class LL
{
private:
    struct Node
    {
        T value;
        Node *next;
        Node(const T &val) : value(val), next(nullptr) {}
    };

    Node *head;

public:
    LL() : head(nullptr) {}
    ~LL() { clear(); }

    void insert(const T &val)
    {
        Node *n = new Node(val);
        n->next = head;
        head = n;
    }

    // remove by key only
    bool remove(const typename T::KeyType &key)
    {
        Node *prev = nullptr;
        Node *curr = head;

        while (curr)
        {
            if (curr->value.key == key)
            {
                if (prev)
                    prev->next = curr->next;
                else
                    head = curr->next;

                delete curr;
                return true;
            }
            prev = curr;
            curr = curr->next;
        }
        return false;
    }

    T *find(const typename T::KeyType &key)
    {
        Node *curr = head;
        while (curr)
        {
            if (curr->value.key == key)
                return &curr->value;
            curr = curr->next;
        }
        return nullptr;
    }

    const T *find(const typename T::KeyType &key) const
    {
        Node *curr = head;
        while (curr)
        {
            if (curr->value.key == key)
                return &curr->value;
            curr = curr->next;
        }
        return nullptr;
    }

    Node *begin() { return head; }
    const Node *begin() const { return head; }

    void clear()
    {
        Node *curr = head;
        while (curr)
        {
            Node *next = curr->next;
            delete curr;
            curr = next;
        }
        head = nullptr;
    }

    // -------- Iterator ----------
    struct iterator
    {
        Node *ptr;
        iterator() : ptr(nullptr) {}
        iterator(Node *p) : ptr(p) {}

        T &operator*() { return ptr->value; }
        iterator &operator++()
        {
            ptr = ptr->next;
            return *this;
        }
        bool operator!=(const iterator &other) const
        {
            return ptr != other.ptr;
        }
        bool operator==(const iterator &other) const
        {
            return ptr == other.ptr;
        }
    };

    struct const_iterator
    {
        const Node *ptr;
        const_iterator() : ptr(nullptr) {}
        const_iterator(const Node *p) : ptr(p) {}

        const T &operator*() const { return ptr->value; }
        const_iterator &operator++()
        {
            ptr = ptr->next;
            return *this;
        }
        bool operator!=(const const_iterator &other) const
        {
            return ptr != other.ptr;
        }
        bool operator==(const const_iterator &other) const
        {
            return ptr == other.ptr;
        }
    };

    iterator begin_iter() { return iterator(head); }
    iterator end_iter() { return iterator(nullptr); }
    const_iterator begin_iter() const { return const_iterator(head); }
    const_iterator end_iter() const { return const_iterator(nullptr); }
};

template <typename T>
struct Hash
{
    size_t operator()(const T &key)
    {
        return (size_t)key;
    }
};
template <>
struct Hash<const char *>
{
    size_t operator()(const char *str) const
    {
        size_t h = 0;
        while (*str)
        {
            h = h * 53 + (size_t)(*str);
            str++;
        }
        return h;
    }
};
template <>
struct Hash<std::string>
{
    size_t operator()(const std::string &str) const
    {
        size_t h = 0;
        for (char c : str)
        {
            h = h * 53 + c;
        }
        return h;
    }
};

template <typename K, typename V>
class HashMap
{
private:
    struct Node
    {
        using KeyType = K;
        K key;
        V value;
    };

    std::vector<LL<Node>> buckets;
    size_t currSize;
    float loadFactorThreshold;

    size_t hashKey(const K &key, size_t mod) const
    {
        return Hash<K>{}(key) % mod;
    }

    void rehash()
    {
        size_t newSize = buckets.size() * 2;
        std::vector<LL<Node>> newBuckets(newSize);

        for (auto &chain : buckets)
        {
            for (auto it = chain.begin_iter(); it != chain.end_iter(); ++it)
            {
                Node &n = *it;
                size_t index = hashKey(n.key, newSize);
                newBuckets[index].insert(n);
            }
        }

        buckets.swap(newBuckets);
    }

    void maybeRehash()
    {
        if ((float)currSize / (float)buckets.size() > loadFactorThreshold)
            rehash();
    }

public:
    HashMap(size_t cap = 20, float lf = 0.75)
        : currSize(0), loadFactorThreshold(lf)
    {
        buckets.resize(cap);
    }

    bool insert(const K &key, const V &val)
    {
        maybeRehash();

        size_t index = hashKey(key, buckets.size());
        Node *found = buckets[index].find(key);

        if (found)
        {
            found->value = val;
            return false;
        }

        buckets[index].insert(Node{key, val});
        currSize++;
        return true;
    }

    bool remove(const K &key)
    {
        size_t index = hashKey(key, buckets.size());
        bool removed = buckets[index].remove(key);
        if (removed)
            currSize--;
        return removed;
    }

    bool contains(const K &key) const
    {
        size_t index = hashKey(key, buckets.size());
        return buckets[index].find(key) != nullptr;
    }

    V *get(const K &key)
    {
        size_t index = hashKey(key, buckets.size());
        Node *n = buckets[index].find(key);
        return n ? &n->value : nullptr;
    }

    const V *get(const K &key) const
    {
        size_t index = hashKey(key, buckets.size());
        const Node *n = buckets[index].find(key);
        return n ? &n->value : nullptr;
    }

    V &operator[](const K &key)
    {
        maybeRehash();

        size_t index = hashKey(key, buckets.size());
        Node *found = buckets[index].find(key);

        if (found)
        {
            return found->value;
        }

        buckets[index].insert(Node{key, V()});
        currSize++;

        found = buckets[index].find(key);
        return found->value;
    }

    const V &operator[](const K &key) const
    {
        size_t index = hashKey(key, buckets.size());
        const Node *found = buckets[index].find(key);
        return found->value;
    }

    size_t size() const
    {
        return currSize;
    }

    void clear()
    {
        for (auto &chain : buckets)
        {
            chain.clear();
        }
        currSize = 0;
    }

    void reserve(size_t newCap)
    {
        if (newCap <= buckets.size())
            return;

        std::vector<LL<Node>> newBuckets(newCap);

        for (auto &chain : buckets)
        {
            for (auto it = chain.begin_iter(); it != chain.end_iter(); ++it)
            {
                Node &n = *it;
                size_t index = hashKey(n.key, newCap);
                newBuckets[index].insert(n);
            }
        }

        buckets.swap(newBuckets);
    }

public:
    struct iterator
    {
        using LLNode = typename LL<Node>::iterator;

        std::vector<LL<Node>> *buckets;
        size_t bucketIndex;
        LLNode chainIter;
        LLNode chainEnd;

        iterator(std::vector<LL<Node>> *b, size_t idx)
            : buckets(b), bucketIndex(idx)
        {
            // Move to first non-empty bucket
            while (bucketIndex < buckets->size() && (*buckets)[bucketIndex].begin() == nullptr)
            {
                bucketIndex++;
            }

            if (bucketIndex < buckets->size())
            {
                chainIter = (*buckets)[bucketIndex].begin_iter();
                chainEnd = (*buckets)[bucketIndex].end_iter();
            }
        }

        bool operator!=(const iterator &other) const
        {
            return bucketIndex != other.bucketIndex ||
                   chainIter != other.chainIter;
        }

        bool operator==(const iterator &other) const
        {
            return bucketIndex == other.bucketIndex && chainIter == other.chainIter;
        }

        Node &operator*()
        {
            return *chainIter;
        }

        iterator &operator++()
        {
            ++chainIter;

            if (chainIter == chainEnd)
            {
                bucketIndex++;
                while (bucketIndex < buckets->size() &&
                       (*buckets)[bucketIndex].begin() == nullptr)
                {
                    bucketIndex++;
                }

                if (bucketIndex < buckets->size())
                {
                    chainIter = (*buckets)[bucketIndex].begin_iter();
                    chainEnd = (*buckets)[bucketIndex].end_iter();
                }
            }

            return *this;
        }
    };

    struct const_iterator
    {
        using LLNode = typename LL<Node>::const_iterator;

        const std::vector<LL<Node>> *buckets;
        size_t bucketIndex;
        LLNode chainIter;
        LLNode chainEnd;

        const_iterator(const std::vector<LL<Node>> *b, size_t idx)
            : buckets(b), bucketIndex(idx)
        {
            while (bucketIndex < buckets->size() && (*buckets)[bucketIndex].begin() == nullptr)
            {
                bucketIndex++;
            }

            if (bucketIndex < buckets->size())
            {
                chainIter = (*buckets)[bucketIndex].begin_iter();
                chainEnd = (*buckets)[bucketIndex].end_iter();
            }
        }

        bool operator!=(const const_iterator &other) const
        {
            return bucketIndex != other.bucketIndex ||
                   chainIter != other.chainIter;
        }

        bool operator==(const const_iterator &other) const
        {
            return bucketIndex == other.bucketIndex &&
                   chainIter == other.chainIter;
        }

        const Node &operator*() const
        {
            return *chainIter;
        }

        const_iterator &operator++()
        {
            ++chainIter;

            if (chainIter == chainEnd)
            {
                bucketIndex++;
                while (bucketIndex < buckets->size() &&
                       (*buckets)[bucketIndex].begin() == nullptr)
                {
                    bucketIndex++;
                }

                if (bucketIndex < buckets->size())
                {
                    chainIter = (*buckets)[bucketIndex].begin_iter();
                    chainEnd = (*buckets)[bucketIndex].end_iter();
                }
            }

            return *this;
        }
    };

    iterator begin()
    {
        return iterator(&buckets, 0);
    }

    iterator end()
    {
        return iterator(&buckets, buckets.size());
    }

    iterator find(const K &key)
    {
        size_t index = hashKey(key, buckets.size());
        auto &chain = buckets[index];

        for (auto it = chain.begin_iter(); it != chain.end_iter(); ++it)
        {
            if ((*it).key == key)
            {
                iterator mapIt(&buckets, index);
                mapIt.chainIter = it;
                mapIt.chainEnd = chain.end_iter();
                return mapIt;
            }
        }
        return end();
    }

    const_iterator begin() const
    {
        return const_iterator(&buckets, 0);
    }

    const_iterator end() const
    {
        return const_iterator(&buckets, buckets.size());
    }

    const_iterator find(const K &key) const
    {
        size_t index = hashKey(key, buckets.size());
        auto &chain = buckets[index];

        for (auto it = chain.begin_iter(); it != chain.end_iter(); ++it)
        {
            if ((*it).key == key)
            {
                const_iterator mapIt(&buckets, index);
                mapIt.chainIter = it;
                mapIt.chainEnd = chain.end_iter();
                return mapIt;
            }
        }
        return end();
    }
};