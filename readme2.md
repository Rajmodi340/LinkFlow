## Concurrency Model (Phase 3.1)

In the current implementation, the system assumes a single-threaded
execution model due to compiler and environment constraints.

Thread safety considerations:
- UrlRepository and LRUCache are shared resources.
- In a multi-threaded environment, these components would require
  synchronization using mutexes or read-write locks.
- This is intentionally deferred and documented as a design decision.



## Phase 3.2 – Load Simulation

To simulate concurrent-like behavior without threads,
multiple rapid redirect calls were executed.

Observations:
- Frequently accessed URLs stayed in cache.
- Rarely accessed URLs were evicted due to LRU policy.
- Cache significantly optimizes redirect performance.




## Phase 3.3 – Scale Considerations

Current limitations at scale:
- Single-node deployment
- Centralized ID generation
- In-memory storage constraints

Planned scaling strategies:
- Distributed ID generation
- Sharded storage
- Multi-node cache deployment

Consistency trade-off:
- Availability is prioritized over strong consistency,
  as redirects must remain highly available.
