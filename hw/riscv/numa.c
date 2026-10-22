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

#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/osdep.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/units.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/error-report.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qapi/error.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/boards.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/qdev-properties.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/numa.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/device_tree.h"

bool riscv_numa_enabled(ਅਚਲ MachineState *ms)
{
    ਵਾਪਸ (ms->numa_state && ms->numa_state->ਅੰਕ_nodes) ? true : false;
}

ਪੂਰਨ riscv_socket_ਗਿਣਤੀ(ਅਚਲ MachineState *ms)
{
    ਵਾਪਸ (riscv_numa_enabled(ms)) ? ms->numa_state->ਅੰਕ_nodes : 1;
}

ਪੂਰਨ riscv_socket_ਪਹਿਲਾ_hartid(ਅਚਲ MachineState *ms, ਪੂਰਨ socket_id)
{
    ਪੂਰਨ i, ਪਹਿਲਾ_hartid = ms->smp.cpus;

    ਜੇ (!riscv_numa_enabled(ms)) {
        ਵਾਪਸ (!socket_id) ? 0 : -1;
    }

    ਲਈ (i = 0; i < ms->smp.cpus; i++) {
        ਜੇ (ms->possible_cpus->cpus[i].props.ਗੰਢ_id != socket_id) {
            ਜਾਰੀ;
        }
        ਜੇ (i < ਪਹਿਲਾ_hartid) {
            ਪਹਿਲਾ_hartid = i;
        }
    }

    ਵਾਪਸ (ਪਹਿਲਾ_hartid < ms->smp.cpus) ? ਪਹਿਲਾ_hartid : -1;
}

ਪੂਰਨ riscv_socket_ਆਖਰੀ_hartid(ਅਚਲ MachineState *ms, ਪੂਰਨ socket_id)
{
    ਪੂਰਨ i, ਆਖਰੀ_hartid = -1;

    ਜੇ (!riscv_numa_enabled(ms)) {
        ਵਾਪਸ (!socket_id) ? ms->smp.cpus - 1 : -1;
    }

    ਲਈ (i = 0; i < ms->smp.cpus; i++) {
        ਜੇ (ms->possible_cpus->cpus[i].props.ਗੰਢ_id != socket_id) {
            ਜਾਰੀ;
        }
        ਜੇ (i > ਆਖਰੀ_hartid) {
            ਆਖਰੀ_hartid = i;
        }
    }

    ਵਾਪਸ (ਆਖਰੀ_hartid < ms->smp.cpus) ? ਆਖਰੀ_hartid : -1;
}

ਪੂਰਨ riscv_socket_hart_ਗਿਣਤੀ(ਅਚਲ MachineState *ms, ਪੂਰਨ socket_id)
{
    ਪੂਰਨ ਪਹਿਲਾ_hartid, ਆਖਰੀ_hartid;

    ਜੇ (!riscv_numa_enabled(ms)) {
        ਵਾਪਸ (!socket_id) ? ms->smp.cpus : -1;
    }

    ਪਹਿਲਾ_hartid = riscv_socket_ਪਹਿਲਾ_hartid(ms, socket_id);
    ਜੇ (ਪਹਿਲਾ_hartid < 0) {
        ਵਾਪਸ -1;
    }

    ਆਖਰੀ_hartid = riscv_socket_ਆਖਰੀ_hartid(ms, socket_id);
    ਜੇ (ਆਖਰੀ_hartid < 0) {
        ਵਾਪਸ -1;
    }

    ਜੇ (ਪਹਿਲਾ_hartid > ਆਖਰੀ_hartid) {
        ਵਾਪਸ -1;
    }

    ਵਾਪਸ ਆਖਰੀ_hartid - ਪਹਿਲਾ_hartid + 1;
}

