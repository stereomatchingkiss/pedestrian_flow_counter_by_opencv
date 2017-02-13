#include "tracker_base.hpp"

#include <ocv_libs/core/rect_utility.hpp>

tracker_base::tracker_base()
{

}

tracker_base::~tracker_base()
{

}

void tracker_base::track(const cv::Mat &image,
                         const std::vector<cv::Rect> &current_blobs,
                         const std::vector<cv::Rect2d> &roi,
                         double lost_track_ratio)
{
    update(image);
    if(!roi.empty()){
        for(auto const &cblob : current_blobs){
            double least_distance = 1e10;
            for(auto const &r : roi){
                auto const distance = ocv::center_euclidean_dist(cv::Rect2d(cblob),
                                                                 r);
                least_distance = std::min(distance, least_distance);
            }

            if(least_distance >= ocv::diagonal_euclidean_dist(cblob) * lost_track_ratio){
                add_blob(image, cblob);
            }
        }
    }else{
        add_blob(image, current_blobs);
    }
}

void tracker_base::add_blob(const cv::Mat &input, const std::vector<cv::Rect2d> &roi)
{
    for(auto const &r : roi){
        add_blob(input, r);
    }
}

void tracker_base::add_blob(const cv::Mat &input, const std::vector<cv::Rect> &roi)
{
    for(auto const &r : roi){
        add_blob(input, cv::Rect2d(r));
    }
}
