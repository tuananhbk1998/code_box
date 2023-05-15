/******************************************************************************** 
 Copyright (C) 2020, PHAM VAN HIEU. All rights reserved.
 Module: Bkav_Ai 2.0
 Author: HieuPV
 Modify: DangTT
 Created: 21/12/2020
 Description: 
********************************************************************************/
#pragma once
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <memory>
// #include "DateTime.hpp"
// #include "json.h"
#include "opencv2/opencv.hpp"
#include <map>
// #include "ScheduleTime/ScheduleTime.hpp"

#define TAG "BkavAICore"
#define HTTP_SERVER_EVENT           "http://0.0.0.0:4003/event"
#define HTTP_SERVER_VIDEO_RECORD    "http://0.0.0.0:4003/video"
#define HTTP_SERVER_BACKUP          "http://0.0.0.0:4003/api/backup"


#ifndef LENGTH_MAX_PATH
    #define LENGTH_MAX_PATH         100
#endif

#define MAX_LENGTH_TIME             50

#ifndef MAX_IMAGE_EVENT
    #define MAX_IMAGE_EVENT         600
#endif
#ifndef MAX_VIDEO_EVENT
    #define MAX_VIDEO_EVENT         50
#endif
#ifndef MAX_TEMP_INDEX_FRAME
    #define MAX_TEMP_INDEX_FRAME    MAX_IMAGE_EVENT //anti duplicate frame event
#endif
#ifdef DEBUG
    #include <android/log.h>
#endif
#ifndef PRINT_LOG
    #ifdef DEBUG
        #define PRINT_LOG(_TAG, ...) do{__android_log_print(ANDROID_LOG_INFO, _TAG,__VA_ARGS__);} while(0)
    #else
        #define PRINT_LOG(_TAG, ...) do {} while (0)
    #endif
#endif

/* Storage_parameter */
#ifndef STORAGE_PARAMETER
    #define INTERNAL         "internal"
    #define SD_CARD          "sd_card"
    #define SD               "480P"
    #define HD               "720P"
    #define FULLHD           "1080P"
    #define LOW_R              "Low"
    #define MEDIUM_R           "Medium"
    #define HIGH_R             "High"  
    #define PATH_INTERNAL    "/data/media"
    #define PATH_SDCARD      "/mnt/media_rw"
    #define STORAGE_IMAGE_SDCARD   "/storage/sdcard1/data/bkavai/images"
    #define STORAGE_IMAGE_INTERNAL "/data/bkavai/images"
    #define STORAGE_VIDEO_SDCARD   "/storage/sdcard1/data/bkavai/video"
    #define STORAGE_VIDEO_INTERNAL "/data/bkavai/video"
    
#endif
/* ---------------- */

/* Element Event */
#ifndef Element_Event
    #define TYPE            "type"
    #define TAGS            "tags"
    #define EXTRAS          "extras"
    #define UUID            "uuid"
    #define MONITOR_UUID    "monitor_uuid"
    #define TIME            "time"
    #define PATH_IMAGE      "pathImage"
    #define PATH_CROP_IMAGE "path_cr_Image"
    #define LOCATIONS       "locations"
    #define V_ID            "id"
    #define IMAGE_ID        "image_id"
    #define CROP_ID         "crop_id"
    #define VIDEO_ID        "video_id"
    #define NAME            "name"
    #define ZONE_S          "zone"
    #define OBJECTS         "objects"
    #define CAM_ID          "cam_id"
    #define BASE64          "base64"
#endif
#ifndef Anpr
    #define LICENSE    "license"
    #define CONFIDENCE "confidence"
#endif
#ifndef Face_Recognize
    #define EMBEDDINGS    "embeddings"
    #define PROFILEID     "profileId"
    #define DUPLICATE_ID  "duplicate_id"
#endif
#ifndef Backpack
    #define OBJECTID      "objectId"
#endif
#ifndef Fire_detect
    #define GPSLOCATION      "gpsLocation"
    #define FIREDISTANCING   "fireDistances"
    #define FIREANGLES       "fireAngles"
#endif
#ifndef Fask_Mask
    #define FMASK         "fmask"
#endif
#ifndef People_Feature
    #define FEATURED      "featured"
#endif
#ifndef Social_Distancing
    #define DISTANCING    "distancing"
#endif
#ifndef recrod_video
    #define START_TIME      "start_time"
    #define END_TIME        "end_time"
    #define PATHVIDEO       "pathVideo"
    #define VIDEO           "video"
#endif
/* -------------- */

/*Params*/
// struct ParamsAI_t
// {
//     std::vector<std::string> alarm_object;
//     std::string alarm_direction;
//     int limited_time;
//     int alarm_amount;
//     std::string type_params;

// };

/*Server config*/
// struct Notify_server
// {
//     bool use = false;
//     uint32_t sleep = 0;
// };
// struct Server_config
// {
//     std::string url_data;
//     std::string url_media;
//     std::string access_token;
//     Notify_server notify_server;
//     bool use = false;
//     bool backup = false;
// };

/* Capture_config */
struct Capture_config
{

    std::string storage     = "internal";
    std::string fomart      = "*.jpg";
    std::string resolution  = "1080P";
    std::string quality     = "Medium";
    std::string PATH_STORAGE = "/data/bkavai/images";
    bool enable             = true;
};

