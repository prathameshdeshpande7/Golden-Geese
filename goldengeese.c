#include <stdio.h>
#include <stdlib.h>

int geese_found_count = 0;
/************************************************************

For this contest, you have to modify the code of the robot_xxx functions

Specifically, you have to implement 3 functions:

  robot_initialize
    which takes a struct Robot *robot, a struct Grid *grid, and uses
    the grid the initialize the robot. 

  robot_move
    which returns the next move of the robot. 
    It should return exactly one of these values
    Grid.LEFT, Grid.RIGHT, Grid.UP, Grid.DOWN

  robot_got_egg
    which takes a struct Robot *, x, y, value 
    
    After robot_move, if your robot lands up in
    a cell containing a goose, and the goose has laid an egg
    then robot_got_egg is called with the co-ordinates of the goose
    and the value of the egg

Overall flow of the code:

The system will call the methods of the robot class in this order:

  struct Robot robot;
  robot_initialize(&robot, &grid);

  then it will call robot_move(&robot) many times (2*grid.rows*grid.cols times)

  if, after any move your robot lands up in a cell with
  a goose, then the system will immediately call
  robot_got_egg(&robot, value)

Feel free to add other structs and data-structures as you feel
necessary. You can also modify the contents of the Robot struct.

However, do not modify any of the code in the `Grid`
or `Evaluator` structs or the corresponding functions. 
And do not read/write any data to or from any files.

************************************************************/

/**************** Declarations Begin: DO NOT MODIFY **************/

struct Grid {
  /* DO NOT MODIFY THIS STRUCT */
  int RIGHT;
  int LEFT;
  int UP;
  int DOWN;
  int rows;
  int cols;
  int num_geese;
};

void grid_init(struct Grid *grid, int rows, int cols, int num_geese);
int grid_is_valid_location(struct Grid *grid, int x, int y);
void grid_new_location(struct Grid *grid, int x, int y, int move_direction,
                       int *new_x, int *new_y);

/**************** Declarations End: **************/


// You can modify anything in this part of the code as necessary
// You can add more classes, functions, data structures

#define INFINITY 9999
struct Goose {
  int goose_x;
  int goose_y;
  int goose_quality;
  int goose_visited_no;
};

enum RobotMode {
    ROBOT_SCAN = 1,
    ROBOT_CALC_MH_DIST,
    ROBOT_TRAVERSE,
    ROBOT_REROUTING,
};

struct Robot {
  struct Grid *grid;
  int x;
  int y;
  int horiz_direction;
  int vert_direction;
  int count_move_no;
  enum RobotMode curr_state;
  struct Goose *goose_info;
  int target_egg_index;
  int next_egg_alive_index;
  int next_egg_alive_in;
  int step_count_x;
  int step_count_y;
  // Feel free to modify this struct as required
};


void robot_initialize(struct Robot *robot, struct Grid *grid) {
  /*
   * grid already exists and is initalized.
   * robot has been allocated, but not initialized.
   * grid->rows is the number of cells in the left-right direction
   * grid->cols is the number of cells in the up-down direction
   * grid->num_geese is the number of geese
   *   
   * Feel free to modify this method as required
   */
  robot->grid = grid;
  robot->x = 0;                 // current x position of robot
  robot->y = 0;                  // current y position of robot
  robot->horiz_direction = grid->RIGHT;
  robot->vert_direction = grid->UP;
  robot->count_move_no = 0;
  robot->curr_state = ROBOT_SCAN;
  robot->goose_info = malloc(robot->grid->num_geese*sizeof(struct Goose));
  robot->target_egg_index = 0;
  robot->next_egg_alive_index = 0;
  robot->next_egg_alive_in = 0;
  robot->step_count_x = 0;
  robot->step_count_y = 0;
}

int get_diff_x(int curr_x, int goose_x) {
    return curr_x - goose_x;
}

int get_diff_y(int curr_y, int goose_y) {
    return curr_y - goose_y;
}

int goose_exists(struct Robot *robot, int newx, int newy) {
    int i;

    for (i = 0; i < robot->grid->num_geese; i++) {
        if ((newx == robot->goose_info[i].goose_x &&
             newy == robot->goose_info[i].goose_y) &&
             robot->next_egg_alive_in > 1) {
            // egg exists at this location
            return 1;
        }
    }
    return 0;
}

