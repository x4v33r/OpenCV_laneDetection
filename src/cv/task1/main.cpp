#include <dirent.h>
#include <sys/stat.h>

#include <fstream>
#include <iostream>
#include <vector>

#include "algorithms.h"
#include "opencv2/opencv.hpp"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

// #define FULL_VERSION 1
// #define FINAL_RUN 1
// #define GENERATE_REF 1

#define RST "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"

#define FRED(x) KRED x RST
#define FGRN(x) KGRN x RST

#define BOLD(x) "\x1B[1m" x RST

#if GENERATE_REF || FINAL_RUN
struct reference
{
    cv::Mat *mat;
    std::vector<std::vector<cv::Point2f>> *bins;
    std::vector<cv::Point2f> *lines;
    std::vector<cv::Vec4i> *lines_cartesian;
    std::vector<cv::Point3i> *local_max;

    reference(cv::Mat *m) : mat(m), bins(nullptr), lines(nullptr), lines_cartesian(nullptr), local_max(nullptr) {}
    reference(std::vector<std::vector<cv::Point2f>> *b)
        : mat(nullptr), bins(b), lines(nullptr), lines_cartesian(nullptr), local_max(nullptr)
    {
    }
    reference(std::vector<cv::Point2f> *l)
        : mat(nullptr), bins(nullptr), lines(l), lines_cartesian(nullptr), local_max(nullptr)
    {
    }
    reference(std::vector<cv::Vec4i> *l)
        : mat(nullptr), bins(nullptr), lines(nullptr), lines_cartesian(l), local_max(nullptr)
    {
    }
    reference(std::vector<cv::Point3i> *m)
        : mat(nullptr), bins(nullptr), lines(nullptr), lines_cartesian(nullptr), local_max(m)
    {
    }
};

// helper functions for saving lines with OpenCV
void line_to_vec2f(std::vector<algorithms::Line> &lines, std::vector<cv::Point2f> &points)
{
    for (auto l : lines)
    {
        points.emplace_back(l.intercept, l.slope);
    }
}

void vec2f_to_line(std::vector<algorithms::Line> &lines, std::vector<cv::Point2f> &points)
{
    for (auto p : points)
    {
        algorithms::Line ell;
        ell.intercept = ((int)p.x);
        ell.slope = p.y;
        lines.emplace_back(ell);
    }
}

void local_max_to_vect3i(std::vector<algorithms::LocalMaximum> &local_max, std::vector<cv::Point3i> &points)
{
    for (auto m : local_max)
    {
        points.emplace_back(m.theta_index, m.rho_index, m.accumulator_value);
    }
}

void vect3i_to_local_max(std::vector<algorithms::LocalMaximum> &local_max, std::vector<cv::Point3i> &points)
{
    for (auto p : points)
    {
        algorithms::LocalMaximum lm;
        lm.theta_index = p.x;
        lm.rho_index = p.y;
        lm.accumulator_value = p.z;
        local_max.push_back(lm);
    }
}

#endif

//===============================================================================
// Configuration
//-------------------------------------------------------------------------------
// TODO:
//  - Nothing!
//  - Do not change anything here
//===============================================================================
struct Config
{
    // gaussian blur
    int kernel_size = 0;
    float sigma = 0.f;

    // edge detection
    int edge_threshold_min = 0;
    int edge_threshold_max = 0;

    // bin bins
    float slope_upper_threshold = 0.f;
    float slope_lower_threshold = 0.f;

    // convex mask
    std::vector<cv::Point2i> convex_mask_pts;

    // hough transform
    int rho = 5;
    int threshold = 55;
    int min_line_length = 10;
    int max_line_gap = 5;
    int max_lines = 5;
};

//===============================================================================
// make_directory()
//-------------------------------------------------------------------------------
// TODO:
//  - Nothing!
//  - Do not change anything here
//===============================================================================
void make_directory(const char *path)
{
#if defined(_WIN32)
    _mkdir(path);
#else
    mkdir(path, 0777);
#endif
}

//===============================================================================
// is_path_existing()
//-------------------------------------------------------------------------------
// TODO:
//  - Nothing!
//  - Do not change anything here
//===============================================================================
bool is_path_existing(const char *path)
{
    struct stat buffer
    {
    };
    return (stat(path, &buffer)) == 0;
}

