/*
 * tegra210_mixer_alt.h - Definitions for Tegra210 MIXER driver
 *
 * Copyright (c) 2015-2019, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHIN
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __TEGRA210_MIXER_ALT_H__
#define __TEGRA210_MIXER_ALT_H__

#define TEGRA210_MIXER_AXBAR_RX_STRIDE					0x40
#define TEGRA210_MIXER_AXBAR_RX_MAX					10
#define TEGRA210_MIXER_AXBAR_RX_LIMIT		(TEGRA210_MIXER_AXBAR_RX_MAX * TEGRA210_MIXER_AXBAR_RX_STRIDE)

/* AXBAR_RX related MIXER offsets */

#define TEGRA210_MIXER_AXBAR_RX1_SOFT_RESET				0x04
#define TEGRA210_MIXER_AXBAR_RX1_STATUS					0x10
#define TEGRA210_MIXER_AXBAR_RX1_CIF_CTRL				0x24
#define TEGRA210_MIXER_AXBAR_RX1_CTRL					0x28
#define TEGRA210_MIXER_AXBAR_RX1_PEAK_CTRL				0x2c
#define TEGRA210_MIXER_AXBAR_RX1_SAMPLE_COUNT			0x30
#define TEGRA210_MIXER_AXBAR_RX1_CYA					0x34
#define TEGRA210_MIXER_AXBAR_RX1_DBG0					0x38
#define TEGRA210_MIXER_AXBAR_RX1_DBG1					0x3c

#define TEGRA210_MIXER_AXBAR_TX_STRIDE					0x40
#define TEGRA210_MIXER_AXBAR_TX_MAX					5
#define TEGRA210_MIXER_AXBAR_TX_LIMIT		(TEGRA210_MIXER_AXBAR_RX_LIMIT + (TEGRA210_MIXER_AXBAR_TX_MAX * TEGRA210_MIXER_AXBAR_TX_STRIDE))

/* AXBAR_TX related MIXER offsets */

#define TEGRA210_MIXER_AXBAR_TX1_ENABLE					0x280
#define TEGRA210_MIXER_AXBAR_TX1_SOFT_RESET				0x284
#define TEGRA210_MIXER_AXBAR_TX1_STATUS					0x290
#define TEGRA210_MIXER_AXBAR_TX1_INT_STATUS				0x294
#define TEGRA210_MIXER_AXBAR_TX1_INT_MASK				0x298
#define TEGRA210_MIXER_AXBAR_TX1_INT_SET				0x29c
#define TEGRA210_MIXER_AXBAR_TX1_INT_CLEAR				0x2a0
#define TEGRA210_MIXER_AXBAR_TX1_CIF_CTRL				0x2a4
#define TEGRA210_MIXER_AXBAR_TX1_ADDER_CONFIG			0x2a8
#define TEGRA210_MIXER_AXBAR_TX1_CYA					0x2ac
#define TEGRA210_MIXER_AXBAR_TX1_DBG0					0x2b0
#define TEGRA210_MIXER_AXBAR_TX1_DBG1					0x2b4

/* MIXER related offsets */
#define TEGRA210_MIXER_ENABLE								0x400
#define TEGRA210_MIXER_SOFT_RESET							0x404
#define TEGRA210_MIXER_CG									0x408
#define TEGRA210_MIXER_STATUS								0x410
#define TEGRA210_MIXER_INT_STATUS							0x414
#define TEGRA210_MIXER_AHUBRAMCTL_GAIN_CONFIG_RAM_CTRL		0x42c
#define TEGRA210_MIXER_AHUBRAMCTL_GAIN_CONFIG_RAM_DATA		0x430
#define TEGRA210_MIXER_AHUBRAMCTL_PEAKM_RAM_CTRL			0x434
#define TEGRA210_MIXER_AHUBRAMCTL_PEAKM_RAM_DATA			0x438
#define TEGRA210_MIXER_CTRL									0x43c
#define TEGRA210_MIXER_CYA									0x440
#define TEGRA210_MIXER_DBG									0x448

