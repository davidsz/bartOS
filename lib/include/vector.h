#ifndef LIB_VECTOR_H
#define LIB_VECTOR_H

#include <stddef.h>
#include "heap.h"

template <typename T>
class Vector {
public:
    explicit Vector();
    Vector(Vector &&other);
    Vector<T> &operator=(Vector<T> &&other);
    ~Vector();

    // TODO: Implement iterators
    T &front() const;
    T &back() const;
    T &operator[](int index);
    const T &operator[](int index) const;

    size_t size() const;
    bool empty() const;

    void push_back(const T &value);
    void reserve(size_t capacity);
    size_t capacity() const;
    void clear();

private:
    T *m_data;
    size_t m_size;
    size_t m_capacity;
};

template <typename T>
Vector<T>::Vector()
    : m_data(nullptr)
    , m_size(0)
    , m_capacity(0)
{
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
    T *newData = (T *)malloc(sizeof(T) * capacity);
    for (size_t i = 0; i < m_size; i++)
        newData[i] = m_data[i];
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