#if GENERATE_REF
//===============================================================================
// generate_ref()
//-------------------------------------------------------------------------------
// TODO:
//  - Nothing!
//  - Do not change anything here
//===============================================================================
void generate_ref(std::string path, reference ref)
{
    cv::FileStorage fs(path, cv::FileStorage::WRITE);
    if (ref.mat != nullptr)
    {
        fs << "image" << *ref.mat;
    }
    else if (ref.bins != nullptr)
    {
        fs << "image" << *ref.bins;
    }
    else if (ref.lines != nullptr)
    {
        fs << "image" << *ref.lines;
    }
    else if (ref.local_max != nullptr)
    {
        fs << "image" << *ref.local_max;
    }
}
#endif

#if FINAL_RUN
//===============================================================================
// get_ref_image()
//-------------------------------------------------------------------------------
// TODO:
//  - Nothing!
//  - Do not change anything here
//===============================================================================
void get_ref_image(std::string ref_directory, std::string name, reference ref)
{
    struct dirent *entry;
    DIR *dir = opendir((ref_directory).c_str());
    while ((entry = readdir(dir)) != NULL)
    {
        std::string entry_name = entry->d_name;
        if (entry_name.find(name) != std::string::npos)
        {
            std::string full_path = ref_directory + name;
            cv::FileStorage fs(full_path, cv::FileStorage::READ);
            if (ref.mat != nullptr)
            {
                fs["image"] >> *ref.mat;
            }
            else if (ref.bins != nullptr)
            {
                fs["image"] >> *ref.bins;
            }
            else if (ref.lines != nullptr)
            {
                fs["image"] >> *ref.lines;
            }
            else if (ref.local_max != nullptr)
            {
                fs["image"] >> *ref.local_max;
            }
            break;
        }
    }
    closedir(dir);
}
#endif

//===============================================================================
// save_image()
//-------------------------------------------------------------------------------
// TODO:
//  - Nothing!
//  - Do not change anything here
//===============================================================================
void save_image(const std::string &out_directory, const std::string &name, size_t number, const cv::Mat &image)
{
    std::stringstream number_stringstream;
    number_stringstream << std::setfill('0') << std::setw(2) << number;
    std::string path = out_directory + number_stringstream.str() + "_" + name + ".png";
    cv::imwrite(path, image);
    std::cout << "saving image: " << path << std::endl;
}

