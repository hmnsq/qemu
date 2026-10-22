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
    أرجع (ms->numa_state && ms->numa_state->عدد_nodes) ? true : false;
}

صحيح riscv_socket_گنتی(ثابت MachineState *ms)
{
    أرجع (riscv_numa_enabled(ms)) ? ms->numa_state->عدد_nodes : 1;
}

صحيح riscv_socket_پہلا_hartid(ثابت MachineState *ms, صحيح socket_id)
{
    صحيح i, پہلا_hartid = ms->smp.cpus;

    إذا (!riscv_numa_enabled(ms)) {
        أرجع (!socket_id) ? 0 : -1;
    }

    لكل (i = 0; i < ms->smp.cpus; i++) {
        إذا (ms->possible_cpus->cpus[i].props.گرہ_id != socket_id) {
            استمر;
        }
        إذا (i < پہلا_hartid) {
            پہلا_hartid = i;
        }
    }

    أرجع (پہلا_hartid < ms->smp.cpus) ? پہلا_hartid : -1;
}

صحيح riscv_socket_آخری_hartid(ثابت MachineState *ms, صحيح socket_id)
{
    صحيح i, آخری_hartid = -1;

    إذا (!riscv_numa_enabled(ms)) {
        أرجع (!socket_id) ? ms->smp.cpus - 1 : -1;
    }

    لكل (i = 0; i < ms->smp.cpus; i++) {
        إذا (ms->possible_cpus->cpus[i].props.گرہ_id != socket_id) {
            استمر;
        }
        إذا (i > آخری_hartid) {
            آخری_hartid = i;
        }
    }

    أرجع (آخری_hartid < ms->smp.cpus) ? آخری_hartid : -1;
}

صحيح riscv_socket_hart_گنتی(ثابت MachineState *ms, صحيح socket_id)
{
    صحيح پہلا_hartid, آخری_hartid;

    إذا (!riscv_numa_enabled(ms)) {
        أرجع (!socket_id) ? ms->smp.cpus : -1;
    }

    پہلا_hartid = riscv_socket_پہلا_hartid(ms, socket_id);
    إذا (پہلا_hartid < 0) {
        أرجع -1;
    }

    آخری_hartid = riscv_socket_آخری_hartid(ms, socket_id);
    إذا (آخری_hartid < 0) {
        أرجع -1;
    }

    إذا (پہلا_hartid > آخری_hartid) {
        أرجع -1;
    }

    أرجع آخری_hartid - پہلا_hartid + 1;
}

bool riscv_socket_check_hartids(ثابت MachineState *ms, صحيح socket_id)
{
    صحيح i, پہلا_hartid, آخری_hartid;

    إذا (!riscv_numa_enabled(ms)) {
        أرجع (!socket_id) ? true : false;
    }

    پہلا_hartid = riscv_socket_پہلا_hartid(ms, socket_id);
    إذا (پہلا_hartid < 0) {
        أرجع false;
    }

    آخری_hartid = riscv_socket_آخری_hartid(ms, socket_id);
    إذا (آخری_hartid < 0) {
        أرجع false;
    }

    لكل (i = پہلا_hartid; i <= آخری_hartid; i++) {
        إذا (ms->possible_cpus->cpus[i].props.گرہ_id != socket_id) {
            أرجع false;
        }
    }

    أرجع true;
}

uint64_t riscv_socket_mem_وتھ(ثابت MachineState *ms, صحيح socket_id)
{
    صحيح i;
    uint64_t mem_وتھ = 0;

    إذا (!riscv_numa_enabled(ms)) {
        أرجع 0;
    }

    لكل (i = 0; i < ms->numa_state->عدد_nodes; i++) {
        إذا (i == socket_id) {
            اكسر;
        }
        mem_وتھ += ms->numa_state->nodes[i].گرہ_mem;
    }

    أرجع (i == socket_id) ? mem_وتھ : 0;
}

uint64_t riscv_socket_mem_ناپ(ثابت MachineState *ms, صحيح socket_id)
{
    إذا (!riscv_numa_enabled(ms)) {
        أرجع (!socket_id) ? ms->ram_ناپ : 0;
    }

    أرجع (socket_id < ms->numa_state->عدد_nodes) ?
            ms->numa_state->nodes[socket_id].گرہ_mem : 0;
}

