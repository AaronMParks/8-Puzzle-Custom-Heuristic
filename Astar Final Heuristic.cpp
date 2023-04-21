
#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <sys/time.h>

using namespace std;

// The size of the puzzle board
#define BOARD_SIZE 3

// The goal state of the puzzle
int goal_state[BOARD_SIZE][BOARD_SIZE] = {
    {1, 2, 3},
    {8, 0, 4},
    {7, 6, 5}
};

int numOutPlace(int b[BOARD_SIZE][BOARD_SIZE]) {
    int outPlace = 0;
    for(int row=0; row < BOARD_SIZE; row++) {
        for(int column=0; column < BOARD_SIZE; column++) {
            if(goal_state[row][column] != b[row][column]) {
                outPlace++;
            }
        }
    }
    return outPlace;
}

// The node of the puzzle board
struct Node {
    int board[BOARD_SIZE][BOARD_SIZE]; // The current state of the board
    int g, h, f; // The values used by A* FINAL algorithm
    Node* parent; // The parent node
    vector<Node*> successors; // The children

    Node(int b[BOARD_SIZE][BOARD_SIZE], int g_, int h_, Node* parent_) {
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                board[i][j] = b[i][j];
            }
        }
        g = g_;
        h = h_;
        f = g + h;
        parent = parent_;
    }

    bool operator==(const Node& other) const {
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (board[i][j] != other.board[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }
};

// The hash function for the unordered_map
struct NodeHash {
    size_t operator()(const Node* n) const {
        size_t h = 0;
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                h = h * 31 + n->board[i][j];
            }
        }
        return h;
    }
};

// The comparator function for the priority queue
struct NodeCompare {
    bool operator()(const Node* n1, const Node* n2) const {
        return n1->f > n2->f;
    }
};

// Find the location of 0
vector<int> zeroLocation(int b[BOARD_SIZE][BOARD_SIZE]) {
    for(int row=0; row < BOARD_SIZE; row++) {
        for(int column=0; column < BOARD_SIZE; column++) {
            if(b[row][column] == 0) {
                return {row, column};
            }
        }
    }
    return {0, 0};
}


double getCost(Node* start_node, Node* end_node) {
    // Compute the cost of the path between the start and end nodes
    double cost = 0.0;
    Node* current_node = end_node;
    while (current_node != start_node) {
        cost += current_node->g - current_node->parent->g;
        current_node = current_node->parent;
    }
    return cost;
}

// DFS through recursion to update node within closed
void propagateImprovement(Node* node) {
    for (Node* successor : node->successors) {
        // If the successor's parent is the current node, continue propagation
        if (successor->parent == node) {
            // Calculate the new g value for the successor
            double new_g = node->g + getCost(node, successor);
            // If the new g value is better than the current g value, update the parent and g values
            if (new_g < successor->g) {
                successor->parent = node;
                successor->g = new_g;
                // Recursively propagate improvement to the successor's descendants
                propagateImprovement(successor);
            }
        }
    }
}

void boardOutput(int b[BOARD_SIZE][BOARD_SIZE]) {
    cout << " -------------" << endl;
    cout << " | " << b[0][0] << " | " << b[0][1] << " | " << b[0][2] << " | " <<  endl;
    cout << " -------------" << endl;
    cout << " | " << b[1][0] << " | " << b[1][1] << " | " << b[1][2] << " | " << endl;
    cout << " -------------" << endl;
    cout << " | " << b[2][0] << " | " << b[2][1] << " | " << b[2][2] << " | " << endl;
    cout << " -------------" << endl;
}

