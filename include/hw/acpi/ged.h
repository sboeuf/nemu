/*
 *
 * Copyright (c) 2018 Intel Corporation
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

#ifndef HW_ACPI_GED_H
#define HW_ACPI_GED_H

#include "qemu/osdep.h"
#include "exec/memory.h"
#include "hw/acpi/aml-build.h"
#include "hw/acpi/cpu.h"
#include "hw/acpi/memory_hotplug.h"

#define ACPI_GED_EVENT_IO_BASE 0xb000

#define ACPI_GED_IRQ_SEL_OFFSET   0x0
#define ACPI_GED_IRQ_SEL_LEN      0x4
#define ACPI_GED_IRQ_SEL_INIT     0x0
#define ACPI_GED_IRQ_SEL_CPU      0x1
#define ACPI_GED_IRQ_SEL_MEM      0x2
#define ACPI_GED_IRQ_SEL_NVDIMM   0x4
#define ACPI_GED_IRQ_SEL_PCI      0x8
#define ACPI_GED_IRQ_REG_LEN      ACPI_GED_IRQ_SEL_LEN
#define ACPI_GED_MSI_ADDR_OFFSET  0x4
#define ACPI_GED_MSI_DATA_OFFSET  0x14
#define ACPI_GED_MSI_ADDR_REG_LEN 0x10
#define ACPI_GED_MSI_DATA_REG_LEN 0x8
#define ACPI_GED_MSI_DWORD_LEN    0x4
#define ACPI_GED_MSI_QWORD_LEN    0x8
#define ACPI_GED_IO_LEN           (ACPI_GED_IRQ_REG_LEN + \
                                   ACPI_GED_MSI_ADDR_REG_LEN + \
                                   ACPI_GED_MSI_DATA_REG_LEN)

#define ACPI_OPREG_MNAD_OFFSET 0x4
#define ACPI_OPREG_MXAD_OFFSET 0xC
#define ACPI_OPREG_MNDT_OFFSET 0x14
#define ACPI_OPREG_MXDT_OFFSET 0x18

#define GED_DEVICE           "GED"
#define AML_GED_IRQ_REG      "IREG"
#define AML_GED_IRQ_SEL      "ISEL"
#define AML_GED_MSI_ADDR_REG "MADR"
#define AML_GED_MSI_MIN_ADDR "MNAD"
#define AML_GED_MSI_MAX_ADDR "MXAD"
#define AML_GED_MSI_DATA_REG "MDAT"
#define AML_GED_MSI_MIN_DATA "MNDT"
#define AML_GED_MSI_MAX_DATA "MXDT"
#define AML_BUF_MSI_RESOURCE "BMSI"
#define AML_BUF_MSI__MNA_FLD "_MNA"
#define AML_BUF_MSI__MXA_FLD "_MXA"
#define AML_BUF_MSI__MND_FLD "_MND"
#define AML_BUF_MSI__MXD_FLD "_MXD"

#define MSI_DEF_ADDR_MIN 0x0
#define MSI_DEF_ADDR_MAX 0xfffffffffffffffe
#define MSI_DEF_DATA_MIN 0x0
#define MSI_DEF_DATA_MAX 0xfffffffe

typedef struct Aml Aml;

typedef enum {
    GED_CPU_HOTPLUG    = 1,
    GED_MEMORY_HOTPLUG = 2,
    GED_NVDIMM_HOTPLUG = 3,
    GED_PCI_HOTPLUG    = 4,
} GedEventType;

typedef struct GedEvent {
    uint32_t     selector;
    GedEventType event;
} GedEvent;

typedef struct GEDState {
    MemoryRegion io;
    uint32_t     sel;
    uint32_t     irq;
    uint64_t     msi_min_addr;
    uint64_t     msi_max_addr;
    uint32_t     msi_min_data;
    uint32_t     msi_max_data;
    QemuMutex    lock;
} GEDState;

void acpi_ged_init(MemoryRegion *as, Object *owner, GEDState *ged_st,
                   hwaddr base_addr, uint32_t ged_irq);
void acpi_ged_set_event(GEDState *ged_st, uint32_t ged_irq_sel);
void build_ged_aml(Aml *table, const char* name, uint32_t ged_irq,
                   GedEvent *events, uint32_t events_size);

#endif
