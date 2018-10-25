#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "processfork/processfork.h"

static const char* HEARTBEAT_FILEPATH =
        "/tmp/test-daemonizedsleep-heartbeat.txt";

static int daemonize() {
    int resultcode;
    switch (resultcode = processfork_daemonize_fork()) {
        case 0: {
            return 1;
        } break;
        case 1:
            return 0;
        default: {
            fprintf(stderr, "ERR: abnormal result code: %d\n", resultcode);
        }
    }
    return -1;
}

static int write_heartbeat(int i) {
    FILE* fp;
    if (NULL == (fp = fopen(HEARTBEAT_FILEPATH, "w+"))) {
        fprintf(stderr,
                "ERR: failed on open heart-beat file [%s]\n",
                HEARTBEAT_FILEPATH);
        return -1;
    }
    fprintf(fp, "Heart-beat %d.\n", i);
    fclose(fp);
    return 0;
}

static int write_finished() {
    FILE* fp;
    if (NULL == (fp = fopen(HEARTBEAT_FILEPATH, "w+"))) {
        fprintf(stderr,
                "ERR: failed on open heart-beat file [%s]\n",
                HEARTBEAT_FILEPATH);
        return -1;
    }
    fprintf(fp, "Daemonized sleep completed.\n");
    fclose(fp);
    return 0;
}

int main(int argc, char* const argv[]) {
    int i;
    if (0 != daemonize()) {
        return 0;
    }
    for (i = 0; i < 10; i++) {
        if (0 != write_heartbeat(i)) {
            break;
        }
        sleep(1);
    }
    write_finished();
    return 0;
}
