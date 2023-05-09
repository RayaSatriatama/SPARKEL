/*LIBRARY*/

#include <iostream>
#include <vector> // array dinamis 
#include <string>
#include <cstdlib> // untuk menggunakan fungsi system("pause")
#include <algorithm>
#include <fstream> // Menyimpan dan mengakses data file
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

int nVehicle;
int capacity;
int queueCapacity;
int queueVehicle;

struct Account { // struktur data untuk user
    string username;
    string password;
    bool operator==(const Account& other) const {
        return username == other.username && password == other.password;
    }
};

vector<Account> user_list; // daftar user yang telah terdaftar
vector<Account> admin_list;

struct Vehicle { // struct untuk menyimpan informasi kendaraan
    string username;
    string plateNumber; // nomor plat kendaraan
    string type; // tipe kendaraan
    time_t timeIn; // data mentah untuk waktu masuk 
    time_t timeOut; // data mentah untuk waktu keluar
    float spendTime; // waktu yang dihabiskan untuk parkir
    float parkingRates; // tarif parkir
    Vehicle* next; // pointer ke kendaraan berikutnya dalam linked list
};

struct SetVehicle {
    string type;
    float parkingRates;
    int field;
    int nVehicle;
    int queueCapacity;
    int queueVehicle;
};

SetVehicle vehicleType[] = {
    {"motor", 2000, 5, 0, 3, 0},
    {"mobil", 5000, 5, 0, 3, 0},
    {"truk", 5000, 5, 0, 3, 0},
    {"bus", 5000, 5, 0, 3, 0}
};

const int totalVehicleType = sizeof(vehicleType) / sizeof(vehicleType[0]);

struct Queue {
    int front, rear, capacity;
    Vehicle* vehicles;

    Queue(int c) {
        front = rear = -1;
        capacity = c;
        vehicles = new Vehicle[capacity+1];
    }

    ~Queue() {
        delete[] vehicles;
    }

    bool isFull() {
        return (rear+1)%(capacity+1) == front;
    }

    bool isEmpty() {
        return front == -1;
    }

    void enqueue(Vehicle temp) {
        if (isFull()) {
            cout << "Maaf, antrian sudah penuh" << endl;
        } else {
            rear = (rear+1)%(capacity+1);
            vehicles[rear] = temp;
            if (front == -1) {
                front = rear;
            }
            cout << "Kendaraan dengan nomor plat " << temp.plateNumber << " berhasil ditambahkan ke antrian." << endl;
        }
    }

    void dequeue() {
        if (!isEmpty()) {
            Vehicle vehicle = vehicles[front];
            front = (front+1)%(capacity+1);
            cout << "Kendaraan dengan nomor plat " << vehicle.plateNumber << " telah masuk ke parkiran." << endl;
            if (front == (rear+1)%(capacity+1)) {
                front = rear = -1;
            }
        }
    }

    void display() {
        if (isEmpty()) {
            cout << "Maaf, data antrian kosong" << endl;
        } else {
            int indexFront = front;
            while (indexFront != rear) {
                tm timeInConversion = *localtime(&vehicles[indexFront].timeIn);
                cout << "Nomor Plat: " << vehicles[indexFront].plateNumber << " | Tipe Kendaraan: " << vehicles[indexFront].type << " | Waktu Masuk: " << timeInConversion.tm_hour << ":" << timeInConversion.tm_min << ":" << timeInConversion.tm_sec << endl;
                indexFront = (indexFront+1)%(capacity+1);
            }
            tm timeInConversion = *localtime(&vehicles[indexFront].timeIn);
            cout << "Nomor Plat: " << vehicles[indexFront].plateNumber << " | Tipe Kendaraan: " << vehicles[indexFront].type << " | Waktu Masuk: " << timeInConversion.tm_hour << ":" << timeInConversion.tm_min << ":" << timeInConversion.tm_sec << endl;    
        }
    }
};

