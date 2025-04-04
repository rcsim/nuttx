/****************************************************************************
 * arch/arm/src/rp23xx/hardware/rp23xx_powman.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

#ifndef __ARCH_ARM_SRC_RP23XX_HARDWARE_RP23XX_POWMAN_H
#define __ARCH_ARM_SRC_RP23XX_HARDWARE_RP23XX_POWMAN_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "hardware/rp23xx_memorymap.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Register offsets *********************************************************/

#define RP23XX_POWMAN_BADPASSWD_OFFSET              0x00000000
#define RP23XX_POWMAN_VREG_CTRL_OFFSET              0x00000004
#define RP23XX_POWMAN_VREG_STS_OFFSET               0x00000008
#define RP23XX_POWMAN_VREG_OFFSET                   0x0000000c
#define RP23XX_POWMAN_VREG_LP_ENTRY_OFFSET          0x00000010
#define RP23XX_POWMAN_VREG_LP_EXIT_OFFSET           0x00000014
#define RP23XX_POWMAN_BOD_CTRL_OFFSET               0x00000018
#define RP23XX_POWMAN_BOD_OFFSET                    0x0000001c
#define RP23XX_POWMAN_BOD_LP_ENTRY_OFFSET           0x00000020
#define RP23XX_POWMAN_BOD_LP_EXIT_OFFSET            0x00000024
#define RP23XX_POWMAN_LPOSC_OFFSET                  0x00000028
#define RP23XX_POWMAN_CHIP_RESET_OFFSET             0x0000002c
#define RP23XX_POWMAN_WDSEL_OFFSET                  0x00000030
#define RP23XX_POWMAN_SEQ_CFG_OFFSET                0x00000034
#define RP23XX_POWMAN_STATE_OFFSET                  0x00000038
#define RP23XX_POWMAN_POW_FASTDIV_OFFSET            0x0000003c
#define RP23XX_POWMAN_POW_DELAY_OFFSET              0x00000040
#define RP23XX_POWMAN_EXT_CTRL0_OFFSET              0x00000044
#define RP23XX_POWMAN_EXT_CTRL1_OFFSET              0x00000048
#define RP23XX_POWMAN_EXT_TIME_REF_OFFSET           0x0000004c
#define RP23XX_POWMAN_LPOSC_FREQ_KHZ_INT_OFFSET     0x00000050
#define RP23XX_POWMAN_LPOSC_FREQ_KHZ_FRAC_OFFSET    0x00000054
#define RP23XX_POWMAN_XOSC_FREQ_KHZ_INT_OFFSET      0x00000058
#define RP23XX_POWMAN_XOSC_FREQ_KHZ_FRAC_OFFSET     0x0000005c
#define RP23XX_POWMAN_SET_TIME_63TO48_OFFSET        0x00000060
#define RP23XX_POWMAN_SET_TIME_47TO32_OFFSET        0x00000064
#define RP23XX_POWMAN_SET_TIME_31TO16_OFFSET        0x00000068
#define RP23XX_POWMAN_SET_TIME_15TO0_OFFSET         0x0000006c
#define RP23XX_POWMAN_READ_TIME_UPPER_OFFSET        0x00000070
#define RP23XX_POWMAN_READ_TIME_LOWER_OFFSET        0x00000074
#define RP23XX_POWMAN_ALARM_TIME_63TO48_OFFSET      0x00000078
#define RP23XX_POWMAN_ALARM_TIME_47TO32_OFFSET      0x0000007c
#define RP23XX_POWMAN_ALARM_TIME_31TO16_OFFSET      0x00000080
#define RP23XX_POWMAN_ALARM_TIME_15TO0_OFFSET       0x00000084
#define RP23XX_POWMAN_TIMER_OFFSET                  0x00000088
#define RP23XX_POWMAN_PWRUP0_OFFSET                 0x0000008c
#define RP23XX_POWMAN_PWRUP1_OFFSET                 0x00000090
#define RP23XX_POWMAN_PWRUP2_OFFSET                 0x00000094
#define RP23XX_POWMAN_PWRUP3_OFFSET                 0x00000098
#define RP23XX_POWMAN_CURRENT_PWRUP_REQ_OFFSET      0x0000009c
#define RP23XX_POWMAN_LAST_SWCORE_PWRUP_OFFSET      0x000000a0
#define RP23XX_POWMAN_DBG_PWRCFG_OFFSET             0x000000a4
#define RP23XX_POWMAN_BOOTDIS_OFFSET                0x000000a8
#define RP23XX_POWMAN_DBGCONFIG_OFFSET              0x000000ac
#define RP23XX_POWMAN_SCRATCH0_OFFSET               0x000000b0
#define RP23XX_POWMAN_SCRATCH1_OFFSET               0x000000b4
#define RP23XX_POWMAN_SCRATCH2_OFFSET               0x000000b8
#define RP23XX_POWMAN_SCRATCH3_OFFSET               0x000000bc
#define RP23XX_POWMAN_SCRATCH4_OFFSET               0x000000c0
#define RP23XX_POWMAN_SCRATCH5_OFFSET               0x000000c4
#define RP23XX_POWMAN_SCRATCH6_OFFSET               0x000000c8
#define RP23XX_POWMAN_SCRATCH7_OFFSET               0x000000cc
#define RP23XX_POWMAN_BOOT0_OFFSET                  0x000000d0
#define RP23XX_POWMAN_BOOT1_OFFSET                  0x000000d4
#define RP23XX_POWMAN_BOOT2_OFFSET                  0x000000d8
#define RP23XX_POWMAN_BOOT3_OFFSET                  0x000000dc
#define RP23XX_POWMAN_INTR_OFFSET                   0x000000e0
#define RP23XX_POWMAN_INTE_OFFSET                   0x000000e4
#define RP23XX_POWMAN_INTF_OFFSET                   0x000000e8
#define RP23XX_POWMAN_INTS_OFFSET                   0x000000ec

