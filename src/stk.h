/**
 * @brief stack library
 */

#ifndef STK_H_
#define STK_H_

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef _DEBUG
// TODO: Why redefine a macro that is essentially equivalent to _DEBUG?
#define STK_ENABLE_DEBUG_INFO 1
#else
#define STK_ENABLE_DEBUG_INFO 0
#endif

/**
 * @brief to stdout logging function
 * 
 * @param fstr format strnig (non-null) (must be printf-compatible format string)
 * @param ...  format args (each argument needs to have corresponding %[] in format string, format is the same as in printf)
 */
void
stkLog( const char *fstr, ... );

/// Stack implementation
typedef struct __StkStackImpl *StkStack;

/// stack-related operation status
typedef enum __StkStatus {
    STK_STATUS_OK,        ///< all's ok
    STK_STATUS_BAD_ALLOC, ///< allocation error (no memory)
    STK_STATUS_CORRUPTED, ///< invalid stack (null/...)
    STK_STATUS_POP_ERROR, ///< no elements to pop
} StkStatus;


/// Debug info representation structure
typedef struct __StkStackDebugInfo {
    const char * variableName; ///< variable name
    const char * elemTypeName; ///< name of stack element type
    const char * fileName;     ///< create file name
    size_t       line;         ///< line number
} StkStackDebugInfo;

inline StkStackDebugInfo
stkBuildStackDebugInfo( const char *variableName, const char *elemTypeName, const char *fileName, size_t line ) {
    StkStackDebugInfo debugInfo = {
        .variableName = variableName,
        .elemTypeName = elemTypeName,
        .fileName = fileName,
        .line = line,
    };

    return debugInfo;
}

/**
 * @brief stack constructor 'implementation' function
 * 
 * @param[in]  elementSize     size of single element
 * @param[in]  initialCapacity initial capacity of stack
 * @param[out] dst             destinaiton stack
 * 
 * @note this function should not be called manually
 * 
 * @return stack construction status
 */
StkStatus
__stkStackCtor( size_t elementSize, size_t initialCapacity, StkStack *dst );

/**
 * @brief stack constructor 'implementation' function
 * 
 * @param[in]  elementSize     size of single element
 * @param[in]  initialCapacity initial capacity of stack
 * @param[out] dst             stack constructor destination
 * 
 * @note this function should not be called manually
 * 
 * @return stack construction status
 */
StkStatus
__stkStackCtorDbg( size_t elementSize, size_t initialCapacity, StkStackDebugInfo dbgInfo, StkStack *dst ); // TODO: Why have this under ifdef?

#if STK_ENABLE_DEBUG_INFO // TODO: try to make your ifdefs shorter, they are hard to read

/**
 * @brief stack constructor
 * 
 * @param[in]  type            type of stack value
 * @param[in]  initialCapacity stack initial capacity
 * @param[out] stack           stack (must be lvalue)
 * 
 * @return operation status
 */
#define stkStackCtor(type, initialCapacity, stack) (__stkStackCtorDbg(sizeof(type), (initialCapacity), stkBuildStackDebugInfo(#stack, #type, __FILE__, __LINE__), &stack))

#else

/**
 * @brief stack constructor
 * 
 * @param[in]  type            type of stack value
 * @param[in]  initialCapacity stack initial capacity
 * @param[out] stack           stack (must be lvalue)
 * 
 * @return operation status
 */
#define stkStackCtor(type, initialCapacity, stack) (__stkStackCtor(sizeof(type), (initialCapacity), &stack))

#endif

/**
 * @brief stack destructor
 * 
 * @param[in] stk stack to destroy
 */
StkStatus
stkStackDtor( StkStack stk );

/**
 * @brief stack value pushing function
 * 
 * @param[in,out] stk stack to push value to
 * @param[in]     src data to push
 * 
 * @note src should have at least [elementSize] bytes of readable space
 * 
 * @return operation status
 */
StkStatus
stkStackPush( StkStack *stk, const void *src );

/**
 * @brief stack value popping function
 * 
 * @param[in,out] stk stack to pop value from
 * @param[out]    dst buffer to write to (should have at least [elementSize] bytes of space available for write)
 * 
 * @return operation status
 */
StkStatus
stkStackPop( StkStack *stk, void *dst );

#endif // !defiend(STK_H_)

// stk.h file end
