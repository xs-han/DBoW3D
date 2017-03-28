/*
 * SpatialPoint.cpp
 *
 *  Created on: Mar 21, 2017
 *      Author: xushen
 */

#include "SpatialPoint.h"

SpatialPoint::SpatialPoint():_PointId(-1) {
	// TODO Auto-generated constructor stub

}

SpatialPoint::~SpatialPoint() {
	// TODO Auto-generated destructor stub
}

SpatialPoint::SpatialPoint(int PointId, Point3d pt3d, vector<int> CameraIdx,
		vector<int> FeatureIdx):
				_PointId(PointId),
				_pt3d(pt3d.x, pt3d.y, pt3d.z),
				_CameraIdx(CameraIdx),
				_FeatureIdx(FeatureIdx)
{

}

const vector<int>& SpatialPoint::getCameraIdx() const {
	return _CameraIdx;
}

const vector<int>& SpatialPoint::getFeatureIdx() const {
	return _FeatureIdx;
}

int SpatialPoint::getPointId() const {
	return _PointId;
}

const Matx31d& SpatialPoint::getPt3d() const {
	return _pt3d;
}
// Create point from 3D coordinate and corresponding 2D key points

