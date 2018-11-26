#include "sk_buff.h"
#include <stdio.h>

int main(){
    struct sk_buff *skb = alloc_skb(20);
    skb_put(skb, "hello world", 11);
    skb_push(skb, "header ", 7);
    printf("%s", skb->data);
}
