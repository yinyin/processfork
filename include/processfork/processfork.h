#ifndef _PROCESSFORK_PROCESSFORK_H_
#define _PROCESSFORK_PROCESSFORK_H_ 1

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <unistd.h>

pid_t processfork_daemonize_fork();

pid_t processfork_execve_fork(int fd_stdin,
                              int fd_stdout,
                              int fd_stderr,
                              char* const cmd_argv[],
                              char* const cmd_envp[]);

#ifdef __cplusplus
}
#endif

#endif /* _PROCESSFORK_PROCESSFORK_H_ */
