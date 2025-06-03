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