#include "gc.h"
#include <stdio.h>

#define KB(bytes)       ((bytes) << 10)
#define MB(bytes)       ((bytes) << 20)
#define GB(bytes)       ((bytes) << 30)

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

void test4()
{
    printf("Test GC - 4\n\n");
    static int* a;
    static int* b;

    gc_register_root(&a);

    a = gc_malloc(100);
    b = gc_malloc(100);
    gc_run();
    a = NULL;
    b = NULL;
    gc_run();

    printf("..\n");
}

int main(int argc, char* argv[])
{
    gc_init(KB(4), GC_DEBUG_ALL, &argc);
    //test1();
    //test2();
    //test3();
    test4();
    gc_shutdown();

    return 0;
}
