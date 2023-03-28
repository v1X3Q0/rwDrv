#ifndef LINUX_KRWP_H
#define LINUX_KRWP_H

#include <string>
#include <unistd.h>

#include <krw_ptr.h>

SABER(task_struct,
    JEAN(list_entry<kern_bkptr<task_struct>>, task_struct, tasks);
    RIDER(pid_t, task_struct, p_pid));

#endif // LINUX_KRWP_H–