#define TEGRA210_MIXER_AXBAR_TX_ENABLE_SHIFT			0
#define TEGRA210_MIXER_AXBAR_TX_EN						(1 << TEGRA210_MIXER_AXBAR_TX_ENABLE_SHIFT)

/* TODO Add fields for MIXER_AXBAR_TX1_ADDER_CONFIG register */
#define TEGRA210_MIXER_AXBAR_TX2_ADDER_CONFIG	(TEGRA210_MIXER_AXBAR_TX1_ADDER_CONFIG + TEGRA210_MIXER_AXBAR_TX_STRIDE)
#define TEGRA210_MIXER_AXBAR_TX3_ADDER_CONFIG	(TEGRA210_MIXER_AXBAR_TX2_ADDER_CONFIG + TEGRA210_MIXER_AXBAR_TX_STRIDE)
#define TEGRA210_MIXER_AXBAR_TX4_ADDER_CONFIG	(TEGRA210_MIXER_AXBAR_TX3_ADDER_CONFIG + TEGRA210_MIXER_AXBAR_TX_STRIDE)
#define TEGRA210_MIXER_AXBAR_TX5_ADDER_CONFIG	(TEGRA210_MIXER_AXBAR_TX4_ADDER_CONFIG + TEGRA210_MIXER_AXBAR_TX_STRIDE)

#define TEGRA210_MIXER_AXBAR_TX2_ENABLE	(TEGRA210_MIXER_AXBAR_TX1_ENABLE + TEGRA210_MIXER_AXBAR_TX_STRIDE)
#define TEGRA210_MIXER_AXBAR_TX3_ENABLE	(TEGRA210_MIXER_AXBAR_TX2_ENABLE + TEGRA210_MIXER_AXBAR_TX_STRIDE)
#define TEGRA210_MIXER_AXBAR_TX4_ENABLE	(TEGRA210_MIXER_AXBAR_TX3_ENABLE + TEGRA210_MIXER_AXBAR_TX_STRIDE)
#define TEGRA210_MIXER_AXBAR_TX5_ENABLE	(TEGRA210_MIXER_AXBAR_TX4_ENABLE + TEGRA210_MIXER_AXBAR_TX_STRIDE)

/* Fields in TEGRA210_MIXER_ENABLE */
#define TEGRA210_MIXER_ENABLE_SHIFT						0
#define TEGRA210_MIXER_ENABLE_MASK						(1 << TEGRA210_MIXER_ENABLE_SHIFT)
#define TEGRA210_MIXER_EN								(1 << TEGRA210_MIXER_ENABLE_SHIFT)

/* Fields in TEGRA210_MIXER_CTRL */
#define TEGRA210_MIXER_CTRL_ENABLE_BYPASS_MODE		1
#define TEGRA210_MIXER_CTRL_DISABLE_BYPASS_MODE		0

/* Fields in TEGRA210_MIXER_AHUBRAMCTL_GAIN_CONFIG_RAM_CTRL */
#define TEGRA210_MIXER_AHUBRAMCTL_GAIN_CONFIG_RAM_ADDR_0			0x0
#define TEGRA210_MIXER_AHUBRAMCTL_GAIN_CONFIG_RAM_ADDR_STRIDE	0x10

#define TEGRA210_MIXER_AHUBRAMCTL_GAIN_CONFIG_RAM_CTRL_READ_BUSY_SHIFT	31
#define TEGRA210_MIXER_AHUBRAMCTL_GAIN_CONFIG_RAM_CTRL_READ_BUSY_MASK		(1 << TEGRA210_MIXER_AHUBRAMCTL_GAIN_CONFIG_RAM_CTRL_READ_BUSY_SHIFT)
#define TEGRA210_MIXER_AHUBRAMCTL_GAIN_CONFIG_RAM_CTRL_READ_BUSY		(1 << TEGRA210_MIXER_AHUBRAMCTL_GAIN_CONFIG_RAM_CTRL_READ_BUSY_SHIFT)

