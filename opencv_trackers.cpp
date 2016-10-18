#include "opencv_trackers.hpp"

#include <ocv_libs/core/rect_utility.hpp>

opencv_trackers::opencv_trackers(std::function<bool (const cv::Rect2d &)> legal_roi_strategy,
                                 std::function<void (const cv::Rect2d &, const cv::Rect2d &)> lost_track_strategy,
                                 std::string tracker_type) :
    legal_roi_strategy_(std::move(legal_roi_strategy)),
    lost_track_strategy_(std::move(lost_track_strategy)),
    tracker_type_(std::move(tracker_type))
{

}

void opencv_trackers::draw_track_blobs(cv::Mat const &input, cv::Mat &output)
{
    input.copyTo(output);
    for(auto const &tracker : trackers_){
        int const x = static_cast<int>(tracker.current_roi_.x);
        int const y = static_cast<int>(tracker.current_roi_.y - 20);
        //cv::putText(output, std::to_string(tracker.track_id_),
        cv::putText(output, "id : " + std::to_string(tracker.track_id_) +
                    ", " + std::to_string(tracker.current_roi_.area()),
        {x, y}, cv::FONT_HERSHEY_TRIPLEX, 1, {255, 250, 0});
        cv::rectangle(output, tracker.current_roi_, {255,0,0});
    }
}

size_t opencv_trackers::get_max_track_frame() const noexcept
{
    return max_track_frame_;
}

void opencv_trackers::update(const cv::Mat &image)
{
    roi_.clear();
    std::cout<<"tracker size : "<<trackers_.size()<<std::endl;
    for(auto it = std::begin(trackers_); it != std::end(trackers_);){        
        cv::Rect2d current_roi;
        if(it->track_->update(image, current_roi) && it->track_time_ < max_track_frame_){
            roi_.emplace_back(current_roi);
            it->current_roi_ = current_roi;
            ++(it->track_time_);
            ++it;
        }else{
            lost_track_strategy_(it->origin_roi_, it->current_roi_);
            it = trackers_.erase(it);
        }
    }
}

void opencv_trackers::add_blob(const cv::Mat &input, const cv::Rect2d &roi)
{
    if(legal_roi_strategy_(roi)){
        cv::Ptr<cv::Tracker> tracker = cv::Tracker::create(tracker_type_);
        tracker->init(input, roi);
        trackers_.emplace_back(tracker, 0, roi, track_id_++);
    }
}

void opencv_trackers::match_existing_rects(const cv::Mat &image,
                                           const std::vector<cv::Rect> &current_blobs)
{        
    tracker_base::match_existing_rects(image, current_blobs, roi_, lost_track_ratio_);
}

void opencv_trackers::set_lost_track_ratio(double ratio) noexcept
{
    lost_track_ratio_ = ratio;
}

void opencv_trackers::set_max_track_frame(size_t size) noexcept
{
    max_track_frame_ = size;
}

opencv_trackers::blob::blob() : track_time_(0)
{

}

opencv_trackers::blob::blob(cv::Ptr<cv::Tracker> track,
                            size_t track_time,
                            const cv::Rect2d &roi,
                            size_t track_id) :
    origin_roi_(roi),
    track_(track),
    track_id_(track_id),
    track_time_(track_time)
{

}
