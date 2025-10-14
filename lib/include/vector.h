#ifndef LIB_VECTOR_H
#define LIB_VECTOR_H

#include <stddef.h>
#include "heap.h"
#include "utility.h"
#include "log.h"

template <typename T>
class Vector {
public:
    explicit Vector();
    ~Vector();
    // Copy
    Vector(const Vector &other);
    Vector<T> &operator=(Vector<T> &other);
    // Move
    Vector(Vector &&other);
    Vector<T> &operator=(Vector<T> &&other);

    T &front() const;
    T &back() const;
    T &operator[](int index);
    const T &operator[](int index) const;

    void resize(size_t size);
    size_t size() const;
    bool empty() const;
    void reserve(size_t capacity);
    size_t capacity() const;
    void push_back(const T &value);
    void clear();

    class Iterator {
        T *m_current;
    public:
        explicit Iterator(T *p) : m_current(p) {}

        T &operator*() const { return *m_current; }
        T *operator->() const { return m_current; }

        Iterator &operator++() { ++m_current; return *this; }
        Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

        Iterator &operator--() { --m_current; return *this; }
        Iterator operator--(int) { Iterator tmp = *this; --(*this); return tmp; }

        bool operator==(const Iterator &other) const { return m_current == other.m_current; }
        bool operator!=(const Iterator &other) const { return m_current != other.m_current; }

        Iterator operator+(int n) const { return Iterator(m_current + n); }
        Iterator operator-(int n) const { return Iterator(m_current - n); }

        ptrdiff_t operator-(const Iterator &other) const { return m_current - other.m_current; }

        bool operator<(const Iterator &other) const { return m_current < other.m_current; }
        bool operator>(const Iterator &other) const { return m_current > other.m_current; }
        bool operator<=(const Iterator &other) const { return m_current <= other.m_current; }
        bool operator>=(const Iterator &other) const { return m_current >= other.m_current; }

        friend class Vector;
    };

    Iterator begin() { return Iterator(m_data); }
    Iterator end() { return Iterator(m_data + m_size); }

    Iterator insert(Iterator pos, const T &value) {
        size_t index = pos.m_current - m_data;
        if (index > m_size)
            index = m_size;
        if (m_size == m_capacity)
            reserve(m_capacity * 2);
        for (size_t i = m_size; i > index; i--)
            m_data[i] = move(m_data[i - 1]);
        m_data[index] = value;
        m_size++;
        return Iterator(m_data + index);
    }

    Iterator erase(const Iterator &pos) {
        size_t index = pos.m_current - m_data;
        if (index >= m_size)
            return end();
        for (size_t i = index; i + 1 < m_size; i++)
            m_data[i] = move(m_data[i + 1]);
        m_size--;
        return Iterator(m_data + index);
    }

private:
    T *m_data;
    size_t m_size;
    size_t m_capacity;
};

// TODO: check initialization list support
template <typename T>
Vector<T>::Vector()
    : m_data(nullptr)
    , m_size(0)
    , m_capacity(0)
{
    reserve(16);
}

template <typename T>
Vector<T>::Vector(const Vector &other)
    : m_size(other.m_size)
    , m_capacity(other.m_capacity)
{
    m_data = new T[m_capacity];
    for (size_t i = 0; i < m_size; ++i)
        m_data[i] = other.m_data[i];
}

template <typename T>
Vector<T> &Vector<T>::operator=(Vector<T> &other)
{
    if (this != &other) {
        delete[] m_data;
        m_size = other.m_size;
        m_capacity = other.m_capacity;
        m_data = new T[m_capacity];
        for (size_t i = 0; i < m_size; ++i) {
            m_data[i] = other.m_data[i];
        }
    }
    return *this;
}

template <typename T>
Vector<T>::Vector(Vector &&other)
    : m_data(other.m_data), m_size(other.m_size), m_capacity(other.m_capacity)
{
    other.m_data = nullptr;
    other.m_size = 0;
    other.m_capacity = 0;
}

template <typename T>
Vector<T> &Vector<T>::operator=(Vector<T> &&other)
{
    if (this != &other) {
        delete[] m_data;
        m_data = other.m_data;
        m_size = other.m_size;
        m_capacity = other.m_capacity;
        other.m_data = nullptr;
        other.m_size = 0;
        other.m_capacity = 0;
    }
    return *this;
}

template <typename T>
Vector<T>::~Vector()
{
    clear();
}

template <typename T>
T &Vector<T>::front() const
{
    return m_data[0];
}

template <typename T>
T &Vector<T>::back() const
{
    return m_data[m_size - 1];
}

template <typename T>
T &Vector<T>::operator[](int index)
{
    return m_data[index];
}

template <typename T>
const T &Vector<T>::operator[](int index) const
{
    return m_data[index];
}

template <typename T>
void Vector<T>::push_back(const T &value)
{
    if (m_size == m_capacity)
        reserve(m_capacity * 2);
    m_data[m_size++] = value;
}

template <typename T>
void Vector<T>::resize(size_t size)
{
    if (size > m_capacity)
        reserve(size);
    if (size > m_size) {
        for (size_t i = m_size; i < size; i++)
            m_data[i] = T();
    } else if (size < m_size) {
        for (size_t i = size; i < m_size; i++)
            m_data[i].~T();
    }
    m_size = size;
}

template <typename T>
size_t Vector<T>::size() const
{
    return m_size;
}

template <typename T>
bool Vector<T>::empty() const
{
    return m_size == 0;
}

template <typename T>
void Vector<T>::reserve(size_t capacity)
{
    if (capacity <= m_capacity)
        return;
    T *newData = new T[capacity];
    for (size_t i = 0; i < m_size; i++)
        newData[i] = move(m_data[i]);
    delete[] m_data;
    m_data = newData;
    m_capacity = capacity;
}

template <typename T>
size_t Vector<T>::capacity() const
{
    return m_capacity;
}

template <typename T>
void Vector<T>::clear()
{
    delete[] m_data;
    m_data = nullptr;
    m_size = 0;
    m_capacity = 0;
}

#endif // LIB_VECTOR_H
