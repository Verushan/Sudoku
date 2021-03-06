#include <iostream>
#include <vector>
#include <sstream>
#include <forward_list>
#include <cmath>
#include <chrono>

using namespace std;

/*
    RECOMMENDED INSIGHT

    A great explanation of Dancing Links:
    https://garethrees.org/2007/06/10/zendoku-generation/#section-4

    I suggest you give this a read before proceeding to understand the code
*/

int gridSize = 0; // Used to store the grid size

vector<vector<int>> grid;
forward_list<int> solution; // This stores the values to be inserted into the grid

bool isSolved = false; // Used to check if there's no solution

class Sudoku
{
private:
    class header; // Declared ahead of time so it can be used in node class
    class node
    {
    public:
        int row;
        header *head;
        node *up, *down, *left, *right;
    };

    class header : public node
    {
    public:
        int count; // How many nodes in this header
        header *left, *right;
    };
    //Above is the required structure for the header and node for Dancing Links

    int n, numRows, numCols;
    header *rootNode; //

    vector<vector<node>> rows;
    vector<header> cols;

public:
    Sudoku()
    {
        n = gridSize;              // Dimension of the grid
        numCols = (n * n * 4) + 1; // The +1 is to cater for the root node
        numRows = n * n * n;       // Dimensions

        rows = vector<vector<node>>(numRows, vector<node>(4, node())); // Generate rows
        cols = vector<header>(numCols, header());                      //Generate columns

        rootNode = &(cols[numCols - 1]); // The rootnode just represents a gateway to the other columns nodes

        for (int i = 0; i < numCols; i++) // Set column nodes
        {
            //This part links all column nodes to appropriate right and left pointers.

            cols[i].up = &cols[i];
            cols[i].down = &cols[i];
            cols[i].left = &(cols[(i + numCols - 1) % numCols]);
            cols[i].right = &(cols[(i + 1) % numCols]);
            cols[i].count = 0; // We set to 0 since no nodes belong to this column as yet
        }

        int n2 = n * n;         // This is used just to make getting n^2 easier
        int boxWidth = sqrt(n); // Size of square block

        /*
            VISUALIZATION FOR CONSTRAINT MATRIX
            The code below generates the constraint matrix
            A visualization of this can be seen at:
            https://www.stolaf.edu/people/hansonr/sudoku/exactcovermatrix.htm
        */

        for (int r = 0; r < numRows; r++) // Generating constraint matrix
        {
            int colInd, currRow = r / (n * n), currCol = (r / n) % n, offset = r % n;
            /*
                The above calculations are used to get the respective row, and column
                based on the row that's from 1 till n * n * n
            */

            /*
                The if statement below, checks if the cell we're generating a constraint for
                is already filled in, if it is, we also check that the possible value for that
                cell matches the value property for the particular constraint we're generating.
            */

            if (grid[currRow][currCol] != 0 && grid[currRow][currCol] != offset + 1) // Must only have 1 constriant for givens
                continue;

            for (int c = 0; c < 4; c++) // Each row has 4 constraints
            {
                int colStart = c * n2; // Used to keep track of which set of columns we're choosing a constraint from

                switch (c)
                {
                case 0: // Value constraints
                    colInd = r / n;
                    break;
                case 1: // Row contraints
                    colInd = n2 + (currRow * n) + offset;
                    break;
                case 2: // Col Constraints
                    colInd = colStart + ((r / n) % n) * n + offset;
                    break;
                case 3: // Box Constraint
                    colInd = colStart + (boxWidth * (currRow / boxWidth) + (currCol / boxWidth)) * n + offset;
                    break;

                    // Each one of these calculations generate the correct node being chosed for this specific constraint
                }

                //Doubly linked vertically and horizontally cells

                rows[r][c].row = r;
                rows[r][c].down = &cols[colInd]; // The latest node's down pointer will point to it's header

                //The up, left, right and down pointers are set similarly to the way the headers were linked
                rows[r][c].right = &(rows[r][(c + 1) % 4]);
                rows[r][c].left = &(rows[r][(c + 3) % 4]);
                rows[r][c].up = cols[colInd].up;   // The header's previous up pointer will be this nodes up pointer
                rows[r][c].head = &cols[colInd];   // Every node contains which header they belong to
                rows[r][c].up->down = &rows[r][c]; // Make the node above us point to us as their down pointer
                rows[r][c].down = &cols[colInd];   // This node's down pointer will point the column header it belongs to

                cols[colInd].up->down = &(rows[r][c]); // Linking the appropriate column to the node
                cols[colInd].up = &(rows[r][c]);
                cols[colInd].count++; // Since we've added a node to this column, we increment it's node count
            }
        }
    };

