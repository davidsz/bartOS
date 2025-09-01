#ifndef LIB_LIST_H
#define LIB_LIST_H

#include <stddef.h>

template <typename T>
class List {
private:
    struct Node {
        T data;
        Node *next;
        Node(const T &value, Node *n = nullptr) : data(value), next(n) {}
    };
    Node *m_head;

public:
    explicit List();
    ~List();

    T &front() const;
    T &back() const;
    void push_front(const T &value);
    void push_back(const T &value);
    size_t length() const;
    void clear();

    class Iterator {
        Node *m_current;
    public:
        explicit Iterator(Node *ptr): m_current(ptr) {}

        T &operator*() const { return m_current->data; }
        T *operator->() const { return &(m_current->data); }

        // Prefix increment
        Iterator &operator++() {
            if (m_current)
                m_current = m_current->next;
            return *this;
        }

        // Postfix increment
        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const Iterator &other) const {
            return m_current == other.m_current;
        }
        bool operator!=(const Iterator &other) const {
            return m_current != other.m_current;
        }
    };

    Iterator begin() { return Iterator(m_head); }
    Iterator end() { return Iterator(nullptr); }
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
    if (!m_head)
        return T();
    return m_head->data;
}

template <typename T>
T &List<T>::back() const
{
    if (!m_head)
        return T();
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
    if (!m_head) {
        m_head = new Node(value);
        return;
    }
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
