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
#include <osd.h>

/**
 *	osd_udelay - Adds delay in micro seconds.
 *	@usec:	Delay number in micro seconds.
 *
 *      Algorithm: Invokes OSD delay function for adding delay
 *
 *	Dependencies: None
 *
 *	Protection: None.
 *
 *	Return: None.
 */
void osd_udelay(unsigned long usec)
{
	udelay(usec);
}

/**
 *	osd_usleep_range - Adds sleep in micro seconds
 *	@umin:	Minimum sleep required in micro seconds.
 *	@umax:	Maximum sleep required in micro seconds.
 *
 *	Algorithm: Invokes OSD function to add sleep.
 *
 *	Dependencies: None
 *
 *	Protection: None.
 *
 *	Return: None.
 */
void osd_usleep_range(unsigned long umin, unsigned long umax)
{
	usleep_range(umin, umax);
}

/**
 *	osd_msleep - Adds sleep in milli seconds.
 *	@msec:  Minimum sleep required in milli seconds.
 *
 *      Algorithm: Invokes OSD function to add sleep.
 *
 *	Dependencies: None
 *
 *	Protection: None.
 *
 *	Return: None.
 */
void osd_msleep(unsigned int msec)
{
	msleep(msec);
}

/**
 *	osd_info - Prints log at INFO log level
 *	@priv: OSD private data
 *	@fmt: log format
 *
 *	Algorithm: Invokes OSD function to prints the data in kernel log
 *
 *	Dependencies: None
 *
 *	Protection: None.
 *
 *	Return: None.
 */
void osd_info(void *priv, const char *fmt, ...)
{
	dev_info(((struct ether_priv_data *)priv)->dev, fmt);
}

/**
 *	osd_err - Prints log at ERR log level
 *	@priv: OSD private data
 *	@fmt: log format
 *
 *	Algorithm: Invokes OSD function to prints the data in kernel log
 *
 *	Dependencies: None
 *
 *	Protection: None.
 *
 *	Return: None.
 */
void osd_err(void *priv, const char *fmt, ...)
{
	dev_err(((struct ether_priv_data *)priv)->dev, fmt);
}

/**
 *	ether_alloc_skb - Allocate and DMA map Rx buffer.
 *	@pdata: OSD private data structure.
 *	@rx_swcx: Rx ring software context.
 *	@dma_rx_buf_len: DMA Rx buffer length.
 *
 *	Algorithm: Allocate network buffer (skb) and map skb->data to
 *	DMA mappable address.
 *
 *	Dependencies: None.
 *
 *	Protection: None.
 *
 *	Return: 0 - success, ENOMEM - failure.
 */
static inline int ether_alloc_skb(struct ether_priv_data *pdata,
				  struct osi_rx_swcx *rx_swcx,
				  unsigned int dma_rx_buf_len)
{
	struct sk_buff *skb = NULL;
	dma_addr_t dma_addr;

	skb = netdev_alloc_skb_ip_align(pdata->ndev, dma_rx_buf_len);
	if (unlikely(skb == NULL)) {
		dev_err(pdata->dev, "RX skb allocation failed\n");
		return -ENOMEM;
	}

	dma_addr = dma_map_single(pdata->dev, skb->data, dma_rx_buf_len,
				  DMA_FROM_DEVICE);
	if (unlikely(dma_mapping_error(pdata->dev, dma_addr) != 0)) {
		dev_err(pdata->dev, "RX skb dma map failed\n");
		dev_kfree_skb_any(skb);
		return -ENOMEM;
	}

	rx_swcx->buf_virt_addr = skb;
	rx_swcx->buf_phy_addr = dma_addr;

	return 0;
}

/**
 *      ether_realloc_rx_skb - Re-fill DMA channel Rx ring
 *      @osi: OSI private data structure.
 *      @rx_ring: DMA channel Rx ring instance.
 *      @chan: DMA Rx channel number.
 *
 *      Algorithm: Re-fill Rx DMA channel ring until dirty rx index is equal to
 *      current rx index.
 *      1) Invokes OSD layer to allocate the buffer and map the buffer to DMA
 *      mappable address.
 *      2) Fill Rx descriptors with required data.
 *      3) Program DMA rx channel tail pointer.
 *
 *      Dependencies: None.
 *
 *      Protection: None.
 *
 *      Return: None.
 */
