#include <bits/stdc++.h>
using namespace std;

/*
==============================================================
             SMART TRAFFIC FLOW SIMULATOR
                    USING C++
==============================================================

*/

// ------------------------------------------------------------
// CONSTANTS
// ------------------------------------------------------------

const int ROAD_COUNT = 4;
const int MAX_VEHICLES = 500;

// ------------------------------------------------------------
// ROAD ARRAY
// ------------------------------------------------------------

string roadNames[ROAD_COUNT] =
{
    "North Road",
    "South Road",
    "East Road",
    "West Road"
};

// ------------------------------------------------------------
// 1D ARRAYS
// ------------------------------------------------------------

int roadVehicleCount[ROAD_COUNT] = {0, 0, 0, 0};
int roadCapacity[ROAD_COUNT] = {25, 30, 28, 32};
int signalTime[ROAD_COUNT] = {30, 30, 30, 30};

// ------------------------------------------------------------
// 2D ARRAY - TRAFFIC FLOW MATRIX
// ------------------------------------------------------------

int trafficMatrix[ROAD_COUNT][ROAD_COUNT] =
{
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
};

// ------------------------------------------------------------
// STRUCTURE: VEHICLE
// ------------------------------------------------------------

struct Vehicle
{
    int id;
    string number;
    string type;

    int source;
    int destination;
    int currentRoad;

    int arrivalMinute;
    int waitingTime;

    double speed;
    double distance;

    bool emergency;
    bool active;
    bool rerouted;
};

// ------------------------------------------------------------
// STRUCTURE: JUNCTION
// ------------------------------------------------------------

struct Junction
{
    int id;
    string name;

    int road1;
    int road2;

    int activeRoad;
    int greenTime;
};

// ------------------------------------------------------------
// STRUCTURE: EVENT
// ------------------------------------------------------------

struct TrafficEvent
{
    int minute;
    string type;
    string description;
};

// ------------------------------------------------------------
// VECTOR
// ------------------------------------------------------------

vector<Vehicle> vehicles;

// ------------------------------------------------------------
// QUEUES
// ------------------------------------------------------------

queue<int> roadQueues[ROAD_COUNT];
queue<int> emergencyQueue;

// ------------------------------------------------------------
// STACK
// ------------------------------------------------------------

stack<TrafficEvent> eventStack;

// ------------------------------------------------------------
// DEQUE
// ------------------------------------------------------------

deque<int> recentVehicles;

// ------------------------------------------------------------
// MAP
// ------------------------------------------------------------

map<string, int> vehicleTypeCount;
map<string, int> weatherCount;

// ------------------------------------------------------------
// SET
// ------------------------------------------------------------

set<string> destinations;

// ------------------------------------------------------------
// PAIR
// ------------------------------------------------------------

vector<pair<string, int>> roadPairs;

// ------------------------------------------------------------
// JUNCTION ARRAY
// ------------------------------------------------------------

Junction junctions[4];

// ------------------------------------------------------------
// GLOBAL VARIABLES
// ------------------------------------------------------------

int currentMinute = 0;
int nextVehicleID = 1;

int totalGenerated = 0;
int totalProcessed = 0;
int totalEmergency = 0;
int totalRerouted = 0;
int totalAccidents = 0;

long long totalWaitingTime = 0;
double totalDistance = 0;

string currentWeather = "Clear";

// ------------------------------------------------------------
// RANDOM NUMBER
// ------------------------------------------------------------

int randomNumber(int low, int high)
{
    return low + rand() % (high - low + 1);
}

// ------------------------------------------------------------
// EVENT FUNCTION
// ------------------------------------------------------------

void addEvent(string type, string description)
{
    TrafficEvent e;

    e.minute = currentMinute;
    e.type = type;
    e.description = description;

    eventStack.push(e);
}

// ------------------------------------------------------------
// INITIALIZE JUNCTIONS
// ------------------------------------------------------------

