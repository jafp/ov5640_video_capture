/*
 * Copyright 2014 Jacob Aslund <jacob@itbuster.dk>
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "ipu_csc.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

static int alloc_and_map_output(ipu_csc_t* ipu_csc) {
	ipu_csc_format_t* fmt = ipu_csc->output_format;

	ipu_csc->output_size = fmt->width * fmt->height * fmt->bpp / 8;
	ipu_csc->task.output.width = fmt->width;
	ipu_csc->task.output.height = fmt->height;
	ipu_csc->task.output.format = fmt->fmt;
	ipu_csc->task.output.paddr = ipu_csc->output_size;

	// Allocate output memory
	if (ioctl(ipu_csc->fd, IPU_ALLOC, &ipu_csc->task.output.paddr) < 0) {
		return -1;
	}

	// Create memory map for output image
	ipu_csc->output_buffer = (unsigned char* ) mmap(0, ipu_csc->output_size, 
		PROT_READ | PROT_WRITE, MAP_SHARED, ipu_csc->fd, ipu_csc->task.output.paddr);
	if (!ipu_csc->output_buffer) {
		return -1;
	}
	return 0;
}

int ipu_csc_init(ipu_csc_t* ipu_csc, ipu_csc_format_t* input_format,
	ipu_csc_format_t* output_format) {

	memset(ipu_csc, 0, sizeof(*ipu_csc));
	memset(&ipu_csc->task, 0, sizeof(ipu_csc->task));

	ipu_csc->input_format = input_format;
	ipu_csc->output_format = output_format;

	ipu_csc->fd = open("/dev/mxc_ipu", O_RDWR, 0);
	if (ipu_csc->fd < 0) {
		return -1;
	}
	
	if (alloc_and_map_output(ipu_csc) != 0) {
		return -1;
	}
	
	return 0;
}

int ipu_csc_close(ipu_csc_t* ipu_csc) {
	if (ipu_csc->fd) {
		close(ipu_csc->fd);
	}
	if (ipu_csc->output_buffer) {
		munmap(ipu_csc->output_buffer, ipu_csc->output_size);
	}
	if (ipu_csc->task.output.paddr) {
		ioctl(ipu_csc->fd, IPU_FREE, &ipu_csc->task.output.paddr);
	}
	return 0;
}

int ipu_csc_convert(ipu_csc_t* ipu_csc, const pix_t* input, pix_t* output) {

	ipu_csc_format_t* input_format = ipu_csc->input_format;

	int inp_size = (input_format->width * input_format->height * input_format->bpp) / 8;
	void * inp_buf;

	ipu_csc->task.input.width = input_format->width;
	ipu_csc->task.input.height = input_format->height;
	ipu_csc->task.input.format = input_format->fmt;
	ipu_csc->task.input.paddr = inp_size;

	// Allocate contingous physical memory for input image
	// input.paddr contains the amount needed
	// this value will be replaced with physical address on success
	if (ioctl(ipu_csc->fd, IPU_ALLOC, &ipu_csc->task.input.paddr) < 0) {
		perror("ioctl IPU_ALLOC fail");
		return -1;
	}

	// Create memory map and obtain the allocated memory virtual address
	inp_buf = mmap(0, inp_size, PROT_READ | PROT_WRITE, MAP_SHARED, 
		ipu_csc->fd, ipu_csc->task.input.paddr);
	if (!inp_buf) {
		perror("mmap fail\n");
		return  -1;
	}

	memcpy(inp_buf, input, inp_size);

	// Perform color space conversion 
	if (ioctl(ipu_csc->fd, IPU_QUEUE_TASK, &ipu_csc->task) < 0)  {
		perror("ioct IPU_QUEUE_TASK fail");
		return -1;
	}

	memcpy(output, ipu_csc->output_buffer, ipu_csc->output_size);

	if (inp_buf) {
		munmap(inp_buf, inp_size);
	}
	if (ipu_csc->task.input.paddr) {
		ioctl(ipu_csc->fd, IPU_FREE, &ipu_csc->task.input.paddr);
	}
	return 0;
}
