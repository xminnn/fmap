## fmap
一个基于skiplist和mmap的、可持久化的、**可直接使用映射内存**、高性能kv数据库。可用与c/c++用来做缓存层，实现程序快速重启，快速拉起等。非常适合c++游戏后端开发使用，替代redis。

## 使用示例

```c
struct fmap *mp = fmap_mount("test.bin");
int p = 0;
struct fmap_index *t = fmap_put(mp, "/user/1", &p, sizeof(p));
struct fmap_index *it = fmap_get(mp, "/user/1");
int* ptr = fmap_val(mp, it, sizeof(p));
fmap_del(mp, "/user/1");
fmap_unmount(mp);
```

```c
struct fmap *mp = fmap_mount("test.bin");
struct fmap_index *t = fmap_put(mp, "/user/1", 0, sizeof(int));
struct fmap_index *it = fmap_get(mp, "/user/1");
int* ptr = fmap_val(mp, it, sizeof(int));
fmap_del(mp, "/user/1");
fmap_unmount(mp);
```

```c
// struct test {
//     int i;
// };
struct fmap_index *fstart = fmap_get_ge(mp, "/login/");
struct fmap_index *fend = fmap_get_ge(mp, "/login/~");
while (fstart && fstart != fend) {
    struct test *t = fmap_val(mp, fstart, sizeof(struct test));
    printf("%d\n", t->i);
    fstart = fmap_nxt(mp, fstart);
}
```

## 性能

非4k最低大小限制下，插入查找int测试
```
性能测试参考:
    -O0: 100w插入: 1467ms. 100w查找: 390ms
    -O3: 100w查找: 263ms
```

4k最低大小下(目的是为确保了sync能够完成页回存，需要确保落盘的场景下)，性能会有所降低(mac air下测试：100w存入3-4s, 查找700-800ms)，主要是因为100w需要的内存更大(4G)，需要而外创建并且映射bin文件，并且从磁盘中拉取数据到内存从而耗时。

如果确定小数据不需要落盘，可以删除`fmap.c:90`行的 total_size 最小限制