فراغ riscv_socket_fdt_write_id(ثابت MachineState *ms, ثابت حرف *گرہ_name,
                               صحيح socket_id)
{
    إذا (riscv_numa_enabled(ms)) {
        qemu_fdt_setprop_cell(ms->fdt, گرہ_name, "numa-node-id", socket_id);
    }
}

فراغ riscv_socket_fdt_write_فاصلہ_میٹرکس(ثابت MachineState *ms)
{
    صحيح i, j, idx;
    g_autofree uint32_t *dist_میٹرکس = عدم;
    uint32_t dist_میٹرکس_ناپ;

    إذا (riscv_numa_enabled(ms) && ms->numa_state->have_numa_فاصلہ) {
        dist_میٹرکس_ناپ = riscv_socket_گنتی(ms) * riscv_socket_گنتی(ms);
        dist_میٹرکس_ناپ *= (3 * حجم(uint32_t));
        dist_میٹرکس = g_malloc0(dist_میٹرکس_ناپ);

        لكل (i = 0; i < riscv_socket_گنتی(ms); i++) {
            لكل (j = 0; j < riscv_socket_گنتی(ms); j++) {
                idx = (i * riscv_socket_گنتی(ms) + j) * 3;
                dist_میٹرکس[idx + 0] = cpu_منزل_be32(i);
                dist_میٹرکس[idx + 1] = cpu_منزل_be32(j);
                dist_میٹرکس[idx + 2] =
                    cpu_منزل_be32(ms->numa_state->nodes[i].فاصلہ[j]);
            }
        }

        qemu_fdt_جوڑو_subnode(ms->fdt, "/distance-map");
        qemu_fdt_setprop_string(ms->fdt, "/distance-map", "compatible",
                                "numa-distance-map-v1");
        qemu_fdt_setprop(ms->fdt, "/distance-map", "distance-matrix",
                         dist_میٹرکس, dist_میٹرکس_ناپ);
    }
}

CpuInstanceProperties
riscv_numa_cpu_اشاریہ_منزل_props(MachineState *ms, بنانشانی cpu_اشاریہ)
{
    MachineClass *mc = MACHINE_GET_CLASS(ms);
    ثابت CPUArchIdList *possible_cpus = mc->possible_cpu_arch_ids(ms);

    أكد(cpu_اشاریہ < possible_cpus->len);
    أرجع possible_cpus->cpus[cpu_اشاریہ].props;
}

int64_t riscv_numa_get_default_cpu_گرہ_id(ثابت MachineState *ms, صحيح idx)
{
    int64_t nidx = 0;

    إذا (ms->numa_state->عدد_nodes > ms->smp.cpus) {
        error_report("Number of NUMA nodes (%d)"
                     " cannot exceed the number of available CPUs (%u).",
                     ms->numa_state->عدد_nodes, ms->smp.cpus);
        باہر_نکلو(EXIT_FAILURE);
    }
    إذا (ms->numa_state->عدد_nodes) {
        nidx = idx / (ms->smp.cpus / ms->numa_state->عدد_nodes);
        إذا (ms->numa_state->عدد_nodes <= nidx) {
            nidx = ms->numa_state->عدد_nodes - 1;
        }
    }

    أرجع nidx;
}

ثابت CPUArchIdList *riscv_numa_possible_cpu_arch_ids(MachineState *ms)
{
    صحيح n;
    بنانشانی صحيح max_cpus = ms->smp.max_cpus;

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
            n / (ms->smp.clusters * ms->smp.cores * ms->smp.دھاگے);
        ms->possible_cpus->cpus[n].props.has_cluster_id = true;
        ms->possible_cpus->cpus[n].props.cluster_id =
            (n / (ms->smp.cores * ms->smp.دھاگے)) % ms->smp.clusters;
        ms->possible_cpus->cpus[n].props.has_core_id = true;
        ms->possible_cpus->cpus[n].props.core_id =
            (n / ms->smp.دھاگے) % ms->smp.cores;
        ms->possible_cpus->cpus[n].props.has_thread_id = true;
        ms->possible_cpus->cpus[n].props.thread_id =
            n % ms->smp.دھاگے;
    }

    أرجع ms->possible_cpus;
}
