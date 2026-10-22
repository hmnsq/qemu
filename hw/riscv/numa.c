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

#అన్తర్భావ "qemu/osdep.h"
#అన్తర్భావ "qemu/units.h"
#అన్తర్భావ "qemu/error-report.h"
#అన్తర్భావ "qapi/error.h"
#అన్తర్భావ "hw/core/boards.h"
#అన్తర్భావ "hw/core/qdev-properties.h"
#అన్తర్భావ "hw/riscv/numa.h"
#అన్తర్భావ "system/device_tree.h"

bool riscv_numa_enabled(స్థిరరాశి MachineState *ms)
{
    తిరిగి (ms->numa_state && ms->numa_state->అంకె_nodes) ? true : false;
}

పూర్ణాంకం riscv_socket_లెక్క(స్థిరరాశి MachineState *ms)
{
    తిరిగి (riscv_numa_enabled(ms)) ? ms->numa_state->అంకె_nodes : 1;
}

పూర్ణాంకం riscv_socket_మొదటి_hartid(స్థిరరాశి MachineState *ms, పూర్ణాంకం socket_id)
{
    పూర్ణాంకం i, మొదటి_hartid = ms->smp.cpus;

    ఒకవేళ (!riscv_numa_enabled(ms)) {
        తిరిగి (!socket_id) ? 0 : -1;
    }

    కొరకు (i = 0; i < ms->smp.cpus; i++) {
        ఒకవేళ (ms->possible_cpus->cpus[i].props.కణుపు_id != socket_id) {
            కొనసాగు;
        }
        ఒకవేళ (i < మొదటి_hartid) {
            మొదటి_hartid = i;
        }
    }

    తిరిగి (మొదటి_hartid < ms->smp.cpus) ? మొదటి_hartid : -1;
}

పూర్ణాంకం riscv_socket_చివరి_hartid(స్థిరరాశి MachineState *ms, పూర్ణాంకం socket_id)
{
    పూర్ణాంకం i, చివరి_hartid = -1;

    ఒకవేళ (!riscv_numa_enabled(ms)) {
        తిరిగి (!socket_id) ? ms->smp.cpus - 1 : -1;
    }

    కొరకు (i = 0; i < ms->smp.cpus; i++) {
        ఒకవేళ (ms->possible_cpus->cpus[i].props.కణుపు_id != socket_id) {
            కొనసాగు;
        }
        ఒకవేళ (i > చివరి_hartid) {
            చివరి_hartid = i;
        }
    }

    తిరిగి (చివరి_hartid < ms->smp.cpus) ? చివరి_hartid : -1;
}

పూర్ణాంకం riscv_socket_hart_లెక్క(స్థిరరాశి MachineState *ms, పూర్ణాంకం socket_id)
{
    పూర్ణాంకం మొదటి_hartid, చివరి_hartid;

    ఒకవేళ (!riscv_numa_enabled(ms)) {
        తిరిగి (!socket_id) ? ms->smp.cpus : -1;
    }

    మొదటి_hartid = riscv_socket_మొదటి_hartid(ms, socket_id);
    ఒకవేళ (మొదటి_hartid < 0) {
        తిరిగి -1;
    }

    చివరి_hartid = riscv_socket_చివరి_hartid(ms, socket_id);
    ఒకవేళ (చివరి_hartid < 0) {
        తిరిగి -1;
    }

    ఒకవేళ (మొదటి_hartid > చివరి_hartid) {
        తిరిగి -1;
    }

    తిరిగి చివరి_hartid - మొదటి_hartid + 1;
}

bool riscv_socket_check_hartids(స్థిరరాశి MachineState *ms, పూర్ణాంకం socket_id)
{
    పూర్ణాంకం i, మొదటి_hartid, చివరి_hartid;

    ఒకవేళ (!riscv_numa_enabled(ms)) {
        తిరిగి (!socket_id) ? true : false;
    }

    మొదటి_hartid = riscv_socket_మొదటి_hartid(ms, socket_id);
    ఒకవేళ (మొదటి_hartid < 0) {
        తిరిగి false;
    }

    చివరి_hartid = riscv_socket_చివరి_hartid(ms, socket_id);
    ఒకవేళ (చివరి_hartid < 0) {
        తిరిగి false;
    }

    కొరకు (i = మొదటి_hartid; i <= చివరి_hartid; i++) {
        ఒకవేళ (ms->possible_cpus->cpus[i].props.కణుపు_id != socket_id) {
            తిరిగి false;
        }
    }

    తిరిగి true;
}

uint64_t riscv_socket_mem_స్థానభ్రంశం(స్థిరరాశి MachineState *ms, పూర్ణాంకం socket_id)
{
    పూర్ణాంకం i;
    uint64_t mem_స్థానభ్రంశం = 0;

    ఒకవేళ (!riscv_numa_enabled(ms)) {
        తిరిగి 0;
    }

    కొరకు (i = 0; i < ms->numa_state->అంకె_nodes; i++) {
        ఒకవేళ (i == socket_id) {
            ఆపు;
        }
        mem_స్థానభ్రంశం += ms->numa_state->nodes[i].కణుపు_mem;
    }

    తిరిగి (i == socket_id) ? mem_స్థానభ్రంశం : 0;
}

uint64_t riscv_socket_mem_కొలత(స్థిరరాశి MachineState *ms, పూర్ణాంకం socket_id)
{
    ఒకవేళ (!riscv_numa_enabled(ms)) {
        తిరిగి (!socket_id) ? ms->ram_కొలత : 0;
    }

    తిరిగి (socket_id < ms->numa_state->అంకె_nodes) ?
            ms->numa_state->nodes[socket_id].కణుపు_mem : 0;
}

