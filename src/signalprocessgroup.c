#include <signal.h>
#include <syslog.h>
#include <unistd.h>

#include "recordlogs/recordlogs.h"

#include "processfork/signalprocessgroup.h"

int processfork_signal_process_group(pid_t process_id, int sig) {
    pid_t pgid;
    int retcode;
    if (0 > (pgid = getpgid(process_id))) {
        RECORD_ERROR(LOG_ERR,
                     "failed to get process group ID of process: %d",
                     (int)(process_id));
        return -1;
    }
    if (0 != (retcode = kill(-pgid, sig))) {
        RECORD_ERROR(LOG_ERR,
                     "failed to signal process group [%d] for process [PID=%d]",
                     (int)(pgid),
                     (int)(process_id));
    }
    return retcode;
}