//===============================================================================
// run()
//-------------------------------------------------------------------------------
// TODO:
//  - Nothing!
//  - Do not change anything here
//===============================================================================
void run(const cv::Mat &input_image, const std::string &out_directory, const std::string &ref_directory, Config config)
{
    size_t image_counter = 0;
    //=============================================================================
    // Grayscale image
    //=============================================================================
    std::cout << "Step 1 - calculating grayscale image... " << std::endl;
    cv::Mat grayscale = cv::Mat::zeros(input_image.size(), CV_8UC1);
    algorithms::compute_grayscale(input_image, grayscale);
    save_image(out_directory, "grayscale", ++image_counter, grayscale);
#if GENERATE_REF
    generate_ref(ref_directory + "01_grayscale.json", reference{&grayscale});
#endif
    std::cout << "Step 2 - gaussian blur... " << std::endl;
#if FINAL_RUN
    get_ref_image(ref_directory, "01_grayscale.json", reference{&grayscale});
#endif
    cv::Mat blurred_image = cv::Mat::zeros(input_image.size(), CV_8UC1);
    algorithms::gaussian_blur(grayscale, blurred_image, config.kernel_size, config.sigma);
    save_image(out_directory, "blurred_image", ++image_counter, blurred_image);
#if GENERATE_REF
    generate_ref(ref_directory + "02_blurred_image.json", reference{&blurred_image});
#endif
    // TODO: fix all generate ref/final run references
    // everything is "documented" in the CV 2022 repo (how this is done, refs are generated on the server)

    //=============================================================================
    // Gradient image
    //=============================================================================
    std::cout << "Step 3 - calculating gradient image... " << std::endl;
    cv::Mat gradient_x = cv::Mat::zeros(input_image.size(), CV_32FC1);
    cv::Mat gradient_y = cv::Mat::zeros(input_image.size(), CV_32FC1);
    cv::Mat gradient_abs = cv::Mat::zeros(input_image.size(), CV_32FC1);
#if FINAL_RUN
    get_ref_image(ref_directory, "02_blurred_image.json", reference{&blurred_image});
#endif
    algorithms::compute_gradient(blurred_image, gradient_x, gradient_y, gradient_abs);
    save_image(out_directory, "gradient_x", ++image_counter, gradient_x);
    save_image(out_directory, "gradient_y", ++image_counter, gradient_y);
    save_image(out_directory, "gradient_abs", ++image_counter, gradient_abs);
#if GENERATE_REF
    generate_ref(ref_directory + "03_gradient_x.json", reference{&gradient_x});
    generate_ref(ref_directory + "04_gradient_y.json", reference{&gradient_y});
    generate_ref(ref_directory + "05_gradient_abs.json", reference{&gradient_abs});
#endif
    //=============================================================================
    // Own Canny
    //=============================================================================
    std::cout << "Step 4 - calculate canny canny_edges... " << std::endl;
    cv::Mat non_maxima = cv::Mat::zeros(input_image.size(), CV_32FC1);
#if FINAL_RUN
    get_ref_image(ref_directory, "03_gradient_x.json", reference{&gradient_x});
    get_ref_image(ref_directory, "04_gradient_y.json", reference{&gradient_y});
    get_ref_image(ref_directory, "05_gradient_abs.json", reference{&gradient_abs});
#endif
    algorithms::non_maxima_suppression(gradient_abs, gradient_x, gradient_y, non_maxima);
    save_image(out_directory, "non_maxima", ++image_counter, non_maxima);
    non_maxima.convertTo(non_maxima, CV_8UC1);
#if GENERATE_REF
    generate_ref(ref_directory + "06_non_maxima.json", reference{&non_maxima});
#endif

    cv::Mat hysteresis = cv::Mat::zeros(input_image.size(), CV_8UC1);
#if FINAL_RUN
    get_ref_image(ref_directory, "06_non_maxima.json", reference{&non_maxima});
#endif
    algorithms::hysteresis(non_maxima, config.edge_threshold_min, config.edge_threshold_max, hysteresis);
    save_image(out_directory, "hysteresis", ++image_counter, hysteresis);
#if GENERATE_REF
    generate_ref(ref_directory + "07_hysteresis.json", reference{&hysteresis});
#endif
    cv::Mat canny_edges = cv::Mat::zeros(input_image.size(), CV_8UC1);
    algorithms::canny_edges(blurred_image, config.edge_threshold_min, config.edge_threshold_max, canny_edges);
    save_image(out_directory, "canny", ++image_counter, canny_edges);
#if GENERATE_REF
    generate_ref(ref_directory + "08_canny.json", reference{&canny_edges});
#endif
    //=============================================================================
    // Masking
    //=============================================================================
    std::cout << "Step 5 - masking... " << std::endl;
    cv::Mat triangular_mask = cv::Mat::zeros(input_image.size(), CV_8UC1);
    cv::Mat applied_mask = cv::Mat::zeros(input_image.size(), CV_8UC1);
#if FINAL_RUN
    get_ref_image(ref_directory, "08_canny.json", reference{&canny_edges});
#endif
    algorithms::apply_convex_mask(canny_edges, config.convex_mask_pts, triangular_mask, applied_mask);
    save_image(out_directory, "convex_mask", ++image_counter, triangular_mask);
    save_image(out_directory, "applied_mask", ++image_counter, applied_mask);
#if GENERATE_REF
    generate_ref(ref_directory + "10_applied_mask.json", reference{&applied_mask});
#endif
    //=============================================================================
    // Hough Transform
    //=============================================================================
    std::cout << "Step 6 - Hough bins... " << std::endl;
    // get hough_lines: houghP (cartesian) and hough (polar)
    std::vector<cv::Vec4i> hough_lines;
    cv::Mat output_lines_hough, output_lines_hough_drawing;
    input_image.copyTo(output_lines_hough_drawing);
#if FINAL_RUN
    get_ref_image(ref_directory, "10_applied_mask.json", reference{&applied_mask});
#endif
    cv::HoughLinesP(applied_mask, hough_lines, config.rho, CV_PI / 180, config.threshold, config.min_line_length,
                    config.max_line_gap);
    // visualize bins
    for (auto &hough_line : hough_lines)
    {
        cv::line(output_lines_hough_drawing, cv::Point(hough_line[0], hough_line[1]),
                 cv::Point(hough_line[2], hough_line[3]), cv::Scalar(0, 0, 255), 3, cv::LINE_AA);
    }
    cv::addWeighted(output_lines_hough_drawing, 0.5, input_image, 0.5, 0., output_lines_hough);
    save_image(out_directory, "hough_lines", ++image_counter, output_lines_hough);

    //=============================================================================
    // Average/Bin Lines
    //=============================================================================
    std::cout << "Step 7 - process bins... " << std::endl;
    // average hough_lines, and bin based on slope
    std::vector<std::vector<algorithms::Line>> out_params;
    algorithms::bin_lines(input_image, hough_lines, config.slope_upper_threshold, config.slope_lower_threshold,
                          out_params);

#if GENERATE_REF
    std::vector<std::vector<cv::Point2f>> out_params_f;
    for (auto i : out_params)
    {
        std::vector<cv::Point2f> test;
        line_to_vec2f(i, test);
        out_params_f.emplace_back(test);
    }
    generate_ref(ref_directory + "12_bin_lines.json", reference{&out_params_f});
#endif
    cv::Mat output_lines_bin, output_lines_bin_drawing;
    input_image.copyTo(output_lines_bin_drawing);

    std::vector<cv::Scalar> color = {cv::Scalar(255, 0, 0), cv::Scalar(0, 255, 0), cv::Scalar(0, 0, 255)};
    for (int i = 0; i < out_params.size(); i++)
    {
        for (auto l : out_params.at(i))
        {
            cv::Point2i x0((input_image.rows / 2.f - l.intercept) / l.slope, input_image.rows / 2);
            cv::Point2i x1((-l.intercept) / l.slope, input_image.rows);
            cv::line(output_lines_bin_drawing, x0, x1, color.at(i), 3, cv::LINE_AA);
        }
    }
    cv::addWeighted(output_lines_bin_drawing, 0.5, input_image, 0.5, 0., output_lines_bin);
    save_image(out_directory, "bin_lines", ++image_counter, output_lines_bin);

    // average hough_lines
    std::vector<algorithms::Line> averaged_lines;
    cv::Mat output_lines_averaged, output_lines_averaged_drawing;
    input_image.copyTo(output_lines_averaged_drawing);
#if FINAL_RUN
    out_params.clear();
    std::vector<std::vector<cv::Point2f>> out_params_f;
    get_ref_image(ref_directory, "12_bin_lines.json", reference{&out_params_f});
    for (auto i : out_params_f)
    {
        std::vector<algorithms::Line> test;
        vec2f_to_line(test, i);
        out_params.emplace_back(test);
    }
#endif
    algorithms::average_lines(out_params, averaged_lines);
#if GENERATE_REF
    std::vector<cv::Point2f> averaged_lines_f;
    line_to_vec2f(averaged_lines, averaged_lines_f);
    generate_ref(ref_directory + "13_averaged_lines_lines.json", reference{&averaged_lines_f});
#endif
    // just to visualize the slopes better
    for (int i = 0; i < averaged_lines.size(); i++)
    {
        auto l = averaged_lines.at(i);
        cv::Point2i x0((input_image.rows / 2.f - l.intercept) / l.slope, input_image.rows / 2);
        cv::Point2i x1((-l.intercept) / l.slope, input_image.rows);
        cv::line(output_lines_averaged_drawing, x0, x1, color.at(i), 3, cv::LINE_AA);
    }
    cv::addWeighted(output_lines_averaged_drawing, 0.5, input_image, 0.5, 0., output_lines_averaged);
    save_image(out_directory, "averaged_lines", ++image_counter, output_lines_averaged);
    //=============================================================================
    // Compute Intersection in projective space
    //=============================================================================
    std::cout << "Step 8 - compute intersection... " << std::endl;
    // compute line intersection between the first two bins
#if FINAL_RUN
    averaged_lines.clear();
    std::vector<cv::Point2f> averaged_lines_f;
    get_ref_image(ref_directory, "13_averaged_lines_lines.json", reference{&averaged_lines_f});
    vec2f_to_line(averaged_lines, averaged_lines_f);
#endif
    // visualize average hough_lines
    cv::Mat output_image, output_image_drawing;
    input_image.copyTo(output_image_drawing);

    // only do this if the student code works
    if (averaged_lines.size() == 3)
    {
        cv::Point2i pt1 = algorithms::compute_intersection(averaged_lines.at(0), averaged_lines.at(1), 100);
        cv::Point2i pt3 = algorithms::compute_intersection(averaged_lines.at(1), averaged_lines.at(2), 100);
        // transform to the cv coordinate system
        pt1.y = input_image.rows - pt1.y;
        pt3.y = input_image.rows - pt3.y;

        cv::Point2i pt2 = pt1.y < pt3.y ? pt1 : pt3;
        std::vector<cv::Point2i> pts{pt1, pt2, pt3};

        for (int i = 0; i < averaged_lines.size(); i++)
        {
            cv::line(output_image_drawing,
                     cv::Point2i((-averaged_lines.at(i).intercept) / averaged_lines.at(i).slope, input_image.rows),
                     pts[i], color.at(i), 3, cv::LINE_AA);
        }
        cv::addWeighted(output_image_drawing, 0.5, input_image, 0.5, 0., output_image);
    }
    else
    {
        input_image.copyTo(output_image);
    }
    save_image(out_directory, "intersected_lines", ++image_counter, output_image);

    //=============================================================================
    // BONUS
    //=============================================================================
    // =============================================================================
    // Hough Lines Own Implementation
    //=============================================================================
    std::cout << "Bonus - Hough transform... " << std::endl;

    int width = input_image.size().width;
    int height = input_image.size().height;

    // number of angles we have to test per point in the image
    int num_angles = 180;
    // equals the normalized diagonal of the image. Normalized by using the distance resolution of the accumulator
    int num_rho = ceil(((width + height) * 2 + 1) / config.rho);
    cv::Mat accumulator = cv::Mat::zeros(cv::Size(num_angles, num_rho), CV_32SC1);

    algorithms::hough_fill_accumulator(applied_mask, config.rho, CV_PI / 180, num_rho, num_angles, accumulator);

    // save accumulator
    save_image(out_directory + "bonus/", "bonus_accumulator", ++image_counter, accumulator);

#if GENERATE_REF
    generate_ref(ref_directory + "bonus_accumulator.json", reference{&accumulator});
#endif

    std::vector<algorithms::LocalMaximum> local_maxima;
#if FINAL_RUN
    get_ref_image(ref_directory, "bonus_accumulator.json", reference{&accumulator});
#endif

    std::vector<cv::Vec4i> hough_lines_own_pts;
    std::vector<cv::Vec2f> hough_lines_own;
    cv::Mat output_lines_hough_own, output_lines_hough_drawing_own;
    input_image.copyTo(output_lines_hough_drawing_own);

    algorithms::hough_get_local_maxima(accumulator, config.threshold, config.max_lines, num_rho, num_angles,
                                       local_maxima);
#if GENERATE_REF
    std::vector<cv::Point3i> local_max_pts;
    local_max_to_vect3i(local_maxima, local_max_pts);
    generate_ref(ref_directory + "bonus_localmaxima", reference{&local_max_pts});
#endif

    cv::Mat output_maxima = cv::Mat::zeros(accumulator.size(), CV_8UC1);
    for (auto lm : local_maxima)
    {
        int accum = std::max(0, std::min(lm.accumulator_value, 255));
        cv::circle(output_maxima, cv::Point2i(lm.theta_index, lm.rho_index), accum, cv::Scalar(accum), 4, cv::FILLED);
    }
    // save accumulator
    save_image(out_directory + "bonus/", "bonus_localmaxima", ++image_counter, output_maxima);
#if FINAL_RUN
    local_maxima.clear();
    std::vector<cv::Point3i> local_max_pts;
    get_ref_image(ref_directory, "bonus_localmaxima", reference{&local_max_pts});
    vect3i_to_local_max(local_maxima, local_max_pts);
#endif
    algorithms::hough_convert_to_pts(local_maxima, 1000, num_rho, config.rho, CV_PI / 180, hough_lines_own_pts);

    for (auto &hough_line : hough_lines_own_pts)  // change to hough_lines
    {
        cv::line(output_lines_hough_drawing_own, cv::Point(hough_line[0], hough_line[1]),
                 cv::Point(hough_line[2], hough_line[3]), cv::Scalar(0, 0, 255), 3, cv::LINE_AA);
    }
    cv::addWeighted(output_lines_hough_drawing_own, 0.5, input_image, 0.5, 0., output_lines_hough_own);
    save_image(out_directory + "bonus/", "bonus_hough_lines", ++image_counter, output_lines_hough_own);

#if GENERATE_REF
    generate_ref(ref_directory + "bonus_hough_lines.json", reference{&output_lines_hough_own});
#endif
    // bin the lines
    std::vector<std::vector<algorithms::Line>> out_params_bonus;
#if FINAL_RUN
    get_ref_image(ref_directory, "bonus_hough_lines.json", reference{&hough_lines_own_pts});
#endif
    algorithms::bin_lines(input_image, hough_lines_own_pts, config.slope_upper_threshold, config.slope_lower_threshold,
                          out_params_bonus);
#if GENERATE_REF
    std::vector<std::vector<cv::Point2f>> out_params_bonus_f;
    for (auto i : out_params_bonus)
    {
        std::vector<cv::Point2f> test;
        line_to_vec2f(i, test);
        out_params_bonus_f.emplace_back(test);
    }
    generate_ref(ref_directory + "bonus_bin_lines.json", reference{&out_params_bonus_f});
#endif
    // average the lines
    std::vector<algorithms::Line> averaged_lines_bonus;
#if FINAL_RUN
    out_params_bonus.clear();
    std::vector<std::vector<cv::Point2f>> out_params_bonus_f;
    get_ref_image(ref_directory, "bonus_bin_lines.json", reference{&out_params_bonus_f});
    for (auto i : out_params_bonus_f)
    {
        std::vector<algorithms::Line> test;
        vec2f_to_line(test, i);
        out_params_bonus.emplace_back(test);
    }
#endif
    algorithms::average_lines(out_params_bonus, averaged_lines_bonus);

    cv::Mat output_image_bonus, output_image_drawing_bonus;
    input_image.copyTo(output_image_drawing_bonus);

    // only do this if the student code works
    if (averaged_lines_bonus.size() == 3)
    {
        cv::Point2i pt1 = algorithms::compute_intersection(averaged_lines_bonus.at(0), averaged_lines_bonus.at(1), 100);
        cv::Point2i pt3 = algorithms::compute_intersection(averaged_lines_bonus.at(1), averaged_lines_bonus.at(2), 100);
        // transform to the cv coordinate system
        pt1.y = input_image.rows - pt1.y;
        pt3.y = input_image.rows - pt3.y;

        cv::Point2i pt2 = pt1.y < pt3.y ? pt1 : pt3;
        std::vector<cv::Point2i> pts{pt1, pt2, pt3};

        for (int i = 0; i < averaged_lines_bonus.size(); i++)
        {
            cv::line(output_image_drawing_bonus,
                     cv::Point2i((-averaged_lines_bonus.at(i).intercept) / averaged_lines_bonus.at(i).slope,
                                 input_image.rows),
                     pts[i], color.at(i), 3, cv::LINE_AA);
        }
        cv::addWeighted(output_image_drawing_bonus, 0.5, input_image, 0.5, 0., output_image_bonus);
    }
    else
    {
        input_image.copyTo(output_image_bonus);
    }

    save_image(out_directory + "bonus/", "bonus_intersected_lines", ++image_counter, output_image_bonus);
}

