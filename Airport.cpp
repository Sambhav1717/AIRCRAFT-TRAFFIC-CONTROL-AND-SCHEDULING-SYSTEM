// airport.cpp
#include "Airport.h"

// ---------------------------
// TimeHandler Utility Class
// ---------------------------
int TimeHandler::toMinutes(int hhmm)
{
    return (hhmm / 100) * 60 + (hhmm % 100);
}

int TimeHandler::toHHMM(int minutes)
{
    return (minutes / 60) * 100 + (minutes % 60);
}

int TimeHandler::addMinutes(int hhmm, int minutesToAdd)
{
    return toHHMM(toMinutes(hhmm) + minutesToAdd);
}

int TimeHandler::subtractMinutes(int hhmm, int minutesToSubtract)
{
    return toHHMM(toMinutes(hhmm) - minutesToSubtract);
}

bool TimeHandler::isConflict(int time1Start, int time1End, int time2Start, int time2End)
{
    return max(time1Start, time2Start) < min(time1End, time2End);
}

int TimeHandler::timeToMinutes(const string& timeStr) {
    int hours = stoi(timeStr.substr(0, 2));
    int minutes = stoi(timeStr.substr(3, 2));
    return hours * 60 + minutes;
}


// ---------------------------
// Gate Class
// ---------------------------
Gate::Gate(string id) : gateID(id), isAvailable(true) {}

void Gate::occupy() { isAvailable = false; }

void Gate::free() { isAvailable = true; }

void Gate::displaygate() { cout << "Gate ID: " << gateID << " Status: " << (isAvailable ? "Available" : "Occupied") << endl; }

bool Gate::getStatus() const { return isAvailable; }

string Gate::getGateID() const { return gateID; }

bool Gate::isTimeSlotAvailable(int start, int end)
{
    for (const auto &entry : gateOccupiedSlot)
    {
        int s = entry.first.first;
        int e = entry.first.second;
        if (TimeHandler::isConflict(s, e, start, end))
            return false;
    }
    return true;
}

void Gate::assignSlot(pair<int, int> newSlot, Flight* flight)
{
    gateOccupiedSlot.push_back({newSlot, flight});
}

//new 
vector<pair<pair<int, int>, Flight*>> Gate::getOccupiedSlots(){return gateOccupiedSlot;}

void Gate::freeSlot(const pair<int, int>& slot) {
    for (auto it = gateOccupiedSlot.begin(); it != gateOccupiedSlot.end(); ++it) {
        if (it->first == slot) {
            gateOccupiedSlot.erase(it);
            free(); // Mark gate as available if no slots left
            break;
        }
    }
}

// ---------------------------
// Terminal Class
// ---------------------------
Terminal::Terminal(string id) : terminalID(id) {}

void Terminal::addGate(const Gate &gate) { gates.push_back(gate); }

vector<Gate> &Terminal::getGates() { return gates; }

void Terminal::displayterminaldetails() const
{
    cout << endl
         << "Terminal " << terminalID << endl;
    cout << "Gates : \n";
    for (auto i : gates)
    {
        i.displaygate();
    }
}

string Terminal::getTerminalID() const { return terminalID; }

// ---------------------------
// Runway Class
// ---------------------------
Runway::Runway(string id, int leng) : runwayID(id), length(leng), isAvailable(true) {}

void Runway::occupy() { isAvailable = false; }

void Runway::free() { isAvailable = true; }

bool Runway::getStatus() const { return isAvailable; }

string Runway::getRunwayID() const { return runwayID; }

void Runway::displayrunwaydetails() const
{
    cout << "Runway ID: " << runwayID << " Length: " << length << " Status: " << (isAvailable ? "Available" : "Occupied") << endl;
}

bool Runway::isAvailableInSlot(int start, int end)
{
    for (const auto &entry : runwayOccupiedSlot)
    {
        int s = entry.first.first;  //start time of occupied slots
        int e = entry.first.second;  //end time
        if (TimeHandler::isConflict(s, e, start, end))
            return false;
    }
    return true;
}

void Runway::addSlot(pair<int, int> newSlot, Flight* flight)
{
    runwayOccupiedSlot.push_back({newSlot, flight});
}

//new
vector<pair<pair<int, int>, Flight*>>& Runway::getOccupiedSlots(){return runwayOccupiedSlot;}

void Runway::freeSlot(const pair<int, int>& slot) {
    for (auto it = runwayOccupiedSlot.begin(); it != runwayOccupiedSlot.end(); ++it) {
        if (it->first == slot) {
            runwayOccupiedSlot.erase(it);
            free(); // Mark runway as available if no slots left
            break;
        }
    }
}

// ---------------------------
// Airport Class
// ---------------------------
Airport::Airport(string id, string n, string loc, int cap) : airportID(id), name(n), location(loc), capacity(cap), emergencyStatus(false) {}

void Airport::addRunway(const Runway &runway) { runways.push_back(runway); }

void Airport::addTerminal(const Terminal &terminal) { terminals.push_back(terminal); }

void Airport::scheduleLanding(Flight *flight) { incomingFlights.push_back(flight); }

void Airport::scheduleTakeoff(Flight *flight) { outgoingFlights.push_back(flight); }

string Airport::get_id() { return airportID; }

