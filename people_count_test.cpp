#include "people_count_test.hpp"
#include "dlib_tracker.hpp"
#include "opencv_trackers.hpp"

#include <dlib/image_processing.h>
#include <dlib/opencv/cv_image.h>

#include <ocv_libs/core/contour_utility.hpp>
#include <ocv_libs/core/rect_utility.hpp>

#include <opencv2/core.hpp>
#include <opencv2/bgsegm.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video/background_segm.hpp>

#include <iostream>

namespace{

cv::VideoWriter create_video_writer(std::string const &video)
{
    cv::VideoCapture cap(video);
    int const frame_width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    int const frame_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
    int const fps = cap.get(CV_CAP_PROP_FPS);
    //CV_FOURCC('P','I','M','1') CV_FOURCC('M','J','P','G')
    return cv::VideoWriter("out.avi", CV_FOURCC('P','I','M','1'),
                           fps, {frame_width,frame_height});
}

} //nameless namespace

people_count_test::people_count_test()
{

}

people_count_test::~people_count_test()
{

}

void people_count_test::init_data()
{
    process_frame_ = 0;
    in_blob_size_ = 0;
    out_blob_size_ = 0;
    fgbg_ = cv::createBackgroundSubtractorMOG2(500, 60, true);

    auto legal_roi_strategy = [this](auto const &r)
    {
        return r.area()> min_human_size_ && (r.height > r.width);
    };

    auto lost_track_strategy = [this](auto &&origin, auto &&current)
    {
        if(origin.y > current.y){
            ++in_blob_size_;
        }else{
            ++out_blob_size_;
        }
    };

    //tracker_ = std::make_unique<opencv_trackers>(legal_roi_strategy,
    //                                             lost_track_strategy, "KCF");
    tracker_ = std::make_unique<dlib_tracker>(legal_roi_strategy, lost_track_strategy);
}

void people_count_test::draw_measure_line()
{
    int const ypos = img_.rows / 2;
    cv::line(img_, {0, ypos - offset_}, {img_.cols, ypos - offset_}, {255, 0, 0});
    cv::line(img_, {0, ypos}, {img_.cols, ypos}, {255, 0, 0});
    cv::line(img_, {0, ypos + offset_}, {img_.cols, ypos + offset_}, {255, 0, 0});
}

void people_count_test::preprocess_input()
{
    cv::cvtColor(img_, gray_img_, CV_BGR2GRAY);
    fgbg_->apply(gray_img_, fg_);
}

void people_count_test::preprocess_fg(cv::Mat const &kernel)
{
    cv::GaussianBlur(fg_, fg_, {5,5}, 0);
    cv::threshold(fg_, fg_, 127, 255, cv::THRESH_BINARY);
    cv::dilate(fg_, fg_, kernel);
    if(debug_){
        fg_.copyTo(contour_img_);
    }
    cv::findContours(fg_, contours_, cv::RETR_EXTERNAL,
                     cv::CHAIN_APPROX_SIMPLE);
    if(debug_){
        contour_img_.copyTo(fg_);
        contour_img_.setTo(0);
    }
}

void people_count_test::remove_outlier_blobs()
{
    int const key_point = img_.rows / 2;
    auto it = std::remove_if(std::begin(bound_rects_), std::end(bound_rects_),
                             [=](cv::Rect const &r)
    {
        auto const center = ocv::rect_center(r);
        return center.y < key_point - offset_ ||
                center.y > key_point + offset_ ||
                r.area() < min_human_size_;
    });
    bound_rects_.erase(it, std::end(bound_rects_));
}

void people_count_test::track_people(cv::Mat const &kernel)
{
    preprocess_fg(kernel);
    //cv::drawContours(contour_img_, contours_, -1, {255}, -1);
    ocv::contours_to_bounding_rect(contours_, bound_rects_);
    remove_outlier_blobs();
    tracker_->match_existing_rects(img_, bound_rects_);
    tracker_->draw_track_blobs(img_, img_);
    put_inout_msg();
    if(debug_){
        cv::imshow("contour_img", contour_img_);
        cv::imshow("fg", fg_);
    }
}

void people_count_test::track(std::string const &video,
                              size_t neglect_frame_size)
{
    cv::VideoCapture cap(video);
    if(cap.isOpened()){
        init_data();
        cv::Mat const kernel = cv::getStructuringElement(cv::MORPH_RECT, {7,7});
        cv::VideoWriter vwrite = create_video_writer(video);
        cap>>img_;
        for(; !img_.empty(); cap>>img_){
            preprocess_input();
            draw_measure_line();
            if(process_frame_ > neglect_frame_size){
                track_people(kernel);
            }
            cv::imshow("img", img_);
            vwrite.write(img_);

            int const c = cv::waitKey(30);
            if (c == 'q' || c == 'Q' || (c & 255) == 27){
                break;
            }
            std::cout<<"frame "<<process_frame_++<<std::endl;
        }
    }else{
        std::cerr<<"cannot open "<<video<<"\n";
    }
}

void people_count_test::put_inout_msg()
{
    cv::putText(img_, "in " + std::to_string(in_blob_size_),
    {0 + 20, img_.rows / 2 - 20}, cv::FONT_HERSHEY_TRIPLEX, 1, {255, 0, 0});
    cv::putText(img_, "out " + std::to_string(out_blob_size_),
    {0 + 20, img_.rows / 2 + 20}, cv::FONT_HERSHEY_TRIPLEX, 1, {255, 0, 0});
}

void people_count_test::test_00(std::string const &file)
{
    track(file, 10);
}
