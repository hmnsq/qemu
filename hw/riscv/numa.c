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

#समावेश "qemu/osdep.h"
#समावेश "qemu/units.h"
#समावेश "qemu/error-report.h"
#समावेश "qapi/error.h"
#समावेश "hw/core/boards.h"
#समावेश "hw/core/qdev-properties.h"
#समावेश "hw/riscv/numa.h"
#समावेश "system/device_tree.h"

bool riscv_numa_enabled(स्थिर MachineState *ms)
{
    वापस (ms->numa_state && ms->numa_state->अंक_nodes) ? true : false;
}

पूर्णांक riscv_socket_गिनती(स्थिर MachineState *ms)
{
    वापस (riscv_numa_enabled(ms)) ? ms->numa_state->अंक_nodes : 1;
}

पूर्णांक riscv_socket_first_hartid(स्थिर MachineState *ms, पूर्णांक socket_id)
{
    पूर्णांक i, first_hartid = ms->smp.cpus;

    अगर (!riscv_numa_enabled(ms)) {
        वापस (!socket_id) ? 0 : -1;
    }

    क्रम (i = 0; i < ms->smp.cpus; i++) {
        अगर (ms->possible_cpus->cpus[i].props.गाँठ_id != socket_id) {
            जारी;
        }
        अगर (i < first_hartid) {
            first_hartid = i;
        }
    }

    वापस (first_hartid < ms->smp.cpus) ? first_hartid : -1;
}

पूर्णांक riscv_socket_last_hartid(स्थिर MachineState *ms, पूर्णांक socket_id)
{
    पूर्णांक i, last_hartid = -1;

    अगर (!riscv_numa_enabled(ms)) {
        वापस (!socket_id) ? ms->smp.cpus - 1 : -1;
    }

    क्रम (i = 0; i < ms->smp.cpus; i++) {
        अगर (ms->possible_cpus->cpus[i].props.गाँठ_id != socket_id) {
            जारी;
        }
        अगर (i > last_hartid) {
            last_hartid = i;
        }
    }

    वापस (last_hartid < ms->smp.cpus) ? last_hartid : -1;
}

पूर्णांक riscv_socket_hart_गिनती(स्थिर MachineState *ms, पूर्णांक socket_id)
{
    पूर्णांक first_hartid, last_hartid;

    अगर (!riscv_numa_enabled(ms)) {
        वापस (!socket_id) ? ms->smp.cpus : -1;
    }

    first_hartid = riscv_socket_first_hartid(ms, socket_id);
    अगर (first_hartid < 0) {
        वापस -1;
    }

    last_hartid = riscv_socket_last_hartid(ms, socket_id);
    अगर (last_hartid < 0) {
        वापस -1;
    }

    अगर (first_hartid > last_hartid) {
        वापस -1;
    }

    वापस last_hartid - first_hartid + 1;
}

bool riscv_socket_check_hartids(स्थिर MachineState *ms, पूर्णांक socket_id)
{
    पूर्णांक i, first_hartid, last_hartid;

    अगर (!riscv_numa_enabled(ms)) {
        वापस (!socket_id) ? true : false;
    }

    first_hartid = riscv_socket_first_hartid(ms, socket_id);
    अगर (first_hartid < 0) {
        वापस false;
    }

    last_hartid = riscv_socket_last_hartid(ms, socket_id);
    अगर (last_hartid < 0) {
        वापस false;
    }

    क्रम (i = first_hartid; i <= last_hartid; i++) {
        अगर (ms->possible_cpus->cpus[i].props.गाँठ_id != socket_id) {
            वापस false;
        }
    }

    वापस true;
}

uint64_t riscv_socket_mem_offset(स्थिर MachineState *ms, पूर्णांक socket_id)
{
    पूर्णांक i;
    uint64_t mem_offset = 0;

    अगर (!riscv_numa_enabled(ms)) {
        वापस 0;
    }

    क्रम (i = 0; i < ms->numa_state->अंक_nodes; i++) {
        अगर (i == socket_id) {
            अवरोध;
        }
        mem_offset += ms->numa_state->nodes[i].गाँठ_mem;
    }

    वापस (i == socket_id) ? mem_offset : 0;
}

