//
// Created by xushen on 10/13/17.
//

#include "ImageStream.h"
#include <opencv2/highgui.hpp>
#include "dirent.h"
#include <iostream>
using namespace std;

ImageStream::ImageStream(const string &folder):imageFolder(folder) {
    setImageFolder(folder);
    allImages.clear();
    currentImagePtr = allImages.begin();
}

ImageStream::ImageStream() {
    imageFolder = "unknown";
    allImages.clear();
    currentImagePtr = allImages.begin();
}

void ImageStream::setImageFolder(const string &folder) {
    allImages.clear();
    if (*(imageFolder.end() - 1) != '/') {
        imageFolder += "/";
    }
    // get all file names
    DIR * pdir;
    struct dirent *entry;
    if ((pdir = opendir(imageFolder.c_str()))) {
        while ((entry = readdir(pdir))) {
            if (strcmp(entry->d_name, ".") != 0
                && strcmp(entry->d_name, "..") != 0
                && strstr(entry->d_name, ".jpg")) {
                allImages.push_back(imageFolder + entry->d_name);
                //cout << "found file:" << *(allImages.end()-1) << endl;
            }
        }
        if ((int) allImages.size() <= 1) {
            cout << "No enough jpg images exist in this folder." << endl;
            exit(-1);
        }
        closedir(pdir);
    } else {
        cout << "Error: incorrect jpg folder path." << endl;
        exit(-1);
    }
}

bool ImageStream::getNextImage(Mat &m) {
    if(!finish){
        m = imread(*currentImagePtr);
        finish = (++currentImagePtr) == allImages.end() != nullptr || false;
        return true;
    } else{
        return false;
    }
}

ImageStream &ImageStream::operator>>(Mat &m) {
    bool flag = getNextImage(m);
    return *this;
}

ImageStream::operator bool() {
    return !finish;
}

ImageStream::ImageStream(const ImageStream & ims) = default;

bool ImageStream::isFinish() const {
    return finish;
}
