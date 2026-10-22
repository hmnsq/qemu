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

bool riscv_numa_enabled(ಧ್ರುವ MachineState *ms)
{
    ಪ್ರತ್ಯಾಗಮ (ms->numa_state && ms->numa_state->ಅಂಕೆ_nodes) ? true : false;
}

ಪೂರ್ಣಾಙ್ಕ riscv_socket_ಎಣಿಕೆ(ಧ್ರುವ MachineState *ms)
{
    ಪ್ರತ್ಯಾಗಮ (riscv_numa_enabled(ms)) ? ms->numa_state->ಅಂಕೆ_nodes : 1;
}

ಪೂರ್ಣಾಙ್ಕ riscv_socket_first_hartid(ಧ್ರುವ MachineState *ms, ಪೂರ್ಣಾಙ್ಕ socket_id)
{
    ಪೂರ್ಣಾಙ್ಕ i, first_hartid = ms->smp.cpus;

    ಯದಿ (!riscv_numa_enabled(ms)) {
        ಪ್ರತ್ಯಾಗಮ (!socket_id) ? 0 : -1;
    }

    ಪ್ರತಿ (i = 0; i < ms->smp.cpus; i++) {
        ಯದಿ (ms->possible_cpus->cpus[i].props.ಗಂಟು_id != socket_id) {
            ಅನುವರ್ತ;
        }
        ಯದಿ (i < first_hartid) {
            first_hartid = i;
        }
    }

    ಪ್ರತ್ಯಾಗಮ (first_hartid < ms->smp.cpus) ? first_hartid : -1;
}

ಪೂರ್ಣಾಙ್ಕ riscv_socket_last_hartid(ಧ್ರುವ MachineState *ms, ಪೂರ್ಣಾಙ್ಕ socket_id)
{
    ಪೂರ್ಣಾಙ್ಕ i, last_hartid = -1;

    ಯದಿ (!riscv_numa_enabled(ms)) {
        ಪ್ರತ್ಯಾಗಮ (!socket_id) ? ms->smp.cpus - 1 : -1;
    }

    ಪ್ರತಿ (i = 0; i < ms->smp.cpus; i++) {
        ಯದಿ (ms->possible_cpus->cpus[i].props.ಗಂಟು_id != socket_id) {
            ಅನುವರ್ತ;
        }
        ಯದಿ (i > last_hartid) {
            last_hartid = i;
        }
    }

    ಪ್ರತ್ಯಾಗಮ (last_hartid < ms->smp.cpus) ? last_hartid : -1;
}

