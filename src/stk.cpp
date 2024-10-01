/**
 * @brief stack core implementation file
 */

#include "stk.h"

void
stkLog( const char *fstr, ... ) {
    va_list args;

    va_start(args, fstr);
    vfprintf(stderr, fstr, args); // TODO: better logging system can be a feature.
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

    uint64_t          data[1];     ///< data buffer
} StkStackImpl;

/// stack corruption
typedef enum __StkStackCorruption {
    STK_STACK_CORRUPTION_NOTHING,                 ///< all's ok

    STK_STACK_CORRUPTION_NULL,                    ///< invalid
    STK_STACK_CORRUPTION_SIZE_MORE_THAN_CAPACITY, ///< size is more than capacity
    STK_STACK_CORRUPTION_INVALID_CAPACITY,        ///< just invalid (non-power-of-2) capacity
} StkStackCorruption;

// TODO: (don't do it if you don't have time) can you make macro named_enum(enum my_enum { a, b, c })
//       so that named_enum_get_name(my_enum, expression that contains value (a)) // => "a"
const char *
stkStackImplStatusStr( StkStackCorruption status ) {
#define ELEM_(c) case c: return #c
    switch (status) {
    ELEM_(STK_STACK_CORRUPTION_NOTHING                );
    ELEM_(STK_STACK_CORRUPTION_NULL                   );
    ELEM_(STK_STACK_CORRUPTION_SIZE_MORE_THAN_CAPACITY);
    ELEM_(STK_STACK_CORRUPTION_INVALID_CAPACITY       );
    }

    return "unknown";
#undef ELEM_
} // stkStackImplStatusStr function end

/**
 * @brief stack debug info logging function
 * 
 * @param[in] dbgInfo debug info to log
 */
static void
stkLogDebugInfo( const StkStackDebugInfo *const dbgInfo ) {
    assert(dbgInfo != NULL);
    stkLog("\"%s\" of %s constructed at %s:%d",
        dbgInfo->variableName,
        dbgInfo->elemTypeName,
        dbgInfo->fileName,
        dbgInfo->line
    );
} // stkLogDebugInfo function end

// stack validation function
#define STK_PROPAGATE_STACK_ERROR(stk)                            \
    do {                                                          \
        StkStackCorruption status = stkStackCheckCorruption(stk); \
        if (status != STK_STACK_CORRUPTION_NOTHING) {             \
            stkLog("STK VALIDATION ERROR. DUMP: ");               \
            stkLogStack(stk);                                     \
            return STK_STATUS_CORRUPTED;                          \
        }                                                         \
    } while (false)

#define STK_PROPAGATE_BAD_ALLOC(expr)    \
    do {                                 \
        if ((expr) == NULL)              \
            return STK_STATUS_BAD_ALLOC; \
    } while (false)


// error propagation macro
#define STK_PROPAGATE_ERROR_STATUS(operation)  \
    do {                                       \
        StkStatus status = (operation);        \
        if (status != STK_STATUS_OK)           \
            return status;                     \
    } while (false)

/**
 * @brief stack logging function
 * 
 * @param[in] stk stack pointer
 */
static void
stkLogStack( const StkStackImpl *stk ) {
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
} // stkLogStack function end

/**
 * @brief number checking for power-of-two or zero function
 * 
 * @param[in] n number to check
 * 
 * @return true if is power of two or zero, false otherwise
 */
static inline bool
stkIsPowerOfTwo( const size_t n ) {
    return (n & (n - 1)) == 0;
} // stkIsPowerOfTwo function end

/**
 * @brief stack for corruption checking function
 * 
 * @param[in] stack stack to check corruption of
 * 
 * @return stack corruption status
 */
static StkStackCorruption
stkStackCheckCorruption( const StkStackImpl *stk ) {
    if (stk == NULL)
        return STK_STACK_CORRUPTION_NULL;
    if (!stkIsPowerOfTwo(stk->capacity))
        return STK_STACK_CORRUPTION_INVALID_CAPACITY;
    if (stk->capacity < stk->size)
        return STK_STACK_CORRUPTION_SIZE_MORE_THAN_CAPACITY;
    return STK_STACK_CORRUPTION_NOTHING;
} // stkStackCheckCorruption function end

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

    --number;
    number |= number >> 1;
    number |= number >> 2;
    number |= number >> 4;
    number |= number >> 8;
    number |= number >> 16;
    ++number;

    return number;
} // stkRoundCapacity function end

/**
 * @brief stack reallocation function
 * 
 * @param[in,out] stk      stack to reallocate pointer (nullable)
 * @param[in]     elemSize single stack element size
 * @param[in]     capacity new stack capacity
 * 
 * @return reallocated stack pointer (may be null)
 */
static StkStackImpl *
stkStackImplRealloc( StkStackImpl *stk, size_t elemSize, size_t capacity ) {
    return (StkStackImpl *)realloc(stk, sizeof(StkStackImpl) + elemSize * capacity - 1);
} // stkStackImplRealloc function end

