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

#شامل "qemu/osdep.h"
#شامل "qemu/units.h"
#شامل "qemu/error-report.h"
#شامل "qapi/error.h"
#شامل "hw/core/boards.h"
#شامل "hw/core/qdev-properties.h"
#شامل "hw/riscv/numa.h"
#شامل "system/device_tree.h"

bool riscv_numa_enabled(ثابت MachineState *ms)
{
    بازگشت (ms->numa_state && ms->numa_state->رقم_nodes) ? true : false;
}

صحیح riscv_socket_شمار(ثابت MachineState *ms)
{
    بازگشت (riscv_numa_enabled(ms)) ? ms->numa_state->رقم_nodes : 1;
}

صحیح riscv_socket_first_hartid(ثابت MachineState *ms, صحیح socket_id)
{
    صحیح i, first_hartid = ms->smp.cpus;

    اگر (!riscv_numa_enabled(ms)) {
        بازگشت (!socket_id) ? 0 : -1;
    }

    برای (i = 0; i < ms->smp.cpus; i++) {
        اگر (ms->possible_cpus->cpus[i].props.گره_id != socket_id) {
            ادامه;
        }
        اگر (i < first_hartid) {
            first_hartid = i;
        }
    }

    بازگشت (first_hartid < ms->smp.cpus) ? first_hartid : -1;
}

صحیح riscv_socket_last_hartid(ثابت MachineState *ms, صحیح socket_id)
{
    صحیح i, last_hartid = -1;

    اگر (!riscv_numa_enabled(ms)) {
        بازگشت (!socket_id) ? ms->smp.cpus - 1 : -1;
    }

    برای (i = 0; i < ms->smp.cpus; i++) {
        اگر (ms->possible_cpus->cpus[i].props.گره_id != socket_id) {
            ادامه;
        }
        اگر (i > last_hartid) {
            last_hartid = i;
        }
    }

    بازگشت (last_hartid < ms->smp.cpus) ? last_hartid : -1;
}

صحیح riscv_socket_hart_شمار(ثابت MachineState *ms, صحیح socket_id)
{
    صحیح first_hartid, last_hartid;

    اگر (!riscv_numa_enabled(ms)) {
        بازگشت (!socket_id) ? ms->smp.cpus : -1;
    }

    first_hartid = riscv_socket_first_hartid(ms, socket_id);
    اگر (first_hartid < 0) {
        بازگشت -1;
    }

    last_hartid = riscv_socket_last_hartid(ms, socket_id);
    اگر (last_hartid < 0) {
        بازگشت -1;
    }

    اگر (first_hartid > last_hartid) {
        بازگشت -1;
    }

    بازگشت last_hartid - first_hartid + 1;
}

bool riscv_socket_check_hartids(ثابت MachineState *ms, صحیح socket_id)
{
    صحیح i, first_hartid, last_hartid;

    اگر (!riscv_numa_enabled(ms)) {
        بازگشت (!socket_id) ? true : false;
    }

    first_hartid = riscv_socket_first_hartid(ms, socket_id);
    اگر (first_hartid < 0) {
        بازگشت false;
    }

    last_hartid = riscv_socket_last_hartid(ms, socket_id);
    اگر (last_hartid < 0) {
        بازگشت false;
    }

    برای (i = first_hartid; i <= last_hartid; i++) {
        اگر (ms->possible_cpus->cpus[i].props.گره_id != socket_id) {
            بازگشت false;
        }
    }

    بازگشت true;
}

uint64_t riscv_socket_mem_offset(ثابت MachineState *ms, صحیح socket_id)
{
    صحیح i;
    uint64_t mem_offset = 0;

    اگر (!riscv_numa_enabled(ms)) {
        بازگشت 0;
    }

    برای (i = 0; i < ms->numa_state->رقم_nodes; i++) {
        اگر (i == socket_id) {
            بشکن;
        }
        mem_offset += ms->numa_state->nodes[i].گره_mem;
    }

    بازگشت (i == socket_id) ? mem_offset : 0;
}

uint64_t riscv_socket_mem_size(ثابت MachineState *ms, صحیح socket_id)
{
    اگر (!riscv_numa_enabled(ms)) {
        بازگشت (!socket_id) ? ms->ram_size : 0;
    }

    بازگشت (socket_id < ms->numa_state->رقم_nodes) ?
            ms->numa_state->nodes[socket_id].گره_mem : 0;
}

تهی riscv_socket_fdt_write_id(ثابت MachineState *ms, ثابت نویسه *گره_name,
                               صحیح socket_id)
{
    اگر (riscv_numa_enabled(ms)) {
        qemu_fdt_setprop_cell(ms->fdt, گره_name, "numa-node-id", socket_id);
    }
}

تهی riscv_socket_fdt_write_distance_matrix(ثابت MachineState *ms)
{
    صحیح i, j, idx;
    g_autofree uint32_t *dist_matrix = پوچ;
    uint32_t dist_matrix_size;

    اگر (riscv_numa_enabled(ms) && ms->numa_state->have_numa_distance) {
        dist_matrix_size = riscv_socket_شمار(ms) * riscv_socket_شمار(ms);
        dist_matrix_size *= (3 * اندازه(uint32_t));
        dist_matrix = g_malloc0(dist_matrix_size);

        برای (i = 0; i < riscv_socket_شمار(ms); i++) {
            برای (j = 0; j < riscv_socket_شمار(ms); j++) {
                idx = (i * riscv_socket_شمار(ms) + j) * 3;
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
    ثابت CPUArchIdList *possible_cpus = mc->possible_cpu_arch_ids(ms);

    أكد(cpu_index < possible_cpus->len);
    بازگشت possible_cpus->cpus[cpu_index].props;
}

int64_t riscv_numa_get_default_cpu_گره_id(ثابت MachineState *ms, صحیح idx)
{
    int64_t nidx = 0;

    اگر (ms->numa_state->رقم_nodes > ms->smp.cpus) {
        error_report("Number of NUMA nodes (%d)"
                     " cannot exceed the number of available CPUs (%u).",
                     ms->numa_state->رقم_nodes, ms->smp.cpus);
        exit(EXIT_FAILURE);
    }
    اگر (ms->numa_state->رقم_nodes) {
        nidx = idx / (ms->smp.cpus / ms->numa_state->رقم_nodes);
        اگر (ms->numa_state->رقم_nodes <= nidx) {
            nidx = ms->numa_state->رقم_nodes - 1;
        }
    }

    بازگشت nidx;
}

ثابت CPUArchIdList *riscv_numa_possible_cpu_arch_ids(MachineState *ms)
{
    صحیح n;
    unsigned صحیح max_cpus = ms->smp.max_cpus;

    اگر (ms->possible_cpus) {
        أكد(ms->possible_cpus->len == max_cpus);
        بازگشت ms->possible_cpus;
    }

    ms->possible_cpus = g_malloc0(اندازه(CPUArchIdList) +
                                  اندازه(CPUArchId) * max_cpus);
    ms->possible_cpus->len = max_cpus;
    برای (n = 0; n < ms->possible_cpus->len; n++) {
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

    بازگشت ms->possible_cpus;
}
