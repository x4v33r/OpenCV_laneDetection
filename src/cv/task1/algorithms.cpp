#include "algorithms.h"

//===============================================================================
// compute_grayscale()
//-------------------------------------------------------------------------------
// TODO:  - iterate over all pixels in the image
//        - multiply [R,G,B] with [0.2989, 0.5870, 0.1140] and accumulate
// hints: - use cv::Vec3b type to access the color values of a 3-channel image
//        - be aware that OpenCV treats matrix accesses in row-major order!
//          (iterate through rows then columns)
//
// parameters:
//  - input_image: [CV_8UC3] input image for the grayscale calculation
//  - grayscale_image: [CV_8UC1] output grayscale image
// return: void
//===============================================================================
void algorithms::compute_grayscale(const cv::Mat &input_image, cv::Mat &grayscale_image)
{
}

//===============================================================================
// gaussian_blur()
//-------------------------------------------------------------------------------
// TODO:  - get a [k_s x 1] gaussian filter
//        - obtain a [k_s, k_s] with an outer product
//        - convolve the image with this kernel
//
// parameters:
//  - input_image: [CV_8UC1] input grayscale image
//  - blurred_image: [CV_8UC1] output blurred grayscale image
//  - kernel_size: size (x- and y-direction) of the filter kernel
//  - sigma: Variance of the filter kernel, controlling the smoothness
// return: void
//===============================================================================
void algorithms::gaussian_blur(const cv::Mat &input_image, cv::Mat &blurred_image, const int &kernel_size,
                               const float &sigma)
{
}

//===============================================================================
// compute_gradient()
//-------------------------------------------------------------------------------
// TODO:  - Compute the 1st Scharr derivative in x and once in y direction
//        - compute the per-pixel l2-Norm
// hint: use the arithmetic operations (pow, add, sqrt, ...) of OpenCV
//
// parameters:
//  - grayscale_image: [CV_8UC1] input blurred grayscale image for the gradient calculation
//  - gradient_x: [CV_32FC1] output image for the gradient in x direction
//  - gradient_y: [CV_32FC1] output image for the gradient in y direction
//  - gradient_abs: [CV_32FC1] output image for the gradient image
// return: void
//===============================================================================
void algorithms::compute_gradient(const cv::Mat &grayscale_image, cv::Mat &gradient_x, cv::Mat &gradient_y,
                                  cv::Mat &gradient_abs)
{
}

//================================================================================
// non_maxima_suppression()
//--------------------------------------------------------------------------------
// TODO:
//  - Depending on the gradient direction of the pixel classify each pixel P in one
//    of the following classes:
//    ____________________________________________________________________________
//    | class |direction                | corresponding pixels Q, R               |
//    |-------|-------------------------|-----------------------------------------|
//    | I     | β <= 22.5 or β > 157.5  | Q: same row (y), left column (x−1)      |
//    |       |                         | R: same row (y), right column (x+1)     |
//    |-------|-------------------------|-----------------------------------------|
//    | II    | 22.5 < β <= 67.5        | Q: row above (y-1), left column (x−1)   |
//    |       |                         | R: row below (y+1), right column (x+1)  |
//    |-------|-------------------------|-----------------------------------------|
//    | III   | 67.5 < β <= 112.5       | Q: row above (y-1), same column (x)     |
//    |       |                         | R: row below (y+1), same column (x)     |
//    |-------|-------------------------|-----------------------------------------|
//    | IV    | 112.5 < β <= 157.5      | Q: row below (y+1), left column (x−1)   |
//    |       |                         | R: row above (y-1), right column (x+1)  |
//    |_______|_________________________|_________________________________________|
//  - Compare the value of P with the values of Q and R:
//    If Q or R are greater than P -> set P to 0
//
// parameters:
//  - gradient_image: [CV_32FC1] matrix with the gradient image
//  - gradient_x: [CV_32FC1] matrix with the gradient in x direction
//  - gradient_y: [CV_32FC1] matrix with the gradient in y direction
//  - non_max_sup: [CV_32FC1] output matrix for the non maxima suppression
// return: void
//================================================================================
void algorithms::non_maxima_suppression(const cv::Mat &gradient_image, const cv::Mat &gradient_x,
                                        const cv::Mat &gradient_y, cv::Mat &non_max_sup)
{
    float RAD2DEG = (180.0 / CV_PI);
}

