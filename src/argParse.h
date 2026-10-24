#ifndef ARGPARSE_H
#define ARGPARSE_H

#include "CLI11.hpp" 
#include <string_view>

class ArgParse
{
    public:
        enum class DstFileType
        {
            jpg,
            png,
            txt 
        };

        const std::map<std::string, DstFileType> typeMap{
            {"jpg",  DstFileType::jpg},
            {"png",  DstFileType::png},
            {"txt",  DstFileType::txt},
        };

        ArgParse();
        bool parse(int argc, char* argv[]);        
        
        std::string getSrcFilePath() const;
        std::string getDstFilePath() const;
        DstFileType getDstFileType() const; 
        bool getIsEdge() const;
        bool getIsBlendEdgeLumi() const;
        bool getIsGamma() const;
        int getNewWidth() const;
        int getNewHeight() const;
        std::size_t countOp(std::string_view name) const;    

    private:
        CLI::App m_convApp{"App description"};
        std::string m_srcFilePath;
        std::string m_dstFilePath;
        DstFileType m_dstFileType;
        bool m_isEdge {false};
        bool m_isBlendEdgeLumi {false};
        bool m_isGamma {false};
        int m_newWidth {800};
        int m_newHeight {350};


};

#endif


