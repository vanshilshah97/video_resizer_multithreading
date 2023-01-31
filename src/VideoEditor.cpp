#include "VideoEditor.h"

VideoEditor::VideoEditor(const int &resized_width, const int &resized_height, const int &resized_fps, const std::string &curr_video_path, const std::string &desired_video_path) : capture_ptr(std::make_unique<cv::VideoCapture>(curr_video_path)), video_writer_ptr(std::make_unique<cv::VideoWriter>(desired_video_path, cv::VideoWriter::fourcc('m', 'p', '4', 'v'), resized_fps, cv::Size(resized_width, resized_height))), new_width{resized_width}, new_height{resized_height}, new_fps{resized_fps}, num_frames_processed{0}
{

    notified = false;
    //If there is any error in reading video
    if (!capture_ptr->isOpened())
    {
        std::cerr << "Error opening video stream or file" << std::endl;
        return;
    }
}

bool VideoEditor::frame_worker(std::shared_ptr<FrameInfo> ptr_frame_info)
{
    
    while (true)
    {
        
        capture_ptr->read(src_img);
        if (src_img.empty())
        {
            break;
        }

        auto start = std::chrono::high_resolution_clock::now();
        cv::resize(src_img, dst_img, cv::Size(new_width, new_height), cv::InterpolationFlags::INTER_CUBIC);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> duration = end - start;
        std::unique_lock<std::mutex> lock(ptr_frame_info->Lock);
        ptr_frame_info->time_per_frame.push_back(duration.count());
        num_frames_processed++;
        lock.unlock();
        video_writer_ptr->write(dst_img);
    }

    video_writer_ptr->release();
    capture_ptr->release();

    return true;
}

//Printing function
void VideoEditor::print_info(std::shared_ptr<FrameInfo> ptr_frame_info)
{
    std::unique_lock<std::mutex> lock(ptr_frame_info->Lock);
    if (num_frames_processed != 0)
    {
        {   
            // used a predicate until and unless notified that one second is completed this thread will wait here
            if (!notified)
            {
                ptr_frame_info->ConditionVariable.wait(lock);
            }

            float sum = 0.f;
            float size_of_times = ptr_frame_info->time_per_frame.size();
            for (const auto &t : ptr_frame_info->time_per_frame)
            {
                sum += t;
            }
            float mean = sum / size_of_times;
            float std = 0.;
            for (const auto &t : ptr_frame_info->time_per_frame)
            {
                std += (t - mean) * (t - mean);
            }
            std = sqrt(std / size_of_times);
            std::cout << "The number of frames currently processed for that wall clock second are " << size_of_times << std::endl;
            std::cout << "The number of frames currently processed in total are " << num_frames_processed << std::endl;
            std::cout << "Mean of the processed frames: " << mean << ", Standard deviation of the current frames : " << std << std::endl;
            notified = false;

            ptr_frame_info->time_per_frame.clear();
            
        }
    }
    else
    {
        std::cout << "Initial frame still processing" << std::endl;
        notified = false;
    }
}

//Timer function 
void VideoEditor::async_timer(std::shared_ptr<FrameInfo> ptr_frame_info)
{
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::unique_lock<std::mutex> lock(ptr_frame_info->Lock);
    notified = true;
    ptr_frame_info->ConditionVariable.notify_one();
}