    void print()
    {
        int currRow, currCol, offset;

        cout << endl;

        for (int i = 0; i < 2 * n; i++)
            cout << '-';

        cout << endl;

        for (int &r : solution) // Setting values in grid
        {
            // This gets the appropriate row and column based on the row value of the constraint matrix.
            currRow = r / (n * n);
            currCol = (r / n) % n;
            offset = r % n + 1;

            grid[currRow][currCol] = offset; // Setting the value in the actual grid
        }

        for (int i = 0; i < n; i++) // Printing out the grid
        {
            for (int j = 0; j < n; j++)
            {
                if (grid[i][j] >= 10)
                    cout << char(grid[i][j] + 55);
                else
                    cout << grid[i][j];

                if (j != n - 1)
                    cout << " ";
            }

            cout << endl;
        }
    }

    void solve()
    {
        if (rootNode->right == rootNode) // Means we've found a solution
        {
            isSolved = true;
            print();
            return;
        }

        header *bestCol = rootNode->right; // We set the bestCol to the first column to our rootNode's right first

        for (header *c = rootNode->right; c != rootNode; c = c->right) // Choosing best column
        {
            if (c->count < bestCol->count) // Searches for the best column
                bestCol = c;

            /*
                This is known as a heuristic approach since 
                choosing to cover the node with the least 
                amount of candidates reduces the branching factor of our algorithm
            */
        }

        cover(bestCol); // We then cover this column

        for (node *currRow = bestCol->down; currRow != bestCol; currRow = currRow->down)
        {
            /*
                For each node in the same column as our chosen column, 
                we try building the rest of the solution by trying to cover then next
                column and so on until there's no more columns to cover. 
            */

            solution.push_front(currRow->row); // Add this node to the solution set

            for (node *currCol = currRow->right; currCol != currRow; currCol = currCol->right) // Remove conflicting columns
                cover(currCol->head);                                                          // Cover all columns that this node constraint has

            solve();              // Continue solving the reduced constraint matrix and try to cover all columns.
            solution.pop_front(); // If we fail, we remove the last added row value to the solution set and we relink our previous unlinked nodes

            for (node *currCol = currRow->left; currCol != currRow; currCol = currCol->left) // Recover removed columns
                uncover(currCol->head);
        }

        uncover(bestCol);
    }

private:
    void cover(header *col)
    {
        col->right->left = col->left;  //Make our left and right columns point to each other
        col->left->right = col->right; // In essence "deleting" col from the set of columns

        for (node *r = col->down; r != col; r = r->down) // For each node contained in col
        {
            for (node *c = r->right; c != r; c = c->right) // For each node to the right of this node
            {
                c->up->down = c->down; // Make the node above and below it, point to each other
                c->down->up = c->up;
                c->head->count--; // Decrement the count for the columns that have been affected by these node removals
            }
        }
    }

    void uncover(header *col)
    {
        /*
            Since we covered columns by deleting the column and 
            then the affected nodes, we do this operation in reverse.
            Readding this column nodes first and then readding the column
        */
        for (node *r = col->up; r != col; r = r->up) // Since we covered down to to up, we uncover up to down
        {
            for (node *c = r->left; c != r; c = c->left) // Since we covered right to left, we uncover left to right
            {
                c->up->down = c; // Make this cell's up and down nodes point back to this cell
                c->down->up = c;
                c->head->count++; // Increment the node count since we're readding this node
            }
        }

        col->right->left = col; // Now we uncover the column
        col->left->right = col;
    }
};

int main()
{
    string inp;

    /*
        This while loop will stop when it hits the EOF marker, this is similar to when reading from a textfile
        The EOF marker can be also signalled if there is manual input with Ctrl + D or Ctrl + Z or Ctrl + Z + Enter.
        If this does not work you should modify the input to read the grid you would like to capture.
    */

    while (getline(cin, inp))
    {
        stringstream currStr(inp);
        vector<int> numbers;
        string curr;

        while (getline(currStr, curr, ' '))
            if (int(curr[0]) >= 65 && int(curr[0]) <= 90)
                numbers.push_back(curr[0] - 55);
            else
                numbers.push_back(stoi(curr));

        grid.push_back(numbers);
    }

    gridSize = grid.size(); // Setting dimensions for the grid

    auto start = chrono::high_resolution_clock::now(); // Start the timer

    Sudoku sudGrid;
    sudGrid.solve();

    if (!isSolved)
        cout << "No Solution" << endl;

    auto end = chrono::high_resolution_clock::now();
    cout << "Duration " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl; // Displays time taken to solve

    return 0;
}
