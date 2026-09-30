#ifndef ARGPARSE_H
#define ARGPARSE_H

#include "CLI11.hpp" 

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

    private:
        CLI::App m_convApp{"App description"};
        std::string m_srcFilePath;
        std::string m_dstFilePath;
        DstFileType m_dstFileType;

};

#endif