/* Register definitions *****************************************************/

#define RP23XX_POWMAN_BADPASSWD             (RP23XX_POWMAN_BASE + RP23XX_POWMAN_BADPASSWD_OFFSET)
#define RP23XX_POWMAN_VREG_CTRL             (RP23XX_POWMAN_BASE + RP23XX_POWMAN_VREG_CTRL_OFFSET)
#define RP23XX_POWMAN_VREG_STS              (RP23XX_POWMAN_BASE + RP23XX_POWMAN_VREG_STS_OFFSET)
#define RP23XX_POWMAN_VREG                  (RP23XX_POWMAN_BASE + RP23XX_POWMAN_VREG_OFFSET)
#define RP23XX_POWMAN_VREG_LP_ENTRY         (RP23XX_POWMAN_BASE + RP23XX_POWMAN_VREG_LP_ENTRY_OFFSET)
#define RP23XX_POWMAN_VREG_LP_EXIT          (RP23XX_POWMAN_BASE + RP23XX_POWMAN_VREG_LP_EXIT_OFFSET)
#define RP23XX_POWMAN_BOD_CTRL              (RP23XX_POWMAN_BASE + RP23XX_POWMAN_BOD_CTRL_OFFSET)
#define RP23XX_POWMAN_BOD                   (RP23XX_POWMAN_BASE + RP23XX_POWMAN_BOD_OFFSET)
#define RP23XX_POWMAN_BOD_LP_ENTRY          (RP23XX_POWMAN_BASE + RP23XX_POWMAN_BOD_LP_ENTRY_OFFSET)
#define RP23XX_POWMAN_BOD_LP_EXIT           (RP23XX_POWMAN_BASE + RP23XX_POWMAN_BOD_LP_EXIT_OFFSET)
#define RP23XX_POWMAN_LPOSC                 (RP23XX_POWMAN_BASE + RP23XX_POWMAN_LPOSC_OFFSET)
#define RP23XX_POWMAN_CHIP_RESET            (RP23XX_POWMAN_BASE + RP23XX_POWMAN_CHIP_RESET_OFFSET)
#define RP23XX_POWMAN_WDSEL                 (RP23XX_POWMAN_BASE + RP23XX_POWMAN_WDSEL_OFFSET)
#define RP23XX_POWMAN_SEQ_CFG               (RP23XX_POWMAN_BASE + RP23XX_POWMAN_SEQ_CFG_OFFSET)
#define RP23XX_POWMAN_STATE                 (RP23XX_POWMAN_BASE + RP23XX_POWMAN_STATE_OFFSET)
#define RP23XX_POWMAN_POW_FASTDIV           (RP23XX_POWMAN_BASE + RP23XX_POWMAN_POW_FASTDIV_OFFSET)
#define RP23XX_POWMAN_POW_DELAY             (RP23XX_POWMAN_BASE + RP23XX_POWMAN_POW_DELAY_OFFSET)
#define RP23XX_POWMAN_EXT_CTRL0             (RP23XX_POWMAN_BASE + RP23XX_POWMAN_EXT_CTRL0_OFFSET)
#define RP23XX_POWMAN_EXT_CTRL1             (RP23XX_POWMAN_BASE + RP23XX_POWMAN_EXT_CTRL1_OFFSET)
#define RP23XX_POWMAN_EXT_TIME_REF          (RP23XX_POWMAN_BASE + RP23XX_POWMAN_EXT_TIME_REF_OFFSET)
#define RP23XX_POWMAN_LPOSC_FREQ_KHZ_INT    (RP23XX_POWMAN_BASE + RP23XX_POWMAN_LPOSC_FREQ_KHZ_INT_OFFSET)
#define RP23XX_POWMAN_LPOSC_FREQ_KHZ_FRAC   (RP23XX_POWMAN_BASE + RP23XX_POWMAN_LPOSC_FREQ_KHZ_FRAC_OFFSET)
#define RP23XX_POWMAN_XOSC_FREQ_KHZ_INT     (RP23XX_POWMAN_BASE + RP23XX_POWMAN_XOSC_FREQ_KHZ_INT_OFFSET)
#define RP23XX_POWMAN_XOSC_FREQ_KHZ_FRAC    (RP23XX_POWMAN_BASE + RP23XX_POWMAN_XOSC_FREQ_KHZ_FRAC_OFFSET)
#define RP23XX_POWMAN_SET_TIME_63TO48       (RP23XX_POWMAN_BASE + RP23XX_POWMAN_SET_TIME_63TO48_OFFSET)
#define RP23XX_POWMAN_SET_TIME_47TO32       (RP23XX_POWMAN_BASE + RP23XX_POWMAN_SET_TIME_47TO32_OFFSET)
#define RP23XX_POWMAN_SET_TIME_31TO16       (RP23XX_POWMAN_BASE + RP23XX_POWMAN_SET_TIME_31TO16_OFFSET)
#define RP23XX_POWMAN_SET_TIME_15TO0        (RP23XX_POWMAN_BASE + RP23XX_POWMAN_SET_TIME_15TO0_OFFSET)
#define RP23XX_POWMAN_READ_TIME_UPPER       (RP23XX_POWMAN_BASE + RP23XX_POWMAN_READ_TIME_UPPER_OFFSET)
#define RP23XX_POWMAN_READ_TIME_LOWER       (RP23XX_POWMAN_BASE + RP23XX_POWMAN_READ_TIME_LOWER_OFFSET)
#define RP23XX_POWMAN_ALARM_TIME_63TO48     (RP23XX_POWMAN_BASE + RP23XX_POWMAN_ALARM_TIME_63TO48_OFFSET)
#define RP23XX_POWMAN_ALARM_TIME_47TO32     (RP23XX_POWMAN_BASE + RP23XX_POWMAN_ALARM_TIME_47TO32_OFFSET)
#define RP23XX_POWMAN_ALARM_TIME_31TO16     (RP23XX_POWMAN_BASE + RP23XX_POWMAN_ALARM_TIME_31TO16_OFFSET)
#define RP23XX_POWMAN_ALARM_TIME_15TO0      (RP23XX_POWMAN_BASE + RP23XX_POWMAN_ALARM_TIME_15TO0_OFFSET)
#define RP23XX_POWMAN_TIMER                 (RP23XX_POWMAN_BASE + RP23XX_POWMAN_TIMER_OFFSET)
#define RP23XX_POWMAN_PWRUP0                (RP23XX_POWMAN_BASE + RP23XX_POWMAN_PWRUP0_OFFSET)
#define RP23XX_POWMAN_PWRUP1                (RP23XX_POWMAN_BASE + RP23XX_POWMAN_PWRUP1_OFFSET)
#define RP23XX_POWMAN_PWRUP2                (RP23XX_POWMAN_BASE + RP23XX_POWMAN_PWRUP2_OFFSET)
#define RP23XX_POWMAN_PWRUP3                (RP23XX_POWMAN_BASE + RP23XX_POWMAN_PWRUP3_OFFSET)
#define RP23XX_POWMAN_CURRENT_PWRUP_REQ     (RP23XX_POWMAN_BASE + RP23XX_POWMAN_CURRENT_PWRUP_REQ_OFFSET)
#define RP23XX_POWMAN_LAST_SWCORE_PWRUP     (RP23XX_POWMAN_BASE + RP23XX_POWMAN_LAST_SWCORE_PWRUP_OFFSET)
#define RP23XX_POWMAN_DBG_PWRCFG            (RP23XX_POWMAN_BASE + RP23XX_POWMAN_DBG_PWRCFG_OFFSET)
#define RP23XX_POWMAN_BOOTDIS               (RP23XX_POWMAN_BASE + RP23XX_POWMAN_BOOTDIS_OFFSET)
#define RP23XX_POWMAN_DBGCONFIG             (RP23XX_POWMAN_BASE + RP23XX_POWMAN_DBGCONFIG_OFFSET)
#define RP23XX_POWMAN_SCRATCH0              (RP23XX_POWMAN_BASE + RP23XX_POWMAN_SCRATCH0_OFFSET)
#define RP23XX_POWMAN_SCRATCH1              (RP23XX_POWMAN_BASE + RP23XX_POWMAN_SCRATCH1_OFFSET)
#define RP23XX_POWMAN_SCRATCH2              (RP23XX_POWMAN_BASE + RP23XX_POWMAN_SCRATCH2_OFFSET)
#define RP23XX_POWMAN_SCRATCH3              (RP23XX_POWMAN_BASE + RP23XX_POWMAN_SCRATCH3_OFFSET)
#define RP23XX_POWMAN_SCRATCH4              (RP23XX_POWMAN_BASE + RP23XX_POWMAN_SCRATCH4_OFFSET)
#define RP23XX_POWMAN_SCRATCH5              (RP23XX_POWMAN_BASE + RP23XX_POWMAN_SCRATCH5_OFFSET)
#define RP23XX_POWMAN_SCRATCH6              (RP23XX_POWMAN_BASE + RP23XX_POWMAN_SCRATCH6_OFFSET)
#define RP23XX_POWMAN_SCRATCH7              (RP23XX_POWMAN_BASE + RP23XX_POWMAN_SCRATCH7_OFFSET)
#define RP23XX_POWMAN_BOOT0                 (RP23XX_POWMAN_BASE + RP23XX_POWMAN_BOOT0_OFFSET)
#define RP23XX_POWMAN_BOOT1                 (RP23XX_POWMAN_BASE + RP23XX_POWMAN_BOOT1_OFFSET)
#define RP23XX_POWMAN_BOOT2                 (RP23XX_POWMAN_BASE + RP23XX_POWMAN_BOOT2_OFFSET)
#define RP23XX_POWMAN_BOOT3                 (RP23XX_POWMAN_BASE + RP23XX_POWMAN_BOOT3_OFFSET)
#define RP23XX_POWMAN_INTR                  (RP23XX_POWMAN_BASE + RP23XX_POWMAN_INTR_OFFSET)
#define RP23XX_POWMAN_INTE                  (RP23XX_POWMAN_BASE + RP23XX_POWMAN_INTE_OFFSET)
#define RP23XX_POWMAN_INTF                  (RP23XX_POWMAN_BASE + RP23XX_POWMAN_INTF_OFFSET)
#define RP23XX_POWMAN_INTS                  (RP23XX_POWMAN_BASE + RP23XX_POWMAN_INTS_OFFSET)

