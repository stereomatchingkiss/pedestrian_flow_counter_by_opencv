#include "bg_subtract_detector.hpp"

#include <ocv_libs/core/contour_utility.hpp>
#include <ocv_libs/core/rect_utility.hpp>

#include <opencv2/bgsegm.hpp>
#include <opencv2/imgproc.hpp>

struct bg_subtract_detector::impl
{
    impl(cv::Ptr<cv::BackgroundSubtractor> bg_subtractor,
         double blob_min_size,
         double anchor_ratio,
         int detect_margin,
         cv::Mat const kernel,
         size_t neglect_frame_size = 10);

    void detect_blob(cv::Mat const &input);
    void preprocess_fg()
    {
        cv::GaussianBlur(fg_, fg_, {5,5}, 0);
        cv::threshold(fg_, fg_, 127, 255, cv::THRESH_BINARY);
        cv::dilate(fg_, fg_, kernel_);
        cv::findContours(fg_, contours_, cv::RETR_EXTERNAL,
                         cv::CHAIN_APPROX_SIMPLE);
    }

    void remove_outlier_blobs()
    {
        int const key_point = input_.rows * anchor_ratio_;
        auto it = std::remove_if(std::begin(bound_rects_), std::end(bound_rects_),
                                 [=](cv::Rect const &r)
        {
            auto const center = ocv::rect_center(r);
            return center.y < key_point - detect_margin_ ||
                    center.y > key_point + detect_margin_ ||
                    r.area() < blob_min_size_;
        });
        bound_rects_.erase(it, std::end(bound_rects_));
    }

    double anchor_ratio_;
    cv::Ptr<cv::BackgroundSubtractor> bg_subtract_;
    double blob_min_size_ = 4000;
    std::vector<cv::Rect> bound_rects_;
    std::vector<std::vector<cv::Point>> contours_;
    int detect_margin_ = 50;
    cv::Mat fg_;
    cv::Mat gray_img_;
    cv::Mat input_;
    cv::Mat kernel_ = cv::getStructuringElement(cv::MORPH_RECT, {7,7});
    size_t neglect_frame_size_ = 10;
    size_t process_frame_ = 0;    
};

bg_subtract_detector::bg_subtract_detector(cv::Ptr<cv::BackgroundSubtractor> bg_subtractor,
                                           double blob_min_size,
                                           double anchor_ratio,
                                           int detect_margin,
                                           cv::Mat const kernel,
                                           size_t neglect_frame_size) :
    pimpl_(new bg_subtract_detector::impl(bg_subtractor, blob_min_size, anchor_ratio,
                                          detect_margin, kernel, neglect_frame_size))
{

}

bg_subtract_detector::~bg_subtract_detector()
{

}

void bg_subtract_detector::clear()
{
    pimpl_->process_frame_ = 0;
}

void bg_subtract_detector::detect_blob(const cv::Mat &input)
{
    pimpl_->detect_blob(input);
}

const std::vector<cv::Rect> &bg_subtract_detector::get_roi() const
{
    return pimpl_->bound_rects_;
}

bg_subtract_detector::impl::impl(cv::Ptr<cv::BackgroundSubtractor> bg_subtractor,
                                 double blob_min_size,
                                 double anchor_ratio,
                                 int detect_margin,
                                 cv::Mat const kernel,
                                 size_t neglect_frame_size) :
    anchor_ratio_(anchor_ratio),
    bg_subtract_(bg_subtractor),
    blob_min_size_(blob_min_size),
    detect_margin_(detect_margin),
    kernel_(kernel),
    neglect_frame_size_(neglect_frame_size)
{
    bg_subtract_ = bg_subtractor;
    neglect_frame_size_ = neglect_frame_size;
}

void bg_subtract_detector::impl::detect_blob(cv::Mat const &input)
{
    input_ = input;
    cv::cvtColor(input_, gray_img_, CV_BGR2GRAY);
    bg_subtract_->apply(gray_img_, fg_);    
    if(process_frame_ > neglect_frame_size_){
        preprocess_fg();
        ocv::contours_to_bounding_rect(contours_, bound_rects_);
        remove_outlier_blobs();
    }else{
        ++process_frame_;
    }
}