//===============================================================================
// execute_testcase()
//-------------------------------------------------------------------------------
// TODO:
//  - Nothing!
//  - Do not change anything here
//===============================================================================
void execute_testcase(const rapidjson::Value &config_data)
{
    //=============================================================================
    // Parse input data
    //=============================================================================
    std::string name = config_data["name"].GetString();
    std::string image_path = config_data["image_path"].GetString();

    Config config;

    // gaussian blur
    config.sigma = (float)config_data["sigma"].GetDouble();
    config.kernel_size = (int)config_data["kernel_size"].GetUint();

    // edge detection
    config.edge_threshold_min = (int)config_data["edge_threshold_min"].GetUint();
    config.edge_threshold_max = (int)config_data["edge_threshold_max"].GetUint();

    // bin bins
    config.slope_upper_threshold = (float)config_data["slope_upper_threshold"].GetDouble();
    config.slope_lower_threshold = (float)config_data["slope_lower_threshold"].GetDouble();

    // triangular mask
    const rapidjson::Value &triangular_mask = config_data["triangular_mask"];
    assert(triangular_mask.IsArray());
    for (rapidjson::SizeType i = 0; i < triangular_mask.Size(); i++)
    {
        config.convex_mask_pts.emplace_back(triangular_mask[i][0].GetInt(), triangular_mask[i][1].GetInt());
    }

    // hough transform
    config.rho = (int)config_data["rho"].GetUint();
    config.threshold = (int)config_data["threshold"].GetUint();
    config.min_line_length = (int)config_data["min_line_length"].GetUint();
    config.max_line_gap = (int)config_data["max_line_gap"].GetUint();
    config.max_lines = (int)config_data["max_lines"].GetUint();

    //=============================================================================
    // Load input images
    //=============================================================================
    std::cout << BOLD(FGRN("[INFO]")) << " Input image: " << image_path << std::endl;

    cv::Mat img = cv::imread(image_path);

    if (!img.data)
    {
        std::cout << BOLD(FRED("[ERROR]")) << " Could not load image (" << name + ".png"
                  << ")" << std::endl;
        throw std::runtime_error("Could not load file");
    }

    //=============================================================================
    // Create output directory
    //=============================================================================
    std::string output_directory = "output/" + name + "/";

    std::cout << BOLD(FGRN("[INFO]")) << " Output path: " << output_directory << std::endl;

    make_directory("output/");
    make_directory(output_directory.c_str());
    // create bonus directory
    make_directory((output_directory + "/bonus/").c_str());

    std::string ref_path = "data/intm/";
    std::string ref_directory = ref_path + name + "/";

#if FINAL_RUN
    if (!is_path_existing(ref_directory.c_str()))
    {
        std::cout << BOLD(FRED("[ERROR]")) << " ref directory does not exist!" << std::endl;
        std::cout << BOLD(FGRN("[INFO]")) << " execute with GENERATE_REF 1 first" << std::endl;
        throw std::runtime_error("Could not load ref files");
    }
    else
    {
        std::cout << "opening ref directory" << std::endl;
    }
#endif

#if GENERATE_REF
    make_directory(ref_path.c_str());
    make_directory(ref_directory.c_str());
#endif

    //=============================================================================
    // Starting default task
    //=============================================================================
    std::cout << "Starting MAIN Task..." << std::endl;
    run(img, output_directory, ref_directory, config);
}

