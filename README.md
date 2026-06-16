# DSA_Final_Project
# ReadPass: Digital Content Publishing Paywall Router

ReadPass is an optimized, high-performance console routing engine designed for digital publishing platforms to manage subscriber permissions, chronological clickstreams, targeted pricing models, and content discovery mechanics in real time. 

Developed as a Semester-II B.Tech Computer Science & Engineering Data Structures and Algorithms (DSA) project, this system replaces traditional monolithic lookup workflows with a multi-data-structure ("polyglot") architectural layout in native C++17.

---

## 🚀 Key System Features & Core DSA Mapping

ReadPass decouples its individual administrative duties by assigning each feature to the specific data structure that optimizes its mathematical runtime footprint.

| Module Component | Feature Responsibility | Selected Data Structure / Algorithm | Technical Performance |
| :--- | :--- | :--- | :--- |
| **1. Subscriber Directory** | High-speed lookup, addition, and modification of user tiers. | Hash Table (`std::unordered_map`) | Average O(1) time |
| **2. Access Undo Pipeline** | Instant LIFO rollbacks of erroneous administrative tier changes. | Linear Stack (`std::stack`) | O(1) execution |
| **3. Click Tracker** | Chronological ingestion and buffering of streaming user activity logs. | FIFO Queue (`std::queue`) | O(1) sequencing |
| **4. Coupon Validation** | Rapid validation of active promotional codes at checkout points. | Binary Search over Sorted `std::vector` | O(log N) search space splits |
| **5. Article Sorter** | Dynamic performance classification based on engagement or monetization. | Custom Merge Sort & Quick Sort | O(N log N) data indexing |
| **6. Content Network Web**| Semantic relationship and thematic connection layout maps. | Directed Graph (Adjacency List) | Space efficient O(V + E) layout |
| **7. Reading Path Finder** | Personalized conversion path generation between related contents. | Dijkstra's Algorithm (Min-Priority Heap) | Optimized O(E log V) routing |
| **8. Price Optimizer** | Real-time win-back targeting discount processing for inactive readers. | Greedy Strategy Decision Engine | O(1) immediate step matrix |
| **9. Support Helpdesk** | Tier-priority automated ticket routing and customer triage. | Binary Max Heap (`std::priority_queue`)| O(log N) priority sorting |
| **10. Browsing History Cache** | Fixed-capacity user browsing interaction trail tracker. | Custom Doubly Linked List | O(1) head/tail mutations |

---

## 📊 System Architecture Block Layout

The flow of user event streaming, authorization lookups, and context routing calculations executes through the following in-memory pipeline:


[ Streaming User Activity Interaction ]
                     │
                     ▼
         [ Click Tracker Queue ] ──────► [ Doubly Linked List History ]
                     │
                     ▼
┌───────────────────────┴───────────────────────┐
▼                                               ▼
[ Subscriber Map ] ──► ( Paywall Router ) ◄── [ Content Web Graph ]
(Hash Table)                                      (Adjacency List)
│
▼
[ Dijkstra Path Engine ]


## 🛠️ Compilation and Execution Instructions

This system relies strictly on standard C++17 features and requires no external third-party framework dependencies.

### 1. Prerequisites
Ensure you have a modern C++ compiler installed (GCC v7.1+, Clang v5.0+, or MSVC 2017+).

### 2. Compilation Sequence
Execute the compilation string from your terminal or command prompt environment:



Operational Console Dashboard Layout
Upon launching, the interactive menu system exposes a centralized telemetry control dashboard:

===================================
 READPASS PUBLISHING ROUTER ENGINE 
===================================
1. Subscriber Directory
2. Access Undo Operations
3. Click Stream Tracker
4. Coupon Codes Registry
5. Article Analytics Sorter
6. Content Network Web
7. Path Intersect Finder
8. Pricing Engine Optimization
9. Extended Engine Features
10. Analytics Control Dashboard
11. Display Complexity Matrix
12. Shut Down System
-----------------------------------
Enter Action Target Choice [1-12]:

Verification Verification Steps
Test Case A (System Check): Provide selection choice 10 to display the Analytics Control Dashboard. This checks the current lengths of the initialized storage vectors in internal memory.

Test Case B (Graph Routing Calculation): Select choice 7 and insert source parameter 201 and destination target parameter 202 to verify that Dijkstra's priority-heap execution finds the shortest relationship route across the structural content nodes.

Test Case C (LIFO Undo States): Update a tier modification via selection choice 1, select choice 2 to display the transaction buffer log, and trigger a rollback state verification loop.
```bash
g++ -std=c++17 main.cpp -o readpass_engine
