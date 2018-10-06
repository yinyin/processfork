#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

#include "recordlogs/recordlogs.h"
#include "signalhandling/signalhandling.h"

#include "processfork/changeprocessproperty.h"
#include "processfork/closenonstdiofd.h"
#include "processfork/processfork.h"

static pid_t fork_for_daemonize(int fork_stage) {
    pid_t ret_id;
    ret_id = fork();
    if (-1 == ret_id) {
        RECORD_ERROR(LOG_ERR,
                     "cannot perform stage-%d fork for daemonize.",
                     fork_stage);
        return -1;
    } else if (0 < ret_id) {
        return 0;
    }
    if (0 != ret_id) {
        RECORD_ERROR(LOG_WARNING,
                     "result of fork < 0: %lld.",
                     (long long int)(ret_id));
    }
    return ret_id;
}

pid_t processfork_daemonize_fork() {
    pid_t ret_id;
    ret_id = fork_for_daemonize(1);
    if ((0 == ret_id) || (-1 == ret_id)) {
        return ret_id;
    }
    if (-1 == (ret_id = setsid())) {
        RECORD_ERROR(LOG_ERR, "cannot create new session.");
        return -1;
    }
    if (0 != signalhandling_ignore_signal(SIGHUP)) {
        return -1;
    }
    ret_id = fork_for_daemonize(2);
    return ret_id;
}

static pid_t fork_for_exec() {
    pid_t ret_id;
    ret_id = fork();
    if (-1 == ret_id) {
        RECORD_ERROR(LOG_ERR, "cannot perform fork for child command");
        return -1;
    } else if (0 < ret_id) {
        return ret_id;
    }
    if (0 != ret_id) {
        RECORD_ERROR(LOG_WARNING,
                     "result of fork < 0: %lld.",
                     (long long int)(ret_id));
    }
    return ret_id;
}

static int apply_startup_property(ProcessForkStartupProperty* startup_prop) {
    if (NULL == startup_prop) {
        return 0;
    }
    if (NULL != startup_prop->original_sigmask) {
        if (0 !=
            sigprocmask(SIG_SETMASK, startup_prop->original_sigmask, NULL)) {
            return -1;
        }
    }
    if (NULL != startup_prop->work_directory_path) {
        if (0 != processfork_change_work_directory(
                         startup_prop->work_directory_path)) {
            return -2;
        }
    }
    if ((0 != startup_prop->fd_stdin) || (0 != startup_prop->fd_stdout) ||
        (0 != startup_prop->fd_stderr)) {
        if (0 != processfork_redirect_stdio_fd(startup_prop->fd_stdin,
                                               startup_prop->fd_stdout,
                                               startup_prop->fd_stderr)) {
            return -3;
        }
    }
    if ((0 != startup_prop->run_user_id) || (0 != startup_prop->run_group_id)) {
        if (0 != processfork_set_run_account(startup_prop->run_user_id,
                                             startup_prop->run_group_id)) {
            return -4;
        }
    }
    return 0;
}

static void setup_process_group(pid_t process_id) {
    if (0 == setpgid(process_id, 0)) {
        return;
    }
    if (EACCES != errno) {
        RECORD_ERROR(
                LOG_WARNING,
                "failed on adjust group ID of process [%d] from process [%d]",
                (int)(process_id),
                (int)(getpid()));
    }
}

pid_t processfork_execve_fork(ProcessForkStartupProperty* startup_prop,
                              char* const cmd_argv[],
                              char* const cmd_envp[]) {
    pid_t ret_id;
    ret_id = fork_for_exec();
    if (0 < ret_id) {
        setup_process_group(ret_id);
        return ret_id;
    } else if (-1 == ret_id) {
        return -1;
    }
    setup_process_group(0);
    do {
        if (0 != apply_startup_property(startup_prop)) {
            break;
        }
        processfork_close_nonstdio_fd();
        execve(cmd_argv[0], cmd_argv, cmd_envp);
    } while (0);
    exit(125);
    return -1;
}