/* Register bit definitions *************************************************/

#define RP23XX_POWMAN_BADPASSWD                         (1 << 0)

#define RP23XX_POWMAN_VREG_CTRL_RST_N                   (1 << 15)
#define RP23XX_POWMAN_VREG_CTRL_UNLOCK                  (1 << 13)
#define RP23XX_POWMAN_VREG_CTRL_ISOLATE                 (1 << 12)
#define RP23XX_POWMAN_VREG_CTRL_DISABLE_VOLTAGE_LIMIT   (1 << 8)
#define RP23XX_POWMAN_VREG_CTRL_HT_TH_MASK              (0x00000070)

#define RP23XX_POWMAN_VREG_STS_VOUT_OK                  (1 << 4)
#define RP23XX_POWMAN_VREG_STS_STARTUP                  (1 << 0)

#define RP23XX_POWMAN_VREG_UPDATE_IN_PROGRESS           (1 << 15)
#define RP23XX_POWMAN_VREG_VSEL_MASK                    0x000001f0
#define RP23XX_POWMAN_VREG_HIZ                          (1 << 1)

#define RP23XX_POWMAN_VREG_LP_ENTRY_VSEL_MASK           0x000001f0
#define RP23XX_POWMAN_VREG_LP_ENTRY_MODE                (1 << 2)
#define RP23XX_POWMAN_VREG_LP_ENTRY_HIZ                 (1 << 1)

