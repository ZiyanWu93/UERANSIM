# Goal

## Functions
- Implement an actor-based 5G Core Network
- potentially implement gNB as well

## Non-functional Goals
- Performance
- Efficiency
- Predictability

## Design and Features
- Event Driven Architecture that enables
    - cache-aware consolidation of network functions
    - optimal state exchange between network functions
        - remove HTTP/NGAP within the same core without sacrificing flexibility when deployed remotely
- Data-State Decoupling Runtime
    - optimal state access
        - cache-conscious data structure
        - zero-copy
        - pre-allocated memory
    - unified context matching