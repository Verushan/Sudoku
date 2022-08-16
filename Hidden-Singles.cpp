#include <chrono>
#include <cmath>
#include <iostream>
#include <set>
#include <sstream>
#include <stack>
#include <vector>

using namespace std;

int GRID_SIZE;
int boxSize;

vector<vector<int>> grid;
vector<vector<set<int>>> cand;
stack<pair<int, int>> moves;

bool isSafe(
    int &row, int &col,
    int &value) // Used to check if a given value is possible in a certain cell
{
    for (int i = 0; i < GRID_SIZE; i++) {
        if (grid[i][col] == value ||
            grid[row][i] ==
                value) // This checks the row and columns for the value
            return false;
    }

    int rStart = (row / boxSize) * boxSize, cStart = (col / boxSize) * boxSize;
    // rStart and cStart are used to get the start coordinate for the box our
    // current cell resides in

    for (int r = 0; r < boxSize; r++) {
        for (int c = 0; c < boxSize; c++)
            if (grid[rStart + r][cStart + c] ==
                value) // Checking if a cell in the box contains this value
                return false;
    }

    return true; // If no conflicts were found we can say that this value is
                 // possible for this cell
}

void Display() // Used to print the grid to the terminal
{
    for (int i = 0; i < 2 * GRID_SIZE; i++)
        cout << '-';

    cout << endl;

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] >= 10)
                cout << char(grid[i][j] +
                             55); // Print out the corresponding letter
            else
                cout << grid[i][j]; // Otherwise print the numerical value

            if (j != GRID_SIZE - 1) // Makes sure we're not at the last element
                                    // otherwise it doesn't print a space
                cout << " ";
        }

        cout << endl;
    }
}

bool getEmptyPos(int &r, int &c) {
    /*
        This is a slightly optimized method for choosing an empty cell, this
       code looks for a cell with the least amount of candidates resulting in
       more accurate guesses and minimizes backtracking
    */

    int lowRow, lowCol;
    int lowCount = 10;
    bool found = false;

    for (r = 0; r < GRID_SIZE; r++) {
        for (c = 0; c < GRID_SIZE; c++) {
            if (grid[r]
                    [c]) // If a value is already present we can skip the check
                continue;

            if (cand[r][c].size() == 2 ||
                cand[r][c].size() == 3) // These amount of candidates are ideal
                                        // and are the most efficient options
                return true;

            else if (cand[r][c].size() < lowCount) {
                /*
                    If we find a cell with a small number of candidates that
                   we've found so far, we can take it as our best option so far
                   until we find a better option or we have checked all empty
                   cell's candidates
                */
                lowCount = cand[r][c].size();
                lowRow = r;
                lowCol = c;
                found = true;
            }
        }
    }

    if (found) {
        r = lowRow;
        c = lowCol;
    }

    return found;
}

void updateCand(int &row, int &col, int &val) {
    /*
        This function is used to update the candidates matrix.
        When a certain value is inserted into the grid, the
        corresponding row, column and box is updated meaning,
        we remove the inserted value from all affected cell's candidates
    */

    grid[row][col] = val; // Set the value in the grid

    // Clear it's set since we've inserted it into the grid
    cand[row][col].clear();

    // Add this move to the moves stack incase we need to backtrack
    moves.push({row, col});

    for (int r = 0; r < GRID_SIZE; r++) {
        // Removing this value as a candidate from affected row and column
        cand[r][col].erase(val);
        cand[row][r].erase(val);
    }

    int rowStart = (row / boxSize) * boxSize,
        colStart = (col / boxSize) * boxSize;

    // Removes this value as a candidates from cells in the same box
    for (int r = 0; r < boxSize; r++) {
        for (int c = 0; c < boxSize; c++)
            cand[rowStart + r][colStart + c].erase(val);
    }
}

