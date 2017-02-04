#ifndef PEOPLE_COUNT_TEST_HPP
#define PEOPLE_COUNT_TEST_HPP

#include <opencv2/core.hpp>
#include <opencv2/bgsegm.hpp>
#include <opencv2/tracking.hpp>

#include <memory>

class blob_detector_base;
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
    void put_inout_msg();
    void track(std::string const &video);
    void track_people(cv::Mat const &kernel);

    std::unique_ptr<blob_detector_base> blob_detector_;
    cv::Mat gray_img_;
    cv::Mat img_;
    size_t in_blob_size_ = 0;
    double min_human_size_ = 4000;
    int detect_margin_ = 50;
    size_t out_blob_size_ = 0;    
    std::unique_ptr<tracker_base> tracker_;
};

#endif // PEOPLE_COUNT_TEST_HPP
