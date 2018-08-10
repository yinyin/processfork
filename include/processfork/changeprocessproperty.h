#ifndef _PROCESSFORK_CHANGEPROCESSPROPERTY_H_
#define _PROCESSFORK_CHANGEPROCESSPROPERTY_H_ 1

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>

int processfork_change_work_directory(const char* work_directory_path);
int processfork_set_run_account(uid_t run_uid, gid_t run_gid);
int processfork_redirect_stdio_fd(int fd_stdin, int fd_stdout, int fd_stderr);

#ifdef __cplusplus
}
#endif

#endif /* _PROCESSFORK_CHANGEPROCESSPROPERTY_H_ */
