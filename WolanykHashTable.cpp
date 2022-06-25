// Name        : WolanykHashTable.cpp
// Author      : Jonathan Wolanyk
// Date        : 6/18/2022

#include <algorithm>
#include <climits>
#include <iostream>
#include <fstream>
#include <string> // atoi
#include <vector>
#include <sstream>

using namespace std;

//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

const unsigned int DEFAULT_SIZE = 179;

// forward declarations
double strToDouble(string str, char ch);

// define a structure to hold course information
struct Course {
    string courseId; // unique identifier
    string title;
    vector<string> prerequisites;
};

//============================================================================
// Hash Table class definition
//============================================================================

/**
 * Define a class containing data members and methods to
 * implement a hash table with chaining.
 */
class HashTable {

private:
    // Define structures to hold courses
    struct Node {
        Course course;
        unsigned int key;
        Node* next;

        // default constructor
        Node() {
            key = UINT_MAX;
            next = nullptr;
        }

        // initialize with a course
        Node(Course aCourse) : Node() {
            course = aCourse;
        }

        // initialize with a course and a key
        Node(Course aCourse, unsigned int aKey) : Node(aCourse) {
            key = aKey;
        }
    };

    vector<Node> nodes;

    unsigned int tableSize = DEFAULT_SIZE;

    unsigned int hash(int key);

public:
    HashTable();
    HashTable(unsigned int size);
    virtual ~HashTable();
    void Insert(Course course);
    void PrintAll(string filePath);
    Course Search(string courseId);
};

/**
 * Default constructor
 */
HashTable::HashTable() {

    // Initalize node structure by resizing tableSize
    nodes.resize(tableSize);
}

/**
 * Constructor for specifying size of the table
 * Use to improve efficiency of hashing algorithm
 * by reducing collisions without wasting memory.
 */
HashTable::HashTable(unsigned int size) {
    // invoke local tableSize to size with this->
    this->tableSize = size;
    // resize nodes size
    nodes.resize(tableSize);
}


/**
 * Destructor
 */
HashTable::~HashTable() {
    // erase nodes beginning
    nodes.erase(nodes.begin());
}

/**
 * Calculate the hash value of a given key.
 * Note that key is specifically defined as
 * unsigned int to prevent undefined results
 * of a negative list index.
 *
 * @param key The key to hash
 * @return The calculated hash
 */
unsigned int HashTable::hash(int key) {
    // return key tableSize
    return key % tableSize;
}

/**
 * Insert a course
 *
 * @param course The course to insert
 */
void HashTable::Insert(Course course) {
    // create the key for the given course, atoi turns string to int
    unsigned key = hash(atoi(course.courseId.c_str()));
    // retrieve node using key
    Node* prevNode = &(nodes.at(key));
    // if no entry found for the key
    if (prevNode == nullptr) {
        // assign this node to the key position
        Node* currNode = new Node(course, key);
        nodes.insert(nodes.begin() + key, *currNode);
    }

    // else if node is not used
    else {
        // assign old node key to UINT_MAX, set to key, set old node to course and old node next to null pointer
        // If key has not been used, assign key, course, and make next pointer null
        if (prevNode->key == UINT_MAX) {
            prevNode->key = key;
            prevNode->course = course;
            prevNode->next = nullptr;
        }

        // else find the next open node
        else {
            // add new new node to end using while loop
            while (prevNode->next != nullptr) {
                prevNode = prevNode->next;
            }
            // Make previous node point to new node at the end
            prevNode->next = new Node(course, key);
        }
    }


}

/**
 * Print all courses
 */
void HashTable::PrintAll(string filePath) {
    vector<string> lines;
    fstream courseFile;

    // opens file to read
    courseFile.open(filePath, ios::in);
    if (courseFile.is_open()) {
        string line;

        // populates vector that stores lines of file
        while (getline(courseFile, line)) {
            lines.push_back(line);
        }

        // closes file when finished reading
        courseFile.close();
    }

    sort(lines.begin(), lines.end());

    // for loop to split lines
    for (unsigned int i = 0; i < lines.size(); i++) {
        Course course;
        istringstream ss(lines[i]);
        string token;
        int j = 0;

        // splits line using comma as delimiter
        while (getline(ss, token, ',')) {
            // first term is the class ID
            if (j == 0) {
                course.courseId = token;
            }

            // second term is the class title 
            else if (j == 1) {
                course.title = token;
            }

            // terms past the second term are prerequisites
            else if (j >= 2) {
                course.prerequisites.push_back(token);
            }

            // iterates through the line using integer
            j++;
        }

        // output courseID, title, amount and prerequisites
        cout << "ID: " << course.courseId << " ";
        cout << "| Title: " << course.title << " ";
        if (course.prerequisites.size() > 0) {
            cout << "| Prerequisites: ";
            for (int i = 0; i < course.prerequisites.size(); i++) {
                cout << course.prerequisites[i] << ". ";
            }
        }
        cout << endl;
    }
    cout << endl;
}

