//============================================================================
// Name        : VirtualView.cpp
// Author      : Xushen Han
// Version     :
// Copyright   : Xushen from SJTU
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "./DBoW3/DBoW3.h"
#include "./DBoW3/DescManip.h"
#include "./DBoW3/Vocabulary.h"
#include "System.h"

using namespace std;
using namespace DBoW3;


void testVocCreation(const vector<cv::Mat> &features)
{
    // branching factor and depth levels
    const int k = 9;
    const int L =4;
    const WeightingType weight = TF_IDF;
    const ScoringType score = L2_NORM;

    DBoW3::Vocabulary voc(k, L, weight, score);

    cout << "Creating a small " << k << "^" << L << " vocabulary..." << endl;
    voc.create(features);
    cout << "... done!" << endl;

    cout << "Vocabulary information: " << endl
         << voc << endl << endl;

    // lets do something with this vocabulary
//    cout << "Matching images against themselves (0 low, 1 high): " << endl;
//    BowVector v1, v2;
//    for(size_t i = 0; i < features.size(); i++)
//    {
//        voc.transform(features[i], v1);
//        for(size_t j = 0; j < features.size(); j++)
//        {
//            voc.transform(features[j], v2);
//
//            double score = voc.score(v1, v2);
//            cout << "Image " << i << " vs Image " << j << ": " << score << endl;
//        }
//    }

    // save the vocabulary to disk
    cout << endl << "Saving vocabulary..." << endl;
    voc.save("small_voc.yml.gz");
    cout << "Done" << endl;
}

////// ----------------------------------------------------------------------------

void testDatabase(const  vector<cv::Mat > &features, const  vector<cv::Mat > &features2)
{
    cout << "Creating a small database..." << endl;

    // load the vocabulary from disk
    Vocabulary voc("small_voc.yml.gz");

    Database db(voc, false, 0); // false = do not use direct index
    // (so ignore the last param)
    // The direct index is useful if we want to retrieve the features that
    // belong to some vocabulary node.
    // db creates a copy of the vocabulary, we may get rid of "voc" now

    // add images to the database
    for(size_t i = 0; i < features.size(); i++)
        db.add(features[i]);

    cout << "... done!" << endl;

    cout << "Database information: " << endl << db << endl;

    // and query the database
    cout << "Querying the database: " << endl;

    QueryResults ret;
    for(size_t i = 0; i < features2.size(); i++)
    {
        db.query(features2[i], ret, 20);

        // ret[0] is always the same image in this case, because we added it to the
        // database. ret[1] is the second best match.

        cout << "Searching for Image " << i << ". " << ret << endl;
    }

    cout << endl;

    // we can save the database. The created file includes the vocabulary
    // and the entries added
//    cout << "Saving database..." << endl;
//    db.save("small_db.yml.gz");
//    cout << "... done!" << endl;

//    // once saved, we can load it again
//    cout << "Retrieving database once again..." << endl;
//    Database db2("small_db.yml.gz");
//    cout << "... done! This is: " << endl << db2 << endl;
}

int main() {
	System s;
	s.VisualSFM("/mnt/hgfs/Shared/MediaSoc/dataset/sequences/KeyFrames/");
	s.LoadNVM();
	s.MakeCameraView();
	s.MakeVirtualView("/mnt/hgfs/Shared/MediaSoc/dataset/sequences/KeyFrames/sfm2.nvm",30);
	cout << "End" << endl;

//	vector< cv::Mat   >   features;
//	for(int i = 0; i < (int)s._KeyFrames.size(); i++)
//	{
//		for(int j = 0; j < (int)s._KeyFrames.size(); j++)
//		{
//			if(i == s._KeyFrames[j]._cidx)
//				features.push_back(s._KeyFrames[j]._features);
//		}
//	}
//	if(features.size() != s._KeyFrames.size())
//	{
//		cout << "Here, Error." << endl;
//		exit(-1);
//	}
//
//	for(int i = 0; i < (int)s._KeyFrames.size(); i++)
//	{
//		for(int j = 0; j < (int)s._VirtualFrames.size(); j++)
//		{
//			if(i == s._VirtualFrames[j]._cidx && i >= 2 )
//				features.push_back(s._VirtualFrames[j]._features);
//		}
//	}
//	testVocCreation(features);
//
//
//
//	System s1;
//	s1.VisualSFM("/mnt/hgfs/Shared/MediaSoc/dataset/sequences/KeyFrames_lying/");
//	s1.LoadNVM();
//	s1.MakeCameraView();
//	//s.MakeVirtualView("/mnt/hgfs/Shared/MediaSoc/dataset/sequences/KeyFrames/sfm2.nvm",30);
//	vector< cv::Mat   >   features2;
//	for(int i = 0; i < (int)s1._KeyFrames.size(); i++)
//	{
//		for(int j = 0; j < (int)s1._KeyFrames.size(); j++)
//		{
//			if(i == s1._KeyFrames[j]._cidx)
//				features2.push_back(s1._KeyFrames[j]._features);
//		}
//	}
//	testDatabase(features, features2);

	return 0;
}

