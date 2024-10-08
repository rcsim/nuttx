/****************************************************************************
 * libs/libc/grp/lib_initgroups.c
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

#include <grp.h>

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: initgroups
 *
 * Description:
 *   The group database /etc/group is read to determine all groups of which
 *   user is a member.  The additional group group is also added to this set,
 *   which is then used to set the supplementary group IDs of the calling
 *   process.
 *
 * Input Parameters:
 *   user  - Name of the user to query the /etc/group database for.
 *   group - Additional gid to add to the list of group IDs.
 *
 * Returned Value:
 *   The initgroups() function returns zero if successful, and -1 in case of
 *   failure, in which case errno is set appropriately.
 *
 ****************************************************************************/

int initgroups(FAR const char *user, gid_t group)
{
  /* There currently is no support for supplementary group IDs in NuttX.
   * Thus, just ignore this request silently and report success.
   */

  UNUSED(user);
  UNUSED(group);
  return 0;
}
