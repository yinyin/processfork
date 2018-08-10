#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "processfork/processfork.h"

int main(int argc, char* const argv[]) {
    char* const child_argv[] = {"/usr/bin/printenv", NULL};
    char* const child_envp[] = {"PATH=/usr/bin:/bin", NULL};
    pid_t child_pid;
    int retcode;
    if (-1 == (child_pid = processfork_execve_fork(
                       -1, -1, -1, child_argv, child_envp))) {
        fprintf(stderr, "Failed on invoke child program\n");
        return 1;
    }
    fprintf(stderr, "Child PID: %lld\n", (long long int)(child_pid));
    if (-1 == waitpid(child_pid, &retcode, 0)) {
        fprintf(stderr, "Failed on waiting child process.\n");
        return 1;
    }
    fprintf(stderr,
            "Exit code: %d / sig: %d\n",
            WEXITSTATUS(retcode),
            WTERMSIG(retcode));
    return 0;
}
