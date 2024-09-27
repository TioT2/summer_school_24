#include "stk.h"

/**
 * @brief Main project function // TODO: This is useless
 * 
 * @return return function // TODO: balderdash
 */
int
main( void ) {
    StkStack stack = NULL;

    stkStackCtor(int, 0, stack);

    int t = 0; // TODO: t? not convinced

    t = 3; stkStackPush(&stack, &t);
    t = 1; stkStackPush(&stack, &t);
    t = 2; stkStackPush(&stack, &t);
    t = 4; stkStackPush(&stack, &t);
    t = 5; stkStackPush(&stack, &t);

    StkStatus status;
    while (STK_TRUE) {
        if ((status = stkStackPop(&stack, &t)) != STK_STATUS_OK) // TODO: is_successful?
            break;
        printf("%d\n", t);
    }

    // produce error
    *((int *)stack + 2) = 47;
    stkStackDtor(stack);

    return 0;
} // main function end

// main.cpp function end
