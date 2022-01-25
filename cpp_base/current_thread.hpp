// 编译选项 -rdynamic

#include <string>
#include <cxxabi.h>
#include <execinfo.h>
#include <stdlib.h>
#include <sstream>

namespace StBase {

class CurrentThread {
public:
    static std::string stackTrace() {
        std::stringstream ss;
        const int size = 64;
        void* buffer[size];
        int bufN = ::backtrace(buffer, size);
        char** symbollist = ::backtrace_symbols(buffer, bufN);

        size_t funcSize = 256;
        char* funcname = static_cast<char*>(::malloc(funcSize));

        for (size_t i = 1; i < bufN; i++) {
            char* begin_name = NULL;
            char* begin_offset = NULL;
            char* end_offset = NULL;
            for (char* p = symbollist[i]; *p; p++) {
                if (*p == '(') {
                    begin_name = p;
                } else if (*p == '+') {
                    begin_offset = p;
                } else if (*p == ')' && begin_offset) {
                    end_offset = p;
                    break;
                }
            }

            if (begin_name && begin_offset && end_offset 
                && begin_offset < end_offset)
            {
                *begin_name++ = '\0';
                *begin_offset++ = '\0';
                *end_offset = '\0';
                int status = 0;
                char* ret = abi::__cxa_demangle(begin_name, funcname, &funcSize, &status);
                ss << symbollist[i] << " : ";
                if (status == 0) {
                    funcname = ret;
                    ss << funcname << "+";
                } else {
                    ss << begin_name << "()+";
                }
                ss << begin_offset << '\n';
            } else {
                fprintf(stdout, "  %s\n", symbollist[i]);
            }
        }
        free(funcname);
        free(symbollist);
        return ss.str();
    }
};

}