void setNakedSingles() {
    bool found;
    /*
        We use this to stop checking if no more naked singles were found.
        We do this because finding naked singles and updating candidates could
        generate new naked singles.
    */

    do {
        found = false; // Resets the check

        for (int r = 0; r < GRID_SIZE; r++) {
            for (int c = 0; c < GRID_SIZE; c++) {
                // If a cell only has 1 candidate, we can immediately
                // insert it into the grid

                if (cand[r][c].size() == 1) {
                    grid[r][c] = *cand[r][c].begin();
                    updateCand(r, c, grid[r][c]); // Update the affected cells
                    found = true; // Signal that we've found a naked single so
                                  // we can do a re-check after
                }
            }
        }
    } while (found);
}

bool setHiddenSingles() {
    /*
        This function is used to check for hidden singles.
        In essence, hidden singles are when in a row, col or box,
        there's only 1 cell with a certain value as a candidate.
        This means that that cell has to be that value,
        otherwise no other cell can possibly hold that value
    */

    int Ind, count;
    bool found;

    do {
        found = false; // Used to reset the check

        for (int row = 0; row < GRID_SIZE; row++) {

            // We check each value for a possible hidden single
            for (int value = 1; value <= GRID_SIZE; value++) {
                count = 0; // This will keep track of how many times a cell has
                           // a value as a candidate

                for (int col = 0; count < 2 && col < GRID_SIZE; col++) {
                    /*
                        We check each column unless we've already found
                        the value as a candidate in more than 1 cell,
                        then there's no possibility that this value is a hidden
                       single
                    */

                    // If a cell is already filled, we check
                    // it's value before skipping
                    if (grid[row][col]) {
                        if (grid[row][col] == value)
                            count = 2;

                        /*
                            If this cell matches the value we're looking for,
                            then we stop searching for this value as a hidden
                           single.
                        */

                        continue;
                    }

                    set<int> &nums = cand[row][col]; // Used for easy reference

                    if (nums.find(value) != nums.end()) {
                        /*
                            If we find this value we're looking for as a
                           candidate, we can increment our counter and store
                           it's column index incase this is the only 1 instance
                           we find it
                        */
                        count++;
                        Ind = col;
                    }
                }

                /*
                    If the counter is just 1, that means we
                    found this value as a candidate in only one cell.
                    Therefore, it is a hidden single
                */
                if (count == 1) {
                    updateCand(row, Ind, value);
                    setNakedSingles();
                    /*
                        Since we've found a hidden singles,
                        it could possibly generate new naked singles.
                        So we can call the naked singles function to insert
                       these.
                    */
                    found = true;
                } else if (!count)
                    return false;
                /*
                    If we didn't find it at all, then the board is in an invalid
                    state. So we return false, to tell the solver we need to
                    backtrack
                */
            }
        }

        for (int col = 0; col < GRID_SIZE;
             col++) // Same method as above but for columns instead
        {
            for (int value = 1; value <= GRID_SIZE; value++) {
                count = 0;

                for (int row = 0; count < 2 && row < GRID_SIZE; row++) {
                    if (grid[row][col]) {
                        if (grid[row][col] == value)
                            count = 2;

                        continue;
                    }

                    set<int> &nums = cand[row][col];

                    if (nums.find(value) != nums.end()) {
                        count++;
                        Ind = row;
                    }
                }

                if (count == 1) {
                    updateCand(Ind, col, value);
                    setNakedSingles();
                    found = true;
                } else if (!count)
                    return false;
            }
        }

        int rowS, colS;
        pair<int, int> point;

        // The hidden singles checks each box in the grid
        for (int boxNo = 0; boxNo < GRID_SIZE; boxNo++) {
            rowS = (boxNo / boxSize) * boxSize,
            colS = (boxNo % boxSize) * boxSize;

            for (int value = 1; value <= GRID_SIZE; value++) {
                count = 0;

                for (int r = rowS; count < 2 && r < boxSize + rowS; r++) {
                    for (int c = colS; count < 2 && c < boxSize + colS; c++) {
                        if (grid[r][c]) {
                            if (grid[r][c] == value)
                                count = 2;

                            continue;
                        }

                        set<int> &nums = cand[r][c];

                        if (nums.find(value) != nums.end()) {
                            count++;
                            point = {r, c};
                        }
                    }
                }

                if (count == 1) {
                    updateCand(point.first, point.second, value);
                    setNakedSingles();
                    found = true;
                } else if (!count)
                    return false;
            }
        }
    } while (found); // We continue these sweeps until there's no more
                     // hidden/naked singles

    return true;
}

