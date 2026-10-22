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

#அந்தர்பாவ "qemu/osdep.h"
#அந்தர்பாவ "qemu/units.h"
#அந்தர்பாவ "qemu/error-report.h"
#அந்தர்பாவ "qapi/error.h"
#அந்தர்பாவ "hw/core/boards.h"
#அந்தர்பாவ "hw/core/qdev-properties.h"
#அந்தர்பாவ "hw/riscv/numa.h"
#அந்தர்பாவ "system/device_tree.h"

bool riscv_numa_enabled(மாறிலி MachineState *ms)
{
    திரும்பு (ms->numa_state && ms->numa_state->இலக்கம்_nodes) ? true : false;
}

முழு riscv_socket_மொத்தம்(மாறிலி MachineState *ms)
{
    திரும்பு (riscv_numa_enabled(ms)) ? ms->numa_state->இலக்கம்_nodes : 1;
}

முழு riscv_socket_first_hartid(மாறிலி MachineState *ms, முழு socket_id)
{
    முழு i, first_hartid = ms->smp.cpus;

    எனில் (!riscv_numa_enabled(ms)) {
        திரும்பு (!socket_id) ? 0 : -1;
    }

    க்கு (i = 0; i < ms->smp.cpus; i++) {
        எனில் (ms->possible_cpus->cpus[i].props.முனை_id != socket_id) {
            தொடர்;
        }
        எனில் (i < first_hartid) {
            first_hartid = i;
        }
    }

    திரும்பு (first_hartid < ms->smp.cpus) ? first_hartid : -1;
}

முழு riscv_socket_last_hartid(மாறிலி MachineState *ms, முழு socket_id)
{
    முழு i, last_hartid = -1;

    எனில் (!riscv_numa_enabled(ms)) {
        திரும்பு (!socket_id) ? ms->smp.cpus - 1 : -1;
    }

    க்கு (i = 0; i < ms->smp.cpus; i++) {
        எனில் (ms->possible_cpus->cpus[i].props.முனை_id != socket_id) {
            தொடர்;
        }
        எனில் (i > last_hartid) {
            last_hartid = i;
        }
    }

    திரும்பு (last_hartid < ms->smp.cpus) ? last_hartid : -1;
}

முழு riscv_socket_hart_மொத்தம்(மாறிலி MachineState *ms, முழு socket_id)
{
    முழு first_hartid, last_hartid;

    எனில் (!riscv_numa_enabled(ms)) {
        திரும்பு (!socket_id) ? ms->smp.cpus : -1;
    }

    first_hartid = riscv_socket_first_hartid(ms, socket_id);
    எனில் (first_hartid < 0) {
        திரும்பு -1;
    }

    last_hartid = riscv_socket_last_hartid(ms, socket_id);
    எனில் (last_hartid < 0) {
        திரும்பு -1;
    }

    எனில் (first_hartid > last_hartid) {
        திரும்பு -1;
    }

    திரும்பு last_hartid - first_hartid + 1;
}

bool riscv_socket_check_hartids(மாறிலி MachineState *ms, முழு socket_id)
{
    முழு i, first_hartid, last_hartid;

    எனில் (!riscv_numa_enabled(ms)) {
        திரும்பு (!socket_id) ? true : false;
    }

    first_hartid = riscv_socket_first_hartid(ms, socket_id);
    எனில் (first_hartid < 0) {
        திரும்பு false;
    }

    last_hartid = riscv_socket_last_hartid(ms, socket_id);
    எனில் (last_hartid < 0) {
        திரும்பு false;
    }

    க்கு (i = first_hartid; i <= last_hartid; i++) {
        எனில் (ms->possible_cpus->cpus[i].props.முனை_id != socket_id) {
            திரும்பு false;
        }
    }

    திரும்பு true;
}