int validate_move_location(struct Robot *robot, int move_direction) {

    int newx, newy;
    grid_new_location(robot->grid, robot->x, robot->y,
                      move_direction, &newx, &newy);
    if (grid_is_valid_location(robot->grid, newx, newy)) {
        if (robot->curr_state == ROBOT_REROUTING &&
            goose_exists(robot, newx, newy)) {
            return -1;
        }
        robot->x = newx;
        robot->y = newy;
        robot->count_move_no++;
        return move_direction;
    }
    return -1;
}

int robot_reroute(struct Robot *robot) {
    int valid_move = -1;

    if (robot->step_count_x) {
        valid_move = validate_move_location(robot, robot->horiz_direction);
        if (valid_move != -1) {
            robot->step_count_x--;
            printf("TRAVERSE HORIZ %d, step_count_x %d\n", robot->horiz_direction, robot->step_count_x);
            return robot->horiz_direction;
        } else {
            // reverse the horiz_direction
            robot->horiz_direction = robot->horiz_direction == robot->grid->RIGHT ?
              robot->grid->LEFT : robot->grid->RIGHT;
            printf("Traverse candidate hori %d, vert %d\n", robot->horiz_direction, robot->vert_direction);
            robot->step_count_x--;
            return robot->horiz_direction;
        }
    }

}

int robot_traverse(struct Robot *robot) {

    int valid_move = -1;

    if (robot->step_count_x) {
        valid_move = validate_move_location(robot, robot->horiz_direction);
        if (valid_move != -1) {
            robot->step_count_x--;
            printf("TRAVERSE HORIZ %d, step_count_x %d\n", robot->horiz_direction, robot->step_count_x);
            if (robot->curr_state == ROBOT_REROUTING) {
                robot->curr_state = ROBOT_CALC_MH_DIST;
            }
            return robot->horiz_direction;
        } else {
            // reverse the horiz_direction
            robot->horiz_direction = robot->horiz_direction == robot->grid->RIGHT ?
              robot->grid->LEFT : robot->grid->RIGHT;
            valid_move = validate_move_location(robot, robot->horiz_direction);
            if (valid_move != -1) {
                printf("Traverse candidate hori %d, vert %d\n", robot->horiz_direction, robot->vert_direction);
                if (robot->curr_state == ROBOT_REROUTING) {
                    robot->curr_state = ROBOT_CALC_MH_DIST;
                }
                robot->step_count_x--;
                return robot->horiz_direction;
            }
        }
    }

    if (robot->step_count_y) {
        valid_move = validate_move_location(robot, robot->vert_direction);
        if (valid_move != -1) {
            robot->step_count_y--;
            printf("TRAVERSE VERT %d, step_count_y %d\n", robot->vert_direction, robot->step_count_y);
            if (robot->curr_state == ROBOT_REROUTING) {
                robot->curr_state = ROBOT_CALC_MH_DIST;
            }
            return robot->vert_direction;
        } else {
            // reverse the vert_direction
            robot->vert_direction = robot->vert_direction == robot->grid->UP ?
              robot->grid->DOWN : robot->grid->UP;
            valid_move = validate_move_location(robot, robot->vert_direction);
            if (valid_move != -1) {
                robot->step_count_y--;
                printf("2 Traverse candidate hori %d, vert %d\n", robot->horiz_direction, robot->vert_direction);
                if (robot->curr_state == ROBOT_REROUTING) {
                    robot->curr_state = ROBOT_CALC_MH_DIST;
                }
                return robot->vert_direction;
            }
        }
    }
}

/*
 * Calculate minimum manhattan distance from robot's
 * current location to all goose locations which
 * are having a positive egg value(not reset to 0)
 */
