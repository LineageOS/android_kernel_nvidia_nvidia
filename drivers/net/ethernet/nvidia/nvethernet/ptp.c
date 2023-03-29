/*
 * Copyright (c) 2018-2019, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ether_linux.h"
#include <linux/platform/tegra/ptp-notifier.h>

/**
 * @brief DEFINE_RAW_SPINLOCK: raw spinlock to get HW PTP time and kernel time atomically
 *
 */
static DEFINE_RAW_SPINLOCK(ether_ts_lock);

#ifdef CONFIG_TEGRA_PTP_NOTIFIER
/**
 * @brief Function used to get PTP time
 * @param[in] data: OSI core private data structure
 *
 * @retval "nano seconds" of MAC system time
 */
static inline u64 ether_get_ptptime(void *data)
{
	struct ether_priv_data *pdata = data;
	struct osi_core_priv_data *osi_core = pdata->osi_core;
	unsigned long flags;
	unsigned long long ns;
	unsigned int sec, nsec;
	int ret = -1;

	raw_spin_lock_irqsave(&pdata->ptp_lock, flags);

	ret = osi_get_systime_from_mac(osi_core, &sec, &nsec);
	if (ret != 0) {
		dev_err(pdata->dev, "%s: Failed to read systime from MAC %d\n",
			__func__, ret);
		raw_spin_unlock_irqrestore(&pdata->ptp_lock, flags);
		return ret;
	}

	ns = nsec + (sec * OSI_NSEC_PER_SEC);

	raw_spin_unlock_irqrestore(&pdata->ptp_lock, flags);

	return ns;
}
#endif

/**
 * @brief Adjust MAC hardware time
 *
 * Algorithm: This function is used to shift/adjust the time of the
 * hardware clock.
 *
 * @param[in] ptp: Pointer to ptp_clock_info structure.
 * @param[in] delta: Desired change in nanoseconds.
 *
 * @retval 0 on success
 * @retval "negative value" on failure.
 */
static int ether_adjust_time(struct ptp_clock_info *ptp, s64 delta)
{
	struct ether_priv_data *pdata = container_of(ptp,
						     struct ether_priv_data,
						     ptp_clock_ops);
	struct osi_core_priv_data *osi_core = pdata->osi_core;
	int ret = -1;

	raw_spin_lock(&pdata->ptp_lock);

	ret = osi_adjust_time(osi_core, delta);
	if (ret < 0) {
		dev_err(pdata->dev,
			"%s:failed to adjust time with reason %d\n",
			__func__, ret);
	}

	raw_spin_unlock(&pdata->ptp_lock);

	return ret;
}

/**
 * @brief Adjust MAC hardware frequency
 *
 * Algorithm: This function is used to adjust the frequency of the
 * hardware clock.
 *
 * @param[in] ptp: Pointer to ptp_clock_info structure.
 * @param[in] ppb: Desired period change in parts per billion.
 *
 * @retval 0 on success
 * @retval "negative value" on failure.
 */
static int ether_adjust_freq(struct ptp_clock_info *ptp, s32 ppb)
{
	struct ether_priv_data *pdata = container_of(ptp,
						     struct ether_priv_data,
						     ptp_clock_ops);
	struct osi_core_priv_data *osi_core = pdata->osi_core;
	int ret = -1;

	raw_spin_lock(&pdata->ptp_lock);

	ret = osi_adjust_freq(osi_core, ppb);
	if (ret < 0) {
		dev_err(pdata->dev,
			"%s:failed to adjust frequency with reason code %d\n",
			__func__, ret);
	}

	raw_spin_unlock(&pdata->ptp_lock);

	return ret;
}

/**
 * @brief Gets current hardware time
 *
 * Algorithm: This function is used to read the current time from the
 * hardware clock
 *
 * @param[in] ptp: Pointer to ptp_clock_info structure.
 * @param[in] ts: Pointer to hole time.
 *
 * @retval 0 on success
 * @retval "negative value" on failure.
 */
static int ether_get_time(struct ptp_clock_info *ptp, struct timespec *ts)
{
	struct ether_priv_data *pdata = container_of(ptp,
						     struct ether_priv_data,
						     ptp_clock_ops);
	struct osi_core_priv_data *osi_core = pdata->osi_core;
	unsigned int sec, nsec;

	raw_spin_lock(&pdata->ptp_lock);

	osi_get_systime_from_mac(osi_core, &sec, &nsec);

	raw_spin_unlock(&pdata->ptp_lock);

	ts->tv_sec = sec;
	ts->tv_nsec = nsec;

	return 0;
}

