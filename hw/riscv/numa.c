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

#অন্তর্ভাব "qemu/osdep.h"
#অন্তর্ভাব "qemu/units.h"
#অন্তর্ভাব "qemu/error-report.h"
#অন্তর্ভাব "qapi/error.h"
#অন্তর্ভাব "hw/core/boards.h"
#অন্তর্ভাব "hw/core/qdev-properties.h"
#অন্তর্ভাব "hw/riscv/numa.h"
#অন্তর্ভাব "system/device_tree.h"

bool riscv_numa_enabled(ধ্রুবক MachineState *ms)
{
    ফেরত (ms->numa_state && ms->numa_state->অঙ্ক_nodes) ? true : false;
}

পূর্ণসংখ্যা riscv_socket_গুনতি(ধ্রুবক MachineState *ms)
{
    ফেরত (riscv_numa_enabled(ms)) ? ms->numa_state->অঙ্ক_nodes : 1;
}

পূর্ণসংখ্যা riscv_socket_first_hartid(ধ্রুবক MachineState *ms, পূর্ণসংখ্যা socket_id)
{
    পূর্ণসংখ্যা i, first_hartid = ms->smp.cpus;

    যদি (!riscv_numa_enabled(ms)) {
        ফেরত (!socket_id) ? 0 : -1;
    }

    জন্য (i = 0; i < ms->smp.cpus; i++) {
        যদি (ms->possible_cpus->cpus[i].props.গ্রন্থি_id != socket_id) {
            চালু;
        }
        যদি (i < first_hartid) {
            first_hartid = i;
        }
    }

    ফেরত (first_hartid < ms->smp.cpus) ? first_hartid : -1;
}

পূর্ণসংখ্যা riscv_socket_last_hartid(ধ্রুবক MachineState *ms, পূর্ণসংখ্যা socket_id)
{
    পূর্ণসংখ্যা i, last_hartid = -1;

    যদি (!riscv_numa_enabled(ms)) {
        ফেরত (!socket_id) ? ms->smp.cpus - 1 : -1;
    }

    জন্য (i = 0; i < ms->smp.cpus; i++) {
        যদি (ms->possible_cpus->cpus[i].props.গ্রন্থি_id != socket_id) {
            চালু;
        }
        যদি (i > last_hartid) {
            last_hartid = i;
        }
    }

    ফেরত (last_hartid < ms->smp.cpus) ? last_hartid : -1;
}

পূর্ণসংখ্যা riscv_socket_hart_গুনতি(ধ্রুবক MachineState *ms, পূর্ণসংখ্যা socket_id)
{
    পূর্ণসংখ্যা first_hartid, last_hartid;

    যদি (!riscv_numa_enabled(ms)) {
        ফেরত (!socket_id) ? ms->smp.cpus : -1;
    }

    first_hartid = riscv_socket_first_hartid(ms, socket_id);
    যদি (first_hartid < 0) {
        ফেরত -1;
    }

    last_hartid = riscv_socket_last_hartid(ms, socket_id);
    যদি (last_hartid < 0) {
        ফেরত -1;
    }

    যদি (first_hartid > last_hartid) {
        ফেরত -1;
    }

    ফেরত last_hartid - first_hartid + 1;
}

bool riscv_socket_check_hartids(ধ্রুবক MachineState *ms, পূর্ণসংখ্যা socket_id)
{
    পূর্ণসংখ্যা i, first_hartid, last_hartid;

    যদি (!riscv_numa_enabled(ms)) {
        ফেরত (!socket_id) ? true : false;
    }

    first_hartid = riscv_socket_first_hartid(ms, socket_id);
    যদি (first_hartid < 0) {
        ফেরত false;
    }

    last_hartid = riscv_socket_last_hartid(ms, socket_id);
    যদি (last_hartid < 0) {
        ফেরত false;
    }

    জন্য (i = first_hartid; i <= last_hartid; i++) {
        যদি (ms->possible_cpus->cpus[i].props.গ্রন্থি_id != socket_id) {
            ফেরত false;
        }
    }

    ফেরত true;
}

