#include <iostream>
#include <cstring>
using namespace std;

// ===== CONSTANTS =====
#define SINGLE 0
#define DOUBLE 1
#define SUITE 2

#define READY 0
#define BOOKED 1
#define OCCUPIED 2
#define UNAVAILABLE 3

// ===== STACK =====
struct Booking {
    char name[50];
    int roomID;
    int days;
};

class Stack {
    Booking arr[50];
    int top;
public:
    Stack() { top = -1; }

    void push(Booking b) {
        arr[++top] = b;
    }

    Booking pop() {
        return arr[top--];
    }

    bool empty() {
        return top == -1;
    }

    void display() {
        if (empty()) {
            cout << "Booking history empty\n";
            return;
        }
        for (int i = top; i >= 0; i--) {
            cout << arr[i].name << " | Room "
                 << arr[i].roomID << " | "
                 << arr[i].days << " days\n";
        }
    }
};

// ===== QUEUE =====
struct Request {
    char name[50];
    int type;
    int days;
};

class Queue {
    Request arr[50];
    int front, rear;
public:
    Queue() {
        front = 0;
        rear = -1;
    }

    void push(Request r) {
        arr[++rear] = r;
    }

    Request pop() {
        return arr[front++];
    }

    bool empty() {
        return front > rear;
    }
};

// ===== ROOM =====
struct Room {
    int id, floor, type, status;
    int avail[30];
    Stack history;
};

// ===== LINKED LIST =====
struct Node {
    Room room;
    Node* next;
};

class RoomList {
public:
    Node* head;
    RoomList() { head = NULL; }

    void add(Room r) {
        Node* n = new Node;
        n->room = r;
        n->next = head;
        head = n;
    }
};

// ===== TREE =====
struct Tree {
    int id;
    Room* room;
    Tree* left;
    Tree* right;
};

Tree* insert(Tree* root, int id, Room* r) {
    if (!root) {
        Tree* t = new Tree;
        t->id = id;
        t->room = r;
        t->left = t->right = NULL;
        return t;
    }
    if (id < root->id)
        root->left = insert(root->left, id, r);
    else
        root->right = insert(root->right, id, r);
    return root;
}

Room* search(Tree* root, int id) {
    if (!root) return NULL;
    if (root->id == id) return root->room;
    if (id < root->id) return search(root->left, id);
    return search(root->right, id);
}

// ===== HOTEL =====
class Hotel {
    RoomList rooms;
    Tree* tree;
    Queue highQ, normalQ;

public:
    Hotel() { tree = NULL; }

    void createHotel() {
        int id = 101;
        for (int f = 1; f <= 5; f++) {
            for (int r = 1; r <= 10; r++) {
                Room rm;
                rm.id = id++;
                rm.floor = f;
                rm.type = r % 3;
                rm.status = READY;

                for (int i = 0; i < 30; i++)
                    rm.avail[i] = 0;

                rooms.add(rm);
                tree = insert(tree, rm.id, &rooms.head->room);
            }
        }
    }

    void addRequest() {
        Request r;
        int p;
        cout << "Customer Name: ";
        cin >> r.name;
        cout << "Room Type (0 Single, 1 Double, 2 Suite): ";
        cin >> r.type;
        cout << "Stay Days (<=30): ";
        cin >> r.days;
        cout << "High Priority? (1 Yes / 0 No): ";
        cin >> p;

        if (p) highQ.push(r);
        else normalQ.push(r);
    }

    bool available(Room& r, int days) {
        for (int i = 0; i < days; i++)
            if (r.avail[i] == 1)
                return false;
        return true;
    }

    void book(Request req) {
        Node* t = rooms.head;
        while (t) {
            if (t->room.type == req.type &&
                t->room.status == READY &&
                available(t->room, req.days)) {

                for (int i = 0; i < req.days; i++)
                    t->room.avail[i] = 1;

                t->room.status = BOOKED;

                Booking b;
                strcpy(b.name, req.name);
                b.roomID = t->room.id;
                b.days = req.days;
                t->room.history.push(b);

                cout << "Room " << t->room.id
                     << " BOOKED for " << req.name << endl;
                return;
            }
            t = t->next;
        }
        cout << "No room available for " << req.name << endl;
    }

    void processRequests() {
        int c = 0;
        while (!highQ.empty() && c < 10) {
            book(highQ.pop());
            c++;
        }
        while (!normalQ.empty() && c < 10) {
            book(normalQ.pop());
            c++;
        }
        cout << "Processed " << c << " requests\n";
    }

    void checkIn() {
        int id;
        cout << "Room ID for Check-in: ";
        cin >> id;

        Room* r = search(tree, id);
        if (r && r->status == BOOKED) {
            r->status = OCCUPIED;
            cout << "Customer checked into Room " << id << endl;
        } else {
            cout << "Room not booked\n";
        }
    }

    void cancelBooking() {
        int id;
        cout << "Room ID: ";
        cin >> id;

        Room* r = search(tree, id);
        if (r && !r->history.empty()) {
            Booking b = r->history.pop();
            for (int i = 0; i < b.days; i++)
                r->avail[i] = 0;
            r->status = READY;
            cout << "Cancelled booking of " << b.name << endl;
        } else {
            cout << "No booking found\n";
        }
    }

    void viewOccupied() {
        int c = 0;
        Node* t = rooms.head;
        while (t) {
            if (t->room.status == OCCUPIED)
                c++;
            t = t->next;
        }
        cout << "Occupied Rooms: " << c << endl;
    }

    void showHistory(int id) {
        Room* r = search(tree, id);
        if (r) r->history.display();
        else cout << "Room not found\n";
    }
};

// ===== MAIN =====
int main() {
    Hotel h;
    h.createHotel();

    int ch;
    do {
        cout << "\n1 Add Request\n2 Process Requests\n3 Check-in\n"
             << "4 Cancel Booking\n5 View Occupied\n6 View History\n7 Exit\n";
        cin >> ch;

        switch (ch) {
        case 1: h.addRequest(); break;
        case 2: h.processRequests(); break;
        case 3: h.checkIn(); break;
        case 4: h.cancelBooking(); break;
        case 5: h.viewOccupied(); break;
        case 6:
            int id;
            cout << "Room ID: ";
            cin >> id;
            h.showHistory(id);
            break;
        }
    } while (ch != 7);

    return 0;
}
