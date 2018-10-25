#ifndef _PROCESSFORK_PROCESSFORK_H_
#define _PROCESSFORK_PROCESSFORK_H_ 1

#ifdef __cplusplus
extern "C" {
#endif

#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

int processfork_daemonize_fork();

typedef struct {
    const sigset_t* original_sigmask;
    const char* work_directory_path;
    uid_t run_user_id;
    gid_t run_group_id;
    int fd_stdin;
    int fd_stdout;
    int fd_stderr;
} ProcessForkStartupProperty;

pid_t processfork_execve_fork(ProcessForkStartupProperty* startup_prop,
                              char* const cmd_argv[],
                              char* const cmd_envp[]);

#ifdef __cplusplus
}
#endif

#endif /* _PROCESSFORK_PROCESSFORK_H_ */
