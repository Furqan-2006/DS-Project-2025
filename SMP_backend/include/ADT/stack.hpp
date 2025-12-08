#pragma once
#include <vector>

template <typename T>
class Stack
{
private:
    std::vector<T> data;

public:
    bool isEmpty() const { return data.empty(); }
    size_t size() const { return data.size(); }

    void push(const T &item) { data.push_back(item); }
    void pop()
    {
        if (!data.empty())
        {
            data.pop_back();
        }
    }

    T &top() { return data.back(); }
    const T &top() const { return data.back(); }

    std::vector<T> reverse() const
    {
        std::vector<T> reversed(data.rbegin(), data.rend());
        return reversed;
    }
};