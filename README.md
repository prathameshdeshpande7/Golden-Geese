The Golden Geese

In this game, you control a robot that can travel in a rectangular grid consisting of MxN cells. In some of the cells, there are geese who lay golden eggs. Your robot has to go and collect the golden eggs. Each golden goose has a quality score from 1 to 9. Whenever you collect an egg from a goose, you get as many points as the quality score of that goose. The goal is to collect the maximum points in a fixed number of moves. (Note: in most cases, it will not be practical to implement an optimal algorithm for this, so you'll be expected to implement a heuristic algorithm that works well in most cases.)

Rules of the Game
These are the rules concerning the geese:

  - There can't be more than one goose in any cell
  - Only some cells contain geese. Other cells are empty
  - There can't be more than one egg in any cell with a goose (and of course, there will never be any eggs in cells that don't contain geese)
  - A goose stays in one cell throughout the game. Geese don't move
  - At the start of the game all geese have laid one egg.
  - When your robot visits the cell containing a goose, if that goose has laid an egg, the egg is automatically picked up by your robot.
  - Once the egg of a goose is collected, it waits for 10 of your robot's moves, and then lays another egg of the same value. It now waits until that egg is collected and the process repeats. The egg will appear on exactly the 10th move after it was last collected.
  - It is a mistake to try to collect an egg from a goose in less than 10 moves. If you do so, its egg timer gets reset to 0, and it will lay the next egg 10 movies after your failed collection attempt.
  - The quality score of a goose stays constant during the entire game; so the points you get for collecting the eggs from a particular goose always stay the same
  
Corner Cases
Here are some edge cases concerning the robot's behavior:

  - On each move your robot can take only one step in one of these directions: up, down, left right.
  - A robot must move on each step, it cannot stay in the same place
  - Reaching a cell with a goose results in automatic collection of the egg there. If that goose has not yet laid an egg (i.e. the last collection happened less than 10 moves ago), then then reaching that goose's cell results in resetting the goose's egg timer.
  - Reaching a goose cell on exactly the 10th move after having visited that cell will result in the collection of the new egg (which was laid exactly when your robot reached that cell)
  - Trying to move past the edge of the grid is an error and results in immediately losing the game
  - The robot gets a total of 2MN moves, after which the game ends.

At the start of the game, the robot is in cell (0, 0), and the number of rows (M), the number of columns (N), and number of geese is known to you. But the positions of the geese are not known.
