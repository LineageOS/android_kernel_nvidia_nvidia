/*
 * Copyright (c) 2017-2021, NVIDIA Corporation.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * This file is autogenerated.  Do not edit.
 */

static struct nvhost_gating_register __attribute__((__unused__))
	t23x_host1x_gating_registers[] = {
	/* HOST1X_THOST_COMMON_ICG_EN_OVERRIDE_0 */
	{ .addr = 0x00004808, .prod = 0x00000000, .disable = 0x00000001 },
	{}
};

static struct nvhost_gating_register __attribute__((__unused__))
	t23x_nvenc_gating_registers[] = {
	/* NV_PNVENC_THI_CLK_OVERRIDE */
	{ .addr = 0x00000e00, .prod = 0x00000000, .disable = 0xffffffff },
	/* NV_PNVENC_THI_SLCG_OVERRIDE_HIGH_A */
	{ .addr = 0x00000088, .prod = 0x00000000, .disable = 0x000000ff },
	/* NV_PNVENC_THI_SLCG_OVERRIDE_LOW_A */
	{ .addr = 0x0000008c, .prod = 0x00000000, .disable = 0xffffffff },
	/* NV_PNVENC_FALCON_CGCTL */
	{ .addr = 0x000010a0, .prod = 0x00000000, .disable = 0x00000001 },
	/* NV_PNVENC_TFBIF_MCCIF_FIFOCTRL */
	{ .addr = 0x00001804, .prod = 0x00000000, .disable = 0x00000000 },
	/* NV_PNVENC_FALCON_CG2 */
	{ .addr = 0x00001134, .prod = 0x00010004, .disable = 0x0003fffe },
	/* NV_PNVENC_CG2 */
	{ .addr = 0x00002200, .prod = 0x00000000, .disable = 0xffffffff },
	/* NV_PNVENC_CG3 */
	{ .addr = 0x00002204, .prod = 0x00000000, .disable = 0xffffffff },
	/* NV_PNVENC_CG4 */
	{ .addr = 0x00002208, .prod = 0x00000000, .disable = 0xffffffff },
	{}
};

static struct nvhost_gating_register __attribute__((__unused__))
	t23x_tsec_gating_registers[] = {
	/* NV_PSEC_PRIV_BLOCKER_CTRL_CG1 */
	{ .addr = 0x00001e28, .prod = 0x00000000, .disable = 0x00000001 },
	/* NV_PSEC_RISCV_CG */
	{ .addr = 0x00002398, .prod = 0x00000003, .disable = 0x00000000 },
	{}
};

static struct nvhost_gating_register __attribute__((__unused__))
	t23x_nvjpg_gating_registers[] = {
	/* NV_PNVJPG_THI_CLK_OVERRIDE */
	{ .addr = 0x00000e00, .prod = 0x00000000, .disable = 0xffffffff },
	/* NV_PNVJPG_THI_SLCG_OVERRIDE_HIGH_A */
	{ .addr = 0x00000088, .prod = 0x00000000, .disable = 0x000000ff },
	/* NV_PNVJPG_THI_SLCG_OVERRIDE_LOW_A */
	{ .addr = 0x0000008c, .prod = 0x00000000, .disable = 0xffffffff },
	/* NV_PNVJPG_FALCON_CGCTL */
	{ .addr = 0x000010a0, .prod = 0x00000000, .disable = 0x00000001 },
	/* NV_PNVJPG_TFBIF_MCCIF_FIFOCTRL */
	{ .addr = 0x00001404, .prod = 0x00000000, .disable = 0x00000000 },
	/* NV_PNVJPG_FALCON_CG2 */
	{ .addr = 0x00001134, .prod = 0x00000140, .disable = 0x0003fffe },
	{}
};

static struct nvhost_gating_register __attribute__((__unused__))
	t23x_vic_gating_registers[] = {
	/* NV_PVIC_THI_CLK_OVERRIDE */
	{ .addr = 0x00000e00, .prod = 0x00000000, .disable = 0xffffffff },
	/* NV_PVIC_THI_SLCG_OVERRIDE_HIGH_A */
	{ .addr = 0x00000088, .prod = 0x00000000, .disable = 0x000000ff },
	/* NV_PVIC_THI_SLCG_OVERRIDE_LOW_A */
	{ .addr = 0x0000008c, .prod = 0x00000000, .disable = 0xffffffff },
	/* NV_PVIC_FALCON_CGCTL */
	{ .addr = 0x000010a0, .prod = 0x00000000, .disable = 0x00000001 },
	/* NV_PVIC_TFBIF_MCCIF_FIFOCTRL */
	{ .addr = 0x00002004, .prod = 0x00000000, .disable = 0x00000000 },
	/* NV_PVIC_FALCON_CG2 */
	{ .addr = 0x00001134, .prod = 0x00000000, .disable = 0x0003fffe },
	{}
};

