#ifndef ___GC_H___
#define ___GC_H___
#include <stdint.h>
#include <stdbool.h>

typedef struct _Gc Gc;
typedef struct _GcNode GcNode;
typedef struct _GcRoot GcRoot;

typedef void (*GcNodeDestructor)(GcNode* node);

#define GC_DEBUG_MALLOC         0x00000001
#define GC_DEBUG_DELETE         0x00000002
#define GC_DEBUG_SHUTDOWN       0x00000004
#define GC_DEBUG_RUN            0x00000008

#define GC_DEBUG_ALL            (GC_DEBUG_MALLOC | GC_DEBUG_DELETE | GC_DEBUG_SHUTDOWN | GC_DEBUG_RUN)

typedef struct _Gc
{
    unsigned int        debug;
    void*               stackBottom;        // Bottom of call-stack

    GcNode*             nodeListHead;       // First node in this list
    GcNode*             nodeListTail;       // Last node in this list

    unsigned int        max;
    unsigned int        allocated;

    GcRoot*             rootListHead;       // First root in the list
    GcRoot*             rootListTail;       // Last root in this list
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

typedef struct _GcRoot
{
    GcRoot*             prev;
    GcRoot*             next;
    uintptr_t*          pointer;
}GcRoot;

#ifdef __cplusplus
extern "C" {
#endif

extern Gc gc;

/**
 * Initialize the garbage collector
 * 
 * @param[in] max           - Max heap size
 * @param[in] debug         - Debug flags
 * @param[in] stackBottom   - Call-stack bottom
 */
void gc_init(unsigned int max, unsigned int debug, void* stackBottom);

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

/**
 * Clear all registered roots
 */
void gc_clear_roots();

/**
 * Register a root
 * 
 * @param pointer           - Root pointer
 */
void gc_register_root(void* pointer);

#ifdef __cplusplus
};
#endif

#endif
