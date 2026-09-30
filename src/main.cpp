#include <exception>
#include <iostream>
#include <cstdint>
#include <ostream>
#include <stdexcept>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "imgOp.h"
#include "CLI11.hpp"
#include "argParse.h"



bool setValues(ArgParse& parser, ImgOp::ImageType& imageDstType);

// ./main src dest
int main(int argc, char* argv[]) {
    
    if (argc < 2) {
        std::cerr << "Invalid number of arguments.\nRun with --help for more information.\n"; 
        return 4; 
    }

     
    ArgParse parser{};
    if(!parser.parse(argc, argv)) return 0;
   
    ImgOp::ImageType imageDstType;
    bool res {setValues(parser, imageDstType)};
    if (!res) throw std::runtime_error("setValues failed!");

    ImgOp img {};     
    img.loadImg(parser.getSrcFilePath());
    img.convGrayScaleFromRgb(ImgOp::Mode::luminance); 
    img.WriteAscii(parser.getDstFilePath());   


    return 0;
}


bool setValues(ArgParse& parser, ImgOp::ImageType& imageDstType)
{
    ArgParse::DstFileType fileType{parser.getDstFileType()};
    switch (fileType)
    {
    case ArgParse::DstFileType::jpg:
        imageDstType = ImgOp::ImageType::jpg; 
        break; 
    
    case ArgParse::DstFileType::png:
        imageDstType = ImgOp::ImageType::png;
        break;
    
    case ArgParse::DstFileType::txt:
        imageDstType = ImgOp::ImageType::txt;
        break;

    default:
        return false;
    }

    return true;
}









