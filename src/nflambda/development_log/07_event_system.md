# Goal: Event System Design and Implementation

## Steps

1. [X] Adjust the current event_pool module into event_system module (continuing from "3. Refactor the event pool to a separate module [Commit: d6aa86c]) in development_log/04_runtime_design.md. [Commit: You should commit this step]
   - [X] rename event_pool folder to event_system
   - [X] move event.h and event.c to event_system folder
   - [X] update CMakeLists.txt in the root folder
   - [X] update CMakeLists.txt in the event_system folder
   - [X] update runtime.c to use event_system