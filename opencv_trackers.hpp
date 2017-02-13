#ifndef OPENCV_TRACKERS_H
#define OPENCV_TRACKERS_H

#include "tracker_base.hpp"

#include <opencv2/tracking.hpp>

class opencv_trackers : public tracker_base
{
public:
    explicit opencv_trackers(std::function<bool(cv::Rect2d const &roi)> legal_roi_strategy,
                             std::function<void(cv::Rect2d const &origin_roi,
                                                cv::Rect2d const &current_roi)> lost_track_strategy,
                             std::string tracker_type = "KCF");

    void draw_track_blobs(cv::Mat const &input, cv::Mat &output) override;

    size_t get_max_track_frame() const noexcept override;

    void track(cv::Mat const &image,
               std::vector<cv::Rect> const &current_blobs) override;

    void set_lost_track_ratio(double ratio) noexcept override;
    void set_max_track_frame(size_t size) noexcept override;

private:
    struct blob
    {
        blob();
        blob(cv::Ptr<cv::Tracker> track, size_t track_time, cv::Rect2d const &roi,
             size_t track_id);

        cv::Rect2d current_roi_;
        cv::Rect2d origin_roi_;
        cv::Ptr<cv::Tracker> track_;
        size_t track_id_ = 0;
        size_t track_time_;
    };

    void add_blob(cv::Mat const &input, cv::Rect2d const &roi) override;

    void update(cv::Mat const &image) override;

    std::function<bool(cv::Rect2d const &roi)> legal_roi_strategy_;
    std::function<void(cv::Rect2d const &origin_roi,
                       cv::Rect2d const &current_roi)> lost_track_strategy_;
    double lost_track_ratio_ = 0.5;
    size_t max_track_frame_ = 50;
    std::vector<cv::Rect2d> roi_;
    size_t track_id_ = 0;
    std::string tracker_type_;
    std::vector<blob> trackers_;
};

#endif // OPENCV_TRACKERS_H
