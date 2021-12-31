#include "file_utils.hpp"
#include "timestamp.hpp"

#include "encode.hpp"
#include <iostream>
#include <string.h>
#include <vector>
#include <string>

using namespace std;
using namespace StBase;

typedef enum cv_object {
        CV_OT_FACE = 1, ///< 人脸
        CV_OT_BODY = 2, ///< 行人
        CV_OT_CYCLIST = 3, ///< 骑行人(人+非机动车)
        CV_OT_VEHICLE = 4, ///< 机动车
        CV_OT_BICYCLE = 5, ///< 非机动车
        CV_OT_HEAD_SHOULDER = 6, ///< 头肩
        CV_OT_UNKNOWN
} cv_object_e;
/// cv rectangle definition
typedef struct cv_rect {
        int left; ///< 矩形最左边的坐标
        int top; ///< 矩形最上边的坐标
        int right; ///< 矩形最右边的坐标
        int bottom; ///< 矩形最下边的坐标
} cv_rect_t;

typedef struct cv_tracker {
        cv_object_e label; ///< 目标类型
        int id; ///< 目标的tracking ID
        cv_rect_t rect; ///< 目标所在原图的位置
        float confidence; ///< 目标置信度
} cv_tracker_t;

struct Rect_t {
        std::vector<cv_rect> rects;
        
        Rect_t() = default;
        Rect_t(const std::vector<cv_tracker_t>&);
        Rect_t(const Rect_t &) = default;
        Rect_t(Rect_t &&) = default;

        void operator=(Rect_t&&);
        void format(const std::string &str);
        std::string string() const;
};

struct HeaderShoulderDb_t {
        int id;
        int count;
        int medium;
        Rect_t rectangles;
        std::string url;
        StBase::Timestamp captureTime;

        HeaderShoulderDb_t() = default;
        HeaderShoulderDb_t(const HeaderShoulderDb_t &) = default;
        HeaderShoulderDb_t(HeaderShoulderDb_t &&) = default;

};


void func() {

}

struct AA{
	int a;
	Timestamp b;
};


int main() {
//     std::vector<HeaderShoulderDb_t> vec;
//     HeaderShoulderDb_t a;
//     a.url = "dsadsadsa";
//     a.rectangles.rects.emplace_back(cv_rect({1,2,3,4}));
//     vec.emplace_back(a);
//     cout << vec[0].rectangles.rects[0].right << endl;
	int64_t a = 1640627965;
	Timestamp ttmp(a);
	Timestamp ttmp1 = Timestamp::FromUnixTime(a);
    printf(">>>>>> [%ld] [%s]\n", ttmp.ToUnixTime(), ttmp.FormatString().c_str());
	printf(">>>>>> [%ld] [%s]\n", ttmp1.ToUnixTime(), ttmp1.FormatString().c_str());
        
}