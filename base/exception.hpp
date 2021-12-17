#include <exception>
#include <string>

#include "current_thread.hpp"

namespace StBase
{

class Exception : public std::exception
{
    public:
    template <typename... Argv>
    Exception(const std::string &msg, Argv &&...argvs)
            : message_(StBase::CurrentThread::stackTrace())
    {
        char buf[1024] = { 0 };
        sprintf(buf, msg.c_str(), std::forward<Argv>(argvs)...);
        message_.push_back('\n');
        message_.append(buf, sizeof(buf));
    }

    ~Exception() noexcept override = default;

    const char *what() const noexcept override
    {
        return message_.c_str();
    }

    bool operator==(const std::string &errMsg)
    {
        return message_.compare(errMsg) == 0;
    }

    private:
    std::string message_;
};

}  // namespace StBase