void initializeJunctions()
{
    for (int i = 0; i < 4; i++)
    {
        junctions[i].id = i + 1;
        junctions[i].name =
            "Junction " + to_string(i + 1);

        junctions[i].road1 = i;
        junctions[i].road2 = (i + 1) % ROAD_COUNT;

        junctions[i].activeRoad = i;
        junctions[i].greenTime = 30;
    }
}

// ------------------------------------------------------------
// WEATHER
// ------------------------------------------------------------

string generateWeather()
{
    int value = randomNumber(1, 100);

    if (value <= 60)
        return "Clear";

    if (value <= 80)
        return "Cloudy";

    if (value <= 95)
        return "Rain";

    return "Heavy Rain";
}

// ------------------------------------------------------------
// VEHICLE TYPE
// ------------------------------------------------------------

string generateVehicleType()
{
    int value = randomNumber(1, 100);

    if (value <= 50)
        return "Car";

    if (value <= 70)
        return "Bike";

    if (value <= 85)
        return "Bus";

    if (value <= 95)
        return "Truck";

    return "Emergency";
}

// ------------------------------------------------------------
// VEHICLE NUMBER
// ------------------------------------------------------------

string generateVehicleNumber(int id)
{
    return "TS-" +
           to_string(10 + id % 90) +
           "-" +
           to_string(1000 + id);
}

// ------------------------------------------------------------
// CREATE VEHICLE
// ------------------------------------------------------------

Vehicle createVehicle()
{
    Vehicle v;

    v.id = nextVehicleID++;

    v.number =
        generateVehicleNumber(v.id);

    v.type =
        generateVehicleType();

    v.source =
        randomNumber(0, ROAD_COUNT - 1);

    do
    {
        v.destination =
            randomNumber(0, ROAD_COUNT - 1);
    }
    while (v.destination == v.source);

    v.currentRoad = v.source;

    v.arrivalMinute =
        currentMinute;

    v.waitingTime = 0;

    v.speed =
        randomNumber(30, 60);

    v.distance =
        randomNumber(1, 5);

    v.emergency =
        (v.type == "Emergency");

    v.active = true;
    v.rerouted = false;

    return v;
}

// ------------------------------------------------------------
// ADD VEHICLE
// ------------------------------------------------------------

void addVehicle(Vehicle v)
{
    if ((int)vehicles.size() >= MAX_VEHICLES)
        return;

    int index = vehicles.size();

    vehicles.push_back(v);

    roadQueues[v.currentRoad].push(index);

    roadVehicleCount[v.currentRoad]++;

    trafficMatrix
        [v.source]
        [v.destination]++;

    vehicleTypeCount[v.type]++;

    destinations.insert(
        roadNames[v.destination]
    );

    totalGenerated++;

    recentVehicles.push_back(v.id);

    if (recentVehicles.size() > 10)
        recentVehicles.pop_front();

    if (v.emergency)
    {
        emergencyQueue.push(index);

        totalEmergency++;

        addEvent(
            "EMERGENCY",
            "Emergency vehicle " +
            v.number +
            " entered " +
            roadNames[v.currentRoad]
        );
    }
}

// ------------------------------------------------------------
// GENERATE TRAFFIC
// ------------------------------------------------------------

void generateTraffic()
{
    int count;

    if (currentMinute >= 4 &&
        currentMinute <= 7)
    {
        count = randomNumber(7, 10);
    }
    else
    {
        count = randomNumber(3, 6);
    }

    if (currentWeather == "Rain")
        count++;

    if (currentWeather == "Heavy Rain")
        count += 2;

    for (int i = 0; i < count; i++)
    {
        Vehicle v = createVehicle();

        addVehicle(v);
    }
}

// ------------------------------------------------------------
// UPDATE WEATHER
// ------------------------------------------------------------

void updateWeather()
{
    currentWeather =
        generateWeather();

    weatherCount[currentWeather]++;

    addEvent(
        "WEATHER",
        "Weather changed to " +
        currentWeather
    );
}

// ------------------------------------------------------------
// UPDATE WAITING TIME
// ------------------------------------------------------------

