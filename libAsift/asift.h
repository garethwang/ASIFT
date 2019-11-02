/**
 * @file asift.h
 * @author Gareth Wang (gareth.wang@hotmail.com)
 * @brief Interface of ASIFT feature extractor unified into OpenCV modules.
 * @version 0.1
 * @date 2019-10-30
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef _ASIFT_H_
#define _ASIFT_H_
#include <opencv2/opencv.hpp>
#include "compute_asift_keypoints.h"

//#define IM_X 800
//#define IM_Y 600

namespace asift {

	std::vector<float> ConvertImageFromMatToVector(const cv::Mat& image) {

		int width = image.cols;
		int height = image.rows;

		float* pdata;
		cv::Mat gray_image;
		if (image.channels() == 1) {
			image.copyTo(gray_image);
			gray_image.convertTo(gray_image, CV_32F);
		}
		else {
			std::vector<cv::Mat> bgr;
			cv::split(image, bgr);
			bgr[0].convertTo(bgr[0], CV_32F);
			bgr[1].convertTo(bgr[1], CV_32F);
			bgr[2].convertTo(bgr[2], CV_32F);

			gray_image = (6969 * bgr[2] + 23434 * bgr[1] + 2365 * bgr[0]) / 32768;
		}
		pdata = (float*)gray_image.data;
		std::vector<float> vec_image(pdata, pdata + width * height);

		return vec_image;
	}

	void DetectAndComputeAsift(const cv::Mat& image, std::vector<cv::KeyPoint>& keypoints,
		cv::Mat& descriptors, const bool is_resize = false) {
		std::vector<float> ipixels = ConvertImageFromMatToVector(image);
		int w, h;
		w = image.cols;
		h = image.rows;

		///// Resize the image to area wS*hW in remaining the apsect-ratio	
		///// Resize if the resize flag is true
		float wS, hS;
		bool flag_resize = false;
		if (w >= h && w > 1000) {
			wS = 1000.f;
			hS = 1000.f*h / w;
			flag_resize = true;
		}
		else if (w < h && h > 1000) {
			hS = 1000.f;
			wS = 1000.f*w / h;
			flag_resize = true;
		}

		//float wS = IM_X;
		//float hS = IM_Y;

		float zoom = 0;
		int wS1 = 0, hS1 = 0;
		vector<float> ipixels_zoom;

		if (is_resize&&flag_resize) {
			cout << "WARNING: The input image are resized to " << wS << "x" << hS << " for ASIFT. " << endl
				<< "         But the results will be normalized to the original image size." << endl << endl;

			float InitSigma_aa = 1.6f;

			float fproj_p, fproj_bg;
			char fproj_i;
			float *fproj_x4, *fproj_y4;
			int fproj_o;

			fproj_o = 3;
			fproj_p = 0;
			fproj_i = 0;
			fproj_bg = 0;
			fproj_x4 = 0;
			fproj_y4 = 0;

			float areaS = wS * hS;

			// Resize image 1 
			float area = (float)w * (float)h;
			zoom = sqrt(area / areaS);

			wS1 = (int)(w / zoom);
			hS1 = (int)(h / zoom);

			int fproj_sx = wS1;
			int fproj_sy = hS1;

			float fproj_x1 = 0;
			float fproj_y1 = 0;
			float fproj_x2 = (float)wS1;
			float fproj_y2 = 0;
			float fproj_x3 = 0;
			float fproj_y3 = (float)hS1;

			/* Anti-aliasing filtering along vertical direction */
			if (zoom > 1)
			{
				float sigma_aa = InitSigma_aa * zoom / 2;
				GaussianBlur1D(ipixels, w, h, sigma_aa, 1);
				GaussianBlur1D(ipixels, w, h, sigma_aa, 0);
			}

			// simulate a tilt: subsample the image along the vertical axis by a factor of t.
			ipixels_zoom.resize(wS1*hS1);
			fproj(ipixels, ipixels_zoom, w, h, &fproj_sx, &fproj_sy, &fproj_bg, &fproj_o, &fproj_p,
				&fproj_i, fproj_x1, fproj_y1, fproj_x2, fproj_y2, fproj_x3, fproj_y3, fproj_x4, fproj_y4);

		}
		else {
			ipixels_zoom.resize(w*h);
			ipixels_zoom = ipixels;
			wS1 = w;
			hS1 = h;
			zoom = 1;
		}

		///// Compute ASIFT keypoints
		// number N of tilts to simulate t = 1, \sqrt{2}, (\sqrt{2})^2, ..., {\sqrt{2}}^(N-1)
		int num_of_tilts = 7;

		int verb = 0;
		// Define the SIFT parameters
		siftPar siftparameters;
		default_sift_parameters(siftparameters);

		vector< vector< keypointslist > > keys;

		int num_keys = 0;

		cout << "Computing ASIFT keypoints on the image..." << endl;

		num_keys = compute_asift_keypoints(ipixels_zoom, wS1, hS1, num_of_tilts, verb, keys, siftparameters);

		keypoints.clear();
		descriptors.release();
		for (int tt = 0; tt < (int)keys.size(); tt++) {
			for (int rr = 0; rr < (int)keys[tt].size(); rr++)
			{
				keypointslist::iterator ptr = keys[tt][rr].begin();
				for (int i = 0; i < (int)keys[tt][rr].size(); i++, ptr++)
				{
					cv::KeyPoint keypt(zoom * ptr->x, zoom * ptr->y, -1, ptr->angle);
					keypoints.push_back(keypt); // Convert keypoints from user-define to OpenCV

					cv::Mat des = cv::Mat(1, (int)VecLength, CV_32F, ptr->vec).clone();
					// Normalize the descriptor
					des = des / cv::norm(des);
					descriptors.push_back(des); // Save descriptors of the keypoints
				}
			}
		}
	}
}
#endif