#define RP23XX_POWMAN_VREG_LP_EXIT_VSEL_MASK            0x000001f0
#define RP23XX_POWMAN_VREG_LP_EXIT_MODE                 (1 << 2)
#define RP23XX_POWMAN_VREG_LP_EXIT_HIZ                  (1 << 1)
#define RP23XX_POWMAN_BOD_CTRL                          (1 << 12)
#define RP23XX_POWMAN_BOD_CTRL_ISOLATE                  (1 << 12)

#define RP23XX_POWMAN_BOD_VSEL_MASK                     0x000001f0
#define RP23XX_POWMAN_BOD_EN                            (1 << 0)

#define RP23XX_POWMAN_BOD_LP_ENTRY_VSEL_MASK            0x000001f0
#define RP23XX_POWMAN_BOD_LP_ENTRY_EN                   (1 << 0)

#define RP23XX_POWMAN_BOD_LP_EXIT_VSEL_MASK             0x000001f0
#define RP23XX_POWMAN_BOD_LP_EXIT_EN                    (1 << 0)

#define RP23XX_POWMAN_LPOSC_TRIM_MASK                   0x000003f0
#define RP23XX_POWMAN_LPOSC_MODE_MASK                   0x00000003

#define RP23XX_POWMAN_CHIP_RESET_HAD_WATCHDOG_RESET_RSM             (1 << 28)
#define RP23XX_POWMAN_CHIP_RESET_HAD_HZD_SYS_RESET_REQ              (1 << 27)
#define RP23XX_POWMAN_CHIP_RESET_HAD_GLITCH_DETECT                  (1 << 26)
#define RP23XX_POWMAN_CHIP_RESET_HAD_SWCORE_PD                      (1 << 25)
#define RP23XX_POWMAN_CHIP_RESET_HAD_WATCHDOG_RESET_SWCORE          (1 << 24)
#define RP23XX_POWMAN_CHIP_RESET_HAD_WATCHDOG_RESET_POWMAN          (1 << 23)
#define RP23XX_POWMAN_CHIP_RESET_HAD_WATCHDOG_RESET_POWMAN_ASYNC    (1 << 22)
#define RP23XX_POWMAN_CHIP_RESET_HAD_RESCUE                         (1 << 21)
#define RP23XX_POWMAN_CHIP_RESET_HAD_DP_RESET_REQ                   (1 << 19)
#define RP23XX_POWMAN_CHIP_RESET_HAD_RUN_LOW                        (1 << 18)
#define RP23XX_POWMAN_CHIP_RESET_HAD_BOR                            (1 << 17)
#define RP23XX_POWMAN_CHIP_RESET_HAD_POR                            (1 << 16)
#define RP23XX_POWMAN_CHIP_RESET_RESCUE_FLAG                        (1 << 4)
#define RP23XX_POWMAN_CHIP_RESET_DOUBLE_TAP                         (1 << 0)

