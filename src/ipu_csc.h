/*
 * Copyright 2014 Jacob Aslund <jacob@itbuster.dk>
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
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