bool riscv_socket_check_hartids(ਅਚਲ MachineState *ms, ਪੂਰਨ socket_id)
{
    ਪੂਰਨ i, ਪਹਿਲਾ_hartid, ਆਖਰੀ_hartid;

    ਜੇ (!riscv_numa_enabled(ms)) {
        ਵਾਪਸ (!socket_id) ? true : false;
    }

    ਪਹਿਲਾ_hartid = riscv_socket_ਪਹਿਲਾ_hartid(ms, socket_id);
    ਜੇ (ਪਹਿਲਾ_hartid < 0) {
        ਵਾਪਸ false;
    }

    ਆਖਰੀ_hartid = riscv_socket_ਆਖਰੀ_hartid(ms, socket_id);
    ਜੇ (ਆਖਰੀ_hartid < 0) {
        ਵਾਪਸ false;
    }

    ਲਈ (i = ਪਹਿਲਾ_hartid; i <= ਆਖਰੀ_hartid; i++) {
        ਜੇ (ms->possible_cpus->cpus[i].props.ਗੰਢ_id != socket_id) {
            ਵਾਪਸ false;
        }
    }

    ਵਾਪਸ true;
}

uint64_t riscv_socket_mem_ਵਿੱਥ(ਅਚਲ MachineState *ms, ਪੂਰਨ socket_id)
{
    ਪੂਰਨ i;
    uint64_t mem_ਵਿੱਥ = 0;

    ਜੇ (!riscv_numa_enabled(ms)) {
        ਵਾਪਸ 0;
    }

    ਲਈ (i = 0; i < ms->numa_state->ਅੰਕ_nodes; i++) {
        ਜੇ (i == socket_id) {
            ਰੋਕੋ;
        }
        mem_ਵਿੱਥ += ms->numa_state->nodes[i].ਗੰਢ_mem;
    }

    ਵਾਪਸ (i == socket_id) ? mem_ਵਿੱਥ : 0;
}

uint64_t riscv_socket_mem_ਮਾਪ(ਅਚਲ MachineState *ms, ਪੂਰਨ socket_id)
{
    ਜੇ (!riscv_numa_enabled(ms)) {
        ਵਾਪਸ (!socket_id) ? ms->ram_ਮਾਪ : 0;
    }

    ਵਾਪਸ (socket_id < ms->numa_state->ਅੰਕ_nodes) ?
            ms->numa_state->nodes[socket_id].ਗੰਢ_mem : 0;
}

ਖਾਲੀ riscv_socket_fdt_write_id(ਅਚਲ MachineState *ms, ਅਚਲ ਅੱਖਰ *ਗੰਢ_name,
                               ਪੂਰਨ socket_id)
{
    ਜੇ (riscv_numa_enabled(ms)) {
        qemu_fdt_setprop_cell(ms->fdt, ਗੰਢ_name, "numa-node-id", socket_id);
    }
}

ਖਾਲੀ riscv_socket_fdt_write_ਦੂਰੀ_ਮੈਟ੍ਰਿਕਸ(ਅਚਲ MachineState *ms)
{
    ਪੂਰਨ i, j, idx;
    g_autofree uint32_t *dist_ਮੈਟ੍ਰਿਕਸ = ਨਿਰਾ;
    uint32_t dist_ਮੈਟ੍ਰਿਕਸ_ਮਾਪ;

    ਜੇ (riscv_numa_enabled(ms) && ms->numa_state->have_numa_ਦੂਰੀ) {
        dist_ਮੈਟ੍ਰਿਕਸ_ਮਾਪ = riscv_socket_ਗਿਣਤੀ(ms) * riscv_socket_ਗਿਣਤੀ(ms);
        dist_ਮੈਟ੍ਰਿਕਸ_ਮਾਪ *= (3 * ਆਕਾਰ(uint32_t));
        dist_ਮੈਟ੍ਰਿਕਸ = g_malloc0(dist_ਮੈਟ੍ਰਿਕਸ_ਮਾਪ);

        ਲਈ (i = 0; i < riscv_socket_ਗਿਣਤੀ(ms); i++) {
            ਲਈ (j = 0; j < riscv_socket_ਗਿਣਤੀ(ms); j++) {
                idx = (i * riscv_socket_ਗਿਣਤੀ(ms) + j) * 3;
                dist_ਮੈਟ੍ਰਿਕਸ[idx + 0] = cpu_ਮੰਜ਼ਿਲ_be32(i);
                dist_ਮੈਟ੍ਰਿਕਸ[idx + 1] = cpu_ਮੰਜ਼ਿਲ_be32(j);
                dist_ਮੈਟ੍ਰਿਕਸ[idx + 2] =
                    cpu_ਮੰਜ਼ਿਲ_be32(ms->numa_state->nodes[i].ਦੂਰੀ[j]);
            }
        }

        qemu_fdt_ਜੋੜੋ_subnode(ms->fdt, "/distance-map");
        qemu_fdt_setprop_string(ms->fdt, "/distance-map", "compatible",
                                "numa-distance-map-v1");
        qemu_fdt_setprop(ms->fdt, "/distance-map", "distance-matrix",
                         dist_ਮੈਟ੍ਰਿਕਸ, dist_ਮੈਟ੍ਰਿਕਸ_ਮਾਪ);
    }
}

