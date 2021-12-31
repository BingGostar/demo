#include <string>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/stat.h>

namespace StBase {

class FileUtils
{
public:
    static int ReadFile(const std::string & src, std::vector<char>&& buf) {
        int fd = ::open(src.c_str(), O_RDWR | O_CLOEXEC);
        if (fd < 0) return errno;
        return FileUtils::ReadAll(fd, std::forward<std::vector<char>>(buf));
    }

    static int ReadAll(const int fd, std::vector<char>&& buf) {
        assert(fd >= 0);
        struct stat fstat;
        if (::fstat(fd, &fstat)) {
            return errno;
        }
        if (S_ISREG(fstat.st_mode)) {
            size_t fsize = fstat.st_size;
            buf.resize(fsize);
            return FileUtils::Read(fd, buf.data(), fsize);
        }
        return 0;
    }

    static int Read(const int fd, char* buf, size_t len) {
        assert(fd >= 0);
        char* ptr = buf;
        size_t nLeft = len;
        size_t r;
        while(nLeft > 0) {
            if ((r = ::read(fd, ptr, nLeft)) < 0) {
                if (errno == EINTR) {
                    r = 0;
                } else {
                    return -1;
                }
            }
            nLeft -= r;
            ptr += r;
        }
        return len - nLeft;
    }

    static int Write(const std::string & src, const char* buf, const size_t len) {
        int fd = ::open(src.c_str(), O_RDWR | O_CREAT | O_CLOEXEC, 0664);
        if (fd < 0) {
            return errno;
        }
        int res = FileUtils::Write(fd, buf, len);
        ::close(fd);
        if (res != len) return errno;
        return res;
    }

    static int Write(const int fd, const char* buf, const size_t len) {
        assert(fd >= 0);
        char* ptr = const_cast<char*>(buf);
        size_t nLeft = len;
        size_t r;
        while (nLeft > 0) {
            if ((r = ::write(fd, ptr, nLeft)) < 0) {
                if (errno == EINTR) {
                    r = 0;
                } else {
                    return -1;
                }
            }
            ptr += r;
            nLeft -= r;
        }

        return len;
    }

};

class Path {
public:
    bool is_dir();
    bool is_file();
    bool exists();

    std::string basename(const std::string & src) {
        size_t lastPos = src.find_last_of("/");
        if (lastPos == std::string::npos) return src;
        return src.substr(lastPos + 1);
    }

    std::string dirname(const std::string & src) {
        size_t lastPos = src.find_last_of("/");
        if (lastPos == std::string::npos) return src;
        return src.substr(0, lastPos + 1);
    }
};
    
}