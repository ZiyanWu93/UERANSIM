# Goal: Implement Mailbox and Event Scheduling System

## Overview

This document outlines the plan for implementing a mailbox-based event scheduling system within the UERANSIM project. The mailbox will serve as a simple FIFO queue for events, enabling communication between network functions in a single-threaded environment.

## Architectural Design

### Mailbox Structure

1. **Message Queue Design**
   - [X] rely on memory manager backend
   - [X] Implement a FIFO queue structure to store pending events
   - [X] Each queue entry will contain an EventNf structure with event_id and payloads
   - [X] Single-threaded access model simplifies implementation
   - [X] Consider using a circular buffer or linked list implementation for efficiency

2. **Memory Management**
   - [X] check [[3.md]]

3. **Mailbox Memory Management Integration**
   - [X] Modify mailbox.h to add memory allocator field to Mailbox struct
   - [X] Update mailbox_init to initialize a dedicated memory pool for EventNf objects
   - [X] Refactor mailbox_put to use memory allocator for event storage
   - [X] Update mailbox_pop and add mailbox_return_event to manage memory
   - [X] Implement mailbox_cleanup to free memory pool resources
   - [X] Move to a separate module as a folder, and update the CMakeLists.txt
