/****************************************************************************
 * boards/risc-v/esp32c6/common/include/esp_board_mpu60x0.h
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

#ifndef __BOARDS_RISCV_ESP32C6_COMMON_INCLUDE_ESP_BOARD_MPU60X0_H
#define __BOARDS_RISCV_ESP32C6_COMMON_INCLUDE_ESP_BOARD_MPU60X0_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#ifndef __ASSEMBLY__

#undef EXTERN
#if defined(__cplusplus)
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Name: board_mpu60x0_initialize
 *
 * Description:
 *   Initialize and register the MPU60x0 IMU Sensor driver.
 *
 * Input Parameters:
 *   devno - The device number, used to build the device path as /dev/imuN
 *
 * Returned Value:
 *   Zero (OK) on success; a negated errno value on failure.
 *
 ****************************************************************************/

#ifdef CONFIG_SENSORS_MPU60X0
int board_mpu60x0_initialize(int devno);
#endif

#undef EXTERN
#if defined(__cplusplus)
}
#endif

#endif /* __ASSEMBLY__ */
#endif /* __BOARDS_RISCV_ESP32C6_COMMON_INCLUDE_ESP_BOARD_MPU60X0_H */
