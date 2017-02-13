#ifndef DLIB_TRACKER_H
#define DLIB_TRACKER_H

#include "tracker_base.hpp"

#include <opencv2/core.hpp>

#include <dlib/image_processing.h>

class dlib_tracker : public tracker_base
{
public:
    dlib_tracker(std::function<bool(cv::Rect2d const &roi)> legal_roi_strategy,
                 std::function<void(cv::Rect2d const &origin_roi,
                                    cv::Rect2d const &current_roi)> lost_track_strategy);

    void draw_track_blobs(cv::Mat const &input, cv::Mat &output) override;

    size_t get_max_track_frame() const noexcept override;

    void track(cv::Mat const &image,
               std::vector<cv::Rect> const &current_blobs) override;

    void set_lost_track_ratio(double ratio) noexcept override;
    void set_max_track_frame(size_t size) noexcept override;

private:
    using tracker_type = dlib::correlation_tracker;

    struct blob
    {
        blob();
        blob(tracker_type &&track,
             size_t track_time, cv::Rect2d const &roi,
             size_t track_id);

        cv::Rect2d origin_roi_;
        tracker_type track_;
        size_t track_id_ = 0;
        size_t track_time_ = 0;
    };

    void add_blob(cv::Mat const &input, cv::Rect2d const &roi) override;

    cv::Rect2d to_cv_rect(dlib::drectangle const &rect) const noexcept;

    void to_gray(const cv::Mat &input);

    void update(cv::Mat const &image) override;

    cv::Mat img_;
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

#endif // DLIB_TRACKER_H
