#include "dlib_tracker.hpp"

#include <ocv_libs/core/rect_utility.hpp>

#include <opencv2/imgproc.hpp>

#include <dlib/opencv/cv_image.h>

dlib_tracker::dlib_tracker(std::function<bool (const cv::Rect2d &)> legal_roi_strategy,
                           std::function<void (const cv::Rect2d &,
                                               const cv::Rect2d &)> lost_track_strategy) :
    legal_roi_strategy_(std::move(legal_roi_strategy)),
    lost_track_strategy_(std::move(lost_track_strategy))
{

}

void dlib_tracker::draw_track_blobs(const cv::Mat &input, cv::Mat &output)
{
    input.copyTo(output);
    for(auto const &tracker : trackers_){
        dlib::drectangle const position =
                tracker.track_.get_position();
        cv::Rect2d const current_roi(to_cv_rect(position));
        int const x = static_cast<int>(current_roi.x);
        int const y = static_cast<int>(current_roi.y - 20);
        //cv::putText(output, std::to_string(tracker.track_id_),
        cv::putText(output, "id : " + std::to_string(tracker.track_id_) +
                    ", " + std::to_string(current_roi.area()),
        {x, y}, cv::FONT_HERSHEY_TRIPLEX, 1, {255, 250, 0});
        cv::rectangle(output, current_roi, {255,0,0});
    }
}

size_t dlib_tracker::get_max_track_frame() const noexcept
{
    return max_track_frame_;
}

void dlib_tracker::track(const cv::Mat &image,
                         const std::vector<cv::Rect> &current_blobs)
{
    tracker_base::track(image, current_blobs, roi_, lost_track_ratio_);
}

void dlib_tracker::set_lost_track_ratio(double ratio) noexcept
{
    lost_track_ratio_ = ratio;
}

void dlib_tracker::set_max_track_frame(size_t size) noexcept
{
    max_track_frame_ = size;
}

void dlib_tracker::to_gray(const cv::Mat &input)
{
    if(input.channels() != 1){
        cv::cvtColor(input, img_, CV_BGR2GRAY);
    }else{
        img_ = input;
    }
}

void dlib_tracker::add_blob(const cv::Mat &input, const cv::Rect2d &roi)
{
    to_gray(input);
    tracker_type tracker;
    dlib::cv_image<unsigned char> gray_img(img_);
    tracker.start_track(gray_img,
                        dlib::centered_rect(dlib::point(roi.x + roi.width/2,
                                                        roi.y + roi.height/2),
                                            roi.width,
                                            roi.height));
    trackers_.emplace_back(std::move(tracker), 0, roi, track_id_++);
}

cv::Rect2d dlib_tracker::to_cv_rect(const dlib::drectangle &rect) const noexcept
{
    return cv::Rect2d(rect.left(),
                      rect.top(),
                      rect.width(),
                      rect.height());
}

void dlib_tracker::update(const cv::Mat &image)
{
    to_gray(image);
    roi_.clear();
    std::cout<<"tracker size : "<<trackers_.size()<<std::endl;
    for(auto it = std::begin(trackers_); it != std::end(trackers_);){
        if(it->track_time_ < max_track_frame_){
            dlib::cv_image<unsigned char> gray_img(img_);
            it->track_.update(gray_img);
            roi_.emplace_back(to_cv_rect(it->track_.get_position()));
            ++(it->track_time_);
            ++it;
        }else{
            lost_track_strategy_(it->origin_roi_, to_cv_rect(it->track_.get_position()));
            it = trackers_.erase(it);
        }
    }
}

dlib_tracker::blob::blob(dlib_tracker::tracker_type &&track,
                         size_t track_time,
                         const cv::Rect2d &roi, size_t track_id) :
    origin_roi_(roi),
    track_(std::move(track)),
    track_id_(track_id),
    track_time_(track_time)
{

}
