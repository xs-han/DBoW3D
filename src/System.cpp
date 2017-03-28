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

void System::VisualSFM(const string & imagefolder) {
	int errorflag;
	_jpgfolder = imagefolder;
	// Get all features
	if ((errorflag = CreateOpencvKeypoints(_jpgfolder, 3000, 16) < 0))
		exit(errorflag);
	// Store all features in .sift files
//	if((errorflag = StoreSiftFiles()) < 0)
//		exit(errorflag);
	// TODO: Add integrated VisualSFM commands with socket
	//system("VisualSFM");
}

int System::CreateOpencvKeypoints(string & jpgfolder, int np, int noctave) {
	// modify jpg folder with additionsl "/"
	if (*(jpgfolder.end() - 1) != '/')
		jpgfolder += "/";
	// get all file names
	DIR * pdir;
	struct dirent *entry;
	if ((pdir = opendir(jpgfolder.c_str()))) {
		while ((entry = readdir(pdir))) {
			if (strcmp(entry->d_name, ".") != 0
					&& strcmp(entry->d_name, "..") != 0
					&& strstr(entry->d_name, ".jpg")) {
				_filenames.push_back(jpgfolder + entry->d_name);
				//cout << "found file:" << *(_filenames.end()-1) << endl;
			}
		}
		if ((int) _filenames.size() <= 1) {
			cout << "No enough jpg images exist in this folder." << endl;
			return -1;
		}
		closedir(pdir);
	} else {
		cout << "Error: incorrect jpg folder path." << endl;
		return -2;
	}

	// read all images
	vector<cv::Mat> imgs;
	cv::Ptr<cv::xfeatures2d::SIFT> fde = cv::xfeatures2d::SIFT::create(np,
			noctave, 0.03, 8, 1.4);
	_opencv_keypoints.clear();
	_opencv_descriptors.clear();
	_opencv_keypoints.resize(_filenames.size());
	_opencv_descriptors.resize(_filenames.size());
	imgs.resize(_filenames.size());
	for (int i = 0; i < (int) _filenames.size(); i++) {
		imgs[i] = imread(_filenames[i].c_str(), IMREAD_GRAYSCALE);
		fde->detectAndCompute(imgs[i], cv::noArray(), _opencv_keypoints[i],
				_opencv_descriptors[i], false);
		// Initialize all keyframes
		cout << "Extracting image: " << _filenames[i] << " with "
				<< _opencv_keypoints[i].size() << " features." << endl;
//    	Mat out;
//    	cv::drawKeypoints(imgs[i],_opencv_keypoints[i],out);
//    	imshow(_filenames[i],out);
//    	cv::waitKey(0);
		_KeyFrames.push_back(
				View(_filenames[i], i, imgs[i], _opencv_keypoints[i],
						_opencv_descriptors[i]));
	}
	return 1;
}

void System::MakeVirtualView(const string & nvmfile, int yaw_diff) {
	if (_PC.empty() || _KeyFrames.empty()) {
		cout << "Error: Camera views must be established before virtual views."
				<< endl;
		exit(-1);
	}
	yaw_diff = yaw_diff / M_PI;
	int errorflag = 0;
	for(int i = 0; i < (int)_KeyFrames.size(); i++)
	{
		//cout << "Handling KeyFrame: " << i << endl;
		if (((errorflag = CreateVirtualViews(i, 30)) < 0)) {
			cout << "Error in creating virtual views." << endl;
			exit(errorflag);
		}
		else if(errorflag == 0)
		{
			//cout << "Not created" << endl;
			continue;
		}
		else
		{
			//cout << "Created" << endl;
			if ((errorflag = VisualizeVirtualViews(i, nvmfile) < 0)) {
				cout << "Error in visualizing virtual views." << endl;
				exit(errorflag);
			}
		}
		//cout << "Finished" << endl;
	}

}

