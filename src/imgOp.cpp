#include <cstddef>
#include <cstdint>
#include <cassert>
#include <cmath>
#include <ios>
#include <iostream>
#include <iterator>
#include <fstream>
#include <filesystem>
#include <numeric>
#include <ostream>
#include <stdexcept>
#include <utility>
#include <vector>
#include "imgOp.h"
#include "stb_image.h"
#include "stb_image_write.h"

//#define NDEBUG

namespace fs = std::filesystem;


ImgOp::ImgOp()
{
}


// this function would only work when m_channels == 3 and the datasize of the image is a multipler of 3;
// mode specifies how the resulting vector would be represented; rgb, luminance
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
   
    // loop to set values for easier usage.
    for (int i = 0; i < m_dataSize; i += 3)      
    {
        lumiVal = getLuminanceValue(m_data[i], m_data[i+1], m_data[i+2]); 
        m_data[i] = lumiVal;
        m_data[i+1] = lumiVal;
        m_data[i+2] = lumiVal;
    }
    if (mode == Mode::rgb) return true;   

    std::vector<uint8_t> temp;      
    temp.reserve(m_dataSize);      
    // mode has to be luminance here 
    for (int i = 0; i < m_dataSize; i += 3)
    {
        temp.push_back(m_data[i]); 
    }
   
    m_data = std::move(temp);
    m_channels = 1;

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
    
    else if (imageType == ImageType::txt)
    {
        WriteAscii(filePath);     
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



std::vector<uint32_t> ImgOp::initDownSampleTable() const      
{
    #ifndef NDEBUG
    assert(m_channels == 1 && "m_channels must be 1; this function uses 1 byte per pixel");
    #endif
    if (m_channels != 1) throw std::runtime_error("m_channels is not set to luminance.");

    std::size_t originalDataSize {static_cast<std::size_t>(m_width) * m_height * m_channels};
    std::vector<uint32_t> table;
    std::size_t expected_size = static_cast<std::size_t>(m_width + 1) * (m_height + 1);

    // adding a 0 column and a 0 row; should be 1 byte per pixel
    table.resize((m_width+1) * (m_height+1) * m_channels, 0); 

    uint32_t currTableCell{}; 
    for (int dataY = 0; dataY < m_height; dataY++) 
    {
        for (int dataX = 0; dataX < m_width; dataX++)     
        {
            uint32_t currDataPixel = m_data[dataY * m_width + dataX]; 
            
            int tableX = dataX + 1;
            int tableY = dataY + 1;

            int aboveTable = (tableY-1) * (m_width+1) + tableX; 
            int leftTable = tableY * (m_width+1) + (tableX-1); 
            int topLeftTable = (tableY-1) * (m_width+1) + (tableX-1); 
           

            #ifndef NDEBUG
            assert(aboveTable >= 0 && static_cast<std::size_t>(aboveTable) < expected_size);
            assert(leftTable >= 0 && static_cast<std::size_t>(leftTable) < expected_size);
            assert(topLeftTable >= 0 && static_cast<std::size_t>(topLeftTable) < expected_size);
            #endif

            uint32_t aboveTablePixel = table[aboveTable];
            uint32_t leftTablePixel = table[leftTable];
            uint32_t topLeftTablePixel = table[topLeftTable];

            table[tableY * (m_width+1) + tableX] = currDataPixel + aboveTablePixel + leftTablePixel - topLeftTablePixel; 

        } 

    } 
    return table;
}




void ImgOp::resizeImage(int new_height, int new_width)
{
    #ifndef NDEBUG
    assert(m_channels == 1 && "this function only supports 1 byte per pixel"); 
    #endif
    if (m_channels != 1) throw std::runtime_error("m_channels not set to luminance");
    
    // m_channels should be 1 anyway for now
    std::size_t originalDataSize {static_cast<std::size_t>(m_width) * m_height * m_channels}; 
    std::size_t newDataSize {static_cast<std::size_t>(new_width) * new_height * m_channels};

    std::vector<uint32_t> table {initDownSampleTable()};
    std::vector<uint8_t> tmp; 
    tmp.resize(newDataSize);

    uint8_t currLumi {};
    
    int sourceX1 {};
    int sourceY1 {};
    int sourceX2 {};
    int sourceY2 {};
    for (int y = 0; y < new_height; y++) 
    {
        for (int x = 0; x < new_width; x++)
        {
            sourceX1 = (x * m_width) / new_width; 
            sourceY1 = (y * m_height) / new_height; 
            sourceX2 = ((x+1) * m_width) / new_width; 
            sourceY2 = ((y+1) * m_height) / new_height; 

            int table_stride = m_width + 1;
            uint32_t sumTopLeft = table[sourceY1 * table_stride + sourceX1]; // Top-Left
            uint32_t sumTopRight = table[sourceY1 * table_stride + sourceX2]; // Top-Right
            uint32_t sumBottomLeft = table[sourceY2 * table_stride + sourceX1]; // Bottom-Left
            uint32_t sumBottomRight = table[sourceY2 * table_stride + sourceX2]; // Bottom-Right

            uint32_t sumRec = sumBottomRight - sumBottomLeft - sumTopRight + sumTopLeft;
            currLumi = static_cast<uint8_t>(sumRec / ((sourceX2 - sourceX1) * (sourceY2 - sourceY1))); 
            tmp[y * new_width + x] = currLumi;
        } 
    }

    m_data = std::move(tmp);
    m_height = new_height;
    m_width = new_width;

}



bool ImgOp::areValidIndexAndParts(int Index, int yPart, int xPart) const
{
    int dataSize = m_data.size();
    return (Index >= 0 && Index < m_data.size() && yPart >= 0 && xPart >= 0 && xPart < m_width && yPart < m_height);
}


int ImgOp::sumConvultedPixel(const std::vector<int32_t> &kernelTable, int kernelTableWidth, int x, int y) const
{
    #ifndef NDEBUG
    assert(kernelTableWidth > 0 && "kernelTableWidth must be positive!");
    assert(x >= 0 && "x cannot be negetive!"); 
    assert(y >= 0 && "y cannot be negetive!"); 
    #endif

    if (kernelTableWidth <= 0) throw std::runtime_error("width must be positive"); 
    if (x < 0) throw std::runtime_error("x cannot be negetive"); 
    if (y < 0) throw std::runtime_error("y cannot be negetive"); 
    

    size_t kernelTableLength = kernelTable.size(); 
    int kernelTableHeight = kernelTableLength / kernelTableWidth;
    int sumKernelTable {std::accumulate(kernelTable.begin(), kernelTable.end(), 0)};
    std::vector<uint8_t> currPixelNeighboors(kernelTableLength);
    size_t currPixelNeightboorsSize {currPixelNeighboors.size()};
    uint8_t finalResult {};

    // both offsets start from topLeftIndex
    int yOffset = -1;
    int yPartCalc {}; 

    int xOffset = -1;
    int xPartCalc {}; 
    
    bool areValid {false}; 
    
    uint32_t currRes {};
    double sumMultiplied {0}; 
    for (int i = 0; i < currPixelNeightboorsSize; i++)
    {
        yPartCalc = y+yOffset; 
        xPartCalc = x+xOffset; 
        int currIndex = yPartCalc * m_width + xPartCalc; 
        
        areValid = areValidIndexAndParts(currIndex, yPartCalc, xPartCalc);
        if (areValid) currPixelNeighboors[i] = m_data[currIndex];
        // else currPixelNeighboors[i] should already be 0; 
         
        currRes = currPixelNeighboors[i] * kernelTable[i];
        sumMultiplied += static_cast<int>(currRes); 

        xOffset++;
        if (xOffset > 1) 
        {
            xOffset = -1;
            yOffset++; 
        }
    }

    return sumMultiplied;

}














// kernelTable is a 1d vector that is treated as a 2d vector.
// returns a new convulted Image
std::vector<uint8_t> ImgOp::kernelConvult(ConvultionModes mode) const
{
    int currPixelIndex{}; 
    uint8_t currConvultedPixel {};
    std::vector<uint8_t> convultedImage(m_data.size());
    
    double sumMultiplied {0}; 

    if (mode == ConvultionModes::gaussian) 
    {   
        int sumKernelTable = std::accumulate(m_guassianBlurKernelTable.begin(), m_guassianBlurKernelTable.end(), 0);
        for (int y = 0; y < m_height; y++)
        {
            for (int x = 0; x < m_width; x++)
            {
                sumMultiplied = sumConvultedPixel(m_guassianBlurKernelTable, m_guassianBlurKernelTableWidth, x ,y); 
                currConvultedPixel = static_cast<uint8_t>(round(sumMultiplied / sumKernelTable));
                currPixelIndex = y * m_width + x;
                convultedImage.at(currPixelIndex) = currConvultedPixel; 
            }
        }
    } 

    else if (mode == ConvultionModes::edgeDetection) 
    { 
        std::vector<int32_t> gradientImage {};
        int32_t maxGradient {getMaxGradientsVectorInit(gradientImage)};
        int32_t threshold = 50; 

        int32_t currGradient {};
        for (int y = 0; y < m_height; y++)
            for (int x = 0; x < m_width; x++)
            {
                currGradient = gradientImage[y * m_width + x]; 
                if 
                    (maxGradient == 0) convultedImage[y * m_width + x] = 0;
                else 
                    convultedImage[y * m_width + x] = static_cast<uint8_t>(std::round((static_cast<double>(currGradient) / maxGradient) * 255));
            }
    }
    return convultedImage;
}

// this function is for kernelConvult internal use only
// returns the maximum Gradient and inits the gradients vector parameter
int32_t ImgOp::getMaxGradientsVectorInit(std::vector<int32_t> &gradientVector) const
{
    gradientVector.resize(m_width * m_height); 

    int32_t maxGradient {0};
    int xGradient {0};
    int yGradient {0};
    int32_t currGradient {0};

    for (int y = 0; y < m_height; y++)
        for (int x = 0; x < m_width; x++)
        {
            xGradient = sumConvultedPixel(m_edgeDetectionKernelTableX, m_edgeDetectionKernelTableWidthX, x, y); 
            yGradient = sumConvultedPixel(m_edgeDetectionKernelTableY, m_edgeDetectionKernelTableWidthY, x, y); 
            currGradient = std::hypot(xGradient, yGradient);
            gradientVector[y * m_width + x] = currGradient;
            maxGradient = currGradient > maxGradient ? currGradient : maxGradient;
        } 

    return maxGradient;
}





void ImgOp::edgeDetection()
{
    std::vector<uint8_t> convultedImage = kernelConvult(ConvultionModes::edgeDetection); 
    m_data = std::move(convultedImage);
}





void ImgOp::gaussianBlur()
{
    std::vector<uint8_t> convultedImage = kernelConvult(ConvultionModes::gaussian); 
    m_data = std::move(convultedImage);
}



std::uint8_t ImgOp::getLuminanceValue(const std::uint8_t red, const std::uint8_t green, const std::uint8_t blue) const
{
    return static_cast<std::uint8_t>(0.2126 * static_cast<double>(red) + 0.7152 * static_cast<double>(green) + 0.0722 * static_cast<double>(blue));
}


std::size_t ImgOp::getImgSize() const {return static_cast<std::size_t>(getHeight() * getwidth() * getChannels());}


std::uint8_t ImgOp::getAsciiChar(std::uint8_t luminanceValue) const
{
    std::size_t index {};
    index = (static_cast<size_t>(luminanceValue) * (m_asciiPalette.length() - 1)) / 255;
    return m_asciiPalette[index];
}


// only runs when m_channels = 1; luminance
void ImgOp::WriteAscii(const std::string& filePath) const
{
    
    std::ofstream resFile{filePath};
    #ifndef NDEBUG
    assert(m_channels == 1 && "m_channels must be 1(luminance) to print the image as Ascii");
    assert(resFile.is_open() && "couldn't open file."); 
    #endif
    if (m_channels != 1) throw std::runtime_error("m_channels is not 1");
    if (!resFile.is_open()) throw std::runtime_error("Error: failed to open file.");
    

    int canvas_width = 2546; 
    int canvas_height = 425; 
    std::size_t dataSize = m_data.size();
    std::size_t index{};
    std::uint8_t currChar {}; 
    std::uint8_t currLumiValue {}; 
    std::string rowBuffer {};
    rowBuffer.reserve(m_width * 2); // multipled by 2 because we push 2 chars each time(for round shapes)

   
    int y_padding = (canvas_height > m_height) ? (canvas_height - m_height) / 2 : 0;
    int x_padding = (canvas_width > (m_width * 2)) ? (canvas_width - (m_width * 2)) / 2 : 0;

    std::string indent_spaces(x_padding, ' ');
    
    for (int i = 0; i < y_padding; ++i) 
        resFile << '\n';
    
    for (int row = 0; row < m_height; row++)
    {
        rowBuffer.clear(); 
        for (int col = 0; col < m_width; col++)
        {
            // formula to print a 1d array as 2d; 
            index = row * m_width + col;
            currLumiValue = m_data[index];
            currChar = getAsciiChar(currLumiValue);
        
            // pushing twice for round shapes accuracy 
            rowBuffer.push_back(currChar);
            rowBuffer.push_back(currChar);
        }
        resFile << indent_spaces << rowBuffer << '\n';

    }
    resFile.close();
}




void ImgOp::printAsciiCenteredLive() const 
{
    // Define your terminal bounds
    int term_rows = 425;
    int term_cols = 2546;

    // Calculate padding
    int y_padding = (term_rows > m_height) ? (term_rows - m_height) / 2 : 0;
    int x_padding = (term_cols > (m_width * 2)) ? (term_cols - (m_width * 2)) / 2 : 0;
    std::string indent_spaces(x_padding, ' ');

    // 1. Clear screen and reset cursor to top-left
    std::cout << "\033[2J\033[H";

    // 2. Print empty newlines to push the moon down into the vertical center
    for (int i = 0; i < y_padding; ++i) {
        std::cout << '\n';
    }

    // 3. Stream each row down naturally with horizontal indentation
    for (int row = 0; row < m_height; ++row) 
    {
        std::cout << indent_spaces;
        for (int col = 0; col < m_width; ++col) 
        {
            std::uint8_t currChar = getAsciiChar(m_data[row * m_width + col]);
            std::cout << currChar << currChar; // Doubled for aspect ratio
        }
        std::cout << '\n';
    }
    std::cout << std::flush;
}











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







