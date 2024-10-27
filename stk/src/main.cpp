#include <stk.h>

struct Stack {
    uint32_t          __headingCanaries[8];  ///< heading canaries
    void *            __hashThis;            ///< this pointer
    StkHash           __hash;                ///< stack hash
    size_t            elementSize;           ///< size of single data entry // TODO: size_t for element size, isn't that wasteful?
    size_t            size;                  ///< size (in entries)
    size_t            capacity;              ///< capacity (in entries)
    StkStackDebugInfo __debugInfo;           ///< debug info
    uint32_t          __trailingCanaries[8]; ///< canaries at end of stack head
    uint64_t          data[1];               ///< data buffer
};

int
main( void ) {
    StkStack stack;


    int temp = 0;

    srand(time(NULL));

    stkStackCtor(int, 0, stack);
    // {
    //     Stack *stk = (Stack *)stack;
    // 
    //     for (size_t i = 0; i < sizeof(Stack) / 2; i++) {
    //         uint16_t *ptr = (uint16_t *)stk + i;
    // 
    //         *ptr = rand();
    //     }
    // }

    // stack = ((uint64_t)rand() << 48) | ((uint64_t)rand() << 32) | ((uint64_t)rand() << 16) | ((uint64_t)rand() <<  0);

    temp = 3; stkStackPush(&stack, &temp);
    temp = 1; stkStackPush(&stack, &temp);
    temp = 2; stkStackPush(&stack, &temp);
    temp = 4; stkStackPush(&stack, &temp);
    temp = 5; stkStackPush(&stack, &temp);

    StkStatus status;
    while (true) {
        if ((status = stkStackPop(&stack, &temp)) != STK_STATUS_OK) // TODO: is_successful?
            break;
        printf("%d\n", temp);
    }

    stkStackDtor(stack);

    return 0;
} // main function end

// main.cpp function end