#define RP23XX_POWMAN_WDSEL_RESET_RSM                   (1 << 12)
#define RP23XX_POWMAN_WDSEL_RESET_SWCORE                (1 << 8)
#define RP23XX_POWMAN_WDSEL_RESET_POWMAN                (1 << 4)
#define RP23XX_POWMAN_WDSEL_RESET_POWMAN_ASYNC          (1 << 0)

#define RP23XX_POWMAN_SEQ_CFG_USING_FAST_POWCK          (1 << 20)
#define RP23XX_POWMAN_SEQ_CFG_USING_BOD_LP              (1 << 17)
#define RP23XX_POWMAN_SEQ_CFG_USING_VREG_LP             (1 << 16)
#define RP23XX_POWMAN_SEQ_CFG_USE_FAST_POWCK            (1 << 12)
#define RP23XX_POWMAN_SEQ_CFG_RUN_LPOSC_IN_LP           (1 << 8)
#define RP23XX_POWMAN_SEQ_CFG_USE_BOD_HP                (1 << 7)
#define RP23XX_POWMAN_SEQ_CFG_USE_BOD_LP                (1 << 6)
#define RP23XX_POWMAN_SEQ_CFG_USE_VREG_HP               (1 << 5)
#define RP23XX_POWMAN_SEQ_CFG_USE_VREG_LP               (1 << 4)
#define RP23XX_POWMAN_SEQ_CFG_HW_PWRUP_SRAM0            (1 << 1)
#define RP23XX_POWMAN_SEQ_CFG_HW_PWRUP_SRAM1            (1 << 0)

