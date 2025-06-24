/*
 * Runtime Main Entry Point
 * 
 * This file provides the main() function for the standalone runtime executable.
 * The runtime framework itself is in runtime.c and can be used as a library.
 */

#include <signal.h>
#include <stdio.h>
#include "runtime.h"

int main(int argc, char* argv[])
{
    printf("NFLambda Runtime - Standalone Mode\n");
    printf("Press Ctrl+C to exit\n\n");
    
    // Setup signal handler
    signal(SIGINT, handle_sigint);
    
    // Run the runtime
    runtime();
    
    return 0;
}