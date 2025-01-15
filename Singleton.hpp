#pragma  once

#include <memory>
#include <mutex>

inline int CHOOSE = 1;

#if CHOOSE
template <typename T>
class Singleton {
public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    static T& getInstance() {
        static T instance;
        return instance;
    }

protected:
    Singleton() = default;
    virtual ~Singleton() = default;
};

# else
template <typename T>
class Singleton {
public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

   	static std::shared_ptr<T> getInstance() {
        static std::once_flag flag;
        std::call_once(flag, [&]() {
           m_instance  = std::shared_ptr<T>(new T);
        });
        return m_instance;
    }

    void printAddress() {
        std::cout << m_instance.get() << std::endl;
    }

protected:
    Singleton() = default;
    virtual ~Singleton() = default;

    static std::shared_ptr<T> m_instance;
};

template <typename T>
std::shared_ptr<T> Singleton<T>::m_instance = nullptr;

#endif
