#ifndef APP_EXECUTOR_IMPL_H_
#define APP_EXECUTOR_IMPL_H_

#ifdef APP_H_
#error "app_executor_impl.h" FILE MUST NOT BE INCLUDED IN PROJCECT TREE
#endif

#include "../app.h"

#include <signal.h>
#include <stdlib.h>

//----------------------------------------------------------------
//! @brief signal handler setting up function
//!
//! @param [in] hStdin  standard input handle
//! @apram [in] hStdout standard output handle
//----------------------------------------------------------------
void
appExecutorSetupSignals( HANDLE hStdin, HANDLE hStdout );

#endif

// app_executor_impl.h function end