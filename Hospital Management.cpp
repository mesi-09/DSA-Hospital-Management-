#include <iostream>
#include <string>
#include <ctime>
#include <fstream>
#include <vector>
#include <limits>     
#include <cctype>    
#include <algorithm>  
#include <sstream>    

using namespace std;


struct Visit {
    string diagnosis;
    string assignedDoctor;
    string date;
    double amount;
    bool emergency;
};

struct Patient {
    int id;
    string fullName;
    int age;
    char gender; 
    vector<Visit> visitHistory; 

    
    Patient(int _id = 0, string _fullName = "", int _age = 0, char _gender = ' ')
        : id(_id), fullName(_fullName), age(_age), gender(_gender) {}
};


struct Node {
    Patient patient;
    Node* next;
    Node(const Patient& p) : patient(p), next(nullptr) {}
};

class PatientList {
private:
    Node* head;
    static int nextAvailableId; 

public:
    
    PatientList() : head(nullptr) {}

    
    ~PatientList() {
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
    }

    
    void addPatient(const Patient& p) {
        Node* newNode = new Node(p);
        if (!head) {
            head = newNode;
        } else {
            Node* current = head;
            while (current->next) {
                current = current->next;
            }
            current->next = newNode;
        }
    }

    
    Patient* findById(int id) {
        Node* current = head;
        while (current) {
            if (current->patient.id == id) {
                return &current->patient;
            }
            current = current->next;
        }
        return nullptr; 
    }

    
    void findByName(const string& name, vector<Patient*>& results) {
        Node* current = head;
        while (current) {
            if (current->patient.fullName == name) {
                results.push_back(&current->patient);
            }
            current = current->next;
        }
    }

    
    Node* getHead() const {
        return head;
    }

    
    static int getNextId() {
        return nextAvailableId++;
    }

   
    static void setNextId(int id) {
        if (id >= nextAvailableId) { 
            nextAvailableId = id + 1;
        }
    }
};


int PatientList::nextAvailableId = 1;


PatientList patients;


string getCurrentDate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char dateStr[11]; 
    snprintf(dateStr, sizeof(dateStr), "%04d-%02d-%02d",
             1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday);
    return string(dateStr);
}


bool isValidName(const string& name) {
    if (name.empty()) {
        return false;
    }
    for (char c : name) {
        if (!isalpha(c) && !isspace(c)) {
            return false;
        }
    }
    return true;
}


