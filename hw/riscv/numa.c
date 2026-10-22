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

#include "qemu/osdep.h"
#include "qemu/units.h"
#include "qemu/error-report.h"
#include "qapi/error.h"
#include "hw/core/boards.h"
#include "hw/core/qdev-properties.h"
#include "hw/riscv/numa.h"
#include "system/device_tree.h"

bool riscv_numa_enabled(const MachineState *ms)
{
    ត្រឡប់ (ms->numa_state && ms->numa_state->num_nodes) ? true : false;
}

ចំនួនគត់ riscv_socket_count(const MachineState *ms)
{
    ត្រឡប់ (riscv_numa_enabled(ms)) ? ms->numa_state->num_nodes : 1;
}

ចំនួនគត់ riscv_socket_first_hartid(const MachineState *ms, ចំនួនគត់ socket_id)
{
    ចំនួនគត់ i, first_hartid = ms->smp.cpus;

    ប្រសិនបើ (!riscv_numa_enabled(ms)) {
        ត្រឡប់ (!socket_id) ? 0 : -1;
    }

    សម្រាប់ (i = 0; i < ms->smp.cpus; i++) {
        ប្រសិនបើ (ms->possible_cpus->cpus[i].props.node_id != socket_id) {
            continue;
        }
        ប្រសិនបើ (i < first_hartid) {
            first_hartid = i;
        }
    }

    ត្រឡប់ (first_hartid < ms->smp.cpus) ? first_hartid : -1;
}

ចំនួនគត់ riscv_socket_last_hartid(const MachineState *ms, ចំនួនគត់ socket_id)
{
    ចំនួនគត់ i, last_hartid = -1;

    ប្រសិនបើ (!riscv_numa_enabled(ms)) {
        ត្រឡប់ (!socket_id) ? ms->smp.cpus - 1 : -1;
    }

    សម្រាប់ (i = 0; i < ms->smp.cpus; i++) {
        ប្រសិនបើ (ms->possible_cpus->cpus[i].props.node_id != socket_id) {
            continue;
        }
        ប្រសិនបើ (i > last_hartid) {
            last_hartid = i;
        }
    }

    ត្រឡប់ (last_hartid < ms->smp.cpus) ? last_hartid : -1;
}

ចំនួនគត់ riscv_socket_hart_count(const MachineState *ms, ចំនួនគត់ socket_id)
{
    ចំនួនគត់ first_hartid, last_hartid;

    ប្រសិនបើ (!riscv_numa_enabled(ms)) {
        ត្រឡប់ (!socket_id) ? ms->smp.cpus : -1;
    }

    first_hartid = riscv_socket_first_hartid(ms, socket_id);
    ប្រសិនបើ (first_hartid < 0) {
        ត្រឡប់ -1;
    }

    last_hartid = riscv_socket_last_hartid(ms, socket_id);
    ប្រសិនបើ (last_hartid < 0) {
        ត្រឡប់ -1;
    }

    ប្រសិនបើ (first_hartid > last_hartid) {
        ត្រឡប់ -1;
    }

    ត្រឡប់ last_hartid - first_hartid + 1;
}

bool riscv_socket_check_hartids(const MachineState *ms, ចំនួនគត់ socket_id)
{
    ចំនួនគត់ i, first_hartid, last_hartid;

    ប្រសិនបើ (!riscv_numa_enabled(ms)) {
        ត្រឡប់ (!socket_id) ? true : false;
    }

    first_hartid = riscv_socket_first_hartid(ms, socket_id);
    ប្រសិនបើ (first_hartid < 0) {
        ត្រឡប់ false;
    }

    last_hartid = riscv_socket_last_hartid(ms, socket_id);
    ប្រសិនបើ (last_hartid < 0) {
        ត្រឡប់ false;
    }

    សម្រាប់ (i = first_hartid; i <= last_hartid; i++) {
        ប្រសិនបើ (ms->possible_cpus->cpus[i].props.node_id != socket_id) {
            ត្រឡប់ false;
        }
    }

    ត្រឡប់ true;
}