void updateWaitingTime()
{
    for (Vehicle &v : vehicles)
    {
        if (v.active)
        {
            v.waitingTime++;

            totalWaitingTime++;
        }
    }
}

// ------------------------------------------------------------
// ROAD DENSITY
// ------------------------------------------------------------

double getDensity(int road)
{
    return
        ((double)roadVehicleCount[road] /
         roadCapacity[road]) * 100.0;
}

// ------------------------------------------------------------
// SIGNAL CONTROL
// ------------------------------------------------------------

void updateSignals()
{
    for (int i = 0; i < ROAD_COUNT; i++)
    {
        double density =
            getDensity(i);

        if (density >= 90)
            signalTime[i] = 60;

        else if (density >= 70)
            signalTime[i] = 50;

        else if (density >= 45)
            signalTime[i] = 40;

        else
            signalTime[i] = 30;
    }

    for (int i = 0; i < 4; i++)
    {
        int r1 = junctions[i].road1;
        int r2 = junctions[i].road2;

        if (getDensity(r1) >=
            getDensity(r2))
        {
            junctions[i].activeRoad = r1;
        }
        else
        {
            junctions[i].activeRoad = r2;
        }

        junctions[i].greenTime =
            signalTime[
                junctions[i].activeRoad
            ];
    }
}

// ------------------------------------------------------------
// PROCESS VEHICLE
// ------------------------------------------------------------

void processVehicle(int index)
{
    if (index < 0 ||
        index >= (int)vehicles.size())
        return;

    if (!vehicles[index].active)
        return;

    int road =
        vehicles[index].currentRoad;

    if (roadVehicleCount[road] > 0)
        roadVehicleCount[road]--;

    vehicles[index].active = false;

    vehicles[index].speed =
        40 + randomNumber(0, 20);

    totalProcessed++;

    totalDistance +=
        vehicles[index].distance;
}

// ------------------------------------------------------------
// PROCESS EMERGENCY VEHICLES
// ------------------------------------------------------------

void processEmergencyVehicles()
{
    int count =
        emergencyQueue.size();

    for (int i = 0; i < count; i++)
    {
        int index =
            emergencyQueue.front();

        emergencyQueue.pop();

        if (index < 0 ||
            index >= (int)vehicles.size())
            continue;

        if (!vehicles[index].active)
            continue;

        processVehicle(index);

        addEvent(
            "SIGNAL PRIORITY",
            "Emergency vehicle " +
            vehicles[index].number +
            " received priority"
        );
    }
}

// ------------------------------------------------------------
// PROCESS NORMAL VEHICLES
// ------------------------------------------------------------

void processNormalVehicles()
{
    for (int road = 0;
         road < ROAD_COUNT;
         road++)
    {
        int capacity =
            max(1, signalTime[road] / 10);

        int count = 0;

        while (!roadQueues[road].empty() &&
               count < capacity)
        {
            int index =
                roadQueues[road].front();

            roadQueues[road].pop();

            if (index < 0 ||
                index >= (int)vehicles.size())
                continue;

            if (!vehicles[index].active)
                continue;

            if (vehicles[index].emergency)
                continue;

            processVehicle(index);

            count++;
        }
    }
}

// ------------------------------------------------------------
// ACCIDENT
// ------------------------------------------------------------

void generateAccident()
{
    int chance =
        randomNumber(1, 100);

    if (chance <= 15)
    {
        int road =
            randomNumber(0, ROAD_COUNT - 1);

        addEvent(
            "ACCIDENT",
            "Accident detected on " +
            roadNames[road]
        );

        totalAccidents++;
    }
}

// ------------------------------------------------------------
// REROUTING
// ------------------------------------------------------------

