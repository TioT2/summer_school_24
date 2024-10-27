#include "stk_memory.h"

#include <assert.h>
#include <stdio.h>

#ifdef STK_PLATFORM_WIN32

#include <windows.h>

bool
stkGetMemoryAccessParameters( const void *memory, size_t regionSize, StkMemoryAccess *dst ) {
    MEMORY_BASIC_INFORMATION info = {0};
    SYSTEM_INFO systemInfo = {0};

    GetNativeSystemInfo(&systemInfo);

    if (memory > systemInfo.lpMaximumApplicationAddress) {
        *dst = 0;
        return true;
    }

    if (0 == VirtualQuery(memory, &info, sizeof(info))) {
        DWORD error = GetLastError();
        CHAR *buffer = NULL;

        FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            error,
            MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
            (LPSTR)&buffer,
            0,
            NULL
        );

        if (buffer != NULL) {
            printf("MEMORY ACCESS CHECK ERROR: %s\n", buffer);
            LocalFree(buffer);
        }

        return false;
    }

    StkMemoryAccess res = 0;

    if ((info.AllocationProtect & (PAGE_EXECUTE_WRITECOPY | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_READONLY | PAGE_READWRITE)) != 0)
        res |= STK_MEMORY_ACCESS_READ;
    if ((info.AllocationProtect & (PAGE_EXECUTE_WRITECOPY | PAGE_EXECUTE_READWRITE | PAGE_READWRITE | PAGE_WRITECOPY)) != 0)
        res |= STK_MEMORY_ACCESS_WRITE;

    *dst = res;
    return true;
} // stkGetMemoryAccessParameters function end

#else

bool
stkGetMemoryAccessParameters( const void *memory, size_t regionSize, StkMemoryAccess *dst ) {
    assert(dst != NULL);
    return false;
} // stkGetMemoryAccessParameters function end

#endif

// stk_memory.cpp file end
