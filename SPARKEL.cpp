/*LIBRARY*/

#include <iostream>
#include <vector> // array dinamis 
#include <string>
#include <cstdlib> // untuk menggunakan fungsi system("pause")
#include <algorithm>
#include <fstream> // Menyimpan dan mengakses data fin
#include <ctime> //  proses hitung waktu
#include <regex> // mencari pattern dari plat nomor kendaraan
#include <conio.h>
#include <iomanip>

#ifdef _WIN32
#define CLEAR_SCREEN "cls"
#else
#define CLEAR_SCREEN "clear"
#endif

using namespace std;

/*STRUCTURE DATA*/

int floorWeight = 10;

struct Account { 
    string username;
    string password;
    bool operator==(const Account& other) const {
        return username == other.username && password == other.password;
    }
};

vector<Account> user_list;
vector<Account> admin_list;

struct Vehicle {
    string username;
    string plateNumber;
    string type;
    time_t timeIn; 
    time_t timeOut;
    float spendTime;
    float parkingRates;
    string building;
    int floor;
    string area;
    int spot;
    Vehicle* next;
};

vector<Vehicle> parkingData;

struct SetVehicle {
    string type;
    float parkingRates;
};

SetVehicle vehicleType[] = {
    {"motor", 2000},
    {"mobil", 5000},
    {"truk", 5000},
    {"bus", 5000}
};

int totalVehicleType = sizeof(vehicleType) / sizeof(vehicleType[0]);

struct Stack {
    Vehicle *top;

    Stack() {
        top = nullptr;
    }

    void push(Vehicle vehicle) {
        Vehicle *newVehicle = new Vehicle(vehicle);
        newVehicle->next = top;
        top = newVehicle;
    }

    Vehicle pop() {
        if (top == nullptr) {
            throw "Stack is empty";
        }

        Vehicle vehicle = *top;
        Vehicle *temp = top;
        top = top->next;
        delete temp;

        return vehicle;
    }

    bool isEmpty() {
        return top == nullptr;
    }

    int size() {
        int count = 0;
        Vehicle *temp = top;
        while (temp != nullptr) {
            count++;
            temp = temp->next;
        }
        return count;
    }
};

struct Queue {
    Vehicle *front, *rear;
    int capacity;

    Queue() {}

    Queue(int n) {
        front = rear = nullptr;
        capacity = n;
    }

    bool isFull() {
        int count = 0;
        Vehicle* temp = front;
        while (temp != nullptr) {
            count++;
            temp = temp->next;
        }
        return count == capacity;
    }

    bool isEmpty() {
        return front == nullptr;
    }

    void enqueue(Vehicle* temp) {
        if (isFull()) {
            cout << "Maaf, antrian sudah penuh" << endl;
        } else {
            if (rear == nullptr) {
                front = rear = temp;
            } else {
                rear->next = temp;
                rear = temp;
            }
            cout << "Kendaraan dengan nomor plat " << temp->plateNumber << " berhasil ditambahkan ke antrian." << endl;
        }
    }

    void dequeue() {
        if (!isEmpty()) {
            Vehicle* temp = front;
            front = front->next;
            if (front == nullptr) {
                rear = nullptr;
            }
            cout << "Kendaraan dengan nomor plat " << temp->plateNumber << " telah masuk ke parkiran." << endl;
            delete temp;
        }
    }

    void display() {
        if (isEmpty()) {
            cout << "Maaf, data antrian kosong" << endl;
        } else {
            Vehicle* temp = front;
            while (temp != nullptr) {
                tm timeInConversion = *localtime(&temp->timeIn);
                cout << "Nomor Plat: " << temp->plateNumber << " | Tipe Kendaraan: " << temp->type << " | Waktu Masuk: " << timeInConversion.tm_hour << ":" << timeInConversion.tm_min << ":" << timeInConversion.tm_sec << endl;
                temp = temp->next;    
            }
        }
    }

    int count() {
        int count = 0;
        Vehicle* temp = front;
        while (temp != nullptr) {
            count++;
            temp = temp->next;    
        }
        return count;
    }
};


struct Spot {
    int spotNumber;
    bool isOccupied;

    Spot() {}
    Spot(int spotNumber) : spotNumber(spotNumber), isOccupied(false) {}
};

struct Area {
    string areaName;
    string vehicleType;
    int floorNumber;
    string buildingName;
    vector<Spot> spots;

    Area() {}
    Area(string areaName, string vehicleType, int floorNumber, string buildingName) : areaName(areaName), vehicleType(vehicleType), floorNumber(floorNumber), buildingName(buildingName) {}
    
    int capacity() {
        return spots.size();
    }

    int numOccupied() {
        int count = 0;
        for (auto& spot : spots) {
            if (spot.isOccupied) {
                count++;
            }
        }
        return count;
    }

    int vacantSpot() {
        return capacity() - numOccupied();
    }

    bool isFull() {
        return numOccupied() == capacity();
    }

    bool isEmpty() {
        return numOccupied() == 0;
    }
};

struct Floor {
    int floorNumber;
    string buildingName;
    vector<Area> areas;

    Floor() {}
    Floor(int floorNumber, string buildingName) : floorNumber(floorNumber), buildingName(buildingName) {}

    int capacity() {
        int count = 0;
        for (auto& area : areas) {
            count += area.capacity();
        }
        return count;
    }

    int numOccupied() {
        int count = 0;
        for (auto& area : areas) {
            count += area.numOccupied();
        }
        return count;
    }
    
    int vacantSpot() {
        return capacity() - numOccupied();
    }

    bool isFull() {
        return numOccupied() == capacity();
    }

    bool isEmpty() {
        return numOccupied() == 0;
    }
};

struct Building {
    string buildingName;
    int queueCapacity;
    Queue queue;
    vector<Floor> floors;

    Building() {}
    Building(string buildingName, int queueCapacity) : buildingName(buildingName), queueCapacity(queueCapacity), queue(queueCapacity) {}
    
    void addFloor(int floorNumber, int numAreas, int spotsPerArea, string vehicleType) {
        Floor floor(floorNumber, buildingName);
        for (int j = 0; j < numAreas; j++) {
            string areaName = string(1, 'A' + j);
            Area area(areaName, vehicleType, floorNumber, buildingName);
            for (int k = 0; k < spotsPerArea; k++) {
                Spot spot(k + 1);
                area.spots.push_back(spot);
            }
            //floor.areas.push_back(area);
            floor.areas.insert(floor.areas.begin(), Area(area));
        }
        floors.push_back(floor);
    }

    void removeFloor(int floorIndex) {
        floors.erase(floors.begin() + floorIndex);
    }

    void addArea(int floorIndex, string areaName, int spotsPerArea, string vehicleType) {
        int floorNumber = floors[floorIndex].floorNumber;
        Area area(areaName, vehicleType, floorNumber, buildingName);
        for (int k = 0; k < spotsPerArea; k++) {
            Spot spot(k + 1);
            area.spots.push_back(spot);
        }
        //floors[floorIndex].areas.push_back(Area(area));
        floors[floorIndex].areas.insert(floors[floorIndex].areas.begin(), Area(area));
    }

    void removeArea(int floorIndex, int areaIndex) {
        floors[floorIndex].areas.erase(floors[floorIndex].areas.begin() + areaIndex);
    }

    int capacity() {
        int count = 0;
        for (auto& floor : floors) {
            count += floor.capacity();
        }
        return count;
    }

    int numOccupied() {
        int count = 0;
        for (auto& floor : floors) {
            count += floor.numOccupied();
        }
        return count;
    }

    int vacantSpot() {
        return capacity() - numOccupied();
    }

    bool isFull() {
        return numOccupied() == capacity();
    }

    bool isEmpty() {
        return numOccupied() == 0;
    }
};

vector<Building> building_list;

void defaultBuildingData() {
    building_list.clear();
    Building defaultBuilding("Gedung default", 5);
    defaultBuilding.addFloor(1, 2, 3, "motor");
    defaultBuilding.addFloor(2, 2, 3, "mobil");
    building_list.push_back(defaultBuilding);
}

template <typename T>
struct Tree {
    T value;
    Tree* left;
    Tree* right;
    bool hasValue;

    Tree() : left(nullptr), right(nullptr), hasValue(false) {}

    void insert(T newValue) {
        if (!hasValue) {
            value = newValue;
            hasValue = true;
        } else if (newValue.capacity() <= value.capacity()) {
            if (left == nullptr) {
                left = new Tree();
            }
            left->insert(newValue);
        } else if (newValue.capacity() > value.capacity()) {
            if (right == nullptr) {
                right = new Tree();
            }
            right->insert(newValue);
        }
    }

    void printVacantSpotsInOrder() {
        if (hasValue) {
            if (left != nullptr) {
                left->printVacantSpotsInOrder();
            }
            cout << value.vacantSpot() << " vacant spots" << endl;
            if (right != nullptr) {
                right->printVacantSpotsInOrder();
            }
        }
    }

    T findValueWithMostEmptySpots() {
        T result;
        int maxEmptySpots = 0;
        if (hasValue) {
            if (value.vacantSpot() > maxEmptySpots) {
                result = value;
                maxEmptySpots = value.vacantSpot();
            }
            if (left != nullptr) {
                T leftResult = left->findValueWithMostEmptySpots();
                if (leftResult.vacantSpot() > maxEmptySpots) {
                        result = leftResult;
                        maxEmptySpots = leftResult.vacantSpot();
                }
            }
            if (right != nullptr) {
                T rightResult = right->findValueWithMostEmptySpots();
                if (rightResult.vacantSpot() > maxEmptySpots) {
                        result = rightResult;
                        maxEmptySpots = rightResult.vacantSpot();
                }
            }
        }
        return result;
    }
};

vector<Building> buildings;

/*DISPLAY*/

void displayBorder(){
    int boxWidth = 50;
    char horizontal = char(205);
    for (int i = 0; i < boxWidth; i++) {
        cout << horizontal;
    }
    cout << endl;
}

void displayTitle(string title) {
    system(CLEAR_SCREEN);
    int boxWidth = 48;
    if ((title.length()%2) == 1) {
        title = title + " ";
    }
    char horizontal = char(205);
    char vertical = char(186);
    char topLeft = char(201);
    char topRight = char(187);
    char bottomLeft = char(200);
    char bottomRight = char(188);
    cout << topLeft;
    for (int i = 0; i < boxWidth; i++) {
        cout << horizontal;
    }
    cout << topRight << endl;
    cout << vertical << setw((boxWidth - title.length()) / 2) << " " << title << setw((boxWidth - title.length()) / 2) << " " << vertical << endl;
    cout << bottomLeft;
    for (int i = 0; i < boxWidth; i++) {
        cout << horizontal;
    }
    cout << bottomRight << endl;
}

int displayMenuFromVector(string name, int iBuilding, int iFloor){
    int index = 1;
    if (name == "Building") {
        for (auto& temp : building_list) {
            cout << index++ << ". " << temp.buildingName << endl;
        }
    } else if (name == "Floor") {
        for (auto& temp : building_list[iBuilding].floors) {
            cout << index++ << ". " << temp.floorNumber << endl;
        }
    } else if (name == "Area") {
        for (auto& temp : building_list[iBuilding].floors[iFloor].areas) {
            cout << index++ << ". " << temp.areaName << endl;
        }
    }
    cout << index << ". Kembali\n";
    return index;
}

