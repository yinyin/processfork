#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

#include "recordlogs/recordlogs.h"

#include "processfork/closenonstdiofd.h"

#if defined(__APPLE__) || defined(__linux__)
static int _close_fd_by_dirent(const struct dirent *entry) {
    char *p_strend = NULL;
    int fd = 0;
    if ('.' == entry->d_name[0]) {
        return 1;
    }
    fd = strtol(entry->d_name, &p_strend, 10);
    if ((NULL == p_strend) || (entry->d_name == p_strend)) {
        RECORD_ERROR(LOG_WARNING,
                     "invalid file descriptor number: %s",
                     entry->d_name);
        return -1;
    }
    if (fd < 3) {
        return 2;
    }
    close(fd);
    return 0;
}
static int _close_nonstdio_fd_fdfolder(const char *fd_folder_path) {
    DIR *dp;
    struct dirent *fetch_result;
    if (NULL == (dp = opendir(fd_folder_path))) {
        RECORD_ERROR(LOG_ERR,
                     "cannot open file descriptor folder: %s",
                     fd_folder_path);
        return -1;
    }
    errno = 0;
    while (NULL != (fetch_result = readdir(dp))) {
        _close_fd_by_dirent(fetch_result);
        errno = 0;
    }
    if (0 != errno) {
        RECORD_ERROR(LOG_ERR,
                     "failed on fetching file descriptor directory entry: %s",
                     fd_folder_path);
    }
    closedir(dp);
    return 0;
}
#else
static int _close_nonstdio_fd_posix() {
    int max_fd = (int)(sysconf(_SC_OPEN_MAX));
    for (int fd = 3; fd < max_fd; fd++) {
        close(fd);
    }
    return 0;
}
#endif /* check platform */

int close_nonstdio_fd() {
#if defined(__linux__)
    return _close_nonstdio_fd_fdfolder("/proc/self/fd");
#elif defined(__APPLE__)
    return _close_nonstdio_fd_fdfolder("/dev/fd");
#else
    return _close_nonstdio_fd_posix();
#endif
}
