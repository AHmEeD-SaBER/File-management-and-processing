#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <sstream>
#include <algorithm>

using namespace std;

// File paths
const string DOCTOR_FILE = "doctors.txt";
const string DOC_AVAIL_LIST_FILE = "doc_avail_list.txt";
const string APP_AVAIL_LIST_FILE = "app_avail_list.txt";
const string APP_FILE = "appointments.txt";
const string DOC_PRIMARY_INDEX_FILE = "doctor_primary_index.txt";
const string DOC_SECONDARY_INDEX_FILE = "doctor_secondary_index.txt";
const string APP_PRIMARY_INDEX_FILE = "appointment_primary_index.txt";
const string APP_SECONDARY_INDEX_FILE = "appointment_secondary_index.txt";

// Structures
struct Doctor {
    string id;
    string name;
    string address;
};

struct Appointment {
    string id;
    string doctorId;
    string date;
};

// Indexes
map<string, long> doctorPrimaryIndex;
map<string, vector<string>> doctorSecondaryIndex;
map<string, long> appointmentPrimaryIndex;
map<string, vector<string>> appointmentSecondaryIndex;
map<long, size_t> doc_availList;
map<long, size_t> app_availList;

// Function Prototypes
void loadAllIndices();
void saveAllIndices();
void loaddoc_availList();
void loadApp_availList();
void savedoc_availList();
void saveApp_availList();
void addDoctor();
void searchDoctorByID(const string& doctorId);
void searchDoctorByName(const string& name);
void deleteDoctor(const string& doctorId);
void deleteAppointment(const string& appointmentId);
void addAppointment();
void searchAppointmentByID(const string& appointmentId);
void searchAppointmentByDoctor(const string& doctorId);
void menu();

// Helper function to write length indicator and delimited fields without newline
void writeDelimitedRecord(fstream &file, const string& record) {
    file << record.length() << "|" << record; 
}

// Helper function to read a delimited record
string readDelimitedRecord(fstream &file) {
    string line;
    if (getline(file, line)) {
        size_t pos = line.find('|');
        if (pos != string::npos) {
            // The part before the '|' is the length indicator
            int length = stoi(line.substr(0, pos));
            return line.substr(pos+1);
        }
    }
    return "";
}

// Load all indices at the start of the program
void loadAllIndices() {
    // Load Doctor Primary Index
    ifstream file(DOC_PRIMARY_INDEX_FILE);
    doctorPrimaryIndex.clear();
    if (file) {
        string id;
        long position;
        while (file >> id >> position) {
            doctorPrimaryIndex[id] = position;
        }
        file.close();
    }

    // Load Doctor Secondary Index
    file.open(DOC_SECONDARY_INDEX_FILE);
    doctorSecondaryIndex.clear();
    if (file) {
        string line;
        while (getline(file, line)) {
            istringstream iss(line);
            string name, id;
            iss >> name;
            vector<string> ids;
            while (iss >> id) {
                ids.push_back(id);
            }
            doctorSecondaryIndex[name] = ids;
        }
        file.close();
    }

    // Load Appointment Primary Index
    file.open(APP_PRIMARY_INDEX_FILE);
    appointmentPrimaryIndex.clear();
    if (file) {
        string id;
        long position;
        while (file >> id >> position) {
            appointmentPrimaryIndex[id] = position;
        }
        file.close();
    }

    // Load Appointment Secondary Index
    file.open(APP_SECONDARY_INDEX_FILE);
    appointmentSecondaryIndex.clear();
    if (file) {
        string line;
        while (getline(file, line)) {
            istringstream iss(line);
            string key, appointmentId;
            iss >> key;
            vector<string> ids;
            while (iss >> appointmentId) {
                ids.push_back(appointmentId);
            }
            appointmentSecondaryIndex[key] = ids;
        }
        file.close();
    }

    // Load Availability List
    loaddoc_availList();
    loadApp_availList();
}