#define RP23XX_POWMAN_STATE_CHANGING                    (1 << 13)
#define RP23XX_POWMAN_STATE_WAITING                     (1 << 12)
#define RP23XX_POWMAN_STATE_BAD_HW_REQ                  (1 << 11)
#define RP23XX_POWMAN_STATE_BAD_SW_REQ                  (1 << 10)
#define RP23XX_POWMAN_STATE_PWRUP_WHILE_WAITING         (1 << 9)
#define RP23XX_POWMAN_STATE_REQ_IGNORED                 (1 << 8)
#define RP23XX_POWMAN_STATE_REQ_MASK                    0x000000f0
#define RP23XX_POWMAN_STATE_CURRENT_MASK                0x0000000f
#define RP23XX_POWMAN_POW_FASTDIV_MASK                  0x000007ff

#define RP23XX_POWMAN_POW_DELAY_SRAM_STEP_MASK          0x0000ff00
#define RP23XX_POWMAN_POW_DELAY_XIP_STEP_MASK           0x000000f0
#define RP23XX_POWMAN_POW_DELAY_SWCORE_STEP_MASK        0x0000000f

#define RP23XX_POWMAN_EXT_CTRL0_LP_EXIT_STATE           (1 << 14)
#define RP23XX_POWMAN_EXT_CTRL0_LP_ENTRY_STATE          (1 << 13)
#define RP23XX_POWMAN_EXT_CTRL0_INIT_STATE              (1 << 12)
#define RP23XX_POWMAN_EXT_CTRL0_INIT                    (1 << 8)
#define RP23XX_POWMAN_EXT_CTRL0_GPIO_SELECT_MASK        0x0000003f

