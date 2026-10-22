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

#ضمن "qemu/osdep.h"
#ضمن "qemu/units.h"
#ضمن "qemu/error-report.h"
#ضمن "qapi/error.h"
#ضمن "hw/core/boards.h"
#ضمن "hw/core/qdev-properties.h"
#ضمن "hw/riscv/numa.h"
#ضمن "system/device_tree.h"

bool riscv_numa_enabled(ثابت MachineState *ms)
{
    أرجع (ms->numa_state && ms->numa_state->انگ_nodes) ? true : false;
}

صحيح riscv_socket_ڳڻپ(ثابت MachineState *ms)
{
    أرجع (riscv_numa_enabled(ms)) ? ms->numa_state->انگ_nodes : 1;
}

صحيح riscv_socket_پهريون_hartid(ثابت MachineState *ms, صحيح socket_id)
{
    صحيح i, پهريون_hartid = ms->smp.cpus;

    إذا (!riscv_numa_enabled(ms)) {
        أرجع (!socket_id) ? 0 : -1;
    }

    لكل (i = 0; i < ms->smp.cpus; i++) {
        إذا (ms->possible_cpus->cpus[i].props.ڳنڍ_id != socket_id) {
            استمر;
        }
        إذا (i < پهريون_hartid) {
            پهريون_hartid = i;
        }
    }

    أرجع (پهريون_hartid < ms->smp.cpus) ? پهريون_hartid : -1;
}

صحيح riscv_socket_آخري_hartid(ثابت MachineState *ms, صحيح socket_id)
{
    صحيح i, آخري_hartid = -1;

    إذا (!riscv_numa_enabled(ms)) {
        أرجع (!socket_id) ? ms->smp.cpus - 1 : -1;
    }

    لكل (i = 0; i < ms->smp.cpus; i++) {
        إذا (ms->possible_cpus->cpus[i].props.ڳنڍ_id != socket_id) {
            استمر;
        }
        إذا (i > آخري_hartid) {
            آخري_hartid = i;
        }
    }

    أرجع (آخري_hartid < ms->smp.cpus) ? آخري_hartid : -1;
}

صحيح riscv_socket_hart_ڳڻپ(ثابت MachineState *ms, صحيح socket_id)
{
    صحيح پهريون_hartid, آخري_hartid;

    إذا (!riscv_numa_enabled(ms)) {
        أرجع (!socket_id) ? ms->smp.cpus : -1;
    }

    پهريون_hartid = riscv_socket_پهريون_hartid(ms, socket_id);
    إذا (پهريون_hartid < 0) {
        أرجع -1;
    }

    آخري_hartid = riscv_socket_آخري_hartid(ms, socket_id);
    إذا (آخري_hartid < 0) {
        أرجع -1;
    }

    إذا (پهريون_hartid > آخري_hartid) {
        أرجع -1;
    }

    أرجع آخري_hartid - پهريون_hartid + 1;
}

bool riscv_socket_check_hartids(ثابت MachineState *ms, صحيح socket_id)
{
    صحيح i, پهريون_hartid, آخري_hartid;

    إذا (!riscv_numa_enabled(ms)) {
        أرجع (!socket_id) ? true : false;
    }

    پهريون_hartid = riscv_socket_پهريون_hartid(ms, socket_id);
    إذا (پهريون_hartid < 0) {
        أرجع false;
    }

    آخري_hartid = riscv_socket_آخري_hartid(ms, socket_id);
    إذا (آخري_hartid < 0) {
        أرجع false;
    }

    لكل (i = پهريون_hartid; i <= آخري_hartid; i++) {
        إذا (ms->possible_cpus->cpus[i].props.ڳنڍ_id != socket_id) {
            أرجع false;
        }
    }

    أرجع true;
}

uint64_t riscv_socket_mem_وٿي(ثابت MachineState *ms, صحيح socket_id)
{
    صحيح i;
    uint64_t mem_وٿي = 0;

    إذا (!riscv_numa_enabled(ms)) {
        أرجع 0;
    }

    لكل (i = 0; i < ms->numa_state->انگ_nodes; i++) {
        إذا (i == socket_id) {
            اكسر;
        }
        mem_وٿي += ms->numa_state->nodes[i].ڳنڍ_mem;
    }

    أرجع (i == socket_id) ? mem_وٿي : 0;
}

uint64_t riscv_socket_mem_سائيز(ثابت MachineState *ms, صحيح socket_id)
{
    إذا (!riscv_numa_enabled(ms)) {
        أرجع (!socket_id) ? ms->ram_سائيز : 0;
    }

    أرجع (socket_id < ms->numa_state->انگ_nodes) ?
            ms->numa_state->nodes[socket_id].ڳنڍ_mem : 0;
}

