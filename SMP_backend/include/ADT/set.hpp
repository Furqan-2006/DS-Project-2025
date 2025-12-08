#pragma once

#include "hash_map.hpp"

template <typename Key>
class Set
{
private:
    HashMap<Key, bool> map;

public:
    Set() = default;

    bool insert(const Key &key)
    {
        if (map.contains(key))
        {
            return false;
        }
        map.insert(key, true);
        return true;
    }

    bool erase(const Key &key)
    {
        return map.remove(key);
    }

    bool contains(const Key &key) const
    {
        return map.contains(key);
    }

    size_t size() const
    {
        return map.size();
    }

    void clear()
    {
        map.clear();
    }

    auto begin() { return map.begin(); }
    auto end() { return map.end(); }

    auto begin() const { return map.begin(); }
    auto end() const { return map.end(); }
};