#ifndef IMGOP_H
#define IMGOP_H

#include <cstdint>
#include <iostream>
#include <vector>

class ImgOp
{
private:
    
    enum class ConvultionModes 
    {
        gaussian,
        edgeDetection 
    };


    std::string m_filePath; 
    std::vector<std::uint8_t> m_data {}; 
    int m_width {}; // in pixels
    int m_height {}; // in pixels
    // 3 = rgb, 1 = luminance
    int m_channels {};
    // 70 levels of ramp intensity
    const std::string m_asciiPalette {" .'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$"};
    
    const int m_guassianBlurKernelTableWidth {3};
    std::vector<int32_t> m_guassianBlurKernelTable {
        1, 2, 1,
        2, 4, 2,
        1, 2, 1
    };

    //NOTE: X AXIS 
    const int m_edgeDetectionKernelTableWidthX {3};
    std::vector<int32_t> m_edgeDetectionKernelTableX {
        -1, 0, 1,
        -2, 0, 2,
        -1, 0, 1
    };
    //NOTE: Y AXIS 
    const int m_edgeDetectionKernelTableWidthY {3};
    std::vector<int32_t> m_edgeDetectionKernelTableY {
         -1, -2, -1,
         0, 0, 0,
         1, 2, 1
    };
 


    std::uint8_t getLuminanceValue(const std::uint8_t red, const std::uint8_t green, const std::uint8_t blue) const;
    std::uint8_t getAsciiChar(std::uint8_t luminanceValue) const;
    std::vector<uint32_t> initDownSampleTable() const;
    std::vector<uint8_t> kernelConvult(ConvultionModes mode) const;
    int sumConvultedPixel(const std::vector<int32_t> &kernelTable, int kernelTableWidth, int x, int y) const;
    bool areValidIndexAndParts(int Index, int yPart, int xPart) const;
    int getMaxGradientsVectorInit(std::vector<int> &gradientVector) const;

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
    void gaussianBlur();  
    void edgeDetection(); 

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