/**
 * @brief Set current system time to MAC Hardware
 *
 * Algorithm: This function is used to set the current time to the
 * hardware clock.
 *
 * @param[in] ptp: Pointer to ptp_clock_info structure.
 * @param[in] ts: Time value to set.
 *
 * @retval 0 on success
 * @retval "negative value" on failure.
 */
static int ether_set_time(struct ptp_clock_info *ptp,
		const struct timespec *ts)
{
	struct ether_priv_data *pdata = container_of(ptp,
						     struct ether_priv_data,
						     ptp_clock_ops);
	struct osi_core_priv_data *osi_core = pdata->osi_core;
	int ret = -1;

	raw_spin_lock(&pdata->ptp_lock);

	ret = osi_set_systime_to_mac(osi_core, ts->tv_sec, ts->tv_nsec);
	if (ret < 0) {
		dev_err(pdata->dev,
			"%s:failed to set system time with reason %d\n",
			__func__, ret);
	}

	raw_spin_unlock(&pdata->ptp_lock);

	return ret;
}

/**
 * @brief Describing Ethernet PTP hardware clock
 */
static struct ptp_clock_info ether_ptp_clock_ops = {
	.owner = THIS_MODULE,
	.name = "ether_ptp_clk",
	.max_adj = OSI_ETHER_SYSCLOCK,
	.n_alarm = 0,
	.n_ext_ts = 0,
	.n_per_out = 0,
	.pps = 0,
	.adjfreq = ether_adjust_freq,
	.adjtime = ether_adjust_time,
	.gettime64 = ether_get_time,
	.settime64 = ether_set_time,
};

int ether_ptp_init(struct ether_priv_data *pdata)
{
	if (pdata->hw_feat.tsstssel == OSI_DISABLE) {
		pdata->ptp_clock = NULL;
		dev_err(pdata->dev, "No PTP supports in HW\n"
			"Aborting PTP clock driver registration\n");
		return -1;
	}

	raw_spin_lock_init(&pdata->ptp_lock);

	pdata->ptp_clock_ops = ether_ptp_clock_ops;
	pdata->ptp_clock = ptp_clock_register(&pdata->ptp_clock_ops,
					      pdata->dev);
	if (IS_ERR(pdata->ptp_clock)) {
		pdata->ptp_clock = NULL;
		dev_err(pdata->dev, "Fail to register PTP clock\n");
		return -1;
	}

	/* By default enable nano second accuracy */
	pdata->osi_core->ptp_config.one_nsec_accuracy = OSI_ENABLE;

	return 0;
}

void ether_ptp_remove(struct ether_priv_data *pdata)
{
	if (pdata->ptp_clock) {
		ptp_clock_unregister(pdata->ptp_clock);
	}
}

int ether_handle_hwtstamp_ioctl(struct ether_priv_data *pdata,
		struct ifreq *ifr)
{
	struct osi_core_priv_data *osi_core = pdata->osi_core;
	struct hwtstamp_config config;
	unsigned int hwts_rx_en = 1;
	struct timespec now;

	if (pdata->hw_feat.tsstssel == OSI_DISABLE) {
		dev_info(pdata->dev, "HW timestamping not available\n");
		return -EOPNOTSUPP;
	}

	if (copy_from_user(&config, ifr->ifr_data,
			   sizeof(struct hwtstamp_config))) {
		return -EFAULT;
	}

	dev_info(pdata->dev, "config.flags = %#x, tx_type = %#x,"
		 "rx_filter = %#x\n", config.flags, config.tx_type,
		 config.rx_filter);

	/* reserved for future extensions */
	if (config.flags) {
		return -EINVAL;
	}

	switch (config.tx_type) {
	case HWTSTAMP_TX_OFF:
		pdata->hwts_tx_en = OSI_DISABLE;
		break;

	case HWTSTAMP_TX_ON:
		pdata->hwts_tx_en = OSI_ENABLE;
		break;

	default:
		dev_err(pdata->dev, "tx_type is out of range\n");
		return -ERANGE;
	}
	/* Initialize ptp filter to 0 */
	osi_core->ptp_config.ptp_filter = 0;

