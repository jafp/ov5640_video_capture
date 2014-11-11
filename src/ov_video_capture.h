/*
 * Copyright 2014 Jacob Aslund <jacob@itbuster.dk>
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef _OV_VIDEO_CAPTURE_H_
#define _OV_VIDEO_CAPTURE_H_

#include <opencv2/core/core.hpp>
#include <linux/videodev2.h>

namespace jafp {

struct OvVideoMode {
	int width;
	int height;
	int framerate;
	int capture_mode;
};

struct OvFrameBuffer {
	unsigned char* start;
	unsigned int length;
	unsigned int offset;
};

class OvVideoCapture {
public:
	
	// Various constants
	static const int NumBuffers = 4;
	static const int DefaultInputNo = 1;
	static const int DefaultFormat = V4L2_PIX_FMT_UYVY;
	static const int DefaultFormatChannels = 2;
	
	// Modes
	static const OvVideoMode OV_MODE_640_480_30;
	static const OvVideoMode OV_MODE_320_240_30;
	
	OvVideoCapture(const OvVideoMode& mode = OV_MODE_320_240_30);
	virtual ~OvVideoCapture();
	
	// Opens the device (OV5640 sensor connected to the MIPI CSI2 channel).
	// No parameters are given, as we expect the device to have a fixed device id
	bool open();
	
	// Relase the device. 
	// Free'es the internal frame buffer.
	bool release();
	
	// Returns true if the device already has been opened. 
	inline bool isOpened() const { return is_opened_; }
	
	// Grabs a single frame from the image sensor
	bool grab();
	
	// Decodes the grabbed video frame and returns it to the given
	// image structure.
	bool retrieve(cv::Mat& image);
	
	// Grabs, decodes and returns the grabbed image.
	bool read(cv::Mat& image);

	// Short hand for `read`, to use the capture device liek `cin`.
	inline OvVideoCapture& operator >> (cv::Mat& image) {
		read(image);
		return (*this);
	}

private:
	int fd_;
	int current_buffer_index_;
	int frame_size_;
	bool is_opened_;

	OvFrameBuffer buffers_[NumBuffers];
	const OvVideoMode& mode_;

	bool open_internal();
	bool start_capturing();

};

}


#endif
