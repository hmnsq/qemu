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

#અન્તર્ભાવ "qemu/osdep.h"
#અન્તર્ભાવ "qemu/units.h"
#અન્તર્ભાવ "qemu/error-report.h"
#અન્તર્ભાવ "qapi/error.h"
#અન્તર્ભાવ "hw/core/boards.h"
#અન્તર્ભાવ "hw/core/qdev-properties.h"
#અન્તર્ભાવ "hw/riscv/numa.h"
#અન્તર્ભાવ "system/device_tree.h"

bool riscv_numa_enabled(અચલ MachineState *ms)
{
    પાછા (ms->numa_state && ms->numa_state->અંક_nodes) ? true : false;
}

પૂર્ણાંક riscv_socket_ગણતરી(અચલ MachineState *ms)
{
    પાછા (riscv_numa_enabled(ms)) ? ms->numa_state->અંક_nodes : 1;
}

પૂર્ણાંક riscv_socket_first_hartid(અચલ MachineState *ms, પૂર્ણાંક socket_id)
{
    પૂર્ણાંક i, first_hartid = ms->smp.cpus;

    જો (!riscv_numa_enabled(ms)) {
        પાછા (!socket_id) ? 0 : -1;
    }

    માટે (i = 0; i < ms->smp.cpus; i++) {
        જો (ms->possible_cpus->cpus[i].props.ગાંઠ_id != socket_id) {
            ચાલુ;
        }
        જો (i < first_hartid) {
            first_hartid = i;
        }
    }

    પાછા (first_hartid < ms->smp.cpus) ? first_hartid : -1;
}

પૂર્ણાંક riscv_socket_last_hartid(અચલ MachineState *ms, પૂર્ણાંક socket_id)
{
    પૂર્ણાંક i, last_hartid = -1;

    જો (!riscv_numa_enabled(ms)) {
        પાછા (!socket_id) ? ms->smp.cpus - 1 : -1;
    }

    માટે (i = 0; i < ms->smp.cpus; i++) {
        જો (ms->possible_cpus->cpus[i].props.ગાંઠ_id != socket_id) {
            ચાલુ;
        }
        જો (i > last_hartid) {
            last_hartid = i;
        }
    }

    પાછા (last_hartid < ms->smp.cpus) ? last_hartid : -1;
}

પૂર્ણાંક riscv_socket_hart_ગણતરી(અચલ MachineState *ms, પૂર્ણાંક socket_id)
{
    પૂર્ણાંક first_hartid, last_hartid;

    જો (!riscv_numa_enabled(ms)) {
        પાછા (!socket_id) ? ms->smp.cpus : -1;
    }

    first_hartid = riscv_socket_first_hartid(ms, socket_id);
    જો (first_hartid < 0) {
        પાછા -1;
    }

    last_hartid = riscv_socket_last_hartid(ms, socket_id);
    જો (last_hartid < 0) {
        પાછા -1;
    }

    જો (first_hartid > last_hartid) {
        પાછા -1;
    }

    પાછા last_hartid - first_hartid + 1;
}

bool riscv_socket_check_hartids(અચલ MachineState *ms, પૂર્ણાંક socket_id)
{
    પૂર્ણાંક i, first_hartid, last_hartid;

    જો (!riscv_numa_enabled(ms)) {
        પાછા (!socket_id) ? true : false;
    }

    first_hartid = riscv_socket_first_hartid(ms, socket_id);
    જો (first_hartid < 0) {
        પાછા false;
    }

    last_hartid = riscv_socket_last_hartid(ms, socket_id);
    જો (last_hartid < 0) {
        પાછા false;
    }

    માટે (i = first_hartid; i <= last_hartid; i++) {
        જો (ms->possible_cpus->cpus[i].props.ગાંઠ_id != socket_id) {
            પાછા false;
        }
    }

    પાછા true;
}

