/****************************************************************************
 * arch/x86/src/qemu/qemu_handlers.c
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
#include <nuttx/compiler.h>

#include <assert.h>

#include <nuttx/arch.h>
#include <nuttx/board.h>
#include <arch/io.h>
#include <sched/sched.h>

#include "x86_internal.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static void idt_outb(uint8_t val, uint16_t addr) noinline_function;

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: idt_outb
 *
 * Description:
 *   A slightly slower version of outb
 *
 ****************************************************************************/

static void idt_outb(uint8_t val, uint16_t addr)
{
  outb(val, addr);
}

/****************************************************************************
 * Name: common_handler
 *
 * Description:
 *   Common logic for the ISR/IRQ handlers
 *
 ****************************************************************************/

#ifndef CONFIG_SUPPRESS_INTERRUPTS
static uint32_t *common_handler(int irq, uint32_t *regs)
{
  struct tcb_s **running_task = &g_running_tasks[this_cpu()];
  struct tcb_s *tcb;
  board_autoled_on(LED_INIRQ);

  /* Current regs non-zero indicates that we are processing an interrupt;
   * g_current_regs is also used to manage interrupt level context switches.
   *
   * Nested interrupts are not supported.
   */

  DEBUGASSERT(up_current_regs() == NULL);
  up_set_current_regs(regs);

  x86_savestate((*running_task)->xcp.regs);

  /* Deliver the IRQ */

  irq_dispatch(irq, regs);

  /* Check for a context switch.  If a context switch occurred, then
   * g_current_regs will have a different value than it did on entry.  If an
   * interrupt level context switch has occurred, then restore the floating
   * point state and the establish the correct address environment before
   * returning from the interrupt.
   */

  if (regs != up_current_regs())
    {
#ifdef CONFIG_ARCH_FPU
      /* Restore floating point registers */

      up_restorefpu(up_current_regs());
#endif

      tcb = this_task();
#ifdef CONFIG_ARCH_ADDRENV
      /* Make sure that the address environment for the previously
       * running task is closed down gracefully (data caches dump,
       * MMU flushed) and set up the address environment for the new
       * thread at the head of the ready-to-run list.
       */

      addrenv_switch(tcb);
#endif

      /* Update scheduler parameters */

      nxsched_suspend_scheduler(*running_task);
      nxsched_resume_scheduler(tcb);

      /* Record the new "running" task when context switch occurred.
       * g_running_tasks[] is only used by assertion logic for reporting
       * crashes.
       */

      *running_task = tcb;
    }

  /* If a context switch occurred while processing the interrupt then
   * g_current_regs may have change value.  If we return any value different
   * from the input regs, then the lower level will know that a context
   * switch occurred during interrupt processing.
   */

  regs = up_current_regs();

  /* Set g_current_regs to NULL to indicate that we are no longer in an
   * interrupt handler.
   */

  up_set_current_regs(NULL);
  return regs;
}
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: isr_handler
 *
 * Description:
 *   This gets called from ISR vector handling logic in qemu_vectors.S
 *
 ****************************************************************************/

uint32_t *isr_handler(uint32_t *regs)
{
#ifdef CONFIG_SUPPRESS_INTERRUPTS
  board_autoled_on(LED_INIRQ);
  PANIC();      /* Doesn't return */
  return regs;  /* To keep the compiler happy */
#else
  uint32_t *ret;

  /* Dispatch the interrupt */

  board_autoled_on(LED_INIRQ);
  ret = common_handler((int)regs[REG_IRQNO], regs);
  board_autoled_off(LED_INIRQ);
  return ret;
#endif
}

/****************************************************************************
 * Name: isr_handler
 *
 * Description:
 *   This gets called from IRQ vector handling logic in qemu_vectors.S
 *
 ****************************************************************************/

uint32_t *irq_handler(uint32_t *regs)
{
#ifdef CONFIG_SUPPRESS_INTERRUPTS
  board_autoled_on(LED_INIRQ);
  PANIC();      /* Doesn't return */
  return regs;  /* To keep the compiler happy */
#else
  uint32_t *ret;
  int irq;

  board_autoled_on(LED_INIRQ);

  /* Get the IRQ number */

  irq = (int)regs[REG_IRQNO];

  /* Send an EOI (end of interrupt) signal to the PICs if this interrupt
   * involved the slave.
   */

  if (irq >= IRQ8)
    {
      /* Send reset signal to slave */

      idt_outb(PIC_OCW2_EOI_NONSPEC, PIC2_OCW2);
    }

  /* Send reset signal to master */

  idt_outb(PIC_OCW2_EOI_NONSPEC, PIC1_OCW2);

  /* Dispatch the interrupt */

  ret = common_handler(irq, regs);
  board_autoled_off(LED_INIRQ);
  return ret;
#endif
}
