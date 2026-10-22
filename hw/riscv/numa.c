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
    أرجع (ms->numa_state && ms->numa_state->رقم_nodes) ? true : false;
}

صحيح riscv_socket_شمار(ثابت MachineState *ms)
{
    أرجع (riscv_numa_enabled(ms)) ? ms->numa_state->رقم_nodes : 1;
}

صحيح riscv_socket_اولی_hartid(ثابت MachineState *ms, صحيح socket_id)
{
    صحيح i, اولی_hartid = ms->smp.cpus;

    إذا (!riscv_numa_enabled(ms)) {
        أرجع (!socket_id) ? 0 : -1;
    }

    لكل (i = 0; i < ms->smp.cpus; i++) {
        إذا (ms->possible_cpus->cpus[i].props.گره_id != socket_id) {
            استمر;
        }
        إذا (i < اولی_hartid) {
            اولی_hartid = i;
        }
    }

    أرجع (اولی_hartid < ms->smp.cpus) ? اولی_hartid : -1;
}

صحيح riscv_socket_آخرین_hartid(ثابت MachineState *ms, صحيح socket_id)
{
    صحيح i, آخرین_hartid = -1;

    إذا (!riscv_numa_enabled(ms)) {
        أرجع (!socket_id) ? ms->smp.cpus - 1 : -1;
    }

    لكل (i = 0; i < ms->smp.cpus; i++) {
        إذا (ms->possible_cpus->cpus[i].props.گره_id != socket_id) {
            استمر;
        }
        إذا (i > آخرین_hartid) {
            آخرین_hartid = i;
        }
    }

    أرجع (آخرین_hartid < ms->smp.cpus) ? آخرین_hartid : -1;
}

صحيح riscv_socket_hart_شمار(ثابت MachineState *ms, صحيح socket_id)
{
    صحيح اولی_hartid, آخرین_hartid;

    إذا (!riscv_numa_enabled(ms)) {
        أرجع (!socket_id) ? ms->smp.cpus : -1;
    }

    اولی_hartid = riscv_socket_اولی_hartid(ms, socket_id);
    إذا (اولی_hartid < 0) {
        أرجع -1;
    }

    آخرین_hartid = riscv_socket_آخرین_hartid(ms, socket_id);
    إذا (آخرین_hartid < 0) {
        أرجع -1;
    }

    إذا (اولی_hartid > آخرین_hartid) {
        أرجع -1;
    }

    أرجع آخرین_hartid - اولی_hartid + 1;
}

bool riscv_socket_check_hartids(ثابت MachineState *ms, صحيح socket_id)
{
    صحيح i, اولی_hartid, آخرین_hartid;

    إذا (!riscv_numa_enabled(ms)) {
        أرجع (!socket_id) ? true : false;
    }

    اولی_hartid = riscv_socket_اولی_hartid(ms, socket_id);
    إذا (اولی_hartid < 0) {
        أرجع false;
    }

    آخرین_hartid = riscv_socket_آخرین_hartid(ms, socket_id);
    إذا (آخرین_hartid < 0) {
        أرجع false;
    }

    لكل (i = اولی_hartid; i <= آخرین_hartid; i++) {
        إذا (ms->possible_cpus->cpus[i].props.گره_id != socket_id) {
            أرجع false;
        }
    }

    أرجع true;
}

uint64_t riscv_socket_mem_جابه_جایی(ثابت MachineState *ms, صحيح socket_id)
{
    صحيح i;
    uint64_t mem_جابه_جایی = 0;

    إذا (!riscv_numa_enabled(ms)) {
        أرجع 0;
    }

    لكل (i = 0; i < ms->numa_state->رقم_nodes; i++) {
        إذا (i == socket_id) {
            اكسر;
        }
        mem_جابه_جایی += ms->numa_state->nodes[i].گره_mem;
    }

    أرجع (i == socket_id) ? mem_جابه_جایی : 0;
}

uint64_t riscv_socket_mem_اندازه(ثابت MachineState *ms, صحيح socket_id)
{
    إذا (!riscv_numa_enabled(ms)) {
        أرجع (!socket_id) ? ms->ram_اندازه : 0;
    }

    أرجع (socket_id < ms->numa_state->رقم_nodes) ?
            ms->numa_state->nodes[socket_id].گره_mem : 0;
}

