//
// Created by yawaflua on 15/11/2024.
//

#ifndef CHECK_FILE_EXISTS_H
#define CHECK_FILE_EXISTS_H


inline bool file_exists (const std::string& name) {
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
}

#endif //CHECK_FILE_EXISTS_H
