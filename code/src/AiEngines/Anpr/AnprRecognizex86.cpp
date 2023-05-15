#include"AnprRecognizex86.hpp"

AnprRecognizerx86::AnprRecognizerx86()
{
    this->detector = new AnprDetector();
    this->ocr = OCR();
    this->detector->init(Nations::VN);
}
AnprRecognizerx86::~AnprRecognizerx86()
{

}
int AnprRecognizerx86::recognize(cv::Mat& img, std::vector<PlateInfor>& plates)
{
    plates.clear();
    if(!img.empty())
    {
        try
        {
             // detect
            std::vector<ObjectTrace> objPlates;
            this->detector->detect(img, objPlates);
            std::cout << "Detected object: " << objPlates.size() << endl;
            // adding size of plate
            for(int i = 0; i < objPlates.size(); i++)
            {
                
                // objPlates[i].rect.x -=  objPlates[i].rect.width * 0.1;
                // objPlates[i].rect.x = objPlates[i].rect.x >= 0 ? objPlates[i].rect.x : 0;

                // objPlates[i].rect.y -=  objPlates[i].rect.height * 0.1;
                // objPlates[i].rect.y = objPlates[i].rect.y >= 0 ? objPlates[i].rect.y : 0;

                // objPlates[i].rect.width +=  objPlates[i].rect.width * 0.3;
                // objPlates[i].rect.width = (objPlates[i].rect.x + objPlates[i].rect.width) > img.cols ? (img.cols - objPlates[i].rect.x) : objPlates[i].rect.width;

                // objPlates[i].rect.height +=  objPlates[i].rect.height * 0.3;
                // objPlates[i].rect.height = (objPlates[i].rect.y + objPlates[i].rect.height) > img.rows ? (img.rows - objPlates[i].rect.y) : objPlates[i].rect.height;
                
                cv::Mat imgPlate = img(objPlates[i].rect).clone();                       
                PlateInfor plate;
                plate.imgPlate = imgPlate;
                plate.rect = objPlates[i].rect;
                this->ocr.RunOCR(plate.imgPlate, plate.license);
                plates.push_back(plate);
            }
            
            

    
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
     }
    else
    {
        LOG_FAIL("Execute Anpr recognizer failed, please check your input");
        return STATUS_FAILED; 
    }
    return STATUS_SUCCESS;
}
int AnprRecognizerx86::only_recognize(cv::Mat& img, std::vector<PlateInfor>& plates)
{
    plates.clear();
    if(!img.empty())
    {
        try
        {
            // adding size of plate

            
            // objPlates[i].rect.x -=  objPlates[i].rect.width * 0.1;
            // objPlates[i].rect.x = objPlates[i].rect.x >= 0 ? objPlates[i].rect.x : 0;

            // objPlates[i].rect.y -=  objPlates[i].rect.height * 0.1;
            // objPlates[i].rect.y = objPlates[i].rect.y >= 0 ? objPlates[i].rect.y : 0;

            // objPlates[i].rect.width +=  objPlates[i].rect.width * 0.3;
            // objPlates[i].rect.width = (objPlates[i].rect.x + objPlates[i].rect.width) > img.cols ? (img.cols - objPlates[i].rect.x) : objPlates[i].rect.width;

            // objPlates[i].rect.height +=  objPlates[i].rect.height * 0.3;
            // objPlates[i].rect.height = (objPlates[i].rect.y + objPlates[i].rect.height) > img.rows ? (img.rows - objPlates[i].rect.y) : objPlates[i].rect.height;
            
            PlateInfor plate;
            plate.imgPlate = img;
            // plate.rect = objPlates[i].rect;
            this->ocr.RunOCR(plate.imgPlate, plate.license);
            plates.push_back(plate);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
     }
    else
    {
        LOG_FAIL("Execute Anpr recognizer failed, please check your input");
        return STATUS_FAILED; 
    }
    return STATUS_SUCCESS;
}
int AnprRecognizerx86::recognize_traffic(cv::Mat mFHDImg, cv::Mat mUHDImg, ObjectTrace &objects,PlateInfor &plate)
{
    // phong to anh bien so
    objects.rect.x -=  objects.rect.width * 0.1;
    objects.rect.x = objects.rect.x >= 0 ? objects.rect.x : 0;

    objects.rect.y -=  objects.rect.height * 0.1;
    objects.rect.y = objects.rect.y >= 0 ? objects.rect.y : 0;

    objects.rect.width +=  objects.rect.width * 0.3;
    objects.rect.width = (objects.rect.x + objects.rect.width) > mFHDImg.cols
    ?
    (mFHDImg.cols - objects.rect.x) : objects.rect.width;

    objects.rect.height +=  objects.rect.height * 0.3;
    objects.rect.height = (objects.rect.y + objects.rect.height) > mFHDImg.rows
    ?
    (mFHDImg.rows - objects.rect.y) : objects.rect.height;

    int cols_UHD = mUHDImg.cols;
    int rows_UHD = mUHDImg.rows;
    
    // doc bien so
    cv::Mat imgPlate = mUHDImg(objects.rect).clone();
    float confidence = 0;
    // PlateInfor plate;
    if(isValidPlate(imgPlate))
    {
        this->ocr.RunOCR(imgPlate, plate.license);
    }
    else
    {
        plate.license = "Unknown";
        plate.score = 0;
    }
    return 0;
}
int its(cv::Mat mFHDImg, cv::Mat mUHDImg, ITS_Output outputs)
{
    
}
bool AnprRecognizerx86::isValidPlate(cv::Mat& img)
{
    if(img.rows * img.cols >= 1500) 
        return true;
    return false;
}

int AnprRecognizerx86::detect_traffic(cv::Mat& img,std::vector<ObjectTrace>& objs) // 4K
{
    objs.clear();
    this->detector->detect(img,objs);
    return 0;
}