శూన్య riscv_socket_fdt_write_id(స్థిరరాశి MachineState *ms, స్థిరరాశి అక్షరం *కణుపు_name,
                               పూర్ణాంకం socket_id)
{
    ఒకవేళ (riscv_numa_enabled(ms)) {
        qemu_fdt_setprop_cell(ms->fdt, కణుపు_name, "numa-node-id", socket_id);
    }
}

శూన్య riscv_socket_fdt_write_దూరం_మాతృక(స్థిరరాశి MachineState *ms)
{
    పూర్ణాంకం i, j, idx;
    g_autofree uint32_t *dist_మాతృక = ఖాళీ;
    uint32_t dist_మాతృక_కొలత;

    ఒకవేళ (riscv_numa_enabled(ms) && ms->numa_state->have_numa_దూరం) {
        dist_మాతృక_కొలత = riscv_socket_లెక్క(ms) * riscv_socket_లెక్క(ms);
        dist_మాతృక_కొలత *= (3 * పరిమాణం(uint32_t));
        dist_మాతృక = g_malloc0(dist_మాతృక_కొలత);

        కొరకు (i = 0; i < riscv_socket_లెక్క(ms); i++) {
            కొరకు (j = 0; j < riscv_socket_లెక్క(ms); j++) {
                idx = (i * riscv_socket_లెక్క(ms) + j) * 3;
                dist_మాతృక[idx + 0] = cpu_గమ్యం_be32(i);
                dist_మాతృక[idx + 1] = cpu_గమ్యం_be32(j);
                dist_మాతృక[idx + 2] =
                    cpu_గమ్యం_be32(ms->numa_state->nodes[i].దూరం[j]);
            }
        }

        qemu_fdt_కూడు_subnode(ms->fdt, "/distance-map");
        qemu_fdt_setprop_string(ms->fdt, "/distance-map", "compatible",
                                "numa-distance-map-v1");
        qemu_fdt_setprop(ms->fdt, "/distance-map", "distance-matrix",
                         dist_మాతృక, dist_మాతృక_కొలత);
    }
}

CpuInstanceProperties
riscv_numa_cpu_సూచీ_గమ్యం_props(MachineState *ms, గుర్తులేని cpu_సూచీ)
{
    MachineClass *mc = MACHINE_GET_CLASS(ms);
    స్థిరరాశి CPUArchIdList *possible_cpus = mc->possible_cpu_arch_ids(ms);

    ప్రతిజ్ఞా(cpu_సూచీ < possible_cpus->len);
    తిరిగి possible_cpus->cpus[cpu_సూచీ].props;
}

int64_t riscv_numa_get_default_cpu_కణుపు_id(స్థిరరాశి MachineState *ms, పూర్ణాంకం idx)
{
    int64_t nidx = 0;

    ఒకవేళ (ms->numa_state->అంకె_nodes > ms->smp.cpus) {
        error_report("Number of NUMA nodes (%d)"
                     " cannot exceed the number of available CPUs (%u).",
                     ms->numa_state->అంకె_nodes, ms->smp.cpus);
        నిష్క్రమించు(EXIT_FAILURE);
    }
    ఒకవేళ (ms->numa_state->అంకె_nodes) {
        nidx = idx / (ms->smp.cpus / ms->numa_state->అంకె_nodes);
        ఒకవేళ (ms->numa_state->అంకె_nodes <= nidx) {
            nidx = ms->numa_state->అంకె_nodes - 1;
        }
    }

    తిరిగి nidx;
}

స్థిరరాశి CPUArchIdList *riscv_numa_possible_cpu_arch_ids(MachineState *ms)
{
    పూర్ణాంకం n;
    గుర్తులేని పూర్ణాంకం max_cpus = ms->smp.max_cpus;

    ఒకవేళ (ms->possible_cpus) {
        ప్రతిజ్ఞా(ms->possible_cpus->len == max_cpus);
        తిరిగి ms->possible_cpus;
    }

    ms->possible_cpus = g_malloc0(పరిమాణం(CPUArchIdList) +
                                  పరిమాణం(CPUArchId) * max_cpus);
    ms->possible_cpus->len = max_cpus;
    కొరకు (n = 0; n < ms->possible_cpus->len; n++) {
        ms->possible_cpus->cpus[n].type = ms->cpu_type;
        ms->possible_cpus->cpus[n].arch_id = n;
        ms->possible_cpus->cpus[n].props.has_socket_id = true;
        ms->possible_cpus->cpus[n].props.socket_id =
            n / (ms->smp.clusters * ms->smp.cores * ms->smp.దారాలు);
        ms->possible_cpus->cpus[n].props.has_cluster_id = true;
        ms->possible_cpus->cpus[n].props.cluster_id =
            (n / (ms->smp.cores * ms->smp.దారాలు)) % ms->smp.clusters;
        ms->possible_cpus->cpus[n].props.has_core_id = true;
        ms->possible_cpus->cpus[n].props.core_id =
            (n / ms->smp.దారాలు) % ms->smp.cores;
        ms->possible_cpus->cpus[n].props.has_thread_id = true;
        ms->possible_cpus->cpus[n].props.thread_id =
            n % ms->smp.దారాలు;
    }

    తిరిగి ms->possible_cpus;
}
