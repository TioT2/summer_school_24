#include "stk.h"

void STK_API
stkLog( const char *fstr, ... ) {
    va_list args;

    va_start(args, fstr);
    vprintf(fstr, args);
    va_end(args);
} // stkLog function end

/// stack internal structure
typedef struct __StkStackImpl {
    size_t            elementSize; ///< size of single data entry
    size_t            size;        ///< size (in entries)
    size_t            capacity;    ///< capacity (in entries)

#if STK_ENABLE_DEBUG_INFO
    StkStackDebugInfo debugInfo;   ///< debug info
#endif

    char              data[1];     ///< data buffer
} StkStackImpl;

/// stack internal validation
typedef enum __StkStackImplStatus {
    STK_STACK_IMPL_STATUS_OK,                      ///< all's ok
    STK_STACK_IMPL_STATUS_NULL,                    ///< invalid
    STK_STACK_IMPL_STATUS_SIZE_MORE_THAN_CAPACITY, ///< size is more than capacity
    STK_STACK_IMPL_STATUS_INVALID_CAPACITY,        ///< just invalid capacity
} StkStackImplStatus;

const char *
stkStackImplStatusStr( StkStackImplStatus status ) {
#define ELEM_(c) case c: return #c
    switch (status) {
    ELEM_(STK_STACK_IMPL_STATUS_OK                     );
    ELEM_(STK_STACK_IMPL_STATUS_NULL                   );
    ELEM_(STK_STACK_IMPL_STATUS_SIZE_MORE_THAN_CAPACITY);
    ELEM_(STK_STACK_IMPL_STATUS_INVALID_CAPACITY       );
    }

    return "unknown";
#undef ELEM_
}

#if STK_ENABLE_DEBUG_INFO
static void
stkLogDebugInfo( const StkStackDebugInfo *const dbgInfo ) {
    stkLog("\"%s\" of %s constructed at %s:%d",
        dbgInfo->variableName,
        dbgInfo->elemTypeName,
        dbgInfo->fileName,
        dbgInfo->line
    );
}
#endif

// Stack validation function
#define STK_VALIDATE_STACK(stk)                                                  \
    {                                                                            \
        StkStackImplStatus status = stkStackValidate(stk);                       \
        if (status != STK_STACK_IMPL_STATUS_OK) {                                \
            stkLog("STK VALIDATION ERROR. DUMP: ");                              \
            stkLogStack(stk);                                                    \
            return STK_STATUS_INVALID_STACK;                                     \
        }                                                                        \
    }                                                      

static void
stkLogStack( StkStackImpl *stk ) {
    if (stk == NULL)
        stkLog("NULL");
    stkLog("\n[pointer]  : ");
    if (stk == NULL) {
        stkLog("NULL");
        return;
    }
    stkLog("%p", stk);
    stkLog("\ncapacity   : %d", stk->capacity);
    stkLog("\nelem size  : %d", stk->elementSize);
    stkLog("\nsize       : %d", stk->size);

#if STK_ENABLE_DEBUG_INFO
    stkLog("\ndebug info : ");
    stkLogDebugInfo(&stk->debugInfo);
#endif

    stkLog("\ndata       : [%d elements by %p address]", stk->size, stk->data);

    size_t printCount = stk->size;
    const size_t maxPrint = 32;

    if (printCount > maxPrint) {
        stkLog("\ndata elements (first %d from %d): \n", maxPrint, stk->size);
        printCount = maxPrint;
    } else {
        stkLog("\ndata elements :");
    }

    for (size_t elemIndex = 0; elemIndex < printCount; elemIndex++) {
        stkLog("\n%2d: ", elemIndex);
        for (size_t byteIndex = 0; byteIndex < stk->elementSize; byteIndex++)
            stkLog("%02X", (unsigned int)(unsigned char)stk->data[elemIndex * stk->elementSize + byteIndex]);
    }

    if (stk->size > maxPrint)
        stkLog("\n...\n");
}

static inline StkBool
stkIsPowerOfTwo( const size_t n ) {
    return (n & (n - 1)) == 0;
} // stkIsPowerOfTwo function end

static StkStackImplStatus
stkStackValidate( StkStackImpl *stk ) {
    if (stk == NULL)
        return STK_STACK_IMPL_STATUS_NULL;
    if (!stkIsPowerOfTwo(stk->capacity))
        return STK_STACK_IMPL_STATUS_INVALID_CAPACITY;
    if (stk->capacity < stk->size)
        return STK_STACK_IMPL_STATUS_SIZE_MORE_THAN_CAPACITY;
    return STK_STACK_IMPL_STATUS_OK;
} // stkStackValidate function end

/**
 * @brief to nearest power of two rounding function
 * 
 * @param number number to round
 * 
 * @return rounded number
 */