// Save all indices at the end of the program
void saveAllIndices() {
    // Save Doctor Primary Index
    ofstream file(DOC_PRIMARY_INDEX_FILE);
    for (const auto& entry : doctorPrimaryIndex) {
        file << entry.first << " " << entry.second << "\n";
    }
    file.close();

    // Save Doctor Secondary Index
    file.open(DOC_SECONDARY_INDEX_FILE);
    for (const auto& entry : doctorSecondaryIndex) {
        file << entry.first;
        for (const auto& id : entry.second) {
            file << " " << id;
        }
        file << "\n";
    }
    file.close();

    // Save Appointment Primary Index
    file.open(APP_PRIMARY_INDEX_FILE);
    for (const auto& entry : appointmentPrimaryIndex) {
        file << entry.first << " " << entry.second << "\n";
    }
    file.close();

    // Save Appointment Secondary Index
    file.open(APP_SECONDARY_INDEX_FILE);
    for (const auto& entry : appointmentSecondaryIndex) {
        file << entry.first;
        for (const auto& id : entry.second) {
            file << " " << id;
        }
        file << "\n";
    }
    file.close();

    // Save Availability List
    savedoc_availList();
    saveApp_availList();
}

// Load and save availability list
void loaddoc_availList() {
    ifstream file(DOC_AVAIL_LIST_FILE);
    doc_availList.clear();
    if (file) {
        long position;
        size_t size;
        while (file >> position >> size) {
            doc_availList[position] = size;
        }
        file.close();
    }
}

// Load and save availability list
void loadApp_availList() {
    ifstream file(APP_AVAIL_LIST_FILE);
    app_availList.clear();
    if (file) {
        long position;
        size_t size;
        while (file >> position >> size) {
            app_availList[position] = size;
        }
        file.close();
    }
}

void savedoc_availList() {
    ofstream file(DOC_AVAIL_LIST_FILE);
    for (const auto& entry : doc_availList) {
        file << entry.first << " " << entry.second << "\n";
    }
    file.close();
}

void saveApp_availList() {
    ofstream file(APP_AVAIL_LIST_FILE);
    for (const auto& entry : app_availList) {
        file << entry.first << " " << entry.second << "\n";
    }
    file.close();
}


// Add a doctor to the database
void addDoctor() {
    Doctor doctor;
    cout << "Enter Doctor ID: ";
    cin >> doctor.id;
    cout << "Enter Name: ";
    cin.ignore();
    getline(cin, doctor.name);
    cout << "Enter Address: ";
    getline(cin, doctor.address);

    if (doctorPrimaryIndex.find(doctor.id) != doctorPrimaryIndex.end()) {
        cout << "Doctor ID already exists.\n";
        return;
    }

    fstream file(DOCTOR_FILE, ios::in | ios::out | ios::app);
    if (!file) {
        cerr << "Failed to open doctor file.\n";
        return;
    }

    file.seekp(0, ios::end);
    long position = file.tellp();
    doctorPrimaryIndex[doctor.id] = position;

    // Add to secondary index
    doctorSecondaryIndex[doctor.name].push_back(doctor.id);

    // Write to file (Delimited format without newline)
    string doctorRecord = doctor.id + "|" + doctor.name + "|" + doctor.address + "|";
    writeDelimitedRecord(file, doctorRecord);
    file.close();
    cout << "Doctor added successfully.\n";
}

void addAppointment() {
    Appointment appointment;
    cout << "Enter Appointment ID: ";
    cin >> appointment.id;
    cout << "Enter Doctor ID: ";
    cin >> appointment.doctorId;
    cout << "Enter Appointment Date: ";
    cin.ignore();
    getline(cin, appointment.date);

    cout<<appointment.id<<endl;
    // Check if the appointment ID already exists
    if (appointmentPrimaryIndex.find(appointment.id) != appointmentPrimaryIndex.end()) {
        cout << "Appointment ID already exists.\n";
        return;
    }

    // Validate if the Doctor ID exists
    if (doctorPrimaryIndex.find(appointment.doctorId) == doctorPrimaryIndex.end()) {
        cout << "Doctor ID does not exist. Cannot create appointment.\n";
        return;
    }

    // Open the appointment file
    fstream file(APP_FILE, ios::in | ios::out | ios::app);
    if (!file) {
        cerr << "Failed to open appointment file.\n";
        return;
    }

    file.seekp(0, ios::end); // Move to the end of the file
    long position = file.tellp(); // Record the current position
    appointmentPrimaryIndex[appointment.id] = position; // Update the primary index

    // Add to the secondary index
    appointmentSecondaryIndex[appointment.doctorId].push_back(appointment.id);

    // Write to file (Delimited format with length prefix)
    string appointmentRecord = appointment.id + "|" + appointment.date + "|" + appointment.doctorId + "|";
    writeDelimitedRecord(file, appointmentRecord);
    file.close();

    cout << "Appointment added successfully.\n";
}


