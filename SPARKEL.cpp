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
    string plateNumber; // nomor plat kendaraan
    string type; // tipe kendaraan
    time_t timeIn; // data mentah untuk waktu masuk 
    time_t timeOut; // data mentah untuk waktu keluar
    float spendTime; // waktu yang dihabiskan untuk parkir
    float parkingRates; // tarif parkir
    Vehicle* next; // pointer ke kendaraan berikutnya dalam linked list
};

struct setVehicle {
    string type;
    float parkingRates;
    int field;
    int nVehicle;
};

setVehicle VehicleType[4] = {
    {"motor", 2000, 5, 0},
    {"mobil", 5000, 5, 0},
    {"truk", 5000, 5, 0},
    {"bus", 5000, 5, 0}
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
        bool isNum = all_of(line.begin(), line.end(), ::isdigit); // check pada satu baris terdapat angka saja atau tidak
        if (isNum && !line.empty()) { // check line tidak kosong
            return stoi(line); // mengkonversi menjadi interger
        } else {
            cout << "Error masukan nomor saja!\n";
            system("pause");
            removeTerminalLine(3);
        }
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

string plateNumberValidation() {
    while (true) {
        string plateNumber;
        char c;
        cout << "Plat nomor kendaraan: ";
        while((c = getch()) != 13) {
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
            return plateNumber;
        } else {
            cout << "Format plat nomor kendaraan tidak valid." << endl;
            cout << "Format: AB 1234 ABC atau A 1 A" << endl;
            system("pause");
            removeTerminalLine(4);
        }
    }
}

string vehicleTypeValidation() {
    string type;
    while (true) {
        cout << "Tipe kendaraan: ";
        getline(cin, type);
        for (auto &c : type) { // Mengecilkan huruf kapital
            c = tolower(c);
        }
        for (auto& temp : VehicleType) { // Validasi inputan dengan data tipe kendaraan
            if (temp.type == type) {
                if ((temp.field - temp.nVehicle) <= 0) {
                    cout << "Lahan untuk kendaraan " << temp.type << " penuh!" << endl;
                    break;
                } else {
                    return type;
                    break;
                }
            }
        }
        system("pause");
        removeTerminalLine(3);
    }
}

/*PARKING MANAGEMENT*/

void addVehicleNode(Vehicle*& head, string plateNumber, string type, time_t timeIn, time_t timeOut, float spendTime, float parkingRates, bool isFirstInsert = true) { // fungsi untuk menambahkan kendaraan ke linked list
    Vehicle* newVehicle = new Vehicle{plateNumber, type, timeIn, timeOut, spendTime, parkingRates, NULL}; // Menambah node beserta elemen nya sekaligus
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
    for (auto& vehicle : VehicleType) {
        if (node->type == vehicle.type) {
            float rates = (node->spendTime / 3600) * vehicle.parkingRates;
            return rates;    
        }
    }
    return 0;
}

void saveListToFile(Vehicle* head) {
    ofstream fout("tempVehicleList.bin", ios::out | ios::binary);
    if (fout.is_open()) {
        while (head != NULL) {
            fout << head->plateNumber << " " << head->type << " " << head->timeIn << " " << head->timeOut << " " << head->spendTime << " " << head->parkingRates << endl;
            head = head->next;
        }
        fout.close();
    }
}

void loadListFromFile(Vehicle*& head) {
    ifstream fin("tempVehicleList.bin", ios::in);
    if (!fin.is_open()) {
        ofstream fout("tempVehicleList.bin", ios::out | ios::binary);
        fout.close();
        return;
    } else {
        string plateNumber, plateNumberLeft, plateNumberMid, plateNumberRight, type;
        time_t timeIn, timeOut;
        float spendTime, parkingRates;
        while (fin >> plateNumberLeft >> plateNumberMid >> plateNumberRight >> type >> timeIn >> timeOut >> spendTime >> parkingRates) {
            plateNumber = plateNumberLeft + " " + plateNumberMid + " " + plateNumberRight;
            addVehicleNode(head, plateNumber, type, timeIn, timeOut, spendTime, parkingRates, false);
        }
        fin.close();
    }
}

void saveTempData() {// fungsi untuk menyimpan nilai variabel ke dalam file
    ofstream fout("temp.bin", ios::out | ios::binary);
    for (int i = 0; i <= 3; i++){
        fout << VehicleType[i].type << " " << VehicleType[i].field << " " << VehicleType[i].nVehicle << " " << VehicleType[i].parkingRates << endl;
    }
    fout.close();
}

void loadTempData() {// fungsi untuk mengembalikan nilai variabel dari file
    ifstream fin("temp.bin", ios::in | ios::binary);
    if (!fin.is_open()) { // Cek apakah file sudah ada atau belum
        ofstream fout("temp.bin", ios::out | ios::binary);
        fout.close();
        return;
    } else {
        for (auto& temp : VehicleType) {
            fin >> temp.type >> temp.field >> temp.nVehicle >> temp.parkingRates;
        }
        fin.close();
    }
}

void displayMenuVehicle() {
    int index = 1;
    cout << "Tipe kendaraan | Tarif per jam | Lahan |\n";
    for (auto& temp : VehicleType) {
        cout << index++ << ". " << temp.type << " | Rp" << temp.parkingRates << ",00 | " << temp.field << endl;
    }
    cout << index << ". Kembali\n";
}

void displayRemoveVehicle(Vehicle*& node) {
    cout << "Kendaraan dengan nomor plat " << node->plateNumber << " telah keluar\n";
    timeDifference(node);
    cout << "Tarif yang dibayar: Rp"  << fixed << setprecision(0) << parkingRates(node) << ",00" << endl; 
    for (auto& vehicle : VehicleType) {
        if (node->type == vehicle.type) {
            vehicle.nVehicle--;
        }
    }
    delete node;
}

void removeVehicleAlgorithm(Vehicle*& head, string plateNumber) {
    Vehicle* curr = head;
    Vehicle* prev = NULL;
    while (curr != NULL && curr->plateNumber != plateNumber) {
        prev = curr;
        curr = curr->next;
    }
    if (curr == NULL){ // linked list kosong
        cout << "Tidak ada kendaraan yang parkir!\n";
    } else if (curr != NULL && prev == NULL) { // head yang dihapus
        head = curr->next;
        displayRemoveVehicle(curr);
    } else if (curr != NULL && prev != NULL) { // selain head yang dihapus
        prev->next = curr->next;
        displayRemoveVehicle(curr);
    } else { // tidak ditemukan 
        cout << "Kendaraan dengan nomor plat " << plateNumber << " tidak ditemukan!\n";
    }
}

void displayParkingCapacity() { // Menampilkan Status lahan parkir saat ini
    cout << "Total kapasitas parkir: " << capacity << endl;
    cout << "Kendaraan terparkir: " << nVehicle << endl;
    cout << "Lahan kosong: " << capacity-nVehicle << endl;
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

void addVehicle(Vehicle*& head){
    if (nVehicle == capacity) { // jika parkiran penuh
        cout << "Parkiran sudah penuh atau tambah kapasitas parkir!\n";
    } else {
        cout << "=================== Kendaraan Masuk ===================\n";
        displayParkingCapacity();
        cout << "=======================================================\n";
        displayParkingLot(head);
        cout << "========================================================\n";
        displayMenuVehicle();
        cout << "========================================================\n";
        string type = vehicleTypeValidation(); // Input validasi untuk tipe kendaraan
        for (auto& vehicle : VehicleType) {
            if (type == vehicle.type) {
                vehicle.nVehicle++;
            }
        }
        string plateNumber = plateNumberValidation(); // Input validasi untuk plat nomor
        time_t now = time(0);
        tm *timeIn = localtime(&now); // Konversi waktu dengan hasil dalam bentuk struct
        addVehicleNode(head, plateNumber, type, now, now, 0, 0); // tambahkan kendaraan ke linked list
        cout << "=======================================================\n";                      
        cout << "Kendaraan dengan nomor plat " << plateNumber << " berhasil diparkir pada " << timeIn->tm_hour << ":" << timeIn->tm_min << ":" << timeIn->tm_sec << endl;
    }
    system("pause");
}

void removeVehicle(Vehicle*& head) {
    if (nVehicle == 0) { // jika tidak ada kendaraan yang parkir
        cout << "Tidak ada kendaraan yang parkir!\n";
    } else {
        cout << "=================== Kendaraan Keluar ===================\n";
        displayParkingCapacity();
        cout << "========================================================\n";
        displayParkingLot(head);
        cout << "========================================================\n";
        string plateNumber = plateNumberValidation();
        cout << "========================================================\n";
        removeVehicleAlgorithm(head, plateNumber); // hapus kendaraan dari linked list
    }
    system("pause");
}

void totalCapacity(){
    capacity = 0;
    nVehicle = 0;
    for (int i = 0; i <= 3; i++)
    {
        capacity += VehicleType[i].field;
        nVehicle += VehicleType[i].nVehicle;
    }
}

void changesCapacityType(int n){
    int temp;
    while (true) {
        system(CLEAR_SCREEN);
        cout << "====== Kapasitas Parkir =======\n";
        cout << "Total kapasitas parkir "<< VehicleType[n].type << " : " << VehicleType[n].field << endl;
        cout << "Kendaraan terparkir " << VehicleType[n].type << " : " << VehicleType[n].nVehicle << endl;
        cout << "Lahan kosong "<< VehicleType[n].type << " : " << VehicleType[n].field-VehicleType[n].nVehicle << endl;
        cout << "===============================\n";
        temp = getNumberOnly();
        if (temp < VehicleType[n].nVehicle) { // Pembanding jika terdapat mobil terparkir pada saat kapasitas diubah
            cout << "Kapasitas tidak boleh kurang dari kendaraan yang sedang diparkir!\n";
            system("pause");
        } else {
            break;
        }
    }
    VehicleType[n].field = temp;
}

void changeCapacity() {
    while (true) {
        system(CLEAR_SCREEN);
        cout << "========== Tarif Parkir ========\n";
        displayParkingCapacity();
        cout << "================================\n";
        displayMenuVehicle();
        cout << "================================\n";
        int choice = getNumberOnly();
        int totalIndex = sizeof(VehicleType);
        if (choice >= 1 && choice < totalIndex) {
            changesCapacityType(choice-1);
        } else if (choice == totalIndex) {
            return;
        }
    }
}

void changesFee(int n){
    while (true) {
        system(CLEAR_SCREEN);
        cout << "======== Tarif Parkir ========\n";
        cout << "Harga per jam: " << VehicleType[n-1].parkingRates << endl;
        cout << "===============================\n";
        int temp = getNumberOnly();
        if (temp < 0) { // Pembanding jika terdapat mobil terparkir pada saat kapasitas diubah
            cout << "Kapasitas tidak boleh kurang dari 0!\n";
            system("pause");
        } else {
            VehicleType[n].parkingRates = temp;
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
        int totalIndex = sizeof(VehicleType);
        if (choice >= 1 && choice < totalIndex) {
            changesFee(choice-1);
        } else if (choice == totalIndex) {
            return;
        }
    }
}

void parkingManagement() {
    Vehicle* head = NULL; // linked list kosong saat program pertama kali dijalankan
    int choice;
    string plateNumber, type;
    loadTempData();
    loadListFromFile(head);
    while (true) {
        system(CLEAR_SCREEN);
        saveTempData();
        saveListToFile(head);
        totalCapacity();
        cout << "======== MANAJEMEN PARKIR ========\n" ;
        cout << "1. Kendaraan Masuk\n" ;
        cout << "2. Kendaraan Keluar\n" ;
        cout << "3. Daftar Kendaraan Parkir\n" ;
        cout << "4. Kapasitas\n" ;
        cout << "5. Tarif\n";
        cout << "6. Logout\n" ;
        cout << "==================================\n";
        choice = getNumberOnly();
        switch (choice) {
        case 1: // kendaraan masuk
            system(CLEAR_SCREEN);
            addVehicle(head);
            break;
        case 2: // kendaraan keluar
            system(CLEAR_SCREEN);
            removeVehicle(head);
            break;
        case 3: // tampilkan daftar kendaraan yang parkir
            system(CLEAR_SCREEN);
            displayParkingLot(head);
            system("pause");
            break;
        case 4: // kapasitas
            changeCapacity();
            break;
        case 5: 
            parkingFee();
            break;
        case 6:
            return;
            break;
        }
    }
}

void parkingManagementUser() {
    Vehicle* head = NULL; // linked list kosong saat program pertama kali dijalankan
    bool valid = true;
    loadTempData();
    loadListFromFile(head);
    while (true) {
        system(CLEAR_SCREEN);
        saveListToFile(head);
        totalCapacity();
        cout << "======== MANAJEMEN PARKIR ========\n" ;
        cout << "1. Kendaraan Masuk\n" ;
        cout << "2. Kendaraan Keluar\n" ;
        cout << "3. Logout\n" ;
        cout << "==================================\n";
        int choice = getNumberOnly();
        switch (choice) {
        case 1: // kendaraan masuk
            system(CLEAR_SCREEN);
            if (valid) {
                addVehicle(head);
                valid = false;
            } else {
                cout << "satu akun hanya dapat parkir satu kendaraan\n";
                system("pause");
            }
            break;
        case 2: // kendaraan keluar
            system(CLEAR_SCREEN);
            if (!valid) {
                removeVehicle(head);
                valid = true;
            } else {
                cout << "Anda belum memasukan kendaraan\n";
                system("pause");
            }
            break;
        case 3:
            return;
            break;
        }
    }
}

/*ACCOUNT & MAIN*/

void saveAccount(string username, string password, string filename) {
    ofstream fout(filename, ios::app | ios::binary); // Proses pembuatan file dan pengecekan file binary
    if(fout.is_open()) {
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
        while (!fin.eof()) {
            string username, password;
            fin >> username >> password;
            if (!username.empty() && !password.empty()) {
                account_list.push_back({username, password});
            }
        }
        fin.close();
    }
}

void registerAccount(vector<Account>& account_list, string filename) { // fungsi untuk mendaftarkan user baru
    string username, password;
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
                parkingManagementUser();
            }
            return;
        }
    }
    cout << "Login gagal. Coba lagi.\n" << endl;
    system("pause");
}

void menuLogin(vector<Account>& account_list, string accountType, string filename) {
    while (true) {
        loadAccount("Users.bin", user_list);
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
    while (true){
        system(CLEAR_SCREEN);
        cout<<"=====SPARKEL=====\n";
        cout<<"1. Login Admin\n";
        cout<<"2. Login User\n";
        cout<<"3. Keluar\n";
        cout<<"=================\n";
        int pilih = getNumberOnly();
        switch(pilih){
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