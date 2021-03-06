/*
 * Copyright (c) 2017-2020, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#ifndef __CDI_GPIO_PRIV_H__
#define __CDI_GPIO_PRIV_H__

struct cdi_gpio_plat_data {
	const char *gpio_prnt_chip;
	u32 max_gpio;
};

struct cdi_gpio_desc {
	u32 gpio;
	atomic_t ref_cnt;
};

struct cdi_gpio_priv {
	struct device *pdev;
	struct cdi_gpio_plat_data pdata;
	struct mutex mutex;
	struct gpio_chip gpio_chip;
	struct gpio_chip *tgc;
	struct cdi_gpio_desc *gpios;
	u32 num_gpio;
};

#endif /* __CDI_GPIO_PRIV_H__ */
