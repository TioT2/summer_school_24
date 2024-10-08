/**
 * @brief stack core implementation file
 */

#include "stk.h"

#if defined(STK_ENABLE_CANARIES) || defined(STK_ENABLE_HANDLE_ENCRYPTION)
    #define STK_ENABLE_DEBUG_CONTEXT
#endif

/// @brief debug data holder
static struct __StkDebugContext {
    bool     isInitialized;  ///< debug context initialization flag
    uint32_t headingCanary;  ///< heading canary value
    uint32_t trailingCanary; ///< trailing canary value
    uint32_t dataCanary;     ///< data block canary value
    size_t   handleMask;     ///< pointer mask
} STK_debugContext = { .isInitialized = false };

/**
 * @brief debug context initialization function
 */
static void
stkInitDebugContext( void ) {
    if (!STK_debugContext.isInitialized) {
        srand(time(NULL));

        STK_debugContext.isInitialized  = true;
        STK_debugContext.headingCanary  = ((uint32_t)rand() << 16) | ((uint32_t)rand() <<  0);
        STK_debugContext.trailingCanary = ((uint32_t)rand() << 16) | ((uint32_t)rand() <<  0);
        STK_debugContext.dataCanary     = ((uint32_t)rand() << 16) | ((uint32_t)rand() <<  0);
        STK_debugContext.handleMask     = ((uint64_t)rand() << 48) | ((uint64_t)rand() << 32) | ((uint64_t)rand() << 16) | ((uint64_t)rand() <<  0);

        // to prevent any rand algorithm exploiting
        srand(time(NULL));
    }
} // stkInitDebugContext function end

/// debug context initialization guard
#ifdef STK_ENABLE_DEBUG_CONTEXT
    #define STK_DEBUG_CONTEXT_GUARD do { stkInitDebugContext(); } while (false)
#else
    #define STK_DEBUG_CONTEXT_GUARD do {  } while (false)
#endif // defined(STK_ENABLE_DEBUG_CONTEXT)

void
stkLog( const char *fstr, ... ) {
    va_list args;

    va_start(args, fstr);
    vfprintf(stderr, fstr, args); // TODO: better logging system can be a feature.
    va_end(args);
} // stkLog function end

/**
 * @brief 32-bit memset function
 * 
 * @param[out] memory memory block to fill
 * @param[in]  value  value to fill block by
 * @param[in]  count  count of values to write to memory
 */
static void
stkFillMemory32( void *const memory, const uint32_t value, const size_t count ) {
    // Check memory alignment
    assert((size_t)memory % 4 == 0);

    uint32_t *ptr = (uint32_t *)memory;
    const uint32_t *const end = ptr + count;

    while (ptr != end)
        *ptr++ = value;
} // stkSetMemory32 function end

/// @brief count of canaries
#define STK_CANARY_COUNT 8

/// stack internal structure
typedef struct __StkStackImpl {
#ifdef STK_ENABLE_CANARIES
    uint32_t          __headingCanaries[STK_CANARY_COUNT];  ///< heading canaries
#endif

#ifdef STK_ENABLE_HASHING
    void *            __hashThis;                           ///< this pointer
    StkHash           __hash;                               ///< stack hash
#endif

    size_t            elementSize;                          ///< size of single data entry // TODO: size_t for element size, isn't that wasteful?
    size_t            size;                                 ///< size (in entries)
    size_t            capacity;                             ///< capacity (in entries)

#ifdef STK_ENABLE_DEBUG_INFO
    StkStackDebugInfo __debugInfo;                          ///< debug info
#endif

#ifdef STK_ENABLE_CANARIES
    uint32_t          __trailingCanaries[STK_CANARY_COUNT]; ///< canaries at end of stack head
#endif

    uint64_t          data[1];                              ///< data buffer
} StkStackImpl;

/**
 * @brief stack handle getting function
 * 
 * @param stack stack to get handle of
 * 
 * @return stack handle
 */
