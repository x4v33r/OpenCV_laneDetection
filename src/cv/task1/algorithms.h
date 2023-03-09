#ifndef CGCV_ALGORITHMS_H
#define CGCV_ALGORITHMS_H

#include <opencv2/opencv.hpp>
#include <numeric>

class algorithms
{
   public:
    
    struct Line
    {
       float slope = 0.f;
       int intercept = 0;
    };

    struct LocalMaximum
    {
        int theta_index;        // theta_index-idx accumulator matrix
        int rho_index;          // rho_index-idx accumulator matrix
        int accumulator_value;  // accumulator value
    };
      
    static void compute_grayscale(const cv::Mat &input_image, cv::Mat &grayscale_image);

    static void gaussian_blur(const cv::Mat &input_image, cv::Mat &blurred_image, const int &kernel_size,
                              const float &sigma);

    static void apply_convex_mask(const cv::Mat &input_image, const std::vector<cv::Point2i> &pts,
                                      cv::Mat &convex_mask, cv::Mat &masked_image);

    static cv::Point2i compute_intersection(Line &first_line, Line &second_line, const int &y);

    static void bin_lines(const cv::Mat &input_image,
                          std::vector<cv::Vec4i> &input_lines,
                          const float &slope_upper_threshold,
                          const float &slope_lower_threshold,
                          std::vector<std::vector<Line>> &output_params);

    static void average_lines(std::vector<std::vector<Line>> &input_params,
                              std::vector<Line> &output_params);

    static void compute_gradient(const cv::Mat &grayscale_image, cv::Mat &gradient_x, cv::Mat &gradient_y,
                                 cv::Mat &gradient_abs);

    
    static void non_maxima_suppression(const cv::Mat &gradient_image, const cv::Mat &gradient_x,
                                       const cv::Mat &gradient_y, cv::Mat &non_maxima);

    static void hysteresis(const cv::Mat &non_max_sup, const int threshold_min, const int threshold_max,
                           cv::Mat &output_image);

    static void canny_edges(const cv::Mat &blurred_input_image, const int threshold_min, const int threshold_max,
                          cv::Mat &output_image);
                          
    // Bonus
    static void hough_fill_accumulator(const cv::Mat&input_edges, const int rho, const float theta, const int num_rho,
                                       const int num_angle, cv::Mat &accumulator);

    static void hough_get_local_maxima(const cv::Mat &accumulator, const int threshold, const int max_num_lines,
                                        const int num_rho, const int num_angle,  std::vector<LocalMaximum> &local_maxima);

    static void hough_convert_to_pts(const std::vector<LocalMaximum> &local_maxima, const int factor, const int num_rho, const int rho,
                                        const float theta, std::vector<cv::Vec4i> &line_pts);
};

#endif  // CGCV_ALGORITHMS_H
