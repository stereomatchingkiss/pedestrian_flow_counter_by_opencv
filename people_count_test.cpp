#include "people_count_test.hpp"
#include "bg_subtract_detector.hpp"
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
    in_blob_size_ = 0;
    out_blob_size_ = 0;
    double constexpr anchor_ratio = 0.5;
    blob_detector_ = std::make_unique<bg_subtract_detector>(cv::createBackgroundSubtractorMOG2(500, 60, true),
                                                            min_human_size_, anchor_ratio, detect_margin_);

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

    tracker_ = std::make_unique<opencv_trackers>(legal_roi_strategy,
                                                 lost_track_strategy, "KCF");
    //tracker_ = std::make_unique<dlib_tracker>(legal_roi_strategy, lost_track_strategy);
}

void people_count_test::draw_measure_line()
{
    int const ypos = img_.rows / 2;
    cv::line(img_, {0, ypos - detect_margin_}, {img_.cols, ypos - detect_margin_}, {255, 0, 0});
    cv::line(img_, {0, ypos}, {img_.cols, ypos}, {255, 0, 0});
    cv::line(img_, {0, ypos + detect_margin_}, {img_.cols, ypos + detect_margin_}, {255, 0, 0});
}

void people_count_test::track(std::string const &video)
{
    cv::VideoCapture cap(video);
    if(cap.isOpened()){
        init_data();        
        cv::VideoWriter vwrite = create_video_writer(video);
        cap>>img_;
        for(; !img_.empty(); cap>>img_){
            draw_measure_line();
            blob_detector_->detect_blob(img_);
            tracker_->match_existing_rects(img_, blob_detector_->get_roi());
            tracker_->draw_track_blobs(img_, img_);
            put_inout_msg();
            cv::imshow("img", img_);
            vwrite.write(img_);

            int const c = cv::waitKey(30);
            if (c == 'q' || c == 'Q' || (c & 255) == 27){
                break;
            }            
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
    track(file);
}
