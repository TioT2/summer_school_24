#include "stk.h"

/**
 * @brief Main project function
 * 
 * @return return function
 */
int
main( void ) {
    StkStack stack = NULL;

    stkStackCtor(int, 0, stack);

    int t = 0;

    t = 3; stkStackPush(&stack, &t);
    t = 1; stkStackPush(&stack, &t);
    t = 2; stkStackPush(&stack, &t);
    t = 4; stkStackPush(&stack, &t);
    t = 5; stkStackPush(&stack, &t);

    StkStatus status;
    while (STK_TRUE) {
        if ((status = stkStackPop(&stack, &t)) != STK_STATUS_OK)
            break;
        printf("%d\n", t);
    }

    // produce error
    *((int *)stack + 2) = 47;
    stkStackDtor(stack);

    return 0;
} // main function end

// main.cpp function end
