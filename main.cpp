#include "atc.h"

#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>

using namespace std;

string source,srccode;
string dest,destcode;

void Clear()
{
#if defined _WIN32
    system("cls");
    //clrscr(); // including header file : conio.h
#elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
    system("clear");
    //std::cout<< u8"\033[2J\033[1;1H"; //Using ANSI Escape Sequences 
#elif defined (__APPLE__)
    system("clear");
#endif
}

string toUpperCase(const string& str) {
    string result;
    for (char ch : str) {
        result += toupper(ch);
    }
    return result;
}

// Boyer-Moore preprocessing
void buildBadCharTable(const string &pattern, vector<int>& badChar) {
    fill(badChar.begin(), badChar.end(), -1);
    for (int i = 0; i < pattern.size(); i++) {
        badChar[(int)pattern[i]] = i;
    }
}

bool boyerMooreSearch(const string &text, const string &pattern) {
    vector<int> badChar(256, -1);
    buildBadCharTable(pattern, badChar);
    
    int s = 0;
    int m = pattern.size();
    int n = text.size();
    
    while (s <= (n - m)) {
        int j = m - 1;
        while (j >= 0 && pattern[j] == text[s + j]) {
            j--;
        }
        if (j < 0) {
            return true;
        } else {
            s += max(1, j - badChar[text[s + j]]);
        }
    }
    return false;
}

string airportsearch(const string &pattern){
    ifstream infile("airportlist.txt");
    if (!infile.is_open()) {
        return ""; // File not found
    }
        string line;

        // Skip header line
        getline(infile, line);

        while (getline(infile, line))
        {
            if (line.empty() || line[0] == '#') continue;
            string code,name,loc;

            stringstream ss(line);
            getline(ss, code, ';');
            getline(ss, name, ';');
            getline(ss, loc, ';');

            string text = code + " " + toUpperCase(name) + " " + toUpperCase(loc);

            if (boyerMooreSearch(text, toUpperCase(pattern))) {
                return code;
            }
        }
    return "";
}

void createmenu()
{
    cout << "\n\n============================================\n";
    cout << "   Welcome to Flight Management System\n";
    cout << "============================================\n\n";

    cout << "Please enter the source airport code (e.g., DEL): ";
    cin >> source;
    //source = toUpperCase(source);

    cout << "Please enter the destination airport code (e.g., MUM): ";
    cin >> dest;
    //dest = toUpperCase(dest);
    srccode = airportsearch(source);
    destcode = airportsearch(dest);

    bool foundsource = srccode.empty() ? false : true;
    bool founddestination = destcode.empty() ? false : true;

    if(!founddestination||!foundsource)
    {
        cout << " - Our services do not yet extend to the entered airport codes. -" << endl;
        cout << " - We sincerely apologize for any inconvenience caused. -" << endl;
        createmenu();
    }

    cout << "\nYou have selected:\n";
    cout << "Source Airport: " << srccode << endl;
    cout << "Destination Airport: " << destcode << endl;

    // control.displayscheduledflightsFromTo(srccode,destcode);

    cout << "\nThank you for using the Flight Management System!\n";
    //Clear();
    addloadingdots();

}


int main()
{
    AirportNetwork network;

    createmenu();

    // Load data
    cout<<"Creating airport objects and loading details.\n";
    network.loadAirportDetailsFromFile("airportdetails.txt");
    addloadingdots();

    cout<<"Displaying Source and Destination Airport details.\n";
    addloadingdots();
    network.displayairportdetails(srccode);
    addloadingdots();
    network.displayairportdetails(destcode);

    cout<<"\nEstablishing and displaying airport network.\n";
    network.loadNetworkFromFile("airportnetwork.txt");
    addloadingdots();
    
    network.showNetwork(); // Display the loaded network

    // Compute paths
    cout<<"\nPrecomputing optimal fligh paths.\n";
    addloadingdots();

    network.computeShortestPaths();
    network.computeTimeShortestPaths();

    // Demonstrate path finding
    // cout << "\n=== Path Finding Demo ===" << endl;
    // network.printAllShortestDistances();
    // network.printAllShortestTimes();

    // Show specific routes
    // network.printPathBetween("DEL", "MUM");
    // network.printTimeOptimizedPathBetween("DEL", "MUM");
    // network.printPathBetween("DEL", "BLR");
    // network.printTimeOptimizedPathBetween("DEL", "BLR");

    // network.displayairportdetails();

    // Schedule flights
    ATC control;
    // cout<<"Loading and Scheduling all the flights.\\n";
    // addloadingdots();

    control.loadFlightsFromFile("flights.txt", &network);
    //control.scheduleAllFlights(&network);

    control.scheduleAllFlights(&network,srccode,destcode);
    control.displayscheduledflightsFromTo(srccode,destcode);


    // control.checktestcase(&network);
    // control.scheduleAllFlights(&network);


    // network.displayairportdetails();

    // string src,dest;
    // cout<<"enter source airport : ";
    // getline(cin,src);
    // cout<<"enter destination airport : ";
    // getline(cin,dest);

    // control.displaysheduleflights();
    
    // control.displayscheduledflights(source,dest);

    return 0;
}

/*
You need to make sure AirportNetwork.cpp is compiled and linked along with main.cpp.
Otherwise, the linker doesn't know where the actual blr object is.
*/

/*
Airport.h	Defines Airport class.
Airport.cpp	Implements Airport class functions.
AirportNetwork.h	Declares AirportNetwork class + Route struct + extern Airports.
AirportNetwork.cpp	Defines AirportNetwork functions.
main.cpp	Creates AirportNetwork object and uses it (for example, loadNetworkFromFile(), showNetwork(), etc).
*/