// Search for a doctor by ID
void searchDoctorByID(const string& doctorId) {


    fstream file(DOCTOR_FILE);
    if (!file) {
        cerr << "Failed to open doctor file.\n";
        return;
    }

    if (doctorPrimaryIndex.find(doctorId) == doctorPrimaryIndex.end()) {
        cout << "Doctor not found.\n";
        return;
    }

    long position = doctorPrimaryIndex[doctorId];
    file.seekg(position);

    string doctorRecord = readDelimitedRecord(file);
    if (!doctorRecord.empty()) {
        istringstream iss(doctorRecord);
        string id, name, address;
        getline(iss, id, '|');
        getline(iss, name, '|');
        getline(iss, address, '|');

        cout << "Doctor ID: " << id << "\n"
             << "Name: " << name << "\n"
             << "Address: " << address << endl;
    }

    file.close();
}

// Search for doctors by name
void searchDoctorByName(const string& name) {

    if (doctorSecondaryIndex.find(name) == doctorSecondaryIndex.end()) {
        cout << "No doctors found with the name: " << name << endl;
        return;
    }

    cout << "Doctors with the name " << name << ":\n";
    for (const string& doctorId : doctorSecondaryIndex[name]) {
        searchDoctorByID(doctorId);
    }
}

// Search for an appointment by ID
void searchAppointmentByID(const string& appointmentId) {
    fstream file(APP_FILE);
    if (!file) {
        cerr << "Failed to open appointment file.\n";
        return;
    }

    if (appointmentPrimaryIndex.find(appointmentId) == appointmentPrimaryIndex.end()) {
        cout << "Appointment not found.\n";
        return;
    }

    long position = appointmentPrimaryIndex[appointmentId];
    file.seekg(position);

    string appointmentRecord = readDelimitedRecord(file);
    if (!appointmentRecord.empty()) {
        istringstream iss(appointmentRecord);
        string id, date, doctorId;
        getline(iss, id, '|');
        getline(iss, date, '|');
        getline(iss, doctorId, '|');

        cout << "Appointment ID: " << id << "\n"
             << "Date: " << date << "\n"
             << "Doctor ID: " << doctorId << endl;
    }

    file.close();
}

// Search for appointments by Doctor ID
void searchAppointmentByDoctor(const string& doctorId) {
    if (appointmentSecondaryIndex.find(doctorId) == appointmentSecondaryIndex.end()) {
        cout << "No appointments found for Doctor ID: " << doctorId << endl;
        return;
    }

    cout << "Appointments for Doctor ID " << doctorId << ":\n";
    for (const string& appointmentId : appointmentSecondaryIndex[doctorId]) {
        searchAppointmentByID(appointmentId);
    }
}


void deleteDoctor(const string& doctorId) {

    if (doctorPrimaryIndex.find(doctorId) == doctorPrimaryIndex.end()) {
        cout << "Doctor ID not found.\n";
        return;
    }

    long position = doctorPrimaryIndex[doctorId];
    fstream file(DOCTOR_FILE, ios::in | ios::out);
    if (!file) {
        cerr << "Failed to open doctor file.\n";
        return;
    }

    // Move to the position of the record
    file.seekg(position);
    string doctorRecord = readDelimitedRecord(file);
    if (doctorRecord.empty()) {
        cerr << "Error reading doctor record.\n";
        return;
    }

    file.close();

    file.open(DOCTOR_FILE, ios::in | ios::out);

    // Mark the record as deleted by adding '*' at the beginning
        // Update the record to mark it as deleted
    int strLength = doctorRecord.length();
    string str = to_string(strLength);
    file.seekp(position); // Move back to the start of the record
    file.put('*');        // Replace the first character with '*'
    file.write(str.c_str(), str.size()); // Write the length back
    file.put('|');        // Write the delimiter back
    file.write(doctorRecord.c_str(), doctorRecord.size()); // Write the record back
    doctorPrimaryIndex.erase(doctorId);
    for (auto& entry : doctorSecondaryIndex) {
        entry.second.erase(remove(entry.second.begin(), entry.second.end(), doctorId), entry.second.end());
        if(entry.second.empty()){
            doctorSecondaryIndex.erase(entry.first);
        }
    }

    // Update doc_availList with new length after marking it as deleted
    size_t recordLength = doctorRecord.length() -1; // Include the added '*'
    doc_availList[position] = recordLength;

    file.close();
    cout << "Appointment deleted successfully.\n";
}

