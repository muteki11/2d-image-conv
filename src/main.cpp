#include <exception>
#include <iostream>
#include <cstdint>
#include <ostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "imgOp.h"
#include "CLI11.hpp"
#include "argParse.h"



// ./main src dest
int main(int argc, char* argv[]) {
    
    if (argc < 2) {
        std::cerr << "Invalid number of arguments.\nRun with --help for more information.\n"; 
        return 4; 
    }

     
    ArgParse parser{};
    parser.parse(argc, argv);

    //if (convApp.count("--dst") == 0) dstFilePath = srcFilePath;
   

    ImgOp img {};     
    img.loadImg(parser.getSrcFilePath());
    img.convGrayScaleFromRgb(ImgOp::Mode::rgb); 
    img.saveImg(parser.getDstFilePath(), ImgOp::ImageType::png);
   


    return 0;
}











