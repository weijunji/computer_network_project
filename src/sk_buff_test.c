#include "sk_buff.h"
#include <stdio.h>

int main(){
    struct sk_buff *skb = alloc_skb(20);
    skb_put(skb, "hello world", 11);
    skb_push(skb, "header ", 7);
    skb_put(skb, " tail", 5);
    printf("%s\n", skb_data(skb)); // result: header hello world tail

    skb_pull(skb, 7);
    printf("%s\n", skb_data(skb)); // result: hello world tail

    skb_remove(skb, 5);
    printf("%s\n", skb_data(skb));

    free_skb(skb);
}
