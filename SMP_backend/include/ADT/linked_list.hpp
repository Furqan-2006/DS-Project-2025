#pragma once

template <typename T>
struct Node
{
    T data;
    Node *next;

    Node(T val) : data(val), next(nullptr) {}
};

template <typename T>
class LinkedList
{
private:
    Node<T> *headNode;
    Node<T> *tailNode;
    size_t count;

public:
    LinkedList() : headNode(nullptr), tailNode(nullptr), count(0) {}

    ~LinkedList()
    {
        clear();
    }

    void append(T value)
    {
        Node<T> *newNode = new Node<T>(value);

        if (!headNode)
        {
            headNode = tailNode = newNode;
        }
        else
        {
            tailNode->next = newNode;
            tailNode = newNode;
        }
        count++;
    }

    void prepend(T value)
    {
        Node<T> *newNode = new Node<T>(value);

        if (!headNode)
        {
            headNode = tailNode = newNode;
        }
        else
        {
            newNode->next = headNode;
            headNode = newNode;
        }
        count++;
    }

    void insertAt(size_t index, T value)
    {
        if (index > count)
            return;

        if (index == 0)
        {
            prepend(value);
            return;
        }

        if (index == count)
        {
            append(value);
            return;
        }

        Node<T> *newNode = new Node<T>(value);
        Node<T> *current = headNode;

        for (size_t i = 0; i < index - 1; i++)
        {
            current = current->next;
        }

        newNode->next = current->next;
        current->next = newNode;
        count++;
    }

    bool removeAt(size_t index)
    {
        if (index >= count || !headNode)
            return false;

        Node<T> *toDelete;

        if (index == 0)
        {
            toDelete = headNode;
            headNode = headNode->next;
            if (!headNode)
                tailNode = nullptr;
            delete toDelete;
            count--;
            return true;
        }

        Node<T> *current = headNode;
        for (size_t i = 0; i < index - 1; i++)
        {
            current = current->next;
        }

        toDelete = current->next;
        current->next = toDelete->next;

        if (!current->next)
            tailNode = current;

        delete toDelete;
        count--;
        return true;
    }

    bool remove(T value)
    {
        if (!headNode)
            return false;

        if (headNode->data == value)
        {
            Node<T> *toDelete = headNode;
            headNode = headNode->next;
            if (!headNode)
                tailNode = nullptr;
            delete toDelete;
            count--;
            return true;
        }

        Node<T> *current = headNode;
        while (current->next)
        {
            if (current->next->data == value)
            {
                Node<T> *toDelete = current->next;
                current->next = toDelete->next;
                if (!current->next)
                    tailNode = current;
                delete toDelete;
                count--;
                return true;
            }
            current = current->next;
        }

        return false;
    }

    T *get(size_t index) const
    {
        if (index >= count)
            return nullptr;

        Node<T> *current = headNode;
        for (size_t i = 0; i < index; i++)
        {
            current = current->next;
        }

        return &(current->data);
    }

    bool contains(T value) const
    {
        Node<T> *current = headNode;
        while (current)
        {
            if (current->data == value)
                return true;
            current = current->next;
        }
        return false;
    }

    int indexOf(T value) const
    {
        Node<T> *current = headNode;
        int index = 0;

        while (current)
        {
            if (current->data == value)
                return index;
            current = current->next;
            index++;
        }

        return -1;
    }

    void clear()
    {
        while (headNode)
        {
            Node<T> *temp = headNode;
            headNode = headNode->next;
            delete temp;
        }
        tailNode = nullptr;
        count = 0;
    }

    size_t size() const
    {
        return count;
    }

    bool isEmpty() const
    {
        return count == 0;
    }

    Node<T> *head() const
    {
        return headNode;
    }

    Node<T> *tail() const
    {
        return tailNode;
    }
};

// Key-Value pair LinkedList for HashMap
template <typename K, typename V>
struct LLNode
{
    K key;
    V value;
    LLNode *next;

    LLNode(K ke, V val) : key(ke), value(val), next(nullptr) {}
};

template <typename K, typename V>
class LinkedList
{
private:
    LLNode<K, V> *headNode;
    size_t count;

public:
    LinkedList() : headNode(nullptr), count(0) {}

    ~LinkedList()
    {
        clear();
    }

    void insert(K key, V value)
    {
        LLNode<K, V> *current = headNode;
        while (current)
        {
            if (current->key == key)
            {
                current->value = value;
                return;
            }
            current = current->next;
        }

        LLNode<K, V> *newNode = new LLNode<K, V>(key, value);
        newNode->next = headNode;
        headNode = newNode;
        count++;
    }

    V *find(K key) const
    {
        LLNode<K, V> *current = headNode;
        while (current)
        {
            if (current->key == key)
            {
                return &(current->value);
            }
            current = current->next;
        }
        return nullptr;
    }

    bool remove(K key)
    {
        if (!headNode)
            return false;

        if (headNode->key == key)
        {
            LLNode<K, V> *temp = headNode;
            headNode = headNode->next;
            delete temp;
            count--;
            return true;
        }

        LLNode<K, V> *current = headNode;
        while (current->next)
        {
            if (current->next->key == key)
            {
                LLNode<K, V> *temp = current->next;
                current->next = temp->next;
                delete temp;
                count--;
                return true;
            }
            current = current->next;
        }

        return false;
    }

    void clear()
    {
        while (headNode)
        {
            LLNode<K, V> *temp = headNode;
            headNode = headNode->next;
            delete temp;
        }
        count = 0;
    }

    size_t size() const
    {
        return count;
    }

    bool isEmpty() const
    {
        return count == 0;
    }

    LLNode<K, V> *head() const
    {
        return headNode;
    }
};