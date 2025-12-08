#pragma once

#include <stdexcept>
#include <vector>

template <typename T>
struct QueueNode
{
    T data;
    QueueNode<T> *next;
    QueueNode<T> *prev;

    QueueNode(const T &val) : data(val), next(nullptr), prev(nullptr) {}
};

template <typename T>
class Queue
{
private:
    QueueNode<T> *m_front;
    QueueNode<T> *m_back;
    size_t m_size;

    void clear()
    {
        QueueNode<T> *curr = m_front;
        while (curr != nullptr)
        {
            QueueNode<T> *temp = curr;
            curr = curr->next;
            delete temp;
        }
        m_front = m_back = nullptr;
        m_size = 0;
    }

public:
    Queue() : m_front(nullptr), m_back(nullptr), m_size(0) {}
    ~Queue() { clear(); }

    Queue(const Queue<T> &other) : m_front(nullptr), m_back(nullptr), m_size(0)
    {
        QueueNode<T> *curr = other.m_front;
        while (curr)
        {
            enqueue(curr->data);
            curr = curr->next;
        }
    }

    Queue<T> operator=(const Queue &other)
    {
        if (this != &other)
        {
            clear();
            QueueNode<T> *curr = other.m_front;
            while (curr)
            {
                enqueue(curr->data);
                curr = curr->next;
            }
        }
        return *this;
    }

    bool isEmpty() const { return m_size == 0; }
    size_t size() const { return m_size; }

    void enqueue(const T &item)
    {
        QueueNode<T> *newNode = new QueueNode<T>(item);
        if (isEmpty())
        {
            m_front = m_back = newNode;
        }
        else
        {
            m_back->next = newNode;
            newNode->prev = m_back;
            m_back = newNode;
        }
        m_size++;
    }

    void dequeue()
    {
        if (isEmpty())
        {
            throw std::out_of_range("Dequeue called on empty queue.");
        }
        QueueNode<T> *temp = m_front;
        m_front = m_front->next;
        if (m_front)
        {
            m_front->prev = nullptr;
        }
        else
        {
            m_back = nullptr;
        }
        delete temp;
        m_size--;
    }

    T &front()
    {
        if (isEmpty())
        {
            throw std::out_of_range("Front called on an empty queue.");
        }
        return m_front->data;
    }

    const T &front() const
    {
        if (isEmpty())
        {
            throw std::out_of_range("Front called on an empty queue.");
        }
        return m_front->data;
    }

    T &back()
    {
        if (isEmpty())
        {
            throw std::out_of_range("back called on an empty queue.");
        }
        return m_back->data;
    }

    const T &back() const
    {
        if (isEmpty())
        {
            throw std::out_of_range("back called on an empty queue.");
        }
        return m_back->data;
    }

    std::vector<T> toVector() const
    {
        std::vector<T> result;
        QueueNode<T> *curr = m_front;
        while (curr != nullptr)
        {
            result.push_back(curr->data);
            curr = curr->next;
        }
        return result;
    }

    T *find(unsigned long long ID)
    {
        QueueNode<T> *curr = m_front;
        while (curr)
        {
            if (curr->data.getID() == ID)
            {
                return &(curr->data);
            }
            curr = curr->next;
        }
        return nullptr;
    }
};
