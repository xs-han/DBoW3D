/*
 * View.cpp
 *
 *  Created on: Mar 21, 2017
 *      Author: xushen
 */

#include "View.h"

View::View():_cidx(-1){
	// TODO Auto-generated constructor stub
}

View::View(const string & name,
		const Mat & img,
		const vector<cv::KeyPoint> key,
		const Mat & desc):
				_name(name), _img(img),
				_opencvfeatures(desc),
				_cidx(-1)
{
	for(int i = 0; i < (int)key.size(); i++)
	{
		_opencvkps.push_back(KeyPoint( key[i].pt,
											key[i].size,
											key[i].angle,
											key[i].response,
											key[i].octave,
											key[i].class_id));
	}
}

View::~View() {
	// TODO Auto-generated destructor stub
}

int View::Create(const int cidx, const string & name, const CameraT & cam)
{
	setCidx(cidx);
	setName(name);
	setCameraPose(cam);

	string imagename = "/mnt/hgfs/Shared/MediaSoc/dataset/sequences/" + _name;
	_img = imread(imagename, cv::IMREAD_GRAYSCALE);
	if(_img.empty())
	{
		cout << "Error: The name of image is incorrect." << endl;
		cout << imagename << endl;
		return -1;
	}
	cv::Ptr<cv::xfeatures2d::SIFT> fde = cv::xfeatures2d::SIFT::create(2000, 8);
	fde->detectAndCompute(_img,cv::Mat(),_opencvkps,_opencvfeatures,false);
	return 1;
}

void View::setCameraPose(const CameraT & cam)
{
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++)
			_R(i,j) = cam.m[i][j];
	for(int i = 0; i < 3; i++)
	{
		_T(i) = cam.t[i];
	}
}

int View::setKeypoints(const int fidx, const Point2d & pt)
{
	if(fidx > (int)_kps.size())
		_kps.resize(fidx + 1);
	if(_name.empty())
	{
		cout << "Error: Need name to call Create() first." << endl;
		return -1;
	}

	int minidx = 0; double dis = 640 * 640 + 480 * 480; double curdis;
	for(int i = 0; i < (int)_opencvkps.size(); i++)
	{
		curdis = (_opencvkps[i].pt.x - (float)pt.x) *
				(_opencvkps[i].pt.x - (float)pt.x) +
				(_opencvkps[i].pt.y - (float)pt.y) *
				(_opencvkps[i].pt.y - (float)pt.y);
		if(curdis == 0 )
		{
			_kps[fidx] = _opencvkps[i];
			cv::imshow(_name, _img);
			cv::waitKey(0);
			return 1;
		}
		else if(curdis < dis)
		{
			dis = curdis;
			minidx = i;
		}
	}

	cout << "Warning: No matched point found for feature " << fidx << ". The min dis is: " << dis << endl;
	_kps[fidx] = _opencvkps[minidx];
	return 1;
}

void View::set3dPoints(const int fidx, const cv::Point3d & pt3d)
{
	if(fidx > (int)_pts3d.size())
		_pts3d.resize(fidx + 1);
	// Need To Check: Point2d -> KeyPoint;
	_pts3d[fidx] = cv::Point3d(pt3d);
}

int View::getCidx() const {
	return _cidx;
}

void View::setCidx(int cidx) {
	_cidx = cidx;
}

const string& View::getName() const {
	return _name;
}

void View::setName(const string& name) {
	_name = name;
}

const cv::Matx33d& View::getR() const {
	return _R;
}

const cv::Matx31d& View::getT() const {
	return _T;
}

