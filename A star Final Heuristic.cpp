#include <iostream>
#include <vector>
#include <queue>
#include <unordered_set>
#include <algorithm>
#include <string>
#include <memory>
#include <chrono>

using namespace std;


const vector<int> goal_state = {1, 2, 3, 8, 0, 4, 7, 6, 5};
const int PUZZLE_SIZE = 9;
const int BOARD_DIMENSION = 3;

struct Node {
    vector<int> state;
    int g, h;
    shared_ptr<Node> parent; // Change Node *parent to shared_ptr<Node> parent

    Node(const vector<int> &state, int g, int h, shared_ptr<Node> parent)
        : state(state), g(g), h(h), parent(parent) {}
};
struct SearchResult {
    vector<vector<int>> path;
    int num_nodes_generated;
    int num_nodes_expanded;
    int depth;
    double effective_branching_factor;
    double execution_time;
};

bool operator>(const Node &lhs, const Node &rhs) {
    return (lhs.g + lhs.h) > (rhs.g + rhs.h);
}

string state_to_string(const vector<int> &state) {
    string state_str = "";
    for (const int &tile : state) {
        state_str += to_string(tile);
    }
    return state_str;
}

int manhattan_distance(const vector<int> &state) {
    int distance = 0;
    for (int i = 0; i < PUZZLE_SIZE; ++i) {
        if (state[i] == 0) continue;
        distance += abs(i / BOARD_DIMENSION - (state[i] - 1) / BOARD_DIMENSION)
                  + abs(i % BOARD_DIMENSION - (state[i] - 1) % BOARD_DIMENSION);
    }
    return distance;
}

int corner_conflict(const vector<int> &state) {
    int conflicts = 0;

    auto check_corner_conflict = [&state](int corner, int adjacent1, int adjacent2, int correct1, int correct2) {
        if (state[corner] == correct1 && (state[adjacent1] == correct2 || state[adjacent2] == correct2)) {
            return 2;
        }
        return 0;
    };

    conflicts += check_corner_conflict(0, 1, 3, 1, 2);
    conflicts += check_corner_conflict(2, 1, 5, 3, 2);
    conflicts += check_corner_conflict(6, 3, 7, 7, 8);
    conflicts += check_corner_conflict(8, 5, 7, 5, 6);

    return conflicts;
}

int heuristic(const vector<int> &state) {
    return manhattan_distance(state) + corner_conflict(state);
}

vector<vector<int>> get_successors(const vector<int> &state) {
    const vector<int> dx = {-1, 0, 1, 0};
    const vector<int> dy = {0, 1, 0, -1};
    vector<vector<int>> successors;
    int zero_pos = find(state.begin(), state.end(), 0) - state.begin();
    int x = zero_pos / BOARD_DIMENSION;
    int y = zero_pos % BOARD_DIMENSION;

    for (int i = 0; i < 4; ++i) {
        int nx = x + dx[i];
        int ny = y + dy[i];
        if (nx >= 0 && nx < BOARD_DIMENSION && ny >= 0 && ny < BOARD_DIMENSION) {
            vector<int> successor = state;
            swap(successor[zero_pos], successor[nx * BOARD_DIMENSION + ny]);
            successors.push_back(successor);
        }
    }

    return successors;
}

void print_state(const vector<int> &state) {
    for (int i = 0; i < PUZZLE_SIZE; ++i) {
        if (i % BOARD_DIMENSION == 0) {
            cout << endl;
        }
        cout << state[i] << ' ';
    }
    cout << endl;
}

SearchResult astar(const vector<int> &initial_state) {
    auto start_time = chrono::steady_clock::now();

    priority_queue<Node, vector<Node>, greater<Node>> open;
    unordered_set<string> closed;
    int num_nodes_generated = 0;
    int num_nodes_expanded = 0;

    open.emplace(initial_state, 0, heuristic(initial_state), nullptr);
    num_nodes_generated++;

    while (!open.empty()) {
        Node current = open.top();
        open.pop();

        if (current.state == goal_state) {
            vector<vector<int>> path;
            shared_ptr<Node> node = make_shared<Node>(current);
            while (node) {
                path.push_back(node->state);
                node = node->parent;
            }
            reverse(path.begin(), path.end());

            auto end_time = chrono::steady_clock::now();
            chrono::duration<double> elapsed_time = end_time - start_time;
            
            int depth = path.size() - 1;
            double effective_branching_factor = static_cast<double>(num_nodes_generated) / depth;

            return SearchResult{path, num_nodes_generated, num_nodes_expanded, depth, effective_branching_factor, elapsed_time.count()};
        }

        string state_str = state_to_string(current.state);
        if (closed.find(state_str) != closed.end()) {
            continue;
        }
        closed.insert(state_str);
        num_nodes_expanded++;

        for (const vector<int> &successor : get_successors(current.state)) {
            open.emplace(successor, current.g + 1, heuristic(successor), make_shared<Node>(current));
            num_nodes_generated++;
        }
    }

    return SearchResult{};
}

int main() {
    vector<int> initial_state1 = {2, 8, 3, 1, 6, 4, 0, 7, 5};
    vector<int> initial_state2 = {2, 1, 6, 4, 0, 8, 7, 5, 3};

    int choice;
    cout << "Choose the initial state (1 or 2):" << endl;
    cout << "1. Initial state 1" << endl;
    cout << "2. Initial state 2" << endl;
    cin >> choice;

    vector<int> initial_state = (choice == 1) ? initial_state1 : initial_state2;

    SearchResult result = astar(initial_state);

    if (!result.path.empty()) {
        cout << "Solution found in " << result.path.size() - 1 << " steps:" << endl;
        for (const vector<int> &state : result.path) {
            print_state(state);
        }

        cout << "Execution Time: " << result.execution_time << " seconds" << endl;
        cout << "Number of Nodes Generated: " << result.num_nodes_generated << endl;
        cout << "Number of Nodes Expanded: " << result.num_nodes_expanded << endl;
        cout << "Depth of the Tree: " << result.depth << endl;
        cout << "Effective Branching Factor: " << result.effective_branching_factor << endl;
        cout << "Total Path: " << result.path.size() << endl;
    } else {
        cout << "No solution found." << endl;
    }

    return 0;
}