uint64_t riscv_socket_mem_offset(અચલ MachineState *ms, પૂર્ણાંક socket_id)
{
    પૂર્ણાંક i;
    uint64_t mem_offset = 0;

    જો (!riscv_numa_enabled(ms)) {
        પાછા 0;
    }

    માટે (i = 0; i < ms->numa_state->અંક_nodes; i++) {
        જો (i == socket_id) {
            તોડો;
        }
        mem_offset += ms->numa_state->nodes[i].ગાંઠ_mem;
    }

    પાછા (i == socket_id) ? mem_offset : 0;
}

uint64_t riscv_socket_mem_size(અચલ MachineState *ms, પૂર્ણાંક socket_id)
{
    જો (!riscv_numa_enabled(ms)) {
        પાછા (!socket_id) ? ms->ram_size : 0;
    }

    પાછા (socket_id < ms->numa_state->અંક_nodes) ?
            ms->numa_state->nodes[socket_id].ગાંઠ_mem : 0;
}

શૂન્ય riscv_socket_fdt_write_id(અચલ MachineState *ms, અચલ અક્ષર *ગાંઠ_name,
                               પૂર્ણાંક socket_id)
{
    જો (riscv_numa_enabled(ms)) {
        qemu_fdt_setprop_cell(ms->fdt, ગાંઠ_name, "numa-node-id", socket_id);
    }
}

શૂન્ય riscv_socket_fdt_write_distance_matrix(અચલ MachineState *ms)
{
    પૂર્ણાંક i, j, idx;
    g_autofree uint32_t *dist_matrix = ખાલી;
    uint32_t dist_matrix_size;

    જો (riscv_numa_enabled(ms) && ms->numa_state->have_numa_distance) {
        dist_matrix_size = riscv_socket_ગણતરી(ms) * riscv_socket_ગણતરી(ms);
        dist_matrix_size *= (3 * કદ(uint32_t));
        dist_matrix = g_malloc0(dist_matrix_size);

        માટે (i = 0; i < riscv_socket_ગણતરી(ms); i++) {
            માટે (j = 0; j < riscv_socket_ગણતરી(ms); j++) {
                idx = (i * riscv_socket_ગણતરી(ms) + j) * 3;
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
    અચલ CPUArchIdList *possible_cpus = mc->possible_cpu_arch_ids(ms);

    પ્રતિજ્ઞા(cpu_index < possible_cpus->len);
    પાછા possible_cpus->cpus[cpu_index].props;
}

int64_t riscv_numa_get_default_cpu_ગાંઠ_id(અચલ MachineState *ms, પૂર્ણાંક idx)
{
    int64_t nidx = 0;

    જો (ms->numa_state->અંક_nodes > ms->smp.cpus) {
        error_report("Number of NUMA nodes (%d)"
                     " cannot exceed the number of available CPUs (%u).",
                     ms->numa_state->અંક_nodes, ms->smp.cpus);
        exit(EXIT_FAILURE);
    }
    જો (ms->numa_state->અંક_nodes) {
        nidx = idx / (ms->smp.cpus / ms->numa_state->અંક_nodes);
        જો (ms->numa_state->અંક_nodes <= nidx) {
            nidx = ms->numa_state->અંક_nodes - 1;
        }
    }

    પાછા nidx;
}

અચલ CPUArchIdList *riscv_numa_possible_cpu_arch_ids(MachineState *ms)
{
    પૂર્ણાંક n;
    unsigned પૂર્ણાંક max_cpus = ms->smp.max_cpus;

    જો (ms->possible_cpus) {
        પ્રતિજ્ઞા(ms->possible_cpus->len == max_cpus);
        પાછા ms->possible_cpus;
    }

    ms->possible_cpus = g_malloc0(કદ(CPUArchIdList) +
                                  કદ(CPUArchId) * max_cpus);
    ms->possible_cpus->len = max_cpus;
    માટે (n = 0; n < ms->possible_cpus->len; n++) {
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

    પાછા ms->possible_cpus;
}
