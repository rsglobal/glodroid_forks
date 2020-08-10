/*
 * Copyright 2016 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef CROS_GRALLOC_HANDLE_H
#define CROS_GRALLOC_HANDLE_H

#include <stdint.h>
#include <cutils/native_handle.h>

#define DRV_MAX_PLANES 4
#define DRV_MAX_FDS (DRV_MAX_PLANES + 1)

struct cros_gralloc_handle {
	native_handle_t base;
	/*
	 * File descriptors must immediately follow the native_handle_t base and used file
	 * descriptors must be packed at the beginning of this array to work with
	 * native_handle_clone().
	 *
	 * This field contains 'num_planes' plane file descriptors followed by an optional metadata
	 * reserved region file descriptor if 'reserved_region_size' is greater than zero.
	 */
	int32_t fds[DRV_MAX_FDS];
	uint32_t strides[DRV_MAX_PLANES];
	uint32_t offsets[DRV_MAX_PLANES];
	uint32_t sizes[DRV_MAX_PLANES];
	uint32_t id;
	uint32_t width;
	uint32_t height;
	uint32_t format; /* DRM format */
	uint64_t format_modifier;
	uint64_t use_flags; /* Buffer creation flags */
	uint32_t magic;
	uint32_t pixel_stride;
	int32_t droid_format;
	int32_t usage; /* Android usage. */
	uint32_t num_planes;
	uint64_t reserved_region_size;
	uint64_t total_size; /* Total allocation size */
	/*
	 * Name is a null terminated char array located at handle->base.data[handle->name_offset].
	 */
	uint32_t name_offset;
} __attribute__((packed));

typedef const struct cros_gralloc_handle *cros_gralloc_handle_t;

#define CROS_GRALLOC_MAGIC 0xABCDDCBA

#define GRALLOC_HANDLE_NUM_FDS 1
#define GRALLOC_HANDLE_NUM_INTS (	\
	((sizeof(struct cros_gralloc_handle) - sizeof(native_handle_t))/sizeof(int))	\
	 - GRALLOC_HANDLE_NUM_FDS)

static inline struct cros_gralloc_handle *to_cros_gralloc_handle(buffer_handle_t handle)
{
	return (struct cros_gralloc_handle *)handle;
}

/**
 * Create a buffer handle.
 */
static inline native_handle_t *cros_gralloc_handle_create(int32_t width,
                                                          int32_t height,
                                                          int32_t hal_format,
                                                          int32_t usage)
{
	struct cros_gralloc_handle *handle;
	native_handle_t *nhandle = native_handle_create(GRALLOC_HANDLE_NUM_FDS,
							GRALLOC_HANDLE_NUM_INTS);

	if (!nhandle)
		return NULL;

	handle = to_cros_gralloc_handle(nhandle);
	handle->magic = CROS_GRALLOC_MAGIC;
	handle->width = width;
	handle->height = height;
	handle->droid_format = hal_format;
	handle->usage = usage;
	handle->fds[0] = -1;

	return nhandle;
}


#endif
