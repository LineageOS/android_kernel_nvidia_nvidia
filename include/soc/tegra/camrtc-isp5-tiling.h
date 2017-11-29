/*
 * Copyright (c) 2017 NVIDIA CORPORATION.  All rights reserved.
 *
 * NVIDIA CORPORATION and its licensors retain all intellectual property
 * and proprietary rights in and to this software, related documentation
 * and any modifications thereto.  Any use, reproduction, disclosure or
 * distribution of this software and related documentation without an express
 * license agreement from NVIDIA CORPORATION is strictly prohibited.
 */

#include "camrtc-capture.h"
#include <stdbool.h>
#include <stdint.h>


struct isp5_tile_width {
	uint16_t tile_width_first;
	uint16_t tile_width_middle;
	uint16_t tiles_in_slice;
};

struct isp5_slice_height {
	uint16_t slice_height;
	uint16_t vi_first_slice_height;
	uint16_t slices_in_image;
};

static const uint16_t ISP5_MIN_TILE_WIDTH = 32U;
static const uint16_t ISP5_MAX_TILE_WIDTH = 1024U;
static const uint16_t ISP5_MIN_SLICE_HEIGHT = 32U;
static const uint16_t ISP5_MAX_SLICE_HEIGHT = 540U;

#ifndef max
#define max(x, y) (((x) > (y)) ? (x) : (y))
#endif

#ifndef min
#define min(x, y) (((x) < (y)) ? (x) : (y))
#endif

static inline uint16_t align_down(uint16_t val, uint16_t alignment)
{
	uint16_t rem = (val % alignment);

	return (rem > 0) ? (val - rem) : val;
}

static inline uint16_t align_up(uint16_t val, uint16_t alignment)
{
	uint16_t rem = (val % alignment);

	return (rem > 0) ? (align_down(val, alignment) + alignment) : val;
}

static inline uint16_t div_round_up(uint16_t x, uint16_t y)
{
	return (x + y - 1) / y;
}

/**
 * Calculate suitable tile width for given capture descriptor and ISP program
 */
__attribute__((unused))
static bool isp5_find_tile_width(const struct isp5_program *prg,
						const struct isp_capture_descriptor *cd,
						struct isp5_tile_width *tiling)
{
	const uint16_t img_width = cd->surface_configs.mr_width;

	if (img_width <= ISP5_MAX_TILE_WIDTH) {
		tiling->tile_width_first = img_width;
		tiling->tile_width_middle = 0U;
		tiling->tiles_in_slice = 1;
		return true;
	}

	const uint16_t alignment = prg->overfetch.alignment;
	const uint16_t max_width_first = align_down(ISP5_MAX_TILE_WIDTH -
							prg->overfetch.right +
							prg->overfetch.pru_ovf_h, alignment) -
					prg->overfetch.right + prg->overfetch.pru_ovf_h;

	const uint16_t max_width_middle = align_down(ISP5_MAX_TILE_WIDTH -
							prg->overfetch.right -
							prg->overfetch.left,
							alignment);

	/* Last tile right edge does not need to be aligned */
	const uint16_t max_width_last = ISP5_MAX_TILE_WIDTH - prg->overfetch.left;
	const uint16_t min_width = max(ISP5_MIN_TILE_WIDTH, prg->overfetch.right);

	uint16_t tile_count = 2;

	if (img_width > max_width_first + max_width_last) {
		const uint16_t pixels_left = img_width - max_width_first - max_width_last;
		const uint16_t middle_tiles = div_round_up(pixels_left,
							min(max_width_middle, max_width_first));
		tile_count += middle_tiles;
	}

	/* Divide image into roughly evenly spaced aligned tiles */
	uint16_t tile_width = (div_round_up(img_width, alignment) / tile_count) * alignment;

	/* The right edge of a tile as seen by AP must be aligned
	 * correctly for CAR filter.  When first tile width fulfills
	 * this condition, the rest of tiles are simple to andle by
	 * just aligning their active width
	 */
	uint16_t first_width = min(max_width_first,
					align_down(
					tile_width + prg->overfetch.right -
					prg->overfetch.pru_ovf_h, alignment) -
					prg->overfetch.right + prg->overfetch.pru_ovf_h);
	uint16_t middle_width = (tile_count > 2) ? min(max_width_middle, tile_width) : 0U;
	uint16_t last_width = img_width - first_width - (tile_count - 2) * middle_width;

