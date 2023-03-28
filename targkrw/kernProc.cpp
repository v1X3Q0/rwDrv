#include <localUtil.h>
#include <stdio.h>
#include <unistd.h>
#include "linuxkern_tools.h"
#include "linux_krwp.h"

int kernel_task(size_t* proc_out)
{
	void* proc_tmp = 0;

	proc_tmp = kdlsym("init_task");
	kernel_read(&proc_tmp, sizeof(size_t), (size_t)proc_tmp);
	if (proc_out != 0)
	{
		*proc_out = (size_t)proc_tmp;
	}
	return 0;
}

int findKernProcByPid(pid_t targpid, size_t* procAddress)
{
    int result = -1;
    size_t kern_proc_local = 0;
    task_struct kern0 = 0;

    SAFE_BAIL(kernel_task(&kern_proc_local) == -1);
    kern0 = proc(kern_proc_local);

    for(; kern0.p_list().prev() != 0; kern0 = kern0.p_list().prev())
    {
        FINISH_IF(kern0.p_pid() == targpid);
    }
    goto fail;
finish:
    result = 0;
    if (procAddress != 0)
    {
        *procAddress = *kern0;
    }
fail:
    return result;
}

int findKernProcByPid(pid_t targpid, size_t* procAddress)
{
    int result = -1;
    size_t kern_proc_local = 0;
    eprocess proc_iter = 0;
    size_t sys_proc_initial = 0;

    SAFE_BAIL(system_eproc(&kern_proc_local) == -1);
    proc_iter = eprocess(kern_proc_local);
    sys_proc_initial = *proc_iter;

    for (; *(proc_iter.ActiveProcessLinks().next()) != sys_proc_initial; proc_iter = *(proc_iter.ActiveProcessLinks().next()))
    {
        FINISH_IF(proc_iter.UniqueProcessId() == (void*)targpid);
    }
    goto fail;
finish:
    result = 0;
    if (procAddress != 0)
    {
        *procAddress = *proc_iter;
    }
fail:
    return result;
}

