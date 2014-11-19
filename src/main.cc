/*
 * Copyright 2014 Jacob Aslund <jacob@itbuster.dk>
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include "ov_video_capture.h"

using namespace std;

int main() {
	
	// Create Ov5640 capture device with 320x240 @ 30 fps format
	jafp::OvVideoCapture capture(jafp::OvVideoCapture::OV_MODE_640_480_30);

	if (!capture.open()) {
		perror("open");
		return -1;
	}

	cv::Mat frame;

	// Capture 30 frames
	for (int i = 0; i < 30; i++) {
		capture.read(frame);
		std::cout << "frame " << i << std::endl;

		std::stringstream str;
		str << "frames/frame_" << i << ".png";
		cv::imwrite(str.str(), frame);
	}
	
	capture.release();

	return 0;
}