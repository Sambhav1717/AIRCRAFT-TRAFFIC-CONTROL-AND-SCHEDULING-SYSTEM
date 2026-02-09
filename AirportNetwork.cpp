#include "AirportNetwork.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <utility> // for pair
#include <cctype>  // for ::tolower

using namespace std;

/*
AirportNetwork.cpp → remove the whole class body and just define each function like
as class is already declared in AirportNetwork.h
*/

AirportNetwork::AirportNetwork()
{
    /* code_obj map with all airport codes and their corresponding Airport objects
    code_obj["DEL"] = &del;
    code_obj["MUM"] = &mum;
    code_obj["BLR"] = &blr;
    code_obj["HYD"] = &hyd;
    code_obj["CCU"] = &ccu;
    code_obj["COK"] = &cok;
    code_obj["AMD"] = &amd;
    code_obj["PNQ"] = &pnq;
    */
}

// Add an airport to the network
void AirportNetwork::addAirport(const string &airportName)
{
    graph[code_obj[airportName]]; // Empty initialization for a new airport
}

// Add a route between airports using IATA codes (including distance and flight time)
void AirportNetwork::addRoute(const string &src, const string &dest, int dis, const string &time,double cst)
{
    Airport* s = code_obj[src];
    Airport* d = code_obj[dest];
    // Check if source airport exists
    if (code_obj.find(src) == code_obj.end()) {
        cerr << "Error: Source airport '" << src << "' not found in the network." << endl;
        return;
    }
    
    // Check if destination airport exists
    if (code_obj.find(dest) == code_obj.end()) {
        cerr << "Error: Destination airport '" << dest << "' not found in the network." << endl;
        return;
    }

    // Check if the route already exists in the source's adjacency list
    for (const auto &route : graph[s]) {
        if (route.destination == d) {
            return;
        }
    }

    // Add bidirectional route if it doesn't exist
    graph[s].push_back({d, dis, time, cst});
    graph[d].push_back({s, dis, time, cst});
}

//declare and set the details of every airport
void AirportNetwork::loadAirportDetailsFromFile(const string &filename) {
    ifstream file(filename);
    string line;

    if (!file.is_open()) {
        cout << "Failed to open file." << endl;
        return;
    }

    bool readingAirports = true;

    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        stringstream ss(line);
        vector<string> tokens;
        string token;

        // Split line by ';' and get each string element
        while (getline(ss, token, ';')) {
            tokens.push_back(token);
        }

        if (tokens.size() == 4 && readingAirports) {
            // Airport data: code;name;location;capacity
            string code = tokens[0];
            string name = tokens[1];
            string location = tokens[2];
            int capacity = stoi(tokens[3]);  //string to int

            Airport* airport = new Airport(code, name, location, capacity);
            airports.push_back(airport);
            code_obj[code] = airports.back();  //mapping code to object
        }
        else if (tokens.size() == 5) {
            readingAirports = false; // Switch to runway/terminal reading
            // Runway/Terminal data: code;runway;length;terminal;gates
            string code = tokens[0];
            string runway = tokens[1];
            int length = stoi(tokens[2]);
            string terminalId = tokens[3];
            int numGates = stoi(tokens[4]);

            if (code_obj.find(code) != code_obj.end()) {
                Airport* airport = code_obj[code];

                Runway r(runway, length);
                airport->addRunway(r);

                Terminal t(terminalId);
                for (int i = 0; i < numGates; ++i) {
                    Gate g(terminalId + "_Gate" + to_string(i + 1));
                    t.addGate(g);
                }
                airport->addTerminal(t);
            }
        }
    }

    file.close();
}

// Load the airport network from a file
void AirportNetwork::loadNetworkFromFile(const string &filename)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cout << "Failed to open file.\n";
        return;
    }

    string line;
    while (getline(file, line))
    {
        // The string line from file is treated as an input stream
        stringstream ss(line);
        string from, to, flightTime;
        int distance;
        double cst;

        // Read data from the file separated by spaces
        ss >> from >> to >> distance >> flightTime >> cst;

        addAirport(from);
        addAirport(to);
        addRoute(from, to, distance, flightTime, cst);
    }

    file.close();
}


