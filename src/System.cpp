/*
 * System.cpp
 *
 *  Created on: Mar 21, 2017
 *      Author: xushen
 */

#include "System.h"
using namespace cv;

System::System() {
	// TODO Auto-generated constructor stub
	_K.zeros();
}

System::~System() {
	// TODO Auto-generated destructor stub
}

void System::VisualSFM(const string & imagefolder)
{
	int errorflag;
	jpgfolder = imagefolder;
	// Get all features
	if((errorflag = CreateOpencvKeypoints(jpgfolder, 3000, 16) < 0))
		exit(errorflag);
	// Store all features in .sift files
	if((errorflag = StoreSiftFiles()) < 0)
		exit(errorflag);
	// TODO: Add integrated VisualSFM commands with socket
	system("VisualSFM");
}

int System::CreateOpencvKeypoints(string & jpgfolder, int np, int noctave)
{
	// modify jpg folder with additionsl "/"
	if(*(jpgfolder.end()-1) != '/')
		jpgfolder += "/";
	// get all file names
	DIR * pdir;
    struct dirent *entry;
    if( (pdir = opendir(jpgfolder.c_str()) ))
    {
		while((entry = readdir(pdir)))
		{
			if( strcmp(entry->d_name, ".") != 0 &&
					strcmp(entry->d_name, "..") != 0 &&
					strstr(entry->d_name, ".jpg"))
			{
				_filenames.push_back(jpgfolder + entry->d_name);
				//cout << "found file:" << *(_filenames.end()-1) << endl;
			}
		}
		if((int)_filenames.size() <= 1)
		{
			cout << "No enough jpg images exist in this folder." << endl;
			return -1;
		}
		closedir(pdir);
    }
    else
    {
    	cout << "Error: incorrect jpg folder path." << endl;
    	return -2;
    }

    // read all images
    vector<cv::Mat> imgs;
    cv::Ptr<cv::xfeatures2d::SIFT> fde = cv::xfeatures2d::SIFT::create(np,noctave,0.03,8,1.4);
    _opencv_keypoints.clear();
    _opencv_descriptors.clear();
    _opencv_keypoints.resize(_filenames.size());
    _opencv_descriptors.resize(_filenames.size());
    imgs.resize(_filenames.size());
    for(int i = 0; i < (int)_filenames.size(); i++)
    {
    	imgs[i] = imread(_filenames[i].c_str(),IMREAD_GRAYSCALE);
    	fde->detectAndCompute(imgs[i],cv::noArray(),
    			_opencv_keypoints[i],_opencv_descriptors[i],false);
    	// Initialize all keyframes
    	cout << "Extracting image: " << _filenames[i] << " with " << _opencv_keypoints[i].size() << " features." << endl;
//    	Mat out;
//    	cv::drawKeypoints(imgs[i],_opencv_keypoints[i],out);
//    	imshow(_filenames[i],out);
//    	cv::waitKey(0);
    	_KeyFrames.push_back(View(_filenames[i],
    			imgs[i],_opencv_keypoints[i],_opencv_descriptors[i]));
    }
	return 1;
}

int System::StoreSiftFiles()
{
	//debug
	int nfiles = (int)_filenames.size();
	if(_filenames.size()!=_opencv_keypoints.size() ||
			_opencv_keypoints.size()!=_opencv_descriptors.size())
	{
		cout << "Error: The size of files and features are not same." << endl;
		return -1;
	}

	for(int i = 0; i < nfiles; i++)
	{
		string n = _filenames[i];
		n.erase(n.find(".jpg"),4).append(".sift");
		ofstream of(n, ios::out);
		if(of.is_open())
		{
			of << _opencv_descriptors[i].rows << " "
			   << _opencv_descriptors[i].cols << endl;
		}
		for(int j = 0; j < (int)_opencv_keypoints[i].size(); j++)
		{
			of  << _opencv_keypoints[i][j].pt.y << " "
				<< _opencv_keypoints[i][j].pt.x << " "
				<< _opencv_keypoints[i][j].size << " "
				<< _opencv_keypoints[i][j].angle/180.0*M_PI - M_PI << " "
				<< endl;

			for(int k = 0; k < _opencv_descriptors[i].cols; k++)
			{
				if(k != 0 && k % 20 == 0)
					of << endl;
				of << " " << _opencv_descriptors[i].at<float>(j,k);
			}
			of << endl;
		}

		cout << "Saving sift file: " << n << endl;
		//cout << _opencv_descriptors[i].row(0) << endl;
		of.close();

	}
	return 1;
}

