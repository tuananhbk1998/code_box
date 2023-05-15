#include "../GetFrame/FrameManager.hpp"
#include<iostream>
int main()
{
    cv::Mat img_fhd,img_4k;
    FrameManager frame;   
    while(1){
        frame.getFrames(img_fhd,img_4k);
        cv::Mat image = img_fhd.clone();   //lay anh full hd tu cam, lay 4k thi dung img_4k
    
        if (image.empty())
        {
            continue;
        }
        // xai gi thi xai o day

    }
    return 0;
}