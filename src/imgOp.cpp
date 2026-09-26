#include <fstream>
#include <iostream>
#include <cstdint>
#include <iterator>
#include <ostream>
#include <cassert>
#include <stdexcept>
#include <filesystem>
#include "imgOp.h"
#include "stb_image.h"
#include "stb_image_write.h"

//#define NDEBUG

namespace fs = std::filesystem;


ImgOp::ImgOp()
{
}


// this function would only work when m_channels == 3 and the datasize of the image is a multipler of 3;
bool ImgOp::convGrayScaleFromRgb(Mode mode)
{

    std::size_t m_dataSize {getImgSize()};
    #ifndef NDEBUG 
    assert(m_channels == 3 && "convGrayScaleFromRgb cannot be called when m_channels is not 3(rgb)."); 
    assert(m_dataSize % 3 == 0 && "datasize must be a multiplier of 3 in rgb mode.");
    #endif

    if (m_channels != 3) throw std::runtime_error("convGrayScaleFromRgb cannot be called when m_channels is not 3(rgb).");
    if (m_dataSize % 3 != 0) throw std::runtime_error("datasize must be a mulitpler of 3 in rgb mode.");
    
    std::uint8_t lumiVal{};
    for (int i = 0; i < m_dataSize; i += 3)      
    {
        lumiVal = getLuminanceValue(m_data[i], m_data[i+1], m_data[i+2]); 
        m_data[i] = lumiVal;
        m_data[i+1] = lumiVal;
        m_data[i+2] = lumiVal;
    }
    return true;
}




bool ImgOp::saveImg(const std::string& filePath, ImageType imageType)
{
    #ifndef NDEBUG
    assert(!m_data.empty() && "m_data is empty, cannot save empty image."); 
    #endif

    if (m_data.empty()) throw std::runtime_error("Error: cannot save an empty image.");

    fs::path path = filePath;
    if (!fs::exists(path)) {
        std::ofstream newFile(path);
        if (!newFile.is_open()) throw std::runtime_error("Error: couldn't create file: '" + filePath + "'");
        else newFile.close(); 
    }

    int result{};
    if (imageType == ImageType::png) {
        int strideInBytes {m_width * m_channels};
        result = stbi_write_png(filePath.c_str(), m_width, m_height, m_channels, m_data.data(), strideInBytes);
    }
    
    else if (imageType == ImageType::jpg) {
        int quality = 90; 
        result = stbi_write_jpg(filePath.c_str(), m_width, m_height, m_channels, m_data.data(), quality);
    }
    
    else throw std::runtime_error("Invalid image type!");

    #ifndef NDEBUG
    assert(result && "Error failed to write image to disk.");
    #endif
    
    if (!result) throw std::runtime_error("failed to write image to disk."); 
    return true;
}



bool ImgOp::loadImg(const std::string& filePath)
{
    std::uint8_t* data = stbi_load(filePath.c_str(), &m_width, &m_height, &m_channels, 0);

    #ifndef NDEBUG
    if (!data) std::cerr << "Error: Failed to load Image\n" << stbi_failure_reason() << "\n"; 
    assert(data && "Failed to load Image.");   
    #endif
     
    #ifdef NDEBUG 
    if (!data) 
        throw std::runtime_error((std::string("Failed to load image: ") + stbi_failure_reason()).c_str());
    #endif 

    m_filePath = filePath; 
    m_data.assign(data, data + (getImgSize()));
    stbi_image_free(data);

    std::cout << "Image loaded!\n";    
    return true;
}




std::uint8_t ImgOp::getLuminanceValue(const std::uint8_t red, const std::uint8_t green, const std::uint8_t blue) const
{
    return static_cast<std::uint8_t>(0.2126 * static_cast<double>(red) + 0.7152 * static_cast<double>(green) + 0.0722 * static_cast<double>(blue));
}



std::size_t ImgOp::getImgSize() const {return static_cast<std::size_t>(getHeight() * getwidth() * getChannels());}


void ImgOp::printInfo() const
{
    std::cout << "file path: " << getFilePath() <<"\n";
    std::cout << "Dimensions: " << getwidth() << "x" << getHeight() << "\n";
    std::cout << "Original channels: " << getChannels() << "\n";
}



// this function prints the rgb values of a pixel at offset, returns void
// will not care if an index that is not the start of pixel is passed
void ImgOp::printPixel(const int offset) const
{
    #ifndef NDEBUG
    assert(offset >= 0 && "offset must be either 0 or positive!");     
    assert(offset <= getImgSize() - 3 && "offset must be be smaller then start index of last pixel!");     
    #endif
    
    #ifdef NDEBUG
    if (offset < 0) throw std::runtime_error("Error: offset cannot be negetive."); 
    if (offset > getImgSize() - 3) throw std::runtime_error("Error: offset out of range");  
    #endif
    
    std::cout << "pixel: " << static_cast<int>(m_data[offset]) << ", " << static_cast<int>(m_data[offset+1]) << ", " << static_cast<int>(m_data[offset+2]) << std::endl;




}







