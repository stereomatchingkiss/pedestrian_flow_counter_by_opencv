#ifndef BG_SUBTRACT_DETECTOR_HPP
#define BG_SUBTRACT_DETECTOR_HPP

#include "blob_detector_base.hpp"

#include <opencv2/bgsegm.hpp>

#include <memory>

class bg_subtract_detector : public blob_detector_base
{
public:
    /**
     * @param subtractor self explain
     * @param blob_min_size If the blob size lower than the value,
     * this detector would removed it
     * @param anchor_ratio This determine the location of the anchor line,
     * it will set the anchor line at position input.rows * anchor_ratio
     * @param detect_margin margin between anchor line, this detector only detect
     * the blobs within these margins
     * @param kernel kernel for morphology operations, used to removed small blobs(noise)
     * @param neglect_frame_size some background subtractor need to "learn" the
     * background before they can work well, this param control how many frames
     * they need to learn before it can start to separate foreground and background
     */
    bg_subtract_detector(cv::Ptr<cv::BackgroundSubtractor> bg_subtractor,
                         double blob_min_size = 4000,
                         double anchor_ratio = 0.5,
                         int detect_margin = 50,
                         cv::Mat const kernel = cv::getStructuringElement(cv::MORPH_RECT, {7,7}),
                         size_t neglect_frame_size = 10);
    ~bg_subtract_detector();

    void clear() override;

    /**
     * @brief detect interesting blob in the screen
     * @param input image want to detect interesting blob
     */
    void detect_blob(cv::Mat const &input) override;
    /**
     * @brief get the image after processed
     * @return image after processed
     * @warning must called detect_blob before you call this api
     */
    cv::Mat get_processed_image() const override;
    /**
     * @brief get interesting regions
     * @return bounding boxes of interesting regions
     */
    std::vector<cv::Rect> const& get_roi() const override;

private:
    struct impl;

    std::unique_ptr<impl> pimpl_;
};

#endif // BG_SUBTRACT_DETECTOR_HPP
