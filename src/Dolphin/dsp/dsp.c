#include "Dolphin/dsp.h"

#include "Dolphin/hw_regs.h"
#include "Dolphin/os.h"

/**
 * @TODO: Documentation
 */
u32 DSPCheckMailToDSP()
{
	return __DSPRegs[DSP_MAILBOX_IN_HI] >> 0xF & 1;
}

/**
 * @TODO: Documentation
 */
u32 DSPCheckMailFromDSP(void)
{
	return __DSPRegs[DSP_MAILBOX_OUT_HI] >> 0xF & 1;
}

/**
 * @TODO: Documentation
 * @note UNUSED Size: 000014
 */
void DSPReadCPUToDSPMbox(void)
{
	TRAP_UNIMPLEMENTED;
}

/**
 * @TODO: Documentation
 */
u32 DSPReadMailFromDSP()
{
	return (__DSPRegs[DSP_MAILBOX_OUT_HI] << 0x10) | __DSPRegs[DSP_MAILBOX_OUT_LO];
}

/**
 * @TODO: Documentation
 */
void DSPSendMailToDSP(u32 mail)
{
	__DSPRegs[DSP_MAILBOX_IN_HI] = mail >> 0x10;
	__DSPRegs[DSP_MAILBOX_IN_LO] = mail;
}

/**
 * @TODO: Documentation
 * @note UNUSED Size: 000040
 */
void DSPAssertInt(void)
{
	u32 tmp;
	BOOL interrupts               = OSDisableInterrupts();
	tmp                           = __DSPRegs[DSP_CONTROL_STATUS];
	__DSPRegs[DSP_CONTROL_STATUS] = (tmp & ~0xA8) | 2;
	OSRestoreInterrupts(interrupts);
}

static int __DSP_init_flag;

void DSPInit(void)
{
	BOOL old;
	u16 tmp;

	__DSP_debug_printf("DSPInit(): Build Date: %s %s\n", "Sep  8 2001", "01:51:48");

	if (__DSP_init_flag == 1)
		return;

	old = OSDisableInterrupts();
	__OSSetInterruptHandler(__OS_INTERRUPT_DSP_DSP, __DSPHandler);
	__OSUnmaskInterrupts(OS_INTERRUPTMASK_DSP_DSP);

	tmp                           = __DSPRegs[DSP_CONTROL_STATUS];
	__DSPRegs[DSP_CONTROL_STATUS] = (tmp & ~0xA8) | 0x800;

	tmp                           = __DSPRegs[DSP_CONTROL_STATUS];
	__DSPRegs[DSP_CONTROL_STATUS] = tmp & ~0xAC;

	__DSP_tmp_task   = NULL;
	__DSP_curr_task  = NULL;
	__DSP_last_task  = NULL;
	__DSP_first_task = NULL;
	__DSP_init_flag  = 1;

	OSRestoreInterrupts(old);
}

BOOL DSPCheckInit(void)
{
    return __DSP_init_flag;
}

void DSPReset(void)
{
    BOOL old;
    u16 tmp;

    old = OSDisableInterrupts();
    tmp = __DSPRegs[5];
    tmp = (tmp & ~0xA8) | 0x800 | 1;
    __DSPRegs[5] = tmp;
    __DSP_init_flag = 0;
    OSRestoreInterrupts(old);
}

void DSPHalt(void)
{
    BOOL old;
    u16 tmp;

    old = OSDisableInterrupts();
    tmp = __DSPRegs[5];
    tmp = (tmp & ~0xA8) | 4;
    __DSPRegs[5] = tmp;
    OSRestoreInterrupts(old);
}

void DSPUnhalt(void)
{
    BOOL old;
    u16 tmp;

    old = OSDisableInterrupts();
    tmp = __DSPRegs[5];
    tmp = (tmp & ~0xAC);
    __DSPRegs[5] = tmp;
    OSRestoreInterrupts(old);
}

u32 DSPGetDMAStatus(void)
{
    return (__DSPRegs[5] & (1 << 9));
}

/**
 * @TODO: Documentation
 * @note UNUSED Size: 000070
 */
DSPTaskInfo* DSPAddTask(DSPTaskInfo* task)
{
    BOOL old;

    // ASSERTMSGLINE(0x21E, __DSP_init_flag == 1, "DSPAddTask(): DSP driver not initialized!\n");

    old = OSDisableInterrupts();

    __DSP_insert_task(task);
    task->state = 0;
    task->flags = 1;

    OSRestoreInterrupts(old);
    if (task == __DSP_first_task)
        __DSP_boot_task(task);
    return task;
}

DSPTaskInfo *DSPCancelTask(DSPTaskInfo *task)
{
    BOOL old;

    // ASSERTMSGLINE(0x242, __DSP_init_flag == 1, "DSPCancelTask(): DSP driver not initialized!\n");

    old = OSDisableInterrupts();

    task->flags |= 2;

    OSRestoreInterrupts(old);
    return task;
}

extern DSPTaskInfo *__DSP_rude_task;
extern int __DSP_rude_task_pending;

DSPTaskInfo *DSPAssertTask(DSPTaskInfo *task)
{
    s32 old;

    // ASSERTMSGLINE(0x261, __DSP_init_flag == 1, "DSPAssertTask(): DSP driver not initialized!\n");
    // ASSERTMSGLINE(0x262, task->flags & 1, "DSPAssertTask(): Specified task not in active task list!\n");

    old = OSDisableInterrupts();

    if (__DSP_curr_task == task) {
        __DSP_rude_task = task;
        __DSP_rude_task_pending = 1;
        OSRestoreInterrupts(old);
        return task;
    }
    if (task->priority < __DSP_curr_task->priority) {
        __DSP_rude_task = task;
        __DSP_rude_task_pending = 1;
        if (__DSP_curr_task->state == 1) {
            DSPAssertInt();
        }
        OSRestoreInterrupts(old);
        return task;
    }
    OSRestoreInterrupts(old);
    return NULL;
}
