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




// ./main src dest
int main(int argc, char* argv[]) {
    
    if (argc < 2) {
        std::cerr << "Invalid number of arguments.\nRun with --help for more information.\n"; 
        return 4; 
    }

    CLI::App convApp{"This tool converts 2d images to various formats."};
    argv = convApp.ensure_utf8(argv);

    std::string srcFilePath = "";
    std::string dstFilePath = "";
    
    convApp.add_option("-s,--src", srcFilePath, "specifies the path of the image that is to be modified.");
    convApp.add_option("-d,--dst", dstFilePath, "specifes the path to save the new image, if not used then uses the src path by default. if path doesn't exist creates it.");


    CLI11_PARSE(convApp, argc, argv);
    
    if (convApp.count("--dst") == 0) dstFilePath = srcFilePath;
    
    ImgOp img {};     
    img.loadImg(srcFilePath);
    img.convGrayScaleFromRgb(ImgOp::Mode::rgb); 
    img.saveImg(dstFilePath, ImgOp::ImageType::png);
   


    return 0;
}











