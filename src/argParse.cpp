#include "argParse.h"
#include "CLI11.hpp"

//#define NDEBUG

ArgParse::ArgParse()
{
         
        m_convApp.add_option("-s,--src", m_srcFilePath, "specifies the path of the image that is to be modified.") -> required();
        m_convApp.add_option("-d,--dst", m_dstFilePath, "specifes the path to save the new image, if not used then uses the src path by default. if path doesn't exist creates it.");
        
        m_dstFileType = DstFileType::jpg; 
        m_convApp.add_option("-t,--dstType", m_dstFileType, "specifies the type of the dst file; jpg by default.") 
            -> transform(CLI::CheckedTransformer(typeMap, CLI::ignore_case))
            -> capture_default_str();


}

bool ArgParse::parse(int argc, char* argv[])
{
    argv = m_convApp.ensure_utf8(argv);
    try {
        m_convApp.parse(argc, argv); 
    } 
    catch (const CLI::ParseError &e) {
        m_convApp.exit(e);
        return false; 
    }
    
    if (m_convApp.count("--dst") == 0) m_dstFilePath = m_srcFilePath;

    return true; 
}

std::string ArgParse::getSrcFilePath() const {return m_srcFilePath;}
std::string ArgParse::getDstFilePath() const {return m_dstFilePath;}
ArgParse::DstFileType ArgParse::getDstFileType() const {return m_dstFileType;}
 




