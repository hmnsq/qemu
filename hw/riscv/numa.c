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

#അന്തര്ഭാവ "qemu/osdep.h"
#അന്തര്ഭാവ "qemu/units.h"
#അന്തര്ഭാവ "qemu/error-report.h"
#അന്തര്ഭാവ "qapi/error.h"
#അന്തര്ഭാവ "hw/core/boards.h"
#അന്തര്ഭാവ "hw/core/qdev-properties.h"
#അന്തര്ഭാവ "hw/riscv/numa.h"
#അന്തര്ഭാവ "system/device_tree.h"

bool riscv_numa_enabled(സ്ഥിരം MachineState *ms)
{
    തിരികെ (ms->numa_state && ms->numa_state->അക്കം_nodes) ? true : false;
}

പൂർണ്ണസംഖ്യ riscv_socket_എണ്ണം(സ്ഥിരം MachineState *ms)
{
    തിരികെ (riscv_numa_enabled(ms)) ? ms->numa_state->അക്കം_nodes : 1;
}

പൂർണ്ണസംഖ്യ riscv_socket_first_hartid(സ്ഥിരം MachineState *ms, പൂർണ്ണസംഖ്യ socket_id)
{
    പൂർണ്ണസംഖ്യ i, first_hartid = ms->smp.cpus;

    എങ്കിൽ (!riscv_numa_enabled(ms)) {
        തിരികെ (!socket_id) ? 0 : -1;
    }

    വേണ്ടി (i = 0; i < ms->smp.cpus; i++) {
        എങ്കിൽ (ms->possible_cpus->cpus[i].props.കണ്ണി_id != socket_id) {
            തുടരുക;
        }
        എങ്കിൽ (i < first_hartid) {
            first_hartid = i;
        }
    }

    തിരികെ (first_hartid < ms->smp.cpus) ? first_hartid : -1;
}

പൂർണ്ണസംഖ്യ riscv_socket_last_hartid(സ്ഥിരം MachineState *ms, പൂർണ്ണസംഖ്യ socket_id)
{
    പൂർണ്ണസംഖ്യ i, last_hartid = -1;

    എങ്കിൽ (!riscv_numa_enabled(ms)) {
        തിരികെ (!socket_id) ? ms->smp.cpus - 1 : -1;
    }

    വേണ്ടി (i = 0; i < ms->smp.cpus; i++) {
        എങ്കിൽ (ms->possible_cpus->cpus[i].props.കണ്ണി_id != socket_id) {
            തുടരുക;
        }
        എങ്കിൽ (i > last_hartid) {
            last_hartid = i;
        }
    }

    തിരികെ (last_hartid < ms->smp.cpus) ? last_hartid : -1;
}

പൂർണ്ണസംഖ്യ riscv_socket_hart_എണ്ണം(സ്ഥിരം MachineState *ms, പൂർണ്ണസംഖ്യ socket_id)
{
    പൂർണ്ണസംഖ്യ first_hartid, last_hartid;

    എങ്കിൽ (!riscv_numa_enabled(ms)) {
        തിരികെ (!socket_id) ? ms->smp.cpus : -1;
    }

    first_hartid = riscv_socket_first_hartid(ms, socket_id);
    എങ്കിൽ (first_hartid < 0) {
        തിരികെ -1;
    }

    last_hartid = riscv_socket_last_hartid(ms, socket_id);
    എങ്കിൽ (last_hartid < 0) {
        തിരികെ -1;
    }

    എങ്കിൽ (first_hartid > last_hartid) {
        തിരികെ -1;
    }

    തിരികെ last_hartid - first_hartid + 1;
}

bool riscv_socket_check_hartids(സ്ഥിരം MachineState *ms, പൂർണ്ണസംഖ്യ socket_id)
{
    പൂർണ്ണസംഖ്യ i, first_hartid, last_hartid;

    എങ്കിൽ (!riscv_numa_enabled(ms)) {
        തിരികെ (!socket_id) ? true : false;
    }

    first_hartid = riscv_socket_first_hartid(ms, socket_id);
    എങ്കിൽ (first_hartid < 0) {
        തിരികെ false;
    }

    last_hartid = riscv_socket_last_hartid(ms, socket_id);
    എങ്കിൽ (last_hartid < 0) {
        തിരികെ false;
    }

    വേണ്ടി (i = first_hartid; i <= last_hartid; i++) {
        എങ്കിൽ (ms->possible_cpus->cpus[i].props.കണ്ണി_id != socket_id) {
            തിരികെ false;
        }
    }

    തിരികെ true;
}

