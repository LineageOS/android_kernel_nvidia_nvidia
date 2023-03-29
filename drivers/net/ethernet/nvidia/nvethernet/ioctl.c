/*
 * Copyright (c) 2019, NVIDIA CORPORATION.  All rights reserved.
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

/**
 *	ether_set_avb_algo - function to handle private ioctl
 *	EQOS_AVB_ALGORITHM
 *	@ndev: network device structure
 *	@ifdata: interface private data structure
 *
 *	Algorithm:
 *	- Call osi_set_avb with user passed data
 *
 *	Dependencies: Ethernet interface need to be up.
 *
 *	Protection: None.
 *
 *	Return: 0 - success, negative value - failure.
 */
static int ether_set_avb_algo(struct net_device *ndev,
			      struct ether_ifr_data *ifdata)
{
	struct ether_priv_data *pdata = netdev_priv(ndev);
	struct osi_core_priv_data *osi_core = pdata->osi_core;
	struct osi_core_avb_algorithm l_avb_struct;
	int ret = -1;

	if (ifdata->ptr == NULL) {
		dev_err(pdata->dev, "%s: Invalid data for priv ioctl %d\n",
			__func__, ifdata->ifcmd);
		return ret;
	}

	if (copy_from_user(&l_avb_struct,
			   (struct osi_core_avb_algorithm *)ifdata->ptr,
			   sizeof(struct osi_core_avb_algorithm)) != 0U) {
		dev_err(pdata->dev,
			"Failed to fetch AVB Struct info from user\n");
		return ret;
	}

	return osi_set_avb(osi_core, &l_avb_struct);
}

/**
 *	ether_get_avb_algo - function to get avb data from registers.
 *	This function is called for EQOS_GET_AVB_ALGORITHM
 *	@ndev: network device structure
 *	@ifdata: interface private data structure
 *
 *	Algorithm:
 *	- Call osi_get_avb with user passed data(qindex)
 *
 *	Dependencies: Ethernet interface need to be up. Caller should
 *	check for return vlaue before using return value.
 *
 *	Protection: None.
 *
 *	Return: 0 - success, negative value - failure.
 */
static int ether_get_avb_algo(struct net_device *ndev,
			      struct ether_ifr_data *ifdata)
{
	struct ether_priv_data *pdata = netdev_priv(ndev);
	struct osi_core_priv_data *osi_core = pdata->osi_core;
	struct osi_core_avb_algorithm avb_data;
	int ret;

	if (ifdata->ptr == NULL) {
		dev_err(pdata->dev, "%s: Invalid data for priv ioctl %d\n",
			__func__, ifdata->ifcmd);
		return -EINVAL;
	}

	if (copy_from_user(&avb_data,
			   (struct osi_core_avb_algorithm *)ifdata->ptr,
			   sizeof(struct osi_core_avb_algorithm)) != 0U) {
		dev_err(pdata->dev,
			"Failed to fetch AVB Struct info from user\n");
		return -EFAULT;
	}

	ret = osi_get_avb(osi_core, &avb_data);
	if (ret != 0) {
		dev_err(pdata->dev,
			"Failed to get AVB Struct info from registers\n");
		return ret;
	}
	if (copy_to_user(ifdata->ptr, &avb_data,
			 sizeof(struct osi_core_avb_algorithm)) != 0U) {
		dev_err(pdata->dev, "%s: copy_to_user failed\n", __func__);
		return -EFAULT;
	}

	return ret;
}

/*
 *	ether_config_arp_offload - Handle ioctl to enable/disable ARP offload
 *	@pdata: OS dependent private data structure.
 *	@ifrd_p: Interface request private data pointer.
 *
 *	Algorithm:
 *	1) Copy the priv data from user space. This includes the IP address
 *	to be updated in HW.
 *	2) Check if IP address provided in priv data is valid.
 *	3) If IP address is valid, invoke OSI API to update HW registers.
 *
 *	Dependencies: Interface should be running (enforced by caller).
 *
 *	Protection: None.
 *
 *	Return: 0 - success, -ve value - failure
 */
static int ether_config_arp_offload(struct ether_priv_data *pdata,
				    struct ether_ifr_data *ifrd_p)
{
	int i, ret = -EINVAL;
	struct arp_offload_param param;
	/* TODO: Need Spin lock to prevent multiple apps from
	 * requesting same ioctls to the same MAC instance
	 */
	if (!ifrd_p->ptr) {
		dev_err(pdata->dev, "%s: Invalid data for priv ioctl %d\n",
			__func__, ifrd_p->ifcmd);
		return ret;
	}

	if (copy_from_user(&param, (struct arp_offload_param *)ifrd_p->ptr,
			   sizeof(struct arp_offload_param))) {
		dev_err(pdata->dev, "%s: copy_from_user failed\n", __func__);
		return ret;
	}

	for (i = 0; i < NUM_BYTES_IN_IPADDR; i++) {
		if (param.ip_addr[i] > MAX_IP_ADDR_BYTE) {
			dev_err(pdata->dev, "%s: Invalid IP addr\n", __func__);
			return ret;
		}
	}

	ret = osi_config_arp_offload(pdata->osi_core, ifrd_p->if_flags,
				     param.ip_addr);
	dev_err(pdata->dev, "ARP offload: %s : %s\n",
		ifrd_p->if_flags ? "Enable" : "Disable",
		ret ? "Failed" : "Success");

	return ret;
}

/**
 *	ether_priv_ioctl - Handle private IOCTLs
 *	@ndev: network device structure
 *	@ifr: Interface request structure used for socket ioctl's.
 *
 *	Algorithm:
 *	1) Copy the priv command data from user space.
 *	2) Check the priv command cmd and invoke handler func.
 *	if it is supported.
 *	3) Copy result back to user space.
 *
 *	Dependencies: Interface should be running (enforced by caller).
 *
 *	Protection: None.
 *
 *	Return: 0 - success, negative value - failure.
 */
int ether_handle_priv_ioctl(struct net_device *ndev,
			    struct ifreq *ifr)
{
	struct ether_priv_data *pdata = netdev_priv(ndev);
	struct ether_ifr_data ifdata;
	int ret = -EOPNOTSUPP;

	if (copy_from_user(&ifdata, ifr->ifr_data, sizeof(ifdata)) != 0U) {
		dev_err(pdata->dev, "%s(): copy_from_user failed %d\n"
			, __func__, __LINE__);
		return -EFAULT;
	}

	switch (ifdata.ifcmd) {
	case ETHER_AVB_ALGORITHM:
		ret = ether_set_avb_algo(ndev, &ifdata);
		break;
	case ETHER_GET_AVB_ALGORITHM:
		ret = ether_get_avb_algo(ndev, &ifdata);
		break;
	case ETHER_CONFIG_ARP_OFFLOAD:
		ret = ether_config_arp_offload(pdata, &ifdata);
		break;
	default:
		break;
	}

	ifdata.command_error = ret;
	if (copy_to_user(ifr->ifr_data, &ifdata, sizeof(ifdata)) != 0U) {
		dev_err(pdata->dev, "%s: copy_to_user failed\n", __func__);
		return -EFAULT;
	}

	return ret;
}