void displayMenuVehicle() {
    int index = 1;
    int colWidths[] = {3, 15, 20, 6, 7, 17, 8};
    string headers[] = {"No", "Tipe kendaraan", "Tarif per jam"};

    cout << char(218);
    for (int i = 0; i < 3; i++) {
        if (i > 0) {
            cout << char(194);
        }
        for (int j = 0; j < colWidths[i]; j++) {
            cout << char(196);
        }
    }
    cout << char(191) << endl;

    for (int i = 0; i < 3; i++) {
        cout << char(179) << headers[i];
        for (int j = headers[i].length(); j < colWidths[i]; j++) {
            cout << " ";
        }
    }
    cout << char(179) << endl;

    cout << char(195);
    for (int i = 0; i < 3; i++) {
        if (i > 0) {
            cout << char(197);
        }
        for (int j = 0; j < colWidths[i]; j++) {
            cout << char(196);
        }
    }
    cout << char(180) << endl;

    for (auto& temp : vehicleType) {
        string parkingRatesStr = to_string(temp.parkingRates);
        parkingRatesStr.erase(parkingRatesStr.find_last_not_of('0') + 1);
        if (parkingRatesStr.back() == '.') {
            parkingRatesStr.pop_back();
        }

        string values[] = {to_string(index++), temp.type, "Rp" + parkingRatesStr + ",00"};
        for (int i = 0; i < 3; i++) {
            cout << char(179) << values[i];
            for (int j = values[i].length(); j < colWidths[i]; j++) {
                cout << " ";
            }
        }
        cout << char(179) << endl;
    }

    cout << char(192);
    for (int i = 0; i < 3; i++) {
        if (i > 0) {
            cout << char(193);
        }
        for (int j = 0; j < colWidths[i]; j++) {
            cout << char(196);
        }
    }
    cout << char(217) << endl;
}

/*INPUT VALIDATION*/

void removeTerminalLine(int n) { // Menghapus line di terminal
    for (int i = 0; i < n; i++) {
        cout << "\033[1A\033[2K"; 
    }
}

int getNumberOnly(string custom) {
    string line;
    while (true) {
        if (custom == "" || custom.empty()) {
            cout << "Pilih: ";
        } else {
            cout << custom;
        }
        getline(cin, line);
        bool isNum = all_of (line.begin(), line.end(), ::isdigit); // check pada satu baris terdapat angka saja atau tidak
        if (isNum && !line.empty()) { // check line tidak kosong
            return stoi(line); // mengkonversi menjadi interger
        } else {
            cout << "Error masukan nomor saja!\n";
            system("pause");
            removeTerminalLine(3);
        }
    }
}

string getStringOnly(string custom) {
    string input;
    while (true) {
        if (custom == "" || custom.empty()) {
            cout << "Input: ";
        } else {
            cout << custom;
        }
        getline(cin, input);
        if (input.length() != 0) {
            return input;            
        } else {
            removeTerminalLine(1);
        }
    }
}

int getNumberInRange(int min, int max, string custom) {
    while (true) {
        int num;
        if (custom.empty() || custom == "") {
            num = getNumberOnly("");
        } else {
            num = getNumberOnly(custom);
        }
        if ((max == 0 || min > max) && (num == 1 || num == 0)) {
            return 1;
        } else if (num >= min && num <= max) {
            return num;
        } else {
            if (min != max) {
                cout << "Error masukan nomor antara " << min << " s/d " << max << "!\n";
            } else if (min == max) {
                cout << "Error masukan nomor antara " << min << "!\n";
            }
            system("pause");
            removeTerminalLine(3);
        }
    }
}

int buildingChoiceValidation(){
    while (true) {
        int iExit = building_list.size()+1;
        int choice = getNumberInRange(1, iExit, "");
        if (choice == iExit||building_list[choice-1].isEmpty()) {
            return choice;
        } else if (!(building_list[choice-1].isEmpty())) { 
            cout << "Kosongkan terlebih dahulu tempat parkir!" << endl;
            system("pause");
            removeTerminalLine(3);
        }
    }
}

int floorChoiceValidation(int iBuilding) {
    while (true) {
        int iExit = building_list[iBuilding].floors.size()+1;
        int choiceFloor = getNumberInRange(1, iExit, "");
        if (choiceFloor == iExit||building_list[iBuilding].floors[choiceFloor-1].isEmpty()) {
            return choiceFloor;
        } else if (!(building_list[iBuilding].floors[choiceFloor-1].isEmpty())) { 
            cout << "Kosongkan terlebih dahulu tempat parkir!" << endl;
            system("pause");
            removeTerminalLine(3);
        }
    }
}

int areaChoiceValidation(int iBuilding, int iFloor) {
    while (true) {
        int iExit = building_list[iBuilding].floors[iFloor].areas.size()+1;
        int choiceArea = getNumberInRange(1, iExit,"");
        if (choiceArea == iExit || building_list[iBuilding].floors[iFloor].areas[choiceArea-1].isEmpty()) {
            return choiceArea;
        }
        if (!(building_list[iBuilding].floors[iFloor].areas[choiceArea-1].isEmpty())) { 
            cout << "Kosongkan terlebih dahulu tempat parkir!" << endl;
            system("pause");
            removeTerminalLine(3);
        } 
    }
}

string buildingInputValidation(string custom = "") {
    while (true) {
        string input = getStringOnly(custom);
        bool found = false;
        for(auto& temp : building_list) {
            if (temp.buildingName == input) {
                found = true;
                cout << "Nama tersebut telah digunakan!\n";
                system("pause");
                removeTerminalLine(3);
                break;
            }
        }
        if (!found) {
            return input;
        }
    }
}

int floorInputValidation(int iBuilding) {
    while (true) {
        int input = getNumberOnly("");
        bool found = false;
        for(auto& temp : building_list[iBuilding].floors) {
            if (temp.floorNumber == input) {
                found = true;
                cout << "Nomor tersebut telah digunakan!\n";
                system("pause");
                removeTerminalLine(3);
                break;
            }
        }
        if (!found) {
            return input;
        }
    }
}

string areaInputValidation(int iBuilding, int iFloor, string custom = "") {
    while (true) {
        string input = getStringOnly(custom);
        bool found = false;
        for(auto& temp : building_list[iBuilding].floors[iFloor].areas) {
            if (temp.areaName == input) {
                found = true;
                cout << "Nama tersebut telah digunakan!\n";
                system("pause");
                removeTerminalLine(3);
                break;
            }
        }
        if (!found) {
            return input;
        }
    }
}

char getYorN(){
    string line;
    while(true){
        cout << "Pilih(Y/N) : ";
        getline(cin, line);
        for (auto &c : line) {
            c = toupper(c);
        }
        if(line.length() == 1 && (line[0] == 'Y' || line[0] == 'N')) {
            return line[0];
        }
        removeTerminalLine(1);
    }
}

string userInputValidation() {
    string input;
    while (true) {
        cout << "Username: ";
        getline(cin, input);
        if (input.length() != 0) {
            return input;            
        } else {
            removeTerminalLine(1);
        }
    }
}

string passInputValidation(bool isShowPass = true) {
    while (true) {
        string password;
        char c;
        cout << "Password: ";
        while((c = getch()) != 13) { // Get every character entered until Enter is pressed
            if (c == 8 && password.length() > 0) { // Handle backspace if password is not empty
                password.pop_back();
                cout << "\b \b"; // Erase the last character from the screen
            } else if (c != 8) { // If not backspace, add character to password and display "*" on the screen
                password.push_back(c);
                if (isShowPass) {
                    cout << c;
                } else if (!isShowPass && password.length() > 0) {
                    cout << "*";
                }
            }
        }
        cout << endl;
        if (password.length() != 0) { // Return password only if it is not empty or backspace was pressed
            return password;
        } else {
            removeTerminalLine(1);
        }
    }
}

string plateNumberValidation(Vehicle*& head, bool isRemove = false) {
    while (true) {
        string plateNumber;
        char c;
        cout << "Plat nomor kendaraan: ";
        while ((c = getch()) != 13) {
            if (c == 8 && plateNumber.length() > 0) { // jika backspace ditekan
                plateNumber.pop_back();
                cout << "\b \b"; // hapus karakter yang ada di terminal
            } else if (c != 8) { // menampilkan bentuk kapital
                c = toupper(c);
                plateNumber.push_back(c);
                cout << c;
            }
        }
        cout << endl;
        regex plateNumberPattern("[A-Z]{1,2}\\s{1,1}[0-9]{1,4}\\s{1,1}[A-Z]{1,3}");
        if (regex_match(plateNumber, plateNumberPattern)) { // Validasi menggunakan regex
            if (isRemove) return plateNumber; 
            Vehicle* curr = head;
            bool found = false;
            while (curr != NULL) {
                if (curr->plateNumber == plateNumber) {
                    found = true;
                    break;
                }
                curr = curr->next;
            }
            if (!found) {
                for (auto& temp : building_list) {
                    Vehicle* tempVehicleQueue = temp.queue.front;
                    while (tempVehicleQueue != nullptr) {
                        if (tempVehicleQueue->plateNumber == plateNumber) {
                            found = true;
                            break;
                        }
                        tempVehicleQueue = tempVehicleQueue->next;
                    }
                    if (found) break;
                }
            }
            if (!found) {
                return plateNumber;
            } else {
                cout << "Kendaraan dengan nomor plat tersebut sedang terparkir atau antri." << endl;
            }
        } else {
            cout << "Format plat nomor: AB 1234 ABC atau A 1 A" << endl;
        }
        system("pause");
        removeTerminalLine(3);
    }
}

string vehicleTypeValidation() {
    string type;
    while (true) {
        bool found = false;
        cout << "Tipe kendaraan: ";
        getline(cin, type);
        for (auto &c : type) {
            c = tolower(c);
        }
        for (auto& temp : vehicleType) {
            if (temp.type == type) {
                return type;
                break;
            }
        }
        if (!found) {
            system("pause");
            removeTerminalLine(2);
        }
    }
}

/*PARKING MANAGEMENT*/

void addVehicleNode(Vehicle*& head, string username, string plateNumber, string type, time_t timeIn, time_t timeOut, float spendTime, float parkingRates, string building, int floor, string area, int spot, bool isFirstInsert = true) { // fungsi untuk menambahkan kendaraan ke linked list
    Vehicle* newVehicle = new Vehicle{username, plateNumber, type, timeIn, timeOut, spendTime, parkingRates, building, floor, area, spot, nullptr}; // Menambah node beserta elemen nya sekaligus
    if (isFirstInsert) { // Masukan dari head
        newVehicle->next = head;
        head = newVehicle;
    } else { // Masukan dari tail
        if (head == nullptr) { // jika head kosong
            head = newVehicle;
        } else { // mencari tail
            Vehicle* curr = head;
            while (curr->next != nullptr) {
                curr = curr->next;
            }
            curr->next = newVehicle;
        }
    }
}

void timeDifference(Vehicle*& node) {
    node->timeOut = time(0);
    node->spendTime = difftime(node->timeOut, node->timeIn); // Hitung selisih waktu dalam detik
    int diff = difftime(node->timeOut, node->timeIn);
    int hours = diff / 3600; // Konversi selisih waktu ke jam
    int minutes = (diff % 3600) / 60; // Konversi selisih waktu ke menit
    int seconds = diff % 60; // Konversi selisih waktu ke detik
    cout << "Waktu parkir: " << hours << " jam " << minutes << " menit " << seconds << " detik" << endl; // Menampilkan selisih waktu
}

float parkingRates(Vehicle*& node) {
    for (auto& vehicle : vehicleType) {
        if (node->type == vehicle.type) {
            float rates = (node->spendTime / 3600) * vehicle.parkingRates;
            return rates;    
        }
    }
    return 0;
}
    
// Fungsi untuk mengubah time_t menjadi string dengan format "MM/YYYY"
string formatTime(time_t time) {
    char buffer[80];
    struct tm *timeinfo = localtime(&time);
    strftime(buffer, 80, "%d/%m/%Y %H:%M:%S", timeinfo);
    return string(buffer);
}

// Fungsi untuk membandingkan dua kendaraan berdasarkan waktu keluar
bool compareVehicle(Vehicle a, Vehicle b) {
    return a.timeOut > b.timeOut;
}