string Airport::getAirportname() { return name; }

vector<Runway> &Airport::getRunways() { return runways; }

vector<Terminal> &Airport::getTerminals() { return terminals; }

void Airport::updateWeather(const string &weather) { weatherConditions = weather; }

void Airport::declareEmergency() { emergencyStatus = true; }

void Airport::clearEmergency() { emergencyStatus = false; }

void Airport::displayInfo() const
{
    cout << "Airport: " << name << " (" << airportID << ")" << endl;
    cout << "Location: " << location << endl;
    cout << "Capacity: " << capacity << " flights" << endl;
    cout << "Weather: " << weatherConditions << endl;
    cout << "Emergency Status: " << (emergencyStatus ? "YES" : "NO") << endl;

    for (const Runway &r : runways)
    { // changed
        r.displayrunwaydetails();
    }

    for (const Terminal &t : terminals)
    {
        t.displayterminaldetails();
    }
}

// ---------------------------
// Flight Class
// ---------------------------
Flight::Flight(string num, string air, Airport *orig, Airport *dest, int dep, int arv, string acrft, int p)
    : flightNumber(num), airline(air), origin(orig), destination(dest), schedule({dep, arv}), aircraft(acrft), priority(p) {}

string Flight::getFlightNumber() const { return flightNumber; }

string Flight::getAirline() const { return airline; }

string Flight::getDestination() const { return destination->get_id(); }

string Flight::getOrigin() const { return origin->get_id(); }

pair<int, int> Flight::getSchedule() const { return schedule; }

void Flight::setSchedule(const pair<int, int> &newschedule) { schedule = newschedule; }

int Flight::getPriority() const { return priority; }

void Flight::setrunway(Runway *r, pair<int,int> slot) { runwayassigned.push_back({slot,r}); }

void Flight::setgate(Gate *g, pair<int,int> slot) { gateassigned.push_back({slot,g}); }

void Flight::setterminal(Terminal *t) { terminalassigned.push_back(t); }

void Flight::displayflightdetails()
{
    cout << endl
         << "Flight No: " << flightNumber << endl
         << "Airline: " << airline << endl
         << "From: " << origin->getAirportname()
         << endl
         << "To: " << destination->getAirportname() << endl
         << "Schedule: " << schedule.first << " - " << schedule.second
         << endl
         << "Aircraft: " << aircraft << endl
         << "Priority: " << priority << endl;
    cout << "Departure Gate: " << gateassigned[0].second->getGateID() << " at Terminal: " << terminalassigned[0]->getTerminalID() << " from " << gateassigned[0].first.first << " to " << gateassigned[0].first.second << endl;
    cout << "Departure Runway: " << runwayassigned[0].second->getRunwayID() << " from " << runwayassigned[0].first.first << " to " << runwayassigned[0].first.second << endl;
    cout << "Arrival Gate: " << gateassigned[1].second->getGateID() << " at Terminal: " << terminalassigned[1]->getTerminalID() << " from " << gateassigned[1].first.first << " to " << gateassigned[1].first.second << endl;
    cout << "Arrival Runway: " << runwayassigned[1].second->getRunwayID() << " from " << runwayassigned[1].first.first << " to " << runwayassigned[1].first.second << endl;
}

//new changes
void Flight::setpath(vector<string> p){
    path = p;
}
void Flight::displaypath(){
    cout<<endl<<"path based on distance : ";
    for(auto i : path){cout<<i<<" ";}
    cout<<endl;
}

void Flight::settimepath(vector<string> p){
    timepath = p;
}
void Flight::displaytimepath(){
    cout<<endl<<"path based on time : ";
    for(auto i : timepath){cout<<i<<" ";}
    cout<<endl;
}

void Flight::displayRouteInfo() {
    cout << endl;
    if (!path.empty()) {
        cout << "Planned route (distance-optimized): ";
        for (const auto& airport : path) {
            cout << airport << " ";
        }
        cout << endl;
        cout<<"Shortest Distance - "<<shortestDist<<endl;
    }
    
    if (!timepath.empty()) {
        cout << "Planned route (time-optimized): ";
        for (const auto& airport : timepath) {
            cout << airport << " ";
        }
        cout << endl;
        cout<<"Minimum Time - "<<flightTime<<endl;
    }
}

void Flight::setFlightCost(double cost) {
    this->cost = cost;
}

void Flight::setFlightTime(int timeInMinutes) {
    this->flightTime = timeInMinutes;
}

void Flight::setFlightDistance(int dist) {
    this->shortestDist = dist;
}

double Flight:: getFlightCost() const { return cost; }

int Flight:: getFlightTime(){ return flightTime; }

int Flight:: getFlightDistance(){ return shortestDist; }


//new
Runway* Flight::getRunway() const { 
    return !runwayassigned.empty() ? runwayassigned[0].second : nullptr; 
}

pair<int, int> Flight::getRunwaySlot() const { 
    return !runwayassigned.empty() ? runwayassigned[0].first : make_pair(-1, -1); 
}

Gate* Flight::getGate() const { 
    return !gateassigned.empty() ? gateassigned[0].second : nullptr; 
}

pair<int, int> Flight::getGateSlot() const { 
    return !gateassigned.empty() ? gateassigned[0].first : make_pair(-1, -1); 
}

