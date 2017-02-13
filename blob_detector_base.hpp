#ifndef BLOB_DETECTOR_BASE_HPP
#define BLOB_DETECTOR_BASE_HPP

#include <opencv2/core.hpp>

/**
 * Api convention of blob detector, this class required you to call
 * the api "detect_blob" before you call "get_processed_image" and
 * "get_bounding_boxes()"
 * @code
 * //....get input from camera
 * auto bg_subtract_detector = std::make_unique<bg_subtract_detector>();
 * bg_subtract_detector->detect_blob(input);
 * auto blob_img = bg_subtract_detector->get_processed_image();
 * auto const &blob_regions = bg_subtract_detector->get_roi();
 * @endcode
 */
class blob_detector_base
{
public:
    blob_detector_base();
    virtual ~blob_detector_base();

    blob_detector_base(blob_detector_base const&) = delete;
    blob_detector_base& operator=(blob_detector_base const&) = delete;
    blob_detector_base(blob_detector_base &&) = delete;
    blob_detector_base& operator=(blob_detector_base &&) = delete;

    /**
     * @brief reset state of detector
     */
    virtual void clear() = 0;

    /**
     * @brief detect interesting blob in the screen
     * @param input image want to detect interesting blob
     */
    virtual void detect_blob(cv::Mat const &input) = 0;

    /**
     * @brief get interesting regions
     * @return bounding boxes of interesting regions
     */
    virtual std::vector<cv::Rect> const& get_roi() const = 0;
};

#endif // BLOB_DETECTOR_BASE_HPP