void AirportNetwork::showNetwork()
{
    /*
    for (auto &[airportptr, neighbors] : graph)
    {
        cout << "\nAirport: " << airportptr->getAirportname() << " (" << airportptr->get_id() << ") has direct routes to:\n";
        for (const auto &route : neighbors)
        {

            cout << "  -> " << route.destination->getAirportname() << " (" << route.destination->get_id() << ") - "
                 << route.distance << " km, Duration: " << route.flightTime << "\n";
        }
    }
    */

    for(auto &[airportptr,neighbours] : graph){
        cout << "\nAirport: " << airportptr->get_id() << " has direct routes to : ";
        for (const auto &route : neighbours){
            cout<<route.destination->get_id()<<" ";
        }
        cout<<endl;
    }
}

void AirportNetwork::displayairportdetails(){
    for(const Airport *arp : airports){
        cout<<endl;
        arp->displayInfo();
    }
}

void AirportNetwork::displayairportdetails(string aptcode)
{
    for(Airport *arp : airports){
        string arprtid=arp->get_id();
        if(arprtid==aptcode)
        {
            arp->displayInfo();
        }
    }
}

Airport* AirportNetwork::getairportobject(const string &arp){
    return code_obj[arp];
}

//new changes

void AirportNetwork::computeShortestPaths() {
    int n = airports.size();
    minDist.assign(n, vector<int>(n, 1e9)); // initialize to "infinity"
    next.assign(n, vector<int>(n, -1));
    
    // Build mappings
    for (int i = 0; i < n; ++i) {
        airportToIndex[airports[i]] = i;
        indexToAirport.push_back(airports[i]);
        minDist[i][i] = 0;
        next[i][i] = i;
    }

    // Fill direct distances from graph
    for (auto &[src, routes] : graph) {
        int u = airportToIndex[src];
        for (const Route &r : routes) {
            int v = airportToIndex[r.destination];
            if (r.distance < minDist[u][v]) {
                minDist[u][v] = r.distance;
                next[u][v] = v;
            }
        }
    }

    // Floyd-Warshall main loop
    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (minDist[i][k] + minDist[k][j] < minDist[i][j]) {
                    minDist[i][j] = minDist[i][k] + minDist[k][j];
                    next[i][j] = next[i][k];
                }
            }
        }
    }
}

int AirportNetwork::getShortestDistance(Airport* src, Airport* dest) {
    if (!src || !dest || airportToIndex.find(src) == airportToIndex.end() || airportToIndex.find(dest) == airportToIndex.end())
        return -1;

    int u = airportToIndex[src];
    int v = airportToIndex[dest];

    return (minDist[u][v] >= 1e9) ? -1 : minDist[u][v]; // -1 if unreachable
}


vector<string> AirportNetwork::getShortestPath(Airport* src, Airport* dest) {
    vector<string> path;
    if (!src || !dest || airportToIndex.find(src) == airportToIndex.end() || airportToIndex.find(dest) == airportToIndex.end())
        return path;

    int u = airportToIndex[src];
    int v = airportToIndex[dest];

    if (next[u][v] == -1) return path;

    while (u != v) {
        path.push_back(indexToAirport[u]->get_id());
        u = next[u][v];
    }
    path.push_back(indexToAirport[v]->get_id());

    return path;
}


void AirportNetwork::displayshortestdist(){
    int n = airports.size();

    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            cout<<minDist[i][j] <<" ";
        }
        cout<<endl;
    }

}

//redundant for time

void AirportNetwork::computeTimeShortestPaths() {
    int n = airports.size();
    minTime.assign(n, vector<int>(n, 1e9));
    timeNext.assign(n, vector<int>(n, -1));

    // Initialize with direct route times
    for (auto &[src, routes] : graph) {
        int u = airportToIndex[src];
        minTime[u][u] = 0;
        timeNext[u][u] = u;

        for (const Route& route : routes) {
            int v = airportToIndex[route.destination];
            int minutes = TimeHandler::timeToMinutes(route.flightTime);

            if (minutes < minTime[u][v]) {
                minTime[u][v] = minutes;
                timeNext[u][v] = v;
            }
        }
    }

    // Floyd-Warshall: time-based
    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (minTime[i][k] + minTime[k][j] < minTime[i][j]) {
                    minTime[i][j] = minTime[i][k] + minTime[k][j];
                    timeNext[i][j] = timeNext[i][k];
                }
            }
        }
    }
}