// Fungsi untuk menyimpan data kendaraan ke file csv
void saveToCSV(vector<Vehicle> vehicles, string filename) {
    ofstream file(filename);

    // Menulis header
    file << "Username,Plate Number,Type,Time In,Time Out,Spend Time,Parking Rates,Building,Floor,Area,Spot\n";

    // Mengurutkan data kendaraan berdasarkan waktu keluar
    sort(vehicles.begin(), vehicles.end(), compareVehicle);

    // Menulis data kendaraan ke file csv
    for (Vehicle vehicle : vehicles) {
        file << vehicle.username << ",";
        file << vehicle.plateNumber << ",";
        file << vehicle.type << ",";
        file << vehicle.timeIn << ",";
        file << vehicle.timeOut << ",";
        file << vehicle.spendTime << ",";
        file << vehicle.parkingRates << ",";
        file << vehicle.building << ",";
        file << vehicle.floor << ",";
        file << vehicle.area << ",";
        file << vehicle.spot << "\n";
    }

    file.close();
}

vector<Vehicle> readFromCSV(string filename) {
    vector<Vehicle> vehicles;

    ifstream file(filename);
    string line;

    // Membaca header
    getline(file, line);

    // Membaca data kendaraan dari file csv
    while (getline(file, line)) {
        stringstream ss(line);
        string cell;

        Vehicle vehicle;

        getline(ss, cell, ',');
        vehicle.username = cell;

        getline(ss, cell, ',');
        vehicle.plateNumber = cell;

        getline(ss, cell, ',');
        vehicle.type = cell;

        getline(ss, cell, ',');
        vehicle.timeIn = stoi(cell);

        getline(ss, cell, ',');
        vehicle.timeOut = stoi(cell);

        getline(ss, cell, ',');
        vehicle.spendTime = stof(cell);

        getline(ss, cell, ',');
        vehicle.parkingRates = stof(cell);

        getline(ss, cell, ',');
        vehicle.building = cell;

        getline(ss, cell, ',');
        vehicle.floor = stoi(cell);

        getline(ss, cell, ',');
        vehicle.area = cell;

        getline(ss, cell);
        vehicle.spot = stoi(cell);

        vehicles.push_back(vehicle);
    }

    file.close();

    return vehicles;
}

void saveQueue() {
    ofstream fout("queue.csv");
    if (fout.is_open()) {
        fout << "username,plateNumber,type,timeIn,timeOut,spendTime,parkingRates,building,floor,area,spot" << endl;
        for (auto& building : building_list) {
            Vehicle* vehicle = building.queue.front;
            while (vehicle != nullptr) {
                fout << vehicle->username << "," << vehicle->plateNumber << "," << vehicle->type<< ","<<vehicle->timeIn<<","<<vehicle->timeOut<<","<<vehicle->spendTime<<","<<vehicle->parkingRates<<","<<building.buildingName<<","<<vehicle->floor<<","<<vehicle->area<<","<<vehicle->spot<<"\n";
                vehicle = vehicle->next;
            }
        }
    }
    fout.close();
}

void saveList(Vehicle* head) {
    ofstream fout("tempVehicleList.csv");
    if (fout.is_open()) {
        fout << "username,plateNumber,type,timeIn,timeOut,spendTime,parkingRates,building,floor,area,spot" << endl;
        while (head != NULL) {
            fout << head->username << "," << head->plateNumber << "," << head->type << "," << head->timeIn << "," << head->timeOut << "," << head->spendTime << "," << head->parkingRates << "," << head->building << "," << head->floor << "," << head->area << "," << head->spot << endl;
            head = head->next;
        }
    }
    fout.close();
}


void loadList(Vehicle*& head) {
    ifstream fin("tempVehicleList.csv");
    if (!fin.is_open()) {
        ofstream fout("tempVehicleList.csv");
        fout.close();
        return;
    } else {
        while (head != nullptr) {
            Vehicle* temp = head->next;
            delete head;
            head = temp;
        }
        string line;
        getline(fin, line);
        while (getline(fin, line)) {
            if (line.empty()) {
                break;
            }
            stringstream ss(line);
            string token;
            vector<string> tokens;
            while (getline(ss, token, ',')) {
                tokens.push_back(token);
            }
            if (tokens.size() == 11) {
                string username = tokens[0];
                string plateNumber = tokens[1];
                string type = tokens[2];
                time_t timeIn = stol(tokens[3]);
                time_t timeOut = stol(tokens[4]);
                float spendTime = stof(tokens[5]);
                float parkingRates = stof(tokens[6]);
                string buildingName = tokens[7];
                int floor = stoi(tokens[8]);
                string area = tokens[9];
                int spot = stoi(tokens[10]);
                addVehicleNode(head, username, plateNumber, type, timeIn, timeOut, spendTime, parkingRates, buildingName, floor, area, spot, false);
            }
        }
    }
}

void loadQueue() {
    ifstream fin("queue.csv");
    if (!fin.is_open()) {
        ofstream fout("queue.csv");
        fout.close();
        return;
    } else {
        string line;
        getline(fin, line);
        while (getline(fin, line)) {
            if (line.empty()) {
                break;
            }
            stringstream ss(line);
            string token;
            vector<string> tokens;
            while (getline(ss, token, ',')) {
                tokens.push_back(token);
            }
            if (tokens.size() == 11) {
                string username = tokens[0];
                string plateNumber = tokens[1];
                string type = tokens[2];
                time_t timeIn = stol(tokens[3]);
                time_t timeOut = stol(tokens[4]);
                float spendTime = stof(tokens[5]);
                float parkingRates = stof(tokens[6]);
                string buildingName = tokens[7];
                int floor = stoi(tokens[8]);
                string area = tokens[9];
                int spot = stoi(tokens[10]);

                // Cari bangunan dengan nama yang sesuai
                for (auto& building : building_list) {
                    if (building.buildingName == buildingName) {
                        // Tambahkan kendaraan ke antrian bangunan
                        Vehicle* vehicle = new Vehicle{username, plateNumber, type, timeIn,timeOut, spendTime, parkingRates, buildingName, floor, area, spot, nullptr};
                        building.queue.enqueue(vehicle);
                        break;
                    }
                }
            }
        }
    }
}

void saveTempData() { // fungsi untuk menyimpan nilai variabel ke dalam fin
    ofstream fout;
    fout.open("tempData.csv");
    fout << "Bobot Lantai" << endl;
    fout << floorWeight << endl;

    fout << "-----\n";

    fout << "Jenis Kendaraan,Harga per Jam" << endl;
    for (int i = 0; i < totalVehicleType; i++) {
        fout << vehicleType[i].type << "," << vehicleType[i].parkingRates << endl;
    }

    fout << "-----\n";

    fout << "Building,Queue Capacity, Floor,Area,Area Type,Spot Number,Is Occupied\n";

    for (auto& building : building_list) {
        for (auto& floor : building.floors) {
            for (auto areaIt = floor.areas.rbegin(); areaIt != floor.areas.rend(); ++areaIt) {
                auto& area = *areaIt;
                for (auto& spot : area.spots) {
                    fout << building.buildingName << ","
                        << building.queueCapacity << ","
                        << floor.floorNumber << "," 
                        << area.areaName << ","
                        << area.vehicleType << ","
                        << spot.spotNumber << ",";
                    fout << (spot.isOccupied ? "Yes" : "No") << "\n";
                }
            }
        }
    }

    fout.close();
}

void loadTempData() { // fungsi untuk mengembalikan nilai variabel dari fin
    ifstream fin("tempData.csv");
    if (!fin.is_open()) { // Cek apakah fin sudah ada atau belum
        ofstream fout("tempData.csv");
        fout.close();
        defaultBuildingData();
        return;
    } else {
        string line;
        getline(fin, line);
        while (getline(fin, line)) {
            if (line == "-----") {
                break;
            }
            stringstream ss(line);
            string token;
            
            getline(ss, token);
            floorWeight = stoi(token);
        }
        getline(fin, line);
        while (getline(fin, line)) {
            if (line == "-----") {
                break;
            }

            stringstream ss(line);
            string token;

            getline(ss, token, ',');
            string type = token;

            getline(ss, token, ',');
            float parkingRates = stoi(token);

            SetVehicle temp = {type, parkingRates};

            for (int i = 0; i < totalVehicleType; i++) {
                if (vehicleType[i].type == temp.type) {
                    vehicleType[i] = temp;
                }
            }
        }

        building_list.clear();

        getline(fin, line);
        while (getline(fin, line)) {

            if (line.empty()) {
                    break;
            }
            stringstream ss(line);
            string token;

            getline(ss, token, ',');
            string buildingName = token;

            getline(ss, token, ',');
            int queueCapacity = stoi(token);

            getline(ss, token, ',');
            int floorNumber = stoi(token);

            getline(ss, token, ',');
            string areaName = token;

            getline(ss, token, ',');
            string vehicleType = token;

            getline(ss, token, ',');
            int spotNumber = stoi(token);

            getline(ss, token, ',');
            bool isOccupied = (token == "Yes");

            // mencari building dengan nama tertentu
            bool buildingFound = false;
            for (auto& b : building_list) {
                if (b.buildingName == buildingName) {
                    buildingFound = true;
                    break;
                }
            }

            // jika building tidak ditemukan, tambahkan building baru ke dalam building_list
            if (!buildingFound) {
                Building b(buildingName, queueCapacity);
                building_list.push_back(b);
            }

            // mencari floor dengan nomor tertentu
            bool floorFound = false;
            for (auto& f : building_list.back().floors) {
                if (f.floorNumber == floorNumber) {
                    floorFound = true;
                    break;
                }
            }

            // jika floor tidak ditemukan, tambahkan floor baru ke dalam floors
            if (!floorFound) {
                string buildingName = building_list.back().buildingName;
                Floor f(floorNumber, buildingName);
                building_list.back().floors.push_back(f);
            }

            // mencari area dengan nama tertentu
            bool areaFound = false;
            for (auto& a : building_list.back().floors.back().areas) {
                if (a.areaName == areaName) {
                    areaFound = true;
                    break;
                }
            }

            // jika area tidak ditemukan, tambahkan area baru ke dalam areas
            if (!areaFound) {
                int floorNumber = building_list.back().floors.back().floorNumber;
                string buildingName = building_list.back().buildingName;
                Area a(areaName, vehicleType, floorNumber, buildingName);
                //building_list.back().floors.back().areas.push_back(a);
                building_list.back().floors.back().areas.insert(building_list.back().floors.back().areas.begin(), a);
            }

            // menambahkan spot baru ke dalam spots
            Spot spot(spotNumber);
            spot.isOccupied = isOccupied;
            building_list.back().floors.back().areas.back().spots.push_back(spot);

        }
    fin.close();
    }
}

void loadData(Vehicle*& head) {
    loadTempData();
    loadQueue();
    loadList(head);
}

void saveData(Vehicle*& head) {
    saveTempData();
    saveQueue();
    saveList(head);
}

void displayMenuToAddArea(int iBuilding) {
    while (true) {
        cout << "Pilih Lantai\n";
        int index = displayMenuFromVector("Floor", iBuilding, 0);
        displayBorder();
        int choiceFloor = getNumberInRange(1, building_list[iBuilding].floors.size()+1, "");
        if (choiceFloor == index) {
            return;
        } else if (choiceFloor >= 1 && choiceFloor < index) {
            displayTitle("Tambah Area");
            int iFloor = choiceFloor-1;
            cout << "Nama Area\n";
            string areaName = areaInputValidation(iBuilding, choiceFloor-1);
            int areaCapacity = getNumberOnly("Total Kapasitas Area:");
            string type = vehicleTypeValidation();
            displayBorder();
            
            building_list[iBuilding].addArea(iFloor, areaName, areaCapacity, type);

            int iArea = building_list[iBuilding].floors[iFloor].areas.size()-1;
            cout << "Area " << building_list[iBuilding].floors[iFloor].areas[iArea].areaName << " Lantai " << building_list[iBuilding].floors[iFloor].floorNumber << " di Bangunan " << building_list[iBuilding].buildingName << " telah ditambahkan! \n";
            cout << "Total Kapasitas Area "<< building_list[iBuilding].floors[iFloor].areas[iArea].areaName << ": " << building_list[iBuilding].floors[iFloor].areas[iArea].capacity() << endl;
            displayBorder();
            system("pause");
            return;
        }
    }
}

