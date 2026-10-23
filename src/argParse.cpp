#include "argParse.h"
#include "CLI11.hpp"

//#define NDEBUG

ArgParse::ArgParse()
{
         
        m_convApp.add_option("-s,--src", m_srcFilePath, "specifies the path of the image that is to be modified.") -> required();
        m_convApp.add_option("-d,--dst", m_dstFilePath, "specifes the path to save the new image, if not used then uses the src path by default. if path doesn't exist creates it.");
        
        m_convApp.add_flag("--edge", m_isEdge, "when present will use edge detection.");
        m_convApp.add_flag("--edgeLumi", m_isBlendEdgeLumi, "when present will blend edge detection and guassian blur; different from --edge");
        m_convApp.add_flag("--gamma", m_isGamma, "when present will use gamma modification");


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
bool ArgParse::getIsEdge() const {return m_isEdge;} 
bool ArgParse::getIsBlendEdgeLumi() const {return m_isBlendEdgeLumi;} 
bool ArgParse::getIsGamma() const {return m_isGamma;} 




