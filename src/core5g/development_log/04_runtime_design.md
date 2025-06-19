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
