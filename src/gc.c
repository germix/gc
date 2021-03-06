#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gc.h"

Gc gc;

static void gc_mark_node(GcNode* node);

//
// Comprobar si es un objeto
//
static GcNode* gc_is_reachable_object(uintptr_t ptr)
{
    for(GcNode* node = gc.nodeListHead; node != NULL; node = node->next)
    {
        if(node->pointer == (void*)ptr)
        {
            return node;
        }
    }
    return NULL;
}

//
// Escanear una región
//
static void gc_scan_region(uintptr_t* beg, uintptr_t* end)
{
    for(uintptr_t* ptr = beg; ptr < end; ptr++)
    {
        GcNode* node;

        if(NULL != (node = gc_is_reachable_object(*ptr)))
        {
            if(!node->marked)
            {
                gc_mark_node(node);
            }
        }
    }
}

//
// Marcar nodo y escanear campos
//
static void gc_mark_node(GcNode* node)
{
    // Marcar como alcanzable
    node->marked = true;

    //
    // Recorrer cada una de las referencias dentro de los campos del objeto
    //
    uintptr_t* beg = (uintptr_t*)node->pointer;
    uintptr_t* end = beg + (node->size / sizeof(uintptr_t));
    gc_scan_region(beg, end);
}

//
// Eliminar nodo
//
static void gc_delete_node(GcNode* node)
{
    if(gc.debug & GC_DEBUG_DELETE)
    {
        printf("GC: delete %p pointer\n", node->pointer);
    }

    // Call destructor
    if(node->dtor)
        node->dtor(node);

    // ...
    gc.allocated -= node->size;

    // Free node
    free(node);
}

//
// Fase de marcado
//
static void gc_mark_phase()
{
    uintptr_t _;
    uintptr_t* beg = (uintptr_t*)&_;
    uintptr_t* end = (uintptr_t*)gc.stackBottom;

    //
    // Recorrer cada una de las raices (punteros en call-stack)
    //
    gc_scan_region(beg, end);

    //
    // Recorrer cada una de las raices registradas (variables globales)
    //
    for(GcRoot* root = gc.rootListHead; root != NULL; root = root->next)
    {
        GcNode* node;

        if(NULL != (node = gc_is_reachable_object(*root->pointer)))
        {
            if(!node->marked)
            {
                gc_mark_node(node);
            }
        }
    }
}

//
// Fase de barrido
//
static void gc_sweep_phase()
{
    GcNode* node;
    GcNode* next;
    for(node = gc.nodeListHead; node != NULL; node = next)
    {
        next = node->next;
        if(node->marked)
        {
            node->marked = false;
        }
        else
        {
            if(node->prev)
                node->prev->next = node->next;
            if(node->next)
                node->next->prev = node->prev;
            if(gc.nodeListHead == node)
                gc.nodeListHead = node->next;
            if(gc.nodeListTail == node)
                gc.nodeListTail = node->prev;
            
            gc_delete_node(node);
        }
    }
}

void gc_init(unsigned int max, unsigned int debug, void* stackBottom)
{
    memset(&gc, 0, sizeof(Gc));

    gc.max = max;
    gc.debug = debug;
    gc.stackBottom = stackBottom;
}

void gc_shutdown()
{
    gc_clear_roots();

    gc_run();

    if(gc.debug & GC_DEBUG_SHUTDOWN)
    {
        printf("GC: Memory leak (%d bytes)\n", gc.allocated);
    }
}

void* gc_malloc(int sizeInBytes)
{
    return gc_malloc_dtor(sizeInBytes, NULL);
}

void* gc_malloc_dtor(int sizeInBytes, GcNodeDestructor dtor)
{
    if(gc.debug & GC_DEBUG_MALLOC)
    {
        printf("GC: alloc %d bytes\n", sizeInBytes);
    }
    if(gc.max < (gc.allocated + sizeInBytes))
    {
        gc_run();
    }

    // Tamaño de nodo
    int nodeSize = sizeInBytes + sizeof(GcNode);

    // Reservar nodo
    GcNode* node = (GcNode*)malloc(nodeSize);
    if(!node)
    {
        gc_run();
        node = (GcNode*)malloc(nodeSize);
    }

    // Inicializar nodo
    memset(node, 0, nodeSize);
    node->dtor = dtor;
    node->size = sizeInBytes;
    node->pointer = (((unsigned char*)node) + sizeof(GcNode));
    
    // Enlazar nodo en la lista
    if(gc.nodeListHead == NULL)
    {
        gc.nodeListHead = node;
        gc.nodeListTail = node;
    }
    else
    {
        node->prev = gc.nodeListTail;
        gc.nodeListTail->next = node;
        gc.nodeListTail = node;
    }

    // Actualizar memoria reservada
    gc.allocated += sizeInBytes;

    // Retornar puntero
    return node->pointer;
}

void gc_run()
{
    if(gc.debug & GC_DEBUG_RUN)
    {
        printf("GC: start run\n");
    }
    gc_mark_phase();
    gc_sweep_phase();
    if(gc.debug & GC_DEBUG_RUN)
    {
        printf("GC: finish run\n");
    }
}

void gc_clear_roots()
{
    GcRoot* root;
    GcRoot* next;
    for(root = gc.rootListHead; root != NULL; root = next)
    {
        next = root->next;

        if(root->prev)
            root->prev->next = root->next;
        if(root->next)
            root->next->prev = root->prev;
        if(gc.rootListHead == root)
            gc.rootListHead = root->next;
        if(gc.rootListTail == root)
            gc.rootListTail = root->prev;
            
        free(root);
    }
    gc.rootListHead = NULL;
    gc.rootListTail = NULL;
}

void gc_register_root(void* pointer)
{
    // Crear raíz
    GcRoot* root = malloc(sizeof(GcRoot));
    root->prev = NULL;
    root->next = NULL;
    root->pointer = pointer;
    
    // Enlazar raíz en la lista
    if(gc.rootListHead == NULL)
    {
        gc.rootListHead = root;
        gc.rootListTail = root;
    }
    else
    {
        root->prev = gc.rootListTail;
        gc.rootListTail->next = root;
        gc.rootListTail = root;
    }
}
