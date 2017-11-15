//
// Created by xushen on 10/12/17.
//

#include <opencv2/core.hpp>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
using namespace std;
using namespace cv;

void getCameraParams(string calibFile, Mat& cameraMartix, Mat & distortionCoefficients){
    FileStorage fs(calibFile, FileStorage::READ);
    if (!fs.isOpened())
    {
        cout << "Could not open the calibration file: \"" << calibFile << "\"" << endl;
        exit(-1);
    }
    fs["Camera_Matrix"] >> cameraMartix;
    fs["Distortion_Coefficients"] >> distortionCoefficients;
}

void calcKeyFrames(int noctaves, int npoints, double factor, string d)
{
    Mat refFrame = imread(inputDir + allFiles[0]);
    cv::Ptr<cv::Feature2D> fdetector;
    if(d == "sift")
        fdetector = cv::xfeatures2d::SIFT::create(npoints, noctaves);
    else if(d == "surf")
        fdetector = cv::xfeatures2d::SURF::create(npoints, noctaves, 6);
    else
    {cout << "error in descs" << endl; exit(-1);}
    fdetector->detectAndCompute(refFrame,cv::Mat(),refKey,refDesc,false);
    //refMatches = npoints;

    for(int i = 0; i < (int) allFiles.size(); i++)
    {
        cout << "handling: " << allFiles[i] << endl;
        Mat crtFrame = imread(inputDir + allFiles[i]);
        vector<KeyPoint> crtKey;
        Mat crtDesc;
        cv::Ptr<cv::Feature2D> fdetector2;
        if(d == "sift")
            fdetector2 = cv::xfeatures2d::SIFT::create(npoints, noctaves);
        else if(d == "surf")
            fdetector2 = cv::xfeatures2d::SURF::create(npoints, noctaves, 6);
        else
        {cout << "error in descs" << endl; exit(-1);}
        fdetector2->detectAndCompute(crtFrame,cv::Mat(),crtKey,crtDesc,false);

        int nmatches = GetNumMatches(refDesc, crtDesc);
        if(nmatches < refDesc.rows * factor || i == 0)
        {
            if(imwrite((outputDir + allFiles[i]), crtFrame))
            {
                cout << "Saving " << allFiles[i] << " with " << crtDesc.rows << " descs." << endl;
            }
            else
            {
                cout << "Output error." << endl;
                exit(-2);
            }
            refFrame = crtFrame.clone();
            refDesc = crtDesc.clone();
        }
    }

}

void calcKeyFramesOrb(int noctaves, int npoints, double factor)
{
    refFrame = imread(inputDir + allFiles[0]);
    cv::Ptr<cv::Feature2D> fdetector = cv::ORB::create(npoints, 1.2f, noctaves);
    fdetector->detectAndCompute(refFrame,cv::Mat(),refKey,refDesc,false);

    for(int i = 0; i < (int) allFiles.size(); i++)
    {
        cout << "handling: " << allFiles[i] << endl;
        Mat crtFrame = imread(inputDir + allFiles[i]);
        vector<KeyPoint> crtKey;
        Mat crtDesc;
        cv::Ptr<cv::Feature2D> fdetector = cv::ORB::create(npoints, 1.2f, noctaves);
        fdetector->detectAndCompute(crtFrame,cv::Mat(),crtKey,crtDesc,false);
        cout << crtDesc << endl;
        int nmatches = GetNumMatches(refDesc, crtDesc);
        if(nmatches < refDesc.rows * factor || i == 0)
        {
            if(imwrite((outputDir + allFiles[i]), crtFrame))
            {
                cout << "Saving " << allFiles[i] << endl;
            }
            else
            {
                cout << "Output error." << endl;
                exit(-2);
            }
            refFrame = crtFrame.clone();
            refDesc = crtDesc.clone();
        }
    }
}

int getNumMatches(const Mat &desc1, const Mat &desc2)
{
    std::vector<std::vector<cv::DMatch> > matches12, matches21, good;

    for(int i = 0; i < desc1.rows; i++)
    {
        double mindis = 10000;
        int minid = 0;
        double dis = 0;
        for(int j = 0; j < desc2.rows; j++)
        {
            dis = (cv::sum(cv::abs(desc1.row(i) - desc2.row(j))))[0];
            if(dis < mindis)
            {
                mindis = dis;
                minid = j;
            }
        }

        double mindis_back = 10000;
        int minid_back = 0;

        for(int k = 0; k < desc1.rows; k++)
        {
            double dis_back = 0;
            dis_back = (cv::sum(cv::abs(desc2.row(minid) - desc1.row(k))))[0];
            if(dis_back < mindis_back)
            {
                mindis_back = dis_back;
                minid_back = k;
            }
        }
        std::vector<cv::DMatch> a;
        if(minid_back == i/* && mindis < 10000*/)
        {
            a.clear();
            a.push_back(cv::DMatch(i,minid,0,mindis));
            good.push_back(a);
        }
        else
        {
            //cout << "feature " << i << " is not good" << endl;
        }
    }
    cout << good.size() << " matches got." << endl;
    return (int)good.size();
}

int main(){
    Mat refFrame;
}