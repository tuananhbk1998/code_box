/*
    Module: Object Tracking
    Author: Le Manh Thang
    Created: August 26th, 2021
*/

#ifndef ObjectTracker_hpp
#define ObjectTracker_hpp
#include <iostream>
#include <fstream>
#include <iomanip> // to format image names using setw() and setfill()
//#include <io.h>    // to check file existence using POSIX function access(). On Linux 
#include <unistd.h>
#include <set>
#include <vector>
#include <deque>
#include <memory>
#include <array>
#include "Hungarian.h"
#include "KalmanTracker.h"
#include "TrackerManager.hpp"
using namespace std;
using namespace cv;




class ObjectTracker
{
private:
    /* data */
	double iouThreshold = 0.3;
public:
    ObjectTracker(/* args */);
    ~ObjectTracker();
    vector<std::unique_ptr<TrackerManager>> trackers;
    void update(vector<ObjectTrace> detected);
    void update_scale(vector<ObjectTrace> detected, float scale);
    void update_scale_wh_anpr_VN(vector<ObjectTrace> detected, float scale_w_rect, float scale_h_rect, float scale_w_square, float scale_h_square);
    vector<TrackingTrace> getTracks();
};


#endif