//================================================================================
// hysteresis()
//--------------------------------------------------------------------------------
// TODO:
//  - Set all pixels under the lower threshold to 0
//  - Set all pixels over the high threshold to 255
//  - Classify all weak edges (threshold_min <= weak edge < threshold_max)
//    - If one of the the 8 surrounding pixel values is higher than threshold_max,
//      also the weak pixel is a strong pixel
//    - Check this recursively to be sure not to miss one
//  - Set all remaining, non-classified pixels to 0
//
// parameters:
//  - non_max_sup: [CV_8UC1] input image containing the result of the NMS
//  - threshold_min: the lower threshold
//  - threshold_min: the upper threshold
//  - output_image: [CV_8UC1] output image for results of the hysteresis
// return: void
//================================================================================
void algorithms::hysteresis(const cv::Mat &non_max_sup, const int threshold_min, const int threshold_max,
                            cv::Mat &output_image)
{
}

//================================================================================
// canny_edges()
//--------------------------------------------------------------------------------
// TODO:
//  - Use your implementation of compute_gradient for G_x, G_y, G
//  - Use these results for the non-maxima suppression
//  - Apply the hysteresis
//  - copy the output to the output image
//
// parameters:
//  - blurred_input_image: [CV_8UC1] input blurred grayscale image
//  - threshold_min: the lower threshold
//  - threshold_max: the upper threshold
//  - output_image: [CV_8UC1] output image holding canny edges
// return: void
//================================================================================
void algorithms::canny_edges(const cv::Mat &blurred_input_image, const int threshold_min, const int threshold_max,
                             cv::Mat &output_image)
{
}

//===============================================================================
// apply_convex_mask()
//-------------------------------------------------------------------------------
// TODO: - create a bitwise mask using the pts (use a scalar > 0 as color)
//       - apply the bitwise mask to the input_image
// hint: fillPoly() might lead to unexpected crashes, use fillConvexPoly()
//
// parameters:
//  - input_image: [CV_8UC1] input image containing the edges
//  - pts: vector<cv::Point2i> containing the points that span the triangular mask
//  - convex_mask: [CV_8UC1] output image for the mask
//  - output_image: [CV_8UC1] output image for the masked edges
// return: void
//===============================================================================
void algorithms::apply_convex_mask(const cv::Mat &input_image, const std::vector<cv::Point2i> &pts,
                                   cv::Mat &convex_mask, cv::Mat &masked_image)
{
}

//===============================================================================
// bin_lines()
//-------------------------------------------------------------------------------
// TODO: - obtain two points from the cv::Vec4i
//       - transform the coordinates such that (0,0) is the bottom left corner
//       - compute the parameters w,b
//       - bin lines based on their slope in (left, center, right)
//       -----------------------------------------
//       | class   | condition                   |
//       -----------------------------------------
//       | center  | |w| > slope_upper_threshold |
//       | right   | w < slope_lower_threshold   |
//       | left    | otherwise                   |
//       -----------------------------------------
//       - push the vectors in the order [left, center, right]
// hint: x0 = l[0], h - l[1]
//       x1 = l[2], h - l[3]
//       add a small epsilon to avoid division by 0
//       the color of 12_bin_lines indicates correct classification
//
// parameters:
//  - input_image: [CV_8UC1] matrix containing the input image
//  - input_lines: vector containing the hough lines
//  - slope_upper_threshold: constant for line grouping
//  - slope_lower_threshold: constant for line grouping
//  - output_params: output vector<vector> (left, center, right) of lines
// return: void
//===============================================================================
void algorithms::bin_lines(const cv::Mat &input_image, std::vector<cv::Vec4i> &input_lines,
                           const float &slope_upper_threshold, const float &slope_lower_threshold,
                           std::vector<std::vector<Line>> &output_params)
{
    float eps = 0.001f;
}

