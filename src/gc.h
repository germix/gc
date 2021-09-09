#ifndef ___GC_H___
#define ___GC_H___
#include <stdbool.h>

typedef struct _Gc Gc;
typedef struct _GcNode GcNode;

typedef void (*GcNodeDestructor)(GcNode* node);

typedef struct _Gc
{
    bool                debug;
    void*               stackBottom;        // Bottom of call-stack

    GcNode*             nodeListHead;       // First node in this list
    GcNode*             nodeListTail;       // Last node in this list

    unsigned int        max;
    unsigned int        allocated;
}Gc;

typedef struct _GcNode
{
    GcNode*             next;               // Next node in the linked-list
    GcNode*             prev;               // Previous node in the linked-list
    unsigned int        size;               // Allocated size in bytes
    bool                marked;
    GcNodeDestructor    dtor;               // Destructor
    void*               pointer;            // Memory pointer
}GcNode;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize the garbage collector
 * 
 * @param[in] debug         - Debug mode ON
 * @param[in] stackBottom   - Call-stack bottom
 */
void gc_init(bool debug, void* stackBottom);

/**
 * Shutdown the garbage collector
 */
void gc_shutdown();

/**
 * Allocate a new object
 *
 * @param[in] sizeInBytes   - Size in bytes for the object
 *
 * @return Pointer to the new object instance
 */
void* gc_malloc(int sizeInBytes);

/**
 * Allocate a new object with destructor
 *
 * @param[in] sizeInBytes   - Size in bytes for the object
 * @param[in] dtor          - Destructor for the object
 *
 * @return Pointer to the new object instance
 */
void* gc_malloc_dtor(int sizeInBytes, GcNodeDestructor dtor);

/**
 * Run garbage collector
 */
void gc_run();

#ifdef __cplusplus
};
#endif

#endif