int System::StoreSiftFiles() {
	//debug
	int nfiles = (int) _filenames.size();
	if (_filenames.size() != _opencv_keypoints.size()
			|| _opencv_keypoints.size() != _opencv_descriptors.size()) {
		cout << "Error: The size of files and features are not same." << endl;
		return -1;
	}

	for (int i = 0; i < nfiles; i++) {
		string n = _filenames[i];
		n.erase(n.find(".jpg"), 4).append(".sift");
		ofstream of(n, ios::out);
		if (of.is_open()) {
			of << _opencv_descriptors[i].rows << " "
					<< _opencv_descriptors[i].cols << endl;
		}
		for (int j = 0; j < (int) _opencv_keypoints[i].size(); j++) {
			of << _opencv_keypoints[i][j].pt.y << " "
					<< _opencv_keypoints[i][j].pt.x << " "
					<< _opencv_keypoints[i][j].size << " "
					<< _opencv_keypoints[i][j].angle / 180.0 * M_PI - M_PI
					<< " " << endl;

			for (int k = 0; k < _opencv_descriptors[i].cols; k++) {
				if (k != 0 && k % 20 == 0)
					of << endl;
				of << " " << _opencv_descriptors[i].at<float>(j, k);
			}
			of << endl;
		}

		cout << "Saving sift file: " << n << endl;
		//cout << _opencv_descriptors[i].row(0) << endl;
		of.close();

	}
	return 1;
}

void System::LoadK(const string & CalibFile) {
	ifstream in(CalibFile.c_str());
	if (!in.is_open()) {
		cout << "Open intrinsic parameter file failed." << endl;
		exit(-1);
	}
	in >> _K(0, 0) >> _K(1, 1) >> _K(0, 2) >> _K(1, 2) >> _K(0, 1);
	_K(2, 2) = 1;
	cout << "Get intrinsic parameter:" << endl << _K << endl;
}

void System::LoadNVM(const string & nvm) {
	int errorflag = 0;
	if (nvm == "") {
		if ((errorflag = LoadNVMData(_jpgfolder + "sfm.nvm") < 0))
			exit(errorflag);
	} else {
		if ((errorflag = LoadNVMData(nvm) < 0))
			exit(errorflag);
	}
}

