/*
* Copyright 2014 jafp, Inc. All Rights Reserved.
*/

/*
* The code contained herein is licensed under the GNU Lesser General
* Public License. You may obtain a copy of the GNU Lesser General
* Public License Version 2.1 or later at the following locations:
*
* http://www.opensource.org/licenses/lgpl-license.html
* http://www.gnu.org/copyleft/lgpl.html
*/

#ifndef IPU_CSC_H_
#define IPU_CSC_H_

#include <linux/ipu.h>

typedef unsigned char pix_t;

typedef struct {
	int width;
	int height;
	int bpp;
	int fmt;
} ipu_csc_format_t;	

typedef struct {
	int fd;
	struct ipu_task task;
	int output_size;
	pix_t* output_buffer;
	ipu_csc_format_t* output_format;
	ipu_csc_format_t* input_format;
} ipu_csc_t;

int ipu_csc_init(ipu_csc_t* ipu_csc, ipu_csc_format_t* input_format,
	ipu_csc_format_t* output_format);

int ipu_csc_close(ipu_csc_t* ipu_csc);

int ipu_csc_convert(ipu_csc_t* ipu_csc, const pix_t* input, pix_t* output);

#endif
