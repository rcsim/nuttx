/****************************************************************************
 * boards/arm/stm32/clicker2-stm32/src/stm32_appinit.c
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

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>
#include <syslog.h>

#include <nuttx/board.h>
#include <nuttx/signal.h>
#include <nuttx/syslog/syslog.h>

#include "clicker2-stm32.h"

#ifdef CONFIG_BOARDCTL

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: board_app_initialize
 *
 * Description:
 *   Perform application specific initialization.  This function is never
 *   called directly from application code, but only indirectly via the
 *   (non-standard) boardctl() interface using the command BOARDIOC_INIT.
 *
 *   CONFIG_BOARDCTL=y :
 *     Called from the NSH library
 *
 *   CONFIG_BOARD_LATE_INITIALIZE=y, CONFIG_NSH_LIBRARY=y, &&
 *   CONFIG_BOARDCTL=n :
 *     Called from board_late_initialize().
 *
 * Input Parameters:
 *   arg - The boardctl() argument is passed to the board_app_initialize()
 *         implementation without modification.  The argument has no
 *         meaning to NuttX; the meaning of the argument is a contract
 *         between the board-specific initialization logic and the
 *         matching application logic.  The value could be such things as a
 *         mode enumeration value, a set of DIP switch switch settings, a
 *         pointer to configuration data read from a file or serial FLASH,
 *         or whatever you would like to do with it.  Every implementation
 *         should accept zero/NULL as a default configuration.
 *
 * Returned Value:
 *   Zero (OK) is returned on success; a negated errno value is returned on
 *   any failure to indicate the nature of the failure.
 *
 ****************************************************************************/

int board_app_initialize(uintptr_t arg)
{
  int ret;

  /* Did we already initialize via board_late_initialize()? */

#ifndef CONFIG_BOARD_LATE_INITIALIZE
  ret = stm32_bringup();
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: stm32_bringup() failed: %d\n", ret);
      return ret;
    }
#endif

#ifdef CONFIG_CLICKER2_STM32_SYSLOG_FILE

  /* Delay some time for the automounter to finish mounting before
   * bringing up file syslog.
   */

  nxsig_usleep(CONFIG_CLICKER2_STM32_SYSLOG_FILE_DELAY * 1000);

  syslog_channel_t *channel;
  channel = syslog_file_channel(CONFIG_CLICKER2_STM32_SYSLOG_FILE_PATH);
  if (channel == NULL)
    {
      syslog(LOG_ERR, "ERROR: syslog_file_channel() failed\n");
      return -EINVAL;
    }
#endif

  UNUSED(ret);
  return OK;
}

#endif /* CONFIG_BOARDCTL */
