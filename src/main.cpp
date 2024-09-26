#include "stk.h"

int
main( void ) {
    StkStack stack = NULL;

    stkStackCtor(int, 0, stack);

    int temp = 0; // TODO: t? not convinced

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

    // produce error
    *((int *)stack + 2) = 47;
    stkStackDtor(stack);

    return 0;
} // main function end

// main.cpp function end
