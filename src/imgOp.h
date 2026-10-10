#ifndef IMGOP_H
#define IMGOP_H

#include <cstdint>
#include <iostream>
#include <vector>

class ImgOp
{
private:
    std::string m_filePath; 
    std::vector<std::uint8_t> m_data {}; 
    int m_width {}; // in pixels
    int m_height {}; // in pixels
    
    // 3 = rgb, 1 = luminance
    int m_channels {};
    
    // 70 levels of ramp intensity
    const std::string m_asciiPalette {" .'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$"};
    
    std::uint8_t getLuminanceValue(const std::uint8_t red, const std::uint8_t green, const std::uint8_t blue) const;
    std::uint8_t getAsciiChar(std::uint8_t luminanceValue) const;
    std::vector<uint32_t> initDownSampleTable() const;

public:
    enum class Mode
    {
        rgb,
        luminance
    };
    
    enum class ImageType 
    {
        png,
        jpg,
        txt // for ascii images  
    };



    ImgOp();        
   
    bool loadImg(const std::string& filePath);
    bool convGrayScaleFromRgb(Mode mode); 
    bool saveImg(const std::string& filePath, ImageType imageType);
    void resizeImage(int new_height, int new_width);
   

    std::size_t getImgSize() const; 
    void printInfo() const;
    void printPixel(const int offset) const; 
    void WriteAscii(const std::string& filePath) const;
    void printAsciiCenteredLive() const;

    std::string getFilePath() const {return m_filePath;}
    int getwidth() const {return m_width;}
    int getHeight() const {return m_height;}
    int getChannels() const {return m_channels;}
};



#endif

