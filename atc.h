#ifndef ATC_H
#define ATC_H

#include "AirportNetwork.h"
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <queue>
#include <algorithm>
#include <chrono>
#include <thread>

using namespace std;

void addloadingdots()
{
    for (int i = 3; i > 0; --i) {
        // Printing the remaining seconds
        cout<< "." << endl;

        // Waiting for 1 second
        this_thread::sleep_for(chrono::seconds(1));
    }
}


struct FlightCompare // structure acting like fn = functor
{
    // Priority-based scheduling with EDF as a tie-breaker. (earliest deadline first)
    bool operator()(const Flight *f1, const Flight *f2)
    {
        if (f1->getPriority() != f2->getPriority())
            return f1->getPriority() > f2->getPriority(); // lower priority value comes first

        return f1->getSchedule().first > f2->getSchedule().first; // earlier departure comes first (EDF)
    }
};

class ATC
{
private:
    priority_queue<Flight *, vector<Flight *>, FlightCompare> flightQueue;
    unordered_map<string, vector<Flight *>> flightGraph; // key - src, all outgoing flights from src

public:
    /*
    // Schedule a flight for takeoff and landing
    bool scheduleFlight(AirportNetwork *network)
    {
        Airport *src = network->getairportobject("DEL");
        Airport *dest = network->getairportobject("MUM");

        Flight f("123", "indigo", dest, src, 1100, 1000, "boeing", 1);

        // Try assigning runway and gate at source
        if (!assignRunway(src, f.getFlightNumber(), f.getSchedule(), 10)) // duration for 10 minutes
        {
            cout << "No runway available at source for departure\n";
            return false;
        }
        if (!assignGate(src, f.getFlightNumber(), f.getSchedule(), 60)) // duration for 60 mins
        {
            cout << "No gate available at source\n";
            return false;
        }

        // Try assigning runway and gate at destination
        if (!assignRunway(dest, f.getFlightNumber(), f.getSchedule(), 10))
        {
            cout << "No runway available at destination for arrival\n";
            return false;
        }
        if (!assignGate(dest, f.getFlightNumber(), f.getSchedule(), 60))
        {
            cout << "No gate available at destination\n";
            return false;
        }

        // Schedule in airports
        src->scheduleTakeoff(f);
        dest->scheduleLanding(f);

        return true;
    }
    */

    // GREEDY WAY OF ASSIGNING RUNWAY, NEED TO ADD PRIORITIZATION
    bool assignRunway(Airport *airport, Flight *f, int timestart, int duration)
    {
        int timeEnd = TimeHandler::addMinutes(timestart, duration);
        string flightNumber = f->getFlightNumber();

        for (auto &runway : airport->getRunways())
        {
            if (runway.isAvailableInSlot(timestart, timeEnd))
            {
                runway.addSlot({timestart, timeEnd}, f);
                runway.occupy();
                f->setrunway(&runway, {timestart, timeEnd}); // runway and the time slot alloted
                /*
                cout << "Flight " << flightNumber << " assigned to Runway " << runway.getRunwayID()
                     << " from " << timestart << " to " << timeEnd
                     << " at " << airport->get_id() << endl
                     << endl;
                */
                return true;
            }
        }
        return false;
    }

    // GREEDY WAY OF ASSIGNING GATES
    bool assignGate(Airport *airport, Flight *f, int timestart, int duration)
    {
        int timeEnd = TimeHandler::addMinutes(timestart, duration);
        string flightNumber = f->getFlightNumber();

        for (auto &terminal : airport->getTerminals())
        {
            for (auto &gate : terminal.getGates())
            {
                if (gate.isTimeSlotAvailable(timestart, timeEnd))
                {
                    gate.assignSlot({timestart, timeEnd}, f);
                    gate.occupy();
                    f->setgate(&gate, {timestart, timeEnd}); // gate and time slot alloted
                    f->setterminal(&terminal);
                    /*
                    cout << "Flight " << flightNumber << " assigned to Gate " << gate.getGateID()
                         << " from " << timestart << " to " << timeEnd
                         << " at " << airport->get_id() << endl
                         << endl;
                    */
                    return true;
                }
            }
        }
        return false;
    }

