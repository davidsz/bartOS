#ifndef LIB_LIST_H
#define LIB_LIST_H

#include <stddef.h>

template <typename T>
class List {
public:
    explicit List();
    ~List();

    // TODO: Implement iterators
    T &front() const;
    T &back() const;
    void push_front(const T &value);
    void push_back(const T &value);
    size_t length() const;
    void clear();

private:
    struct Node {
        T data;
        Node *next;
        Node(const T &value, Node *n = nullptr) : data(value), next(n) {}
    };
    Node *m_head;
};

template <typename T>
List<T>::List()
    : m_head(nullptr)
{
}

template <typename T>
List<T>::~List()
{
    clear();
}

template <typename T>
T &List<T>::front() const
{
    return m_head->data;
}

template <typename T>
T &List<T>::back() const
{
    Node *node = m_head;
    while (node->next)
        node = node->next;
    return node->data;
}

template <typename T>
void List<T>::push_front(const T &value)
{
    m_head = new Node(value, m_head);
}

template <typename T>
void List<T>::push_back(const T &value)
{
    Node *node = m_head;
    while (node->next)
        node = node->next;
    node->next = new Node(value);
}

template <typename T>
size_t List<T>::length() const
{
    size_t count = 0;
    Node *node = m_head;
    while (node) {
        count++;
        node = node->next;
    }
    return count;
}

template <typename T>
void List<T>::clear()
{
    Node *node = m_head;
    while (node) {
        Node *next = node->next;
        delete node;
        node = next;
    }
    m_head = nullptr;
}

#endif // LIB_LIST_H
