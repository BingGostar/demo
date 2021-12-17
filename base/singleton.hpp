#include "noncopyable.hpp"
#include <pthread.h>
#include <memory>
#include <type_traits>
#include <assert.h>

namespace StBase
{

template <typename T> class Singleton : noncopyable
{
    public:
    static T &Instance()
    {
        pthread_once(&once_, &Singleton::init);
        assert(value_.get());
        return *value_.get();
    }

    private:
    static void init()
    {
        // static_assert(std::is_base_of<Singleton, T>::value);
        value_ = std::unique_ptr<T>(new T());
    }

    protected:
    Singleton() = default;
    ~Singleton() = default;

    private:
    static std::unique_ptr<T> value_;
    static pthread_once_t once_;
};

template <typename T> std::unique_ptr<T> Singleton<T>::value_ = NULL;
template <typename T> pthread_once_t Singleton<T>::once_ = PTHREAD_ONCE_INIT;

}  // namespace StBase