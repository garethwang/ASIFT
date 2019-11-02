#include <iostream>
#include <opencv2/opencv.hpp>
#include "libAsift/asift.h"

int main() {
	std::string str_img1 = std::string(SOURCE_DIR) + "/data/graffA.jpg";
	std::string str_img2 = std::string(SOURCE_DIR) + "/data/graffB.jpg";
	
	cv::Mat img1 = cv::imread(str_img1);
	cv::Mat img2 = cv::imread(str_img2);
	
	std::vector<cv::KeyPoint> kpts1, kpts2;
	cv::Mat des1, des2;

	asift::DetectAndComputeAsift(img1, kpts1, des1, true);
	asift::DetectAndComputeAsift(img2, kpts2, des2, true);


	std::cout << "Size of des1: [" << des1.cols << "*" << des1.rows << "]\n";
	std::cout << "Size of des2: [" << des2.cols << "*" << des2.rows << "]\n";

	cv::FlannBasedMatcher fbmatcher;
	std::vector< std::vector< cv::DMatch > > matches;
	fbmatcher.knnMatch(des1, des2, matches, 2);

	std::vector<cv::DMatch> good_matches;
	for (size_t i = 0; i < matches.size(); ++i) {
		if (matches[i].size() >= 2) {
			if (matches[i][0].distance < 0.7*matches[i][1].distance) {
				good_matches.push_back(matches[i][0]);
			}
		}
	}
	
	cv::Mat img_match;
	cv::drawMatches(img1, kpts1, img2, kpts2, good_matches, img_match, cv::Scalar::all(-1),
		cv::Scalar::all(-1), vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	cv::imshow("Image Matching(ASIFT)", img_match);	
	cv::waitKey(0);

	return 0;
}

