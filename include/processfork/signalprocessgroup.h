#ifndef _PROCESSFORK_SIGNALPROCESSGROUP_H_
#define _PROCESSFORK_SIGNALPROCESSGROUP_H_ 1

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>

int processfork_signal_process_group(pid_t process_id, int sig);

#ifdef __cplusplus
}
#endif

#endif /* _PROCESSFORK_SIGNALPROCESSGROUP_H_ */