void System::LoadK(const string & CalibFile)
{
	ifstream in(CalibFile.c_str());
	if(!in.is_open())
	{
		cout << "Open intrinsic parameter file failed." << endl;
		exit(-1);
	}
	in >> _K(0,0) >> _K(1,1) >> _K(0,2) >> _K(1,2) >> _K(0,1);
	_K(2,2) = 1;
	cout << "Get intrinsic parameter:" << endl << _K << endl;
}

void System::LoadNVM(const string & nvm)
{
	int errorflag = 0;
	if(nvm == "")
	{
		if((errorflag = LoadNVMData(jpgfolder + "sfm.nvm") < 0))
			exit(errorflag);
	}
	else
	{
		if((errorflag = LoadNVMData(nvm) < 0))
			exit(errorflag);
	}
}

int System::LoadNVMData(const string & nvmFile){
	ifstream in(nvmFile.c_str());
	if (!in.is_open()){
		cout << "Error: NVM file open failed." << endl;
		return -1;
	}

	/////////////////////////////////////////
	//read parameters and intrinsic matrix
	int rotation_parameter_num = 4;
	bool format_r9t = false;
	string token;
	char tokenFirst[256];
	if(in.peek() == 'N')
	{
		in.getline(tokenFirst, 256); //file header
		cout << tokenFirst << endl;

		if(strstr(tokenFirst, "R9T"))
		{
			rotation_parameter_num = 9;    //rotation as 3x3 matrix
			format_r9t = true;
		}

		if(strstr(tokenFirst, "FixedK"))
		{
			istringstream iss(tokenFirst);
			string tmp;
			//double fx, fy, cx, cy;
			if((iss >> tmp) && tmp == "NVM_V3" && (iss >> tmp) && tmp == "FixedK")
			{
				iss >> _K(0,0) >> _K(0,2) >> _K(1,1) >> _K(1,2);
			}
			_K(2,2) = 1;
			cout << "Get intrinsic parameter:" << endl << _K << endl;
		}
	}


	//////////////////////////////////////////
	//read R and T of cameras
	int ncam = 0;
	// read # of cameras
	in >> ncam;
	if(ncam <= 1){
		cout << "Error: Less than one camera exist. Invalid model." << endl;
		return -2;
	}
	//read the camera parameters
	_camera_data.clear();
	_measurements.clear();
	_names.clear();
	//_point3d_ids.clear();
	_camera_data.resize(ncam); // allocate the camera data
	//_measurements.resize(ncam); // allocate the measurements data
	_names.resize(ncam);
	//_point3d_ids.resize(ncam);

	for(int i = 0; i < ncam; ++i)
	{
		double f, q[9], c[3], d[2];
		in >> token >> f;
		for(int j = 0; j < rotation_parameter_num; ++j) in >> q[j];
		in >> c[0] >> c[1] >> c[2] >> d[0] >> d[1];

		_camera_data[i].SetFocalLength(f);
		if(format_r9t)
		{
			_camera_data[i].SetMatrixRotation(q);
			_camera_data[i].SetTranslation(c);
		}
		else
		{
			//older format for compability
			_camera_data[i].SetQuaternionRotation(q);        //quaternion from the file
			_camera_data[i].SetCameraCenterAfterRotation(c); //camera center from the file
		}
		_camera_data[i].SetNormalizedMeasurementDistortion(d[0]);
		_names[i] = token;
	}


	//////////////////////////////////////
	// npoints-> # of total points; nproj-> #of all 2D features
	int npoint = 0, nproj = 0;
	in >> npoint;
	if(npoint <= 0) {
		cout << "Error: The number of points is less than 0." << endl;
		return -3;
	}
	//read image projections and 3D points.
	//for each 3D point, we check the reprojection error of each view
	_point_data.clear();
	_measurements.clear();
	_measurements_idxs.clear();
	//_pointMap.clear();
	_point_data.resize(npoint);
	_measurements.resize(npoint);
	_measurements_idxs.resize(npoint);
	//_pointMap.resize(npoint);
	//int outlier_count = 0;
	//int he = 0;
	for(int id = 0; id < npoint; ++id)
	{
		// pt[3]->[X,Y,Z]; cc[3]->[R,G,B]; nopj-># of projections
		float pt[3]; int cc[3], npj;
		in  >> pt[0] >> pt[1] >> pt[2]
			>> cc[0] >> cc[1] >> cc[2] >> npj;

		_point_data[id] = cv::Point3d(pt[0], pt[1], pt[2]);
		for(int j = 0; j < npj; j++)
		{
			int cidx, fidx; double imx, imy;
			in >> cidx >> fidx >> imx >> imy;
			_measurements[id].push_back(cv::Point2d(imx + _K(0,2), imy + _K(1,2)));
			// Check the location of features
			if(imx + _K(0,2) < 0 || imx + _K(0,2) > 640 || imy + _K(1,2) < 0 || imy + _K(1,2) > 480)
			{
				cout << "This is a valid check for resolution 640x480" << endl;
				cout << "Invalid feature location exist. At Camera: " << cidx << " Feature: " << fidx << endl;
			}
			_measurements_idxs[id].push_back(std::make_pair(cidx, fidx));
		}
		nproj += npj;

		//Point3d PW = Point3d(pt[0], pt[1], pt[2]);
		//bool outlier = false;
		//vector<int> cidxVec;
		//vector<int> fidxVec;
		//vector<pair<double, double> > im_pts;

		//check the reprojection error to filter the outliers
//		for(int j = 0; j < npj; ++j)
//		{
//			int cidx, fidx; double imx, imy;
//			in >> cidx >> fidx >> imx >> imy;
//
//			cidxVec.push_back(cidx);
//			fidxVec.push_back(fidx);
//			im_pts.push_back(make_pair(imx, imy));
//
//			imx += _K(0,2);
//			imy += _K(1,2);

			//CameraT& cam = _camera_data[cidx];
			//Point3d PC;
			//Point2d pt_img_reproj;
			//Point2d pt_img(imx, imy);

			//projectToCam(cam, PW, PC);
			//projectToImage(_K, PC, pt_img_reproj);

			//double reprojErr = norm(pt_img - pt_img_reproj);
//			if (reprojErr > reproj_err_thres){
//				outlier = true;
//			}
//		}

//		if (outlier)
//			outlier_count++;
//		else
//		{
//			for(int j = 0; j < npj; ++j){
//				int cidx, fidx;
//				double imx, imy;
//				cidx = cidxVec[j];
//				fidx = fidxVec[j];
//				imx = im_pts[j].first;
//				imy = im_pts[j].second;
//
////				_camidx.push_back(cidx);    //camera index
////				_ptidx.push_back(id);        //point index
//
//				//add a measurment to the vector
//				bool flag = false;
//				for (int i = 0; i < _measurements[cidx].size(); i++){
//					if (imx == _measurements[cidx][i].x && imy == _measurements[cidx][i].y)
//					{
//						flag = true;
//						break;
//					}
//				}
//				if (flag){
//					cout << "Here!!!!" << endl;
//					he++;
//					continue;
//				}
//
//
//				_measurements[cidx].push_back(cv::Point2d(imx, imy));
//
//				_point3d_ids[cidx].push_back(id);
//				_pointMap[id].push_back(cidx);
//				_pointMap[id].push_back(_measurements[cidx].size() - 1);
//				nproj ++;
//			}
//			_point_data[id] = Point3d(pt[0], pt[1], pt[2]);
//			_ptc.insert(_ptc.end(), cc, cc + 3);
//		}
	}
	///////////////////////////////////////////////////////////////////////////////
	std::cout << ncam << " cameras; " << npoint << " 3D points; " << nproj << " projections\n";
	//cout << "outlier number: " << outlier_count << endl;
	//cout << "Overrided 2D points" << he;

	return 1;
}