void displayMenuToChangeArea(int iBuilding, int iFloor) {
    while (true) {
        displayTitle("Ubah Area");
        cout << "Pilih Area\n";
        int index = displayMenuFromVector("Area", iBuilding, iFloor);
        displayBorder();
        int choiceArea = areaChoiceValidation(iBuilding, iFloor);
        if (choiceArea == index) {
            return;
        } else if (choiceArea >= 1 && choiceArea < index) {
            displayTitle("Ubah Area");
            cout << "Nama Area\n";
            string areaName = areaInputValidation(iBuilding, iFloor);
            int areaCapacity = getNumberOnly("Total Kapasitas Area: ");
            string type = vehicleTypeValidation();
            displayBorder();

            building_list[iBuilding].floors[iFloor].areas[choiceArea-1].vehicleType = type;
            building_list[iBuilding].floors[iFloor].areas[choiceArea-1].areaName = areaName;
            building_list[iBuilding].floors[iFloor].areas[choiceArea-1].spots.clear();    
            for (int k = 0; k < areaCapacity; k++){
                Spot newSpot(k+1);
                building_list[iBuilding].floors[iFloor].areas[choiceArea-1].spots.push_back(Spot(newSpot));
            }

            cout << "Area " << building_list[iBuilding].floors[iFloor].areas[choiceArea-1].areaName << " Lantai " << building_list[iBuilding].floors[iFloor].floorNumber << " di Bangunan " << building_list[iBuilding].buildingName << " sekarang memiliki: \n";
            cout << "Total Kapasitas Area "<< building_list[iBuilding].floors[iFloor].areas[choiceArea-1].areaName << " : " << building_list[iBuilding].floors[iFloor].areas[choiceArea-1].capacity() << endl;
            displayBorder();
            system("pause");
            return;
        }
    }
}

void displayMenuToAddFloor(int iBuilding, int index, bool isAddArea = false) {
    while (true) {
        if (iBuilding >= 0 && iBuilding < index && !isAddArea) {
            system(CLEAR_SCREEN);
            displayTitle("Tambah Lantai");
            int nArea = getNumberOnly("Jumlah Area: ");
            int areaCapacity = getNumberOnly("Kapasitas Area: ");
            string type = vehicleTypeValidation();
            displayBorder();
            
            int iFloor = building_list[iBuilding].floors.size();
            int floorNumber = building_list[iBuilding].floors[iFloor-1].floorNumber + 1;
            building_list[iBuilding].addFloor(floorNumber, nArea, areaCapacity, type);

            cout << "Lantai "<< building_list[iBuilding].floors[iFloor].floorNumber << " di Bangunan " << building_list[iBuilding].buildingName << " telah ditambahkan! \n";
            cout << "Area                       : " << building_list[iBuilding].floors[iFloor].areas.size() << endl;
            cout << "Kapasitas per Area         : " << building_list[iBuilding].floors[iFloor].areas[0].capacity() << endl;
            cout << "Kapasitas Total Lantai " << building_list[iBuilding].floors[iFloor].floorNumber << "   : " << building_list[iBuilding].floors[iFloor].capacity() << endl;
            displayBorder();
            system("pause");
            return;
        } else if (iBuilding >= 0 && iBuilding < index && isAddArea) {
            system(CLEAR_SCREEN);
            displayTitle("Tambah Area");
            displayMenuToAddArea(iBuilding);
            return;
        }
    }
}

void displayMenuToChangeFloor(int iBuilding, bool isChangeAreaCapacity = false) {
    while (true) {
        cout << "Pilih Lantai\n";
        int index = displayMenuFromVector("Floor", iBuilding, 0);
        displayBorder();
        int choiceFloor = floorChoiceValidation(iBuilding);
        if (choiceFloor == index) {
            return;
        } else if (choiceFloor >= 1 && choiceFloor < index && !isChangeAreaCapacity) {
            displayTitle("Ubah Lantai");
            int iFloor = choiceFloor-1;
            cout << "Nomor Lantai\n";
            int floorNumber = floorInputValidation(iBuilding);
            
            int nArea = getNumberOnly("Jumlah Area: ");
            
            int areaCapacity = getNumberOnly("Kapasitas Area: ");
            string type = vehicleTypeValidation();
            displayBorder();

            string alphabet = "A";
            building_list[iBuilding].floors[iFloor].floorNumber = floorNumber;
            building_list[iBuilding].floors[iFloor].areas.clear();
            for (int j = 0; j < nArea; j++) {
                building_list[iBuilding].addArea(iFloor, alphabet, areaCapacity, type);
                if (alphabet.back() == 'Z') {
                    alphabet += 'A';
                } else {
                    alphabet.back()++;
                }
            }
        
            cout << "Lantai "<< building_list[iBuilding].floors[iFloor].floorNumber << " di Bangunan " << building_list[iBuilding].buildingName << " telah ditambahkan! \n";
            cout << "Area                       : " << building_list[iBuilding].floors[iFloor].areas.size() << endl;
            cout << "Kapasitas per Area         : " << building_list[iBuilding].floors[iFloor].areas[0].capacity() << endl;
            cout << "Kapasitas Total Lantai " << building_list[iBuilding].floors[iFloor].floorNumber << "   : " << building_list[iBuilding].floors[iFloor].capacity() << endl;
            displayBorder();
            system("pause");
            return;
        } else if (choiceFloor >= 1 && choiceFloor < index && isChangeAreaCapacity) {
            displayMenuToChangeArea(iBuilding, choiceFloor-1);
            return;
        }
    }
}

void displayMenuToAddBuilding() {
    while (true) {
        cout << "Nama Bangunan\n";
        string buildingName = buildingInputValidation();
        
        int nFloor = getNumberOnly("Jumlah Lantai: ");
        
        int nArea = getNumberOnly("Jumlah Area per Lantai: ");
    
        int areaCapacity = getNumberOnly("Kapasitas per Area: ");
        
        int queueCapacity = getNumberOnly("Maksimal Antrian yang diizinkan: ");
        string type = vehicleTypeValidation();
        displayBorder();
        Building newBuilding(buildingName, queueCapacity);
        for (int i = 1; i <= nFloor; i++) {
            newBuilding.addFloor(i, nArea, areaCapacity, type);
        }
        building_list.push_back(newBuilding);

        cout << "Bangunan " << building_list.back().buildingName << " Sekarang memiliki: \n";
        cout << "Jumlah Lantai          : " << building_list.back().floors.size() << endl;
        cout << "Kapasitas per Lantai   : " << building_list.back().floors[0].capacity() << endl;
        cout << "Area per lantai        : " << building_list.back().floors[0].areas.size() << endl;
        cout << "Kapasitas per Area     : " << building_list.back().floors[0].areas[0].capacity() << endl;
        cout << "Kapasitas Total        : " << building_list.back().capacity() << endl;
        displayBorder();
        system("pause");
        return;
    }
}

void displayMenuToRemoveArea(string ans, int iBuilding, int iFloor) {
    while (true) {
        displayTitle("Hapus Area");
        cout << "Pilih Area\n";
        int index = displayMenuFromVector("Area", iBuilding, iFloor);
        displayBorder();
        int choiceArea = getNumberOnly("");
        if (choiceArea == index) {
            return;
        } else if (ans == "Area" && choiceArea >= 1 && choiceArea < index) {
            cout << "Area " << building_list[iBuilding].floors[iFloor].areas[choiceArea-1].areaName << " telah dihapus.\n";
            building_list[iBuilding].removeArea(iFloor, choiceArea-1);
            system("pause");
            return;
        } 
    }
}


void displayMenuToRemoveFloor(string ans, int iBuilding) {
    while (true) {
        if (ans == "Floor") {
            displayTitle("Hapus Lantai");
        } else if (ans == "Area") {
            displayTitle("Hapus Area");
        }
        cout << "Pilih Lantai\n";
        int index = displayMenuFromVector("Floor", iBuilding, 0);
        displayBorder();
        int choiceFloor = getNumberOnly("");
        if (choiceFloor == index) {
            return;
        } else if (ans == "Floor" && choiceFloor >= 1 && choiceFloor < index) {
            cout << building_list[iBuilding].floors[choiceFloor-1].floorNumber << " telah dihapus.\n";
            system("pause");
            building_list[iBuilding].removeFloor(choiceFloor-1);
            return;
        } else if (ans == "Area" && choiceFloor >= 1 && choiceFloor < index) {
            displayMenuToRemoveArea(ans, iBuilding, choiceFloor-1);
            return;
        }
    }
}

void displayMenuToRemoveBuilding(string ans) {
    while (true) {
        system(CLEAR_SCREEN);
        displayTitle("Tambah Lantai");
        cout << "Pilih Bangunan\n";
        int index = displayMenuFromVector("Building", 0, 0);
        displayBorder();
        int choice = getNumberOnly("");
        if (choice == index) {
            return;
        } else if (ans == "Building" && choice >= 1 && choice < index) {
            cout << building_list[choice-1].buildingName << " telah dihapus.\n";
            building_list.erase(building_list.begin() + choice - 1);
            return;
        } else if (ans == "Floor" && choice >= 1 && choice < index) {
            displayMenuToRemoveFloor(ans, choice-1);
            return;
        } else if (ans == "Area" && choice >= 1 && choice < index) {
            displayMenuToRemoveFloor(ans, choice-1);
            return;
        }
    }
}

void displayMenuToChangeBuilding(int choice) {
    while (true) {
        cout << "Nama Bangunan\n";
        string buildingName = buildingInputValidation();
        
        int nFloor = getNumberOnly("Jumlah Lantai: ");
        
        int nArea = getNumberOnly("Jumlah Area per Lantai: ");
        
        int areaCapacity = getNumberOnly("Kapasitas per Area: ");
    
        int queueCapacity = getNumberOnly("Maksimal Antrian yang diperbolehkan: ");
        string type = vehicleTypeValidation();
        displayBorder();

        building_list[choice-1].buildingName = buildingName;
        building_list[choice-1].queueCapacity = queueCapacity;
        building_list[choice-1].floors.clear();
        for (int i = 0; i < nFloor; i++) {
            int floorNumber = i + 1;
            string buildingName = building_list[choice - 1].buildingName;
            Floor newFloor(floorNumber, buildingName);
            building_list[choice - 1].floors.push_back(newFloor);
            string alphabet = "A";
            for (int j = 0; j < nArea; j++) {
                Area newArea(alphabet, type, floorNumber, buildingName);
                //building_list[choice - 1].floors[i].areas.push_back(newArea);
                building_list[choice - 1].floors[i].areas.insert(building_list[choice - 1].floors[i].areas.begin(), newArea);
                if (alphabet.back() == 'Z') {
                    alphabet += 'A';
                } else {
                    alphabet.back()++;
                }
                for (int k = 0; k < areaCapacity; k++) {
                    Spot newSpot(k + 1);
                    building_list[choice - 1].floors[i].areas[j].spots.push_back(newSpot);
                }
            }
        }
        cout << "Bangunan " << building_list[choice-1].buildingName << " Sekarang memiliki: \n";
        cout << "Jumlah Lantai          : " << building_list[choice-1].floors.size() << endl;
        cout << "Kapasitas per Lantai   : " << building_list[choice-1].floors[0].capacity() << endl;
        cout << "Area per lantai        : " << building_list[choice-1].floors[0].areas.size() << endl;
        cout << "Kapasitas per Area     : " << building_list[choice-1].floors[0].areas[0].capacity() << endl;
        cout << "Kapasitas Total        : " << building_list[choice-1].capacity() << endl;
        displayBorder();
        system("pause");
        return;
    }
}