uint64_t riscv_socket_mem_offset(ধ্রুবক MachineState *ms, পূর্ণসংখ্যা socket_id)
{
    পূর্ণসংখ্যা i;
    uint64_t mem_offset = 0;

    যদি (!riscv_numa_enabled(ms)) {
        ফেরত 0;
    }

    জন্য (i = 0; i < ms->numa_state->অঙ্ক_nodes; i++) {
        যদি (i == socket_id) {
            বিরতি;
        }
        mem_offset += ms->numa_state->nodes[i].গ্রন্থি_mem;
    }

    ফেরত (i == socket_id) ? mem_offset : 0;
}

uint64_t riscv_socket_mem_আকার(ধ্রুবক MachineState *ms, পূর্ণসংখ্যা socket_id)
{
    যদি (!riscv_numa_enabled(ms)) {
        ফেরত (!socket_id) ? ms->ram_আকার : 0;
    }

    ফেরত (socket_id < ms->numa_state->অঙ্ক_nodes) ?
            ms->numa_state->nodes[socket_id].গ্রন্থি_mem : 0;
}

শূন্য riscv_socket_fdt_write_id(ধ্রুবক MachineState *ms, ধ্রুবক অক্ষর *গ্রন্থি_name,
                               পূর্ণসংখ্যা socket_id)
{
    যদি (riscv_numa_enabled(ms)) {
        qemu_fdt_setprop_cell(ms->fdt, গ্রন্থি_name, "numa-node-id", socket_id);
    }
}

শূন্য riscv_socket_fdt_write_distance_matrix(ধ্রুবক MachineState *ms)
{
    পূর্ণসংখ্যা i, j, idx;
    g_autofree uint32_t *dist_matrix = ফাঁকা;
    uint32_t dist_matrix_আকার;

    যদি (riscv_numa_enabled(ms) && ms->numa_state->have_numa_distance) {
        dist_matrix_আকার = riscv_socket_গুনতি(ms) * riscv_socket_গুনতি(ms);
        dist_matrix_আকার *= (3 * আকার(uint32_t));
        dist_matrix = g_malloc0(dist_matrix_আকার);

        জন্য (i = 0; i < riscv_socket_গুনতি(ms); i++) {
            জন্য (j = 0; j < riscv_socket_গুনতি(ms); j++) {
                idx = (i * riscv_socket_গুনতি(ms) + j) * 3;
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
                         dist_matrix, dist_matrix_আকার);
    }
}

CpuInstanceProperties
riscv_numa_cpu_সূচক_to_props(MachineState *ms, unsigned cpu_সূচক)
{
    MachineClass *mc = MACHINE_GET_CLASS(ms);
    ধ্রুবক CPUArchIdList *possible_cpus = mc->possible_cpu_arch_ids(ms);

    প্রতিজ্ঞা(cpu_সূচক < possible_cpus->len);
    ফেরত possible_cpus->cpus[cpu_সূচক].props;
}

int64_t riscv_numa_get_default_cpu_গ্রন্থি_id(ধ্রুবক MachineState *ms, পূর্ণসংখ্যা idx)
{
    int64_t nidx = 0;

    যদি (ms->numa_state->অঙ্ক_nodes > ms->smp.cpus) {
        error_report("Number of NUMA nodes (%d)"
                     " cannot exceed the number of available CPUs (%u).",
                     ms->numa_state->অঙ্ক_nodes, ms->smp.cpus);
        exit(EXIT_FAILURE);
    }
    যদি (ms->numa_state->অঙ্ক_nodes) {
        nidx = idx / (ms->smp.cpus / ms->numa_state->অঙ্ক_nodes);
        যদি (ms->numa_state->অঙ্ক_nodes <= nidx) {
            nidx = ms->numa_state->অঙ্ক_nodes - 1;
        }
    }

    ফেরত nidx;
}

ধ্রুবক CPUArchIdList *riscv_numa_possible_cpu_arch_ids(MachineState *ms)
{
    পূর্ণসংখ্যা n;
    unsigned পূর্ণসংখ্যা max_cpus = ms->smp.max_cpus;

    যদি (ms->possible_cpus) {
        প্রতিজ্ঞা(ms->possible_cpus->len == max_cpus);
        ফেরত ms->possible_cpus;
    }

    ms->possible_cpus = g_malloc0(আকার(CPUArchIdList) +
                                  আকার(CPUArchId) * max_cpus);
    ms->possible_cpus->len = max_cpus;
    জন্য (n = 0; n < ms->possible_cpus->len; n++) {
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

    ফেরত ms->possible_cpus;
}