uint64_t riscv_socket_mem_आकार(स्थिर MachineState *ms, पूर्णांक socket_id)
{
    अगर (!riscv_numa_enabled(ms)) {
        वापस (!socket_id) ? ms->ram_आकार : 0;
    }

    वापस (socket_id < ms->numa_state->अंक_nodes) ?
            ms->numa_state->nodes[socket_id].गाँठ_mem : 0;
}

व्योम riscv_socket_fdt_write_id(स्थिर MachineState *ms, स्थिर अक्षर *गाँठ_name,
                               पूर्णांक socket_id)
{
    अगर (riscv_numa_enabled(ms)) {
        qemu_fdt_setprop_cell(ms->fdt, गाँठ_name, "numa-node-id", socket_id);
    }
}

व्योम riscv_socket_fdt_write_distance_matrix(स्थिर MachineState *ms)
{
    पूर्णांक i, j, idx;
    g_autofree uint32_t *dist_matrix = शून्य;
    uint32_t dist_matrix_आकार;

    अगर (riscv_numa_enabled(ms) && ms->numa_state->have_numa_distance) {
        dist_matrix_आकार = riscv_socket_गिनती(ms) * riscv_socket_गिनती(ms);
        dist_matrix_आकार *= (3 * माप(uint32_t));
        dist_matrix = g_malloc0(dist_matrix_आकार);

        क्रम (i = 0; i < riscv_socket_गिनती(ms); i++) {
            क्रम (j = 0; j < riscv_socket_गिनती(ms); j++) {
                idx = (i * riscv_socket_गिनती(ms) + j) * 3;
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
                         dist_matrix, dist_matrix_आकार);
    }
}

CpuInstanceProperties
riscv_numa_cpu_सूचकांक_to_props(MachineState *ms, अचिन्हित cpu_सूचकांक)
{
    MachineClass *mc = MACHINE_GET_CLASS(ms);
    स्थिर CPUArchIdList *possible_cpus = mc->possible_cpu_arch_ids(ms);

    निश्चित(cpu_सूचकांक < possible_cpus->len);
    वापस possible_cpus->cpus[cpu_सूचकांक].props;
}

int64_t riscv_numa_get_default_cpu_गाँठ_id(स्थिर MachineState *ms, पूर्णांक idx)
{
    int64_t nidx = 0;

    अगर (ms->numa_state->अंक_nodes > ms->smp.cpus) {
        error_report("Number of NUMA nodes (%d)"
                     " cannot exceed the number of available CPUs (%u).",
                     ms->numa_state->अंक_nodes, ms->smp.cpus);
        निकास(निकास_त्रुटि);
    }
    अगर (ms->numa_state->अंक_nodes) {
        nidx = idx / (ms->smp.cpus / ms->numa_state->अंक_nodes);
        अगर (ms->numa_state->अंक_nodes <= nidx) {
            nidx = ms->numa_state->अंक_nodes - 1;
        }
    }

    वापस nidx;
}

स्थिर CPUArchIdList *riscv_numa_possible_cpu_arch_ids(MachineState *ms)
{
    पूर्णांक n;
    अचिन्हित पूर्णांक max_cpus = ms->smp.max_cpus;

    अगर (ms->possible_cpus) {
        निश्चित(ms->possible_cpus->len == max_cpus);
        वापस ms->possible_cpus;
    }

    ms->possible_cpus = g_malloc0(माप(CPUArchIdList) +
                                  माप(CPUArchId) * max_cpus);
    ms->possible_cpus->len = max_cpus;
    क्रम (n = 0; n < ms->possible_cpus->len; n++) {
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

    वापस ms->possible_cpus;
}