void genCand() {
    cand = vector<vector<set<int>>>(
        GRID_SIZE,
        vector<set<int>>(
            GRID_SIZE,
            set<int>())); // Setting candidates size based on the grid size

    for (int r = 0; r < GRID_SIZE; r++) {
        for (int c = 0; c < GRID_SIZE; c++) {
            set<int> &num = cand[r][c]; // The reference is used to make it
                                        // easier to access cand[r][c]

            if (grid[r][c]) // If the cell is already filled there's no need to
                            // search for candidates so we skip
                continue;

            for (int val = 1; val <= GRID_SIZE; val++) {
                if (isSafe(r, c,
                           val)) // If a certain value is safe, we can insert it
                                 // into this candidate's set of possible values
                    num.insert(val);
            }
        }
    }
}

bool solve() {
    int r, c;

    if (!setHiddenSingles())
        return false;

    /*
        If the hidden singles function returns false,
        then we know that the board is in an invalid state,
        so we can then backtrack.
    */

    if (!getEmptyPos(r, c))
        return true;

    /*
        If the getEmptyPos function returns false,
        it means there's no cell left to fill and that means
        that we're done solving so we can return true to signal
        a successful solve.
    */

    vector<vector<set<int>>> copyCand = cand;

    for (int val : copyCand[r][c]) {
        updateCand(r, c, val); // We will take the current value as our guess
                               // and so we update affected cells too

        if (!solve()) // Calls the solve and checks if we've solved or not
        {
            /*
                If we've not solved, then we need to backtrack.
                So we undo our moves until we get to our current coordinate
                on our stack
            */

            pair<int, int> &curr = moves.top();

            while (curr != pair<int, int>{r, c}) {
                grid[curr.first][curr.second] =
                    0;              // Setting our past move to 0 on the board
                moves.pop();        // Removing that move from the stack
                curr = moves.top(); // Setting curr to the new top of the stack
            }

            cand = copyCand;
            /*
                Since the update candidates function updates the possible values
               in the candidates matrix, we need to store and revert the
               candidates matrix before this move, so this is why we need a copy
               of the candidates matrix before each "guess."
            */
        } else
            return true;
    }

    return false;
}

int main() {
    string inp;

    /*
        This while loop will stop when it hits the EOF marker, this is similar
        to when reading from a textfile The EOF marker can be also signalled if
        there is manual input with Ctrl + D or Ctrl + Z or Ctrl + Z + Enter. If
        this does not work you should modify the input to read the grid you
        would like to capture.
    */

    // Getting input and transforming it to a grid of integers
    while (getline(cin, inp)) {
        stringstream line(inp);
        vector<int> numbers;
        string curr;

        while (getline(line, curr, ' ')) {
            if (int(curr[0]) >= 65 && int(curr[0]) <= 90)
                numbers.push_back(curr[0] - 55);
            else
                numbers.push_back(stoi(curr));
        }

        grid.push_back(numbers);
    }

    GRID_SIZE = grid.size(); // Setting dimensions for the grid
    boxSize = sqrt(GRID_SIZE);

    auto start = chrono::high_resolution_clock::now(); // Start the timer

    genCand(); // Will generate the possible candidates for each square

    if (!solve()) // Solved call in the if
        cout << "No Solution" << endl;
    else
        Display();

    auto end = chrono::high_resolution_clock::now(); // Used to time the solve
    cout << "Duration "
         << chrono::duration_cast<chrono::milliseconds>(end - start).count()
         << " ms" << endl; // Displays time taken to solve

    return 0;
}