#define TEGRA210_MIXER_AHUBRAMCTL_GAIN_CONFIG_RAM_CTRL_SEQ_READ_COUNT_SHIFT	16
#define TEGRA210_MIXER_AHUBRAMCTL_GAIN_CONFIG_RAM_CTRL_SEQ_READ_COUNT_MASK	(0xff << TEGRA210_MIXER_AHUBRAMCTL_GAIN_CONFIG_RAM_CTRL_SEQ_READ_COUNT_SHIFT)

#define TEGRA210_MIXER_AHUBRAMCTL_GAIN_CONFIG_RAM_CTRL_RW_SHIFT		14
#define TEGRA210_MIXER_AHUBRAMCTL_GAIN_CONFIG_RAM_CTRL_RW_MASK			(1 << TEGRA210_MIXER_AHUBRAMCTL_GAIN_CONFIG_RAM_CTRL_RW_SHIFT)
#define TEGRA210_MIXER_AHUBRAMCTL_GAIN_CONFIG_RAM_CTRL_RW_WRITE		(1 << TEGRA210_MIXER_AHUBRAMCTL_GAIN_CONFIG_RAM_CTRL_RW_SHIFT)

#define TEGRA210_MIXER_AHUBRAMCTL_GAIN_CONFIG_RAM_CTRL_ADDR_INIT_EN_SHIFT	13
#define TEGRA210_MIXER_AHUBRAMCTL_GAIN_CONFIG_RAM_CTRL_ADDR_INIT_EN_MASK	(1 << TEGRA210_MIXER_AHUBRAMCTL_GAIN_CONFIG_RAM_CTRL_ADDR_INIT_EN_SHIFT)
#define TEGRA210_MIXER_AHUBRAMCTL_GAIN_CONFIG_RAM_CTRL_ADDR_INIT_EN		(1 << TEGRA210_MIXER_AHUBRAMCTL_GAIN_CONFIG_RAM_CTRL_ADDR_INIT_EN_SHIFT)

#define TEGRA210_MIXER_AHUBRAMCTL_GAIN_CONFIG_RAM_CTRL_SEQ_ACCESS_EN_SHIFT	12
#define TEGRA210_MIXER_AHUBRAMCTL_GAIN_CONFIG_RAM_CTRL_SEQ_ACCESS_EN_MASK	(1 << TEGRA210_MIXER_AHUBRAMCTL_GAIN_CONFIG_RAM_CTRL_SEQ_ACCESS_EN_SHIFT)
#define TEGRA210_MIXER_AHUBRAMCTL_GAIN_CONFIG_RAM_CTRL_SEQ_ACCESS_EN		(1 << TEGRA210_MIXER_AHUBRAMCTL_GAIN_CONFIG_RAM_CTRL_SEQ_ACCESS_EN_SHIFT)

#define TEGRA210_MIXER_AHUBRAMCTL_GAIN_CONFIG_RAM_CTRL_RAM_ADDR_SHIFT		0
#define TEGRA210_MIXER_AHUBRAMCTL_GAIN_CONFIG_RAM_CTRL_RAM_ADDR_MASK		(0x1ff << TEGRA210_MIXER_AHUBRAMCTL_GAIN_CONFIG_RAM_CTRL_RAM_ADDR_SHIFT)

#define TEGRA210_MIXER_TOTAL_PATH	(TEGRA210_MIXER_AXBAR_RX_MAX + \
						TEGRA210_MIXER_AXBAR_TX_MAX)

struct tegra210_mixer {
	struct regmap *regmap;
	int gain_coeff[14];
	int gain_value[TEGRA210_MIXER_AXBAR_RX_MAX];
	unsigned int channels_via_control[TEGRA210_MIXER_TOTAL_PATH];
	bool is_shutdown;
};

#endif
