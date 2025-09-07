#ifndef LIB_UNIQUE_H
#define LIB_UNIQUE_H

// TODO: Implement make_unique()

template <typename T>
class Unique {
private:
    T *m_data;

public:
    explicit Unique(T *p = nullptr) : m_data(p) {}

    Unique(const Unique &) = delete;
    Unique &operator=(const Unique &) = delete;

    Unique(Unique &&other) noexcept : m_data(other.m_data) {
        other.m_data = nullptr;
    }
    Unique &operator=(Unique &&other) noexcept {
        if (this != &other) {
            delete m_data;
            m_data = other.m_data;
            other.m_data = nullptr;
        }
        return *this;
    }

    ~Unique() {
        delete m_data;
    }

    T &operator*() const { return *m_data; }
    T *operator->() const { return m_data; }
    bool operator!() const { return !m_data; }

    T *data() const { return m_data; }
    T *release() {
        T *temp = m_data;
        m_data = nullptr;
        return temp;
    }

    void reset(T *p = nullptr) {
        if (m_data != p) {
            delete m_data;
            m_data = p;
        }
    }
};

#endif // LIB_UNIQUE_H