/**
 * Search for the specified courseId
 *
 * @param courseId The course id to search for
 */
Course HashTable::Search(string courseId) {
    Course course;


    // create the key for the given course
    unsigned key = hash(atoi(courseId.c_str()));
    Node* node = &(nodes.at(key));

    // if entry found for the key
    if ((node != nullptr) && (node->key != UINT_MAX) && (node->course.courseId.compare(courseId) == 0)) {
        //return node course
        return node->course;
    }

    // if no entry found or it is initialized to default value
    if ((node == nullptr) || (node->key == UINT_MAX)) {
        // return course
        return course;
    }
    // while node not equal to nullptr
    while (node != nullptr) {
        // if the current node matches, return it
        if (node->key != UINT_MAX && node->course.courseId.compare(courseId) == 0) {
            return node->course;
        }
        //node is equal to next node
        node = node->next;
    }

    return course;
}

//============================================================================
// Static methods used for testing
//============================================================================

/**
 * Display the course information to the console (std::out)
 *
 * @param course struct containing the course info
 */
void displayCourse(Course course) {
    // output key, courseID, title, amount and prerequisites
    cout << "ID: " << course.courseId << " ";
    cout << "| Title: " << course.title << " ";
    if (course.prerequisites.size() > 0) {
        cout << "| Prerequisites: ";
        for (int i = 0; i < course.prerequisites.size(); i++) {
            cout << course.prerequisites[i] << ". ";
        }
    }
    cout << endl << endl;
    return;
}

/**
 * Load a CSV file containing courses into a container
 *
 * @param csvPath the path to the CSV file to load
 * @return a container holding all the courses read
 */
void loadCourses(string filePath, HashTable* hashTable) {
    cout << "Loading CSV file " << filePath << "... " << endl;
    vector<string> lines;
    fstream courseFile;

    // opens file to read
    courseFile.open(filePath, ios::in);
    if (courseFile.is_open()) {
        string line;

        // populates vector that stores lines of file
        while (getline(courseFile, line)) {
            lines.push_back(line);
        }

        // closes file when finished reading
        courseFile.close();
    }

    // for loop to split lines
    for (unsigned int i = 0; i < lines.size(); i++) {
        Course course;
        istringstream ss(lines[i]);
        string token;
        int j = 0;

        // splits line using comma as delimiter
        while (getline(ss, token, ',')) {
            // first term is the class ID
            if (j == 0) {
                course.courseId = token;
            }

            // second term is the class title 
            else if (j == 1) {
                course.title = token;
            }

            // terms past the second term are prerequisites
            else if (j >= 2) {
                course.prerequisites.push_back(token);
            } 

            // iterates through the line using integer
            j++;
        }

        // inserts course into hash table
        hashTable->Insert(course);
    }
    cout << "All courses from " << filePath << " have been loaded successfully. " << endl << endl;
}

/**
 * Simple C function to convert a string to a double
 * after stripping out unwanted char
 *
 * credit: http://stackoverflow.com/a/24875936
 *
 * @param ch The character to strip out
 */
double strToDouble(string str, char ch) {
    str.erase(remove(str.begin(), str.end(), ch), str.end());
    return atof(str.c_str());
}

/**
 * The one and only main() method
 */
int main(int argc, char* argv[]) {

    // process command line arguments
    string filePath, courseKey;
    switch (argc) {
    case 2:
        filePath = argv[1];
        break;
    case 3:
        filePath = argv[1];
        courseKey = argv[2];
        break;
    default:
        filePath = "courses.txt";
    }


    // Define a hash table to hold all the courses
    HashTable* courseTable;

    Course course;
    string courseID;
    courseTable = new HashTable();

    int choice = 0;
    while (choice != 9) {
        cout << "Welcome to the course planner:" << endl;
        cout << "  1. Load Data Structure from File" << endl;
        cout << "  2. Print Sorted Course List" << endl;
        cout << "  3. Print Specific Course" << endl;
        cout << "  9. Exit" << endl;
        cout << "What would you like to do? ";
        cin >> choice;
        cin.ignore();

        switch (choice) {

        case 1:
            cout << "Please enter the file name: ";
            getline(cin, filePath);
            cout << endl;

            // Complete the method call to load the bids
            loadCourses(filePath, courseTable);
            break;

        case 2:
            courseTable->PrintAll(filePath);
            break;

        case 3:
            cout << "Please enter the course ID: ";
            getline(cin, courseID);

            course = courseTable->Search(courseID);

            if (!course.courseId.empty()) {
                displayCourse(course);
            }
            else {
                cout << "Course ID " << courseID << " not found." << endl;
            }

            break;
        }
    }

    cout << "Good bye." << endl;

    return 0;
}
