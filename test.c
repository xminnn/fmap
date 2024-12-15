
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "fmap.h"

#define int64 long long
#define var __auto_type

struct test {
    int i;
};

static char buff[1024];

long long time_curruent_ms() {
    long long now;
    struct timeval tv;
    gettimeofday(&tv, 0);
    now = tv.tv_sec;
    now = now * 1000000;
    now += tv.tv_usec;
    return now / 1000;
}

int main(int argc, char const *argv[]) {
    setvbuf(stdout, NULL, _IOFBF, 0);

    struct fmap *mp = fmap_mount("test.bin");
    int64 start = time_curruent_ms();
    int flag = argc > 1 ? atoi(argv[1]) : 1;
    int count = argc > 2 ? atoi(argv[2]) : 1000000;
    for (int i = 0; i < count; i++) {
        char path[24];
        int p = i;
        int n = snprintf(path, 24, "/test/%d", p);
        
        if (flag == 1) {
            var t = fmap_put(mp, path, &p, sizeof(p));
        } else if (flag == 2) {
            var it = fmap_get(mp, path);
            if (!it) {
                printf("error buff: %s\n", path);
                continue;
            }
        }
        // int ret = fmap_sync(fmap_val(mp, it, sizeof(buff)), k_fmap_flush_async);
        // if (ret != 0) {
        //     printf("ret=%d, errno=%d\n", ret, (int)errno);
        //     break;
        // }
        // fmap_sync(mp, k_fmap_flush_sync, fmap_get(mp, path));
    }
    int64 end = time_curruent_ms();
    if (flag == 1) {
        printf("put time cost: %lldms\n", end - start);
    } else if (flag == 2) {
        printf("search time cost: %lldms\n", end - start);
    }

    // 测试范围遍历
    struct fmap_index *ti = fmap_touch(mp, "/login/234", sizeof(struct test));
    struct test *t = fmap_val(mp, ti, sizeof(struct test));
    t->i = 234;
    ti = fmap_touch(mp, "/login/235", sizeof(struct test));
    t = fmap_val(mp, ti, sizeof(struct test));
    t->i = 235;
    ti = fmap_touch(mp, "/abc/235", sizeof(struct test));
    t = fmap_val(mp, ti, sizeof(struct test));
    t->i = 236;
    struct fmap_index *fstart = fmap_get_ge(mp, "/login/");
    struct fmap_index *fend = fmap_get_ge(mp, "/login/~");
    while (fstart && fstart != fend) {
        struct test *logined = fmap_val(mp, fstart, sizeof(struct test));
        printf("%d\n", logined->i);
        fstart = fmap_nxt(mp, fstart);
    }

    fmap_unmount(mp);
    return 0;
}