	switch (config.rx_filter) {
		/* time stamp no incoming packet at all */
	case HWTSTAMP_FILTER_NONE:
		hwts_rx_en = 0;
		break;

		/* PTP v1, UDP, any kind of event packet */
	case HWTSTAMP_FILTER_PTP_V1_L4_EVENT:
		osi_core->ptp_config.ptp_filter = OSI_MAC_TCR_SNAPTYPSEL_1 |
						  OSI_MAC_TCR_TSIPV4ENA    |
						  OSI_MAC_TCR_TSIPV6ENA;
		break;

		/* PTP v1, UDP, Sync packet */
	case HWTSTAMP_FILTER_PTP_V1_L4_SYNC:
		osi_core->ptp_config.ptp_filter = OSI_MAC_TCR_TSEVENTENA |
						  OSI_MAC_TCR_TSIPV4ENA	 |
						  OSI_MAC_TCR_TSIPV6ENA;
		break;

		/* PTP v1, UDP, Delay_req packet */
	case HWTSTAMP_FILTER_PTP_V1_L4_DELAY_REQ:
		osi_core->ptp_config.ptp_filter = OSI_MAC_TCR_TSMASTERENA |
						  OSI_MAC_TCR_TSEVENTENA  |
						  OSI_MAC_TCR_TSIPV4ENA   |
						  OSI_MAC_TCR_TSIPV6ENA;
		break;

		/* PTP v2, UDP, any kind of event packet */
	case HWTSTAMP_FILTER_PTP_V2_L4_EVENT:
		osi_core->ptp_config.ptp_filter = OSI_MAC_TCR_SNAPTYPSEL_1 |
						  OSI_MAC_TCR_TSIPV4ENA    |
						  OSI_MAC_TCR_TSIPV6ENA    |
						  OSI_MAC_TCR_TSVER2ENA;
		break;

		/* PTP v2, UDP, Sync packet */
	case HWTSTAMP_FILTER_PTP_V2_L4_SYNC:
		osi_core->ptp_config.ptp_filter = OSI_MAC_TCR_TSEVENTENA   |
						  OSI_MAC_TCR_TSIPV4ENA    |
						  OSI_MAC_TCR_TSIPV6ENA    |
						  OSI_MAC_TCR_TSVER2ENA;
		break;

		/* PTP v2, UDP, Delay_req packet */
	case HWTSTAMP_FILTER_PTP_V2_L4_DELAY_REQ:
		osi_core->ptp_config.ptp_filter = OSI_MAC_TCR_TSEVENTENA   |
						  OSI_MAC_TCR_TSMASTERENA  |
						  OSI_MAC_TCR_TSIPV4ENA    |
						  OSI_MAC_TCR_TSIPV6ENA    |
						  OSI_MAC_TCR_TSVER2ENA;
		break;

		/* PTP v2/802.AS1, any layer, any kind of event packet */
	case HWTSTAMP_FILTER_PTP_V2_EVENT:
		osi_core->ptp_config.ptp_filter = OSI_MAC_TCR_SNAPTYPSEL_1 |
						  OSI_MAC_TCR_TSIPV4ENA    |
						  OSI_MAC_TCR_TSIPV6ENA    |
						  OSI_MAC_TCR_TSVER2ENA    |
						  OSI_MAC_TCR_TSIPENA;
		break;

		/* PTP v2/802.AS1, any layer, Sync packet */
	case HWTSTAMP_FILTER_PTP_V2_SYNC:
		osi_core->ptp_config.ptp_filter = OSI_MAC_TCR_TSIPV4ENA    |
						  OSI_MAC_TCR_TSIPV6ENA    |
						  OSI_MAC_TCR_TSVER2ENA    |
						  OSI_MAC_TCR_TSEVENTENA   |
						  OSI_MAC_TCR_TSIPENA      |
						  OSI_MAC_TCR_AV8021ASMEN;
		break;

		/* PTP v2/802.AS1, any layer, Delay_req packet */
	case HWTSTAMP_FILTER_PTP_V2_DELAY_REQ:
		osi_core->ptp_config.ptp_filter = OSI_MAC_TCR_TSIPV4ENA    |
						  OSI_MAC_TCR_TSIPV6ENA    |
						  OSI_MAC_TCR_TSVER2ENA    |
						  OSI_MAC_TCR_TSEVENTENA   |
						  OSI_MAC_TCR_AV8021ASMEN  |
						  OSI_MAC_TCR_TSMASTERENA  |
						  OSI_MAC_TCR_TSIPENA;
		break;

		/* time stamp any incoming packet */
	case HWTSTAMP_FILTER_ALL:
		osi_core->ptp_config.ptp_filter = OSI_MAC_TCR_TSENALL;
		break;

	default:
		dev_err(pdata->dev, "rx_filter is out of range\n");
		return -ERANGE;
	}