فراغ riscv_socket_fdt_write_id(ثابت MachineState *ms, ثابت حرف *ڳنڍ_name,
                               صحيح socket_id)
{
    إذا (riscv_numa_enabled(ms)) {
        qemu_fdt_setprop_cell(ms->fdt, ڳنڍ_name, "numa-node-id", socket_id);
    }
}

فراغ riscv_socket_fdt_write_مفاصلو_ميٽرڪس(ثابت MachineState *ms)
{
    صحيح i, j, idx;
    g_autofree uint32_t *dist_ميٽرڪس = عدم;
    uint32_t dist_ميٽرڪس_سائيز;

    إذا (riscv_numa_enabled(ms) && ms->numa_state->have_numa_مفاصلو) {
        dist_ميٽرڪس_سائيز = riscv_socket_ڳڻپ(ms) * riscv_socket_ڳڻپ(ms);
        dist_ميٽرڪس_سائيز *= (3 * حجم(uint32_t));
        dist_ميٽرڪس = g_malloc0(dist_ميٽرڪس_سائيز);

        لكل (i = 0; i < riscv_socket_ڳڻپ(ms); i++) {
            لكل (j = 0; j < riscv_socket_ڳڻپ(ms); j++) {
                idx = (i * riscv_socket_ڳڻپ(ms) + j) * 3;
                dist_ميٽرڪس[idx + 0] = cpu_منزل_be32(i);
                dist_ميٽرڪس[idx + 1] = cpu_منزل_be32(j);
                dist_ميٽرڪس[idx + 2] =
                    cpu_منزل_be32(ms->numa_state->nodes[i].مفاصلو[j]);
            }
        }

        qemu_fdt_جوڙيو_subnode(ms->fdt, "/distance-map");
        qemu_fdt_setprop_string(ms->fdt, "/distance-map", "compatible",
                                "numa-distance-map-v1");
        qemu_fdt_setprop(ms->fdt, "/distance-map", "distance-matrix",
                         dist_ميٽرڪس, dist_ميٽرڪس_سائيز);
    }
}

CpuInstanceProperties
riscv_numa_cpu_اشاريو_منزل_props(MachineState *ms, بنانشان cpu_اشاريو)
{
    MachineClass *mc = MACHINE_GET_CLASS(ms);
    ثابت CPUArchIdList *possible_cpus = mc->possible_cpu_arch_ids(ms);

    أكد(cpu_اشاريو < possible_cpus->len);
    أرجع possible_cpus->cpus[cpu_اشاريو].props;
}

int64_t riscv_numa_get_default_cpu_ڳنڍ_id(ثابت MachineState *ms, صحيح idx)
{
    int64_t nidx = 0;

    إذا (ms->numa_state->انگ_nodes > ms->smp.cpus) {
        error_report("Number of NUMA nodes (%d)"
                     " cannot exceed the number of available CPUs (%u).",
                     ms->numa_state->انگ_nodes, ms->smp.cpus);
        ٻاهر_نڪرو(EXIT_FAILURE);
    }
    إذا (ms->numa_state->انگ_nodes) {
        nidx = idx / (ms->smp.cpus / ms->numa_state->انگ_nodes);
        إذا (ms->numa_state->انگ_nodes <= nidx) {
            nidx = ms->numa_state->انگ_nodes - 1;
        }
    }

    أرجع nidx;
}

ثابت CPUArchIdList *riscv_numa_possible_cpu_arch_ids(MachineState *ms)
{
    صحيح n;
    بنانشان صحيح max_cpus = ms->smp.max_cpus;

    إذا (ms->possible_cpus) {
        أكد(ms->possible_cpus->len == max_cpus);
        أرجع ms->possible_cpus;
    }

    ms->possible_cpus = g_malloc0(حجم(CPUArchIdList) +
                                  حجم(CPUArchId) * max_cpus);
    ms->possible_cpus->len = max_cpus;
    لكل (n = 0; n < ms->possible_cpus->len; n++) {
        ms->possible_cpus->cpus[n].type = ms->cpu_type;
        ms->possible_cpus->cpus[n].arch_id = n;
        ms->possible_cpus->cpus[n].props.has_socket_id = true;
        ms->possible_cpus->cpus[n].props.socket_id =
            n / (ms->smp.clusters * ms->smp.cores * ms->smp.ڌاڳا);
        ms->possible_cpus->cpus[n].props.has_cluster_id = true;
        ms->possible_cpus->cpus[n].props.cluster_id =
            (n / (ms->smp.cores * ms->smp.ڌاڳا)) % ms->smp.clusters;
        ms->possible_cpus->cpus[n].props.has_core_id = true;
        ms->possible_cpus->cpus[n].props.core_id =
            (n / ms->smp.ڌاڳا) % ms->smp.cores;
        ms->possible_cpus->cpus[n].props.has_thread_id = true;
        ms->possible_cpus->cpus[n].props.thread_id =
            n % ms->smp.ڌاڳا;
    }

    أرجع ms->possible_cpus;
}
