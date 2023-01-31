#include "VideoEditor.h"
#include <thread>
#include <cmath>
/* Good luck! */

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "Welcome to the Invisible AI takehome! Check out " << __FILE__ << " to get started.\n";
        return -1;
    }

    //Desired properties for the new video
    int desired_width = 640;
    int desired_height = 480;
    int desired_fps = 50;
    std::string desired_video_path = "small_example.mp4";
    
    //Custom smart pointers for developed functionalities
    std::shared_ptr<VideoEditor> editor_ptr(new VideoEditor(desired_width, desired_height, desired_fps, argv[1], desired_video_path));
    std::shared_ptr<FrameInfo> frame_info_ptr(new FrameInfo());
    
    //Thread which manages the frame grabbing,resizing and writing
    std::future<bool> bool_completed_thread = std::async(&VideoEditor::frame_worker, editor_ptr, frame_info_ptr);

    while (true)
    {
        //Thread which manages the printing to the terminal
        std::thread printer_thread(&VideoEditor::print_info, editor_ptr, frame_info_ptr);
        //Thread which maintains the timer of one second and notifies the printer thread when its completed
        std::thread timer_thread(&VideoEditor::async_timer, editor_ptr, frame_info_ptr);
        printer_thread.join();
        timer_thread.join();
        if (bool_completed_thread.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
        {
            std::cout << "Program is terminated" << std::endl;
            break;
        }
    }
    return 0;
}
