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

#ଅନ୍ତର୍ଭାଵ "qemu/osdep.h"
#ଅନ୍ତର୍ଭାଵ "qemu/units.h"
#ଅନ୍ତର୍ଭାଵ "qemu/error-report.h"
#ଅନ୍ତର୍ଭାଵ "qapi/error.h"
#ଅନ୍ତର୍ଭାଵ "hw/core/boards.h"
#ଅନ୍ତର୍ଭାଵ "hw/core/qdev-properties.h"
#ଅନ୍ତର୍ଭାଵ "hw/riscv/numa.h"
#ଅନ୍ତର୍ଭାଵ "system/device_tree.h"

bool riscv_numa_enabled(ଅଚଳ MachineState *ms)
{
    ଫେରନ୍ତୁ (ms->numa_state && ms->numa_state->ଅଙ୍କ_nodes) ? true : false;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା riscv_socket_ଗଣନା(ଅଚଳ MachineState *ms)
{
    ଫେରନ୍ତୁ (riscv_numa_enabled(ms)) ? ms->numa_state->ଅଙ୍କ_nodes : 1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା riscv_socket_first_hartid(ଅଚଳ MachineState *ms, ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_id)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, first_hartid = ms->smp.cpus;

    ଯଦି (!riscv_numa_enabled(ms)) {
        ଫେରନ୍ତୁ (!socket_id) ? 0 : -1;
    }

    ପାଇଁ (i = 0; i < ms->smp.cpus; i++) {
        ଯଦି (ms->possible_cpus->cpus[i].props.ଗଣ୍ଠି_id != socket_id) {
            ଜାରି;
        }
        ଯଦି (i < first_hartid) {
            first_hartid = i;
        }
    }

    ଫେରନ୍ତୁ (first_hartid < ms->smp.cpus) ? first_hartid : -1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା riscv_socket_last_hartid(ଅଚଳ MachineState *ms, ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_id)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, last_hartid = -1;

    ଯଦି (!riscv_numa_enabled(ms)) {
        ଫେରନ୍ତୁ (!socket_id) ? ms->smp.cpus - 1 : -1;
    }

    ପାଇଁ (i = 0; i < ms->smp.cpus; i++) {
        ଯଦି (ms->possible_cpus->cpus[i].props.ଗଣ୍ଠି_id != socket_id) {
            ଜାରି;
        }
        ଯଦି (i > last_hartid) {
            last_hartid = i;
        }
    }

    ଫେରନ୍ତୁ (last_hartid < ms->smp.cpus) ? last_hartid : -1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା riscv_socket_hart_ଗଣନା(ଅଚଳ MachineState *ms, ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_id)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା first_hartid, last_hartid;

    ଯଦି (!riscv_numa_enabled(ms)) {
        ଫେରନ୍ତୁ (!socket_id) ? ms->smp.cpus : -1;
    }

    first_hartid = riscv_socket_first_hartid(ms, socket_id);
    ଯଦି (first_hartid < 0) {
        ଫେରନ୍ତୁ -1;
    }

    last_hartid = riscv_socket_last_hartid(ms, socket_id);
    ଯଦି (last_hartid < 0) {
        ଫେରନ୍ତୁ -1;
    }

    ଯଦି (first_hartid > last_hartid) {
        ଫେରନ୍ତୁ -1;
    }

    ଫେରନ୍ତୁ last_hartid - first_hartid + 1;
}

bool riscv_socket_check_hartids(ଅଚଳ MachineState *ms, ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_id)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, first_hartid, last_hartid;

    ଯଦି (!riscv_numa_enabled(ms)) {
        ଫେରନ୍ତୁ (!socket_id) ? true : false;
    }

    first_hartid = riscv_socket_first_hartid(ms, socket_id);
    ଯଦି (first_hartid < 0) {
        ଫେରନ୍ତୁ false;
    }

    last_hartid = riscv_socket_last_hartid(ms, socket_id);
    ଯଦି (last_hartid < 0) {
        ଫେରନ୍ତୁ false;
    }

    ପାଇଁ (i = first_hartid; i <= last_hartid; i++) {
        ଯଦି (ms->possible_cpus->cpus[i].props.ଗଣ୍ଠି_id != socket_id) {
            ଫେରନ୍ତୁ false;
        }
    }

    ଫେରନ୍ତୁ true;
}

