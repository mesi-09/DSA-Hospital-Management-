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
