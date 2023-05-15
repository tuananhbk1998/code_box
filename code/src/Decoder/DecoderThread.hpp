/*
    Module: DecoderThread.cpp
    Author: LE MANH THANG
    Created: Dec 21th, 2021
*/
#ifndef DecoderThread_hpp
#define DecoderThread_hpp

#include "Decoder.hpp"
#include "FrameManager.hpp"
#include <pthread.h>
#include <signal.h>

#define EXPECTED_FPS 12
// #define LINK_VIDEO "../videos/traffic.mp4"
#define LINK_VIDEO "/media/tund/Data/VideoCam/cam5.mp4"
class DecoderThread
{
private:
    /* data */
    Decoder* m_decoder;
    FrameManager* m_frameManager = nullptr;
    pthread_mutex_t m_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_t m_thread;
    static void* threadFunc(void* args);
    void process();
public:
    DecoderThread(FrameManager* frameManger);
    ~DecoderThread();
    void run();
    void stop();
    // void process();
};



#endif