uint64_t riscv_socket_mem_offset(மாறிலி MachineState *ms, முழு socket_id)
{
    முழு i;
    uint64_t mem_offset = 0;

    எனில் (!riscv_numa_enabled(ms)) {
        திரும்பு 0;
    }

    க்கு (i = 0; i < ms->numa_state->இலக்கம்_nodes; i++) {
        எனில் (i == socket_id) {
            நிறுத்து;
        }
        mem_offset += ms->numa_state->nodes[i].முனை_mem;
    }

    திரும்பு (i == socket_id) ? mem_offset : 0;
}

uint64_t riscv_socket_mem_size(மாறிலி MachineState *ms, முழு socket_id)
{
    எனில் (!riscv_numa_enabled(ms)) {
        திரும்பு (!socket_id) ? ms->ram_size : 0;
    }

    திரும்பு (socket_id < ms->numa_state->இலக்கம்_nodes) ?
            ms->numa_state->nodes[socket_id].முனை_mem : 0;
}

வெறுமை riscv_socket_fdt_write_id(மாறிலி MachineState *ms, மாறிலி எழுத்து *முனை_name,
                               முழு socket_id)
{
    எனில் (riscv_numa_enabled(ms)) {
        qemu_fdt_setprop_cell(ms->fdt, முனை_name, "numa-node-id", socket_id);
    }
}

வெறுமை riscv_socket_fdt_write_distance_matrix(மாறிலி MachineState *ms)
{
    முழு i, j, idx;
    g_autofree uint32_t *dist_matrix = வெற்று;
    uint32_t dist_matrix_size;

    எனில் (riscv_numa_enabled(ms) && ms->numa_state->have_numa_distance) {
        dist_matrix_size = riscv_socket_மொத்தம்(ms) * riscv_socket_மொத்தம்(ms);
        dist_matrix_size *= (3 * அளவு(uint32_t));
        dist_matrix = g_malloc0(dist_matrix_size);

        க்கு (i = 0; i < riscv_socket_மொத்தம்(ms); i++) {
            க்கு (j = 0; j < riscv_socket_மொத்தம்(ms); j++) {
                idx = (i * riscv_socket_மொத்தம்(ms) + j) * 3;
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
    மாறிலி CPUArchIdList *possible_cpus = mc->possible_cpu_arch_ids(ms);

    ப்ரதிஜ்ஞா(cpu_index < possible_cpus->len);
    திரும்பு possible_cpus->cpus[cpu_index].props;
}

int64_t riscv_numa_get_default_cpu_முனை_id(மாறிலி MachineState *ms, முழு idx)
{
    int64_t nidx = 0;

    எனில் (ms->numa_state->இலக்கம்_nodes > ms->smp.cpus) {
        error_report("Number of NUMA nodes (%d)"
                     " cannot exceed the number of available CPUs (%u).",
                     ms->numa_state->இலக்கம்_nodes, ms->smp.cpus);
        exit(EXIT_FAILURE);
    }
    எனில் (ms->numa_state->இலக்கம்_nodes) {
        nidx = idx / (ms->smp.cpus / ms->numa_state->இலக்கம்_nodes);
        எனில் (ms->numa_state->இலக்கம்_nodes <= nidx) {
            nidx = ms->numa_state->இலக்கம்_nodes - 1;
        }
    }

    திரும்பு nidx;
}

மாறிலி CPUArchIdList *riscv_numa_possible_cpu_arch_ids(MachineState *ms)
{
    முழு n;
    unsigned முழு max_cpus = ms->smp.max_cpus;

    எனில் (ms->possible_cpus) {
        ப்ரதிஜ்ஞா(ms->possible_cpus->len == max_cpus);
        திரும்பு ms->possible_cpus;
    }

    ms->possible_cpus = g_malloc0(அளவு(CPUArchIdList) +
                                  அளவு(CPUArchId) * max_cpus);
    ms->possible_cpus->len = max_cpus;
    க்கு (n = 0; n < ms->possible_cpus->len; n++) {
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

    திரும்பு ms->possible_cpus;
}