void rerouteVehicle()
{
    if (vehicles.empty())
        return;

    int chance =
        randomNumber(1, 100);

    if (chance <= 20)
    {
        int index =
            randomNumber(
                0,
                vehicles.size() - 1
            );

        if (vehicles[index].active &&
            !vehicles[index].rerouted)
        {
            int oldRoad =
                vehicles[index].currentRoad;

            int newRoad =
                (oldRoad + 1) %
                ROAD_COUNT;

            vehicles[index].currentRoad =
                newRoad;

            vehicles[index].rerouted =
                true;

            if (roadVehicleCount[oldRoad] > 0)
                roadVehicleCount[oldRoad]--;

            roadVehicleCount[newRoad]++;

            totalRerouted++;

            addEvent(
                "REROUTE",
                "Vehicle " +
                vehicles[index].number +
                " changed route from " +
                roadNames[oldRoad] +
                " to " +
                roadNames[newRoad]
            );
        }
    }
}

// ============================================================
// INITIAL DEMO DATA
// ============================================================

void initializeDemoData()
{
    // Initial road traffic so every menu option works immediately.
    roadVehicleCount[0] = 8;
    roadVehicleCount[1] = 12;
    roadVehicleCount[2] = 6;
    roadVehicleCount[3] = 10;

    // Initial 2D traffic-flow matrix.
    int initialMatrix[ROAD_COUNT][ROAD_COUNT] =
    {
        {0, 3, 2, 3},
        {4, 0, 5, 3},
        {2, 4, 0, 2},
        {3, 2, 4, 0}
    };

    for (int i = 0; i < ROAD_COUNT; i++)
        for (int j = 0; j < ROAD_COUNT; j++)
            trafficMatrix[i][j] = initialMatrix[i][j];

    // Demo vehicles. These are loaded before the menu appears.
    vector<Vehicle> demoVehicles =
    {
        {101, "TS-21-1101", "Car",       0, 1, 0, 0, 3, 45, 3.0, false, true,  false},
        {102, "TS-22-1102", "Bike",      1, 2, 1, 0, 5, 52, 2.0, false, true,  false},
        {103, "TS-23-1103", "Bus",       2, 3, 2, 0, 7, 35, 4.0, false, true,  false},
        {104, "TS-24-1104", "Truck",     3, 0, 3, 0, 6, 40, 5.0, false, true,  false},
        {105, "TS-25-1105", "Car",       0, 2, 1, 0, 8, 48, 3.5, false, true,  true },
        {106, "TS-26-1106", "Bike",      1, 3, 1, 0, 4, 55, 2.5, false, true,  false},
        {107, "TS-27-1107", "Emergency", 2, 0, 0, 0, 2, 60, 4.5, true,  true,  false},
        {108, "TS-28-1108", "Car",       3, 1, 3, 0, 9, 42, 3.0, false, true,  false}
    };

    vehicles.clear();
    while (!emergencyQueue.empty()) emergencyQueue.pop();
    for (int i = 0; i < ROAD_COUNT; i++)
        while (!roadQueues[i].empty()) roadQueues[i].pop();

    vehicleTypeCount.clear();
    destinations.clear();
    recentVehicles.clear();

    totalGenerated = 0;
    totalProcessed = 0;
    totalEmergency = 0;
    totalRerouted = 1;
    totalAccidents = 1;
    totalWaitingTime = 0;
    totalDistance = 0;
    nextVehicleID = 109;
    currentMinute = 0;
    currentWeather = "Clear";
    weatherCount.clear();
    weatherCount["Clear"] = 1;

    for (Vehicle v : demoVehicles)
    {
        int index = vehicles.size();
        vehicles.push_back(v);

        roadQueues[v.currentRoad].push(index);
        vehicleTypeCount[v.type]++;
        destinations.insert(roadNames[v.destination]);
        recentVehicles.push_back(v.id);

        totalGenerated++;
        totalWaitingTime += v.waitingTime;
        totalDistance += v.distance;

        if (v.emergency)
        {
            emergencyQueue.push(index);
            totalEmergency++;
        }
    }

    while (!eventStack.empty()) eventStack.pop();

    addEvent("SYSTEM", "Traffic network initialized successfully");
    addEvent("TRAFFIC", "Demo traffic data loaded for presentation");
    addEvent("SIGNAL", "Adaptive signal control initialized");
    addEvent("EMERGENCY", "Emergency vehicle priority lane activated");
    addEvent("ACCIDENT", "Demo accident recorded on East Road");
    addEvent("REROUTE", "Vehicle TS-25-1105 rerouted to avoid congestion");

    updateSignals();
}