void deleteAppointment(const string& appointmentId) {

    if (appointmentPrimaryIndex.find(appointmentId) == appointmentPrimaryIndex.end()) {
        cout << "Appointment ID not found.\n";
        return;
    }

    long position = appointmentPrimaryIndex[appointmentId];
    fstream file(APP_FILE, ios::in | ios::out);
    if (!file) {
        cerr << "Failed to open appointment file.\n";
        return;
    }

    // Move to the position of the record
    file.seekg(position);
    string appRecord = readDelimitedRecord(file);
    if (appRecord.empty()) {
        cerr << "Error reading appointment record.\n";
        return;
    }

    file.close();

    file.open(APP_FILE, ios::in | ios::out);

    // Mark the record as deleted by adding '*' at the beginning
        // Update the record to mark it as deleted
    int strLength = appRecord.length();
    string str = to_string(strLength);
    file.seekp(position); // Move back to the start of the record
    file.put('*');        // Replace the first character with '*'
    file.write(str.c_str(), str.size()); // Write the length back
    file.put('|');        // Write the delimiter back
    file.write(appRecord.c_str(), appRecord.size()); // Write the record back
    appointmentPrimaryIndex.erase(appointmentId);
    for (auto& entry : appointmentSecondaryIndex) {
        entry.second.erase(remove(entry.second.begin(), entry.second.end(), appointmentId), entry.second.end());
        if(entry.second.empty()){
            appointmentSecondaryIndex.erase(entry.first);
        }
    }

    // Update doc_availList with new length after marking it as deleted
    size_t recordLength = appRecord.length() -1; // Include the added '*'
    app_availList[position] = recordLength;

    file.close();
    cout << "Appointment deleted successfully.\n";
}

