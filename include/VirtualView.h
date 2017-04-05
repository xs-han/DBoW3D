/*
 * VirtualView.h
 *
 *  Created on: Mar 21, 2017
 *      Author: xushen
 */

#ifndef VIRTUALVIEW_H_
#define VIRTUALVIEW_H_

#include "View.h"

class VirtualView: public View {
public:
	VirtualView();
	VirtualView(const string & name,
			int cidx,
			const Mat & img,
			const vector<cv::KeyPoint> key,
			const Mat & desc,
			double y);
	virtual ~VirtualView();
	int CreateVirtualFeatures(int nfeatures = 0);

private:
	double _yaw;
};

#endif /* VIRTUALVIEW_H_ */
