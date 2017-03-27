//============================================================================
// Name        : VirtualView.cpp
// Author      : Xushen Han
// Version     :
// Copyright   : Xushen from SJTU
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "../DBoW3/DBoW3.h"
#include "../DBoW3/DescManip.h"
#include "System.h"

using namespace std;
using namespace DBoW3;

int main() {
	System s;
	s.VisualSFM("/mnt/hgfs/Shared/MediaSoc/dataset/sequences/KeyFrames/");
	s.LoadNVM();
	s.MakeCameraView();
	s.MakeVirtualView("/mnt/hgfs/Shared/MediaSoc/dataset/sequences/KeyFrames/sfm2.nvm",30);
	cout << "End" << endl;
	return 0;
}