    void loadFlightsFromFile(const string &filename, AirportNetwork *network)
    {
        ifstream infile(filename);
        string line;

        // Skip header line
        getline(infile, line);

        while (getline(infile, line))
        {
            string flightNum, airline, srcID, destID, aircraft;
            int departure, arrival, priority;

            stringstream ss(line);
            getline(ss, flightNum, ',');
            getline(ss, airline, ',');
            getline(ss, srcID, ',');
            getline(ss, destID, ',');
            ss >> departure;
            ss.ignore(); // Ignore the comma after departure time
            ss >> arrival;
            ss.ignore(); // Ignore the comma after arrival time
            getline(ss, aircraft, ',');
            ss >> priority;

            // Now process the flight details
            Airport *src = network->getairportobject(srcID);
            Airport *dest = network->getairportobject(destID);
            Flight *f = new Flight(flightNum, airline, src, dest, departure, arrival, aircraft, priority);
            flightQueue.push(f);
        }
    }

    void scheduleAllFlights(AirportNetwork *network,string sourceCode,string destinationCode)
    {
        bool displayflag = true;

        while (!flightQueue.empty())
        {
            Flight *f = flightQueue.top();
            flightQueue.pop();

            Airport *src = network->getairportobject(f->getOrigin());
            Airport *dest = network->getairportobject(f->getDestination());

            // // Get and display path information before scheduling - displayed below
            // vector<string> flightpath = network->getShortestPath(src, dest);
            // vector<string> flighttimepath = network->getTimeOptimizedPath(src, dest);

            // f->setpath(flightpath);
            // f->settimepath(flighttimepath);

            // cout << "\nScheduling flight " << f->getFlightNumber() << " from "
            //      << f->getOrigin() << " to " << f->getDestination() << endl;
            // f->displayRouteInfo();

            bool success = true;

            // DEPARTURE RUNWAY ASSIGNMENT
            // check to assign runway for 5 mins during departure, 2 min prior for prep + 3 min clearance
            int runwayassigntime = TimeHandler::subtractMinutes(f->getSchedule().first, 2);

            if (!assignRunway(src, f, runwayassigntime, 5))
            {
                cout << "\nFailed to assign runway at source for flight " << f->getFlightNumber() << endl;
                success = false;
            }

            // DEPARTURE GATE ASSIGNMENT
            // gate assigned 120 min before flight for domestic - 30 min prior prep(turnaround time) + 90 min boarding etc
            int gateassigntime = TimeHandler::subtractMinutes(f->getSchedule().first, 120);

            if (!assignGate(src, f, gateassigntime, 120))
            {
                cout << "Failed to assign gate at source for flight " << f->getFlightNumber() << endl;
                success = false;
            }

            // LANDING RUNWAY ASSIGNMENT
            // check to assign runway for landing at arrival time for 5 mins, 2 mins prior for clearance + 3 mins for touchdown,deceleration,vacating
            runwayassigntime = TimeHandler::subtractMinutes(f->getSchedule().second, 2);
            if (!assignRunway(dest, f, runwayassigntime, 5))
            {
                cout << "Failed to assign runway at destination for flight " << f->getFlightNumber() << endl;
                success = false;
            }

            // LANDIGN GATE ASSIGNMENT
            // gate assigned 120 min before flight for domestic - 30 min prior for prep + 60 min deboarding,unloading,turnaround
            gateassigntime = TimeHandler::subtractMinutes(f->getSchedule().second, 30);
            if (!assignGate(dest, f, gateassigntime, 90))
            {
                cout << "Failed to assign gate at destination for flight " << f->getFlightNumber() << endl;
                success = false;
            }

            if (success)
            {
                src->scheduleTakeoff(f);
                dest->scheduleLanding(f);
                flightGraph[src->get_id()].push_back(f);
                network->setFlightDetails(f);

                // Get and display path information after scheduling
                vector<string> flightpath = network->getShortestPath(src, dest);
                vector<string> flighttimepath = network->getTimeOptimizedPath(src, dest);

                f->setpath(flightpath);
                f->settimepath(flighttimepath);

                // cout << "\nSCHEDULING SUCCESSFULL!!" << endl;
                // //      <<"Flight " << f->getFlightNumber() << " from "
                // //      << f->getOrigin() << " to " << f->getDestination() << endl;

                // f->displayflightdetails();
                // f->displayRouteInfo();

                if(sourceCode==src->get_id() && destinationCode==dest->get_id())
                {
                        if(displayflag){
                        cout<<"Congratulations !! "<<endl;
                        cout<<"A Flight has been scheduled for You !! "<<endl;
                        addloadingdots();
                        cout<<"Flight Details : "<<endl;
                        f->displayflightdetails();
                        src->scheduleTakeoff(f);
                        dest->scheduleLanding(f);
        
                        //new changes
                        vector<string> flightpath = network->getShortestPath(src,dest);
                        f->setpath(flightpath); 

                        // cout<<endl<<"Calculating Optimum route : "<<endl;
                        // addloadingdots();
                        // cout<<"The shortest distance is : "<<network->getShortestDistance(src,dest)<<endl;
                        // f->displaypath();
        
                        // vector<string> flighttimepath = network->getTimeOptimizedPath(src,dest);
                        // f->settimepath(flighttimepath); 
                        // cout<<endl<<"The shortest time is "<<network->getShortestTime(src,dest)<<endl;
                        // f->displaytimepath();

                        f->displayRouteInfo();

                        cout<<endl;
                        cout<<"Checking for more flights from " << sourceCode <<" to " << destinationCode <<endl;
                        addloadingdots();
                        displayflag = false;
                    }
                }

                /* new changes
                vector<string> flightpath = network->getShortestPath(src, dest);
                f->setpath(flightpath);
                cout << endl
                     << "the shortest distance is " << network->getShortestDistance(src, dest) << endl;
                f->displaypath();

                vector<string> flighttimepath = network->getTimeOptimizedPath(src, dest);
                f->settimepath(flighttimepath);
                cout << endl
                     << "the shortest time is " << network->getShortestTime(src, dest) << endl;
                f->displaytimepath();
                */
            }
            else
            {
                cout << "\nSCHEDULING FAILED!!" << endl;
                // cout << "Flight " << f.getFlightNumber() << " could not be scheduled.\n";
                //  Conflict Handling - Retry or Defer
                if (f->getPriority() > 1) // Assuming priority 0 and 1 are critical (Emergency > VIP)
                {
                    // If the flight is not urgent, defer it by a few minutes and re-insert it into the queue
                    int delayTime = 10; // Delay by 10 minutes for non-urgent flights
                    f->setSchedule({f->getSchedule().first + delayTime, f->getSchedule().second + delayTime});
                    cout << "Flight " << f->getFlightNumber() << " deferred for retrying later.\n";
                    flightQueue.push(f);
                }
                else
                {
                    // High priority flight couldn't be scheduled, log and defer
                    cout << "Flight " << f->getFlightNumber() << " could not be scheduled due to resource conflicts. Retrying...\n";
                    int delayTime = 5; // Delay time (for urgent flights retrying quickly)
                    f->setSchedule({f->getSchedule().first + delayTime, f->getSchedule().second + delayTime});
                    flightQueue.push(f);
                }
            }
        }
    }

