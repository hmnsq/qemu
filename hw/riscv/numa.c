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

bool riscv_numa_enabled(ध्रुव MachineState *ms)
{
    प्रत्यागम (ms->numa_state && ms->numa_state->अंक_nodes) ? true : false;
}

पूर्णाङ्क riscv_socket_मोजणी(ध्रुव MachineState *ms)
{
    प्रत्यागम (riscv_numa_enabled(ms)) ? ms->numa_state->अंक_nodes : 1;
}

पूर्णाङ्क riscv_socket_first_hartid(ध्रुव MachineState *ms, पूर्णाङ्क socket_id)
{
    पूर्णाङ्क i, first_hartid = ms->smp.cpus;

    यदि (!riscv_numa_enabled(ms)) {
        प्रत्यागम (!socket_id) ? 0 : -1;
    }

    प्रति (i = 0; i < ms->smp.cpus; i++) {
        यदि (ms->possible_cpus->cpus[i].props.गाठ_id != socket_id) {
            अनुवर्त;
        }
        यदि (i < first_hartid) {
            first_hartid = i;
        }
    }

    प्रत्यागम (first_hartid < ms->smp.cpus) ? first_hartid : -1;
}

पूर्णाङ्क riscv_socket_last_hartid(ध्रुव MachineState *ms, पूर्णाङ्क socket_id)
{
    पूर्णाङ्क i, last_hartid = -1;

    यदि (!riscv_numa_enabled(ms)) {
        प्रत्यागम (!socket_id) ? ms->smp.cpus - 1 : -1;
    }

    प्रति (i = 0; i < ms->smp.cpus; i++) {
        यदि (ms->possible_cpus->cpus[i].props.गाठ_id != socket_id) {
            अनुवर्त;
        }
        यदि (i > last_hartid) {
            last_hartid = i;
        }
    }

    प्रत्यागम (last_hartid < ms->smp.cpus) ? last_hartid : -1;
}

पूर्णाङ्क riscv_socket_hart_मोजणी(ध्रुव MachineState *ms, पूर्णाङ्क socket_id)
{
    पूर्णाङ्क first_hartid, last_hartid;

    यदि (!riscv_numa_enabled(ms)) {
        प्रत्यागम (!socket_id) ? ms->smp.cpus : -1;
    }

    first_hartid = riscv_socket_first_hartid(ms, socket_id);
    यदि (first_hartid < 0) {
        प्रत्यागम -1;
    }

    last_hartid = riscv_socket_last_hartid(ms, socket_id);
    यदि (last_hartid < 0) {
        प्रत्यागम -1;
    }

    यदि (first_hartid > last_hartid) {
        प्रत्यागम -1;
    }

    प्रत्यागम last_hartid - first_hartid + 1;
}

bool riscv_socket_check_hartids(ध्रुव MachineState *ms, पूर्णाङ्क socket_id)
{
    पूर्णाङ्क i, first_hartid, last_hartid;

    यदि (!riscv_numa_enabled(ms)) {
        प्रत्यागम (!socket_id) ? true : false;
    }

    first_hartid = riscv_socket_first_hartid(ms, socket_id);
    यदि (first_hartid < 0) {
        प्रत्यागम false;
    }

    last_hartid = riscv_socket_last_hartid(ms, socket_id);
    यदि (last_hartid < 0) {
        प्रत्यागम false;
    }

    प्रति (i = first_hartid; i <= last_hartid; i++) {
        यदि (ms->possible_cpus->cpus[i].props.गाठ_id != socket_id) {
            प्रत्यागम false;
        }
    }

    प्रत्यागम true;
}