فراغ riscv_socket_fdt_write_id(ثابت MachineState *ms, ثابت حرف *گره_name,
                               صحيح socket_id)
{
    إذا (riscv_numa_enabled(ms)) {
        qemu_fdt_setprop_cell(ms->fdt, گره_name, "numa-node-id", socket_id);
    }
}

فراغ riscv_socket_fdt_write_فاصله_ماتریس(ثابت MachineState *ms)
{
    صحيح i, j, idx;
    g_autofree uint32_t *dist_ماتریس = عدم;
    uint32_t dist_ماتریس_اندازه;

    إذا (riscv_numa_enabled(ms) && ms->numa_state->have_numa_فاصله) {
        dist_ماتریس_اندازه = riscv_socket_شمار(ms) * riscv_socket_شمار(ms);
        dist_ماتریس_اندازه *= (3 * حجم(uint32_t));
        dist_ماتریس = g_malloc0(dist_ماتریس_اندازه);

        لكل (i = 0; i < riscv_socket_شمار(ms); i++) {
            لكل (j = 0; j < riscv_socket_شمار(ms); j++) {
                idx = (i * riscv_socket_شمار(ms) + j) * 3;
                dist_ماتریس[idx + 0] = cpu_مقصد_be32(i);
                dist_ماتریس[idx + 1] = cpu_مقصد_be32(j);
                dist_ماتریس[idx + 2] =
                    cpu_مقصد_be32(ms->numa_state->nodes[i].فاصله[j]);
            }
        }

        qemu_fdt_جمع_کن_subnode(ms->fdt, "/distance-map");
        qemu_fdt_setprop_string(ms->fdt, "/distance-map", "compatible",
                                "numa-distance-map-v1");
        qemu_fdt_setprop(ms->fdt, "/distance-map", "distance-matrix",
                         dist_ماتریس, dist_ماتریس_اندازه);
    }
}

CpuInstanceProperties
riscv_numa_cpu_شاخص_مقصد_props(MachineState *ms, بیعلامت cpu_شاخص)
{
    MachineClass *mc = MACHINE_GET_CLASS(ms);
    ثابت CPUArchIdList *possible_cpus = mc->possible_cpu_arch_ids(ms);

    أكد(cpu_شاخص < possible_cpus->len);
    أرجع possible_cpus->cpus[cpu_شاخص].props;
}

int64_t riscv_numa_get_default_cpu_گره_id(ثابت MachineState *ms, صحيح idx)
{
    int64_t nidx = 0;

    إذا (ms->numa_state->رقم_nodes > ms->smp.cpus) {
        error_report("Number of NUMA nodes (%d)"
                     " cannot exceed the number of available CPUs (%u).",
                     ms->numa_state->رقم_nodes, ms->smp.cpus);
        خروج(EXIT_FAILURE);
    }
    إذا (ms->numa_state->رقم_nodes) {
        nidx = idx / (ms->smp.cpus / ms->numa_state->رقم_nodes);
        إذا (ms->numa_state->رقم_nodes <= nidx) {
            nidx = ms->numa_state->رقم_nodes - 1;
        }
    }

    أرجع nidx;
}

ثابت CPUArchIdList *riscv_numa_possible_cpu_arch_ids(MachineState *ms)
{
    صحيح n;
    بیعلامت صحيح max_cpus = ms->smp.max_cpus;

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
            n / (ms->smp.clusters * ms->smp.cores * ms->smp.نخها);
        ms->possible_cpus->cpus[n].props.has_cluster_id = true;
        ms->possible_cpus->cpus[n].props.cluster_id =
            (n / (ms->smp.cores * ms->smp.نخها)) % ms->smp.clusters;
        ms->possible_cpus->cpus[n].props.has_core_id = true;
        ms->possible_cpus->cpus[n].props.core_id =
            (n / ms->smp.نخها) % ms->smp.cores;
        ms->possible_cpus->cpus[n].props.has_thread_id = true;
        ms->possible_cpus->cpus[n].props.thread_id =
            n % ms->smp.نخها;
    }

    أرجع ms->possible_cpus;
}