void updateDoctorname(const string& doctorId) {

   if (doctorPrimaryIndex.find(doctorId) == doctorPrimaryIndex.end()) {
        cout << "Doctor ID not found.\n";
        return;
    }
    long position = doctorPrimaryIndex[doctorId];
    fstream file(DOCTOR_FILE, ios::in | ios::out);
    if (!file) {
        cerr << "Failed to open doctor file.\n";
        return;
    }


    file.seekg(position);
    string doctorRecord = readDelimitedRecord(file);
    if (doctorRecord.empty()) {
        cerr << "Error reading doctor record.\n";
        return;
    }

    file.close();

    file.open(DOCTOR_FILE, ios::in | ios::out);

    // Parse the current record
    istringstream iss(doctorRecord);
    string id, oldName, address;
    getline(iss, id, '|');
    getline(iss, oldName, '|');
    getline(iss, address, '|');

    // Prompt user for the new name
    cout << "Enter new Doctor Name: ";
    string newName;
    cin.ignore();
    getline(cin, newName);

    // Update the secondary index
    auto& nameList = doctorSecondaryIndex[oldName];
    nameList.erase(remove(nameList.begin(), nameList.end(), doctorId), nameList.end()); // Remove from old name
    if (nameList.empty()) {
        doctorSecondaryIndex.erase(oldName); // Remove entry if no IDs are left
    }
    doctorSecondaryIndex[newName].push_back(doctorId); // Add to the new name

    // Create a new record with the updated name
    string updatedRecord =id + "|" + newName + "|" + address + "|";
    string updatedRecordWithLength = to_string(updatedRecord.size()) + "|" + updatedRecord;

    // Update the file
    file.seekp(position);
    file.write(updatedRecordWithLength.c_str(),updatedRecordWithLength.size());



    file.close();
    cout << "Doctor name updated successfully.\n";
}
void updateAppointmentDate(const string& appointmentId) {
    if (appointmentPrimaryIndex.find(appointmentId) == appointmentPrimaryIndex.end()) {
        cout << "Appointment ID not found.\n";
        return;
    }
    long position = appointmentPrimaryIndex[appointmentId];
    fstream file(APP_FILE, ios::in | ios::out);
    if (!file) {
        cerr << "Failed to open appointment file.\n";
        return;
    }

    // Read the current appointment record
    file.seekg(position);
    string appointmentRecord = readDelimitedRecord(file);
    if (appointmentRecord.empty()) {
        cerr << "Error reading appointment record.\n";
        return;
    }
    file.close();

    file.open(APP_FILE, ios::in | ios::out);

    // Parse the current record
    istringstream iss(appointmentRecord);
    string id, oldDate, doctorId;
    getline(iss, id, '|');
    getline(iss, oldDate, '|');
    getline(iss, doctorId, '|');

    // Prompt user for the new date
    cout << "Enter new Appointment Date: ";
    string newDate;
    cin.ignore();
    getline(cin, newDate);

    // Create a new record with the updated date
    string updatedRecord = id + "|" + newDate + "|" + doctorId + "|";
    string updatedRecordWithLength = to_string(updatedRecord.size()) + "|" + updatedRecord;

    // Update the file
    file.seekp(position);
    file.write(updatedRecordWithLength.c_str(), updatedRecordWithLength.size());

    file.close();
    cout << "Appointment date updated successfully.\n";
}




// Main menu
void menu() {
    int choice;
    do {
        loadAllIndices();
        cout << "\nMenu:\n"
             << "1. Add Doctor\n"
             << "2. Search Doctor by ID\n"
             << "3. Search Doctor by Name\n"
             << "4. Delete Doctor\n"
             << "5. Add Appointment\n"
             << "6. Search Appointment by ID\n"
             << "7. Search Appointment by Doctor\n"
             << "8. Delete Appointment\n"
             << "9. Update Doctor Name\n"
             << "10. update appointment date\n"
             << "0. exit\n"
             << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            addDoctor();
            break;
        case 2:
            {
                string doctorId;
                cout << "Enter Doctor ID: ";
                cin >> doctorId;
                searchDoctorByID(doctorId);
            }
            break;
        case 3:
            {
                string name;
                cout << "Enter Doctor Name: ";
                cin.ignore();
                getline(cin, name);
                searchDoctorByName(name);
            }
            break;
        case 4:
        {
            string doctorId;
            cout << "Enter Doctor ID: ";
            cin >> doctorId;
            deleteDoctor(doctorId);
        }
            break;
        case 5:
        {
            addAppointment();

        }
            break;
        
        case 6:
        {
                string appointmentId;
                cout << "Enter Appointment ID: ";
                cin >> appointmentId;
                searchAppointmentByID(appointmentId);
        }
            break;
        case 7:
        {
                string docId;
                cout << "Enter Doctor ID: ";
                cin >> docId;
                searchAppointmentByDoctor(docId);
        }
            break;
        case 8:
        {
            string appointmentId;
            cout << "Enter Appointment ID: ";
            cin >> appointmentId;
            deleteAppointment(appointmentId);

        }
            break;
        case 9:  
        {
                string doctorId;
                cout << "Enter Doctor ID: ";
                cin >> doctorId;
                updateDoctorname(doctorId);
        }    
            break;
        case 10:  
        {
                string doctorId;
                cout << "Enter Appointment ID: ";
                cin >> doctorId;
                updateAppointmentDate(doctorId);
        }
            break;
        case 0:
        {
            cout << "Exiting...\n";
        }
            break;

        default:
            cout << "Invalid choice, please try again.\n";
        }
        saveAllIndices();
    } while (choice != 10);
}

// Main function
int main() {
    
    menu();

    return 0;
}
