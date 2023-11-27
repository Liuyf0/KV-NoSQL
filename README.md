```
/* 将一个长整型数值转换为字符串表示，
 * s指向字符串中点*/
int sdsll2str(char *s, long long value) {
    char *p, aux;
    unsigned long long v;
    size_t l;

    /* Generate the string representation, this method produces
     * an reversed string. */
    v = (value < 0) ? -value : value;
    p = s;
    do {
        *p++ = '0'+(v%10);
        v /= 10;
    } while(v);
    if (value < 0) *p++ = '-';

    /* Compute length and add null term. */
    l = p-s;
    *p = '\0';

    /* Reverse the string. */
    p--;
    while(s < p) {
        aux = *s;
        *s = *p;
        *p = aux;
        s++;
        p--;
    }
    return l;
}
```


```
/*字符串左右两端修剪*/
sds sdstrim(sds s, const char *cset) {
    struct sdshdr *sh = (void*) (s-(sizeof(struct sdshdr)));
    char *start, *end, *sp, *ep;
    size_t len;

    // 设置和记录指针
    sp = start = s;
    ep = end = s+sdslen(s)-1;

    // 修剪, T = O(N^2)
    while(sp <= end && strchr(cset, *sp)) sp++;
    while(ep > start && strchr(cset, *ep)) ep--;

    // 计算 trim 完毕之后剩余的字符串长度
    len = (sp > ep) ? 0 : ((ep-sp)+1);
  
    // 如果有需要，前移字符串内容
    // T = O(N)
    if (sh->buf != sp) memmove(sh->buf, sp, len);

    // 添加终结符
    sh->buf[len] = '\0';

    // 更新属性
    sh->free = sh->free+(sh->len-len);
    sh->len = len;

    // 返回修剪后的 sds
    return s;
}
```
