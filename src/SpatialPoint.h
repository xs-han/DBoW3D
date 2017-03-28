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
	SpatialPoint(int _PointId, Point3d pt3d,
			vector<int> CameraIdx,
			vector<int> FeatureIdx);
	virtual ~SpatialPoint();
	const vector<int>& getCameraIdx() const;
	const vector<int>& getFeatureIdx() const;
	int getPointId() const;
	const Matx31d& getPt3d() const;

public:
	int _PointId;
	Matx31d _pt3d;
	vector<int> _CameraIdx;
	vector<int> _FeatureIdx;
};

#endif /* SPATIALPOINT_H_ */