/* Video_Config */
struct Video_config
{
    std::string format = "*.jpg";
    std::string record_profile = "480P";
    std::string storage = "internal";
    std::string PATH_STORAGE_VIDEO = "/data/bkavai/video";
    uint8_t recording_frame = 10;
    uint8_t post_event_duration = 1;
    uint8_t pre_event_duration = 1;
    bool enable;
};

/* Element Input */
enum Element_Event {
    INPUT,
    OUTPUT
};
/*Ai Element*/
enum AiElement_t {
    BACK_PACK = 1,
    CARRY_ON_SUIT_CASE,
    GUN_DETECT,
    LAPTOP_DETECT,
    BRIEFCASE_DETECT,
    PERSON_COUNT,
    FACE_MASK,
    SOCIAL_DISTANCING,
    FIRE_DETECT,
    CAR_DETECT,
    ANPR_AI,
    COW_DETECT,
    VEHICLE,
    FACE_DETECT, 
    FACE_RECOGNIZE,
    PEOPLE_FEATURE,
    PERSON_COUNT_DSP,
    INTRUSION,
    OPPOSE, // đi ngược chiều ?
    WRONGLANEVIOLATION, //lấn làn
    REDLIGHTVIOLATION,
    VEHICLE_FEATURE, // 28
    UNKNOWN // viet cac bai AI phia tren UNKNOWN
};
using V_ZONE = std::vector<std::vector<cv::Point> >;
using ZONE   = std::vector<cv::Point>;
using ZONE_F = std::vector<cv::Point2f>;
using LINE   = std::vector<cv::Point>;
/**/
typedef struct 
{
    V_ZONE regions;
    std::vector<std::string> v_name_region;
    std::vector<std::string> v_direction;
    // std::vector<std::string> v_object_alarm; //human, car ...
    std::vector<int> v_id;
    // std::vector<int> v_limit_person; // số lượng tối đa của từng object
    std::vector<int> v_preset_id;

} configRegions_t;

typedef struct
{
    std::string url_data;
    std::string url_media;
    std::string access_token;
    struct {
        bool        use;
        uint64_t    sleep;
    } notify_server;
    bool use; // false
    bool backup; // false

} configServer_t;

typedef struct
{
    std::string addr_server;
    std::string id;
    std::string password;
    std::string recipent;
    std::string sender;
    std::string subject;
    std::string message;
    uint32_t    port;
    bool        authentication;
    bool        TLS;
    bool        use;

} configEmail_t;

typedef struct
{
    std::string addr_server;
    std::string id;
    std::string password;
    std::string upload_directory;
    uint32_t    port;
    bool        passive_mode;
    bool        use;
} configFTP_t;

/*Traffic*/
typedef struct
{
    int                         id;
    std::string                 name;
    ZONE                        regions;
    std::vector<std::string>    rules_object;
    LINE                        directions;
    // std::map<std::string, std::vector<cv::Point>> data;

} trafficOppose_t; //  Oppose | đi ngược chiều

typedef struct {
    std::string                 name;
    V_ZONE                      RedLight;
    V_ZONE                      YellowLight;
    V_ZONE                      GreenLight;
    std::vector<std::string>    rules_lights;
} lights_t;

typedef struct {
    LINE start;
    LINE end;
    LINE left;
    LINE right;
} crossroads_t;

typedef struct {
    int                         id;
    std::string                 name;
    ZONE                        regions;
    std::vector<std::string>    rules_direction;
    std::vector<std::string>    rules_object;
    LINE                        directions;
} lanes_t;

typedef struct
{
    std::vector<lights_t>       lights;
    std::vector<crossroads_t>   crossRoads;
    std::vector<lanes_t>        lanes;
    
} trafficBlowRedLight_t;

typedef struct
{
    std::vector<lanes_t>            lanes;
    std::vector<std::vector<int> >  allows;
} trafficWrongLane_t;

typedef struct
{
    trafficOppose_t         oppose;
    trafficBlowRedLight_t   blowRedLight;
    trafficWrongLane_t      wrongLane;
} configTraffic_t;

/*Major*/
typedef struct
{
    configRegions_t zones;
    struct {
        std::vector<std::string>    objects_alert;
        uint32_t                    amount_object;
        uint32_t                     time_notify;
        uint32_t                     time_repeat_notify;

    } rules;

} configIntrusion_t; // intrusion

typedef struct
{
    configIntrusion_t intrusion;

} configMajor_t;

typedef struct
{
    bool license;

} configVehicleFeature_t;
//
namespace config
{
    typedef struct
    {
        std::vector<std::string>    nation;
        int                         width;
        int                         height;
        configRegions_t             zones;
        configTraffic_t             traffic;
        configMajor_t               majorAI;
        configVehicleFeature_t      vehicleFeature;
        
    } Params;

    typedef struct
    {
        configServer_t  server;
        configEmail_t   email;
        configFTP_t     ftp;
        int             cam_id;
        bool            alarm_output;
        bool            base64;
        
        struct {
            bool image_crop;
            bool image_paronama;

        } event_type;

    } Extras;

    typedef struct
    {
        std::string             uuid;
        std::string             type;
        json                    tags;
        bool                    enable;
        bool                    record;
        uint64_t                sleep;
        config::ScheduleTime    schedule_time;

    } Basic;
    
};

using TIME_STAMP = unsigned long long;
json buff_metadata;
