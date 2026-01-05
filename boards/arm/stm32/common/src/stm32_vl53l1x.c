/****************************************************************************
 * boards/arm/stm32/common/src/stm32_vl53l1x.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to you under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdio.h>
#include <errno.h>
#include <debug.h>

#include <nuttx/i2c/i2c_master.h>
#include <nuttx/sensors/vl53l1x.h>

#include "stm32.h"
#include "stm32_i2c.h"

#ifdef CONFIG_SENSORS_VL53L1X

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: board_vl53l1x_initialize
 *
 * Description:
 *   Initialize and register the VL53L1X Time-of-Flight distance sensor.
 *
 * Input Parameters:
 *   devno - The device number, used to build the device path as
 *           /dev/tofN
 *   busno - The I2C bus number
 *
 * Returned Value:
 *   Zero (OK) on success; a negated errno value on failure.
 *
 ****************************************************************************/

int board_vl53l1x_initialize(int devno, int busno)
{
  struct i2c_master_s *i2c;
  char devpath[10];
  int ret;

  sninfo("Initializing VL53L1X!\n");

  /* Initialize I2C */

  i2c = stm32_i2cbus_initialize(busno);
  if (!i2c)
    {
      snerr("ERROR: Failed to initialize I2C%d\n", busno);
      return -ENODEV;
    }

  /* Register the ToF sensor */

  snprintf(devpath, sizeof(devpath), "/dev/tof%d", devno);
  ret = vl53l1x_register(devpath, i2c);
  if (ret < 0)
    {
      snerr("ERROR: Failed to register VL53L1X: %d\n", ret);
      stm32_i2cbus_uninitialize(i2c);
      return ret;
    }

  sninfo("VL53L1X registered at %s\n", devpath);
  return OK;
}

#endif /* CONFIG_SENSORS_VL53L1X */
