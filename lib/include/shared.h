#ifndef LIB_SHARED_H
#define LIB_SHARED_H

// TODO: Implement make_shared()

template <typename T>
class Shared {
private:
    T *m_data;
    int *m_refCount;

public:
    explicit Shared(T *p = nullptr);
    Shared(const Shared &other);
    Shared &operator=(const Shared &other);
    ~Shared();

    T &operator*() const { return *m_data; }
    T *operator->() const { return m_data; }
    bool operator!() const { return !m_data; }

    T *data() const { return m_data; }
    int count() const { return m_refCount ? *m_refCount : 0; }

private:
    void release();
};

template <typename T>
Shared<T>::Shared(T *p)
    : m_data(p)
    , m_refCount(new int)
{
    *m_refCount = p ? 1 : 0;
}

template <typename T>
Shared<T>::Shared(const Shared &other)
    : m_data(other.m_data)
    , m_refCount(other.m_refCount)
{
    if (m_refCount)
        ++(*m_refCount);
}

template <typename T>
Shared<T> &Shared<T>::operator=(const Shared<T> &other) {
    if (this != &other) {
        release();
        m_data = other.m_data;
        m_refCount = other.m_refCount;
        if (m_refCount)
            ++(*m_refCount);
    }
    return *this;
}

template <typename T>
Shared<T>::~Shared() {
    release();
}

template <typename T>
void Shared<T>::release() {
    if (m_refCount) {
        (*m_refCount)--;
        if (*m_refCount == 0) {
            delete m_data;
            m_data = nullptr;
            delete m_refCount;
            m_refCount = nullptr;
        }
    }
}

#endif // LIB_SHARED_H
