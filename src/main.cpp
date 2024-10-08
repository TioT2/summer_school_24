#include <stk.h>

int
main( void ) {
    StkStack stack;

    stkStackCtor(int, 0, stack);

    int temp = 0;

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