void robot_calc_mh_dist(struct Robot *robot) {
    int min = INFINITY, i, mh_dist;
    int lay_egg_timer = INFINITY, lay_egg_dist;
    int robot_goose_move_diff = 0;
    int diff_x, diff_y;

    printf("HERE ********\n");
    for (i = 0; i < robot->grid->num_geese; i++) {
        // Calculate robot's position to this goose visited move number
        robot_goose_move_diff = robot->count_move_no -
                                robot->goose_info[i].goose_visited_no;

        diff_x = get_diff_x(robot->x,
                            robot->goose_info[i].goose_x);

        diff_y = get_diff_y(robot->y,
                            robot->goose_info[i].goose_y);

        // Calculate manhattan distance from robot to this goose
        mh_dist = abs(diff_x) + abs(diff_y);

        if (robot_goose_move_diff + mh_dist > 10) {
            printf("Greater than 10 rgmd %d, mh_dist %d\n", robot_goose_move_diff, mh_dist);
            if (mh_dist < min) {
                min = mh_dist;
                // Save the Goose index and steps in
                // x and y direction to go to for later traversal
                robot->target_egg_index = i;

                robot->horiz_direction = (diff_x < 0) ?
                        robot->grid->RIGHT : robot->grid->LEFT;
                robot->step_count_x = abs(diff_x);

                robot->vert_direction = (diff_y < 0) ?
                        robot->grid->UP: robot->grid->DOWN;
                robot->step_count_y = abs(diff_y);

                // Found egg to move to.
                // Traverse robot to that location
                robot->curr_state = ROBOT_TRAVERSE;
            }
        } else if (robot_goose_move_diff + mh_dist <= 10) {
            // This goose will lay egg in how much steps?
            // Save the lowest lay egg count and reach there
            // immediately when it arrives.
            // This is the case when all eggs are reset.
            lay_egg_dist = 10 - robot_goose_move_diff;
            if (lay_egg_dist < lay_egg_timer) {
                lay_egg_timer = lay_egg_dist;
                robot->next_egg_alive_index = i;
                robot->next_egg_alive_in = lay_egg_timer;

                robot->horiz_direction = (diff_x < 0) ?
                        robot->grid->RIGHT : robot->grid->LEFT;
                robot->step_count_x = abs(diff_x);

                robot->vert_direction = (diff_y < 0) ?
                        robot->grid->UP: robot->grid->DOWN;
                robot->step_count_y = abs(diff_y);
            }    
            // No eggs live yet. Reroute robot till something
            // comes up.
            printf("Reroute robot till something: rgmd %d, moveno %d step %d, idx %d, alive in %d\n", robot_goose_move_diff, robot->count_move_no, robot->next_egg_alive_in, robot->next_egg_alive_index, robot->next_egg_alive_in);
            robot->curr_state = ROBOT_REROUTING;
            printf("ROBOT X: %d, Y: %d\n", robot->x, robot->y);
        }
    }
}

int robot_scan_grid(struct Robot *robot) {
    /* 
     * This function scans the entire grid
     */
    int valid_move = -1;
    int ret = 0;

    if (robot->grid->num_geese == 0) {
        robot->step_count_x = robot->grid->rows;
        robot->step_count_y = robot->grid->cols;
        ret = robot_traverse(robot);
        return ret;
    } else {
        if (geese_found_count == robot->grid->num_geese) {
            /* We have found all the geese in the scan.
            * Now, stop scanning and start calculating
            * Manhattan Distance of the current location
            * to all Geese locations.
            */
            robot->curr_state = ROBOT_CALC_MH_DIST;
            robot_calc_mh_dist(robot);

    #if 0
            if (robot->curr_state == ROBOT_TRAVERSE) {
                ret = robot_traverse(robot);
            } else if (robot->curr_state == ROBOT_REROUTING) {
                ret = robot_reroute(robot);
            }
    #endif
            ret = robot_traverse(robot);
            printf("Found ALL :%d %d\n", geese_found_count, robot->count_move_no);
            return ret;

        } else {
            /* scan entire grid once, go RIGHT till reached edge,
             * then one step UP, then reverse direction,
             * go LEFT till reached edge, go UP, reverse direction
             * and so on till we complete MN moves
             */  
            valid_move = validate_move_location(robot, robot->horiz_direction);
            if (valid_move != -1) {
                // continue moving in the current horiz_direction
                return robot->horiz_direction;
            } else {
                // we have reached an edge.
                // reverse the horiz_direction
                robot->horiz_direction = robot->horiz_direction == robot->grid->RIGHT ?
                  robot->grid->LEFT : robot->grid->RIGHT;

                // go UP one step
                valid_move = validate_move_location(robot, robot->vert_direction);
                if (valid_move != -1) {
                    return robot->vert_direction;
                }
            }
        }
    }
}