int AirportNetwork::getShortestTime(Airport* src, Airport* dest) {
    if (!src || !dest || airportToIndex.find(src) == airportToIndex.end() || airportToIndex.find(dest) == airportToIndex.end())
        return -1;

    int u = airportToIndex[src];
    int v = airportToIndex[dest];

    return (minTime[u][v] >= 1e9) ? -1 : minTime[u][v];
}

vector<string> AirportNetwork::getTimeOptimizedPath(Airport* src, Airport* dest) {
    vector<string> path;

    if (!src || !dest || airportToIndex.find(src) == airportToIndex.end() || airportToIndex.find(dest) == airportToIndex.end())
        return path;

    int u = airportToIndex[src];
    int v = airportToIndex[dest];

    if (timeNext[u][v] == -1) return path;

    while (u != v) {
        path.push_back(indexToAirport[u]->get_id());
        u = timeNext[u][v];
    }
    path.push_back(indexToAirport[v]->get_id());

    return path;
}

void AirportNetwork::printAllShortestDistances() const {
    cout << "\nAll shortest distances between airports (km):\n";
    for (int i = 0; i < indexToAirport.size(); ++i) {
        for (int j = 0; j < indexToAirport.size(); ++j) {
            if (i != j && minDist[i][j] < 1e9) {
                cout << indexToAirport[i]->get_id() << " -> " << indexToAirport[j]->get_id() 
                     << ": " << minDist[i][j] << " km\n";
            }
        }
    }
}

void AirportNetwork::printAllShortestTimes() const {
    cout << "\nAll shortest flight times between airports (minutes):\n";
    for (int i = 0; i < indexToAirport.size(); ++i) {
        for (int j = 0; j < indexToAirport.size(); ++j) {
            if (i != j && minTime[i][j] < 1e9) {
                cout << indexToAirport[i]->get_id() << " -> " << indexToAirport[j]->get_id() 
                     << ": " << minTime[i][j] << " minutes (" 
                     << minTime[i][j]/60 << "h " << minTime[i][j]%60 << "m)\n";
            }
        }
    }
}

void AirportNetwork::printPathBetween(const string& src,const string& dest) {
    Airport* srcAirport = code_obj[src];
    Airport* destAirport = code_obj[dest];
    
    vector<string> path = getShortestPath(srcAirport, destAirport);
    int distance = getShortestDistance(srcAirport, destAirport);
    
    cout << "\nShortest path from " << src << " to " << dest << ":\n";
    cout << "Distance: " << distance << " km\n";
    cout << "Route: ";
    for (const auto& airport : path) {
        cout << airport << " ";
    }
    cout << endl;
}

void AirportNetwork::printTimeOptimizedPathBetween(const string& src,const string& dest) {
    Airport* srcAirport = code_obj[src];
    Airport* destAirport = code_obj[dest];
    
    vector<string> path = getTimeOptimizedPath(srcAirport, destAirport);
    int time = getShortestTime(srcAirport, destAirport);
    
    cout << "\nTime-optimized path from " << src << " to " << dest << ":\n";
    cout << "Total time: " << time << " minutes (" 
         << time/60 << "h " << time%60 << "m)\n";
    cout << "Route: ";
    for (const auto& airport : path) {
        cout << airport << " ";
    }
    cout << endl;
}

void AirportNetwork::setFlightDetails(Flight *f) {
    // Get the source and destination airport codes
    Airport* src = code_obj[f->getOrigin()];
    string dest = f->getDestination();
    Airport* destination = code_obj[f->getDestination()];

    // Find the source and destination airports in the graph
    for (const auto &route : graph[src]) {
        if (route.destination->get_id() == dest) {
            // Set the cost and time for the flight

            //optional
            double dist = minDist[airportToIndex[src]][airportToIndex[destination]]; //cost from floyd Warshall output
            
            f->setFlightCost(route.cost); // Set the cost
            f->setFlightTime(TimeHandler::timeToMinutes(route.flightTime)); // Set the time in minutes
            f->setFlightDistance(dist);
            break; // Exit the loop once the route is found
        }
    }
}
