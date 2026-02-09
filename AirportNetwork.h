#ifndef AIRPORT_NETWORK_H
#define AIRPORT_NETWORK_H

#include "Airport.h"
#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

// Structure to hold information about a route
struct Route
{
    Airport *destination; // Destination airport code
    // Airport object in every Route is memory expensive so pointer
    int distance;      // Distance between airports (in km)
    string flightTime; // Flight time (HH:MM format)
    double cost;
};

class AirportNetwork
{
private:
    vector<Airport*> airports;
    unordered_map<Airport*, vector<Route>> graph; //source to destination,distance and time map
    unordered_map<string, Airport *> code_obj;  //airport code to its object map

    // For Floyd-Warshall
    vector<vector<int>> minDist;
    vector<vector<int>> next;
    unordered_map<Airport*, int> airportToIndex;  //mapping airport to int value 
    vector<Airport*> indexToAirport;

    vector<vector<int>> minTime;     // stores minimum time in minutes
    vector<vector<int>> timeNext;     // for reconstructing path by time

public:
    AirportNetwork();
    void addAirport(const string &airportName);
    void addRoute(const string &src, const string &dest, int dis, const string &time,double cst);
    void loadNetworkFromFile(const string &filename);
    void loadAirportDetailsFromFile(const string &filename);
    void showNetwork();
    void displayairportdetails();
    void displayairportdetails(string aptcode);
    Airport* getairportobject(const string &arp);

    void computeShortestPaths();
    int getShortestDistance(Airport* src, Airport* dest);
    vector<string> getShortestPath(Airport* src, Airport* dest);
    void displayshortestdist();

    //redundant for time
    void computeTimeShortestPaths();
    int getShortestTime(Airport* src, Airport* dest);
    vector<string> getTimeOptimizedPath(Airport* src, Airport* dest);

    void printAllShortestDistances() const;
    void printAllShortestTimes() const;
    void printPathBetween(const string& src,const string& dest);
    void printTimeOptimizedPathBetween(const string& src,const string& dest);

    void setFlightDetails(Flight *f);

};

#endif // AIRPORT_NETWORK_H

/*
AirportNetwork.h → contains class declaration.
AirportNetwork.cpp → contains function definitions.
main.cpp → where you use the object.
*/