	if (!pdata->hwts_tx_en && !hwts_rx_en) {
		/* disable the PTP configuration */
		osi_ptp_configuration(osi_core, OSI_DISABLE);
	} else {
		/* Store SYS CLOCK */
		osi_core->ptp_config.ptp_clock = OSI_ETHER_SYSCLOCK;
		/* initialize system time */
		getnstimeofday(&now);
		/* Store sec and nsec */
		osi_core->ptp_config.sec = now.tv_sec;
		osi_core->ptp_config.nsec = now.tv_nsec;
		/* one nsec accuracy */
		osi_core->ptp_config.one_nsec_accuracy = OSI_ENABLE;
		/* Enable the PTP configuration */
		osi_ptp_configuration(osi_core, OSI_ENABLE);
#ifdef CONFIG_TEGRA_PTP_NOTIFIER
		/* Register broadcasting MAC timestamp to clients */
		tegra_register_hwtime_source(ether_get_ptptime, pdata);
#endif
	}

	return (copy_to_user(ifr->ifr_data, &config,
			     sizeof(struct hwtstamp_config))) ? -EFAULT : 0;
}

/**
 * @brief Function to handle PTP private IOCTL
 *
 * Algorithm: This function is used to query hardware time and
 * the kernel time simultaneously.
 *
 * @param [in] pdata: Pointer to private data structure.
 * @param [in] ifr: Interface request structure used for socket ioctl
 *
 * @note PTP clock driver need to be successfully registered during
 *	initialization and HW need to support PTP functionality.
 *
 * @retval 0 on success.
 * @retval "negative value" on failure.
 */

int ether_handle_priv_ts_ioctl(struct ether_priv_data *pdata,
			       struct ifreq *ifr)
{
	struct ifr_data_timestamp_struct req;
	struct osi_core_priv_data *osi_core = pdata->osi_core;
	unsigned long flags;
	unsigned int sec, nsec;
	int ret = -1;

	if (ifr->ifr_data == NULL) {
		dev_err(pdata->dev, "%s: Invalid data for priv ioctl\n",
			__func__);
		return -EFAULT;
	}

	if (copy_from_user(&req, ifr->ifr_data, sizeof(req))) {
		dev_err(pdata->dev, "%s: Data copy from user failed\n",
			__func__);
		return -EFAULT;
	}

	raw_spin_lock_irqsave(&ether_ts_lock, flags);
	switch (req.clockid) {
	case CLOCK_REALTIME:
		ktime_get_real_ts(&req.kernel_ts);
		break;

	case CLOCK_MONOTONIC:
		ktime_get_ts(&req.kernel_ts);
		break;

	default:
		dev_err(pdata->dev, "Unsupported clockid\n");
	}

	ret = osi_get_systime_from_mac(osi_core, &sec, &nsec);
	if (ret != 0) {
		dev_err(pdata->dev, "%s: Failed to read systime from MAC %d\n",
			__func__, ret);
		raw_spin_unlock_irqrestore(&ether_ts_lock, flags);
		return ret;
	}
	req.hw_ptp_ts.tv_sec = sec;
	req.hw_ptp_ts.tv_nsec = nsec;

	raw_spin_unlock_irqrestore(&ether_ts_lock, flags);

	dev_dbg(pdata->dev, "tv_sec = %ld, tv_nsec = %ld\n",
		req.hw_ptp_ts.tv_sec, req.hw_ptp_ts.tv_nsec);

	if (copy_to_user(ifr->ifr_data, &req, sizeof(req))) {
		dev_err(pdata->dev, "%s: Data copy to user failed\n",
			__func__);
		return -EFAULT;
	}

	return ret;
}
