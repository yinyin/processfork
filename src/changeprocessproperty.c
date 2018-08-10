#include <fcntl.h>
#include <syslog.h>
#include <unistd.h>

#include "recordlogs/recordlogs.h"

#include "processfork/changeprocessproperty.h"

int processfork_change_work_directory(const char* work_directory_path) {
    if (0 != chdir(work_directory_path)) {
        RECORD_ERROR(LOG_ERR,
                     "failed on changing work directory [%s]",
                     work_directory_path);
        return -1;
    }
    return 0;
}

int processfork_change_run_account(uid_t run_uid, gid_t run_gid) {
    if (0 != run_gid) {
        if (0 != setgid(run_gid)) {
            RECORD_ERROR(LOG_ERR, "failed on set GID as [%d]", (int)(run_gid));
            return -1;
        }
        if (0 != setegid(run_gid)) {
            RECORD_ERROR(LOG_ERR, "failed on set EGID as [%d]", (int)(run_gid));
            return -1;
        }
    }
    if (0 != run_uid) {
        if (0 != setuid(run_uid)) {
            RECORD_ERROR(LOG_ERR, "failed on set UID as [%d]", (int)(run_uid));
            return -1;
        }
        if (0 != seteuid(run_uid)) {
            RECORD_ERROR(LOG_ERR, "failed on set EUID as [%d]", (int)(run_uid));
            return -1;
        }
    }
    return 0;
}

int processfork_redirect_stdio_fd(int fd_stdin, int fd_stdout, int fd_stderr) {
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
