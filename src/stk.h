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

/// Boolean type definition
typedef int StkBool;

/// TRUE flag
#define STK_TRUE  1

/// FALSE flag
#define STK_FALSE 0

/// API call convention
#define STK_API __cdecl

#ifdef _DEBUG
#define STK_ENABLE_DEBUG_INFO 1
#else
#define STK_ENABLE_DEBUG_INFO 0
#endif

/**
 * @brief logging function
 * 
 * @param fstr format strnig
 * @param ...  format args
 */
void STK_API
stkLog( const char *fstr, ... );

/// Stack implementation
typedef struct __StkStackImpl *StkStack;

/// stack-related operation status representation structure
typedef enum __StkStatus {
    STK_STATUS_OK,            ///< all's ok
    STK_STATUS_BAD_ALLOC,     ///< allocation error (no memory)
    STK_STATUS_INVALID_STACK, ///< invalid stack (null/...)
    STK_STATUS_POP_ERROR,     ///< no elements to pop
} StkStatus;

#if STK_ENABLE_DEBUG_INFO

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
 * @param elementSize     size of single element
 * @param initialCapacity initial capacity of stack
 * @param dst             destinaiton stack
 * 
 * @note this function should not be called manually
 * 
 * @return stack construction status
 */
StkStatus STK_API
__stkStackCtorDbg( size_t elementSize, size_t initialCapacity, StkStackDebugInfo dbgInfo, StkStack *dst );

/**
 * @brief stack constructor
 * 
 * @param type            type of stack value
 * @param initialCapacity stack initial capacity
 * @param stack           stack (must be lvalue)
 * 
 * @return operation status
 */
#define stkStackCtor(type, initialCapacity, stack) (__stkStackCtorDbg(sizeof(type), (initialCapacity), stkBuildStackDebugInfo(#stack, #type, __FILE__, __LINE__), &stack))

#else

/**
 * @brief stack constructor 'implementation' function
 * 
 * @param elementSize     size of single element
 * @param initialCapacity initial capacity of stack
 * @param dst             destinaiton stack
 * 
 * @note this function should not be called manually
 * 
 * @return stack construction status
 */
StkStatus STK_API
__stkStackCtor( size_t elementSize, size_t initialCapacity, StkStack *dst );

/**
 * @brief stack constructor
 * 
 * @param type            type of stack value
 * @param initialCapacity stack initial capacity
 * @param stack           stack (must be lvalue)
 * 
 * @return operation status
 */
#define stkStackCtor(type, initialCapacity, stack) (__stkStackCtor(sizeof(type), (initialCapacity), &stack))

#endif

/**
 * @brief stack destructor
 * 
 * @param stk stack to destroy
 */
StkStatus STK_API
stkStackDtor( StkStack stk );

/**
 * @brief stack value pushing function
 * 
 * @param stk stack to push value to
 * @param src data to push
 * 
 * @note src should have at least [elementSize] bytes of readable space
 * 
 * @return operation status
 */
StkStatus STK_API
stkStackPush( StkStack *stk, const void *src );

/**
 * @brief stack value popping function
 * 
 * @param stk stack to pop value from
 * @param dst buffer to write to
 * 
 * @note dst should have at least [elementSize] bytes of writable space
 * 
 * @return operation status
 */
StkStatus STK_API
stkStackPop( StkStack *stk, void *dst );

#endif // !defiend(STK_H_)

// stk.h file end
