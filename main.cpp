#include <limits>
#include <chrono> // Include for high-resolution timing
#include <csignal>
#include <cctype>
#include "include/Board.h"
#include "include/helper.h"
#include "include/compare.h"


#define RUN_ONCE

/*
To compile: 
g++ -std=c++20 -O3 -fno-omit-frame-pointer -o main main.cpp src/Board.cpp src/helper.cpp src/compare.cpp

To run:
./main
*/

std::string LOSING_FILE;
std::string WINNING_FILE;

void signalHandler(int signal) {
    std::cout << "Interrupt signal (" << signal << ") received.\n";
    // Save the current state of boards
    saveBoardsToFile(LOSING, LOSING_FILE);
    saveBoardsToFile(WINNING, WINNING_FILE);
    std::cout << "Boards saved due to SIGINT.\n";
    // Exit gracefully
    exit(signal);
}


int main() {
    // Start measuring time
    std::signal(SIGINT, signalHandler);

    int N, K;
    std::vector<std::vector<std::pair<int, int>>> curr, defaultCurr;

    char useDefaults;
    std::cout << "Do you want to use the default N, K values? (y/n): ";
    std::cin >> useDefaults;

    if (std::tolower(useDefaults) == 'n') {
        std::cout << "Enter value for K (Number of tokens): ";
        std::cin >> K;
        std::cout << "Enter value for N (Number of columns): ";
        std::cin >> N;
        // Initialize curr with the appropriate dimensions
        curr = std::vector<std::vector<std::pair<int, int>>>(N, std::vector<std::pair<int, int>>(K, {0, 0}));
        defaultCurr = curr;
    } else {
        // Default values
        curr = {
            {{0, 0}, {0, 0}, {0, 0}}, // Row 0
            {{0, 0}, {0, 0}, {0, 0}}, // Row 1
            {{0, 0}, {0, 0}, {0, 0}}, // Row 2
            {{0, 0}, {0, 0}, {0, 0}}, // Row 3
            {{0, 0}, {0, 0}, {0, 0}}, // Row 4
        };
        N = curr.size();
        K = curr[0].size();
        defaultCurr = curr;
        std::cout << "Using default board: N = " << N << ", K = " << K << std::endl;
    }
    // Initialize map
    initMap(N, K);

    int goal = 0;

    while (true) {
        // Create the board

        curr = defaultCurr;

        WINNING.clear();
        LOSING.clear();
        LOSING_BOUND = 20;
        WINNING_BOUND = 20;
        PREV_LOSING = 0;
        PREV_WINNING = 0;

        Board myBoard(N, K, goal, curr);

        std::cout << "Initial board: " << myBoard << std::endl;

        // Set file paths
        std::stringstream losing_ss;
        losing_ss << "losing/N" << N << "_K" << K << "_goal" << goal << "_board.txt";
        std::string LOSING_FILE = losing_ss.str();

        std::stringstream winning_ss;
        winning_ss << "winning/N" << N << "_K" << K << "_goal" << goal << "_board.txt";
        std::string WINNING_FILE = winning_ss.str();

        // Load boards from files
        //loadBoardsFromFile(LOSING_FILE, LOSING, N, K, goal);
        //loadBoardsFromFile(WINNING_FILE, WINNING, N, K, goal);

        // Start timing
        auto start = std::chrono::high_resolution_clock::now();

        // Execute negaMax algorithm
        int best = negaMax(myBoard, true, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), 0);
        bool continueGame = true;
        if (best == -1) {
            std::cout << "Pusher failed to reach goal of " << goal << std::endl;
            continueGame = false;
        } else {
            std::cout << "Pusher reached goal of " << best << std::endl;
        }

        // Stop timing
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
        std::cout << "Execution time: " << duration.count() << " seconds" << '\n';

        // Save boards to files
        //saveBoardsToFile(LOSING, LOSING_FILE);
        //saveBoardsToFile(WINNING, WINNING_FILE);
        if (!continueGame) {
            break;
        } else {
            goal++;
        }
        if (goal == 15) {
            break;
        }
    }
    std::cout << "Paintability of K_{" << K << "*" << N << "}: " << goal << std::endl;

    return 0;
}