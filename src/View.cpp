/*
 * View.cpp
 *
 *  Created on: Mar 21, 2017
 *      Author: xushen
 */

#include "View.h"

#define mynorm(x,y) (((x) * (x) + (y) * (y)))

View::View():_cidx(-1){
	// TODO Auto-generated constructor stub
}

View::View(const string & name,
			const Mat & img,
			const vector<cv::KeyPoint> key,
			const Mat & desc):
			_name(name), _cidx(-1),_img(img),
			_opencvfeatures(desc)
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
	//_kps.resize(_opencvkps.size());
	//_opencvfeatures.create(desc.rows,desc.cols,CV_32F);
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

int View::setKeypoints(const int fidx, const Point2d & pt, bool getfeatures)
{
//	if(fidx > (int)_kps.size())
//	{
//		cout << "Error: Incorrect feature idx. Larger than size of kps." << endl;
//		return -1;
//	}
	if(_name.empty())
	{
		cout << "Error: Need name to be created first." << endl;
		return -2;
	}

	//cout << sqrt(mynorm(_opencvkps[fidx].pt.x - pt.x, _opencvkps[fidx].pt.y - pt.y)) << endl;
	if(sqrt(mynorm((_opencvkps[fidx].pt.x - pt.x), (_opencvkps[fidx].pt.y - pt.y))) > 1 )
	{
		cout << "Error: Key point " << fidx << "can not be found." << endl;
		return -3;
	}

	_kps.push_back(KeyPoint( _opencvkps[fidx].pt,
			_opencvkps[fidx].size,
			_opencvkps[fidx].angle,
			_opencvkps[fidx].response,
			_opencvkps[fidx].octave,
			_opencvkps[fidx].class_id));
	fidx2kpsidx[fidx] = (int)_features.size();
	kpsidx2fidx[(int)_features.size()] = fidx;
	if(getfeatures)
		_features.push_back(_opencvfeatures.row(fidx));
	return 1;
}

int View::set3dPoints(const int fidx, const int PointId)
{
	if(fidx2kpsidx[fidx] != (int)_pts3dId.size())
	{
		cout << "Need to set 2D features first." << endl;
		return -1;
	}
	// Need To Check: Point2d -> KeyPoint;
	_pts3dId.push_back(PointId);
	return 1;
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

const vector<cv::Mat>& View::getFeatures() const {
	return _features;
}

const vector<cv::KeyPoint>& View::getKps() const {
	return _kps;
}

const vector<int>& View::getPts3dId() const {
	return _pts3dId;
}

const cv::Mat& View::getImg() const {
	return _img;
}
