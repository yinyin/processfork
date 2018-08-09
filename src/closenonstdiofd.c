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
#define FD_COLLECT_BUFFER_COUNT 8

static int parse_fd_from_dirent(const struct dirent *entry) {
    char *p_strend = NULL;
    int fd = 0;
    if ('.' == entry->d_name[0]) {
        return -2;
    }
    fd = strtol(entry->d_name, &p_strend, 10);
    if ((NULL == p_strend) || (entry->d_name == p_strend)) {
        RECORD_ERROR(LOG_WARNING,
                     "invalid file descriptor number: %s",
                     entry->d_name);
        return -1;
    }
    if (fd < 3) {
        return -3;
    }
    return fd;
}
static int collect_nonstdio_fd(int *fd_collect_buffer,
                               const char *fd_folder_path) {
    DIR *dp;
    struct dirent *fetch_result;
    int next_fd_idx;
    memset(fd_collect_buffer, 0, sizeof(int) * FD_COLLECT_BUFFER_COUNT);
    if (NULL == (dp = opendir(fd_folder_path))) {
        RECORD_ERROR(LOG_ERR,
                     "cannot open file descriptor folder: %s",
                     fd_folder_path);
        return -1;
    }
    next_fd_idx = 0;
    errno = 0;
    while (NULL != (fetch_result = readdir(dp))) {
        int collected_fd = parse_fd_from_dirent(fetch_result);
        if (collected_fd > 0) {
            fd_collect_buffer[next_fd_idx] = collected_fd;
            next_fd_idx++;
            if (next_fd_idx == FD_COLLECT_BUFFER_COUNT) {
                errno = 0;
                break;
            }
        }
        errno = 0;
    }
    if (0 != errno) {
        RECORD_ERROR(LOG_ERR,
                     "failed on fetching file descriptor directory entry: %s",
                     fd_folder_path);
        next_fd_idx = -1;
    }
    closedir(dp);
    return next_fd_idx;
}
static int _close_nonstdio_fd_fdfolder(const char *fd_folder_path) {
    int fd_collect_buffer[FD_COLLECT_BUFFER_COUNT];
    int collected_count;
    int failure_count = 0;
    do {
        int i;
        collected_count =
                collect_nonstdio_fd(fd_collect_buffer, fd_folder_path);
        failure_count = (0 > collected_count) ? (failure_count + 1) : 0;
        for (i = 0; i < FD_COLLECT_BUFFER_COUNT; i++) {
            int fd;
            if (0 == (fd = fd_collect_buffer[i])) {
                break;
            }
            close(fd);
        }
    } while (((collected_count < 0) && (failure_count < 8)) ||
             (FD_COLLECT_BUFFER_COUNT == collected_count));
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