static void ether_realloc_rx_skb(struct ether_priv_data *pdata,
				 struct osi_rx_ring *rx_ring,
				 unsigned int chan)
{
	struct osi_dma_priv_data *osi_dma = pdata->osi_dma;
	struct osi_core_priv_data *osi_core = pdata->osi_core;
	struct osi_rx_swcx *rx_swcx = NULL;
	struct osi_rx_desc *rx_desc = NULL;
	unsigned long val;
	int ret = 0;

	while (rx_ring->refill_idx != rx_ring->cur_rx_idx) {
		rx_swcx = rx_ring->rx_swcx + rx_ring->refill_idx;
		rx_desc = rx_ring->rx_desc + rx_ring->refill_idx;

		ret = ether_alloc_skb(pdata, rx_swcx, osi_dma->rx_buf_len);
		if (ret < 0) {
			val = osi_core->xstats.re_alloc_rxbuf_failed[chan];
			osi_core->xstats.re_alloc_rxbuf_failed[chan] =
				osi_update_stats_counter(val, 1UL);
			break;
		}

		osi_rx_dma_desc_init(rx_swcx, rx_desc);
		INCR_RX_DESC_INDEX(rx_ring->refill_idx, 1U);
	}

	osi_update_rx_tailptr(osi_dma, rx_ring, chan);
}

/**
 *	osd_receive_packet - Handover received packet to network stack.
 *	@priv: OSD private data structure.
 *	@rxring: Pointer to DMA channel Rx ring.
 *	@chan: DMA Rx channel number.
 *	@dma_buf_len: Rx DMA buffer length.
 *	@rx_pkt_len: Received packet length.
 *
 *	Algorithm:
 *	1) Unmap the DMA buffer address.
 *	2) Updates socket buffer with len and ether type and handover to
 *	Linux network stack.
 *	3) Refill the Rx ring based on threshold.
 *
 *	Dependencies: Rx completion need to make sure that Rx descriptors
 *	processed properly.
 *
 *	Protection: None.
 *
 *	Return: None.
 */
void osd_receive_packet(void *priv, void *rxring, unsigned int chan,
			unsigned int dma_buf_len, void *rxpkt_cx,
			void *rx_pkt_swcx)
{
	struct ether_priv_data *pdata = (struct ether_priv_data *)priv;
	struct ether_rx_napi *rx_napi = pdata->rx_napi[chan];
	struct osi_rx_ring *rx_ring = (struct osi_rx_ring *)rxring;
	struct osi_rx_swcx *rx_swcx = (struct osi_rx_swcx *)rx_pkt_swcx;
	struct osi_rx_pkt_cx *rx_pkt_cx = (struct osi_rx_pkt_cx *)rxpkt_cx;
	struct sk_buff *skb = (struct sk_buff *)rx_swcx->buf_virt_addr;
	dma_addr_t dma_addr = (dma_addr_t)rx_swcx->buf_phy_addr;
	struct net_device *ndev = pdata->ndev;
	struct osi_pkt_err_stats *pkt_err_stat = &pdata->osi_dma->pkt_err_stats;
	struct skb_shared_hwtstamps *shhwtstamp;
	unsigned long val;

	dma_unmap_single(pdata->dev, dma_addr, dma_buf_len, DMA_FROM_DEVICE);

	/* Process only the Valid packets */
	if (likely((rx_pkt_cx->flags & OSI_PKT_CX_VALID) ==
		   OSI_PKT_CX_VALID)) {
		skb_put(skb, rx_pkt_cx->pkt_len);

		if (likely(rx_pkt_cx->rxcsum == OSI_CHECKSUM_UNNECESSARY)) {
			skb->ip_summed = CHECKSUM_UNNECESSARY;
		} else {
			skb->ip_summed = CHECKSUM_NONE;
		}

		if ((rx_pkt_cx->flags & OSI_PKT_CX_VLAN) == OSI_PKT_CX_VLAN) {
			val = pdata->osi_dma->dstats.rx_vlan_pkt_n;
			pdata->osi_dma->dstats.rx_vlan_pkt_n =
				osi_update_stats_counter(val, 1UL);
			__vlan_hwaccel_put_tag(skb, htons(ETH_P_8021Q),
					       rx_pkt_cx->vlan_tag);
		}

		/* Handle time stamp */
		if ((rx_pkt_cx->flags & OSI_PKT_CX_PTP) == OSI_PKT_CX_PTP) {
			shhwtstamp = skb_hwtstamps(skb);
			memset(shhwtstamp, 0,
			       sizeof(struct skb_shared_hwtstamps));
			shhwtstamp->hwtstamp = ns_to_ktime(rx_pkt_cx->ns);
		}

		skb->dev = ndev;
		skb->protocol = eth_type_trans(skb, ndev);
		ndev->stats.rx_bytes += skb->len;
		if (likely(ndev->features & NETIF_F_GRO)) {
			napi_gro_receive(&rx_napi->napi, skb);
		} else {
			netif_receive_skb(skb);
		}
	} else {
		ndev->stats.rx_crc_errors = pkt_err_stat->rx_crc_error;
		ndev->stats.rx_errors++;
	}

	ndev->stats.rx_packets++;
	rx_swcx->buf_virt_addr = NULL;
	rx_swcx->buf_phy_addr = 0;

	if (osi_get_refill_rx_desc_cnt(rx_ring) >= 16U)
		ether_realloc_rx_skb(pdata, rx_ring, chan);
}