int System::LoadNVMData(const string & nvmFile) {
	ifstream in(nvmFile.c_str());
	if (!in.is_open()) {
		cout << "Error: NVM file open failed." << endl;
		return -1;
	}

	/////////////////////////////////////////
	//read parameters and intrinsic matrix
	int rotation_parameter_num = 4;
	bool format_r9t = false;
	string token;
	char tokenFirst[256];
	if (in.peek() == 'N') {
		in.getline(tokenFirst, 256); //file header
		cout << tokenFirst << endl;

		if (strstr(tokenFirst, "R9T")) {
			rotation_parameter_num = 9;    //rotation as 3x3 matrix
			format_r9t = true;
		}

		if (strstr(tokenFirst, "FixedK")) {
			istringstream iss(tokenFirst);
			string tmp;
			//double fx, fy, cx, cy;
			if ((iss >> tmp) && tmp == "NVM_V3" && (iss >> tmp)
					&& tmp == "FixedK") {
				iss >> _K(0, 0) >> _K(0, 2) >> _K(1, 1) >> _K(1, 2);
			}
			_K(2, 2) = 1;
			cout << "Get intrinsic parameter:" << endl << _K << endl;
		}
	}

	//////////////////////////////////////////
	//read R and T of cameras
	int ncam = 0;
	// read # of cameras
	in >> ncam;
	if (ncam <= 1) {
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

	for (int i = 0; i < ncam; ++i) {
		double f, q[9], c[3], d[2];
		in >> token >> f;
		for (int j = 0; j < rotation_parameter_num; ++j)
			in >> q[j];
		in >> c[0] >> c[1] >> c[2] >> d[0] >> d[1];

		_camera_data[i].SetFocalLength(f);
		if (format_r9t) {
			_camera_data[i].SetMatrixRotation(q);
			_camera_data[i].SetTranslation(c);
		} else {
			//older format for compability
			_camera_data[i].SetQuaternionRotation(q); //quaternion from the file
			_camera_data[i].SetCameraCenterAfterRotation(c); //camera center from the file
		}
		_camera_data[i].SetNormalizedMeasurementDistortion(d[0]);
		_names[i] = token;
	}

	//////////////////////////////////////
	// npoints-> # of total points; nproj-> #of all 2D features
	int npoint = 0, nproj = 0;
	in >> npoint;
	if (npoint <= 0) {
		cout << "Error: The number of points is less than 0." << endl;
		return -3;
	}
	//read image projections and 3D points.
	//for each 3D point, we check the reprojection error of each view
	_point_data.clear();
	_measurements.clear();
	_measurements_cidxs.clear();
	_measurements_fidxs.clear();
	//_pointMap.clear();
	_point_data.resize(npoint);
	_measurements.resize(npoint);
	_measurements_cidxs.resize(npoint);
	_measurements_fidxs.resize(npoint);
	//_pointMap.resize(npoint);
	//int outlier_count = 0;
	//int he = 0;
	for (int id = 0; id < npoint; ++id) {
		// pt[3]->[X,Y,Z]; cc[3]->[R,G,B]; nopj-># of projections
		float pt[3];
		int cc[3], npj;
		in >> pt[0] >> pt[1] >> pt[2] >> cc[0] >> cc[1] >> cc[2] >> npj;

		_point_data[id] = cv::Point3d(pt[0], pt[1], pt[2]);
		for (int j = 0; j < npj; j++) {
			int cidx, fidx;
			double imx, imy;
			in >> cidx >> fidx >> imx >> imy;
			_measurements[id].push_back(
					cv::Point2d(imx + _K(0, 2), imy + _K(1, 2)));
			// Check the location of features
			if (imx + _K(0, 2) < 0 || imx + _K(0, 2) > 640 || imy + _K(1, 2) < 0
					|| imy + _K(1, 2) > 480) {
				cout << "This is a valid check for resolution 640x480" << endl;
				cout << "Invalid feature location exist. At Camera: " << cidx
						<< " Feature: " << fidx << endl;
			}
			_measurements_cidxs[id].push_back(cidx);
			_measurements_fidxs[id].push_back(fidx);
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
	std::cout << ncam << " cameras; " << npoint << " 3D points; " << nproj
			<< " projections\n";
	//cout << "outlier number: " << outlier_count << endl;
	//cout << "Overrided 2D points" << he;

	return 1;
}

int System::CreateCameraViews() {
	// Check valid solution of NVM data.
	int npoints = (int) _point_data.size();
	if (_point_data.size() != _measurements.size()
			|| _measurements.size() != _measurements_cidxs.size()) {
		cout
				<< "Error: The size of 3D points' locations, measures are not same."
				<< endl;
		return -1;
	}
	int ncam = (int) _KeyFrames.size();
	if (_KeyFrames.size() != _names.size()
			|| _names.size() != _camera_data.size()) {
		cout
				<< "Error: The # of original images and cameras from nvm are not same."
				<< endl;
		return -2;
	}

	//Set camera poses
	for (int i = 0; i < ncam; i++) {
		//debug
		int* debugarray = new int[ncam];
		for (int j = 0; j < ncam; j++)
			debugarray[j] = 0;

		//get same camera via name, because the order of camera data
		// is different from that the order of filenames in the jpg folder
		int chosenj = 0;
		for (int j = 0; j < ncam; j++) {
			if (_KeyFrames[j].getName() == _jpgfolder + _names[i]) {
				_KeyFrames[j].setCameraPose(_camera_data[i]);
				chosenj = j;
				debugarray[j]++;
			}
		}
		// Currently, we store key frames in the alphabet order of filenames.
		// but in the nvm file, the order of images is changed.
		// Image index/Camera index is in accordance with the order in nvm file.
		// Here, we change the order of key frames to the nvm order.
		std::swap(_KeyFrames[i], _KeyFrames[chosenj]);

		//debug
		int sum = 0;
		for (int j = 0; j < ncam; j++)
			sum += debugarray[j];
		if (sum != 1) {
			if (sum == 0) {
				cout << "No camera is set. Incorrect case." << endl;
				return -3;
			} else {
				cout << "More than one cameras are set. Incorrect case."
						<< endl;
				return -4;
			}
		}
		delete[] debugarray;
	}

	//debug
	for (int i = 0; i < ncam; i++) {
		if (_KeyFrames[i].getName() != _jpgfolder + _names[i]) {
			cout << "Error in exchange." << endl;
			return -4;
		}
	}

	//Set poionts' data;
	for (int i = 0; i < npoints; i++) {
		_PC.push_back(
				SpatialPoint(i, _point_data[i], _measurements_cidxs[i],
						_measurements_fidxs[i]));

		int nfeatures = (int) _measurements[i].size();
		if (_measurements[i].size() != _measurements_cidxs[i].size()) {
			cout << "Error: The size of measures and idxes are not same."
					<< endl;
			return -5;
		}

		//set 2D features, 3D pionts
		for (int j = 0; j < nfeatures; j++) {
			int cameraidx = _measurements_cidxs[i][j];
			int featureidx = _measurements_fidxs[i][j];
			if (cameraidx >= ncam) {
				cout << "Error: The idx of camera cannot be found." << endl;
				return -6;
			}
			Point2d p = _measurements[i][j];
			//2D
			if (_KeyFrames[cameraidx].setKeypoints(featureidx, p, true) < 0) {
				cout << "Error in setKeypoints." << endl;
				return -7;
			}
			//3D
			if (_KeyFrames[cameraidx].set3dPoints(featureidx, i) < 0) {
				cout << "Error in set3dPoints." << endl;
				return -8;
			}
		}

	}
	return 1;
}

void System::MakeCameraView() {
	int errorflag = 0;
	if ((errorflag = CreateCameraViews() < 0))
		exit(errorflag);
	cout << "Everything is all right." << endl;
	//PrintAllKeypoints(0);
	//PrintAllFrames(0);
}

const void System::PrintAllKeypoints(int KeyFrameId) const {
	int n = 0;
	vector<cv::KeyPoint> kps = _KeyFrames[KeyFrameId].getKps();
	Mat out;
	for (auto i = kps.begin(); i != kps.end(); i++) {
		if ((*i).angle != -1) {
			cout << "Key point " << n << " exist at <" << (*i).pt.x << ","
					<< (*i).pt.y << ">" << " for 3D point: "
					<< _PC[_KeyFrames[KeyFrameId].getPts3dId()[n]].getPt3d()
					<< endl;
		}
		n++;
	}
	cv::drawKeypoints(_KeyFrames[KeyFrameId].getImg(), kps, out);
	namedWindow(_KeyFrames[KeyFrameId].getName());
	imshow(_KeyFrames[KeyFrameId].getName(), out);
	waitKey(0);
	destroyAllWindows();
}

const void System::PrintAllFrames(int Pt3dId) const {
	vector<int> cidx = _PC[Pt3dId].getCameraIdx();
	vector<int> fidx = _PC[Pt3dId].getFeatureIdx();
	if (cidx.size() != fidx.size())
		cout << "Error in Printing." << endl;
	for (int i = 0; i < (int) cidx.size(); i++) {
		View v = _KeyFrames[cidx[i]];
		vector<KeyPoint> k;
		k.clear();
		k.push_back(v.getKps()[v.fidx2kpsidx[fidx[i]]]);
		cv::namedWindow(v.getName());
		Mat out;
		cv::drawKeypoints(v.getImg(), k, out);
		imshow(v.getName(), out);
		cout << v.getFeatures().row(v.fidx2kpsidx[fidx[i]]) << endl;
	}
	waitKey(0);
	destroyAllWindows();
}

int System::CreateVirtualViews(const int cidx, int yaw_diff) {
	// If # of cameras before is smaller than 10, we don't generate virtual views
	vector<int> c2v;
	c2v.clear();
	if(_KeyFrames[cidx].getCidx() < 10)
	{
		_Camera2Virtual.push_back(c2v);
		return 0;
	}
	// real camera C: [R T];
	CameraT real_C;
	real_C.SetCameraT(_KeyFrames[cidx].getC());
	int v_cidx = (int)_KeyFrames.size();
	double y = 0;
	for(int j = 1; (y = (j*((double)yaw_diff) / 180 * M_PI)) < M_PI / 2; j++)
	{
		double v_r[3];
		double v_camc[3];
		CameraT v_C;
		vector<cv::KeyPoint> key;
		Mat desc;
		desc.resize(0,0);
		key.clear();

		// two direacitons: +y and -y

		//determine virtual camera pose [R t]
		// determine new yaw. The pitch and roll keep
		real_C.GetRodriguesRotation(v_r);
		real_C.GetCameraCenter(v_camc);
		v_r[1] = v_r[1] - y;
		// set new camera pose
		v_C.SetCameraT(real_C);
		v_C.SetRodriguesRotation(v_r);
		v_C.SetCameraCenterAfterRotation(v_camc);

		// generate new virtual views
		VirtualView v1("v"+(_KeyFrames[cidx].getName())+std::to_string(-j),
														v_cidx++ ,
														cv::Mat(),
														key,
														desc,
														-y);
		v1.setCameraPose(v_C);

		//TODO: Get features for virtual views. (key1, desc1)
		for(auto p = _PC.begin(); p != _PC.end(); p++)
		{
			Matx31d point_camera = v1._R * (p->getPt3d()) + v1._T;
			Matx31d point_homo_image = _K * point_camera;
			Matx21d point_image(point_homo_image(0)/point_homo_image(2),
								point_homo_image(1)/point_homo_image(2));
			if(		point_camera(2) < 0 ||
					point_image(0) < 0 ||
					point_image(0) >= 640 ||
					point_image(1) < 0 ||
					point_image(1) >= 480)
			{
				continue;
			}
			Mat desc1 = Mat::zeros(1,128,CV_32F); int n = 0;
			for(auto p_real_cameras = (p->_CameraIdx).begin(), p_real_features = (p->_FeatureIdx).begin();
					p_real_cameras != (p->_CameraIdx).end() && p_real_features != (p->_FeatureIdx).end();
					p_real_cameras++, p_real_features++)
			{
				if((*p_real_cameras) <= _KeyFrames[cidx].getCidx() && (*p_real_cameras) > _KeyFrames[cidx].getCidx() - 6)
				{
					desc1 = desc1 + _KeyFrames[*p_real_cameras]._features.row(_KeyFrames[*p_real_cameras].fidx2kpsidx[*p_real_features]);
					n++;
				}
			}
			if(n > 1)
			{
				desc1 = desc1 / n;
				desc.push_back(desc1);
				key.push_back(KeyPoint(point_image(0), point_image(1), 0, 0));
				//cout << p->_PointId << " " << key.size()-1 << " " << point_image(0) << " " << point_image(1) << endl;
			}
			else
			{
				//cout << "Less than 2 frames keep this point." << endl;
				continue;
			}
		}
		v1._features = desc.clone();
		v1._kps = key;

		c2v.push_back((int)_VirtualFrames.size());
		_VirtualFrames.push_back(v1);


		//determine virtual camera pose [R t]
		// determine new yaw. The pitch and roll keep
		real_C.GetRodriguesRotation(v_r);
		real_C.GetCameraCenter(v_camc);
		v_r[1] = v_r[1] + y;
		// set new camera pose
		v_C.SetCameraT(real_C);
		v_C.SetRodriguesRotation(v_r);
		v_C.SetCameraCenterAfterRotation(v_camc);

		// generate new virtual views
		VirtualView v2("v"+(_KeyFrames[cidx].getName())+std::to_string(j),
														v_cidx++ ,
														cv::Mat(),
														key,
														desc,
														y);
		v2.setCameraPose(v_C);

		//TODO: Get features for virtual views. (key1, desc1)
		for(auto p = _PC.begin(); p != _PC.end(); p++)
		{
			Matx31d point_camera = v2._R * (p->getPt3d()) + v2._T;
			Matx31d point_homo_image = _K * point_camera;
			Matx21d point_image(point_homo_image(0)/point_homo_image(2),
								point_homo_image(1)/point_homo_image(2));
			if(		point_camera(2) < 0 ||
					point_image(0) < 0 ||
					point_image(0) >= 640 ||
					point_image(1) < 0 ||
					point_image(1) >= 480)
			{
				continue;
			}
			Mat desc1 = Mat::zeros(1,128,CV_32F); int n = 0;
			for(auto p_real_cameras = (p->_CameraIdx).begin(), p_real_features = (p->_FeatureIdx).begin();
					p_real_cameras != (p->_CameraIdx).end() && p_real_features != (p->_FeatureIdx).end();
					p_real_cameras++, p_real_features++)
			{
				if((*p_real_cameras) <= _KeyFrames[cidx].getCidx() && (*p_real_cameras) > _KeyFrames[cidx].getCidx() - 6)
				{
					desc1 = desc1 + _KeyFrames[*p_real_cameras]._features.row(_KeyFrames[*p_real_cameras].fidx2kpsidx[*p_real_features]);
					n++;
				}
			}
			if(n > 1)
			{
				desc1 = desc1 / n;
				desc.push_back(desc1);
				key.push_back(KeyPoint(point_image(0), point_image(1), 0, 0));
				//cout << p->_PointId << " " << key.size()-1 << " " << point_image(0) << " " << point_image(1) << endl;
			}
			else
			{
				//cout << "Less than 2 frames keep this point." << endl;
				continue;
			}
		}
		v2._features = desc.clone();
		v2._kps = key;

		c2v.push_back((int)_VirtualFrames.size());
		_VirtualFrames.push_back(v2);
	}
	_Camera2Virtual.push_back(c2v);
	return 1;
}

int System::VisualizeVirtualViews(const int cidx, const string & nvmFile) {
	ofstream in(nvmFile.c_str(),ios::app);
	if (!in.is_open()) {
		cout << "Error: NVM file open failed." << endl;
		return -1;
	}
//	char tokenFirst[65536];
//	int ncam;
//	in.getline(tokenFirst, 65536);
//	in >> ncam;
//	in << ncam + (int)_Camera2Virtual[cidx].size() << endl;
//	for(int i = 0; i < ncam; i++)
//	{
//		in.getline(tokenFirst, 65536);
//		in << tokenFirst;
//	}
//
//	string a;
//	in >> a;

	for(int i = 0; i < (int)_Camera2Virtual[cidx].size(); i++)
	{
		int v_cidx = _Camera2Virtual[cidx][i];
		VirtualView v = _VirtualFrames[v_cidx];
		double q[4], cen[3], f;
		v.getC().GetQuaternionRotation(q);
		v.getC().GetCameraCenter(cen);
		f = v.getC().GetFocalLength();
		in << v.getName() << " " << f << " " << q[0] << " " << q[1] << " " << q[2]
			<< " " << q[3] << " " << cen[0] << " " << cen[1] << " " << cen[2] << " " << 0  << " "<< 0 << endl;
	}
	//in << a;
	in.close();
	return 1;
}