//===============================================================================
// main()
//-------------------------------------------------------------------------------
// TODO:
//  - Nothing!
//  - Do not change anything here
//===============================================================================
int main(int argc, char *argv[])
{
    std::cout << "CV/task1 framework version 1.0" << std::endl;  // DO NOT REMOVE THIS LINE!!!
    std::cout << "===================================" << std::endl;
    std::cout << "               CV Task 1           " << std::endl;
    std::cout << "===================================" << std::endl;

    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <config-file>" << std::endl;
        return 1;
    }

    std::string path = std::string(argv[1]);
    std::ifstream fs(path);
    if (!fs)
    {
        std::cout << "Error: Failed to open file " << path << std::endl;
        return 2;
    }
    std::stringstream buffer;
    buffer << fs.rdbuf();

    rapidjson::Document doc;
    rapidjson::ParseResult check;
    check = doc.Parse<0>(buffer.str().c_str());

    if (check)
    {
        if (doc.HasMember("testcases"))
        {
            rapidjson::Value &testcases = doc["testcases"];
            for (rapidjson::SizeType i = 0; i < testcases.Size(); i++)
            {
                rapidjson::Value &testcase = testcases[i];
                try
                {
                    execute_testcase(testcase);
                }
                catch (const std::exception &e)
                {
                    std::cout << e.what() << std::endl;
                    std::cout << BOLD(FRED("[ERROR]")) << " Program exited with errors!" << std::endl;
                    return -1;
                }
            }
        }
        std::cout << "Program exited normally!" << std::endl;
    }
    else
    {
        std::cout << "Error: Failed to parse file " << argv[1] << ":" << check.Offset() << std::endl;
        return 3;
    }
    return 0;
}