/**
 *	osd_transmit_complete - Transmit completion routine.
 *	@priv: OSD private data structure.
 *	@buffer: Buffer address to free.
 *	@dmaaddr: DMA address to unmap.
 *	@len: Length of data.
 *	@tx_done_pkt_cx: Pointer to struct which has tx done status info.
 *	This struct has flags to indicate tx error, whether DMA address
 *	is mapped from paged/linear buffer.
 *
 *	Algorithm:
 *	1) Updates stats for linux network stack.
 *	2) unmap and free the buffer DMA address and buffer.
 *	3) Time stamp will be update to stack if available.
 *
 *	Dependencies: Tx completion need to make sure that Tx descriptors
 *	processed properly.
 *
 *	Protection: None.
 *
 *	Return: None.
 */
void osd_transmit_complete(void *priv, void *buffer, unsigned long dmaaddr,
			   unsigned int len, void *tx_done_pkt_cx)
{
	struct osi_txdone_pkt_cx *txdone_pkt_cx = (struct osi_txdone_pkt_cx *)
						  tx_done_pkt_cx;
	struct ether_priv_data *pdata = (struct ether_priv_data *)priv;
	struct osi_dma_priv_data *osi_dma = pdata->osi_dma;
	struct sk_buff *skb = (struct sk_buff *)buffer;
	dma_addr_t dma_addr = (dma_addr_t)dmaaddr;
	struct skb_shared_hwtstamps shhwtstamp;
	struct net_device *ndev = pdata->ndev;
	struct osi_tx_ring *tx_ring;
	struct netdev_queue *txq;
	unsigned int chan;

	ndev->stats.tx_packets++;
	ndev->stats.tx_bytes += len;

	if ((txdone_pkt_cx->flags & OSI_TXDONE_CX_TS) == OSI_TXDONE_CX_TS) {
		memset(&shhwtstamp, 0, sizeof(struct skb_shared_hwtstamps));
		shhwtstamp.hwtstamp = ns_to_ktime(txdone_pkt_cx->ns);
		/* pass tstamp to stack */
		skb_tstamp_tx(skb, &shhwtstamp);
	}

	if (dma_addr) {
		if ((txdone_pkt_cx->flags & OSI_TXDONE_CX_PAGED_BUF) ==
		    OSI_TXDONE_CX_PAGED_BUF) {
			dma_unmap_page(pdata->dev, dmaaddr,
				       len, DMA_TO_DEVICE);
		} else {
			dma_unmap_single(pdata->dev, dmaaddr,
					 len, DMA_TO_DEVICE);
		}
	}

	if (skb) {
		chan = skb_get_queue_mapping(skb);
		tx_ring = osi_dma->tx_ring[chan];
		txq = netdev_get_tx_queue(ndev, chan);

		if (netif_tx_queue_stopped(txq) &&
		    ether_avail_txdesc_cnt(tx_ring) >= TX_DESC_THRESHOLD) {
			netif_tx_wake_queue(txq);
			netdev_dbg(ndev, "Tx ring[%d] - waking Txq\n", chan);
		}

		dev_consume_skb_any(skb);
	}
}
