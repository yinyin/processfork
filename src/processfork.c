#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

#include "recordlogs/recordlogs.h"
#include "signalhandling/signalhandling.h"

#include "processfork/closenonstdiofd.h"

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

pid_t daemonize_fork() {
    pid_t ret_id;
    ret_id = fork_for_daemonize(1);
    if ((0 == ret_id) || (-1 == ret_id)) {
        return ret_id;
    }
    if (-1 == (ret_id = setsid())) {
        RECORD_ERROR(LOG_ERR, "cannot create new session.");
        return -1;
    }
    if (0 != ignore_signal(SIGHUP)) {
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

static int redirect_stdio_fds(int fd_stdin, int fd_stdout, int fd_stderr) {
    int retcode = -1;
    do {
        if (-1 == fd_stdin) {
            if (-1 == (fd_stdin = open("/dev/null", O_RDONLY))) {
                RECORD_ERROR(LOG_ERR, "failed on open /dev/null for STDIN");
                break;
            }
        }
        retcode = -2;
        if (-1 == dup2(fd_stdin, STDIN_FILENO)) {
            RECORD_ERROR(LOG_ERR, "failed on override STDIN.");
            break;
        }
        retcode = -3;
        if (-1 != fd_stdout) {
            if (-1 == dup2(fd_stdout, STDOUT_FILENO)) {
                RECORD_ERROR(LOG_ERR, "failed on override STDOUT.");
                break;
            }
        }
        if (-1 != fd_stderr) {
            if (-1 == dup2(fd_stderr, STDERR_FILENO)) {
                RECORD_ERROR(LOG_ERR, "failed on override STDERR.");
                break;
            }
        }
        retcode = 0;
    } while (0);
    if (-1 != fd_stdin) {
        close(fd_stdin);
    }
    if (-1 != fd_stdout) {
        close(fd_stdout);
    }
    if (-1 != fd_stderr) {
        close(fd_stderr);
    }
    return retcode;
}

pid_t execve_fork(int fd_stdin,
                  int fd_stdout,
                  int fd_stderr,
                  char* const cmd_argv[],
                  char* const cmd_envp[]) {
    pid_t ret_id;
    ret_id = fork_for_exec();
    if ((0 < ret_id) || (-1 == ret_id)) {
        return ret_id;
    }
    redirect_stdio_fds(fd_stdin, fd_stdout, fd_stderr);
    close_nonstdio_fd();
    execve(cmd_argv[0], cmd_argv, cmd_envp);
    exit(126);
    return -1;
}