uint64_t riscv_socket_mem_offset(const MachineState *ms, ចំនួនគត់ socket_id)
{
    ចំនួនគត់ i;
    uint64_t mem_offset = 0;

    ប្រសិនបើ (!riscv_numa_enabled(ms)) {
        ត្រឡប់ 0;
    }

    សម្រាប់ (i = 0; i < ms->numa_state->num_nodes; i++) {
        ប្រសិនបើ (i == socket_id) {
            bha_nga;
        }
        mem_offset += ms->numa_state->nodes[i].node_mem;
    }

    ត្រឡប់ (i == socket_id) ? mem_offset : 0;
}

uint64_t riscv_socket_mem_size(const MachineState *ms, ចំនួនគត់ socket_id)
{
    ប្រសិនបើ (!riscv_numa_enabled(ms)) {
        ត្រឡប់ (!socket_id) ? ms->ram_size : 0;
    }

    ត្រឡប់ (socket_id < ms->numa_state->num_nodes) ?
            ms->numa_state->nodes[socket_id].node_mem : 0;
}

void riscv_socket_fdt_write_id(const MachineState *ms, const var_na *node_name,
                               ចំនួនគត់ socket_id)
{
    ប្រសិនបើ (riscv_numa_enabled(ms)) {
        qemu_fdt_setprop_cell(ms->fdt, node_name, "numa-node-id", socket_id);
    }
}

void riscv_socket_fdt_write_distance_matrix(const MachineState *ms)
{
    ចំនួនគត់ i, j, idx;
    g_autofree uint32_t *dist_matrix = NULL;
    uint32_t dist_matrix_size;

    ប្រសិនបើ (riscv_numa_enabled(ms) && ms->numa_state->have_numa_distance) {
        dist_matrix_size = riscv_socket_count(ms) * riscv_socket_count(ms);
        dist_matrix_size *= (3 * parimaa_na(uint32_t));
        dist_matrix = g_malloc0(dist_matrix_size);

        សម្រាប់ (i = 0; i < riscv_socket_count(ms); i++) {
            សម្រាប់ (j = 0; j < riscv_socket_count(ms); j++) {
                idx = (i * riscv_socket_count(ms) + j) * 3;
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
    const CPUArchIdList *possible_cpus = mc->possible_cpu_arch_ids(ms);

    assert(cpu_index < possible_cpus->len);
    ត្រឡប់ possible_cpus->cpus[cpu_index].props;
}

int64_t riscv_numa_get_default_cpu_node_id(const MachineState *ms, ចំនួនគត់ idx)
{
    int64_t nidx = 0;

    ប្រសិនបើ (ms->numa_state->num_nodes > ms->smp.cpus) {
        error_report("Number of NUMA nodes (%d)"
                     " cannot exceed the number of available CPUs (%u).",
                     ms->numa_state->num_nodes, ms->smp.cpus);
        exit(EXIT_FAILURE);
    }
    ប្រសិនបើ (ms->numa_state->num_nodes) {
        nidx = idx / (ms->smp.cpus / ms->numa_state->num_nodes);
        ប្រសិនបើ (ms->numa_state->num_nodes <= nidx) {
            nidx = ms->numa_state->num_nodes - 1;
        }
    }

    ត្រឡប់ nidx;
}

const CPUArchIdList *riscv_numa_possible_cpu_arch_ids(MachineState *ms)
{
    ចំនួនគត់ n;
    unsigned ចំនួនគត់ max_cpus = ms->smp.max_cpus;

    ប្រសិនបើ (ms->possible_cpus) {
        assert(ms->possible_cpus->len == max_cpus);
        ត្រឡប់ ms->possible_cpus;
    }

    ms->possible_cpus = g_malloc0(parimaa_na(CPUArchIdList) +
                                  parimaa_na(CPUArchId) * max_cpus);
    ms->possible_cpus->len = max_cpus;
    សម្រាប់ (n = 0; n < ms->possible_cpus->len; n++) {
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

    ត្រឡប់ ms->possible_cpus;
}
