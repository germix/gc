#include "gc.h"
#include <stdio.h>

void test1()
{
    printf("Test GC - 1\n\n");
    for(int i = 0; i < 100; i++)
    {
        gc_malloc(100);
    }
}

void test2()
{
    printf("Test GC - 2\n\n");

    int** x = gc_malloc(100);
    x[0] = gc_malloc(10);
    x[1] = gc_malloc(10);
    x[2] = gc_malloc(10);

    printf("Before gc_run()\n");
    gc_run();
    printf("After gc_run()\n");

    x[0] = gc_malloc(10);
    x[1] = gc_malloc(10);
    x[2] = gc_malloc(10);

    printf("Before gc_run()\n");
    gc_run();
    printf("After gc_run()\n");

}

int main(int argc, char* argv[])
{
    gc_init(true, &argc);
    //test1();
    test2();
    gc_shutdown();

    return 0;
}
