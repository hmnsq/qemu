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

#अन्तर्भाव "qemu/osdep.h"
#अन्तर्भाव "qemu/units.h"
#अन्तर्भाव "qemu/error-report.h"
#अन्तर्भाव "qapi/error.h"
#अन्तर्भाव "hw/core/boards.h"
#अन्तर्भाव "hw/core/qdev-properties.h"
#अन्तर्भाव "hw/riscv/numa.h"
#अन्तर्भाव "system/device_tree.h"

bool riscv_numa_enabled(स्थिरांक MachineState *ms)
{
    परत (ms->numa_state && ms->numa_state->अंक_nodes) ? true : false;
}

पूर्णांक riscv_socket_मोजणी(स्थिरांक MachineState *ms)
{
    परत (riscv_numa_enabled(ms)) ? ms->numa_state->अंक_nodes : 1;
}

पूर्णांक riscv_socket_first_hartid(स्थिरांक MachineState *ms, पूर्णांक socket_id)
{
    पूर्णांक i, first_hartid = ms->smp.cpus;

    जर (!riscv_numa_enabled(ms)) {
        परत (!socket_id) ? 0 : -1;
    }

    साठी (i = 0; i < ms->smp.cpus; i++) {
        जर (ms->possible_cpus->cpus[i].props.गाठ_id != socket_id) {
            सुरूठेवा;
        }
        जर (i < first_hartid) {
            first_hartid = i;
        }
    }

    परत (first_hartid < ms->smp.cpus) ? first_hartid : -1;
}

पूर्णांक riscv_socket_last_hartid(स्थिरांक MachineState *ms, पूर्णांक socket_id)
{
    पूर्णांक i, last_hartid = -1;

    जर (!riscv_numa_enabled(ms)) {
        परत (!socket_id) ? ms->smp.cpus - 1 : -1;
    }

    साठी (i = 0; i < ms->smp.cpus; i++) {
        जर (ms->possible_cpus->cpus[i].props.गाठ_id != socket_id) {
            सुरूठेवा;
        }
        जर (i > last_hartid) {
            last_hartid = i;
        }
    }

    परत (last_hartid < ms->smp.cpus) ? last_hartid : -1;
}

पूर्णांक riscv_socket_hart_मोजणी(स्थिरांक MachineState *ms, पूर्णांक socket_id)
{
    पूर्णांक first_hartid, last_hartid;

    जर (!riscv_numa_enabled(ms)) {
        परत (!socket_id) ? ms->smp.cpus : -1;
    }

    first_hartid = riscv_socket_first_hartid(ms, socket_id);
    जर (first_hartid < 0) {
        परत -1;
    }

    last_hartid = riscv_socket_last_hartid(ms, socket_id);
    जर (last_hartid < 0) {
        परत -1;
    }

    जर (first_hartid > last_hartid) {
        परत -1;
    }

    परत last_hartid - first_hartid + 1;
}

bool riscv_socket_check_hartids(स्थिरांक MachineState *ms, पूर्णांक socket_id)
{
    पूर्णांक i, first_hartid, last_hartid;

    जर (!riscv_numa_enabled(ms)) {
        परत (!socket_id) ? true : false;
    }

    first_hartid = riscv_socket_first_hartid(ms, socket_id);
    जर (first_hartid < 0) {
        परत false;
    }

    last_hartid = riscv_socket_last_hartid(ms, socket_id);
    जर (last_hartid < 0) {
        परत false;
    }

    साठी (i = first_hartid; i <= last_hartid; i++) {
        जर (ms->possible_cpus->cpus[i].props.गाठ_id != socket_id) {
            परत false;
        }
    }

    परत true;
}

uint64_t riscv_socket_mem_offset(स्थिरांक MachineState *ms, पूर्णांक socket_id)
{
    पूर्णांक i;
    uint64_t mem_offset = 0;

    जर (!riscv_numa_enabled(ms)) {
        परत 0;
    }

    साठी (i = 0; i < ms->numa_state->अंक_nodes; i++) {
        जर (i == socket_id) {
            थांबा;
        }
        mem_offset += ms->numa_state->nodes[i].गाठ_mem;
    }

    परत (i == socket_id) ? mem_offset : 0;
}

uint64_t riscv_socket_mem_size(स्थिरांक MachineState *ms, पूर्णांक socket_id)
{
    जर (!riscv_numa_enabled(ms)) {
        परत (!socket_id) ? ms->ram_size : 0;
    }

    परत (socket_id < ms->numa_state->अंक_nodes) ?
            ms->numa_state->nodes[socket_id].गाठ_mem : 0;
}

शून्य riscv_socket_fdt_write_id(स्थिरांक MachineState *ms, स्थिरांक अक्षर *गाठ_name,
                               पूर्णांक socket_id)
{
    जर (riscv_numa_enabled(ms)) {
        qemu_fdt_setprop_cell(ms->fdt, गाठ_name, "numa-node-id", socket_id);
    }
}

शून्य riscv_socket_fdt_write_distance_matrix(स्थिरांक MachineState *ms)
{
    पूर्णांक i, j, idx;
    g_autofree uint32_t *dist_matrix = रिक्त;
    uint32_t dist_matrix_size;

    जर (riscv_numa_enabled(ms) && ms->numa_state->have_numa_distance) {
        dist_matrix_size = riscv_socket_मोजणी(ms) * riscv_socket_मोजणी(ms);
        dist_matrix_size *= (3 * आकार(uint32_t));
        dist_matrix = g_malloc0(dist_matrix_size);

        साठी (i = 0; i < riscv_socket_मोजणी(ms); i++) {
            साठी (j = 0; j < riscv_socket_मोजणी(ms); j++) {
                idx = (i * riscv_socket_मोजणी(ms) + j) * 3;
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
riscv_numa_cpu_index_to_props(MachineState *ms, अचिन्हित cpu_index)
{
    MachineClass *mc = MACHINE_GET_CLASS(ms);
    स्थिरांक CPUArchIdList *possible_cpus = mc->possible_cpu_arch_ids(ms);

    प्रतिज्ञा(cpu_index < possible_cpus->len);
    परत possible_cpus->cpus[cpu_index].props;
}

int64_t riscv_numa_get_default_cpu_गाठ_id(स्थिरांक MachineState *ms, पूर्णांक idx)
{
    int64_t nidx = 0;

    जर (ms->numa_state->अंक_nodes > ms->smp.cpus) {
        error_report("Number of NUMA nodes (%d)"
                     " cannot exceed the number of available CPUs (%u).",
                     ms->numa_state->अंक_nodes, ms->smp.cpus);
        निकास(निकास_त्रुटि);
    }
    जर (ms->numa_state->अंक_nodes) {
        nidx = idx / (ms->smp.cpus / ms->numa_state->अंक_nodes);
        जर (ms->numa_state->अंक_nodes <= nidx) {
            nidx = ms->numa_state->अंक_nodes - 1;
        }
    }

    परत nidx;
}

स्थिरांक CPUArchIdList *riscv_numa_possible_cpu_arch_ids(MachineState *ms)
{
    पूर्णांक n;
    अचिन्हित पूर्णांक max_cpus = ms->smp.max_cpus;

    जर (ms->possible_cpus) {
        प्रतिज्ञा(ms->possible_cpus->len == max_cpus);
        परत ms->possible_cpus;
    }

    ms->possible_cpus = g_malloc0(आकार(CPUArchIdList) +
                                  आकार(CPUArchId) * max_cpus);
    ms->possible_cpus->len = max_cpus;
    साठी (n = 0; n < ms->possible_cpus->len; n++) {
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

    परत ms->possible_cpus;
}