	/* Ensure that last tile is wide enough. Width of the first
	 * tile a this point is guaranteed to be greater than:
	 *
	 * ((max_tile_width - total overfetch - 2*alignment) / 2) - alignment >= 407 pixels
	 *
	 * So there is no risk that this correction will cause it to
	 * be too narrow.
	 */
	if (last_width < min_width) {
		uint16_t corr = align_up(min_width-last_width, alignment);

		first_width -= corr;
		last_width  += corr;

	} else if (last_width > max_width_last) {
		const uint16_t corr = last_width - max_width_last;

		/* Try first increasing middle tile width */
		if (tile_count > 2) {
			const uint16_t max_middle_corr = max_width_middle - middle_width;
			const uint16_t middle_corr =
					min(max_middle_corr,
						align_up(div_round_up(corr, tile_count-2),
							alignment));
			middle_width += middle_corr;
			last_width -= middle_corr * (tile_count-2);
		}

		if (last_width > max_width_last) {
			const uint first_corr = align_up(last_width - max_width_last, alignment);

			first_width += first_corr;
			last_width -= first_corr;
		}
	}

	if (first_width < min_width) {
		return false;
	}

	if (first_width > max_width_first) {
		return false;
	}

	if (last_width < min_width) {
		return false;
	}

	if (last_width > max_width_last) {
		return false;
	}

	if (tile_count > 2U) {

		if (middle_width < min_width) {
			return false;
		}

		if (middle_width > max_width_middle) {
			return false;
		}
	}

	tiling->tile_width_first = first_width;
	tiling->tile_width_middle = middle_width;
	tiling->tiles_in_slice = tile_count;

	return true;
}

__attribute__((unused))
static bool isp5_find_tile_width_dpcm(const struct isp5_program *prg,
					const struct isp_capture_descriptor *cd,
					struct isp5_tile_width *tiling)
{
	const uint16_t alignment = max(prg->overfetch.alignment, 8);
	const uint16_t max_width_first = align_down(ISP5_MAX_TILE_WIDTH - prg->overfetch.right,
						alignment);
	const uint16_t max_width_middle = align_down(ISP5_MAX_TILE_WIDTH - prg->overfetch.right -
						prg->overfetch.left,
						alignment);
	const uint16_t max_width_last = ISP5_MAX_TILE_WIDTH - prg->overfetch.left;
	const uint16_t min_width = max(ISP5_MIN_TILE_WIDTH, prg->overfetch.right);

	if (cd->surface_configs.chunk_width_middle > max_width_middle) {
		return false;
	}

	tiling->tile_width_middle = cd->surface_configs.chunk_width_middle;

	/* Width of first tile must set so that left overfetch area of
	 * 2nd tile fits into 2nd chunk.
	 */
	tiling->tile_width_first = align_up(cd->surface_configs.chunk_width_first +
						prg->overfetch.left +
						prg->overfetch.right -
						prg->overfetch.pru_ovf_h, alignment) -
					prg->overfetch.right +
					prg->overfetch.pru_ovf_h;

	if (tiling->tile_width_first < min_width) {
		return false;
	}

	if (tiling->tile_width_first > max_width_first) {
		return false;
	}

	if (tiling->tile_width_first + prg->overfetch.right >
		cd->surface_configs.chunk_width_first + cd->surface_configs.chunk_overfetch_width) {
		return false;
	}

	tiling->tiles_in_slice = 1U + div_round_up(cd->surface_configs.mr_width -
						cd->surface_configs.chunk_width_first,
						cd->surface_configs.chunk_width_middle);
	const uint16_t last_width = cd->surface_configs.mr_width -
					tiling->tile_width_first -
					(tiling->tiles_in_slice - 1) * tiling->tile_width_middle;

	if (last_width < min_width) {
		return false;
	}

	if (last_width > max_width_last) {
		return false;
	}

	return true;
}

__attribute__((unused))
static bool isp5_find_slice_height(uint16_t img_height,
				struct isp5_slice_height *slicing)
{
	if (img_height < ISP5_MIN_SLICE_HEIGHT) {
		return false;
	}

	if (img_height % 2 != 0U) {
		return false;
	}

	if (img_height <= ISP5_MAX_SLICE_HEIGHT) {
		slicing->slices_in_image = U16_C(1);
		slicing->slice_height = img_height;
		slicing->vi_first_slice_height = img_height;
		return true;
	}

	uint16_t slice_height = ISP5_MAX_SLICE_HEIGHT;
	uint16_t slice_count = div_round_up(img_height, ISP5_MAX_SLICE_HEIGHT);
	uint16_t last_height = img_height - ISP5_MAX_SLICE_HEIGHT * (slice_count - 1);

	if (last_height < ISP5_MIN_SLICE_HEIGHT) {
		const uint16_t corr = ISP5_MIN_SLICE_HEIGHT - last_height;
		const uint16_t slice_corr = align_up(div_round_up(corr, slice_count - 1), 2U);

		slice_height -= slice_corr;
	}

	slicing->slice_height = slice_height;
	slicing->slices_in_image = slice_count;
	slicing->vi_first_slice_height = (slice_count == 1U) ? slice_height : slice_height + 18U;

	return true;
}
