/****************************************************************************
 * arch/xtensa/src/esp32s3/esp32s3_reset_reasons.h
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

/****************************************************************************
 * Included Files
 ****************************************************************************/

#ifndef __ARCH_XTENSA_SRC_ESP32S3_ESP32S3_RESET_REASONS_H
#define __ARCH_XTENSA_SRC_ESP32S3_ESP32S3_RESET_REASONS_H

/****************************************************************************
 * Included Files
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
 * Public Types
 ****************************************************************************/

/**
 *+-------------------------------Terminology-------------------------------+
 *|                                                                         |
 *| CPU Reset:    Reset CPU core only, once reset done, CPU will execute    |
 *|               from reset vector                                         |
 *|                                                                         |
 *| Core Reset:   Reset the whole digital system except RTC sub-system      |
 *|                                                                         |
 *| System Reset: Reset the whole digital system, including RTC sub-system  |
 *|                                                                         |
 *| Chip Reset:   Reset the whole chip, including the analog part           |
 *|                                                                         |
 *+-------------------------------------------------------------------------+
 */

/* Naming conventions: RESET_REASON_{reset level}_{reset reason}
 * Note refer to TRM: <Reset and Clock> chapter
 */

typedef enum
{
  RESET_REASON_CHIP_POWER_ON   = 0x01, /* Power on reset */
  RESET_REASON_CHIP_BROWN_OUT  = 0x01, /* VDD voltage is not stable and resets the chip */
  RESET_REASON_CHIP_SUPER_WDT  = 0x01, /* Super watch dog resets the chip */
  RESET_REASON_CORE_SW         = 0x03, /* Software resets the digital core by RTC_CNTL_SW_SYS_RST */
  RESET_REASON_CORE_DEEP_SLEEP = 0x05, /* Deep sleep reset the digital core */
  RESET_REASON_CORE_MWDT0      = 0x07, /* Main watch dog 0 resets digital core */
  RESET_REASON_CORE_MWDT1      = 0x08, /* Main watch dog 1 resets digital core */
  RESET_REASON_CORE_RTC_WDT    = 0x09, /* RTC watch dog resets digital core */
  RESET_REASON_CPU0_MWDT0      = 0x0b, /* Main watch dog 0 resets CPU 0 */
  RESET_REASON_CPU1_MWDT0      = 0x0b, /* Main watch dog 0 resets CPU 1 */
  RESET_REASON_CPU0_SW         = 0x0c, /* Software resets CPU 0 by RTC_CNTL_SW_PROCPU_RST */
  RESET_REASON_CPU1_SW         = 0x0c, /* Software resets CPU 1 by RTC_CNTL_SW_APPCPU_RST */
  RESET_REASON_CPU0_RTC_WDT    = 0x0d, /* RTC watch dog resets CPU 0 */
  RESET_REASON_CPU1_RTC_WDT    = 0x0d, /* RTC watch dog resets CPU 1 */
  RESET_REASON_SYS_BROWN_OUT   = 0x0f, /* VDD voltage is not stable and resets the digital core */
  RESET_REASON_SYS_RTC_WDT     = 0x10, /* RTC watch dog resets digital core and rtc module */
  RESET_REASON_CPU0_MWDT1      = 0x11, /* Main watch dog 1 resets CPU 0 */
  RESET_REASON_CPU1_MWDT1      = 0x11, /* Main watch dog 1 resets CPU 1 */
  RESET_REASON_SYS_SUPER_WDT   = 0x12, /* Super watch dog resets the digital core and rtc module */
  RESET_REASON_SYS_CLK_GLITCH  = 0x13, /* Glitch on clock resets the digital core and rtc module */
  RESET_REASON_CORE_EFUSE_CRC  = 0x14, /* eFuse CRC error resets the digital core */
  RESET_REASON_CORE_USB_UART   = 0x15, /* USB UART resets the digital core */
  RESET_REASON_CORE_USB_JTAG   = 0x16, /* USB JTAG resets the digital core */
  RESET_REASON_CORE_PWR_GLITCH = 0x17, /* Glitch on power resets the digital core */
} soc_reset_reason_t;

/****************************************************************************
 * Name: esp32s3_reset_reasons
 *
 * Description:
 *   Get the cause of the last reset of the given CPU
 *
 ****************************************************************************/

soc_reset_reason_t esp32s3_reset_reasons(int cpu);

#ifdef __cplusplus
}
#endif
#undef EXTERN

#endif /* __ASSEMBLY__ */
#endif /* __ARCH_XTENSA_SRC_ESP32S3_ESP32S3_RESET_REASONS_H */
