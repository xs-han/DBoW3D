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
#include <map>

using namespace std;
using namespace cv;

class View {
public:
	View();
	View(const string & name,
			int cidx,
			const Mat & img,
			const vector<cv::KeyPoint> key,
			const Mat & desc);
	virtual ~View();
	//int Create(const int cidx, const string & name, const CameraT & cam);
	void setCameraPose(const CameraT & cam);
	int setKeypoints(const int fidx, const cv::Point2d & pt, bool getfeatures);
	int set3dPoints(const int fidx, const int PointId);
	int getCidx() const;
	void setCidx(int cidx);
	const string& getName() const;
	void setName(const string& name);
	const cv::Matx33d& getR() const;
	const cv::Matx31d& getT() const;

	const vector<cv::KeyPoint>& getKps() const;
	const vector<int>& getPts3dId() const;
	const cv::Mat& getImg() const;
	const cv::Mat& getFeatures() const;
	const CameraT& getC() const;
	void setC(const CameraT& c);

	map<int, int> fidx2kpsidx;
	map<int, int> kpsidx2fidx;

public:
	string _name;
	int _cidx = -1; // camera index
	CameraT _C;
	cv::Matx33d _R;
	cv::Matx31d _T;

public:
	cv::Mat _img;
	vector<cv::KeyPoint> _opencvkps;
	cv::Mat _opencvfeatures;

	vector<cv::KeyPoint> _kps;
	cv::Mat _features;
	vector<int> _pts3dId;
};

#endif /* VIEW_H_ */