// ============================================================
// OPTION 2
// SEARCH VEHICLE
// ============================================================

void searchVehicle()
{
    cout << "\n";
    cout << "============================================================\n";
    cout << "                    SEARCH VEHICLE\n";
    cout << "============================================================\n";

    if (vehicles.empty())
    {
        cout << "\nNo vehicles available yet.\n";
        cout << "Start the simulation using Option 1.\n";
        return;
    }

    cout << "\nAvailable Vehicle IDs:\n";

    for (const Vehicle &v : vehicles)
    {
        cout << v.id << " ";
    }

    cout << "\n\nEnter Vehicle ID: ";

    int id;
    cin >> id;

    int index = -1;

    for (int i = 0;
         i < (int)vehicles.size();
         i++)
    {
        if (vehicles[i].id == id)
        {
            index = i;
            break;
        }
    }

    if (index == -1)
    {
        cout << "\nVehicle not found.\n";
        return;
    }

    Vehicle v =
        vehicles[index];

    cout << "\n---------------- VEHICLE DETAILS ----------------\n";

    cout << "Vehicle ID       : "
         << v.id << "\n";

    cout << "Vehicle Number   : "
         << v.number << "\n";

    cout << "Vehicle Type     : "
         << v.type << "\n";

    cout << "Source Road      : "
         << roadNames[v.source] << "\n";

    cout << "Destination Road : "
         << roadNames[v.destination] << "\n";

    cout << "Current Road     : "
         << roadNames[v.currentRoad] << "\n";

    cout << "Arrival Minute   : "
         << v.arrivalMinute << "\n";

    cout << "Waiting Time     : "
         << v.waitingTime << " minutes\n";

    cout << "Speed            : "
         << v.speed << " km/h\n";

    cout << "Distance         : "
         << v.distance << " km\n";

    cout << "Emergency        : "
         << (v.emergency ? "YES" : "NO")
         << "\n";

    cout << "Rerouted         : "
         << (v.rerouted ? "YES" : "NO")
         << "\n";

    cout << "Status           : "
         << (v.active ? "ACTIVE" : "PROCESSED")
         << "\n";
}

// ============================================================
// OPTION 3
// TRAFFIC MATRIX
// ============================================================

void displayTrafficMatrix()
{
    cout << "\n";
    cout << "============================================================\n";
    cout << "                  TRAFFIC FLOW MATRIX\n";
    cout << "============================================================\n\n";

    cout << left
         << setw(18)
         << "FROM / TO";

    for (int i = 0; i < ROAD_COUNT; i++)
    {
        cout << setw(14)
             << ("R" + to_string(i + 1));
    }

    cout << "\n";

    cout << "------------------------------------------------------------\n";

    for (int i = 0; i < ROAD_COUNT; i++)
    {
        cout << left
             << setw(18)
             << ("R" + to_string(i + 1));

        for (int j = 0; j < ROAD_COUNT; j++)
        {
            cout << setw(14)
                 << trafficMatrix[i][j];
        }

        cout << "\n";
    }

    cout << "\nRoad Mapping:\n";

    for (int i = 0; i < ROAD_COUNT; i++)
    {
        cout << "R"
             << i + 1
             << " = "
             << roadNames[i]
             << "\n";
    }

    cout << "\nTraffic matrix displayed successfully.\n";
}

// ============================================================
// OPTION 4
// JUNCTION STATUS
// ============================================================

void displayJunctionStatus()
{
    cout << "\n";
    cout << "============================================================\n";
    cout << "                    JUNCTION STATUS\n";
    cout << "============================================================\n";

    for (int i = 0; i < 4; i++)
    {
        cout << "\n"
             << junctions[i].name << "\n";

        cout << "Connected Roads : "
             << roadNames[junctions[i].road1]
             << " <-> "
             << roadNames[junctions[i].road2]
             << "\n";

        cout << "Active Road     : "
             << roadNames[junctions[i].activeRoad]
             << "\n";

        cout << "Green Signal    : "
             << junctions[i].greenTime
             << " seconds\n";
    }
}