Queue vehicleQueue[totalVehicleType] = {
    Queue(vehicleType[0].queueCapacity), 
    Queue(vehicleType[1].queueCapacity), 
    Queue(vehicleType[2].queueCapacity),
    Queue(vehicleType[3].queueCapacity)
};

/*INPUT VALIDATION*/

void removeTerminalLine(int n) { // Menghapus line di terminal
    for (int i = 0; i < n; i++) {
        cout << "\033[1A\033[2K"; 
    }
}

int getNumberOnly() {
    string line;
    while (true) {
        cout << "Pilih: ";
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

char getYorN(){
    string line;
    while(true){
        cout << "Pilih? (Y/N) : ";
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
                for (auto& tempVehicleQueue : vehicleQueue) {
                    for (int i = 0; i < tempVehicleQueue.capacity; i++) {
                        if (tempVehicleQueue.vehicles[i].plateNumber == plateNumber) {
                            found = true;
                            break;
                        }
                    }
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
        for (auto &c : type) { // Mengecilkan huruf kapital
            c = tolower(c);
        }
        for (auto& temp : vehicleType) { // Validasi inputan dengan data tipe kendaraan
            if (temp.type == type) {
                if (((temp.field - temp.nVehicle) <= 0) && (temp.queueCapacity - temp.queueVehicle) <= 0) {
                    cout << "Lahan untuk kendaraan " << temp.type << " penuh!" << endl;
                    found = true;
                    system("pause");
                    removeTerminalLine(3);
                    break;
                } else {
                    return type;
                    break;
                }
            }
        }
        if (!found) {
            system("pause");
            removeTerminalLine(2);
        }
    }
}

/*PARKING MANAGEMENT*/

void addVehicleNode(Vehicle*& head, string username, string plateNumber, string type, time_t timeIn, time_t timeOut, float spendTime, float parkingRates, bool isFirstInsert = true) { // fungsi untuk menambahkan kendaraan ke linked list
    Vehicle* newVehicle = new Vehicle{username, plateNumber, type, timeIn, timeOut, spendTime, parkingRates, NULL}; // Menambah node beserta elemen nya sekaligus
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

void insertVehicleToQueue(Queue& vehicleQueue, string username, string plateNumber, string type, time_t timeIn, time_t timeOut, float spendTime, float parkingRates) {
    Vehicle newVehicle = {username, plateNumber, type, timeIn, timeOut, spendTime, parkingRates, NULL};
    for (int i = 0; i < totalVehicleType; i++) {
        if (newVehicle.type == vehicleType[i].type) {
            vehicleQueue.enqueue(newVehicle); // menambahkan kendaraan ke dalam antrian
            break;
        }
    }
}

void saveListToFile(Vehicle* head, string filename, bool queue = false) {
    ofstream fout(filename, ios::out | ios::binary);
    if (!fout.is_open()) {
        return;
    } else if (fout.is_open() && !queue) {
        while (head != NULL) {
            fout << head->plateNumber << " " << head->type << " " << head->timeIn << " " << head->timeOut << " " << head->spendTime << " " << head->parkingRates << " " << head->username << endl;
            head = head->next;
        }
    } else if (fout.is_open() && queue) {
        for (int i = 0; i < totalVehicleType; i++) {
            for (int j = 0; j <= vehicleQueue[i].rear; j++) {
                fout << vehicleQueue[i].vehicles[j].plateNumber << " " << vehicleQueue[i].vehicles[j].type  << " " << vehicleQueue[i].vehicles[j].timeIn << " " << vehicleQueue[i].vehicles[j].timeOut << " " << vehicleQueue[i].vehicles[j].spendTime << " " << vehicleQueue[i].vehicles[j].parkingRates << " " << vehicleQueue[i].vehicles[j].username << endl;
            }
        }
    }
    fout.close();
}

void loadListFromFile(Vehicle*& head, string filename, bool queue = false) {
    ifstream fin(filename, ios::in | ios::binary);
    if (!fin.is_open()) {
        ofstream fout(filename, ios::out | ios::binary);
        fout.close();
        return;
    } else {
        string username, plateNumber, type, plateNumberLeft, plateNumberMid, plateNumberRight;
        time_t timeIn, timeOut;
        float spendTime, parkingRates;
        if (!queue) {
            while (fin >> plateNumberLeft >> plateNumberMid >> plateNumberRight >> type >> timeIn >> timeOut >> spendTime >> parkingRates >> username) {
                plateNumber = plateNumberLeft + " " + plateNumberMid + " " + plateNumberRight;
                addVehicleNode(head, username, plateNumber, type, timeIn, timeOut, spendTime, parkingRates, false);
            }
        } else if (queue) {
            while (fin >> plateNumberLeft >> plateNumberMid >> plateNumberRight >> type >> timeIn >> timeOut >> spendTime >> parkingRates >> username) {
                int i = 0;
                plateNumber = plateNumberLeft + " " + plateNumberMid + " " + plateNumberRight;
                if (vehicleType[i].type == type) {
                    insertVehicleToQueue(vehicleQueue[i], username, plateNumber, type, timeIn, timeOut, spendTime, parkingRates);
                }
                vehicleType[i].queueVehicle = 0;
                for (auto& vehicle : vehicleType) {
                    if (type == vehicle.type) {
                        vehicle.queueVehicle++;
                    }
                }
                i++;
            }
        } 
    }
}

void saveTempData() { // fungsi untuk menyimpan nilai variabel ke dalam file
    ofstream fout("temp.bin", ios::out | ios::binary);
    for (int i = 0; i < totalVehicleType; i++) {
        fout << vehicleType[i].type << " " << vehicleType[i].parkingRates << " " << vehicleType[i].field << " " << vehicleType[i].nVehicle << " " << vehicleType[i].queueCapacity << " " << vehicleType[i].queueVehicle << endl;
    }
    fout.close();
}

void loadTempData() { // fungsi untuk mengembalikan nilai variabel dari file
    ifstream fin("temp.bin", ios::in | ios::binary);
    if (!fin.is_open()) { // Cek apakah file sudah ada atau belum
        ofstream fout("temp.bin", ios::out | ios::binary);
        fout.close();
        return;
    } else {
        for (auto& temp : vehicleType) {
            fin >> temp.type >> temp.parkingRates >> temp.field >> temp.nVehicle >> temp.queueCapacity >> temp.queueVehicle;
        }
        fin.close();
    }
}

void loadData(Vehicle*& head) {
    loadTempData();
    loadListFromFile(head, "tempVehicleList.bin");
    loadListFromFile(vehicleQueue->vehicles, "queue.bin", true);
}

void saveData(Vehicle*& head) {
    saveTempData();
    saveListToFile(head, "tempVehicleList.bin");
    saveListToFile(vehicleQueue->vehicles, "queue.bin", true);
}

void displayMenuVehicle() {
    int index = 1;
    cout << "Tipe kendaraan | Tarif | Lahan | Terisi | Maksimal Antrian | Panjang Antrian |\n";
    for (auto& temp : vehicleType) {
        cout << index++ << ". " << temp.type << " | Rp" << temp.parkingRates << ",00/Jam | " << temp.field << " | "  << temp.nVehicle << " | " << temp.queueCapacity << " | "  << temp.queueVehicle << " | " << endl;
    }
}

void displayRemoveVehicle(Vehicle*& node) {
    cout << "Kendaraan dengan nomor plat " << node->plateNumber << " telah keluar\n";
    timeDifference(node);
    cout << "Tarif yang dibayar: Rp"  << fixed << setprecision(0) << parkingRates(node) << ",00" << endl; 
    for (auto& vehicle : vehicleType) {
        if (node->type == vehicle.type) {
            vehicle.nVehicle--;
        }
    }
    delete node;
}

void removeVehicleAlgorithm(Vehicle*& head, string plateNumber) {
    Vehicle* curr = head;
    Vehicle* prev = NULL;
    time_t now = time(0);
    while (curr != NULL && curr->plateNumber != plateNumber) {
        prev = curr;
        curr = curr->next;
    }
    if (curr == NULL) { // linked list kosong
        cout << "Tidak ada kendaraan yang parkir!\n";
    } else if (curr != NULL && prev == NULL) { // head yang dihapus
        head = curr->next;
        displayRemoveVehicle(curr);
        for (int i = 0; i < totalVehicleType; i++) {
            if (!(vehicleQueue[i].isEmpty()) && vehicleQueue[i].vehicles->type == curr->type) {
                addVehicleNode(head, vehicleQueue[i].vehicles->username, vehicleQueue[i].vehicles->plateNumber, vehicleQueue[i].vehicles->type, now, vehicleQueue[i].vehicles->timeOut, vehicleQueue[i].vehicles->spendTime, vehicleQueue[i].vehicles->parkingRates);
                vehicleQueue[i].dequeue();
                vehicleType[i].nVehicle++;
                vehicleType[i].queueVehicle--;
                break;
            }
        }
    } else if (curr != NULL && prev != NULL) { // selain head yang dihapus
        prev->next = curr->next;
        displayRemoveVehicle(curr);
        for (int i = 0; i < totalVehicleType; i++) {
            if (!(vehicleQueue[i].isEmpty())) {
                addVehicleNode(head, vehicleQueue[i].vehicles->username, vehicleQueue[i].vehicles->plateNumber, vehicleQueue[i].vehicles->type, now, vehicleQueue[i].vehicles->timeOut, vehicleQueue[i].vehicles->spendTime, vehicleQueue[i].vehicles->parkingRates);
                vehicleQueue[i].dequeue();
                vehicleType[i].nVehicle++;
                vehicleType[i].queueVehicle--;
                break;
            }
        }
    } else { // tidak ditemukan 
        cout << "Kendaraan dengan nomor plat " << plateNumber << " tidak ditemukan!\n";
    }
}

void displayParkingCapacity() { // Menampilkan Status lahan parkir saat ini
    cout << "Total Lahan terisi: " << nVehicle << "/" << capacity << endl;
    cout << "Total Lahan kosong: " << capacity-nVehicle << "/" << capacity << endl;
}

void displayParkingLot(Vehicle* head) { // fungsi untuk menampilkan isi linked list
    if (head == NULL) { // jika linked list kosong
        cout << "Tidak ada kendaraan yang parkir!\n";
    } else { // jika linked list tidak kosong
        Vehicle* curr = head;
        cout << "Daftar Kendaraan yang Parkir:\n";
        while (curr != NULL) { // Traversal list yang digunakan untuk menampilkan
            tm timeInConversion = *localtime(&curr->timeIn);
            cout << "Nomor Plat: " << curr->plateNumber << " | Tipe Kendaraan: " << curr->type << " | Waktu Masuk: " << timeInConversion.tm_hour << ":" << timeInConversion.tm_min << ":" << timeInConversion.tm_sec << endl;
            curr = curr->next;
        }
    }
}
/*
void displayVehicleData(string username, string plateNumber, string type, time_t timeIn, time_t timeOut, float spendTime, float parkingRates, bool isTimeIn, bool isTimeOut) {
    tm timeInConversion = *localtime(&timeIn);
    tm timeOutConversion = *localtime(&timeOut);
    if (isTimeIn && isTimeOut){
        cout << "Nomor Plat: " << plateNumber << " | Tipe Kendaraan: " << type << " | Waktu Masuk: " << timeInConversion.tm_hour << ":" << timeInConversion.tm_min << ":" << timeInConversion.tm_sec << " | Waktu Keluar: " << timeOutConversion.tm_hour << ":" << timeOutConversion.tm_min << ":" << timeOutConversion.tm_sec << " | Lama Parkir: " << spendTime << " | Tarif: " << parkingRates << endl;
    } else if (isTimeIn) {
        cout << "Nomor Plat: " << plateNumber << " | Tipe Kendaraan: " << type << " | Waktu Masuk: " << timeInConversion.tm_hour << ":" << timeInConversion.tm_min << ":" << timeInConversion.tm_sec << endl;
    }
}
*/
void addVehicle(Vehicle*& head, string username, bool isAdmin = false) {
    if (nVehicle == capacity) { // jika parkiran penuh
        cout << "Parkiran dan Antrian sudah penuh!\n";
    } else {
        string plateNumber, type;
        cout << "=================== Kendaraan Masuk ===================\n";
        displayParkingCapacity();
        cout << "=======================================================\n";
        if (isAdmin) {
            displayParkingLot(head);
            cout << "========================================================\n";
        }
        displayMenuVehicle();
        cout << "========================================================\n";
        type = vehicleTypeValidation(); // Input validasi untuk tipe kendaraan
        char choose = 'O';
        for (int i = 0; i < totalVehicleType; i++) {
            if (type == vehicleType[i].type && vehicleType[i].nVehicle == vehicleType[i].field) {
                cout << "Parkiran sedang penuh!\n";
                cout << "Apakah anda ingin memasukan kendaraan kedalam list antrian parkir? (Y/N)\n";
                choose = getYorN();
                if (choose == 'N') {
                    break;
                } else if (choose == 'Y') {
                    removeTerminalLine(4);
                    cout << "Masukan kedalam antrian\n";
                    type = vehicleTypeValidation(); // Input validasi untuk tipe kendaraan
                    plateNumber = plateNumberValidation(head); // Input validasi untuk plat nomor
                    insertVehicleToQueue(vehicleQueue[i], username, plateNumber, type, 0, 0, 0, 0);
                    for (auto& vehicle : vehicleType) {
                        if (type == vehicle.type) {
                            vehicle.queueVehicle++;
                        }
                    }
                    break;
                }
            }
        }
        if (choose == 'O') {
            for (auto& vehicle : vehicleType) {
                if (type == vehicle.type) {
                    vehicle.nVehicle++;
                }
            }
            plateNumber = plateNumberValidation(head); // Input validasi untuk plat nomor
            time_t now = time(0);
            tm *timeIn = localtime(&now); // Konversi waktu dengan hasil dalam bentuk struct
            addVehicleNode(head, username, plateNumber, type, now, now, 0, 0); // tambahkan kendaraan ke linked list
            cout << "=======================================================\n";                      
            cout << "Kendaraan dengan nomor plat " << plateNumber << " berhasil diparkir pada " << timeIn->tm_hour << ":" << timeIn->tm_min << ":" << timeIn->tm_sec << endl;
        }
    }
    system("pause");
}

void removeVehicle(Vehicle*& head, string username, bool isAdmin = false) {
    if (nVehicle == 0) { // jika tidak ada kendaraan yang parkir
        cout << "Tidak ada kendaraan yang parkir!\n";
    } else {
        cout << "=================== Kendaraan Keluar ===================\n";
        displayParkingCapacity();
        cout << "========================================================\n";
        if (isAdmin) displayParkingLot(head);
        else if (!isAdmin) {
            for (auto account : user_list) {
                Vehicle* curr = head;
                bool found = false;
                while (curr != NULL) {
                    if (curr->username == account.username) {
                        found = true;
                        break;
                    }
                    curr = curr->next;
                }
                if (!found) {
                    for (auto& tempVehicleQueue : vehicleQueue) {
                        for (int i = 0; i < tempVehicleQueue.capacity; i++) {
                            if (tempVehicleQueue.vehicles[i].username == account.username) {
                                found = true;
                                break;
                            }
                        }
                    }
                }
                if (account.username == username && found) {
                    tm timeInConversion = *localtime(&curr->timeIn);
                    cout << "Nomor Plat: " << curr->plateNumber << " | Tipe Kendaraan: " << curr->type << " | Waktu Masuk: " << timeInConversion.tm_hour << ":" << timeInConversion.tm_min << ":" << timeInConversion.tm_sec << endl;
                }
            }
        }
        cout << "========================================================\n";
        string plateNumber = plateNumberValidation(head, true);
        cout << "========================================================\n";
        removeVehicleAlgorithm(head, plateNumber); // hapus kendaraan dari linked list
    }
    system("pause");
}

void changesCapacityQueueType(int index) {
    int temp;
    while (true) {
        system(CLEAR_SCREEN);
        cout << "====== Kapasitas Antrian =======\n";
        cout << "Maksimal antrian kendaraan "<< vehicleType[index].type << " : " << vehicleType[index].queueCapacity << " Kendaraan" << endl;
        cout << "Antrian kendaraan " << vehicleType[index].type << " : " << vehicleType[index].queueVehicle << " Kendaraan" << endl;
        cout << "===============================\n";
        temp = getNumberOnly();
        if (temp < vehicleType[index].queueVehicle) { // Pembanding jika terdapat mobil terparkir pada saat kapasitas diubah
            cout << "Antrian tidak boleh kurang dari kendaraan yang sedang diparkir!\n";
            system("pause");
        } else {
            break;
        }
    }
    vehicleType[index].queueCapacity = temp;
}

void parkingQueue(bool isAdmin = false) {
    while (true) {
        for (int i = 0; i < totalVehicleType; i++) {
            cout << "Antrian kendaraan " << vehicleType[i].type << endl;
            vehicleQueue[i].display();
        }
        if (isAdmin) {
            displayMenuVehicle();
            cout << totalVehicleType + 1 << ". Kembali\n";
            cout << "================================\n";
            int choice = getNumberOnly();
            if (choice >= 1 && choice <= totalVehicleType) {
                changesCapacityQueueType(choice-1);
            } else if (choice == totalVehicleType + 1) {
                return;
                break;
            }
        } else if (!isAdmin) {
            cout << "1. Kembali\n";
            cout << "================================\n";
            int choice = getNumberOnly();
            if (choice == 1) {
                return;
                break;
            }
        }
    }
}

void totalCapacity() {
    capacity = 0;
    nVehicle = 0;
    for (int i = 0; i <= 3; i++)
    {
        capacity += vehicleType[i].field;
        nVehicle += vehicleType[i].nVehicle;
    }
}

void changeCapacityQueue() {
    while (true) {
        system(CLEAR_SCREEN);
        cout << "======= Kapasitas Antrian ======\n";
        cout << "Total panjang antrian: " << queueCapacity << " Kendaraan" << endl;
        cout << "Panjang antrian: " << queueVehicle << " Kendaraan" << endl;
        cout << "================================\n";
        displayMenuVehicle();
        cout << totalVehicleType + 1 << ". Kembali\n";
        cout << "================================\n";
        int choice = getNumberOnly();
        if (choice >= 1 && choice <= totalVehicleType) {
            changesCapacityQueueType(choice-1);
        } else if (choice == totalVehicleType + 1) {
            return;
            break;
        }
    }
}

void changesCapacityType(int index) {
    int temp;
    while (true) {
        system(CLEAR_SCREEN);
        cout << "====== Kapasitas Parkir =======\n";
        cout << "Total kapasitas parkir "<< vehicleType[index].type << " : " << vehicleType[index].field << endl;
        cout << "Kendaraan terparkir " << vehicleType[index].type << " : " << vehicleType[index].nVehicle << endl;
        cout << "Lahan kosong "<< vehicleType[index].type << " : " << vehicleType[index].field-vehicleType[index].nVehicle << endl;
        cout << "===============================\n";
        temp = getNumberOnly();
        if (temp < vehicleType[index].nVehicle) { // Pembanding jika terdapat mobil terparkir pada saat kapasitas diubah
            cout << "Kapasitas tidak boleh kurang dari kendaraan yang sedang diparkir!\n";
            system("pause");
        } else {
            break;
        }
    }
    for (int i = 0; i < (temp - vehicleType[index].field); i++) {
        vehicleQueue[i].dequeue();
        vehicleType[i].nVehicle++;
        vehicleType[i].queueVehicle--;
    }
    vehicleType[index].field = temp;
}

void changeCapacity() {
    while (true) {
        system(CLEAR_SCREEN);
        cout << "========== Kapasitas Parkir ========\n";
        displayParkingCapacity();
        cout << "================================\n";
        displayMenuVehicle();
        cout << totalVehicleType + 1 << ". Kembali\n";
        cout << "================================\n";
        int choice = getNumberOnly();
        if (choice >= 1 && choice <= totalVehicleType) {
            changesCapacityType(choice-1);
        } else if (choice == totalVehicleType + 1) {
            return;
            break;
        }
    }
}

void changesFee(int n) {
    while (true) {
        system(CLEAR_SCREEN);
        cout << "======== Tarif Parkir ========\n";
        cout << "Harga per jam: " << vehicleType[n-1].parkingRates << endl;
        cout << "===============================\n";
        int temp = getNumberOnly();
        if (temp < 0) { // Pembanding jika terdapat mobil terparkir pada saat kapasitas diubah
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
        cout << "========= Tarif Parkir =========\n";
        displayParkingCapacity();
        cout << "================================\n";
        displayMenuVehicle();
        cout << "================================\n";
        int choice = getNumberOnly();
        if (choice >= 1 && choice <= totalVehicleType) {
            changesFee(choice-1);
        } else if (choice == totalVehicleType + 1) {
            return;
            break;
        }
    }
}

void parkingManagement() {
    Vehicle* head = NULL; // linked list kosong saat program pertama kali dijalankan
    int choice;
    string plateNumber, type;
    loadData(head);
    while (true) {
        system(CLEAR_SCREEN);
        saveData(head);
        totalCapacity();
        cout << "======== MANAJEMEN PARKIR ========\n" ;
        cout << "1. Kendaraan Masuk\n" ;
        cout << "2. Kendaraan Keluar\n" ;
        cout << "3. Kendaraan Terparkir\n" ;
        cout << "4. Antrian\n";
        cout << "5. Kapasitas\n";
        cout << "6. Tarif\n";
        cout << "7. Logout\n" ;
        cout << "==================================\n";
        choice = getNumberOnly();
        switch (choice) {
        case 1: // kendaraan masuk
            system(CLEAR_SCREEN);
            addVehicle(head, "NONE", true);
            break;
        case 2: // kendaraan keluar
            system(CLEAR_SCREEN);
            removeVehicle(head, "NONE", true);
            break;
        case 3: // tampilkan daftar kendaraan yang parkir
            system(CLEAR_SCREEN);
            displayParkingLot(head);
            system("pause");
            break;
        case 4:
            system(CLEAR_SCREEN);
            parkingQueue(true);
            break;
        case 5: // kapasitas
            changeCapacity();
            break;
        case 6: 
            parkingFee();
            break;
        case 7:
            return;
            break;
        }
    }
}

void parkingManagementUser(string username) {
    Vehicle* head = NULL; // linked list kosong saat program pertama kali dijalankan
    bool valid = true;
    loadData(head);
    while (true) {
        system(CLEAR_SCREEN);
        saveData(head);
        totalCapacity();
        cout << "======== MANAJEMEN PARKIR ========\n" ;
        cout << "1. Kendaraan Masuk\n";
        cout << "2. Kendaraan Keluar\n";
        cout << "3. Lihat antrian\n";
        cout << "4. Logout\n";
        cout << "==================================\n";
        int choice = getNumberOnly();
        switch (choice) {
        case 1: // kendaraan masuk
            system(CLEAR_SCREEN);
            if (valid) {
                addVehicle(head, username);
                valid = false;
            } else {
                cout << "satu akun hanya dapat parkir satu kendaraan\n";
                system("pause");
            }
            break;
        case 2: // kendaraan keluar
            system(CLEAR_SCREEN);
            if (!valid) {
                removeVehicle(head, username);
                valid = true;
            } else {
                cout << "Anda belum memasukan kendaraan\n";
                system("pause");
            }
            break;
        case 3:
            system(CLEAR_SCREEN);
            parkingQueue();
            break;
        case 4:
            return;
            break;
        }
    }
}

/*ACCOUNT & MAIN*/

void saveAccount(string username, string password, string filename) {
    ofstream fout(filename, ios::app | ios::binary); // Proses pembuatan file dan pengecekan file binary
    if (fout.is_open()) {
        fout << username << " " << password << endl;
        fout.close();
    } 
}

void loadAccount(string filename, vector<Account>& account_list) {
    ifstream fin(filename, ios::in | ios::binary);
    if (!fin.is_open()) { // Cek apakah file sudah ada atau belum
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

void registerAccount(vector<Account>& account_list, string filename) { // fungsi untuk mendaftarkan user baru
    string username, password, plateNumber;
    system(CLEAR_SCREEN);
    cout << "=== REGISTER ===" << endl;
    username = userInputValidation();
    password = passInputValidation(true);
    for (auto account : account_list) {
        if (account.username == username) {
            cout << "Username telah digunakan!\n" << endl;
            system("pause");
            return;
        }
    }
    account_list.push_back({username, password}); // tambahkan user baru ke dalam daftar user
    saveAccount(username, password, filename);
    cout << "Registrasi berhasil!\n" << endl;
    system("pause");
}

void loginAccount(vector<Account>& account_list) {// fungsi untuk melakukan login
    string username, password;
    cout << "=== LOGIN ===" << endl;
    username = userInputValidation();
    password = passInputValidation(false);
    for (auto account : account_list) { // cek apakah username dan password sesuai dengan data yang telah terdaftar
        if (account.username == username && account.password == password) {
            cout << "Login berhasil!\n" << endl;
            system("pause");
            if (account_list == admin_list) {
                parkingManagement();
            } else if (account_list == user_list) {
                parkingManagementUser(username);
            }
            return;
        }
    }
    cout << "Login gagal. Coba lagi.\n" << endl;
    system("pause");
}

void menuLogin(vector<Account>& account_list, string accountType, string filename) {
    while (true) {
        loadAccount(filename, account_list);
        system(CLEAR_SCREEN);
        cout << "=== MENU LOGIN " << accountType <<" ===\n";
        cout << "1. Login\n";
        cout << "2. Register\n";
        cout << "3. Kembali\n";
        cout << "=======================\n";
        int choice = getNumberOnly();
        switch (choice) {
        case 1: // Login
            system(CLEAR_SCREEN);
            loginAccount(account_list);
            break;
        case 2: // Register
            system(CLEAR_SCREEN);
            registerAccount(account_list, filename);
            break;
        case 3:
            return;
            break;
        }
    }
}

int main() { // menu login
    while (true) {
        system(CLEAR_SCREEN);
        cout << "=====SPARKEL=====\n";
        cout << "1. Login Admin\n";
        cout << "2. Login User\n";
        cout << "3. Keluar\n";
        cout << "=================\n";
        int pilih = getNumberOnly();
        switch(pilih) {
        case 1:
            menuLogin(admin_list, "Admin", "Admins.bin");
            break;
        case 2:
            menuLogin(user_list, "User", "Users.bin");
            break;
        case 3:
            cout << "Terima kasih telah menggunakan program ini.\n";
            system("pause");
            return 0;
            break;
        }
    }
}