uint64_t riscv_socket_mem_offset(ଅଚଳ MachineState *ms, ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_id)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    uint64_t mem_offset = 0;

    ଯଦି (!riscv_numa_enabled(ms)) {
        ଫେରନ୍ତୁ 0;
    }

    ପାଇଁ (i = 0; i < ms->numa_state->ଅଙ୍କ_nodes; i++) {
        ଯଦି (i == socket_id) {
            ଭାଙ୍ଗ;
        }
        mem_offset += ms->numa_state->nodes[i].ଗଣ୍ଠି_mem;
    }

    ଫେରନ୍ତୁ (i == socket_id) ? mem_offset : 0;
}

uint64_t riscv_socket_mem_size(ଅଚଳ MachineState *ms, ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_id)
{
    ଯଦି (!riscv_numa_enabled(ms)) {
        ଫେରନ୍ତୁ (!socket_id) ? ms->ram_size : 0;
    }

    ଫେରନ୍ତୁ (socket_id < ms->numa_state->ଅଙ୍କ_nodes) ?
            ms->numa_state->nodes[socket_id].ଗଣ୍ଠି_mem : 0;
}

ଶୂନ୍ୟ riscv_socket_fdt_write_id(ଅଚଳ MachineState *ms, ଅଚଳ ଅକ୍ଷର *ଗଣ୍ଠି_name,
                               ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_id)
{
    ଯଦି (riscv_numa_enabled(ms)) {
        qemu_fdt_setprop_cell(ms->fdt, ଗଣ୍ଠି_name, "numa-node-id", socket_id);
    }
}

ଶୂନ୍ୟ riscv_socket_fdt_write_distance_matrix(ଅଚଳ MachineState *ms)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, j, idx;
    g_autofree uint32_t *dist_matrix = ଖାଲି;
    uint32_t dist_matrix_size;

    ଯଦି (riscv_numa_enabled(ms) && ms->numa_state->have_numa_distance) {
        dist_matrix_size = riscv_socket_ଗଣନା(ms) * riscv_socket_ଗଣନା(ms);
        dist_matrix_size *= (3 * ଆକାର(uint32_t));
        dist_matrix = g_malloc0(dist_matrix_size);

        ପାଇଁ (i = 0; i < riscv_socket_ଗଣନା(ms); i++) {
            ପାଇଁ (j = 0; j < riscv_socket_ଗଣନା(ms); j++) {
                idx = (i * riscv_socket_ଗଣନା(ms) + j) * 3;
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
    ଅଚଳ CPUArchIdList *possible_cpus = mc->possible_cpu_arch_ids(ms);

    ପ୍ରତିଜ୍ଞା(cpu_index < possible_cpus->len);
    ଫେରନ୍ତୁ possible_cpus->cpus[cpu_index].props;
}

int64_t riscv_numa_get_default_cpu_ଗଣ୍ଠି_id(ଅଚଳ MachineState *ms, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    int64_t nidx = 0;

    ଯଦି (ms->numa_state->ଅଙ୍କ_nodes > ms->smp.cpus) {
        error_report("Number of NUMA nodes (%d)"
                     " cannot exceed the number of available CPUs (%u).",
                     ms->numa_state->ଅଙ୍କ_nodes, ms->smp.cpus);
        exit(EXIT_FAILURE);
    }
    ଯଦି (ms->numa_state->ଅଙ୍କ_nodes) {
        nidx = idx / (ms->smp.cpus / ms->numa_state->ଅଙ୍କ_nodes);
        ଯଦି (ms->numa_state->ଅଙ୍କ_nodes <= nidx) {
            nidx = ms->numa_state->ଅଙ୍କ_nodes - 1;
        }
    }

    ଫେରନ୍ତୁ nidx;
}

ଅଚଳ CPUArchIdList *riscv_numa_possible_cpu_arch_ids(MachineState *ms)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_cpus = ms->smp.max_cpus;

    ଯଦି (ms->possible_cpus) {
        ପ୍ରତିଜ୍ଞା(ms->possible_cpus->len == max_cpus);
        ଫେରନ୍ତୁ ms->possible_cpus;
    }

    ms->possible_cpus = g_malloc0(ଆକାର(CPUArchIdList) +
                                  ଆକାର(CPUArchId) * max_cpus);
    ms->possible_cpus->len = max_cpus;
    ପାଇଁ (n = 0; n < ms->possible_cpus->len; n++) {
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

    ଫେରନ୍ତୁ ms->possible_cpus;
}