void displayMenuToAdd(string ans){
    while (true) {
        if (ans == "Building") {
            displayMenuToAddBuilding();
            return;
        } else {
            system(CLEAR_SCREEN);
            if (ans == "Floor") {
                displayTitle("Tambah Lantai");
            } else if (ans == "Area") {
                displayTitle("Tambah Area");
            }
            cout << "Pilih Bangunan\n";
            int index = displayMenuFromVector("Building", 0, 0);
            displayBorder();
            int choice = getNumberInRange(1, building_list.size()+1, "");
            if (choice == index) {
                return;
            } else if (ans == "Floor" && choice >= 1 && choice <= index) {
                displayMenuToAddFloor(choice-1, index);
                return;
            } else if (ans == "Area" && choice >= 1 && choice <= index) {
                displayMenuToAddFloor(choice-1, index, true);
                return;
            }
        }
    }
}

void displayMenuToChange(string ans){
    while (true) {
        cout << "Pilih Bangunan\n";
        int index = displayMenuFromVector("Building", 0, 0);;
        displayBorder();
        int choice = buildingChoiceValidation();
        if (choice == index) {
            return;
        } else if (ans == "Building" && choice >= 1 && choice <= index) {
            system(CLEAR_SCREEN);
            displayTitle("Ubah Bangunan");
            displayMenuToChangeBuilding(choice);
            break;
        } else if (ans == "Floor" && choice >= 1 && choice <= index) {
            system(CLEAR_SCREEN);
            displayTitle("Ubah Lantai");
            displayMenuToChangeFloor(choice-1);
            break;
        } else if (ans == "Area" && choice >= 1 && choice <= index) {
            system(CLEAR_SCREEN);
            displayTitle("Ubah Area");
            displayMenuToChangeFloor(choice-1, true);
            break;
        }
    }
}

void displayParkingPlace(int nBuilding, int nFloor, int nArea, bool isBuilding = false, bool isFloor = false, bool isArea = false, bool isSpot = false) {
    if (building_list.size() <= 0) { 
        cout << "Tidak terdapat list Bangunan" << endl;
        return;
    } else if (isSpot) {
        int colWidths[] = {3, 5, 7};
        string headers[] = {"No", "Spot", "Terisi"};

        // top border
        cout << char(218);
        for (int i = 0; i < 3; i++) {
            if (i > 0) {
                cout << char(194);
            }
            for (int j = 0; j < colWidths[i]; j++) {
                cout << char(196);
            }
        }
        cout << char(191) << endl;

        // header
        for (int i = 0; i < 3; i++) {
            cout << char(179) << headers[i];
            for (int j = headers[i].length(); j < colWidths[i]; j++) {
                cout << " ";
            }
        }
        cout << char(179) << endl;

        // separator
        cout << char(195);
        for (int i = 0; i < 3; i++) {
            if (i > 0) {
                cout << char(197);
            }
            for (int j = 0; j < colWidths[i]; j++) {
                cout << char(196);
            }
        }
        cout << char(180) << endl;

        // data
        int index = 1;
        for (auto& temp : building_list[nBuilding].floors[nFloor].areas[nArea].spots) {
            string values[] = {to_string(index++), to_string(temp.spotNumber), to_string(temp.isOccupied)};
            for (int i = 0; i < 3; i++) {
                cout << char(179) << values[i];
                for (int j = values[i].length(); j < colWidths[i]; j++) {
                    cout << " ";
                }
            }
            cout << char(179) << endl;
        }

        // bottom border
        cout << char(192);
        for (int i = 0; i < 3; i++) {
            if (i > 0) {
                cout << char(193);
            }
            for (int j = 0; j < colWidths[i]; j++) {
                cout << char(196);
            }
        }
        cout << char(217) << endl;
    } else if (isArea) {
        int colWidths[] = {3, 5, 15, 10, 7};
        string headers[] = {"No", "Area", "Tipe kendaraan", "Kapasitas", "Terisi"};

        // top border
        cout << char(218);
        for (int i = 0; i < 5; i++) {
            if (i > 0) {
                cout << char(194);
            }
            for (int j = 0; j < colWidths[i]; j++) {
                cout << char(196);
            }
        }
        cout << char(191) << endl;

        // header
        for (int i = 0; i < 5; i++) {
            cout << char(179) << headers[i];
            for (int j = headers[i].length(); j < colWidths[i]; j++) {
                cout << " ";
            }
        }
        cout << char(179) << endl;

       // separator
       cout<<char(195);
       for(int i=0;i<5;i++){
           if(i>0){
               cout<<char(197);
           }
           for(int j=0;j<colWidths[i];j++){
               cout<<char(196);
           }
       }
       cout<<char(180)<<endl;

       // data
       int index=1;
       for(auto& temp:building_list[nBuilding].floors[nFloor].areas){
           string values[]={to_string(index++),temp.areaName,temp.vehicleType,to_string(temp.capacity()),to_string(temp.numOccupied())};
           for(int i=0;i<5;i++){
               cout<<char(179)<<values[i];
               for(int j=values[i].length();j<colWidths[i];j++){
                   cout<<" ";
               }
           }
           cout<<char(179)<<endl;
       }

        // bottom border
        cout << char(192);
        for (int i = 0; i < 5; i++) {
            if (i > 0) {
                cout << char(193);
            }
            for (int j = 0; j < colWidths[i]; j++) {
                cout << char(196);
            }
        }
        cout << char(217) << endl;
    } else if (isFloor) {
        int colWidths[] = {3, 6, 10, 7};
        string headers[] = {"No", "Lantai", "Kapasitas", "Terisi"};

        // top border
        cout << char(218);
        for (int i = 0; i < 4; i++) {
            if (i > 0) {
                cout << char(194);
            }
            for (int j = 0; j < colWidths[i]; j++) {
                cout << char(196);
            }
        }
        cout << char(191) << endl;

        // header
        for (int i = 0; i < 4; i++) {
            cout << char(179) << headers[i];
            for (int j = headers[i].length(); j < colWidths[i]; j++) {
                cout << " ";
            }
        }
        cout << char(179) << endl;

       // separator
       cout<<char(195);
       for(int i=0;i<4;i++){
           if(i>0){
               cout<<char(197);
           }
           for(int j=0;j<colWidths[i];j++){
               cout<<char(196);
           }
       }
       cout<<char(180)<<endl;

       // data
       int index=1;
       for(auto& temp:building_list[nBuilding].floors){
           string values[]={to_string(index++),to_string(temp.floorNumber),to_string(temp.capacity()),to_string(temp.numOccupied())};
           for(int i=0;i<4;i++){
               cout<<char(179)<<values[i];
               for(int j=values[i].length();j<colWidths[i];j++){
                   cout<<" ";
               }
           }
           cout<<char(179)<<endl;
       }

        // bottom border
        cout << char(192);
        for (int i = 0; i < 4; i++) {
            if (i > 0) {
                cout << char(193);
            }
            for (int j = 0; j < colWidths[i]; j++) {
                cout << char(196);
            }
        }
        cout << char(217) << endl;
    } else if (isBuilding) {
        int colWidths[] = {3, 15, 10, 7};
        string headers[] = {"No", "Nama Bangunan", "Kapasitas", "Terisi"};

       // top border
       cout<<char(218);
       for(int i=0;i<4;i++){
           if(i>0){
               cout<<char(194);
           }
           for(int j=0;j<colWidths[i];j++){
               cout<<char(196);
           }
       }
       cout<<char(191)<<endl;

       // header
       for(int i=0;i<4;i++){
           cout<<char(179)<<headers[i];
           for(int j=headers[i].length();j<colWidths[i];j++){
               cout<<" ";
           }
       }
       cout<<char(179)<<endl;

       // separator
       cout<<char(195);
       for(int i=0;i<4;i++){
           if(i>0){
               cout<<char(197);
           }
           for(int j=0;j<colWidths[i];j++){
               cout<<char(196);
           }
       }
       cout<<char(180)<<endl;

       // data
        int index = 1;
        for (auto& temp : building_list) {
            string values[] = {to_string(index++), temp.buildingName, to_string(temp.capacity()), to_string(temp.numOccupied())};
            for (int i = 0; i < 4; i++) {
                cout << char(179) << values[i];
                for (int j = values[i].length(); j < colWidths[i]; j++) {
                    cout << " ";
                }
            }
            cout << char(179) << endl;
        }

        // bottom border
        cout << char(192);
        for (int i = 0; i < 4; i++) {
            if (i > 0) {
                cout << char(193);
            }
            for (int j = 0; j < colWidths[i]; j++) {
                cout << char(196);
            }
        }
        cout << char(217) << endl;
    }
}

void displayRemoveVehicle(Vehicle*& node) {
    cout << "Kendaraan dengan nomor plat " << node->plateNumber << " telah keluar\n";
    timeDifference(node);
    cout << "Tarif yang dibayar: Rp"  << fixed << setprecision(0) << parkingRates(node) << ",00" << endl;
    displayBorder();
    parkingData.push_back(*node);
    saveToCSV(parkingData, "Data.csv");
}