// ============================================================
// OPTION 5
// VEHICLE STATISTICS
// ============================================================

void displayVehicleStatistics()
{
    cout << "\n";
    cout << "============================================================\n";
    cout << "                  VEHICLE STATISTICS\n";
    cout << "============================================================\n";

    if (vehicleTypeCount.empty())
    {
        cout << "\nNo vehicles generated yet.\n";
        cout << "Run Option 1 for dynamic statistics.\n";
        return;
    }

    for (auto item : vehicleTypeCount)
    {
        cout << left
             << setw(15)
             << item.first
             << " : "
             << item.second
             << "\n";
    }

    cout << "\nTotal Vehicles : "
         << totalGenerated << "\n";

    cout << "Processed       : "
         << totalProcessed << "\n";

    cout << "Destinations    : "
         << destinations.size() << "\n";
}

// ============================================================
// OPTION 6
// RECENT VEHICLES
// ============================================================

void displayRecentVehicles()
{
    cout << "\n";
    cout << "============================================================\n";
    cout << "                    RECENT VEHICLES\n";
    cout << "============================================================\n";

    if (recentVehicles.empty())
    {
        cout << "\nNo recent vehicles.\n";
        cout << "Run Option 1 to generate vehicles.\n";
        return;
    }

    for (int id : recentVehicles)
    {
        for (const Vehicle &v : vehicles)
        {
            if (v.id == id)
            {
                cout << "ID: "
                     << v.id
                     << " | "
                     << v.number
                     << " | "
                     << v.type
                     << "\n";

                break;
            }
        }
    }
}

// ============================================================
// OPTION 7
// EVENT HISTORY
// ============================================================

void displayEventHistory()
{
    cout << "\n";
    cout << "============================================================\n";
    cout << "                    EVENT HISTORY\n";
    cout << "============================================================\n";

    if (eventStack.empty())
    {
        cout << "\nNo events recorded yet.\n";
        cout << "Run Option 1 for simulation events.\n";
        return;
    }

    stack<TrafficEvent> temp =
        eventStack;

    int count = 0;

    while (!temp.empty() &&
           count < 15)
    {
        TrafficEvent e =
            temp.top();

        temp.pop();

        cout << "\n[Minute "
             << e.minute
             << "] "
             << e.type
             << "\n";

        cout << e.description
             << "\n";

        count++;
    }
}

// ============================================================
// OPTION 8
// HIGHEST WAITING VEHICLES
// ============================================================

void displayHighestWaiting()
{
    cout << "\n";
    cout << "============================================================\n";
    cout << "              HIGHEST WAITING VEHICLES\n";
    cout << "============================================================\n";

    if (vehicles.empty())
    {
        cout << "\nNo vehicles available.\n";
        return;
    }

    vector<Vehicle> temp =
        vehicles;

    sort(
        temp.begin(),
        temp.end(),
        [](Vehicle a, Vehicle b)
        {
            return
                a.waitingTime >
                b.waitingTime;
        }
    );

    int count =
        min(5, (int)temp.size());

    for (int i = 0;
         i < count;
         i++)
    {
        cout << "\n"
             << i + 1
             << ". Vehicle "
             << temp[i].id
             << " | "
             << temp[i].number
             << " | Waiting: "
             << temp[i].waitingTime
             << " minutes";
    }

    cout << "\n";
}

// ============================================================
// OPTION 9
// ROAD STATUS
// ============================================================

