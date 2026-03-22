#include <stdio.h>
#include <stdlib.h>

int a = 5; 
int b;

void light() {
    int one[1000];
    int two[1000];
    int y;
    printf("Local stack: %p\n", &y);
}

int main() {
    int x;
    int *p = malloc(sizeof(int));

    printf("text: %p\n", main);
    printf("data: %p\n", &a);
    printf("bss: %p\n", &b);
    printf("stack: %p\n", &x);
    printf("heap: %p\n", p);

    light();
    free(p);
    return 0;
}
