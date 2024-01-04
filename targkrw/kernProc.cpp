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

// int findKernProcByPid(pid_t targpid, size_t* procAddress)
// {
//     int result = -1;
//     size_t kern_proc_local = 0;
//     task_struct kern0 = 0;

//     SAFE_BAIL(kernel_task(&kern_proc_local) == -1);
//     kern0 = proc(kern_proc_local);

//     for(; kern0.p_list().prev() != 0; kern0 = kern0.p_list().prev())
//     {
//         FINISH_IF(kern0.p_pid() == targpid);
//     }
//     goto fail;
// finish:
//     result = 0;
//     if (procAddress != 0)
//     {
//         *procAddress = *kern0;
//     }
// fail:
//     return result;
// }

int get_pid_task(pid_t targ_pid, size_t* task_out)
{
   int result = -1;
   void* init_task = 0;

   init_task = kdlsym("init_task");
   SAFE_BAIL(init_task == 0);

   result = 0;
fail:
   return result;
}