void addPatientRecord() {
    int patientIdInput;
    cout << "Enter patient ID if existing (must be > 0), or 0 to register a new patient: ";
    cin >> patientIdInput;
    
    if (cin.fail()) {
        cout << "Inv alid input. Please enter a number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 

    Patient* currentPatient = nullptr; 

    if (patientIdInput > 0) {
       
        currentPatient = patients.findById(patientIdInput);
        if (!currentPatient) {
            cout << "Patient with ID " << patientIdInput << " not found. Registering as a new patient.\n";
            patientIdInput = 0; 
        }
    }

    if (patientIdInput == 0) { 
        Patient newPatient;
        newPatient.id = PatientList::getNextId(); 

        cout << "Enter full name: ";
        getline(cin, newPatient.fullName);
        if (!isValidName(newPatient.fullName)) {
            cout << "Invalid name. Name must contain only letters and spaces. Patient not added.\n";
            PatientList::setNextId(newPatient.id - 1); 
            return;
        }

        cout << "Enter age: ";
        cin >> newPatient.age;
         
        if (cin.fail()) {
            cout << "Invalid input. Please enter a number for age.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            PatientList::setNextId(newPatient.id - 1);
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (newPatient.age < 0) {
            cout << "Invalid age. Must be non-negative. Patient not added.\n";
            PatientList::setNextId(newPatient.id - 1);
            return;
        }

        cout << "Enter gender (M/F): ";
        cin >> newPatient.gender;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        newPatient.gender = toupper(newPatient.gender); 
        if (newPatient.gender != 'M' && newPatient.gender != 'F') {
            cout << "Invalid gender. Please enter 'M' or 'F'. Patient not added.\n";
            PatientList::setNextId(newPatient.id - 1); 
            return;
        }

        patients.addPatient(newPatient); 
        currentPatient = patients.findById(newPatient.id); 
        cout << "New patient registered with ID: " << newPatient.id << ".\n";
    }

    
    if (currentPatient) {
        Visit newVisit;
        newVisit.date = getCurrentDate(); 

        int isEmergencyChoice;
        cout << "Is this an emergency visit? (1 - YES, 0 - NO): ";
        cin >> isEmergencyChoice;
        
        if (cin.fail()) {
            cout << "Invalid input. Please enter 1 for YES or 0 for NO.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        newVisit.emergency = (isEmergencyChoice == 1); 

        cout << "Enter diagnosis for this visit: ";
        getline(cin, newVisit.diagnosis);

        cout << "Assign doctor for this visit: ";
        getline(cin, newVisit.assignedDoctor);

        cout << "Enter total amount for this visit: ";
        cin >> newVisit.amount;
        
        if (cin.fail()) {
            cout << "Invalid input. Please enter a number for amount.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (newVisit.amount < 0) {
            cout << "Invalid amount. Must be non-negative. Setting amount to 0.\n";
            newVisit.amount = 0.0; 
        }

        currentPatient->visitHistory.push_back(newVisit); 
        cout << "Visit record added for patient ID " << currentPatient->id << ".\n";
    } else {
        
        cout << "Failed to add patient or visit record. An internal error occurred.\n";
    }
}

void updatePatientInfo() {
    int id;
    cout << "Enter patient ID to update core information: ";
    cin >> id;
   
    if (cin.fail()) {
        cout << "Invalid input. Please enter a number for ID.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (id <= 0) {
        cout << "Invalid ID. Must be a positive number.\n";
        return;
    }

    Patient* p = patients.findById(id);
    if (!p) {
        cout << "Patient not found.\n";
        return;
    }

    cout << "What core patient information do you want to update?\n";
    cout << "1. Full name\n2. Age\n3. Gender\n";
    int choice;
    cin >> choice;
    
    if (cin.fail()) {
        cout << "Invalid input. Please enter a number for your choice.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    switch (choice) {
        case 1: {
            string name;
            cout << "Enter new full name: ";
            getline(cin, name);
            if (!isValidName(name)) { 
                cout << "Invalid name. Name must contain only letters and spaces. Update failed.\n";
                return;
            }
            p->fullName = name;
            break;
        }
        case 2: {
            int age;
            cout << "Enter new age: ";
            cin >> age;
           
            if (cin.fail()) {
                cout << "Invalid input. Please enter a number for age.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (age < 0) { 
                cout << "Invalid age. Must be non-negative. Update failed.\n";
                return;
            }
            p->age = age;
            break;
        }
        case 3: {
            char gender;
            cout << "Enter new gender (M/F): ";
            cin >> gender;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            gender = toupper(gender); 
            if (gender != 'M' && gender != 'F') { 
                cout << "Invalid gender. Please enter 'M' or 'F'. Update failed.\n";
                return;
            }
            p->gender = gender;
            break;
        }
        default:
            cout << "Invalid choice.\n";
            return;
    }

    cout << "Patient core info updated.\n";
}
void searchPatient() {
    int choice;
    cout << "Search by: 1 - ID, 2 - Name: ";
    cin >> choice;
    
    if (cin.fail()) {
        cout << "Invalid input. Please enter a number.\n";
        cin.clear();
       // cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (choice == 1) { 
        int id;
        cout << "Enter ID: ";
        cin >> id;
        
        if (cin.fail()) {
            cout << "Invalid input. Please enter a number for ID.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (id <= 0) { 
            cout << "Invalid ID. Must be a positive number.\n";
            return;
        }

        Patient* p = patients.findById(id);
        if (p) {
            cout << "\n--- Patient Found (ID: " << p->id << ") ---\n";
            cout << "Full Name: " << p->fullName << "\n";
            cout << "Age: " << p->age << "\n";
            cout << "Gender: " << p->gender << "\n";
            cout << "--- Visit History ---\n";
            if (p->visitHistory.empty()) {
                cout << "No visit records found for this patient.\n";
            } else {
               
                for (size_t i = 0; i < p->visitHistory.size(); ++i) {
                    const Visit& v = p->visitHistory[i];
                    cout << "  Visit " << (i + 1) << ":\n";
                    cout << "    Date: " << v.date << "\n";
                    cout << "    Diagnosis: " << v.diagnosis << "\n";
                    cout << "    Doctor: " << v.assignedDoctor << "\n";
                    cout << "    Amount: " << v.amount << "\n";
                    cout << "    Emergency: " << (v.emergency ? "Yes" : "No") << "\n";
                }
            }
            cout << "--------------------------------------\n";
        } else {
            cout << "Patient not found.\n";
        }
    } else if (choice == 2) { 
        string name;
        cout << "Enter full name: ";
        getline(cin, name);

        vector<Patient*> results;
        patients.findByName(name, results); 
        if (results.empty()) {
            cout << "No patients found with that name.\n";
        } else {
            cout << "Found " << results.size() << " patient(s) with the name '" << name << "':\n";
            // Display details for each found patient
            for (auto* p : results) {
                cout << "\n--- Patient Found (ID: " << p->id << ") ---\n";
                cout << "Full Name: " << p->fullName << "\n";
                cout << "Age: " << p->age << "\n";
                cout << "Gender: " << p->gender << "\n";
                cout << "--- Visit History ---\n";
                if (p->visitHistory.empty()) {
                    cout << "  No visit records found for this patient.\n";
                } else {
                    for (size_t i = 0; i < p->visitHistory.size(); ++i) {
                        const Visit& v = p->visitHistory[i];
                        cout << "  Visit " << (i + 1) << ":\n";
                        cout << "    Date: " << v.date << "\n";
                        cout << "    Diagnosis: " << v.diagnosis << "\n";
                        cout << "    Doctor: " << v.assignedDoctor << "\n";
                        cout << "    Amount: " << v.amount << "\n";
                        cout << "    Emergency: " << (v.emergency ? "Yes" : "No") << "\n";
                    }
                }
                cout << "--------------------------------------\n";
            }
        }
    } else {
        cout << "Invalid choice.\n";
    }
}
void showAllRecordsForPatient() {
    int id;
    cout << "Enter patient ID to show all records: ";
    cin >> id;
   
    if (cin.fail()) {
        cout << "Invalid input. Please enter a number for ID.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (id <= 0) {
        cout << "Invalid ID. Must be a positive number.\n";
        return;
    }

    Patient* p = patients.findById(id);
    if (!p) {
        cout << "No records found for patient ID " << id << ".\n";
        return;
    }

    cout << "\n--- All Records for Patient ID: " << p->id << " (" << p->fullName << ") ---\n";
    if (p->visitHistory.empty()) {
        cout << "No visit records found for this patient.\n";
    } else {
       
        for (size_t i = 0; i < p->visitHistory.size(); ++i) {
            const Visit& v = p->visitHistory[i];
            cout << "  Visit " << (i + 1) << ":\n";
            cout << "    Date: " << v.date << "\n";
            cout << "    Diagnosis: " << v.diagnosis << "\n";
            cout << "    Doctor: " << v.assignedDoctor << "\n";
            cout << "    Amount: " << v.amount << "\n";
            cout << "    Emergency: " << (v.emergency ? "Yes" : "No") << "\n";
            cout << "  --------------------------------------\n";
        }
    }
    cout << "--------------------------------------\n";
}


void saveDataToFile(const string& filename) {
    ofstream file(filename);
    if (!file) {
        cout << "Error opening file for writing.\n";
        return;
    }

    Node* current = patients.getHead();
    while (current) {
        const Patient& p = current->patient;
        
        file << "PATIENT," << p.id << "," << p.fullName << "," << p.age << "," << p.gender << "\n";

        
        for (const auto& v : p.visitHistory) {
            file << "VISIT," << p.id << "," << v.diagnosis << "," << v.assignedDoctor << ","
                 << v.date << "," << v.amount << "," << (v.emergency ? "1" : "0") << "\n";
        }
        current = current->next;
    }

    file.close();
    cout << "Data saved to " << filename << "\n";
}


void loadDataFromFile(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cout << "No existing data file found (" << filename << "). Starting with empty system.\n";
        return;
    }

    string line;
    Patient* currentPatient = nullptr; 
    int maxId = 0; 

    while (getline(file, line)) {
        stringstream ss(line);
        string type;
        getline(ss, type, ','); 

        if (type == "PATIENT") {
            int id, age;
            string fullName, genderStr;
            
            ss >> id; ss.ignore();
            getline(ss, fullName, ',');
            ss >> age; ss.ignore(); 
            getline(ss, genderStr, ',');

            Patient newPatient(id, fullName, age, genderStr[0]);
            patients.addPatient(newPatient); 
            currentPatient = patients.findById(id); 

            if (id > maxId) {
                maxId = id;
            }

        } else if (type == "VISIT") {
            if (!currentPatient) {
                
                cerr << "Error: VISIT record found without a preceding PATIENT record. Skipping.\n";
                continue;
            }
            int patientIdFromFile;
            string diagnosis, assignedDoctor, date;
            double amount;
            int emergencyInt;

            ss >> patientIdFromFile; ss.ignore(); 
            getline(ss, diagnosis, ',');
            getline(ss, assignedDoctor, ',');
            getline(ss, date, ',');
            ss >> amount; ss.ignore(); 
            ss >> emergencyInt; 

            Visit newVisit;
            newVisit.diagnosis = diagnosis;
            newVisit.assignedDoctor = assignedDoctor;
            newVisit.date = date;
            newVisit.amount = amount;
            newVisit.emergency = (emergencyInt == 1);

            
            if (currentPatient->id == patientIdFromFile) {
                currentPatient->visitHistory.push_back(newVisit);
            } else {
                
                Patient* targetPatient = patients.findById(patientIdFromFile);
                if (targetPatient) {
                    targetPatient->visitHistory.push_back(newVisit);
                    currentPatient = targetPatient; 
                } else {
                    cerr << "Error: Visit for unknown patient ID " << patientIdFromFile << " encountered. Skipping.\n";
                }
            }
        }
    }
    file.close();
    PatientList::setNextId(maxId); 
    cout << "Data loaded from " << filename << ".\n";
}

int main() {
    
    loadDataFromFile("patients_data.csv");

    while (true) {
        cout << "\n--- Hospital Management System Menu ---\n";
        cout << "1. Add patient (or new visit for existing patient)\n";
        cout << "2. Search patient\n";
        cout << "3. Update patient core info\n";
        cout << "4. Show all records for a patient\n";
        cout << "5. Save data\n";
        cout << "6. Exit\n";
        cout << "Enter your choice: ";

        int choice;
        cin >> choice;
       
        if (cin.fail()) {
            cout << "Invalid input. Please enter a number from the menu.\n";
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 

        switch (choice) {
            case 1:
                addPatientRecord();
                break;
            case 2:
                searchPatient();
                break;
            case 3:
                updatePatientInfo();
                break;
            case 4:
                showAllRecordsForPatient();
                break;
            case 5:
                saveDataToFile("patients_data.csv");
                break;
            case 6:
                cout << "Exiting...\n";
                saveDataToFile("patients_data.csv"); 
                return 0; 
            default:
                cout << "Invalid choice, please try again.\n";
        }
    }
    return 0; 
}
