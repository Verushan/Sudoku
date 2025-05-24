These 2 Sudoku solvers are based on two methods namely:
1) Hidden/Naked Singles
2) Dancing Links

The hidden singles method relys on a domain specific optimization to a brute force search. Whilst Dancing Links, relies on solving the more general problem of constraint satisfaction. Both of these approachs seemed rather interesting so I decided to implement both!

# Hidden Singles
There are a few key observations in Sudoku that allow us to narrow down the amount of searching and possible candidates that we allow for a cell.

## Ordering the exploration
A key optimization is to "guess" values for cells that have fewer possibilities. This massively narrows down the possibilities if we keep this approach every time we want to explore the next cell to complete the Sudoku quicker and reduce backtracking.

## Usage of sets
Each row, column and 3 x 3 box (in the case of a 9 x 9 grid) has it's own set keeping track of the values present within it. This makes it quicker to query for conflicts and these sets are updated accordingly when we place a value in a cell or remove them.

## Naked Single
A naked single is when a cell has only one possible value (Any other value would result in a violation of the Sudoku constraints). In this case, we can simply place this value in the cell without thinking there's any other possibility.

It is also possible that placing naked singles reduces other cell's candidates to the point that they also become a naked single. So in order to further reduce the search, we constantly search the grid for naked singles until none are found. This may seem computationly intensive, but the amount of searching it reduces outweight this cost.

## Hidden pairs
These are when there are two cells that share their last two candidates. E.g. Values {2, 4} are possible for two cells in a row.
Checking which of these two values are possible for one of those two cells gives us the remaining value for the last cell.

# Dancing Links
Dancing Links is such a beautiful algorithm. This algorithm aims to solve the general case of a constraint satisfactory problem. This works by creating a circular, doubly linked list representing a grid, each row in this grid represents a constraints. Constraints that depend on each other are linked.

The goal is somewhat changed when looking at the matrix, we would now like to choose rows such that when put together, they satisfy all columns (Each column is a specific constraints). We don't want any of these columns being satisfied more than once.

This table is quite taxing in terms of memory to generate as there are many possibilities. However, once generated, it is very efficient to search through especially when removing, re-adding constraints during the search.

# Implementation
Both these codes work for any grid sizes 4x4 till 25x25. To work with larger grids, you'd need to write your own method to attach values to certain characters.

# Examples
The structure of the input can be found in the grids folder which can be piped when running either one of the solvers.

# Final thoughts
In the end, the Hidden Singles method performed the best. It makes sense considering it is specifically optimized for Sudoku. The Dancing Links method didn't lag behind significantly though! It really shows the potential for such an algorithm since it's a general solution for constraint satisfactory problems as a whole!

I hope my code and comments helps broaden your understanding on these two methods of solving Sudoku puzzles. Enjoy!

Credit for Dancing Links goes out to Donald E. Knuth for finding this beautiful method.
Thesis for Dancing Links: https://www.kth.se/social/files/58861771f276547fe1dbf8d1/HLaestanderMHarrysson_dkand14.pdf