int System::CreateCameraViews()
{
	// Check valid solution of NVM data.
	int npoints = (int)_point_data.size();
	if(_point_data.size()!=_measurements.size() ||
			_measurements.size() != _measurements_idxs.size())
	{
		cout << "Error: The size of 3D points' locations, measures are not same." << endl;
		return -1;
	}
	int ncam = _camera_data.size();
	if(_KeyFrames.size()!=_names.size())
	{
		cout << "Error: The size of cameras and corresponding names are not same." << endl;
		return -2;
	}

	_KeyFrames.resize(ncam);
	for(int i = 0; i < ncam; i++)
	{
		_KeyFrames[i].Create(i, _names[i], _camera_data[i]);
	}
	for(int i = 0; i < npoints; i++)
	{
		int nfeatures = (int)_measurements[i].size();
		if(_measurements[i].size()!=_measurements_idxs[i].size())
		{
			cout << "Error: The size of measures and idxes are not same." << endl;
			return -3;
		}

		for(int j = 0; j < nfeatures; j++)
		{
			int cameraidx = _measurements_idxs[i][j].first;
			int featureidx = _measurements_idxs[i][j].second;
			if(cameraidx > ncam)
			{
				cout << "Error: The idx of camera cannot be found." << endl;
				return -4;
			}

			Point2d p = _measurements[i][j];
			_KeyFrames[cameraidx].setKeypoints(featureidx,p);
		}
	}
	return 1;
}

void System::MakeCameraView()
{
	CreateCameraViews();
}

