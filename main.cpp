#include <exception>
#include <iostream>
#include <cstdint>
#include <ostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "imgOp.h"


int main(int argc, char* argv[]) {
    
    if (argc != 2) {
        std::cerr << "only 1 argument must be passed!" << std::endl; 
        return 1; 
    }
    
    ImgOp img {};     
    img.loadImg(argv[1]);
    img.convGrayScaleFromRgb(ImgOp::Mode::rgb); 
    img.saveImg("new_image.jpg", ImgOp::ImageType::jpg);


    return 0;
}