int robot_move(struct Robot *robot) {
  /*
   * Where should the robot move next
   *
   * Must return one of grid->LEFT, grid->RIGHT, grid->UP, grid->DOWN
   *
   * It is an error to return a direction that causes
   * the robot to move off the grid.
   *
   * The current implementation is a simple 
   * left-right-left up-down-up traversal of the entire grid
   * 
   * Please modify this method so that it does something
   * more intelligent.
   * 
   * Note: in this function, do not access any data of the
   * Evaluator struct. At the beginning, your robot does not know
   * anything other than grid->rows, grid->cols, grid->num_geese
   * grid->LEFT, grid->RIGHT, grid->UP, grid->DOWN
   * 
   * Specifically, all the locations and values of geese need to be
   * automatically deduced by your robot based on when robot_got_egg is called
   */
    int ret, index;
    switch (robot->curr_state) {
        case ROBOT_SCAN:
            ret = robot_scan_grid(robot);
            printf("SCAN ret %d\n", ret);
            break;
        case ROBOT_CALC_MH_DIST:
            robot_calc_mh_dist(robot);
            ret = robot_traverse(robot);
            break;
#if 0
        case ROBOT_TRAVERSE:
            if (step_count_x == 0 && step_count_y == 0) {
                robot->curr_state = ROBOT_CALC_MH_DIST;
                ret = -1;
            } else {
                ret = robot_traverse(robot);
                printf ("XOOO %d\n", ret);
            }
            break;
        case ROBOT_REROUTING:
            ret = robot_reroute(robot);
            break;
#endif
    }
    return ret;
}

void robot_got_egg(struct Robot *robot, int x, int y, int value) {
  /* 
   * An egg of `value` has been found at location `x, y` 
   *
   * Currently, this function does nothing. 
   * You probably want to modify this function so that it remembers 
   * the location and value of this golden goose. If this is a 
   * high value goose, you might consider returning here; but make
   * sure to not return before 10 steps.
   */

   /* Save the goose information in Goose structure
    * x, y, value and the overall move number we took
    * to reach this goose
    */
   robot->goose_info[geese_found_count].goose_x = x;
   robot->goose_info[geese_found_count].goose_y = y;
   robot->goose_info[geese_found_count].goose_quality = value;
   robot->goose_info[geese_found_count].goose_visited_no = robot->count_move_no;
   printf("GOOSE #%d: x: %d, y: %d, quality: %d, visited_move: %d\n", geese_found_count+1,
        robot->goose_info[geese_found_count].goose_x,
        robot->goose_info[geese_found_count].goose_y,
        robot->goose_info[geese_found_count].goose_quality,
        robot->goose_info[geese_found_count].goose_visited_no);
   geese_found_count++;
   printf("%d, %d: [%d]\n",
            x, y, value);
}

/***************************************************************

DO NOT MODIFY ANYTHING AFTER THIS LINE

The rest of this file contains evaluation code. You don't need to
understand the code, and it is not even necessary to read the code.
You are not allowed to change anything here
code, nor are you allowed to access any of these
variables/data-members here.

All you need to know is that this code will instantiate
the Robot class, `initialize` it, and call the `move` and `got_egg`
methods in a loop.

Any modification of code after this line, will result in disqualification
of your entry 

***************************************************************/

void grid_init(struct Grid *grid, int rows, int cols, int num_geese) {
  grid->RIGHT = 1;
  grid->LEFT = 2;
  grid->UP = 3;
  grid->DOWN = 4;
  grid->rows = rows;
  grid->cols = cols;
  grid->num_geese = num_geese;
}
  
int grid_is_valid_location(struct Grid *grid, int x, int y) {
  /* Helper function to check if x,y is a valid location in the grid */
  return 0 <= x && x < grid->rows && 0 <= y && y < grid->cols;
}

void grid_new_location(struct Grid *grid, int x, int y, int move_direction,
                       int *new_x, int *new_y) {
  /* Helper function to determine new co-ordinates after move

     If robot is at `x,y` and moves in `move_direction`, what will be
     the new_x and new_y. This does not do any boundary value checking.
  */
  if (move_direction == grid->LEFT) {
    *new_x = x-1;
    *new_y = y;
  } else if (move_direction == grid->RIGHT) {
    *new_x = x+1;
    *new_y = y;
  } else if (move_direction == grid->UP) {
    *new_x = x;
    *new_y = y+1;
  } else if (move_direction == grid->DOWN) {
    *new_x = x;
    *new_y = y-1;
  }
}

struct Evaluator {
  int **geese;
  int **last_visited;
  int score;
  int move_no;
  int robot_x;
  int robot_y;
  struct Robot robot;
  struct Grid grid;
};

