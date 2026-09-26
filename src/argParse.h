#ifndef ARGPARSE_H
#define ARGPARSE_H

#include "CLI11.hpp" 

class ArgParse
{
    private:
        CLI::App m_convApp{"App description"};
        std::string m_srcFilePath;
        std::string m_dstFilePath;
 

    public:
        ArgParse();
        bool parse(int argc, char* argv[]);        
        
        std::string getSrcFilePath() const;
        std::string getDstFilePath() const;
};

#endif


