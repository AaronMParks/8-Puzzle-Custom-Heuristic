# 8-Puzzle-Custom-Heuristic
A custom heuristic to the popular sliding 8-puzzle game.
This program is a game playing program that solves the 8-puzzle game, a sliding puzzle that consists of a 3x3 grid with eight numbered tiles and one empty tile. The goal of the game is to arrange the tiles in numerical order by sliding them around the grid using the empty space.

The program uses the A* search algorithm with heuristics to efficiently search the state space and find the optimal solution to the game. It provides the user with the ability to choose between two initial states for the puzzle, and outputs the steps required to solve the puzzle, the execution time, and the number of nodes generated and expanded during the search.

The program implements two different heuristics to estimate the distance between the current state and the goal state: the Manhattan distance and the corner conflict heuristic. The Manhattan distance heuristic calculates the sum of the horizontal and vertical distances of each tile from its correct position. The corner conflict heuristic adds to the Manhattan distance by penalizing moves that would cause conflicts between the tiles in the corner of the grid. 

The program provides the user with the ability to choose between these three heuristics, allowing for flexibility in the search strategy used to solve the puzzle. The choice of heuristic can significantly impact the execution time and number of nodes generated during the search.

Overall, this program provides an efficient and flexible solution to solving the 8-puzzle game using the A* search algorithm and heuristics.

1. Clone the repository to your local machine:
    
    git clone https://github.com/username/8-Puzzle-Custom-Heuristic.git

2. Compile the program using your C++ compiler:
    
    g++ main.cpp -o 8-Puzzle-Custom-Heuristic

3. Run the program
    
    ./8-Puzzle-Custom-Heuristic

Usage

The program will prompt you to choose between two initial states for the 8-puzzle game. Once you select an initial state, the program will run the A* search algorithm with the chosen heuristic and display the solution path, execution time, number of nodes generated, number of nodes expanded, depth of the tree, and effective branching factor.
Heuristics

The program includes two heuristics that can be used to solve the 8-puzzle game:

    Manhattan distance
    Corner conflict

To choose a heuristic, modify the heuristic variable in the main() function.

Contributing

If you find any bugs or have suggestions for improvements, please feel free to create an issue or submit a pull request.
License

This program is licensed under the MIT License.