void evaluator_read_input(struct Evaluator *evaluator, char *input_file) {
  FILE *fin = fopen("input.txt", "r");
  int rows, cols, num_geese;
  int i, j;
  fscanf(fin, "%d %d %d", &rows, &cols, &num_geese);
  grid_init(&evaluator->grid, rows, cols, num_geese);
  evaluator->geese = (int **)malloc(rows*sizeof(int*));
  for(i=0; i<rows; i++) {
    evaluator->geese[i] = (int *)malloc(cols*sizeof(int));
    for(j=0; j<cols; j++) {
      fscanf(fin, "%d", &(evaluator->geese[i][j]));
    }
  }
}

void evaluator_init_robot(struct Evaluator *evaluator) {
  int i, j;
  evaluator->move_no = 0;
  evaluator->score = 0;
  evaluator->robot_x = 0;
  evaluator->robot_y = 0;
  
  evaluator->last_visited = (int **)malloc(evaluator->grid.rows*sizeof(int *));
  for(i=0; i<evaluator->grid.rows; i++) {
    evaluator->last_visited[i] = (int *)malloc(evaluator->grid.cols*sizeof(int));
    for(j=0; j<evaluator->grid.cols; j++) {
      evaluator->last_visited[i][j] = -1;
    }
  }
  robot_initialize(&evaluator->robot, &evaluator->grid);
}

void evaluator_process_current_cell(struct Evaluator *evaluator) {
  /* check if there's a goose in the current location
   * non-zero means there is a goose */
  int raw_goose_value, goose_value, last_visit;
  
  raw_goose_value = evaluator->geese[evaluator->robot_x][evaluator->robot_y];

  // check if 10 moves have passed since last visit
  last_visit = evaluator->last_visited[evaluator->robot_x][evaluator->robot_y];
  if (last_visit >= 0 && evaluator->move_no - last_visit < 10) {
    // visited a goose too soon
    goose_value = 0;
  } else {
    goose_value = raw_goose_value;
  }

  if (goose_value > 0) {
    evaluator->score += goose_value;
    robot_got_egg(&evaluator->robot, evaluator->robot_x, evaluator->robot_y,
                  goose_value);
    /* printf("%d, %d: %d[%d; %d]\n",
              evaluator->robot_x, evaluator->robot_y,
              goose_value, raw_goose_value,
              evaluator->score);
    */

    // update the last_visited matrix
    evaluator->last_visited[evaluator->robot_x][evaluator->robot_y] =
      evaluator->move_no;
  }
}

int evaluator_evaluate_robot_once(struct Evaluator *evaluator) {
  /* 
   * Evaluate one full circuit of the robot 2*rows*cols moves
   *
   * This will be called multiple times by `evaluate` to get
   * average performance of the robot
   */
  int new_x, new_y;
  int max_moves, move_direction;
  evaluator_init_robot(evaluator);
  
  // robot starts in cell 0
  // if there is a goose there, we need to process it
  // before any robot moves
  evaluator_process_current_cell(evaluator);

  // now start moving the robot
  max_moves = 2*evaluator->grid.rows*evaluator->grid.cols;
  for(evaluator->move_no=0; evaluator->move_no<max_moves;
      evaluator->move_no++) {
    move_direction = robot_move(&evaluator->robot);
    grid_new_location(&evaluator->grid,
                      evaluator->robot_x, evaluator->robot_y, move_direction,
                      &new_x, &new_y);
    evaluator->robot_x = new_x;
    evaluator->robot_y = new_y;
    if (!grid_is_valid_location(&evaluator->grid,
                                evaluator->robot_x, evaluator->robot_y)) {
      return -1;
    }
    evaluator_process_current_cell(evaluator);
  }

  return evaluator->score;
}

void evaluator_evaluate(struct Evaluator *evaluator) {
  int NUM_EVALS = 1, score_sum = 0, num_errors = 0;
  int i;
  int retval;
  FILE *fout;
  evaluator_read_input(evaluator, "input.txt");
  for(i=0; i<NUM_EVALS; i++) {
    retval = evaluator_evaluate_robot_once(evaluator);
    if (retval < 0) {
      num_errors++;
    } else {
      score_sum += retval;
    }
  }
  fout = fopen("output.txt", "w");
  if (num_errors == NUM_EVALS) {
    fprintf(fout, "error\n");
  } else {
    fprintf(fout, "%d\n", (int)(score_sum/NUM_EVALS));
  }
}

int main(void) {
  struct Evaluator evaluator;
  evaluator_evaluate(&evaluator);
}