void displayRoadStatus()
{
    cout << "\n";
    cout << "============================================================\n";
    cout << "                     ROAD STATUS\n";
    cout << "============================================================\n\n";

    cout << left
         << setw(16) << "ROAD"
         << setw(12) << "VEHICLES"
         << setw(12) << "CAPACITY"
         << setw(12) << "DENSITY"
         << setw(12) << "SIGNAL"
         << "\n";

    cout << "------------------------------------------------------------\n";

    for (int i = 0; i < ROAD_COUNT; i++)
    {
        double density =
            getDensity(i);

        cout << left
             << setw(16)
             << roadNames[i]
             << setw(12)
             << roadVehicleCount[i]
             << setw(12)
             << roadCapacity[i]
             << setw(10)
             << fixed
             << setprecision(1)
             << density
             << "% "
             << setw(10)
             << signalTime[i]
             << " sec\n";
    }
}

// ============================================================
// OPTION 10
// COMPLEXITY
// ============================================================

void displayComplexity()
{
    cout << "\n";
    cout << "============================================================\n";
    cout << "                  COMPLEXITY ANALYSIS\n";
    cout << "============================================================\n\n";

    cout << left
         << setw(32)
         << "Operation"
         << "Complexity\n";

    cout << "------------------------------------------------------------\n";

    cout << setw(32)
         << "Array Access"
         << "O(1)\n";

    cout << setw(32)
         << "2D Matrix Access"
         << "O(1)\n";

    cout << setw(32)
         << "Queue Enqueue"
         << "O(1)\n";

    cout << setw(32)
         << "Queue Dequeue"
         << "O(1)\n";

    cout << setw(32)
         << "Stack Push"
         << "O(1)\n";

    cout << setw(32)
         << "Stack Pop"
         << "O(1)\n";

    cout << setw(32)
         << "Vector Access"
         << "O(1)\n";

    cout << setw(32)
         << "Map Search"
         << "O(log n)\n";

    cout << setw(32)
         << "Set Search"
         << "O(log n)\n";

    cout << setw(32)
         << "Vehicle Search"
         << "O(n)\n";

    cout << setw(32)
         << "Sorting"
         << "O(n log n)\n";

    cout << "\nSpace Complexity:\n";

    cout << "Vehicle Vector : O(n)\n";
    cout << "Queues         : O(n)\n";
    cout << "Event Stack    : O(e)\n";
    cout << "Matrix         : O(1)\n";
}

// ============================================================
// OPTION 11
// FINAL STATISTICS
// ============================================================

void displayFinalStatistics()
{
    cout << "\n";
    cout << "============================================================\n";
    cout << "                  FINAL STATISTICS\n";
    cout << "============================================================\n";

    int active = 0;

    for (const Vehicle &v : vehicles)
    {
        if (v.active)
            active++;
    }

    cout << "\nSimulation Minute    : "
         << currentMinute;

    cout << "\nCurrent Weather      : "
         << currentWeather;

    cout << "\nTotal Generated      : "
         << totalGenerated;

    cout << "\nTotal Processed      : "
         << totalProcessed;

    cout << "\nActive Vehicles      : "
         << active;

    cout << "\nEmergency Vehicles   : "
         << totalEmergency;

    cout << "\nRerouted Vehicles     : "
         << totalRerouted;

    cout << "\nAccidents            : "
         << totalAccidents;

    cout << "\nTotal Waiting Time   : "
         << totalWaitingTime
         << " vehicle-minutes";

    cout << fixed
         << setprecision(2);

    cout << "\nTotal Distance       : "
         << totalDistance
         << " km";

    if (totalGenerated > 0)
    {
        cout << "\nAverage Waiting Time : "
             << (double)totalWaitingTime /
                totalGenerated
             << " minutes";
    }

    cout << "\n";
}

// ============================================================
// PAIR DISPLAY
// ============================================================

void displayPairs()
{
    roadPairs.clear();

    for (int i = 0;
         i < ROAD_COUNT;
         i++)
    {
        roadPairs.push_back(
            make_pair(
                roadNames[i],
                roadVehicleCount[i]
            )
        );
    }

    cout << "\nRoad - Vehicle Pairs:\n";

    for (auto p : roadPairs)
    {
        cout << "("
             << p.first
             << ", "
             << p.second
             << " vehicles)\n";
    }
}

// ============================================================
// SIMULATION
// ============================================================