static struct nvhost_gating_register __attribute__((__unused__))
	t23x_nvdec_gating_registers[] = {
	/* NV_PNVDEC_THI_CLK_OVERRIDE */
	{ .addr = 0x00000e00, .prod = 0x00000000, .disable = 0xffffffff },
	/* NV_PNVDEC_THI_SLCG_OVERRIDE_HIGH_A */
	{ .addr = 0x00000088, .prod = 0x00000000, .disable = 0x000000ff },
	/* NV_PNVDEC_THI_SLCG_OVERRIDE_LOW_A */
	{ .addr = 0x0000008c, .prod = 0x00000000, .disable = 0xffffffff },
	/* NV_PNVDEC_FALCON_CGCTL */
	{ .addr = 0x000010a0, .prod = 0x00000000, .disable = 0x00000001 },
	/* NV_PNVDEC_TFBIF_MCCIF_FIFOCTRL */
	{ .addr = 0x00004e04, .prod = 0x00000000, .disable = 0x00000000 },
	/* NV_PNVDEC_FALCON_CG2 */
	{ .addr = 0x00001134, .prod = 0x00010004, .disable = 0x0003fffe },
	/* NV_PNVDEC_CG2 */
	{ .addr = 0x00002328, .prod = 0x00000000, .disable = 0xffffffff },
	/* NV_PNVDEC_CG3 */
	{ .addr = 0x0000232c, .prod = 0x00800000, .disable = 0xffffffff },
	/* NV_PNVDEC_CG4 */
	{ .addr = 0x00002950, .prod = 0xffff000e, .disable = 0xffffffff },
	/* NV_PNVDEC_SLCG_GIP_THROTTLE */
	{ .addr = 0x00002340, .prod = 0x006db6db, .disable = 0x00ffffff },
	{}
};

static struct nvhost_gating_register __attribute__((__unused__))
	t23x_nvdla_gating_registers[] = {
	/* NV_CNVDLA_CLK_OVERRIDE */
	{ .addr = 0x00000e00, .prod = 0x00000000, .disable = 0xffffffff },
	/* NV_CNVDLA_SLCG_OVERRIDE_HIGH_A */
	{ .addr = 0x00000088, .prod = 0x00000000, .disable = 0x000000ff },
	/* NV_CNVDLA_SLCG_OVERRIDE_LOW_A */
	{ .addr = 0x0000008c, .prod = 0x00000000, .disable = 0xffffffff },
	/* NV_PNVDLA_FALCON_CGCTL */
	{ .addr = 0x000010a0, .prod = 0x00000000, .disable = 0x00000001 },
	/* NV_PNVDLA_FALCON_CG2 */
	{ .addr = 0x00001134, .prod = 0x00020008, .disable = 0x0003fffe },
	{}
};

static struct nvhost_gating_register __attribute__((__unused__))
	t23x_ofa_gating_registers[] = {
	/* NV_POFA_THI_CLK_OVERRIDE */
	{ .addr = 0x00000e00, .prod = 0x00000000, .disable = 0xffffffff },
	/* NV_POFA_THI_SLCG_OVERRIDE_HIGH_A */
	{ .addr = 0x00000088, .prod = 0x00000000, .disable = 0x000000ff },
	/* NV_POFA_THI_SLCG_OVERRIDE_LOW_A */
	{ .addr = 0x0000008c, .prod = 0x00000000, .disable = 0xffffffff },
	/* NV_POFA_FALCON_CGCTL */
	{ .addr = 0x000010a0, .prod = 0x00000000, .disable = 0x00000001 },
	/* NV_POFA_TFBIF_MCCIF_FIFOCTRL */
	{ .addr = 0x00001404, .prod = 0x00000000, .disable = 0x00000000 },
	/* NV_POFA_FALCON_CG2 */
	{ .addr = 0x00001134, .prod = 0x00010004, .disable = 0x0003fffe },
	{}
};
