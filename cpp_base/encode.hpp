#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <boost/algorithm/hex.hpp>
#include <string>

namespace StBase {

class Encode {
public:
    static std::string Base64Encode(const char* buf, size_t size) {
        using namespace boost::archive::iterators;
        typedef base64_from_binary<transform_width<const char*, 6, 8>> base64EncodeIter;
        return std::string(base64EncodeIter(buf), base64EncodeIter(buf + size));
    }

    static std::string Base64Decode(const char* buf, size_t size) {
        using namespace boost::archive::iterators;
        typedef transform_width<binary_from_base64<const char*>, 8, 6> base64DecodeIter;
        return std::string(base64DecodeIter(buf), base64DecodeIter(buf + size));
    }

    static std::string CheckMd5(const char* buf, size_t size) {
        using boost::uuids::detail::md5;
        md5 hash;
        md5::digest_type digest;
        hash.process_bytes(buf, size);
        hash.get_digest(digest);
        const auto charDigest = reinterpret_cast<const char *>(&digest);
        std::string result;
        boost::algorithm::hex(charDigest, charDigest + sizeof(md5::digest_type), std::back_inserter(result));
        return result;
    }
};

}