void runSimulation()
{
    int minutes;

    cout << "\n============================================================\n";
    cout << "                START TRAFFIC SIMULATION\n";
    cout << "============================================================\n";

    cout << "\nEnter simulation duration (10-20 minutes): ";

    cin >> minutes;

    if (cin.fail())
    {
        cin.clear();

        cin.ignore(
            numeric_limits<streamsize>::max(),
            '\n'
        );

        cout << "\nInvalid input.\n";
        return;
    }

    if (minutes < 10)
        minutes = 10;

    if (minutes > 20)
        minutes = 20;

    cout << "\nSimulation started for "
         << minutes
         << " minutes.\n";

    for (int i = 0;
         i < minutes;
         i++)
    {
        currentMinute++;

        updateWeather();

        generateTraffic();

        updateWaitingTime();

        generateAccident();

        rerouteVehicle();

        updateSignals();

        processEmergencyVehicles();

        processNormalVehicles();

        cout << "\nMinute "
             << currentMinute
             << " completed"
             << " | Weather: "
             << currentWeather
             << " | Vehicles: "
             << totalGenerated;
    }

    cout << "\n\n============================================================\n";
    cout << "              SIMULATION COMPLETED\n";
    cout << "============================================================\n";

    displayFinalStatistics();
}

// ============================================================
// MENU
// ============================================================

void showMenu()
{
    cout << "\n\n";

    cout << "############################################################\n";
    cout << "#             SMART TRAFFIC FLOW SIMULATOR                #\n";
    cout << "############################################################\n";

    cout << "\n";
    cout << "1.  Start Traffic Simulation\n";
    cout << "2.  Search Vehicle\n";
    cout << "3.  Display Traffic Matrix\n";
    cout << "4.  Display Junction Status\n";
    cout << "5.  Display Vehicle Statistics\n";
    cout << "6.  Display Recent Vehicles\n";
    cout << "7.  Display Event History\n";
    cout << "8.  Display Highest Waiting Vehicles\n";
    cout << "9.  Display Road Status\n";
    cout << "10. Display Complexity Analysis\n";
    cout << "11. Display Final Statistics\n";
    cout << "12. Display Road Pairs\n";
    cout << "0.  Exit\n";

    cout << "\nEnter your choice: ";
}

// ============================================================
// MAIN
// ============================================================

int main()
{
    srand(
        (unsigned)time(nullptr)
    );

    initializeJunctions();
    initializeDemoData();

    cout << "\n============================================================\n";
    cout << "          SMART TRAFFIC FLOW SIMULATOR USING C++\n";
    cout << "============================================================\n";

    cout << "\nSystem initialized successfully.\n";
    cout << "4 Roads and 4 Junctions are ready.\n";
    cout << "Demo data loaded: 8 vehicles, traffic matrix, events and statistics.\n";
    cout << "All menu options are ready to use independently.\n";

    int choice;

    while (true)
    {
        showMenu();

        cin >> choice;

        if (cin.fail())
        {
            cin.clear();

            cin.ignore(
                numeric_limits<streamsize>::max(),
                '\n'
            );

            cout << "\nInvalid input. Please enter a number.\n";

            continue;
        }

        switch (choice)
        {
            case 1:
                runSimulation();
                break;

            case 2:
                searchVehicle();
                break;

            case 3:
                displayTrafficMatrix();
                break;

            case 4:
                displayJunctionStatus();
                break;

            case 5:
                displayVehicleStatistics();
                break;

            case 6:
                displayRecentVehicles();
                break;

            case 7:
                displayEventHistory();
                break;

            case 8:
                displayHighestWaiting();
                break;

            case 9:
                displayRoadStatus();
                break;

            case 10:
                displayComplexity();
                break;

            case 11:
                displayFinalStatistics();
                break;

            case 12:
                displayPairs();
                break;

            case 0:
                cout << "\n============================================================\n";
                cout << "              PROGRAM TERMINATED\n";
                cout << "============================================================\n";
                return 0;

            default:
                cout << "\nInvalid choice. Select 0-12.\n";
        }
    }

    return 0;
}