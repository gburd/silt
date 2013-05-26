# SILT: A Memory-Efficient, High-Performance Key-Value Store #

**SILT** (Small Index Large Table) is a memory-efficient, highperformance
key-value store system based on flash storage that scales to serve billions of
key-value items on a single node. It requires only 0.7 bytes of DRAM per entry
and retrieves key/value pairs using on average 1.01 flash reads each. SILT
combines new algorithmic and systems techniques to balance the use of memory,
storage, and computation.

Our (Hyeontaek Lim, Bin Fan, and David G. Andersen from Carnegie Mellon
University in colaboration with Michael Kaminsky from Intel Labs) contributions
include: (1) the design of three basic key-value stores each with a different
emphasis on memory-efficiency and write-friendliness; (2) synthesis of the
basic key-value stores to build a SILT key-value store system; and (3) an
analytical model for tuning system parameters carefully to meet the needs of
different workloads. SILT requires one to two orders of magnitude less memory
to provide comparable throughput to current high-performance key-value systems
on a commodity desktop system with flash storage. [1]



[1] http://www.cs.cmu.edu/~dga/papers/silt-sosp2011.pdf
