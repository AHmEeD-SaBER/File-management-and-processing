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
void writeDelimitedRecord(fstream &file, const string& record, size_t availableSize) {
    string newRecord = to_string(record.length()) + "|" + record;

    // Pad the record with spaces or zeros to completely overwrite the deleted space
    if (newRecord.size() < availableSize) {
        newRecord.append(availableSize - newRecord.size(), ' '); 
        cout<<"New Record"<<newRecord<<"\n"; // Fill remaining space
    } 

    // Write the record to the file
    file.write(newRecord.c_str(), newRecord.size());
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

    fstream file(DOCTOR_FILE, ios::in | ios::out);
    if (!file) {
        cerr << "Failed to open doctor file.\n";
        return;
    }
    string doctorRecord = doctor.id + "|" + doctor.name + "|" + doctor.address + "|";
    for(auto it = doc_availList.begin(); it != doc_availList.end(); ++it) {
        if(it->second >= doctorRecord.size()) {
            file.seekp(it->first);
            cout<<"Avail List entered "<<file.tellp()<<endl;
            string str = to_string(it->second);
            writeDelimitedRecord(file, doctorRecord, it->second+str.size()+1);
            doctorPrimaryIndex[doctor.id] = it->first;
            doctorSecondaryIndex[doctor.name].push_back(doctor.id);
            doc_availList.erase(it);
            return;
        }
    }

    file.seekp(0, ios::end);
    long position = file.tellp();
    doctorPrimaryIndex[doctor.id] = position;

    // Add to secondary index
    doctorSecondaryIndex[doctor.name].push_back(doctor.id);

    // Write to file (Delimited format without newline)
    writeDelimitedRecord(file, doctorRecord, doctorRecord.size());
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
    writeDelimitedRecord(file, appointmentRecord, appointmentRecord.size());
    file.close();

    cout << "Appointment added successfully.\n";
}

int binarySearch(const map<long, size_t>& index, int id) {
    long start = 0;
    long end = index.size() - 1;
    while (start <= end) {
        long mid = start + (end - start) / 2;
        if (id == mid) {
            return index.at(mid);
        }
        else if (id > mid) {
            start = mid + 1;
        }
        else {
            end = mid - 1;
        }
    }
    return -1;
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
    // Remove the doctor from the secondary index (by name)
    for (auto& entry : doctorSecondaryIndex) {
        vector<string>& ids = entry.second;
        auto it = find(ids.begin(), ids.end(), doctorId);
        if (it != ids.end()) {
            ids.erase(it);  // Remove the doctor ID from the list

            // If no IDs are left, delete the entry from the secondary index
            if (ids.empty()) {
                doctorSecondaryIndex.erase(entry.first);
            }
            break;  // Exit the loop once the doctor ID is found and removed
        }
    }

    // Update doc_availList with new length after marking it as deleted
    size_t recordLength = doctorRecord.length(); // Include the added '*'
    doc_availList[position] = recordLength + 1;

    file.close();
    cout << "Doctor deleted successfully.\n";
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
        vector<string>& ids = entry.second;
        auto it = find(ids.begin(), ids.end(), appointmentId);
        if (it != ids.end()) {
            ids.erase(it);  // Remove the doctor ID from the list

            // If no IDs are left, delete the entry from the secondary index
            if (ids.empty()) {
                appointmentSecondaryIndex.erase(entry.first);
            }
            break;  // Exit the loop once the doctor ID is found and removed
        }
    }

    // Update doc_availList with new length after marking it as deleted
    size_t recordLength = appRecord.length(); // Include the added '*'
    app_availList[position] = recordLength + 1;

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
void trim(string& str) {
    str.erase(0, str.find_first_not_of(" \t\n\r"));
    str.erase(str.find_last_not_of(" \t\n\r") + 1);
}
void getdate(const string& appointmentId) {
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

        cout << "doctor id: " << id << "\n"
             << "Date: " << date << "\n";

    }

    file.close();
}
void getmultipledates(const string& doctorId) {
    if (appointmentSecondaryIndex.find(doctorId) == appointmentSecondaryIndex.end()) {
        cout << "No appointments found for Doctor ID: " << doctorId << endl;
        return;
    }

    cout << "Appointments for Doctor ID " << doctorId << ":\n";
    for (const string& appointmentId : appointmentSecondaryIndex[doctorId]) {
        getdate(appointmentId);
    }
}
void getIDs(const string& doctorId) {
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

        cout << "Doctor ID: " << id << "\n";
    }

    file.close();
}
void getMultipleIDs(const string& name) {

    if (doctorSecondaryIndex.find(name) == doctorSecondaryIndex.end()) {
        cout << "No doctors found with the name: " << name << endl;
        return;
    }

    cout << "Doctors with the name " << name << ":\n";
    for (const string& doctorId : doctorSecondaryIndex[name]) {
        getIDs(doctorId);
    }
}
void getAddresses(const string& doctorId) {
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

        cout << "Doctor address: " << address << "\n";
    }

    file.close();
}
void getMultipleaddress(const string& name) {

    if (doctorSecondaryIndex.find(name) == doctorSecondaryIndex.end()) {
        cout << "No doctors found with the name: " << name << endl;
        return;
    }

    cout << "Doctors with the name " << name << ":\n";
    for (const string& doctorId : doctorSecondaryIndex[name]) {
        getAddresses(doctorId);
    }
}
void searchAppointmentfordoctor(const string& appointmentId) {
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

        cout << "Appointment ID: " << id << "\n";

    }

    file.close();
}
void searchAppointment(const string& doctorId) {
    if (appointmentSecondaryIndex.find(doctorId) == appointmentSecondaryIndex.end()) {
        cout << "No appointments found for Doctor ID: " << doctorId << endl;
        return;
    }

    cout << "Appointments for Doctor ID " << doctorId << ":\n";
    for (const string& appointmentId : appointmentSecondaryIndex[doctorId]) {
        searchAppointmentfordoctor(appointmentId);
    }
}
void searchdoctorforappointment(const string& appointmentId) {
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

        cout << "doctor id: " << doctorId << "\n";

    }

    file.close();
}
void searchdoctor(const string& doctorId) {
    if (appointmentSecondaryIndex.find(doctorId) == appointmentSecondaryIndex.end()) {
        cout << "No appointments found for Doctor ID: " << doctorId << endl;
        return;
    }

    cout << "Appointments for Doctor ID " << doctorId << ":\n";
    for (const string& appointmentId : appointmentSecondaryIndex[doctorId]) {
        searchdoctorforappointment(appointmentId);
    }
}