CpuInstanceProperties
riscv_numa_cpu_ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_props(MachineState *ms, ਬਿਨਾਨਿਸ਼ਾਨੀ cpu_ਸੂਚਕਾਂਕ)
{
    MachineClass *mc = MACHINE_GET_CLASS(ms);
    ਅਚਲ CPUArchIdList *possible_cpus = mc->possible_cpu_arch_ids(ms);

    ਪ੍ਰਤਿਜ੍ਞਾ(cpu_ਸੂਚਕਾਂਕ < possible_cpus->len);
    ਵਾਪਸ possible_cpus->cpus[cpu_ਸੂਚਕਾਂਕ].props;
}

int64_t riscv_numa_get_default_cpu_ਗੰਢ_id(ਅਚਲ MachineState *ms, ਪੂਰਨ idx)
{
    int64_t nidx = 0;

    ਜੇ (ms->numa_state->ਅੰਕ_nodes > ms->smp.cpus) {
        error_report("Number of NUMA nodes (%d)"
                     " cannot exceed the number of available CPUs (%u).",
                     ms->numa_state->ਅੰਕ_nodes, ms->smp.cpus);
        ਬਾਹਰ_ਨਿਕਲੋ(EXIT_FAILURE);
    }
    ਜੇ (ms->numa_state->ਅੰਕ_nodes) {
        nidx = idx / (ms->smp.cpus / ms->numa_state->ਅੰਕ_nodes);
        ਜੇ (ms->numa_state->ਅੰਕ_nodes <= nidx) {
            nidx = ms->numa_state->ਅੰਕ_nodes - 1;
        }
    }

    ਵਾਪਸ nidx;
}

ਅਚਲ CPUArchIdList *riscv_numa_possible_cpu_arch_ids(MachineState *ms)
{
    ਪੂਰਨ n;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ max_cpus = ms->smp.max_cpus;

    ਜੇ (ms->possible_cpus) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ms->possible_cpus->len == max_cpus);
        ਵਾਪਸ ms->possible_cpus;
    }

    ms->possible_cpus = g_malloc0(ਆਕਾਰ(CPUArchIdList) +
                                  ਆਕਾਰ(CPUArchId) * max_cpus);
    ms->possible_cpus->len = max_cpus;
    ਲਈ (n = 0; n < ms->possible_cpus->len; n++) {
        ms->possible_cpus->cpus[n].type = ms->cpu_type;
        ms->possible_cpus->cpus[n].arch_id = n;
        ms->possible_cpus->cpus[n].props.has_socket_id = true;
        ms->possible_cpus->cpus[n].props.socket_id =
            n / (ms->smp.clusters * ms->smp.cores * ms->smp.ਧਾਗੇ);
        ms->possible_cpus->cpus[n].props.has_cluster_id = true;
        ms->possible_cpus->cpus[n].props.cluster_id =
            (n / (ms->smp.cores * ms->smp.ਧਾਗੇ)) % ms->smp.clusters;
        ms->possible_cpus->cpus[n].props.has_core_id = true;
        ms->possible_cpus->cpus[n].props.core_id =
            (n / ms->smp.ਧਾਗੇ) % ms->smp.cores;
        ms->possible_cpus->cpus[n].props.has_thread_id = true;
        ms->possible_cpus->cpus[n].props.thread_id =
            n % ms->smp.ਧਾਗੇ;
    }

    ਵਾਪਸ ms->possible_cpus;
}