inline static StkStack
stkStackGetHandle( StkStackImpl *stack ) {
#ifdef STK_ENABLE_HANDLE_ENCRYPTION
    return (StkStack)((size_t)stack ^ STK_debugContext.handleMask);
#else
    return (StkStack)stack;
#endif
} // stkStackGetHandle function end

/**
 * @brief stack pointer from handle getting function
 * 
 * @param handle handle to get corresponding stack for
 * 
 * @return stack pointer
 */
inline static StkStackImpl *
stkStackFromHandle( StkStack handle ) {
#ifdef STK_ENABLE_HANDLE_ENCRYPTION
    return (StkStackImpl *)(handle ^ STK_debugContext.handleMask);
#else
    return (StkStackImpl *)handle;
#endif
} // stkStackFromHandle function end

/// 32-bit allocation poison value
const uint32_t STK_ALLOCATION_POISON32 = (uint32_t)0xDEDC0DE;

/**
 * @brief stack poisoning function
 * 
 * @param stack stack to poison pointer (stack is assumed to be valid)
 */
static void
stkStackPoison( StkStackImpl *const stk ) {
    assert(stk != NULL);

    size_t size = (sizeof(StkStack) + stk->capacity * stk->elementSize - sizeof(stk->data)) / 4;

    stkFillMemory32(stk->data, STK_ALLOCATION_POISON32, size);
} // stkStackPoison function end

#ifdef STK_ENABLE_HASHING
/**
 * @brief stack hash recalculation function
 * 
 * @param stk stack to recalculate hash for pointer
 */
static void
stkStackRecalculateHash( StkStackImpl *const stk ) {
    assert(stk != NULL);

    memset(&stk->__hash, 0, sizeof(StkHash));
    stk->__hashThis = stk;
    stk->__hash = stkHash(stk, sizeof(StkStackImpl));
} // stkStackRecalculateHash function end

/**
 * @brief stack hash validness checking function
 * 
 * @param[in] stk stack to check hash of
 * 
 * @return true if hash is valid, false otehrwise
 */
static bool
stkStackCheckHash( const StkStackImpl *const stk ) {
    assert(stk != NULL);

    StkHash oldHash = stk->__hash;

    // a bit of 'unsafe' core here...
    memset((StkStackImpl *)&stk->__hash, 0, sizeof(StkHash));
    StkHash newHash = stkHash(stk, sizeof(StkStackImpl));
    ((StkStackImpl *)stk)->__hash = oldHash;

    return stkHashCompare(&oldHash, &newHash);
} // stkStackCheckHash function end

#endif

/// stack corruption
typedef enum __StkStackCorruption {
    STK_STACK_CORRUPTION_NULL                      = 0x00000001, ///< invalid
    STK_STACK_CORRUPTION_SIZE_MORE_THAN_CAPACITY   = 0x00000002, ///< size is more than capacity
    STK_STACK_CORRUPTION_INVALID_CAPACITY          = 0x00000004, ///< just invalid (non-power-of-2) capacity
    STK_STACK_CORRUPTION_POISONED                  = 0x00000008, ///< given stack is poisoned (e.g. was passed through stkStackRealloc function)

    STK_STACK_CORRUPTION_DAMAGED_HASH              = 0x00000010, ///< damaged hash value
    STK_STACK_CORRUPTION_DAMAGED_LEFT_CANARY       = 0x00000020, ///< damaged left canary
    STK_STACK_CORRUPTION_DAMAGED_RIGHT_CANARY      = 0x00000040, ///< damaged right canary
    STK_STACK_CORRUPTION_DAMAGED_DATA_CANARY       = 0x00000080, ///< damaged canary located after data block
} StkStackCorruptionBit;

/// @brief corruption
typedef uint32_t StkStackCorruption;

/// @brief No stack corruption
const uint32_t STK_STACK_CORRUPTION_NOTHING = (StkStackCorruption)0;