uint64_t riscv_socket_mem_offset(ध्रुव MachineState *ms, पूर्णाङ्क socket_id)
{
    पूर्णाङ्क i;
    uint64_t mem_offset = 0;

    यदि (!riscv_numa_enabled(ms)) {
        प्रत्यागम 0;
    }

    प्रति (i = 0; i < ms->numa_state->अंक_nodes; i++) {
        यदि (i == socket_id) {
            भङ्ग;
        }
        mem_offset += ms->numa_state->nodes[i].गाठ_mem;
    }

    प्रत्यागम (i == socket_id) ? mem_offset : 0;
}

uint64_t riscv_socket_mem_size(ध्रुव MachineState *ms, पूर्णाङ्क socket_id)
{
    यदि (!riscv_numa_enabled(ms)) {
        प्रत्यागम (!socket_id) ? ms->ram_size : 0;
    }

    प्रत्यागम (socket_id < ms->numa_state->अंक_nodes) ?
            ms->numa_state->nodes[socket_id].गाठ_mem : 0;
}

शून्य riscv_socket_fdt_write_id(ध्रुव MachineState *ms, ध्रुव वर्ण *गाठ_name,
                               पूर्णाङ्क socket_id)
{
    यदि (riscv_numa_enabled(ms)) {
        qemu_fdt_setprop_cell(ms->fdt, गाठ_name, "numa-node-id", socket_id);
    }
}

शून्य riscv_socket_fdt_write_distance_matrix(ध्रुव MachineState *ms)
{
    पूर्णाङ्क i, j, idx;
    g_autofree uint32_t *dist_matrix = रिक्त;
    uint32_t dist_matrix_size;

    यदि (riscv_numa_enabled(ms) && ms->numa_state->have_numa_distance) {
        dist_matrix_size = riscv_socket_मोजणी(ms) * riscv_socket_मोजणी(ms);
        dist_matrix_size *= (3 * परिमाण(uint32_t));
        dist_matrix = g_malloc0(dist_matrix_size);

        प्रति (i = 0; i < riscv_socket_मोजणी(ms); i++) {
            प्रति (j = 0; j < riscv_socket_मोजणी(ms); j++) {
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
riscv_numa_cpu_index_to_props(MachineState *ms, unsigned cpu_index)
{
    MachineClass *mc = MACHINE_GET_CLASS(ms);
    ध्रुव CPUArchIdList *possible_cpus = mc->possible_cpu_arch_ids(ms);

    प्रतिज्ञा(cpu_index < possible_cpus->len);
    प्रत्यागम possible_cpus->cpus[cpu_index].props;
}

int64_t riscv_numa_get_default_cpu_गाठ_id(ध्रुव MachineState *ms, पूर्णाङ्क idx)
{
    int64_t nidx = 0;

    यदि (ms->numa_state->अंक_nodes > ms->smp.cpus) {
        error_report("Number of NUMA nodes (%d)"
                     " cannot exceed the number of available CPUs (%u).",
                     ms->numa_state->अंक_nodes, ms->smp.cpus);
        exit(EXIT_FAILURE);
    }
    यदि (ms->numa_state->अंक_nodes) {
        nidx = idx / (ms->smp.cpus / ms->numa_state->अंक_nodes);
        यदि (ms->numa_state->अंक_nodes <= nidx) {
            nidx = ms->numa_state->अंक_nodes - 1;
        }
    }

    प्रत्यागम nidx;
}

ध्रुव CPUArchIdList *riscv_numa_possible_cpu_arch_ids(MachineState *ms)
{
    पूर्णाङ्क n;
    unsigned पूर्णाङ्क max_cpus = ms->smp.max_cpus;

    यदि (ms->possible_cpus) {
        प्रतिज्ञा(ms->possible_cpus->len == max_cpus);
        प्रत्यागम ms->possible_cpus;
    }

    ms->possible_cpus = g_malloc0(परिमाण(CPUArchIdList) +
                                  परिमाण(CPUArchId) * max_cpus);
    ms->possible_cpus->len = max_cpus;
    प्रति (n = 0; n < ms->possible_cpus->len; n++) {
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

    प्रत्यागम ms->possible_cpus;
}
