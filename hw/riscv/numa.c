/*
 * QEMU RISC-V NUMA Helper
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2 or later, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ಅನ್ತರ್ಭಾವ "qemu/osdep.h"
#ಅನ್ತರ್ಭಾವ "qemu/units.h"
#ಅನ್ತರ್ಭಾವ "qemu/error-report.h"
#ಅನ್ತರ್ಭಾವ "qapi/error.h"
#ಅನ್ತರ್ಭಾವ "hw/core/boards.h"
#ಅನ್ತರ್ಭಾವ "hw/core/qdev-properties.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/numa.h"
#ಅನ್ತರ್ಭಾವ "system/device_tree.h"

bool riscv_numa_enabled(ಸ್ಥಿರಾಂಕ MachineState *ms)
{
    ಹಿಂತಿರುಗಿ (ms->numa_state && ms->numa_state->ಅಂಕೆ_nodes) ? true : false;
}

ಪೂರ್ಣಾಂಕ riscv_socket_ಎಣಿಕೆ(ಸ್ಥಿರಾಂಕ MachineState *ms)
{
    ಹಿಂತಿರುಗಿ (riscv_numa_enabled(ms)) ? ms->numa_state->ಅಂಕೆ_nodes : 1;
}

ಪೂರ್ಣಾಂಕ riscv_socket_ಮೊದಲ_hartid(ಸ್ಥಿರಾಂಕ MachineState *ms, ಪೂರ್ಣಾಂಕ socket_id)
{
    ಪೂರ್ಣಾಂಕ i, ಮೊದಲ_hartid = ms->smp.cpus;

    ಆದರೆ (!riscv_numa_enabled(ms)) {
        ಹಿಂತಿರುಗಿ (!socket_id) ? 0 : -1;
    }

    ಗಾಗಿ (i = 0; i < ms->smp.cpus; i++) {
        ಆದರೆ (ms->possible_cpus->cpus[i].props.ಗಂಟು_id != socket_id) {
            ಮುಂದುವರಿ;
        }
        ಆದರೆ (i < ಮೊದಲ_hartid) {
            ಮೊದಲ_hartid = i;
        }
    }

    ಹಿಂತಿರುಗಿ (ಮೊದಲ_hartid < ms->smp.cpus) ? ಮೊದಲ_hartid : -1;
}

ಪೂರ್ಣಾಂಕ riscv_socket_ಕೊನೆಯ_hartid(ಸ್ಥಿರಾಂಕ MachineState *ms, ಪೂರ್ಣಾಂಕ socket_id)
{
    ಪೂರ್ಣಾಂಕ i, ಕೊನೆಯ_hartid = -1;

    ಆದರೆ (!riscv_numa_enabled(ms)) {
        ಹಿಂತಿರುಗಿ (!socket_id) ? ms->smp.cpus - 1 : -1;
    }

    ಗಾಗಿ (i = 0; i < ms->smp.cpus; i++) {
        ಆದರೆ (ms->possible_cpus->cpus[i].props.ಗಂಟು_id != socket_id) {
            ಮುಂದುವರಿ;
        }
        ಆದರೆ (i > ಕೊನೆಯ_hartid) {
            ಕೊನೆಯ_hartid = i;
        }
    }

    ಹಿಂತಿರುಗಿ (ಕೊನೆಯ_hartid < ms->smp.cpus) ? ಕೊನೆಯ_hartid : -1;
}

ಪೂರ್ಣಾಂಕ riscv_socket_hart_ಎಣಿಕೆ(ಸ್ಥಿರಾಂಕ MachineState *ms, ಪೂರ್ಣಾಂಕ socket_id)
{
    ಪೂರ್ಣಾಂಕ ಮೊದಲ_hartid, ಕೊನೆಯ_hartid;

    ಆದರೆ (!riscv_numa_enabled(ms)) {
        ಹಿಂತಿರುಗಿ (!socket_id) ? ms->smp.cpus : -1;
    }

    ಮೊದಲ_hartid = riscv_socket_ಮೊದಲ_hartid(ms, socket_id);
    ಆದರೆ (ಮೊದಲ_hartid < 0) {
        ಹಿಂತಿರುಗಿ -1;
    }

    ಕೊನೆಯ_hartid = riscv_socket_ಕೊನೆಯ_hartid(ms, socket_id);
    ಆದರೆ (ಕೊನೆಯ_hartid < 0) {
        ಹಿಂತಿರುಗಿ -1;
    }

    ಆದರೆ (ಮೊದಲ_hartid > ಕೊನೆಯ_hartid) {
        ಹಿಂತಿರುಗಿ -1;
    }

    ಹಿಂತಿರುಗಿ ಕೊನೆಯ_hartid - ಮೊದಲ_hartid + 1;
}

bool riscv_socket_check_hartids(ಸ್ಥಿರಾಂಕ MachineState *ms, ಪೂರ್ಣಾಂಕ socket_id)
{
    ಪೂರ್ಣಾಂಕ i, ಮೊದಲ_hartid, ಕೊನೆಯ_hartid;

    ಆದರೆ (!riscv_numa_enabled(ms)) {
        ಹಿಂತಿರುಗಿ (!socket_id) ? true : false;
    }

    ಮೊದಲ_hartid = riscv_socket_ಮೊದಲ_hartid(ms, socket_id);
    ಆದರೆ (ಮೊದಲ_hartid < 0) {
        ಹಿಂತಿರುಗಿ false;
    }

    ಕೊನೆಯ_hartid = riscv_socket_ಕೊನೆಯ_hartid(ms, socket_id);
    ಆದರೆ (ಕೊನೆಯ_hartid < 0) {
        ಹಿಂತಿರುಗಿ false;
    }

    ಗಾಗಿ (i = ಮೊದಲ_hartid; i <= ಕೊನೆಯ_hartid; i++) {
        ಆದರೆ (ms->possible_cpus->cpus[i].props.ಗಂಟು_id != socket_id) {
            ಹಿಂತಿರುಗಿ false;
        }
    }

    ಹಿಂತಿರುಗಿ true;
}

uint64_t riscv_socket_mem_ಸ್ಥಾನಾಂತರ(ಸ್ಥಿರಾಂಕ MachineState *ms, ಪೂರ್ಣಾಂಕ socket_id)
{
    ಪೂರ್ಣಾಂಕ i;
    uint64_t mem_ಸ್ಥಾನಾಂತರ = 0;

    ಆದರೆ (!riscv_numa_enabled(ms)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    ಗಾಗಿ (i = 0; i < ms->numa_state->ಅಂಕೆ_nodes; i++) {
        ಆದರೆ (i == socket_id) {
            ನಿಲ್ಲಿಸು;
        }
        mem_ಸ್ಥಾನಾಂತರ += ms->numa_state->nodes[i].ಗಂಟು_mem;
    }

    ಹಿಂತಿರುಗಿ (i == socket_id) ? mem_ಸ್ಥಾನಾಂತರ : 0;
}

uint64_t riscv_socket_mem_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ MachineState *ms, ಪೂರ್ಣಾಂಕ socket_id)
{
    ಆದರೆ (!riscv_numa_enabled(ms)) {
        ಹಿಂತಿರುಗಿ (!socket_id) ? ms->ram_ಗಾತ್ರ : 0;
    }

    ಹಿಂತಿರುಗಿ (socket_id < ms->numa_state->ಅಂಕೆ_nodes) ?
            ms->numa_state->nodes[socket_id].ಗಂಟು_mem : 0;
}

ಶೂನ್ಯ riscv_socket_fdt_write_id(ಸ್ಥಿರಾಂಕ MachineState *ms, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗಂಟು_name,
                               ಪೂರ್ಣಾಂಕ socket_id)
{
    ಆದರೆ (riscv_numa_enabled(ms)) {
        qemu_fdt_setprop_cell(ms->fdt, ಗಂಟು_name, "numa-node-id", socket_id);
    }
}

ಶೂನ್ಯ riscv_socket_fdt_write_ದೂರ_ಮಾತೃಕೆ(ಸ್ಥಿರಾಂಕ MachineState *ms)
{
    ಪೂರ್ಣಾಂಕ i, j, idx;
    g_autofree uint32_t *dist_ಮಾತೃಕೆ = ಖಾಲಿ;
    uint32_t dist_ಮಾತೃಕೆ_ಗಾತ್ರ;

    ಆದರೆ (riscv_numa_enabled(ms) && ms->numa_state->have_numa_ದೂರ) {
        dist_ಮಾತೃಕೆ_ಗಾತ್ರ = riscv_socket_ಎಣಿಕೆ(ms) * riscv_socket_ಎಣಿಕೆ(ms);
        dist_ಮಾತೃಕೆ_ಗಾತ್ರ *= (3 * ಗಾತ್ರ(uint32_t));
        dist_ಮಾತೃಕೆ = g_malloc0(dist_ಮಾತೃಕೆ_ಗಾತ್ರ);

        ಗಾಗಿ (i = 0; i < riscv_socket_ಎಣಿಕೆ(ms); i++) {
            ಗಾಗಿ (j = 0; j < riscv_socket_ಎಣಿಕೆ(ms); j++) {
                idx = (i * riscv_socket_ಎಣಿಕೆ(ms) + j) * 3;
                dist_ಮಾತೃಕೆ[idx + 0] = cpu_ಗಮ್ಯ_be32(i);
                dist_ಮಾತೃಕೆ[idx + 1] = cpu_ಗಮ್ಯ_be32(j);
                dist_ಮಾತೃಕೆ[idx + 2] =
                    cpu_ಗಮ್ಯ_be32(ms->numa_state->nodes[i].ದೂರ[j]);
            }
        }

        qemu_fdt_ಕೂಡಿಸು_subnode(ms->fdt, "/distance-map");
        qemu_fdt_setprop_string(ms->fdt, "/distance-map", "compatible",
                                "numa-distance-map-v1");
        qemu_fdt_setprop(ms->fdt, "/distance-map", "distance-matrix",
                         dist_ಮಾತೃಕೆ, dist_ಮಾತೃಕೆ_ಗಾತ್ರ);
    }
}

CpuInstanceProperties
riscv_numa_cpu_ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_props(MachineState *ms, ಚಿಹ್ನೆರಹಿತ cpu_ಸೂಚ್ಯಂಕ)
{
    MachineClass *mc = MACHINE_GET_CLASS(ms);
    ಸ್ಥಿರಾಂಕ CPUArchIdList *possible_cpus = mc->possible_cpu_arch_ids(ms);

    ಪ್ರತಿಜ್ಞಾ(cpu_ಸೂಚ್ಯಂಕ < possible_cpus->len);
    ಹಿಂತಿರುಗಿ possible_cpus->cpus[cpu_ಸೂಚ್ಯಂಕ].props;
}

int64_t riscv_numa_get_default_cpu_ಗಂಟು_id(ಸ್ಥಿರಾಂಕ MachineState *ms, ಪೂರ್ಣಾಂಕ idx)
{
    int64_t nidx = 0;

    ಆದರೆ (ms->numa_state->ಅಂಕೆ_nodes > ms->smp.cpus) {
        error_report("Number of NUMA nodes (%d)"
                     " cannot exceed the number of available CPUs (%u).",
                     ms->numa_state->ಅಂಕೆ_nodes, ms->smp.cpus);
        ನಿರ್ಗಮಿಸು(EXIT_FAILURE);
    }
    ಆದರೆ (ms->numa_state->ಅಂಕೆ_nodes) {
        nidx = idx / (ms->smp.cpus / ms->numa_state->ಅಂಕೆ_nodes);
        ಆದರೆ (ms->numa_state->ಅಂಕೆ_nodes <= nidx) {
            nidx = ms->numa_state->ಅಂಕೆ_nodes - 1;
        }
    }

    ಹಿಂತಿರುಗಿ nidx;
}

ಸ್ಥಿರಾಂಕ CPUArchIdList *riscv_numa_possible_cpu_arch_ids(MachineState *ms)
{
    ಪೂರ್ಣಾಂಕ n;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ max_cpus = ms->smp.max_cpus;

    ಆದರೆ (ms->possible_cpus) {
        ಪ್ರತಿಜ್ಞಾ(ms->possible_cpus->len == max_cpus);
        ಹಿಂತಿರುಗಿ ms->possible_cpus;
    }

    ms->possible_cpus = g_malloc0(ಗಾತ್ರ(CPUArchIdList) +
                                  ಗಾತ್ರ(CPUArchId) * max_cpus);
    ms->possible_cpus->len = max_cpus;
    ಗಾಗಿ (n = 0; n < ms->possible_cpus->len; n++) {
        ms->possible_cpus->cpus[n].type = ms->cpu_type;
        ms->possible_cpus->cpus[n].arch_id = n;
        ms->possible_cpus->cpus[n].props.has_socket_id = true;
        ms->possible_cpus->cpus[n].props.socket_id =
            n / (ms->smp.clusters * ms->smp.cores * ms->smp.ಎಳೆಗಳು);
        ms->possible_cpus->cpus[n].props.has_cluster_id = true;
        ms->possible_cpus->cpus[n].props.cluster_id =
            (n / (ms->smp.cores * ms->smp.ಎಳೆಗಳು)) % ms->smp.clusters;
        ms->possible_cpus->cpus[n].props.has_core_id = true;
        ms->possible_cpus->cpus[n].props.core_id =
            (n / ms->smp.ಎಳೆಗಳು) % ms->smp.cores;
        ms->possible_cpus->cpus[n].props.has_thread_id = true;
        ms->possible_cpus->cpus[n].props.thread_id =
            n % ms->smp.ಎಳೆಗಳು;
    }

    ಹಿಂತಿರುಗಿ ms->possible_cpus;
}
