#ifndef TRACKER_BASE_H
#define TRACKER_BASE_H

#include <opencv2/core.hpp>

#include <functional>

class tracker_base
{
public:
    tracker_base();

    tracker_base(tracker_base const&) = delete;
    tracker_base& operator=(tracker_base const&) = delete;
    tracker_base(tracker_base &&) = delete;
    tracker_base& operator=(tracker_base &&) = delete;

    virtual ~tracker_base();

    virtual void draw_track_blobs(cv::Mat const &input, cv::Mat &output) = 0;

    virtual size_t get_max_track_frame() const noexcept = 0;

    virtual void track(cv::Mat const &image,
                       std::vector<cv::Rect> const &current_blobs) = 0;
    virtual void set_lost_track_ratio(double ratio) noexcept = 0;
    virtual void set_max_track_frame(size_t size) noexcept = 0;

protected:
    virtual void track(const cv::Mat &image, const std::vector<cv::Rect> &current_blobs,
                       std::vector<cv::Rect2d> const &roi,
                       double lost_track_ratio);

private:
    virtual void add_blob(cv::Mat const &input, cv::Rect2d const &roi) = 0;
    virtual void add_blob(cv::Mat const &input, std::vector<cv::Rect2d> const &roi);
    virtual void add_blob(cv::Mat const &input, std::vector<cv::Rect> const &roi);

    virtual void update(cv::Mat const &image) = 0;
};

#endif // TRACKER_BASE_H