    void displayscheduleflights()
    {
        cout << "\n--- Scheduled Flights by Source Airport ---\n";

        for (const auto &entry : flightGraph)
        {
            const string &sourceAirport = entry.first;
            const vector<Flight *> &flights = entry.second;

            cout << "\nSource Airport: " << sourceAirport << endl;
            for (const Flight *f : flights)
            {
                cout << "  Flight " << f->getFlightNumber()
                     << " to " << f->getDestination()
                     << " at " << f->getSchedule().first
                     << " (" << f->getAirline() << ")\n";
            }
        }
    }

    void displayscheduledflightsFromTo(const string &srcCode, const string &destCode)
    {
        auto it = flightGraph.find(srcCode);
        if (it == flightGraph.end() || it->second.empty())
        {
            cout << "No flights scheduled from " << srcCode << ".\n";
            return;
        }

        //cout << "\nScheduled Flights from " << srcCode << " to " << destCode << ":\n";
        bool found = false;

        for (const Flight *f : it->second)
        {
            if (f->getDestination() == destCode)
            {
                found = true;
                cout << endl;
                cout << "Flight No - " << f->getFlightNumber() << " (" << f->getAirline() << ")\n"
                     << "Departure - " << f->getSchedule().first << endl
                     << "Arriving - " << f->getSchedule().second << endl
                     << "Cost - " << f->getFlightCost() << endl;
            }
        }

        if (!found)
        {
            cout << "No flights found from " << srcCode << " to " << destCode << ".\n";
        }
    }
};

#endif

/*
A high conflict rate (many flights failing to get assigned)
The need to prioritize some flights (VIP, international, etc.)
Limited gates/runways and dense scheduling windows
Then you might need:

Interval scheduling algorithms
Priority queues / heuristics
Even backtracking or constraint solvers

Interval Scheduling Algorithms: For resource-intensive systems like this, you could look into interval scheduling algorithms (like Earliest Deadline First (EDF) or Weighted Shortest Job First (WSJF)) if you want more sophisticated handling of flight scheduling, especially when considering overlapping resources or multiple flights competing for the same time slots.
Backtracking/Constraint Solvers: If your scheduling becomes more complex and the conflicts increase (for example, if there are many flights with tight schedules), you could explore constraint satisfaction solvers or backtracking algorithms to optimize the resource allocation and minimize delays.
*/