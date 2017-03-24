/*
 * SpatialPoint.h
 *
 *  Created on: Mar 21, 2017
 *      Author: xushen
 */

#ifndef SPATIALPOINT_H_
#define SPATIALPOINT_H_

#include <iostream>
#include <opencv2/core/core.hpp>

using namespace std;
using namespace cv;


class SpatialPoint {
public:
	SpatialPoint();
	int Create();
	virtual ~SpatialPoint();

private:
	Point3d _pt3d;
	vector<int> _CameraIdx;
	vector<int> _FeatureIdx;
};

#endif /* SPATIALPOINT_H_ */