#define RP23XX_POWMAN_EXT_CTRL1_LP_EXIT_STATE           (1 << 14)
#define RP23XX_POWMAN_EXT_CTRL1_LP_ENTRY_STATE          (1 << 13)
#define RP23XX_POWMAN_EXT_CTRL1_INIT_STATE              (1 << 12)
#define RP23XX_POWMAN_EXT_CTRL1_INIT                    (1 << 8)
#define RP23XX_POWMAN_EXT_CTRL1_GPIO_SELECT_MASK        0x0000003f
#define RP23XX_POWMAN_EXT_TIME_REF_MASK                 0x00000013
#define RP23XX_POWMAN_EXT_TIME_REF_DRIVE_LPCK           (1 << 4)
#define RP23XX_POWMAN_EXT_TIME_REF_SOURCE_SEL_MASK      0x00000003
#define RP23XX_POWMAN_LPOSC_FREQ_KHZ_INT_MASK           0x0000003f
#define RP23XX_POWMAN_LPOSC_FREQ_KHZ_FRAC_MASK          0x0000ffff
#define RP23XX_POWMAN_XOSC_FREQ_KHZ_INT_MASK            0x0000ffff
#define RP23XX_POWMAN_XOSC_FREQ_KHZ_FRAC_MASK           0x0000ffff
#define RP23XX_POWMAN_SET_TIME_63TO48_MASK              0x0000ffff
#define RP23XX_POWMAN_SET_TIME_47TO32_MASK              0x0000ffff
#define RP23XX_POWMAN_SET_TIME_31TO16_MASK              0x0000ffff
#define RP23XX_POWMAN_SET_TIME_15TO0_MASK               0x0000ffff
#define RP23XX_POWMAN_READ_TIME_UPPER_MASK              0xffffffff
#define RP23XX_POWMAN_READ_TIME_LOWER_MASK              0xffffffff
#define RP23XX_POWMAN_ALARM_TIME_63TO48_MASK            0x0000ffff
#define RP23XX_POWMAN_ALARM_TIME_47TO32_MASK            0x0000ffff
#define RP23XX_POWMAN_ALARM_TIME_31TO16_MASK            0x0000ffff
#define RP23XX_POWMAN_ALARM_TIME_15TO0_MASK             0x0000ffff

#define RP23XX_POWMAN_TIMER_USING_GPIO_1HZ              (1 << 19)
#define RP23XX_POWMAN_TIMER_USING_GPIO_1KHZ             (1 << 18)
#define RP23XX_POWMAN_TIMER_USING_LPOSC                 (1 << 17)
#define RP23XX_POWMAN_TIMER_USING_XOSC                  (1 << 16)
#define RP23XX_POWMAN_TIMER_USE_GPIO_1HZ                (1 << 13)
#define RP23XX_POWMAN_TIMER_USE_GPIO_1KHZ               (1 << 10)
#define RP23XX_POWMAN_TIMER_USE_XOSC                    (1 << 9)
#define RP23XX_POWMAN_TIMER_USE_LPOSC                   (1 << 8)
#define RP23XX_POWMAN_TIMER_ALARM                       (1 << 6)
#define RP23XX_POWMAN_TIMER_PWRUP_ON_ALARM              (1 << 5)
#define RP23XX_POWMAN_TIMER_ALARM_ENAB                  (1 << 4)
#define RP23XX_POWMAN_TIMER_CLEAR                       (1 << 2)
#define RP23XX_POWMAN_TIMER_RUN                         (1 << 1)
#define RP23XX_POWMAN_TIMER_NONSEC_WRITE                (1 << 0)

#define RP23XX_POWMAN_PWRUP0_RAW_STATUS                 (1 << 10)
#define RP23XX_POWMAN_PWRUP0_STATUS                     (1 << 9)
#define RP23XX_POWMAN_PWRUP0_MODE                       (1 << 8)
#define RP23XX_POWMAN_PWRUP0_DIRECTION                  (1 << 7)
#define RP23XX_POWMAN_PWRUP0_ENABLE                     (1 << 6)
#define RP23XX_POWMAN_PWRUP0_SOURCE_MASK                0x0000003f