void handleQuery(const string& query) {
    // Convert query to lowercase for consistent parsing
    string lowerQuery = query;
    transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);

    // Basic format validation
    size_t selectPos = lowerQuery.find("select");
    size_t fromPos = lowerQuery.find("from");
    size_t wherePos = lowerQuery.find("where");



    // Extract sections of the query
    string field = lowerQuery.substr(selectPos + 7, fromPos - (selectPos + 7)); // Text after "select " up to " from "
    string tableName = lowerQuery.substr(fromPos + 5, wherePos - (fromPos + 6)); // Text after "from " up to " where "
    string condition = lowerQuery.substr(wherePos + 6); // Text after "where "
    trim(field);



    // Validate the condition format
    size_t equalPos = condition.find('=');
    if (equalPos == string::npos) {
        cout << "Invalid condition format.\n";
        return;
    }

    string conditionField = condition.substr(0, equalPos);
    string conditionValue = condition.substr(equalPos + 1);

    // Ensure condition value has single quotes
    if (conditionValue.front() == '\'' && conditionValue.back() == '\'') {
        conditionValue = conditionValue.substr(1, conditionValue.length() - 2); // Remove surrounding quotes
    } else {
        cout << "Invalid condition value format (missing quotes).\n";
        return;
    }
    // Dispatch based on table name and field
    if (tableName == "doctors") {
        if (conditionField == "doctor id") {
            if (field == "all") {

                searchDoctorByID(conditionValue);
            } else {
                fstream file(DOCTOR_FILE);
                if (!file) {
                    cerr << "Failed to open doctor file.\n";
                    return;
                }

                if (doctorPrimaryIndex.find(conditionValue) == doctorPrimaryIndex.end()) {
                    cout << "Doctor not found.\n";
                    return;
                }

                long position = doctorPrimaryIndex[conditionValue];
                file.seekg(position);
                string doctorRecord = readDelimitedRecord(file);
                if (!doctorRecord.empty()) {
                    istringstream iss(doctorRecord);
                    string id, name, address;
                    getline(iss, id, '|');
                    getline(iss, name, '|');
                    getline(iss, address, '|');
                    if (field=="doctor name") {
                        cout << "Doctor Name: " << name << endl;
                    }
                    else if (field=="doctor address") {
                        cout << "Doctor Address: " << address << endl;
                    }
                }
                file.close();
            }
        }else if (conditionField=="doctor name") {
            if (field == "all") {
                searchDoctorByName(conditionValue);
            }
            else if (field=="doctor id") {
                getMultipleIDs(conditionValue);
            }
            else if (field=="doctor address") {
                getMultipleaddress(conditionValue);
            }

        }
    } if (tableName == "appointments") {
            if (conditionField == "doctor id") {
                if (field == "all") {
                    searchAppointmentByDoctor(conditionValue);
                } else if (field == "appointment date") {
                    getmultipledates(conditionValue);
                }
                else if (field=="appointment id") {
                    searchAppointment(conditionValue);
                }
            } else if (conditionField == "appointment id") {
                if (field == "all") {
                    searchAppointmentByID(conditionValue);
                } else if (field == "doctor id") {
                    searchdoctor(conditionValue);
                }
                else if (field=="appointment date") {
                    getdate(conditionValue);
                }
            } else {
                cout << "Invalid condition field for Appointments table.\n";
            }
        }

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
             << "11. query\n"
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
            case 11:
            {
                string query;
                cout << "Enter query: ";
                cin.ignore();
                getline(cin, query);
                handleQuery(query);
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
