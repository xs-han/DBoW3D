/*
 * VirtualView.cpp
 *
 *  Created on: Mar 21, 2017
 *      Author: xushen
 */

#include "VirtualView.h"

VirtualView::VirtualView():_yaw(-1) {
	// TODO Auto-generated constructor stub

}

VirtualView::~VirtualView() {
	// TODO Auto-generated destructor stub
}

VirtualView::VirtualView(const string & name,
			int cidx,
			const Mat & img,
			const vector<cv::KeyPoint> key,
			const Mat & desc,
			double y)//:
			//_name(name), _cidx(cidx),_img(img),
			//_features(desc), _yaw(y)
{
	_name = name;
	_cidx = cidx;
	_img = img;
	_features = desc;
	_yaw = y;
	for(int i = 0; i < (int)key.size(); i++)
	{
		_kps.push_back(KeyPoint( key[i].pt,
											key[i].size,
											key[i].angle,
											key[i].response,
											key[i].octave,
											key[i].class_id));
	}
}

int VirtualView::CreateVirtualFeatures(int nfeatures) {
	return 1;
}
