#include "argParse.h"
#include "CLI11.hpp"



ArgParse::ArgParse()
{
        m_convApp.add_option("-s,--src", m_srcFilePath, "specifies the path of the image that is to be modified.") -> required();
        m_convApp.add_option("-d,--dst", m_dstFilePath, "specifes the path to save the new image, if not used then uses the src path by default. if path doesn't exist creates it.");
}

bool ArgParse::parse(int argc, char* argv[])
{
    argv = m_convApp.ensure_utf8(argv);
    try {
        CLI11_PARSE(m_convApp, argc, argv);
    } 
    catch (const CLI::ParseError &e) {
        return m_convApp.exit(e) == 0;
    }
    return true; 
}

std::string ArgParse::getSrcFilePath() const {return m_srcFilePath;}
std::string ArgParse:: getDstFilePath() const {return m_dstFilePath;}
 