ಪೂರ್ಣಾಙ್ಕ riscv_socket_hart_ಎಣಿಕೆ(ಧ್ರುವ MachineState *ms, ಪೂರ್ಣಾಙ್ಕ socket_id)
{
    ಪೂರ್ಣಾಙ್ಕ first_hartid, last_hartid;

    ಯದಿ (!riscv_numa_enabled(ms)) {
        ಪ್ರತ್ಯಾಗಮ (!socket_id) ? ms->smp.cpus : -1;
    }

    first_hartid = riscv_socket_first_hartid(ms, socket_id);
    ಯದಿ (first_hartid < 0) {
        ಪ್ರತ್ಯಾಗಮ -1;
    }

    last_hartid = riscv_socket_last_hartid(ms, socket_id);
    ಯದಿ (last_hartid < 0) {
        ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಯದಿ (first_hartid > last_hartid) {
        ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಪ್ರತ್ಯಾಗಮ last_hartid - first_hartid + 1;
}

bool riscv_socket_check_hartids(ಧ್ರುವ MachineState *ms, ಪೂರ್ಣಾಙ್ಕ socket_id)
{
    ಪೂರ್ಣಾಙ್ಕ i, first_hartid, last_hartid;

    ಯದಿ (!riscv_numa_enabled(ms)) {
        ಪ್ರತ್ಯಾಗಮ (!socket_id) ? true : false;
    }

    first_hartid = riscv_socket_first_hartid(ms, socket_id);
    ಯದಿ (first_hartid < 0) {
        ಪ್ರತ್ಯಾಗಮ false;
    }

    last_hartid = riscv_socket_last_hartid(ms, socket_id);
    ಯದಿ (last_hartid < 0) {
        ಪ್ರತ್ಯಾಗಮ false;
    }

    ಪ್ರತಿ (i = first_hartid; i <= last_hartid; i++) {
        ಯದಿ (ms->possible_cpus->cpus[i].props.ಗಂಟು_id != socket_id) {
            ಪ್ರತ್ಯಾಗಮ false;
        }
    }

    ಪ್ರತ್ಯಾಗಮ true;
}

uint64_t riscv_socket_mem_offset(ಧ್ರುವ MachineState *ms, ಪೂರ್ಣಾಙ್ಕ socket_id)
{
    ಪೂರ್ಣಾಙ್ಕ i;
    uint64_t mem_offset = 0;

    ಯದಿ (!riscv_numa_enabled(ms)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಪ್ರತಿ (i = 0; i < ms->numa_state->ಅಂಕೆ_nodes; i++) {
        ಯದಿ (i == socket_id) {
            ಭಙ್ಗ;
        }
        mem_offset += ms->numa_state->nodes[i].ಗಂಟು_mem;
    }

    ಪ್ರತ್ಯಾಗಮ (i == socket_id) ? mem_offset : 0;
}

uint64_t riscv_socket_mem_size(ಧ್ರುವ MachineState *ms, ಪೂರ್ಣಾಙ್ಕ socket_id)
{
    ಯದಿ (!riscv_numa_enabled(ms)) {
        ಪ್ರತ್ಯಾಗಮ (!socket_id) ? ms->ram_size : 0;
    }

    ಪ್ರತ್ಯಾಗಮ (socket_id < ms->numa_state->ಅಂಕೆ_nodes) ?
            ms->numa_state->nodes[socket_id].ಗಂಟು_mem : 0;
}

ಶೂನ್ಯ riscv_socket_fdt_write_id(ಧ್ರುವ MachineState *ms, ಧ್ರುವ ವರ್ಣ *ಗಂಟು_name,
                               ಪೂರ್ಣಾಙ್ಕ socket_id)
{
    ಯದಿ (riscv_numa_enabled(ms)) {
        qemu_fdt_setprop_cell(ms->fdt, ಗಂಟು_name, "numa-node-id", socket_id);
    }
}

ಶೂನ್ಯ riscv_socket_fdt_write_distance_matrix(ಧ್ರುವ MachineState *ms)
{
    ಪೂರ್ಣಾಙ್ಕ i, j, idx;
    g_autofree uint32_t *dist_matrix = ರಿಕ್ತ;
    uint32_t dist_matrix_size;

    ಯದಿ (riscv_numa_enabled(ms) && ms->numa_state->have_numa_distance) {
        dist_matrix_size = riscv_socket_ಎಣಿಕೆ(ms) * riscv_socket_ಎಣಿಕೆ(ms);
        dist_matrix_size *= (3 * ಪರಿಮಾಣ(uint32_t));
        dist_matrix = g_malloc0(dist_matrix_size);

        ಪ್ರತಿ (i = 0; i < riscv_socket_ಎಣಿಕೆ(ms); i++) {
            ಪ್ರತಿ (j = 0; j < riscv_socket_ಎಣಿಕೆ(ms); j++) {
                idx = (i * riscv_socket_ಎಣಿಕೆ(ms) + j) * 3;
                dist_matrix[idx + 0] = cpu_to_be32(i);
                dist_matrix[idx + 1] = cpu_to_be32(j);
                dist_matrix[idx + 2] =
                    cpu_to_be32(ms->numa_state->nodes[i].distance[j]);
            }
        }

        qemu_fdt_add_subnode(ms->fdt, "/distance-map");
        qemu_fdt_setprop_string(ms->fdt, "/distance-map", "compatible",
                                "numa-distance-map-v1");
        qemu_fdt_setprop(ms->fdt, "/distance-map", "distance-matrix",
                         dist_matrix, dist_matrix_size);
    }
}

CpuInstanceProperties
riscv_numa_cpu_index_to_props(MachineState *ms, unsigned cpu_index)
{
    MachineClass *mc = MACHINE_GET_CLASS(ms);
    ಧ್ರುವ CPUArchIdList *possible_cpus = mc->possible_cpu_arch_ids(ms);

    ಪ್ರತಿಜ್ಞಾ(cpu_index < possible_cpus->len);
    ಪ್ರತ್ಯಾಗಮ possible_cpus->cpus[cpu_index].props;
}

int64_t riscv_numa_get_default_cpu_ಗಂಟು_id(ಧ್ರುವ MachineState *ms, ಪೂರ್ಣಾಙ್ಕ idx)
{
    int64_t nidx = 0;

    ಯದಿ (ms->numa_state->ಅಂಕೆ_nodes > ms->smp.cpus) {
        error_report("Number of NUMA nodes (%d)"
                     " cannot exceed the number of available CPUs (%u).",
                     ms->numa_state->ಅಂಕೆ_nodes, ms->smp.cpus);
        exit(EXIT_FAILURE);
    }
    ಯದಿ (ms->numa_state->ಅಂಕೆ_nodes) {
        nidx = idx / (ms->smp.cpus / ms->numa_state->ಅಂಕೆ_nodes);
        ಯದಿ (ms->numa_state->ಅಂಕೆ_nodes <= nidx) {
            nidx = ms->numa_state->ಅಂಕೆ_nodes - 1;
        }
    }

    ಪ್ರತ್ಯಾಗಮ nidx;
}

ಧ್ರುವ CPUArchIdList *riscv_numa_possible_cpu_arch_ids(MachineState *ms)
{
    ಪೂರ್ಣಾಙ್ಕ n;
    unsigned ಪೂರ್ಣಾಙ್ಕ max_cpus = ms->smp.max_cpus;

    ಯದಿ (ms->possible_cpus) {
        ಪ್ರತಿಜ್ಞಾ(ms->possible_cpus->len == max_cpus);
        ಪ್ರತ್ಯಾಗಮ ms->possible_cpus;
    }

    ms->possible_cpus = g_malloc0(ಪರಿಮಾಣ(CPUArchIdList) +
                                  ಪರಿಮಾಣ(CPUArchId) * max_cpus);
    ms->possible_cpus->len = max_cpus;
    ಪ್ರತಿ (n = 0; n < ms->possible_cpus->len; n++) {
        ms->possible_cpus->cpus[n].type = ms->cpu_type;
        ms->possible_cpus->cpus[n].arch_id = n;
        ms->possible_cpus->cpus[n].props.has_socket_id = true;
        ms->possible_cpus->cpus[n].props.socket_id =
            n / (ms->smp.clusters * ms->smp.cores * ms->smp.threads);
        ms->possible_cpus->cpus[n].props.has_cluster_id = true;
        ms->possible_cpus->cpus[n].props.cluster_id =
            (n / (ms->smp.cores * ms->smp.threads)) % ms->smp.clusters;
        ms->possible_cpus->cpus[n].props.has_core_id = true;
        ms->possible_cpus->cpus[n].props.core_id =
            (n / ms->smp.threads) % ms->smp.cores;
        ms->possible_cpus->cpus[n].props.has_thread_id = true;
        ms->possible_cpus->cpus[n].props.thread_id =
            n % ms->smp.threads;
    }

    ಪ್ರತ್ಯಾಗಮ ms->possible_cpus;
}