// The A* FINAL algorithm function
void AStar(int initial_state[BOARD_SIZE][BOARD_SIZE]) {
    // The open and closed lists
    priority_queue<Node*, vector<Node*>, NodeCompare> open;
    unordered_map<Node*, bool, NodeHash> closed;

    int depth = 0;
    // The initial node
    int h = numOutPlace(initial_state);
    Node* initial_node = new Node(initial_state, 0, h, nullptr);

    // Add the initial node to the open list
    open.push(initial_node);

    while (!open.empty()) {
        // Get the node with the lowest f value from the open list
        Node* best_node = open.top();
        open.pop();
        boardOutput(best_node->board);
        cout << endl << endl;
        depth = best_node->g;

        // Add the best node to the closed list
        closed[best_node] = true;

        // Check if the best node is the goal state
        if (*best_node == Node(goal_state, 0, 0, nullptr)) {
            break;
        }

        vector<int>zero = zeroLocation(best_node->board);
        int row = zero[0];
        int column = zero[1];

        // Expand the current node

        if (row - 1 >= 0) {
            // Move the tile UP
            int new_board[BOARD_SIZE][BOARD_SIZE];
            memcpy(new_board, best_node->board, sizeof(new_board));
            swap(new_board[row][column], new_board[row - 1][column]);
            int h = numOutPlace(new_board);
            Node* new_node = new Node(new_board, best_node->g + 1, h, best_node);
            if (closed.find(new_node) == closed.end()) {
                // The child is not in the closed list, check to see if in open list
                vector<Node*> open_vector;
                bool already_in_open = false;
                while (!open.empty()) {
                    Node* n = open.top();
                    open.pop();
                    if (*n == *new_node) {
                        already_in_open = true;
                        if (new_node->g < n->g) {
                            // Found a cheaper path to an already generated node on OPEN
                            // Reset its parent link to point to BESTNODE
                            n->parent = best_node;
                            n->g = new_node->g;
                            n->f = n->g + n->h;
                            // We changed the cost of n, we need to reorder the queue
                            open.push(n);
                        }
                        break;
                    }
                    else{
                        open_vector.push_back(n);
                    }
                }
                for(int i=0; i<open_vector.size(); i++) {
                    open.push(open_vector[i]);
                }
                if (!already_in_open) {
                    new_node->parent = best_node;
                    open.push(new_node);
                    best_node->successors.push_back(new_node);
                }
            }
            else {
                // Node was already in CLOSED
                auto it_closed = closed.find(new_node);
                if (it_closed != closed.end()) {
                    Node* old_node = it_closed->first;
                    if (new_node->g < old_node->g) {
                        // Found a cheaper path to an already explored node on CLOSED
                        // Reset its parent link to point to BESTNODE
                        old_node->parent = best_node;
                        old_node->g = new_node->g;
                        
                        // Propagate the improvement downward to the old node's successors
                        propagateImprovement(old_node);
                        
                        // Add old node as a successor of BESTNODE
                        best_node->successors.push_back(old_node);
                    }
                }
            }
        }
        if (column + 1 <= BOARD_SIZE - 1) {
            // // Move the tile RIGHT
            int new_board[BOARD_SIZE][BOARD_SIZE];
            memcpy(new_board, best_node->board, sizeof(new_board));
            swap(new_board[row][column], new_board[row][column + 1]);
            int h = numOutPlace(new_board);
            Node* new_node = new Node(new_board, best_node->g + 1, h, best_node);
            if (closed.find(new_node) == closed.end()) {
                vector<Node*> open_vector;
                bool already_in_open = false;
                while (!open.empty()) {
                    Node* n = open.top();
                    open.pop();
                    if (*n == *new_node) {
                        already_in_open = true;
                        if (new_node->g < n->g) {
                            n->parent = best_node;
                            n->g = new_node->g;
                            n->f = n->g + n->h;
                            open.push(n);
                        }
                        break;
                    }
                    else{
                        open_vector.push_back(n);
                    }
                }
                for(int i=0; i<open_vector.size(); i++) {
                    open.push(open_vector[i]);
                }
                if (!already_in_open) {
                    new_node->parent = best_node;
                    open.push(new_node);
                    best_node->successors.push_back(new_node);
                }
            }
            else {
                auto it_closed = closed.find(new_node);
                if (it_closed != closed.end()) {
                    Node* old_node = it_closed->first;
                    if (new_node->g < old_node->g) {
                        old_node->parent = best_node;
                        old_node->g = new_node->g;
                        propagateImprovement(old_node);
                        best_node->successors.push_back(old_node);
                    }
                }
            }
        }
        if (row + 1 <= BOARD_SIZE - 1) {
            // // Move the tile DOWN
            int new_board[BOARD_SIZE][BOARD_SIZE];
            memcpy(new_board, best_node->board, sizeof(new_board));
            swap(new_board[row][column], new_board[row + 1][column]);
            int h = numOutPlace(new_board);
            Node* new_node = new Node(new_board, best_node->g + 1, h, best_node);
            if (closed.find(new_node) == closed.end()) {
                vector<Node*> open_vector;
                bool already_in_open = false;
                while (!open.empty()) {
                    Node* n = open.top();
                    open.pop();
                    if (*n == *new_node) {
                        already_in_open = true;
                        if (new_node->g < n->g) {
                            n->parent = best_node;
                            n->g = new_node->g;
                            n->f = n->g + n->h;
                            open.push(n);
                        }
                        break;
                    }
                    else{
                        open_vector.push_back(n);
                    }
                }
                for(int i=0; i<open_vector.size(); i++) {
                    open.push(open_vector[i]);
                }
                if (!already_in_open) {
                    new_node->parent = best_node;
                    open.push(new_node);
                    best_node->successors.push_back(new_node);
                }
            }
            else {
                auto it_closed = closed.find(new_node);
                if (it_closed != closed.end()) {
                    Node* old_node = it_closed->first;
                    if (new_node->g < old_node->g) {
                        old_node->parent = best_node;
                        old_node->g = new_node->g;
                        propagateImprovement(old_node);
                        best_node->successors.push_back(old_node);
                    }
                }
            }
        }
        if (column - 1 >= 0) {
            // // Move the tile LEFT
            int new_board[BOARD_SIZE][BOARD_SIZE];
            memcpy(new_board, best_node->board, sizeof(new_board));
            swap(new_board[row][column], new_board[row][column - 1]);
            int h = numOutPlace(new_board);
            Node* new_node = new Node(new_board, best_node->g + 1, h, best_node);
            if (closed.find(new_node) == closed.end()) {
                vector<Node*> open_vector;
                bool already_in_open = false;
                while (!open.empty()) {
                    Node* n = open.top();
                    open.pop();
                    if (*n == *new_node) {
                        already_in_open = true;
                        if (new_node->g < n->g) {
                            n->parent = best_node;
                            n->g = new_node->g;
                            n->f = n->g + n->h;
                            open.push(n);
                        }
                        break;
                    }
                    else{
                        open_vector.push_back(n);
                    }
                }
                for(int i=0; i<open_vector.size(); i++) {
                    open.push(open_vector[i]);
                }
                if (!already_in_open) {
                    new_node->parent = best_node;
                    open.push(new_node);
                    best_node->successors.push_back(new_node);
                }
            }
            else {
                auto it_closed = closed.find(new_node);
                if (it_closed != closed.end()) {
                    Node* old_node = it_closed->first;
                    if (new_node->g < old_node->g) {
                        old_node->parent = best_node;
                        old_node->g = new_node->g;
                        propagateImprovement(old_node);
                        best_node->successors.push_back(old_node);
                    }
                }
            }
        }
    }
    cout << "GOAL FOUND ^^" << endl;
    int NG = open.size() + closed.size();
    cout << "Nodes Generated = " << NG << endl;
    int NE = closed.size();
    cout << "Nodes Expanded = " << NE << endl;
    int D = depth;
    cout << "Depth of Tree = " << D << endl;
    float b_star = NG / D;
    cout << "Effective Branch Factor = " << b_star << endl;
    int TP = D + 1;
    cout << "Total Path = " << TP << endl;
}

int main() {
    // The initial state of the puzzle
    int initial_state1[BOARD_SIZE][BOARD_SIZE] = {
    {2, 8, 3},
    {1, 6, 4},
    {0, 7, 5}};
    int initial_state2[BOARD_SIZE][BOARD_SIZE] = {
    {2, 1, 6},
    {4, 0, 8},
    {7, 5, 3}};
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);
    cout << "START BOARD" << endl;
    AStar(initial_state2);
    gettimeofday(&end_time, NULL);
	long elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000000 + (end_time.tv_usec - start_time.tv_usec);
    cout << "Elapsed Time = " << elapsed_time << " micro-seconds" << endl;
};