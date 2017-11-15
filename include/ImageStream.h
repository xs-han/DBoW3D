//
// Created by xushen on 10/13/17.
//

#ifndef VIRTUALVIEW_IMAGESTREAM_H
#define VIRTUALVIEW_IMAGESTREAM_H

#include <opencv2/core.hpp>
using namespace cv;
using namespace std;

class ImageStream {
    string imageFolder;
    vector<string> allImages;
    auto currentImagePtr;
    bool finish;
public:
    bool isFinish() const;

public:
    ImageStream();

    explicit ImageStream(const string & folder);

    explicit ImageStream(const ImageStream & ims);

    void setImageFolder(const string & folder);

    bool getNextImage(Mat & m);

    virtual ImageStream& operator >> (Mat & m);

    virtual explicit operator bool();

};


#endif //VIRTUALVIEW_IMAGESTREAM_H
