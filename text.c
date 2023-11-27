#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
 * 类型别名，用于指向 sdshdr 的 buf 属性
 */
typedef char *sds;

/*
 * 保存字符串对象的结构
 */
typedef struct sdshdr {
    
    // buf 中已占用空间的长度 8
    int len;

    // buf 中剩余可用空间的长度 
    int free;

    // 数据空间
    char buf[];
}SDS;

/*
 * 返回 sds 实际保存的字符串的长度
 *
 * T = O(1)
 */
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

void sdsrange(sds s, int start, int end) {
    struct sdshdr *sh = (struct sdshdr *) (s-(sizeof(struct sdshdr)));
    int newlen, len = 11;

    if (len == 0) return;
    if (start < 0) {
        start = len+start;
        if (start < 0) start = 0;
    }
    if (end < 0) {
        end = len+end;
        if (end < 0) end = 0;
    }
    newlen = (start > end) ? 0 : (end-start)+1;
    if (newlen != 0) {
        if (start >= (signed)len) {
            newlen = 0;
        } else if (end >= (signed)len) {
            end = len-1;
            newlen = (start > end) ? 0 : (end-start)+1;
        }
    } else {
        start = 0;
    }

    // 如果有需要，对字符串进行移动
    // T = O(N)
    if (start && newlen) memmove(sh->buf, sh->buf+start, newlen);

    // 添加终结符
    sh->buf[newlen] = 0;

    // 更新属性
    sh->free = sh->free+(sh->len-newlen);
    sh->len = newlen;
}

int main(int argc, char* argv[])
{   
    int a = 1, b = 2;
    struct sdshdr shh = {11,5,"hello world\0"};
    sdsrange(shh.buf,1,-1);
    return 0;
}