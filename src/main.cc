
#include <iostream>
#include "ov_video_capture.h"

using namespace std;

int main() {
	
	// Create Ov5640 capture device with 320x240 @ 30 fps format
	jafp::OvVideoCapture capture(jafp::OvVideoCapture::OV_MODE_320_240_30);

	if (!capture.open()) {
		perror("open");
		return -1;
	}

	cv::Mat frame;

	// Capture 30 frames
	for (int i = 0; i < 30; i++) {
		capture >> frame;
	}
	
	capture.release();

	return 0;
}