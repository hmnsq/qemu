/*
 * RISC-V IOMMU - Hardware Performance Monitor (HPM) helpers
 *
 * Copyright (C) 2022-2023 Rivos Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2 or later, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef HW_RISCV_IOMMU_HPM_H
#define HW_RISCV_IOMMU_HPM_H

#അന്തര്ഭാവ "qom/object.h"
#അന്തര്ഭാവ "hw/riscv/riscv-iommu.h"

uint64_t riscv_iommu_hpmcycle_read(RISCVIOMMUState *s);
ശൂന്യം riscv_iommu_hpm_incr_ctr(RISCVIOMMUState *s, RISCVIOMMUContext *ctx,
                              unsigned event_id);
ശൂന്യം riscv_iommu_hpm_timer_cb(ശൂന്യം *priv);
ശൂന്യം riscv_iommu_process_iocntinh_cy(RISCVIOMMUState *s, bool prev_cy_inh);
ശൂന്യം riscv_iommu_process_hpmcycle_write(RISCVIOMMUState *s);
ശൂന്യം riscv_iommu_process_hpmevt_write(RISCVIOMMUState *s, uint32_t evt_reg);

#endif
