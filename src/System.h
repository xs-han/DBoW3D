/*
 * System.h
 *
 *  Created on: Mar 21, 2017
 *      Author: xushen
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "CameraViews.h"
#include "VirtualView.h"
#include "SpatialPoint.h"
#include "DataInterface.h"
#include "dirent.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <math.h>

using namespace std;

class System {
public:
	System();
	void LoadNVM(const string & nvm = "");
	void LoadK(const string & CalibFile);
	void MakeCameraView();
	void VisualSFM(const string & imagefolder);
	virtual ~System();

private:
	int LoadNVMData(const string & nvmFile);
	int CreateCameraViews();
	int CreateOpencvKeypoints(string & imagefolder, int np, int noctave);
	int StoreSiftFiles();

private:
	// Opencv sift variables
	vector<vector<KeyPoint> > _opencv_keypoints;
	vector<Mat> _opencv_descriptors;

	// All Points and Cameras
	vector<SpatialPoint> _PC;
	vector<View> _KeyFrames;
	vector<VirtualView> _VirtualFrames;

	// NVM variables
	// 2D cameras
	vector<CameraT> _camera_data;
	vector<string> _names;
	// 3D points
	vector<cv::Point3d> _point_data; // [X, Y, Z]
	vector<vector<pair<int, int> > > _measurements_idxs; // [cidx, fidx]
	vector<vector<cv::Point2d> > _measurements; // [mx+cx, my+cy]
//	vector<vector<int> > _point3d_ids;
//	vector<vector<int> > _pointMap;
//	vector<int> _ptc;

	cv::Matx33d _K;
	string jpgfolder;
	vector<string> _filenames;
};

#endif /* SYSTEM_H_ */
