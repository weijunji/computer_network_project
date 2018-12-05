#include "sk_buff.h"

struct sk_buff* alloc_skb(unsigned int size){
    struct sk_buff* skb = (struct sk_buff*) malloc(sizeof(struct sk_buff));
    skb->head = (unsigned char*) malloc(sizeof(unsigned char) * (MAX_TCP_HEADER + size + MAX_TAIL));
    skb->end = skb->head + (MAX_TCP_HEADER + size + MAX_TAIL);
    skb->data = skb->head;
    skb->tail = skb->head;
    _skb_reserver(skb, MAX_TCP_HEADER);
    skb->next = NULL;
    skb->prev = NULL;
    return skb;
}

void free_skb(struct sk_buff* skb){
    free(skb->head);
    free(skb);
}

static void _skb_reserver(struct sk_buff *skb, int len){
    skb->data += len;
    skb->tail += len;
}

void skb_put(struct sk_buff *skb, unsigned char* data, unsigned int len){
    memcpy(skb->tail, data, len);
    skb->tail += len;
    *(skb->tail) = '\0';
    skb->len += len;
}

void skb_push(struct sk_buff *skb, unsigned char* data, unsigned int len){
    skb->data -= len;
    memcpy(skb->data, data, len);
    skb->len += len;
}

void skb_pull(struct sk_buff *skb, unsigned int len){
    skb->data += len;
    skb->len -= len;
}

void skb_remove(struct sk_buff *skb, unsigned int len){
    skb->tail -= len;
    *(skb->tail) = '\0';
    skb->len -= len;
}
