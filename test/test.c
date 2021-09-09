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

void dtor(GcNode* node)
{
    printf("Node dtor(%p)\n", node);
    printf("  size: %d\n", node->size);
    printf("  pointer: %p\n", node->pointer);
}

void test3()
{
    printf("Test GC - 3\n\n");

    gc_malloc_dtor(100, dtor);
}

int main(int argc, char* argv[])
{
    gc_init(true, &argc);
    //test1();
    //test2();
    test3();
    gc_shutdown();

    return 0;
}
