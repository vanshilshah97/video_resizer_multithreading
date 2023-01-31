
#ifndef VIDEOEDITOR_H
#define VIDEOEDITOR_H

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <chrono>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <future>
#include <memory>

struct FrameInfo
{

    std::vector<float> time_per_frame;
    std::mutex Lock;
    std::condition_variable ConditionVariable;
};

class VideoEditor
{

public:
    VideoEditor(const int &resized_width, const int &resized_height, const int &resized_fps, const std::string &curr_video_path, const std::string &desired_video_path);
    bool frame_worker(std::shared_ptr<FrameInfo> ptr_frame_info);
    void print_info(std::shared_ptr<FrameInfo> ptr_frame_info);
    void async_timer(std::shared_ptr<FrameInfo> ptr_frame_info);

private:
    int new_width;
    int new_height;
    int new_fps;
    int num_frames_processed;
    bool notified;
    cv::Mat src_img;
    cv::Mat dst_img;
    std::unique_ptr<cv::VideoCapture> capture_ptr;
    std::unique_ptr<cv::VideoWriter> video_writer_ptr;
};

#endif // VIDEOEDITOR_H