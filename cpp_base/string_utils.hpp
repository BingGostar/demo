#include <string>
#include <vector>

namespace StBase {

class StringUtils {
public:
    static std::vector<std::string> Split(const std::string & s, const std::string & delim) {
        std::vector<std::string> tokens;
        size_t lastPos = s.find_first_not_of(delim, 0);
        size_t pos = s.find_first_of(delim, lastPos);
        while (lastPos != std::string::npos || pos != std::string::npos) {
            tokens.emplace_back(s.substr(lastPos, pos - lastPos));
            lastPos = s.find_first_not_of(delim, pos);
            pos = s.find_first_of(delim, lastPos);
        }
        return tokens;
    }
};



}