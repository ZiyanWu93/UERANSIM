# Goal: Runtime Design and Implmentation

## steps

1. Implement `trigger_event` API: [Commit: 6291834]
   - Design the API to schedule events for processing.
   - Accept a payload (initially a simple number) as input.
   - Acquire a new event from the event pool.
   - Populate the event with the provided payload.
   - Enqueue the event into the mailbox for later retrieval.
   - Implement this functionality in `runtime.c`.
   - Todo: integrate event pool and mailbox into runtime.c

2. Add event pool so that runtime can support allocating new event and returning event to pool [Commit: 3a8b4b6]

3. Refactor the event pool to a separate module [Commit: d6aa86c]
    - Create a separate folder for event pool
    - Create a test file for event pool
    - Add a CMakeLists.txt file to the event pool folder
    - Add a CMakeLists.txt file to the UERANSIM root folder

4. Integrate Mailbox system for the runtime [Commit: ]