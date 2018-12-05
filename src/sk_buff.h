#ifndef SK_BUFF_H
#define SK_BUFF_H

#include <stdlib.h>
#include <string.h>

#define LL_MAX_HEADER 128
#define MAX_HEADER (LL_MAX_HEADER + 48)
#define MAX_TCP_HEADER	(128 + MAX_HEADER)
#define MAX_TAIL 32

struct sk_buff{
    // 双向链表
    struct sk_buff  *next;
    struct sk_buff  *prev;

    // head ->   |--------|
    //           |        |
    // data ->   |--------|
    //           |        |
    //           |  data  |
    //           |        |
    //           |--------|
    // tail ->   |   \0   |
    //           |        |
    // end  ->   |--------|
    unsigned char   *head,
                    *data,
                    *tail,
                    *end;

    unsigned int    len,      // 向下一层传递的长度
                    data_len; // 实际数据长度（不包括头尾）

    unsigned short  protocol; // 通知上层用什么协议处理

    char*           dest_ip;   // 目的ip
    int             dest_port; // 目的端口
    unsigned char*  dest_mac; // 目的mac
};

#define skb_data(SKB) (SKB->data)

// 申请空间
struct sk_buff* alloc_skb(unsigned int size);
// free
void free_skb(struct sk_buff* skb);
// 下移数据区，预留头部空间
static void _skb_reserver(struct sk_buff *skb, int len);
// 发送用，在数据区尾部添加空间
void skb_put(struct sk_buff *skb, unsigned char* data, unsigned int len);
// 发送用，在数据区头部添加空间
void skb_push(struct sk_buff *skb, unsigned char* data, unsigned int len);
// 接收用，忽略数据区头部的数据，即移除包头
void skb_pull(struct sk_buff *skb, unsigned int len);
// 接收用，移除尾部
void skb_remove(struct sk_buff *skb, unsigned int len);

#endif