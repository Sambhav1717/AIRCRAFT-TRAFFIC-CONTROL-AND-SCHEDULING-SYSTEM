# AIRCRAFT-TRAFFIC-CONTROL-AND-SCHEDULING-SYSTEM
# Overview
This project simulates an Air Traffic Control (ATC) system to manage airport operations, flight scheduling, and resource allocation efficiently. It models airports, flights, runways, and gates while ensuring conflict-free scheduling.

# Objective
Simulate real-world ATC operations
Optimize flight scheduling using priority-based algorithms
Allocate runways and gates without time conflicts
Compute shortest paths between airports

# Data Structures Used
std::vector – Store airports, flights, routes
std::priority_queue – Flight scheduling (EDF + priority-based)
std::unordered_map – Fast airport lookup
Graph representation – Airport network

# Algorithms Implemented
Floyd–Warshall Algorithm – Shortest path (distance & time)
Priority Queue Scheduling – Earliest Deadline First (EDF)
Greedy Resource Allocation – Runway & gate assignment
Time Conflict Detection

# Modules
Airport Module – Manages airports, terminals, gates, runways
Flight Module – Handles flight data and scheduling
AirportNetwork Module – Manages routes between airports
ATC Module – Core scheduling & conflict resolution
TimeHandler Module – Time conversion and arithmetic

# Key Features
Airport and flight data loading from files
Priority-based flight scheduling
Runway & gate allocation with conflict prevention
Shortest route calculation
Conflict resolution and rescheduling

# Technologies Used
C++
STL (Vector, Priority Queue, Unordered Map)
Object-Oriented Programming

# Conclusion
This project provides a structured simulation of air traffic control operations using DSA concepts such as priority queues, graphs, greedy algorithms, and shortest path computation. It can be extended to support more advanced scheduling and real-time ATC systems.