uint64_t riscv_socket_mem_offset(സ്ഥിരം MachineState *ms, പൂർണ്ണസംഖ്യ socket_id)
{
    പൂർണ്ണസംഖ്യ i;
    uint64_t mem_offset = 0;

    എങ്കിൽ (!riscv_numa_enabled(ms)) {
        തിരികെ 0;
    }

    വേണ്ടി (i = 0; i < ms->numa_state->അക്കം_nodes; i++) {
        എങ്കിൽ (i == socket_id) {
            നിർത്തുക;
        }
        mem_offset += ms->numa_state->nodes[i].കണ്ണി_mem;
    }

    തിരികെ (i == socket_id) ? mem_offset : 0;
}

uint64_t riscv_socket_mem_size(സ്ഥിരം MachineState *ms, പൂർണ്ണസംഖ്യ socket_id)
{
    എങ്കിൽ (!riscv_numa_enabled(ms)) {
        തിരികെ (!socket_id) ? ms->ram_size : 0;
    }

    തിരികെ (socket_id < ms->numa_state->അക്കം_nodes) ?
            ms->numa_state->nodes[socket_id].കണ്ണി_mem : 0;
}

ശൂന്യം riscv_socket_fdt_write_id(സ്ഥിരം MachineState *ms, സ്ഥിരം അക്ഷരം *കണ്ണി_name,
                               പൂർണ്ണസംഖ്യ socket_id)
{
    എങ്കിൽ (riscv_numa_enabled(ms)) {
        qemu_fdt_setprop_cell(ms->fdt, കണ്ണി_name, "numa-node-id", socket_id);
    }
}

ശൂന്യം riscv_socket_fdt_write_distance_matrix(സ്ഥിരം MachineState *ms)
{
    പൂർണ്ണസംഖ്യ i, j, idx;
    g_autofree uint32_t *dist_matrix = ശൂന്യ;
    uint32_t dist_matrix_size;

    എങ്കിൽ (riscv_numa_enabled(ms) && ms->numa_state->have_numa_distance) {
        dist_matrix_size = riscv_socket_എണ്ണം(ms) * riscv_socket_എണ്ണം(ms);
        dist_matrix_size *= (3 * വലുപ്പം(uint32_t));
        dist_matrix = g_malloc0(dist_matrix_size);

        വേണ്ടി (i = 0; i < riscv_socket_എണ്ണം(ms); i++) {
            വേണ്ടി (j = 0; j < riscv_socket_എണ്ണം(ms); j++) {
                idx = (i * riscv_socket_എണ്ണം(ms) + j) * 3;
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
    സ്ഥിരം CPUArchIdList *possible_cpus = mc->possible_cpu_arch_ids(ms);

    പ്രതിജ്ഞാ(cpu_index < possible_cpus->len);
    തിരികെ possible_cpus->cpus[cpu_index].props;
}

int64_t riscv_numa_get_default_cpu_കണ്ണി_id(സ്ഥിരം MachineState *ms, പൂർണ്ണസംഖ്യ idx)
{
    int64_t nidx = 0;

    എങ്കിൽ (ms->numa_state->അക്കം_nodes > ms->smp.cpus) {
        error_report("Number of NUMA nodes (%d)"
                     " cannot exceed the number of available CPUs (%u).",
                     ms->numa_state->അക്കം_nodes, ms->smp.cpus);
        exit(EXIT_FAILURE);
    }
    എങ്കിൽ (ms->numa_state->അക്കം_nodes) {
        nidx = idx / (ms->smp.cpus / ms->numa_state->അക്കം_nodes);
        എങ്കിൽ (ms->numa_state->അക്കം_nodes <= nidx) {
            nidx = ms->numa_state->അക്കം_nodes - 1;
        }
    }

    തിരികെ nidx;
}

സ്ഥിരം CPUArchIdList *riscv_numa_possible_cpu_arch_ids(MachineState *ms)
{
    പൂർണ്ണസംഖ്യ n;
    unsigned പൂർണ്ണസംഖ്യ max_cpus = ms->smp.max_cpus;

    എങ്കിൽ (ms->possible_cpus) {
        പ്രതിജ്ഞാ(ms->possible_cpus->len == max_cpus);
        തിരികെ ms->possible_cpus;
    }

    ms->possible_cpus = g_malloc0(വലുപ്പം(CPUArchIdList) +
                                  വലുപ്പം(CPUArchId) * max_cpus);
    ms->possible_cpus->len = max_cpus;
    വേണ്ടി (n = 0; n < ms->possible_cpus->len; n++) {
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

    തിരികെ ms->possible_cpus;
}