// TODO: (don't do it if you don't have time) can you make macro named_enum(enum my_enum { a, b, c })
//       so that named_enum_get_name(my_enum, expression that contains value (a)) // => "a"
static void
stkLogStackCorruption( StkStackCorruption corruption ) {
    stkLog("|");
    if (corruption & STK_STACK_CORRUPTION_NULL                     ) stkLog(" invalid |");
    if (corruption & STK_STACK_CORRUPTION_SIZE_MORE_THAN_CAPACITY  ) stkLog(" size is more than capacity |");
    if (corruption & STK_STACK_CORRUPTION_INVALID_CAPACITY         ) stkLog(" just invalid (non-power-of-2) capacity |");
    if (corruption & STK_STACK_CORRUPTION_POISONED                 ) stkLog(" poisoned (e.g. passed through stkReallocStack function) |");
    if (corruption & STK_STACK_CORRUPTION_DAMAGED_HASH             ) stkLog(" invalid hash value |");
    if (corruption & STK_STACK_CORRUPTION_DAMAGED_LEFT_CANARY      ) stkLog(" damaged left canary |");
    if (corruption & STK_STACK_CORRUPTION_DAMAGED_RIGHT_CANARY     ) stkLog(" damaged right canary |");
    if (corruption & STK_STACK_CORRUPTION_DAMAGED_DATA_CANARY      ) stkLog(" damaged data canary |");
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

// stack validation macro
#ifdef STK_ENABLE_CORRUPTION_CHECKS
    #define STK_PROPAGATE_STACK_CORRUPTED(stk)                        \
        do {                                                          \
            StkStackCorruption status = stkStackCheckCorruption(stk); \
            if (status != STK_STACK_CORRUPTION_NOTHING) {             \
                stkLog("\nSTK VALIDATION ERROR.");                    \
                stkLog("\n    CORRUPTION STATUS: ");                  \
                stkLogStackCorruption(status);                        \
                stkLog("\n    DUMP: ");                               \
                stkLogStack(stk);                                     \
                return STK_STATUS_CORRUPTED;                          \
            }                                                         \
        } while (false)
#else
    #define STK_PROPAGATE_STACK_CORRUPTED(stk) do {  } while (false)
#endif

/// bad allocation error propagation macro
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

#ifdef STK_ENABLE_DEBUG_INFO
    stkLog("\ndebug info : ");
    stkLogDebugInfo(&stk->__debugInfo);
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
            stkLog("%02X", (unsigned int)(uint8_t)((const uint8_t *)stk->data)[elemIndex * stk->elementSize + byteIndex]);
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
 * @brief size alignment function
 * 
 * @param[in] size      size to align
 * @param[in] alignment alignment (non-zero)
 * 
 * @return size aligned to greater or equal
 */
static size_t
stkSizeAlignUp( const size_t size, const size_t alignment ) {
    assert(alignment > 0);
    if (size % alignment == 0)
        return size;
    return size + alignment - size % alignment;
} // stkSizeAlignUp function end

/**
 * @brief canary offset getting function
 * 
 * @param[in] elementSize stack element size
 * @param[in] capacity    stack capacity
 * 
 * @return offset of data canary in data array
 */
static size_t
stkStackGetDataCanaryOffset( size_t elementSize, size_t capacity ) {
    return stkSizeAlignUp(elementSize * capacity, sizeof(uint32_t));
} // stkStackGetDataCanaryOffset function end

/**
 * @brief stack for corruption checking function
 * 
 * @param[in] stack stack to check corruption of
 * 
 * @return stack corruption status
 */
static StkStackCorruption
stkStackCheckCorruption( const StkStackImpl *stk ) {
    StkStackCorruption corruption = STK_STACK_CORRUPTION_NOTHING;

    if (stk == NULL) {
        corruption |= STK_STACK_CORRUPTION_NULL;

        // there is no checks that possible for null pointer
        return corruption;
    }
    if (!stkIsPowerOfTwo(stk->capacity))
        corruption |= STK_STACK_CORRUPTION_INVALID_CAPACITY;
    if (stk->capacity < stk->size)
        corruption |= STK_STACK_CORRUPTION_SIZE_MORE_THAN_CAPACITY;

#ifdef STK_ENABLE_CANARIES
    struct __StkCanaryInfo {
        StkStackCorruptionBit  corruptionBit;
        uint32_t               expectedCanary;
        const uint32_t        *canaries;
    } canaryInfos[] = {
        {STK_STACK_CORRUPTION_DAMAGED_LEFT_CANARY,  STK_debugContext.headingCanary , stk->__headingCanaries},
        {STK_STACK_CORRUPTION_DAMAGED_RIGHT_CANARY, STK_debugContext.trailingCanary, stk->__trailingCanaries},
        {STK_STACK_CORRUPTION_DAMAGED_DATA_CANARY,  STK_debugContext.dataCanary    , (const uint32_t *)((const uint8_t *)stk->data + stkStackGetDataCanaryOffset(stk->elementSize, stk->capacity))},
    };

    for (uint32_t canaryInfoIndex = 0; canaryInfoIndex < sizeof(canaryInfos) / sizeof(canaryInfos[0]); canaryInfoIndex++) {
        struct __StkCanaryInfo *info = canaryInfos + canaryInfoIndex;
        bool doContinue = true;

        for (uint32_t i = 0; doContinue && i < STK_CANARY_COUNT; i++) {
            if (info->canaries[i] != info->expectedCanary) {
                corruption |= info->corruptionBit;
                doContinue = false;
            }
        }
    }
#endif

#ifdef STK_ENABLE_HASHING
    if (!stkStackCheckHash(stk))
        corruption |= STK_STACK_CORRUPTION_DAMAGED_HASH;
#endif

    return corruption;
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
 * @note also writes debug canary
 * 
 * @return reallocated stack pointer (may be null)
 */
static StkStackImpl *
stkStackRealloc( StkStackImpl *stk, size_t elemSize, size_t capacity ) {
    // align data block
    size_t dataBlockSize;
    if (capacity == 0)
        dataBlockSize = 0;
    else
        dataBlockSize = elemSize * capacity;

#ifdef STK_ENABLE_CANARIES
    dataBlockSize = stkSizeAlignUp(dataBlockSize, 4);
    dataBlockSize += STK_CANARY_COUNT * sizeof(uint32_t);
#endif

    StkStackImpl *impl = (StkStackImpl *)realloc(stk, sizeof(StkStackImpl) + dataBlockSize - sizeof(stk->data));

#ifdef STK_ENABLE_CANARIES
    {
        size_t offset = stkStackGetDataCanaryOffset(elemSize, capacity);

        uint32_t *canary = (uint32_t *)((uint8_t *)impl->data + offset);
        for (size_t i = 0; i < STK_CANARY_COUNT; i++)
            canary[i] = STK_debugContext.dataCanary;
    }
#endif

    return impl;
} // stkStackRealloc function end

/**
 * @brief stack capacity extension function
 * 
 * @param[in,out] stk stack to extend (non-null)
 * 
 * @return operation status
 */
static StkStatus
stkStackExtend( StkStackImpl **stk ) {
    assert(stk != NULL);

    StkStackImpl *imp = *stk;

    size_t newCapacity = imp->capacity * 2;
    if (newCapacity == 0)
        newCapacity = 1;

    // reallocate stack and propagate bad_alloc if reallocation failed
    STK_PROPAGATE_BAD_ALLOC(imp = stkStackRealloc(imp, imp->elementSize, newCapacity));

    // zero allocated memory out
    memset((uint8_t *)imp->data + imp->capacity * imp->elementSize, 0, imp->capacity * imp->elementSize);
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
    STK_PROPAGATE_BAD_ALLOC(imp = stkStackRealloc(imp, imp->elementSize, newCapacity));
    // TODO: poison shrunk part?
    imp->capacity = newCapacity;

    *stk = imp;

    return STK_STATUS_OK;
} // stkStackShrink function end

StkStatus
__stkStackCtor( const size_t elementSize, const size_t initialCapacity, StkStack *const dst ) {
    STK_DEBUG_CONTEXT_GUARD;

    assert(dst != NULL);

    // calculate power-of-two capacity
    const size_t capacity = stkRoundCapacity(initialCapacity);
    StkStackImpl *stk;

    // check allocation
    STK_PROPAGATE_BAD_ALLOC(stk = stkStackRealloc(NULL, elementSize, capacity));

    memset(stk, 0, sizeof(StkStackImpl) + elementSize * capacity - sizeof(stk->data));

    stk->elementSize = elementSize;
    stk->size = 0;
    stk->capacity = capacity;

    // fill 
#ifdef STK_ENABLE_CANARIES
    stkFillMemory32(&stk->__headingCanaries,  STK_debugContext.headingCanary,  8);
    stkFillMemory32(&stk->__trailingCanaries, STK_debugContext.trailingCanary, 8);
#endif

#ifdef STK_ENABLE_HASHING
    stkStackRecalculateHash(stk);
#endif

    STK_PROPAGATE_STACK_CORRUPTED(stk);

    *dst = stkStackGetHandle(stk);

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

    StkStackImpl *stack = stkStackFromHandle(*dst);

#ifdef STK_ENABLE_DEBUG_INFO
    stack->__debugInfo = debugInfo;
#endif

#ifdef STK_ENABLE_HASHING
    stkStackRecalculateHash(stack);
#endif

    return STK_STATUS_OK;
} // __stkStackCtor function end

StkStatus
stkStackPush( StkStack *const stk, const void *const src ) {
    STK_DEBUG_CONTEXT_GUARD;

    assert(stk != NULL);
    assert(src != NULL);

    StkStackImpl *imp = stkStackFromHandle(*stk);

    STK_PROPAGATE_STACK_CORRUPTED(imp);

    // extend stack if needed
    if (imp->capacity == imp->size) {
        STK_PROPAGATE_ERROR_STATUS(stkStackExtend(&imp));
        *stk = stkStackGetHandle(imp);
    }

    // actually, copy element
    if (imp->elementSize != 0)
        memcpy((uint8_t *)imp->data + imp->size * imp->elementSize, src, imp->elementSize);
    imp->size += 1;

#ifdef STK_ENABLE_HASHING
    stkStackRecalculateHash(imp);
#endif

    STK_PROPAGATE_STACK_CORRUPTED(imp);

    return STK_STATUS_OK;
} // stkStackPush function end

StkStatus
stkStackPop( StkStack *const stk, void *dst ) {
    STK_DEBUG_CONTEXT_GUARD;

    assert(stk != NULL);

    StkStackImpl *imp = stkStackFromHandle(*stk);

    assert(imp != NULL);

    STK_PROPAGATE_STACK_CORRUPTED(imp);

    if (imp->size == 0)
        return STK_STATUS_POP_ERROR; // TODO: this is an entirely different kind of error

    imp->size -= 1;

    if (dst != NULL && imp->elementSize != 0)
        memcpy(dst, (uint8_t *)imp->data + imp->size * imp->elementSize, imp->elementSize);

    if (imp->size < imp->capacity / 4) {
        STK_PROPAGATE_ERROR_STATUS(stkStackShrink(&imp));
        *stk = stkStackGetHandle(imp);
    }

#ifdef STK_ENABLE_HASHING
    stkStackRecalculateHash(imp);
#endif

    STK_PROPAGATE_STACK_CORRUPTED(imp);

    return STK_STATUS_OK;
} // stkStackPop function end

StkStatus
__stkStackDtor( StkStack *handle ) {
    STK_DEBUG_CONTEXT_GUARD;

    if (handle != 0) {
        StkStackImpl *stk = stkStackFromHandle(*handle);
        // it's not ok to destroy corrupted stack
        STK_PROPAGATE_STACK_CORRUPTED(stk);
        free(stk);
        *handle = 0;
    }

    return STK_STATUS_OK;
} // stkStackDtor function end

// stk.cpp file end
