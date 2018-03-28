/**********************************************************************
 * @file FileUtils.h
 * @author TomatoPi
 * @version 1.0
 *
 * File manipulation utils for the program
 **********************************************************************/
#ifndef DEF_FILE_UTILS_H
#define DEF_FILE_UTILS_H

#include <vector>
#include <string>
#include <dirent.h>

namespace utils {

    /**
     * @brief listFiles, function to list files present at given path
     * @return vector of files founded
     **/
    static std::vector<std::string> listFiles(std::string path) {

        std::vector<std::string> files;
        DIR *rep = opendir(path.c_str());

        if (rep){

            struct dirent *ent;

            while( (ent = readdir(rep)) != NULL ){

                files.push_back(ent->d_name);
            }
        }
        else{

            printf("Error : can't open given directory : \"%s\"\n"
                , path.c_str());
        }
        closedir(rep);

        return files;
    }

    /**
     * @brief isDirectory, function to verify i a given path is a directory
     *  or a file
     * @return true if ths file is a directory
     **/
    static bool isDirectory(std::string path) {

        DIR* dir = opendir(path.c_str());
        return !!dir;
    }

}

#endif