//===============================================================================
// average_lines()
//-------------------------------------------------------------------------------
// TODO:  Calculate average slope and intercept of binned bins
// hint:  input vector size [3, |L_i|], output vector size [3]
//        for b: divide by |L_i| in float, then round to nearest int
//
// parameters:
//  - input_params: input vector<vector> containing the line params (w, b)
//  - output_params: output vector containing the averaged line parameters (w, b)
// return: void
//===============================================================================
void algorithms::average_lines(std::vector<std::vector<Line>> &input_params, std::vector<Line> &output_params)
{
}

//===============================================================================
// compute_intersection()
//-------------------------------------------------------------------------------
// TODO: - for each line, compute two points on the line at p_y = {0,y} in P^2
//       - transform the points to projective space
//       - compute two lines using the cross product
//       - compute the intersection point with the cross product
//       - perform perspective division (round to the nearest integer)
// hint: use cv::Point3f for points in projective space
//
// parameters:
//  - first_line: struct storing the parameters (slope, intercept) of the first line
//  - second_line: struct storing the parameters (slope, intercept) of the second line
//  - y: y-coordinate of p_2, p_4
// return: cv::Point2i intersection point
//===============================================================================
cv::Point2i algorithms::compute_intersection(Line &first_line, Line &second_line, const int &y)
{
}

//================================================================================
// BONUS
//================================================================================

//================================================================================
// hough_fill_accumulator()
//--------------------------------------------------------------------------------
// TODO: BONUS:
//  - precalculate needed sine and cosine values (weigh by rho^{-1})
//  - fill out accumulator array
//  - iterate over all pixels (x, y)
//  - if not equal to 0:
//      - iterate over num_angle
//      - compute r, add the diagonal
//      - increase the corresponding entry in accumulator
//
// parameters:
//  - input_edges: [CV_8UC1] input canny edges
//  - rho: distance resolution of the accumulator in pixels
//  - theta: theta_index resolution of the accumulator in radians
//    (how fast the line in the Hough-space rotates)
//  - num_rho: normalized diagonal of the image
//  - num_angle: number of angles to be tested per pixel
//  - accumulator: [CV_8UC1] output matrix used to store evidence for potential lines
//================================================================================
void algorithms::hough_fill_accumulator(const cv::Mat &input_edges, const int rho, const float theta, const int num_rho,
                                        const int num_angle, cv::Mat &accumulator)
{
    float tab_sin[num_angle], tab_cos[num_angle];
}

//================================================================================
// hough_get_local_maxima()
//--------------------------------------------------------------------------------
// TODO: BONUS:
//  - find the local maxima in accumulator
//  - Condition: maximum in cross window neighborhood, A(theta_index,rho_index) > threshold
//  - sort the local maxima according to the accumulator value
//  - store the first min(total, max_num_lines) lines to the output buffer 'local_maxima'
//
// parameters:
//  - accumulator: [CV_8UC1] input hough-transformed version of our edge image.
//    (to be searched for local maxima/intersections)
//  - threshold: accumulator threshold parameter
//  - max_num_lines: maximum number of local maxima
//  - num_rho: number of rho to be tested per pixel
//  - num_angle: number of angles to be tested per pixel
//  - local_maxima: output vector of local maxima in the accumulator array
//================================================================================
void algorithms::hough_get_local_maxima(const cv::Mat &accumulator, const int threshold, const int max_num_lines,
                                        const int num_rho, const int num_angle, std::vector<LocalMaximum> &local_maxima)
{
    local_maxima.clear();
}

//================================================================================
// hough_convert_to_pts()
//--------------------------------------------------------------------------------
// TODO: BONUS:
//  - convert each local maxima to the hessian normal form
//  - compute two points for each line, according to the assignment sheet
//
// parameters:
//  - local_maxima: input detected lines, vector filled with (rho, theta) pairs
//  - factor: constant for calculation of point coordinates
//  - num_rho: number of radii
//  - rho: rho to be used for calculation
//  - theta: angle to be used for calculation
//  - line_pts: vector with the x/y coordinates for the two points in each entry
//================================================================================
void algorithms::hough_convert_to_pts(const std::vector<LocalMaximum> &local_maxima, const int factor, const int num_rho,
                                      const int rho, const float theta, std::vector<cv::Vec4i> &line_pts)
{
    line_pts.clear();
}