/**
 * @brief stack capacity extension function
 * 
 * @param[in,out] stk stack to extend
 * 
 * @return operation status
 */
static StkStatus
stkStackExtend( StkStackImpl **stk ) {
    assert(stk != NULL);

    StkStackImpl *imp = *stk;

    STK_PROPAGATE_STACK_ERROR(imp);

    size_t newCapacity = imp->capacity * 2;
    if (newCapacity == 0)
        newCapacity = 1;

    // reallocate stack and propagate bad_alloc if reallocation failed
    STK_PROPAGATE_BAD_ALLOC(imp = stkStackImplRealloc(imp, imp->elementSize, newCapacity));

    // zero allocated memory out
    memset(imp->data + imp->capacity * imp->elementSize, 0, imp->capacity * imp->elementSize);
    imp->capacity = newCapacity;

    *stk = imp;

    return STK_STATUS_OK;
} // stkStackExtend function end

/**
 * @brief stack shrinking function
 * 
 * @param[in,out] stk stack to shrink
 * 
 * @note stack size should be 4 times less than stack capacity for reallocation happend
 * 
 * @return operation status
 */
static StkStatus
stkStackShrink( StkStackImpl **stk ) {
    assert(stk != NULL);

    StkStackImpl *imp = *stk;
    assert(imp != NULL);
    assert(imp->capacity > imp->size * 4);

    size_t newCapacity = imp->capacity / 4;
    STK_PROPAGATE_BAD_ALLOC(imp = stkStackImplRealloc(imp, imp->elementSize, newCapacity));
    // TODO: poison shrunk part?
    imp->capacity = newCapacity;

    *stk = imp;

    return STK_STATUS_OK;
} // stkStackShrink function end

StkStatus
__stkStackCtor( const size_t elementSize, const size_t initialCapacity, StkStack *const dst ) {
    assert(dst != NULL);

    // calculate power-of-two capacity
    const size_t capacity = stkRoundCapacity(initialCapacity);
    StkStackImpl *stk;

    // check allocation
    STK_PROPAGATE_BAD_ALLOC(stk = stkStackImplRealloc(NULL, elementSize, capacity));

    memset(stk, 0, sizeof(StkStackImpl) + elementSize * capacity);

    *stk = {
        .elementSize = elementSize,
        .size = 0,
        .capacity = capacity,
    };

    STK_PROPAGATE_STACK_ERROR(stk);

    *dst = stk;

    return STK_STATUS_OK;
} // __SstkStackCtor function end

StkStatus
__stkStackCtorDbg(
    const size_t      elementSize,
    const size_t      initialCapacity,
    StkStackDebugInfo debugInfo,
    StkStack *const   dst
) {
    assert(dst != NULL);

    STK_PROPAGATE_ERROR_STATUS(__stkStackCtor(elementSize, initialCapacity, dst));

#if STK_ENABLE_DEBUG_INFO
    (*dst)->debugInfo = debugInfo;
#endif

    return STK_STATUS_OK;
} // __stkStackCtor function end

StkStatus
stkStackPush( StkStack *const stk, const void *const src ) {
    assert(stk != NULL);
    assert(src != NULL);

    StkStackImpl *imp = *stk;

    STK_PROPAGATE_STACK_ERROR(imp);

    // extend stack if needed
    if (imp->capacity == imp->size) {
        STK_PROPAGATE_ERROR_STATUS(stkStackExtend(&imp));
        *stk = imp;
    }

    // actually, copy element
    if (imp->elementSize != 0)
        memcpy(imp->data + imp->size * imp->elementSize, src, imp->elementSize);
    imp->size += 1;

    STK_PROPAGATE_STACK_ERROR(imp);

    return STK_STATUS_OK;
} // stkStackPush function end

StkStatus
stkStackPop( StkStack *const stk, void *dst ) {
    assert(stk != NULL);

    StkStackImpl *imp = *stk;

    assert(imp != NULL);

    STK_PROPAGATE_STACK_ERROR(imp);

    if (imp->size == 0)
        return STK_STATUS_POP_ERROR; // TODO: this is an entirely different kind of error

    imp->size -= 1;

    if (dst != NULL && imp->elementSize != 0)
        memcpy(dst, imp->data + imp->size * imp->elementSize, imp->elementSize);

    if (imp->size < imp->capacity / 4) {
        STK_PROPAGATE_ERROR_STATUS(stkStackShrink(&imp));
        *stk = imp;
    }

    STK_PROPAGATE_STACK_ERROR(imp);

    return STK_STATUS_OK;
} // stkStackPop function end

StkStatus
stkStackDtor( StkStack stk ) {
    STK_PROPAGATE_STACK_ERROR(stk);
    free(stk); // TODO: idempotent destructor? free(stk), stk = {};
    return STK_STATUS_OK;
} // stkStackDtor function end

// stk.cpp file end
