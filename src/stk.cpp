#include "stk.h"

void STK_API
stkLog( const char *fstr, ... ) {
    va_list args;

    va_start(args, fstr);
    vprintf(fstr, args); // TODO: stderr? Also, better logging system can be a feature.
    va_end(args);
} // stkLog function end

/// stack internal structure
typedef struct __StkStackImpl {
    size_t            elementSize; ///< size of single data entry // TODO: size_t for element size, isn't that wasteful?
    size_t            size;        ///< size (in entries)
    size_t            capacity;    ///< capacity (in entries)

#if STK_ENABLE_DEBUG_INFO
    StkStackDebugInfo debugInfo;   ///< debug info
#endif

    char              data[1];     ///< data buffer // TODO: alignment, be careful?
} StkStackImpl;

/// stack internal validation
typedef enum __StkStackImplStatus { // TODO: corruption status?
    STK_STACK_IMPL_STATUS_OK,                      ///< all's ok
    STK_STACK_IMPL_STATUS_NULL,                    ///< invalid
    STK_STACK_IMPL_STATUS_SIZE_MORE_THAN_CAPACITY, ///< size is more than capacity
    STK_STACK_IMPL_STATUS_INVALID_CAPACITY,        ///< just invalid capacity
} StkStackImplStatus;

// TODO: (don't do it if you don't have time) can you make macro named_enum(enum my_enum { a, b, c })
//       so that named_enum_get_name(my_enum, expression that contains value (a)) // => "a"
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
    // TODO: assert non-null
    stkLog("\"%s\" of %s constructed at %s:%d",
        dbgInfo->variableName,
        dbgInfo->elemTypeName,
        dbgInfo->fileName,
        dbgInfo->line
    );
}
#endif


// Stack validation function // TODO: less of a macro, more of a function
// TODO: do { ... } while(0) // TODO: naming (e.g. propagate_stack_error)
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
stkIsPowerOfTwo( const size_t n ) { // TODO: static without the prefix?
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
// TODO: ^ english C3

    // TODO: You can do faster, like so:
    // -- number;
    // number |= number >> 1;
    // number |= number >> 2;
    // number |= number >> 4;
    // number |= number >> 8;
    // number |= number >> 16;
    // ++ number;

    size_t i = 0;
    while (number != 0) {
        number >>= 1;
        i += 1;
    }
    return (size_t)1 << (i + 1);
} // stkRoundPow2 function end // TODO: Why? I mean, you are wrong even.

static StkStackImpl *
stkStackImplRealloc( StkStackImpl *stk, size_t elemSize, size_t capacity ) {
    return (StkStackImpl *)realloc(stk, sizeof(StkStackImpl) + elemSize * capacity); // TODO: extra 1 byte.
    // TODO: zero it out?
} // stkStackImplRealloc function end

static StkStatus
stkStackExtend( StkStackImpl **stk ) {
    assert(stk != NULL);

    StkStackImpl *imp = *stk;

    assert(imp != NULL); // TODO: double check
    STK_VALIDATE_STACK(imp);

    size_t newCapacity = imp->capacity * 2;
    if (newCapacity == 0) // TODO: How is this possible, fix in constructor?
        newCapacity = 1;
    // reallocate stack
    if ((imp /* TODO: Were is that imp? */= stkStackImplRealloc(imp, imp->elementSize, newCapacity)) == NULL)
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
    // TODO: poison shrunk part?
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
    memset(stk, 0, sizeof(StkStackImpl) + elementSize * capacity); // TODO: too early, what if stk is NULL?

    // check allocation
    if (stk == NULL)
        return STK_STATUS_BAD_ALLOC;

    // fill stk data
    stk->capacity = capacity; // TODO: designated initializers?
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
    StkStack *const   dst // TODO: the only shortened name? 
) {
    StkStatus status;
    if ((status = __stkStackCtor(elementSize, initialCapacity, dst)) != STK_STATUS_OK) // TODO: macro?
        return status;
    (*dst)->debugInfo = debugInfo;
    return STK_STATUS_OK;
    assert(dst != NULL); // TODO: Task: find function arguments when this is reachable. Do your worst.
} // __SstkStackCtor function end // TODO: Ss?

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
        if (status != STK_STATUS_OK) // TODO: and again
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
        return STK_STATUS_POP_ERROR; // TODO: this is an entirely different kind of error

    imp->size -= 1;

    if (imp->elementSize != 0)
        memcpy(dst, imp->data + imp->size * imp->elementSize, imp->elementSize);

    if (imp->size < imp->capacity / 4) {
        StkStatus status = stkStackShrink(&imp);
        if (status != STK_STACK_IMPL_STATUS_OK) // TODO: fix the warning
            return status;
        *stk = imp;
    }

    STK_VALIDATE_STACK(imp);

    return STK_STATUS_OK;
} // stkStackPop function end

StkStatus STK_API
stkStackDtor( StkStack stk ) {
    STK_VALIDATE_STACK(stk);
    free(stk); // TODO: idempotent destructor? free(stk), stk = {};
    return STK_STATUS_OK;
} // stkStackDtor function end

// stk.cpp file end
