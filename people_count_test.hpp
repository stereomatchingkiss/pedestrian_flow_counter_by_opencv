#ifndef PEOPLE_COUNT_TEST_HPP
#define PEOPLE_COUNT_TEST_HPP

#include <opencv2/core.hpp>
#include <opencv2/bgsegm.hpp>
#include <opencv2/tracking.hpp>

#include <memory>

class tracker_base;

class people_count_test
{
public:
    people_count_test();

    ~people_count_test();

    void test_00(std::string const &file);

private:
    void draw_measure_line();
    void init_data();
    void preprocess_fg(cv::Mat const &kernel);
    void preprocess_input();
    void put_inout_msg();
    void remove_outlier_blobs();
    void track(std::string const &video,
               size_t neglect_frame_size);
    void track_people(cv::Mat const &kernel);

    std::vector<cv::Rect> bound_rects_;
    cv::Mat contour_img_;
    std::vector<std::vector<cv::Point>> contours_;
    bool debug_ = true;
    cv::Mat fg_;
    cv::Ptr<cv::BackgroundSubtractor> fgbg_;
    cv::Mat gray_img_;
    cv::Mat img_;
    size_t in_blob_size_ = 0;
    double min_human_size_ = 4000;
    int offset_ = 50;
    size_t out_blob_size_ = 0;
    size_t process_frame_ = 0;
    std::unique_ptr<tracker_base> tracker_;
};

#endif // PEOPLE_COUNT_TEST_HPP