void removeVehicleAlgorithm(Vehicle*& head, string plateNumber) {
    Vehicle* curr = head;
    Vehicle* prev = NULL;
    time_t now = time(0);
    while (curr != NULL && curr->plateNumber != plateNumber) {
        prev = curr;
        curr = curr->next;
    }
    if (curr == NULL) {
        cout << "Tidak ada kendaraan yang parkir!\n";
        displayBorder();
        return;
    } else if (curr != NULL && prev == NULL) {
        head = curr->next;
        displayRemoveVehicle(curr);
        for (auto& temp : building_list) {
            if (temp.buildingName == curr->building) {
                for (auto& tempFloor : temp.floors) {
                    if (tempFloor.floorNumber == curr->floor) {
                        for (auto& tempArea : tempFloor.areas) {
                            if (tempArea.areaName == curr->area) {
                                for (auto& tempSpot : tempArea.spots) {
                                    if (tempSpot.spotNumber == curr->spot) {
                                        tempSpot.isOccupied = false;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        for (auto& building : building_list) {
            if (!building.queue.isEmpty() && building.queue.front->type == curr->type) {
                Vehicle* vehicle = building.queue.front;
                addVehicleNode(head, vehicle->username, vehicle->plateNumber, vehicle->type, now, vehicle->timeOut, vehicle->spendTime, vehicle->parkingRates, vehicle->building, curr->floor, curr->area, curr->spot);
                building.queue.dequeue();
                break;
            }
        }
        delete curr;
    } else if (curr != NULL && prev != NULL) {
        prev->next = curr->next;
        displayRemoveVehicle(curr);
        for (auto& temp : building_list) {
            if (temp.buildingName == curr->building) {
                for (auto& tempFloor : temp.floors) {
                    if (tempFloor.floorNumber == curr->floor) {
                        for (auto& tempArea : tempFloor.areas) {
                            if (tempArea.areaName == curr->area) {
                                for (auto& tempSpot : tempArea.spots) {
                                    if (tempSpot.spotNumber == curr->spot) {
                                        tempSpot.isOccupied = false;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        for (auto& building : building_list) {
            if (!building.queue.isEmpty() && building.queue.front->type == curr->type) {
                Vehicle* vehicle = building.queue.front;
                addVehicleNode(head, vehicle->username, vehicle->plateNumber, vehicle->type, now, vehicle->timeOut, vehicle->spendTime, vehicle->parkingRates, vehicle->building, curr->floor, curr->area, curr->spot);
                building.queue.dequeue();
                break;
            }
        }
        delete curr;
    } else {
        cout << "Kendaraan dengan nomor plat " << plateNumber << " tidak ditemukan!\n";
    }
}

void displayParkingVehicle(Vehicle* vehicle, string title) {
    if (vehicle == NULL) {
        displayBorder();
        cout << "Tidak ada kendaraan yang parkir!\n";
        displayBorder();
        return;
    } else {
        displayTitle(title);
        int colWidths[] = {15, 12, 7, 12, 13, 20, 7, 7, 7};
        string headers[] = {"Username", "Nomor Plat", "Tipe", "Waktu Masuk", "Tarif Parkir", "Bangunan", "Lantai", "Area", "Spot"};

        // top border
        cout << char(218);
        for (int i = 0; i < 9; i++) {
            if (i > 0) {
                cout << char(194);
            }
            for (int j = 0; j < colWidths[i]; j++) {
                cout << char(196);
            }
        }
        cout << char(191) << endl;

        // header
        for (int i = 0; i < 9; i++) {
            cout << char(179) << headers[i];
            for (int j = headers[i].length(); j < colWidths[i]; j++) {
                cout << " ";
            }
        }
        cout << char(179) << endl;

        // separator
        cout<<char(195);
        for(int i=0;i<9;i++){
            if(i>0){
                cout<<char(197);
            }
            for(int j=0;j<colWidths[i];j++){
                cout<<char(196);
            }
        }
        cout<<char(180)<<endl;

        // data
        Vehicle* currentVehicle = vehicle;
        while(currentVehicle != NULL){
            time_t now = time(0);
            tm timeInConversion=*localtime(&currentVehicle->timeIn);
            float spendTime = difftime(now, currentVehicle->timeIn);
            int parkingRates;
            for (auto& temp : vehicleType) {
                if (currentVehicle->type == temp.type) {
                    parkingRates = (spendTime / 3600) * temp.parkingRates;
                    break;  
                }
            }
            string parkingRatesStr="Rp"+to_string(parkingRates)+",00";

            string values[]={currentVehicle->username,currentVehicle->plateNumber,currentVehicle->type,to_string(timeInConversion.tm_hour)+":"+to_string(timeInConversion.tm_min)+":"+to_string(timeInConversion.tm_sec),parkingRatesStr,currentVehicle->building,to_string(currentVehicle->floor),currentVehicle->area,to_string(currentVehicle->spot)};
            for(int i=0;i<9;i++){
                cout<<char(179)<<values[i];
                for(int j=values[i].length();j<colWidths[i];j++){
                    cout<<" ";
                }
            }
            cout<<char(179)<<endl;
            
            currentVehicle = currentVehicle->next;
        }

        // bottom border
        cout << char(192);
        for (int i = 0; i < 9; i++) {
            if (i > 0) {
                cout << char(193);
            }
            for (int j = 0; j < colWidths[i]; j++) {
                cout << char(196);
            }
        }
        cout << char(217) << endl;
    }
    displayBorder();
}

void displayParkingPlaceSpot(Vehicle*& head, string username, string plateNumber, string type, int nBuilding, int nFloor, int nArea) {
    displayTitle("Kendaran Masuk");
    displayParkingPlace(nBuilding-1, nFloor-1, nArea-1, false, false, false, true);
    int iExit = building_list[nBuilding-1].floors[nFloor-1].areas[nArea-1].spots.size()+1; 
    cout << iExit << ". Kembali\n";
    displayBorder();
    int nSpot;
    while (true) {
        nSpot = getNumberInRange(1, iExit, "");
        if (nSpot == iExit) {
            return;
        } else if (building_list[nBuilding-1].floors[nFloor-1].areas[nArea-1].spots[nSpot-1].isOccupied) {
            cout << "Spot terisi" << endl;
            system("pause");
            removeTerminalLine(3);
        } else {
            building_list[nBuilding-1].floors[nFloor-1].areas[nArea-1].spots[nSpot-1].isOccupied = true;
            break;
        }
    } 
    displayBorder();
    time_t now = time(0);
    tm *timeIn = localtime(&now);
    addVehicleNode(head, username, plateNumber, type, now, now, 0, 0, building_list[nBuilding-1].buildingName, building_list[nBuilding-1].floors[nFloor-1].floorNumber, building_list[nBuilding-1].floors[nFloor-1].areas[nArea-1].areaName, building_list[nBuilding-1].floors[nFloor-1].areas[nArea-1].spots[nSpot-1].spotNumber);                    
    cout << "Kendaraan dengan nomor plat " << plateNumber << " berhasil diparkir pada " << timeIn->tm_hour << ":" << timeIn->tm_min << ":" << timeIn->tm_sec << endl;
}

void displayParkingPlaceArea(Vehicle*& head, string username, string plateNumber, string type, int nBuilding, int nFloor) {
    displayTitle("Kendaran Masuk");
    displayParkingPlace(nBuilding-1, nFloor-1, 0, false, false, true);
    int iExit = building_list[nBuilding-1].floors[nFloor-1].areas.size()+1; 
    cout << iExit << ". Kembali\n";
    displayBorder();
    int nArea;
    do {
        nArea = getNumberInRange(1, iExit, "");
        if (nArea == iExit) {
            return;
        } else if (building_list[nBuilding-1].floors[nFloor-1].areas[nArea-1].isFull()) {
            cout << "Area terisi penuh" << endl;
            system("pause");
            removeTerminalLine(3);
        } else if (type != building_list[nBuilding-1].floors[nFloor-1].areas[nArea-1].vehicleType) {
            cout << "Tipe kendaraan yang anda masukan tidak sesuai" << endl;
            system("pause");
            removeTerminalLine(3);
        }
    } while (building_list[nBuilding-1].floors[nFloor-1].areas[nArea-1].isFull() || type != building_list[nBuilding-1].floors[nFloor-1].areas[nArea-1].vehicleType);
    displayParkingPlaceSpot(head, username, plateNumber, type, nBuilding, nFloor, nArea);
}

void displayParkingPlaceFloor(Vehicle*& head, string username, string plateNumber, string type, int nBuilding) {
    displayTitle("Kendaran Masuk");
    displayParkingPlace(nBuilding-1, 0, 0, false, true);
    int iExit = building_list[nBuilding-1].floors.size()+1; 
    cout << iExit << ". Kembali\n";
    displayBorder();
    int nFloor;
    do {
        nFloor = getNumberInRange(1, iExit, "");
        if (nFloor == iExit) {
            return;
        } else if (building_list[nBuilding-1].floors[nFloor-1].isFull()) {
            cout << "Lantai terisi penuh" << endl;
            system("pause");
            removeTerminalLine(3);
        }
    } while (building_list[nBuilding-1].floors[nFloor-1].isFull());
    displayParkingPlaceArea(head, username, plateNumber, type, nBuilding, nFloor);
}

void displayParkingPlaceBuilding(Vehicle*& head, string username, string plateNumber, string type) {
    displayTitle("Kendaran Masuk");
    displayParkingPlace(0, 0, 0, true);
    int iExit = building_list.size()+1; 
    cout << iExit << ". Kembali\n";
    displayBorder();
    int nBuilding;
    if (building_list.empty()) {
        cout << "Tidak Ada Bangunan!\n";
        system("pause");
        return;
    }
    char choose;
    do {
        nBuilding = getNumberInRange(1, iExit, "");
        if (nBuilding == iExit) {
            return;
            break;
        } else if (building_list[nBuilding-1].isFull()) {
            displayBorder();
            bool isFound = false;
            for (auto& tempBuilding : building_list) {
                for (auto& tempFloor : tempBuilding.floors) {
                    for (auto& tempArea : tempFloor.areas) {
                        if (tempArea.vehicleType == type) {
                            isFound = true;
                            break;
                        }
                    }   
                }    
            }
            cout << "Bangunan terisi penuh\n";
            if (!isFound) {
                cout << "Tipe kendaraan anda tidak sesuai\n";
            } else if (isFound) {
                cout << "Apakah anda ingin mengantri ?\n";
                choose = getYorN();
                if (choose == 'Y') {
                    displayBorder();
                    cout << "Kendaraan telah masuk kedalam antrian!\n";
                    Vehicle* newVehicle = new Vehicle{username, plateNumber, type, 0, 0, 0, 0, building_list[nBuilding-1].buildingName, 0, "", 0, nullptr};
                    building_list[nBuilding-1].queue.enqueue(newVehicle);
                }
            }
            displayBorder();
            system("pause");
            return;
        }
    } while (building_list[nBuilding-1].isFull());
    displayParkingPlaceFloor(head, username, plateNumber, type, nBuilding);
}

void addVehicle(Vehicle*& head, string username) {
    bool isFull = true;
    int iBuilding = 0;
    for (auto& temp : building_list) {
        if (!(temp.isFull())) {
            isFull = false;
            break;
        }
        iBuilding++;
    }
    if (isFull && building_list[iBuilding].queue.isFull()) {
        cout << "Parkiran dan Antrian sudah penuh!\n";
        system("pause");
        return;
    } else {
        string plateNumber, type, buildingName, areaName;
        int spotNumber, floorNumber;
        bool found = false;
        displayTitle("Kendaraan Masuk");
        type = vehicleTypeValidation();
        plateNumber = plateNumberValidation(head);
        displayBorder();
        for(auto& temp : building_list) {
            if (!temp.isFull()) {
                buildingName = temp.buildingName;
                for (auto&tempa : temp.floors) {
                    if (!tempa.isFull()) {
                        floorNumber = tempa.floorNumber;
                        for(auto areaIt = tempa.areas.rbegin(); areaIt != tempa.areas.rend(); ++areaIt) {
                            auto& tempb = *areaIt;
                            if (!tempb.isFull() || tempb.vehicleType == type) {
                                areaName = tempb.areaName;
                                for (auto& tempc : tempb.spots) {
                                    if (!tempc.isOccupied) {
                                        spotNumber = tempc.spotNumber;
                                        tempc.isOccupied = true;
                                        found = true;
                                        break;
                                    }
                                }
                                if (found) {
                                    break;
                                }
                            }
                        }
                        if (found) {
                            break;
                        }
                    } 
                }
                if (found) {
                    break;
                }
            }
        }
        if (found) {
            time_t now = time(0);
            tm *timeIn = localtime(&now);
            addVehicleNode(head, username, plateNumber, type, now, now, 0, 0, buildingName, floorNumber, areaName, spotNumber);                    
            cout << "Kendaraan dengan nomor plat " << plateNumber << " berhasil diparkir pada " << timeIn->tm_hour << ":" << timeIn->tm_min << ":" << timeIn->tm_sec << endl;
            cout << "Tempat Parkir anda berada di spot " << spotNumber << " area " << areaName << " lantai " << floorNumber << " bangunan " << buildingName << endl;
            displayBorder();
            system("pause");
        } else {
            cout << "Tidak ada tempat parkir yang tersedia!\n";
        }
    }
    //displayParkingPlaceBuilding(head, username, plateNumber, type);
    return;
}

void removeVehicle(Vehicle*& head, string username, bool isAdmin = false) {
    if (head == NULL) {
        displayBorder();
        cout << "Tidak ada kendaraan yang parkir!\n";
        displayBorder();
        system("pause");
        return;
    } else {
        displayTitle("Kendaraan Keluar");
        if (!isAdmin) {
            int colWidths[] = {15, 15, 15};
            string headers[] = {"Plat Nomor", "Tipe Kendaraan", "Waktu Masuk"};

            cout << char(218);
            for (int i = 0; i < 3; i++) {
                if (i > 0) {
                    cout << char(194);
                }
                for (int j = 0; j < colWidths[i]; j++) {
                    cout << char(196);
                }
            }
            cout << char(191) << endl;

            for (int i = 0; i < 3; i++) {
                cout << char(179) << headers[i];
                for (int j = headers[i].length(); j < colWidths[i]; j++) {
                    cout << " ";
                }
            }
            cout << char(179) << endl;

            cout << char(195);
            for (int i = 0; i < 3; i++) {
                if (i > 0) {
                    cout << char(197);
                }
                for (int j = 0; j < colWidths[i]; j++) {
                    cout << char(196);
                }
            }
            cout << char(180) << endl;
            bool found = false;
            for (auto& account : user_list) {
                Vehicle* vehicle = head;
                while (vehicle != nullptr) {
                    if (vehicle->username == account.username) {
                        if (account.username == username) {
                            found = true;
                            tm timeInConversion = *localtime(&vehicle->timeIn);
                            stringstream ss;
                            ss<<timeInConversion.tm_hour<<":"<<timeInConversion.tm_min<<":"<<timeInConversion.tm_sec;
                            string values[] = {vehicle->plateNumber, vehicle->type, ss.str()};
                            for (int i = 0; i < 3; i++) {
                                cout << char(179) << values[i];
                                for (int j = values[i].length(); j < colWidths[i]; j++) {
                                    cout << " ";
                                }
                            }
                            cout << char(179) << endl;
                        }
                    }
                    vehicle = vehicle->next;
                }
            }

            if (!found) {
                displayTitle("Kendaraan Keluar");
                cout << "Tidak ada kendaraan yang parkir\n";
                displayBorder();
                system("pause");
                return;
            }

            cout << char(192);
            for (int i = 0; i < 3; i++) {
                if (i > 0) {
                    cout << char(193);
                }
                for (int j = 0; j < colWidths[i]; j++) {
                    cout << char(196);
                }
            }
            cout << char(217) << endl;
        displayBorder();
        } else if (isAdmin) {
            displayParkingVehicle(head, "Kendaraan Keluar");
        }
        string plateNumber = plateNumberValidation(head, true);
        displayBorder();
        removeVehicleAlgorithm(head, plateNumber);
        return;
    }
}

void displayMenuQueue() {
    int index = 1;
    int colWidths[] = {15, 12, 16, 10};
    string headers[] = {"Nomor Antrian", "Plat Nomor", "Tipe Kendaraan", "Bangunan"};

    cout << char(218);
    for (int i = 0; i < 4; i++) {
        if (i > 0) {
            cout << char(194);
        }
        for (int j = 0; j < colWidths[i]; j++) {
            cout << char(196);
        }
    }
    cout << char(191) << endl;

    for (int i = 0; i < 4; i++) {
        cout << char(179) << headers[i];
        for (int j = headers[i].length(); j < colWidths[i]; j++) {
            cout << " ";
        }
    }
    cout << char(179) << endl;

    cout << char(195);
    for (int i = 0; i < 4; i++) {
        if (i > 0) {
            cout << char(197);
        }
        for (int j = 0; j < colWidths[i]; j++) {
            cout << char(196);
        }
    }
    cout << char(180) << endl;

    for (auto& building : building_list) {
        if (!building.queue.isEmpty()) {
            Vehicle* vehicle = building.queue.front;
            while (vehicle != nullptr) {
                string values[] = {to_string(index++), vehicle->plateNumber, vehicle->type, building.buildingName};
                for (int i = 0; i < 4; i++) {
                    cout << char(179) << values[i];
                    for (int j = values[i].length(); j < colWidths[i]; j++) {
                        cout << " ";
                    }
                }
                cout << char(179) << endl;
                vehicle = vehicle->next;
            }
        }
    }

    cout << char(192);
    for (int i = 0; i < 4; i++) {
        if (i > 0) {
            cout << char(193);
        }
        for (int j = 0; j < colWidths[i]; j++) {
            cout << char(196);
        }
    }
    cout << char(217) << endl;
}

void parkingQueue() {
    while (true) {
        bool isEmpty = true;
        for (auto& temp : building_list) {
            if (!temp.queue.isEmpty()) { 
                isEmpty = false;
                break;
            }
        }
        if (isEmpty) {
            displayBorder();
            cout << "Tidak terdapat antrian\n";
        } else if (!isEmpty) {
            displayTitle("Antrian");
            displayMenuQueue();
        }
        displayBorder();
        system("pause");
        return;
        break;
    }
}

void changeCapacityArea() {
    while (true) {
        system(CLEAR_SCREEN);
        displayTitle("Kapasitas Parkir");
        cout << "1. Tambah Area\n";
        cout << "2. Hapus Area\n";
        cout << "3. Ubah Kapasitas Area\n";
        cout << "4. Kembali\n";
        displayBorder();

        int choice = getNumberOnly("");

        switch (choice) {
            case 1:
                system(CLEAR_SCREEN);
                displayTitle("Tambah Area");
                cout << "Pilih Bangunan\n";
                displayMenuToAdd("Area");
                break;
            case 2:
                system(CLEAR_SCREEN);
                displayTitle("Hapus Area");
                displayMenuToRemoveBuilding("Area");
                break;
            case 3:
                system(CLEAR_SCREEN);
                displayTitle("Ubah Area");
                displayMenuToChange("Area");
                break;
            case 4:
                return;
                break;
        }
    }
}

void changeCapacityFLoor() {
    while (true) {
        system(CLEAR_SCREEN);
        displayTitle("Kapasitas Parkir");
        cout << "1. Tambah Lantai\n";
        cout << "2. Hapus Lantai\n";
        cout << "3. Ubah Kapasitas Lantai\n";;
        cout << "4. Kembali\n";
        displayBorder();

        int choice = getNumberOnly("");

        switch (choice) {
            case 1:
                displayMenuToAdd("Floor");
                break;
            case 2:
                system(CLEAR_SCREEN);
                displayTitle("Hapus Lantai");
                displayMenuToRemoveBuilding("Floor");
                break;
            case 3:
                system(CLEAR_SCREEN);
                displayTitle("Ubah Lantai");
                displayMenuToChange("Floor");
                break;
            case 4:
                return;
                break;
        }
    }
}

void changeCapacityBuilding() {
    while (true) {
        system(CLEAR_SCREEN);
        displayTitle("Kapasitas Parkir");
        cout << "1. Tambah Bangunan\n";
        cout << "2. Hapus Bangunan\n";
        cout << "3. Ubah Kapasitas Bangunan\n";
        cout << "4. Kembali\n";
        displayBorder();
        
        int choice = getNumberOnly("");

        switch (choice) {
            case 1:
                system(CLEAR_SCREEN);
                displayTitle("Tambah Bangunan");
                displayMenuToAdd("Building");
                break;
            case 2:
                system(CLEAR_SCREEN);
                displayTitle("Hapus Bangunan");
                displayMenuToRemoveBuilding("Building");
                break;
            case 3:
                system(CLEAR_SCREEN);
                displayTitle("Ubah Bangunan");
                displayMenuToChange("Building");
                break;
            case 4:
                return;
                break;
        }
    }
}

void changeCapacity() {
    while (true) {
        system(CLEAR_SCREEN);
        displayTitle("Kapasitas Parkir");
        cout << "1. Bangunan\n";
        cout << "2. Lantai\n";
        cout << "3. Area\n";
        cout << "4. Kembali\n";
        displayBorder();

        int choice = getNumberOnly("");

        switch (choice) {
            case 1:
                changeCapacityBuilding();
                break;
            case 2:
                changeCapacityFLoor();
                break;
            case 3:
                changeCapacityArea();
                break;
            case 4:
                return;
                break;
        }
    }
}

void changesFee(int n) {
    while (true) {
        system(CLEAR_SCREEN);
        displayTitle("Tarif Parkir");
        cout << "Harga per jam: " << vehicleType[n-1].parkingRates << endl;
        displayBorder();

        int temp = getNumberOnly("Harga: ");
        if (temp < 0) {
            cout << "Kapasitas tidak boleh kurang dari 0!\n";
            system("pause");
        } else {
            vehicleType[n].parkingRates = temp;
            break;
        }
    }
}

void parkingFee() {
    while (true) {
        system(CLEAR_SCREEN);
        displayTitle("Tarif Parkir");
        displayMenuVehicle();
        cout << "5. Kembali\n";
        displayBorder();

        int choice = getNumberOnly("");
        if (choice >= 1 && choice <= totalVehicleType) {
            changesFee(choice-1);
        } else if (choice == totalVehicleType + 1) {
            return;
            break;
        }
    }
}

void vacantPark() {
    while (true) {
        displayTitle("Cari Lahan Kosong Terbanyak");
        Tree<Building> buildingTree;
        for (Building& building : building_list) {
            buildingTree.insert(building);
        }

        Tree<Floor> floorTree;
        for (auto& building : building_list) {
            for (auto& floor : building.floors) {
                floorTree.insert(floor);
            }
        }

        Tree<Area> areaTree;
        for (Building& building : building_list) {
            for (Floor& floor : building.floors) {
                for (Area& area : floor.areas) {
                    areaTree.insert(area);
                }
            }
        }

        displayTitle("Lahan kosong");
        cout << "1. Cari Bangunan dengan lahan paling kosong\n";
        cout << "2. Cari Lantai dengan lahan paling kosong\n";
        cout << "3. Cari Area dengan lahan paling kosong\n";
        cout << "4. Kembali\n";
        displayBorder();

        int choice = getNumberOnly("");

        switch (choice) {
            case 1: {
                Building buildingWithMostEmptySpots = buildingTree.findValueWithMostEmptySpots();
                displayBorder();
                cout << "Bangunan " << buildingWithMostEmptySpots.buildingName << " memiliki " << buildingWithMostEmptySpots.vacantSpot() << " lahan kosong" << endl;
                displayBorder();
                system("pause");
                break;
            }
            case 2: {
                Floor floorWithMostEmptySpots = floorTree.findValueWithMostEmptySpots();
                displayBorder();
                cout << "Lantai " << floorWithMostEmptySpots.floorNumber << " di bangunan " << floorWithMostEmptySpots.buildingName << " memiliki " << floorWithMostEmptySpots.vacantSpot() << " lahan kosong" << endl;
                displayBorder();
                system("pause");
                break;
            }
            case 3: {
                Area areaWithMostEmptySpots = areaTree.findValueWithMostEmptySpots();
                displayBorder();
                cout << "Area " << areaWithMostEmptySpots.areaName <<" di Lantai " << areaWithMostEmptySpots.floorNumber << " di bangunan " << areaWithMostEmptySpots.buildingName << " memiliki " << areaWithMostEmptySpots.vacantSpot() << " lahan kosong" << endl;
                displayBorder();
                system("pause");
                break;
            }
            case 4:
                return;
                break;
        }
    }
}

void findNearestEmptySpotAlgorithm(int iBuilding, string vehicleType, int& spotNumber, string& areaName, int& floorNumber, string& buildingName) {
    Building building = building_list[iBuilding];
    int nFloors = building.floors.size();
    vector<vector<int>> graph(nFloors, vector<int>(nFloors, INT_MAX));
    
    // Ubah jarak antar lantai
    for (int i = 0; i < nFloors; i++) {
        for (int j = 0; j < nFloors; j++) {
            if (i != j) {
                graph[i][j] = (j+1 - 1) * floorWeight;
            }
        }
    }

    // Hitung bobot jarak antar area
    for (int i = 0; i < nFloors; i++) {
        Floor floor = building.floors[i];
        int weight = 1;
        for (int j = floor.areas.size() - 1; j >= 0; j--) {
            Area area = floor.areas[j];
            if (area.vehicleType == vehicleType) {
                graph[i][floor.floorNumber-1] = weight;
                weight += area.capacity() + 1;
            }
        }
    }
    
    // Implementasi algoritma Dijkstra
    vector<bool> visited(nFloors, false);
    vector<int> distance(nFloors, INT_MAX);
    distance[0] = 0;
    
    for (int i = 0; i < nFloors - 1; i++) {
        int u = -1;
        for (int j = 0; j < nFloors; j++) {
            if (!visited[j] && (u == -1 || distance[j] < distance[u])) {
                u = j;
            }
        }
        
        visited[u] = true;
        for (int v = 0; v < nFloors; v++) {
            if (!visited[v] && graph[u][v] != INT_MAX && distance[u] + graph[u][v] < distance[v]) {
                distance[v] = distance[u] + graph[u][v];
            }
        }
    }
    
    // Cari lahan kosong terdekat
    int min_distance = INT_MAX;
    for (int i = 0; i < nFloors; i++) {
        Floor floor = building.floors[i];
        if (distance[i] < min_distance) {
            for (Area area : floor.areas) {
                if (area.vehicleType == vehicleType) {
                    for (Spot spot : area.spots) {
                        if (!spot.isOccupied) {    
                            spotNumber = spot.spotNumber;
                            areaName = area.areaName;
                            floorNumber = floor.floorNumber;
                            buildingName = building.buildingName;
                            min_distance = distance[i];
                            break;
                        }
                    }
                }
            }
        }
    }
}

void findShortestPath() {
    displayTitle("Cari Lahan Kosong Terdekat");
    int spotNumber;
    string areaName = "";
    int floorNumber;
    string buildingName;
    cout << "Pilih bangunan terdekat\n";
    int iExit = displayMenuFromVector("Building", 0, 0);
    displayBorder();
    int choice = getNumberInRange(1, iExit, "");
    if (choice == iExit) {
        return;
    }
    displayBorder();
    string type = vehicleTypeValidation();
    displayBorder();
    findNearestEmptySpotAlgorithm(choice-1, type, spotNumber, areaName, floorNumber, buildingName);
    if (spotNumber < 1 || floorNumber < 1 || areaName.empty() || buildingName.empty()) {
        cout << "Tidak terdapat lahan kosong untuk kendaraan " << type <<endl;
    } else if (!areaName.empty()) {
       cout << "Lahan kosong terdekat berada di spot " << spotNumber << " area " << areaName << " lantai " << floorNumber << " bangunan " << buildingName << endl;
    }
    displayBorder();
    system("pause");
}

void findNearestVacantSpot(bool isAdmin = false) {
    while (true) {
        if (isAdmin) {
            displayTitle("Carian Lahan Kosong Terdekat");
            cout << "1. Cari Lahan Kosong Terdekat" << endl;
            cout << "2. Ubah Jarak Antar Lantai" << endl;
            cout << "3. Keluar" << endl;
            displayBorder();
            int choice = getNumberInRange(1, 3, "");
            switch (choice) {
                case 1: {
                    findShortestPath();
                    break;
                }
                case 2: {
                    displayTitle("Ubah jarak antar lantai");
                    cout << "Bobot saat ini: " << floorWeight << endl;
                    cout << "Masukkan jarak antar lantai baru\n";
                    floorWeight = getNumberOnly("Jarak antar lantai: ");
                    break;
                }
                case 3:
                    return;
                    break;
            }
        } else if (!isAdmin) {
            findShortestPath();
            return;
        }
    }
}

void parkingManagement(Vehicle*& head) {
    int choice;
    loadData(head);
    while (true) {
        saveData(head);
        displayTitle("SPARKEL");
        cout << "1. Kendaraan Masuk\n" ;
        cout << "2. Kendaraan Keluar\n" ;
        cout << "3. Kendaraan Terparkir\n" ;
        cout << "4. Cari lahan kosong\n";
        cout << "5. Cari lahan terdekat\n";
        cout << "6. Antrian\n";
        cout << "7. Kapasitas\n";
        cout << "8. Tarif\n";
        cout << "9. Logout\n" ;
        displayBorder();

        choice = getNumberOnly("");

        switch (choice) {
            case 1:
                addVehicle(head, "NONE");
                break;
            case 2:
                removeVehicle(head, "NONE", true);
                break;
            case 3:
                displayParkingVehicle(head, "Kendaraan Parkir");
                system("pause");
                break;
            case 4:
                vacantPark();
                break;
            case 5:
                findNearestVacantSpot(true);
                break;
            case 6:
                parkingQueue();
                break;
            case 7:
                changeCapacity();
                break;
            case 8: 
                parkingFee();
                break;
            case 9:
                return;
                break;
        }
    }
}

tm floatToTm(float time) {
    time_t seconds = time;
    tm duration{};
    duration.tm_hour = seconds / 3600;
    duration.tm_min = (seconds % 3600) / 60;
    duration.tm_sec = seconds % 60;
    return duration;
}

string tmToString(tm duration) {
    stringstream ss;
    ss << setw(2) << setfill('0') << duration.tm_hour << ":";
    ss << setw(2) << setfill('0') << duration.tm_min << ":";
    ss << setw(2) << setfill('0') << duration.tm_sec;
    return ss.str();
}

void displayVehicles(Vehicle vehicles[], int nVehicles) {
    int index = 1;
    int colWidths[] = {3, 12, 7, 20, 7, 5, 5, 20, 20, 9, 15};
    string headers[] = {"No", "Plat Nomor", "Tipe", "Bangunan", "Lantai", "Area", "Spot", "Waktu Masuk", "Waktu Keluar", "Durasi", "Tarif"};

    cout << char(218);
    for (int i = 0; i < 11; i++) {
        if (i > 0) {
            cout << char(194);
        }
        for (int j = 0; j < colWidths[i]; j++) {
            cout << char(196);
        }
    }
    cout << char(191) << endl;

    for (int i = 0; i < 11; i++) {
        cout << char(179) << headers[i];
        for (int j = headers[i].length(); j < colWidths[i]; j++) {
            cout << " ";
        }
    }
    cout << char(179) << endl;

    cout << char(195);
    for (int i = 0; i < 11; i++) {
        if (i > 0) {
            cout << char(197);
        }
        for (int j = 0; j < colWidths[i]; j++) {
            cout << char(196);
        }
    }
    cout << char(180) << endl;

    for (int k = 0; k < nVehicles; k++) {
        Vehicle temp = vehicles[k];
        string timeInStr = formatTime(temp.timeIn);
        string timeOutStr = formatTime(temp.timeOut);
        tm duration = floatToTm(temp.spendTime);
        string spendTimeStr = tmToString(duration);
        string parkingRatesStr = to_string(temp.parkingRates);
        parkingRatesStr.erase(parkingRatesStr.find_last_not_of('0') + 1);
        if (parkingRatesStr.back() == '.') {
            parkingRatesStr.pop_back();
        }
        string values[] = {to_string(index++), temp.plateNumber, temp.type, temp.building, to_string(temp.floor), temp.area, to_string(temp.spot), timeInStr, timeOutStr, spendTimeStr, "Rp" + parkingRatesStr + ",00"};
        for (int i = 0; i < 11; i++) {
            cout << char(179) << values[i];
            for (int j = values[i].length(); j < colWidths[i]; j++) {
                cout << " ";
            }
        }
        cout << char(179) << endl;
    }

    cout << char(192);
    for (int i = 0; i < 11; i++) {
        if (i > 0) {
            cout << char(193);
        }
        for (int j = 0; j < colWidths[i]; j++) {
            cout << char(196);
        }
    }
    cout << char(217) << endl;
}

void history(string username) {
    displayTitle("Histori");
    parkingData = readFromCSV("Data.csv");

    // Memasukkan data kendaraan dengan username tertentu ke dalam stack
    Stack history;
    for (Vehicle vehicle : parkingData) {
        if (vehicle.username == username) {
            history.push(vehicle);
        }
    }

    // Membuat array dari data kendaraan di dalam stack
    int nVehicles = history.size();
    Vehicle vehicles[nVehicles];
    for (int i = 0; i < nVehicles; i++) {
        vehicles[i] = history.pop();
    }

    // Menampilkan tabel dengan data kendaraan
    displayVehicles(vehicles, nVehicles);
    displayBorder();
    system("pause");
}

void parkingManagementUser(Vehicle*& head, string username) {
    loadData(head);
    while (true) {
        saveData(head);
        displayTitle("SPARKEL");
        cout << "1. Kendaraan Masuk\n";
        cout << "2. Kendaraan Keluar\n";
        cout << "3. Lihat antrian\n";
        cout << "4. Cari lahan paling kosong\n";
        cout << "5. Cari lahan kosong terdekat\n";
        cout << "6. Histori\n";
        cout << "7. Logout\n";
        displayBorder();

        int choice = getNumberOnly("");

        switch (choice) {
            case 1:
                addVehicle(head, username);
                break;
            case 2:
                removeVehicle(head, username);
                break;
            case 3:
                parkingQueue();
                break;
            case 4:
                vacantPark();
                break;
            case 5:
                findNearestVacantSpot();
                break;
            case 6:
                history(username);
                break;
            case 7:
                return;
                break;
        }
    }
}

/*ACCOUNT & MAIN*/

void saveAccount(string username, string password, string filename) {
    ofstream fout(filename, ios::app | ios::binary);
    if (fout.is_open()) {
        fout << username << " " << password << endl;
        fout.close();
    } 
}

void loadAccount(string filename, vector<Account>& account_list) {
    ifstream fin(filename, ios::in | ios::binary);
    if (!fin.is_open()) {
        ofstream fout(filename, ios::out | ios::binary);
        fout.close();
        return;
    } else {
        string username, password;
        while (fin >> username >> password) {
            if (!username.empty() && !password.empty()) {
                account_list.push_back({username, password});
            }
        }
        fin.close();
    }
}

void registerAccount(vector<Account>& account_list, string filename) {
    
    displayTitle("Register");

    string username, password, plateNumber;
    username = userInputValidation();
    password = passInputValidation(true);

    displayBorder();

    for (auto account : account_list) {
        if (account.username == username) {
            cout << "Username telah digunakan!\n" << endl;
            system("pause");
            return;
        }
    }

    account_list.push_back({username, password});
    saveAccount(username, password, filename);
    cout << "Registrasi berhasil!\n" << endl;
    system("pause");
}

void loginAccount(Vehicle*& head, vector<Account>& account_list) {
    string username, password;
    displayTitle("Login");

    username = userInputValidation();
    password = passInputValidation(false);

    displayBorder();

    for (auto account : account_list) {
        if (account.username == username && account.password == password) {
            cout << "Login berhasil!\n" << endl;
            system("pause");
            if (account_list == admin_list) {
                parkingManagement(head);
            } else if (account_list == user_list) {
                parkingManagementUser(head, username);
            }
            return;
        }
    }

    cout << "Login gagal. Coba lagi.\n" << endl;
    system("pause");
}

void menuLogin(Vehicle*& head, vector<Account>& account_list, string accountType, string filename) {
    while (true) {
        loadAccount(filename, account_list);
        displayTitle("Menu Login " + accountType);
        cout << "1. Login\n";
        cout << "2. Register\n";
        cout << "3. Kembali\n";
        displayBorder();

        int choice = getNumberOnly("");

        switch (choice) {
            case 1:
                loginAccount(head, account_list);
                break;
            case 2:
                registerAccount(account_list, filename);
                break;
            case 3:
                return;
                break;
        }
    }
}

int main() {
    Vehicle* head = NULL;
    while (true) {
        displayTitle("SPARKEL");
        cout << "1. Login Admin\n";
        cout << "2. Login User\n";
        cout << "3. Keluar\n";
        displayBorder();

        int pilih = getNumberOnly("");

        switch(pilih) {
            case 1:
                menuLogin(head, admin_list, "Admin", "Admins.bin");
                break;
            case 2:
                menuLogin(head, user_list, "User", "Users.bin");
                break;
            case 3:
                cout << "Terima kasih telah menggunakan program ini.\n";
                system("pause");
                return 0;
                break;
        }
    }
}
