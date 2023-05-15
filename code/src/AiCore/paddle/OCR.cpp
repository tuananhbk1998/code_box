#include "OCR.hpp"

OCR::OCR()
{
    PaddleOCR::OCRConfig config("/media/tund/Disk1/2.MyProject/Paddle/PaddleOCR/deploy/cpp_infer/tools/config.txt");

    this->det = DBDetector(config.det_model_dir, config.use_gpu, config.gpu_id,
                config.gpu_mem, config.cpu_math_library_num_threads,
                config.use_mkldnn, config.max_side_len, config.det_db_thresh,
                config.det_db_box_thresh, config.det_db_unclip_ratio,
                config.use_polygon_score, config.visualize,
                config.use_tensorrt, config.use_fp16);
    this->rec = CRNNRecognizer(config.rec_model_dir, config.use_gpu, config.gpu_id,
                        config.gpu_mem, config.cpu_math_library_num_threads,
                        config.use_mkldnn, config.char_list_file,
                        config.use_tensorrt, config.use_fp16);
    this->cls = nullptr;
}
OCR::~OCR()
{

}



int OCR::RunOCR(cv::Mat frame, std::string& out )
{
    std::vector<std::vector<std::vector<int>>> boxes;

    this->det.Run(frame, boxes);
    out = this->rec.Run(boxes,frame, this->cls);

}