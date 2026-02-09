#ifndef AIRPORT_H
#define AIRPORT_H

#include <vector>
#include <string>
#include <iostream>

using namespace std;

class Flight;
class Airport;
class Terminal;
class Gate;
class Runway;

// ---------------------------
// TimeHandler Utility Class
// ---------------------------
class TimeHandler {
public:
    static int toMinutes(int hhmm);
    static int toHHMM(int minutes);
    static int addMinutes(int hhmm, int minutesToAdd);
    static int subtractMinutes(int hhmm, int minutesToSubtract);
    static bool isConflict(int time1Start, int time1End, int time2Start, int time2End);
    static int timeToMinutes(const string& timeStr);
    
};

// ---------------------------
// Gate Class
// ---------------------------
class Gate {
private:
    string gateID;
    bool isAvailable;
    vector<pair<pair<int, int>, Flight*>> gateOccupiedSlot;

public:
    Gate(string id);
    void occupy();
    void free();
    void displaygate();
    bool getStatus() const;
    string getGateID() const;
    bool isTimeSlotAvailable(int start, int end);
    void assignSlot(pair<int, int> newSlot, Flight* flight);

    vector<pair<pair<int, int>, Flight*>> getOccupiedSlots();
    void freeSlot(const pair<int, int>& slot);
};

// ---------------------------
// Terminal Class
// ---------------------------
class Terminal {
private:
    string terminalID;
    vector<Gate> gates;

public:
    Terminal(string id);
    void addGate(const Gate &gate);
    vector<Gate> &getGates();
    void displayterminaldetails() const;
    string getTerminalID() const;
};

// ---------------------------
// Runway Class
// ---------------------------
class Runway {
private:
    string runwayID;
    int length;
    bool isAvailable;
    vector<pair<pair<int, int>, Flight*>> runwayOccupiedSlot;  //slot and the flight in it
    //reconsider using map later

public:
    Runway(string id, int leng);
    void occupy();
    void free();
    bool getStatus() const;
    string getRunwayID() const;
    void displayrunwaydetails() const;
    bool isAvailableInSlot(int start, int end);
    void addSlot(pair<int, int> newSlot, Flight* flight);

    vector<pair<pair<int, int>, Flight*>> &getOccupiedSlots();
    void freeSlot(const pair<int, int>& slot);
};

// ---------------------------
// Airport Class
// ---------------------------
class Airport {
private:
    string airportID;
    string name;
    string location;
    vector<Runway> runways;
    vector<Terminal> terminals;
    vector<Flight*> incomingFlights;
    vector<Flight*> outgoingFlights;
    string weatherConditions;
    bool emergencyStatus;
    int capacity;

public:
    Airport(string id, string n, string loc, int cap);
    void addRunway(const Runway &runway);
    void addTerminal(const Terminal &terminal);
    void scheduleLanding(Flight* flight);
    void scheduleTakeoff(Flight* flight);
    string get_id();
    string getAirportname();
    vector<Runway> &getRunways();
    vector<Terminal> &getTerminals();
    void updateWeather(const string &weather);
    void declareEmergency();
    void clearEmergency();
    void displayInfo() const;
};

// ---------------------------
// Flight Class
// ---------------------------
class Flight {
private:
    string flightNumber;
    string airline;
    Airport *origin;
    Airport *destination;
    pair<int, int> schedule;
    string aircraft;
    //no need for the pair
    vector< pair<pair<int,int>,Runway*> > runwayassigned;  //0 index for sec airport and 1 for dest airport
    //{{start,endtime},runway assigned}
    vector< pair<pair<int,int>,Gate*> > gateassigned;
    vector<Terminal*> terminalassigned;
    int priority;
    vector<string> path;
    vector<string> timepath;

    double cost;        // Flight cost
    int shortestDist;
    int flightTime;     // Duration of flight in minutes

public:
    Flight(string num, string air, Airport *orig, Airport *dest, int dep, int arv, string acrft, int p);
    string getFlightNumber() const;
    string getAirline() const;
    string getDestination() const;
    string getOrigin() const;
    pair<int, int> getSchedule() const;
    void setSchedule(const pair<int, int> &newschedule);
    int getPriority() const;
    void setrunway(Runway *r,pair<int,int> slot);
    void setgate(Gate *g,pair<int,int> slot);
    void setterminal(Terminal *t);
    void displayflightdetails();

    //new changes
    void setpath(vector<string> p);
    void displaypath();
    void settimepath(vector<string>);
    void displaytimepath();

    void displayRouteInfo();

    void setFlightCost(double cost);
    void setFlightTime(int timeInMinutes);
    void setFlightDistance(int dist);
    double getFlightCost()const;
    int getFlightTime();
    int getFlightDistance();

    //new
    Runway* getRunway() const;
    pair<int, int> getRunwaySlot() const;
    Gate* getGate() const;
    pair<int, int> getGateSlot() const;
};

#endif // AIRPORT_H