static size_t
stkRoundCapacity( size_t number ) {
    if (number == 0)
        return 0;

    // awaful, but portable


    size_t i = 0;
    while (number != 0) {
        number >>= 1;
        i += 1;
    }
    return (size_t)1 << (i + 1);
} // stkRoundPow2 function end

static StkStackImpl *
stkStackImplRealloc( StkStackImpl *stk, size_t elemSize, size_t capacity ) {
    return (StkStackImpl *)realloc(stk, sizeof(StkStackImpl) + elemSize * capacity);
} // stkStackImplRealloc function end

static StkStatus
stkStackExtend( StkStackImpl **stk ) {
    assert(stk != NULL);

    StkStackImpl *imp = *stk;

    assert(imp != NULL);
    STK_VALIDATE_STACK(imp);

    size_t newCapacity = imp->capacity * 2;
    if (newCapacity == 0)
        newCapacity = 1;
    // reallocate stack
    if ((imp = stkStackImplRealloc(imp, imp->elementSize, newCapacity)) == NULL)
        return STK_STATUS_BAD_ALLOC;
    memset(imp->data + imp->capacity * imp->elementSize, 0, imp->capacity * imp->elementSize);
    imp->capacity = newCapacity;

    *stk = imp;

    return STK_STATUS_OK;
} // stkStackExtend function end

static StkStatus
stkStackShrink( StkStackImpl **stk ) {
    assert(stk != NULL);

    StkStackImpl *imp = *stk;
    assert(imp != NULL);
    assert(imp->capacity > imp->size * 4);

    size_t newCapacity = imp->capacity / 4;
    if ((imp = stkStackImplRealloc(imp, imp->elementSize, newCapacity)) == NULL)
        return STK_STATUS_BAD_ALLOC;
    imp->capacity = newCapacity;

    *stk = imp;

    return STK_STATUS_OK;
} // stkStackShrink function end



StkStatus STK_API
__stkStackCtor( const size_t elementSize, const size_t initialCapacity, StkStack *const dst ) {
    assert(dst != NULL);

    // calculate power-of-two capacity
    const size_t capacity = stkRoundCapacity(initialCapacity);
    StkStackImpl *stk = stkStackImplRealloc(NULL, elementSize, capacity);
    // rebuild into calloc???
    memset(stk, 0, sizeof(StkStackImpl) + elementSize * capacity);

    // check allocation
    if (stk == NULL)
        return STK_STATUS_BAD_ALLOC;

    // fill stk data
    stk->capacity = capacity;
    stk->elementSize = elementSize;
    stk->size = 0;

    STK_VALIDATE_STACK(stk);

    *dst = stk;

    return STK_STATUS_OK;
} // __SstkStackCtor function end

#if STK_ENABLE_DEBUG_INFO

StkStatus STK_API
__stkStackCtorDbg(
    const size_t      elementSize,
    const size_t      initialCapacity,
    StkStackDebugInfo debugInfo,
    StkStack *const   dst
) {
    StkStatus status;
    if ((status = __stkStackCtor(elementSize, initialCapacity, dst)) != STK_STATUS_OK)
        return status;
    (*dst)->debugInfo = debugInfo;
    return STK_STATUS_OK;
    assert(dst != NULL);
} // __SstkStackCtor function end

#endif

StkStatus STK_API
stkStackPush( StkStack *const stk, const void *const src ) {
    assert(stk != NULL);
    assert(src != NULL);

    StkStackImpl *imp = *stk;
    STK_VALIDATE_STACK(imp);

    // extend stack if needed
    if (imp->capacity == imp->size) {
        StkStatus status = stkStackExtend(&imp);
        if (status != STK_STATUS_OK)
            return status;
        *stk = imp;
    }

    // actually, copy element
    if (imp->elementSize != 0)
        memcpy(imp->data + imp->size * imp->elementSize, src, imp->elementSize);
    imp->size += 1;

    STK_VALIDATE_STACK(imp);

    return STK_STATUS_OK;
} // stkStackPush function end

StkStatus STK_API
stkStackPop( StkStack *const stk, void *dst ) {
    assert(stk != NULL);
    assert(dst != NULL);

    StkStackImpl *imp = *stk;

    assert(imp != NULL);

    STK_VALIDATE_STACK(imp);

    if (imp->size == 0)
        return STK_STATUS_POP_ERROR;

    imp->size -= 1;

    if (imp->elementSize != 0)
        memcpy(dst, imp->data + imp->size * imp->elementSize, imp->elementSize);

    if (imp->size < imp->capacity / 4) {
        StkStatus status = stkStackShrink(&imp);
        if (status != STK_STACK_IMPL_STATUS_OK)
            return status;
        *stk = imp;
    }

    STK_VALIDATE_STACK(imp);

    return STK_STATUS_OK;
} // stkStackPop function end

StkStatus STK_API
stkStackDtor( StkStack stk ) {
    STK_VALIDATE_STACK(stk);
    free(stk);
    return STK_STATUS_OK;
} // stkStackDtor function end

// stk.cpp file end
