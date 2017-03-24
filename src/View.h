/*
 * View.h
 *
 *  Created on: Mar 21, 2017
 *      Author: xushen
 */

#ifndef VIEW_H_
#define VIEW_H_

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include "DataInterface.h"

using namespace std;
using namespace cv;

class View {
public:
	View();
	View(const string & name,
			const Mat & img,
			const vector<cv::KeyPoint> key,
			const Mat & desc);
	virtual ~View();
	int Create(const int cidx, const string & name, const CameraT & cam);
	void setCameraPose(const CameraT & cam);
	int setKeypoints(const int fidx, const cv::Point2d & pt);
	void set3dPoints(const int fidx, const cv::Point3d & pt3d);
	int getCidx() const;
	void setCidx(int cidx);
	const string& getName() const;
	void setName(const string& name);
	const cv::Matx33d& getR() const;
	const cv::Matx31d& getT() const;

private:
	string _name;
	int _cidx; // camera index
	cv::Matx33d _R;
	cv::Matx31d _T;

	cv::Mat _img;
	vector<cv::KeyPoint> _opencvkps;
	cv::Mat _opencvfeatures;

	vector<cv::KeyPoint> _kps;
	vector<cv::Mat> _features;
	vector<cv::Point3d> _pts3d;
};

#endif /* VIEW_H_ */
