#include <jni.h>

#include <dirent.h>

#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>

static std::string const LOCAL_PATH("/sdcard/nong");
static std::string const GAME_PATH("/data/data/com.cos8oxx.geometrynong/files");

bool copyFile(
    std::string const& srcPath,
    std::string const& dstPath)
{
    std::ifstream src(srcPath, std::ios::binary);
    std::ofstream dst(dstPath, std::ios::binary);

    dst << src.rdbuf();

    return src && dst;
}

void removeFile(std::string const& path)
{
    system(("rm " + path).c_str());
}

inline std::string getStem(std::string const& path)
{
    std::string filename;
    auto pos = path.rfind('.');

    if (pos != std::string::npos)
        filename = path.substr(0, pos);

    return filename;
}

inline bool isMp3(std::string const& path)
{
    std::string s;
    std::stringstream ss(path);

    while (std::getline(ss, s, '.'));

    return s == "mp3";
}

inline bool validNong(std::string const& path)
{
    if (isMp3(path))
    {
        auto const filename = getStem(path);

        return std::find_if(
            filename.begin(), 
            filename.end(),
            [](char c)
            {
                return !std::isdigit(c);
            }) == filename.end();
    }

    return false;
}

std::vector<std::string> getNongs()
{
    std::vector<std::string> nongList;

    auto dir = opendir(LOCAL_PATH.c_str());

    if (dir)
    {
        auto info = readdir(dir);

        while (info)
        {
            if (validNong(info->d_name))
                nongList.push_back(info->d_name);

            info = readdir(dir);
        }
    }

    closedir(dir);
    return nongList;
}

JNIEXPORT jint JNI_OnLoad(JavaVM*, void*)
{
    std::ofstream log(LOCAL_PATH + "/log.txt");

    auto nongList = getNongs();

    if (nongList.size())
    {
        uint32_t nongCounter = 0;

        for (auto const& path : nongList)
        {
            if (copyFile(
                LOCAL_PATH + '/' + path,
                GAME_PATH + '/' + path))
            {
                ++nongCounter;
            }   
            else
            {
                log << "Failed to copy nong \"" << path << "\"\n";
            }

            removeFile(LOCAL_PATH + '/' + path);
        }

        log << nongCounter << (nongCounter == 1 ? " nong " : " nongs ") << "copied.";
    }
    else
    {
        log << "No nongs found.";
    }

    return JNI_VERSION_1_4;
}