#define RP23XX_POWMAN_PWRUP1_RAW_STATUS                 (1 << 10)
#define RP23XX_POWMAN_PWRUP1_STATUS                     (1 << 9)
#define RP23XX_POWMAN_PWRUP1_MODE                       (1 << 8)
#define RP23XX_POWMAN_PWRUP1_DIRECTION                  (1 << 7)
#define RP23XX_POWMAN_PWRUP1_ENABLE                     (1 << 6)
#define RP23XX_POWMAN_PWRUP1_SOURCE_MASK                0x0000003f

#define RP23XX_POWMAN_PWRUP2_RAW_STATUS                 (1 << 10)
#define RP23XX_POWMAN_PWRUP2_STATUS                     (1 << 9)
#define RP23XX_POWMAN_PWRUP2_MODE                       (1 << 8)
#define RP23XX_POWMAN_PWRUP2_DIRECTION                  (1 << 7)
#define RP23XX_POWMAN_PWRUP2_ENABLE                     (1 << 6)
#define RP23XX_POWMAN_PWRUP2_SOURCE_MASK                0x0000003f

#define RP23XX_POWMAN_PWRUP3_RAW_STATUS                 (1 << 10)
#define RP23XX_POWMAN_PWRUP3_STATUS                     (1 << 9)
#define RP23XX_POWMAN_PWRUP3_MODE                       (1 << 8)
#define RP23XX_POWMAN_PWRUP3_DIRECTION                  (1 << 7)
#define RP23XX_POWMAN_PWRUP3_ENABLE                     (1 << 6)
#define RP23XX_POWMAN_PWRUP3_SOURCE_MASK                0x0000003f
#define RP23XX_POWMAN_CURRENT_PWRUP_REQ_MASK            0x0000007f
#define RP23XX_POWMAN_LAST_SWCORE_PWRUP_MASK            0x0000007f
#define RP23XX_POWMAN_DBG_PWRCFG                        (1 << 0)
#define RP23XX_POWMAN_DBG_PWRCFG_IGNORE                 (1 << 0)
#define RP23XX_POWMAN_BOOTDIS_MASK                      0x00000003
#define RP23XX_POWMAN_BOOTDIS_NEXT                      (1 << 1)
#define RP23XX_POWMAN_BOOTDIS_NOW                       (1 << 0)
#define RP23XX_POWMAN_DBGCONFIG_MASK                    0x0000000f
#define RP23XX_POWMAN_DBGCONFIG_DP_INSTID_MASK          0x0000000f

#define RP23XX_POWMAN_INTR_PWRUP_WHILE_WAITING          (1 << 3)
#define RP23XX_POWMAN_INTR_STATE_REQ_IGNORED            (1 << 2)
#define RP23XX_POWMAN_INTR_TIMER                        (1 << 1)
#define RP23XX_POWMAN_INTR_VREG_OUTPUT_LOW              (1 << 0)

#define RP23XX_POWMAN_INTE_PWRUP_WHILE_WAITING          (1 << 3)
#define RP23XX_POWMAN_INTE_STATE_REQ_IGNORED            (1 << 2)
#define RP23XX_POWMAN_INTE_TIMER                        (1 << 1)
#define RP23XX_POWMAN_INTE_VREG_OUTPUT_LOW              (1 << 0)

#define RP23XX_POWMAN_INTF_PWRUP_WHILE_WAITING          (1 << 3)
#define RP23XX_POWMAN_INTF_STATE_REQ_IGNORED            (1 << 2)
#define RP23XX_POWMAN_INTF_TIMER                        (1 << 1)
#define RP23XX_POWMAN_INTF_VREG_OUTPUT_LOW              (1 << 0)

#define RP23XX_POWMAN_INTS_PWRUP_WHILE_WAITING          (1 << 3)
#define RP23XX_POWMAN_INTS_STATE_REQ_IGNORED            (1 << 2)
#define RP23XX_POWMAN_INTS_TIMER                        (1 << 1)
#define RP23XX_POWMAN_INTS_VREG_OUTPUT_LOW              (1 << 0)

#endif /* __ARCH_ARM_SRC_RP23XX_HARDWARE_RP23XX_POWMAN_H */
