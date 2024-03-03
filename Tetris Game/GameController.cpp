#include "GameController.h"
#include <iostream>
#include <fstream>
#include <string>
#include "BlockFall.h"
#include <algorithm>

vector<string> readCommands(const string &fileName) {
    vector<string> lines;
    ifstream inputFile(fileName);

    string line;
    while (getline(inputFile, line)) {
        lines.push_back(line);
    }

    inputFile.close();
    return lines;
}

bool GameController::play(BlockFall &game, const string &commands_file) {
    // TODO: Implement the gameplay here while reading the commands from the input file given as the 3rd command-line
    //       argument. The return value represents if the gameplay was successful or not: false if game over,
    //       true otherwise.

    vector<string> lines = readCommands(commands_file);
    int rightValue = 0;
    for (string &line: lines) {
        vector<vector<bool>> &activeShape = game.active_rotation->shape;
        unsigned long long int activeRow = activeShape.size();
        unsigned long long int activeCol = activeShape[0].size();
        if (line == "PRINT_GRID") {
            cout << "Score: " << game.current_score << endl;
            cout << "High Score: " << game.leaderboard.head_leaderboard_entry->score << endl;

            for (int i = 0; i < game.rows; ++i) {
                for (int j = 0; j < game.cols; ++j) {
                    if (i < activeRow && rightValue <= j &&
                        j < activeCol + rightValue) {
                        if (activeShape[i][j - rightValue]) {
                            cout << occupiedCellChar;
                        } else {
                            if (game.grid[i][j]) {
                                cout << occupiedCellChar;
                            } else {
                                cout << unoccupiedCellChar;
                            }
                        }
                    } else {
                        if (game.grid[i][j]) {
                            cout << occupiedCellChar;
                        } else {
                            cout << unoccupiedCellChar;
                        }
                    }
                }
                cout << endl;
            }
            cout << endl;
            cout << endl;
        } else if (line == "DROP") {
            vector<int> scoreCount;
            int distance = game.rows;
            int countOnes = 0;
            for(int i = 0 ; i < game.active_rotation->shape.size() ; i++) {
                for(int j = 0 ; j < game.active_rotation->shape[0].size() ; j++) {
                    if(game.active_rotation->shape[i][j]) {
                        countOnes++;
                    }
                }
            }
            for (int j = 0; j < activeCol; j++) {
                for (int i = activeRow - 1; i >= 0; i--) {
                    if (activeShape[i][j]) {
                        for (int k = i + 1; k < game.rows; k++) {
                            if (game.grid[k][j + rightValue]) {
                                if (distance >= k - i - 1) {
                                    distance = k - i - 1;
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            if (distance == game.rows) {
                distance = game.rows - activeRow;
            }

            for (int i = 0; i < activeRow; i++) {
                for (int j = 0; j < activeCol; j++) {
                    if (activeShape[i][j]) {
                        game.grid[i + distance][j + rightValue] = 1;
                    }
                }
            }

            if (game.gravity_mode_on) {
                for (int j = 0; j < game.cols; j++) {
                    int count = 0;
                    for (int i = 0; i < game.rows; i++) {
                        if (game.grid[i][j]) {
                            count++;
                            game.grid[i][j] = 0;
                        }
                    }
                    for (int k = game.rows - 1; k > game.rows - count - 1; k--) {
                        game.grid[k][j] = 1;
                    }
                }
            }

            game.current_score += distance * countOnes;


            bool doesPowerUp = false;
            for (int i = 0; i <= game.rows - game.power_up.size(); ++i) {
                for (int j = 0; j <= game.cols - game.power_up[0].size(); ++j) {
                    bool doesExist = true;
                    for (int si = 0; si < game.power_up.size(); ++si) {
                        for (int sj = 0; sj < game.power_up[0].size(); ++sj) {
                            if (game.power_up[si][sj] != game.grid[i + si][j + sj]) {
                                doesExist = false;
                                break;
                            }
                        }
                        if (!doesExist) {
                            break;
                        }
                    }
                    if (doesExist) {
                        doesPowerUp = true;
                        break;
                    }
                }
                if (doesPowerUp) {
                    break;
                }
            }

            if (doesPowerUp) {
                cout << "Before clearing:" << endl;
                for (int i = 0; i < game.rows; ++i) {
                    for (int j = 0; j < game.cols; ++j) {
                        if (game.grid[i][j]) {
                            cout << occupiedCellChar;
                        } else {
                            cout << unoccupiedCellChar;
                        }
                    }
                    cout << endl;
                }
                cout << endl;
                cout << endl;

                int countOne = 0;

                for (int i = 0; i < game.rows; ++i) {
                    for (int j = 0; j < game.cols; ++j) {
                        if(game.grid[i][j]) {
                            game.grid[i][j] = 0;
                            countOne++;
                        }

                    }
                }
                game.current_score += countOne + 1000;
            }

            bool isFull = false;
            int fullCount = 0;
            vector<int> fullRows;
            for(int i = 0; i < game.rows; i++) {
                bool isFull2 = true;
                for(bool nums : game.grid[i]) {
                    if(!nums) {
                        isFull2 = false;
                        break;
                    }
                }
                if(isFull2){
                    isFull = true;
                    fullCount++;
                    fullRows.push_back(i);
                }
            }


            if(isFull) {
                game.current_score += game.cols * fullCount;
                cout << "Before clearing:" << endl;
                for (int i = 0; i < game.rows; ++i) {
                    for (int j = 0; j < game.cols; ++j) {
                        if (game.grid[i][j]) {
                            cout << occupiedCellChar;
                        } else {
                            cout << unoccupiedCellChar;
                        }
                    }
                    cout << endl;
                }
                cout << endl;
                cout << endl;

                for (int fullRow:fullRows) {
                    for (int i = fullRow; i > 0; --i) {
                        game.grid[i] = game.grid[i-1];
                    }
                }
            }


            game.initial_block = game.initial_block->next_block;
            game.active_rotation = game.initial_block;
            rightValue = 0;
            if (!game.initial_block) {
                cout << "YOU WIN!\n"
                        "No more block.\n"
                        "Final grid and score:" << endl;
                cout << endl;
                cout << "Score: " << game.current_score << endl;
                cout << "High Score: " << game.leaderboard.head_leaderboard_entry->score << endl;
                for (int i = 0; i < game.rows; ++i) {
                    for (int j = 0; j < game.cols; ++j) {
                        if (game.grid[i][j]) {
                            cout << occupiedCellChar;
                        } else {
                            cout << unoccupiedCellChar;
                        }
                    }
                    cout << endl;
                }
                cout << endl;

                cout << "Leaderboard\n"
                        "-----------" << endl;
                game.leaderboard.print_leaderboard();
                game.leaderboard.write_to_file(game.leaderboard_file_name);
                return true;
            }

            bool isItFit = true;
            for (int j = 0; j < game.active_rotation->shape[0].size(); ++j) {
                for (int i = game.active_rotation->shape.size() - 1; i >= 0; --i) {
                    if (game.active_rotation->shape[i][j] == 1 && game.grid[i][j] != 0) {
                        isItFit = false;
                        break;
                    }
                }
            }
            if(!isItFit) {
                cout << "GAME OVER!\n"
                        "Next block that couldn't fit:" << endl;

                for (int i = 0; i < game.active_rotation->shape.size(); ++i) {
                    for (int j = 0; j < game.active_rotation->shape[0].size(); ++j) {
                        if (game.active_rotation->shape[i][j]) {
                            cout << occupiedCellChar;
                        } else {
                            cout << unoccupiedCellChar;
                        }
                    }
                    cout << endl;
                }
                cout << endl;

                cout << "Final grid and score:" << endl;
                cout << endl;
                cout << "Score: " << game.current_score << endl;
                cout << "High Score: " << game.leaderboard.head_leaderboard_entry->score << endl;
                for (int i = 0; i < game.rows; ++i) {
                    for (int j = 0; j < game.cols; ++j) {
                        if (game.grid[i][j]) {
                            cout << occupiedCellChar;
                        } else {
                            cout << unoccupiedCellChar;
                        }
                    }
                    cout << endl;
                }
                cout << endl;

                cout << "Leaderboard\n"
                        "-----------" << endl;
                game.leaderboard.print_leaderboard();
                game.leaderboard.write_to_file(game.leaderboard_file_name);
                return false;
            }


        } else if (line == "MOVE_RIGHT") {
            bool canMoveRight = true;
            for (int i = 0; i < activeRow; i++) {
                for (int j = activeCol - 1; j >= 0; j--) {
                    if (activeShape[i][j]) {
                        if ((j + rightValue + 1) >= game.cols || game.grid[i][j + rightValue + 1]) {
                            canMoveRight = false;
                        }
                        break;
                    }
                }
            }

            if (canMoveRight) rightValue++;
        } else if (line == "MOVE_LEFT") {
            if (rightValue > 0) {
                rightValue--;
            }

        } else if (line == "GRAVITY_SWITCH") {
            if (!game.gravity_mode_on) {
                for (int j = 0; j < game.cols; j++) {
                    int count = 0;
                    for (int i = 0; i < game.rows; i++) {
                        if (game.grid[i][j]) {
                            count++;
                            game.grid[i][j] = 0;
                        }
                    }
                    for (int k = game.rows - 1; k > game.rows - count - 1; k--) {
                        game.grid[k][j] = 1;
                    }
                }

                bool isFull = false;
                int fullCount = 0;
                vector<int> fullRows;
                for(int i = 0; i < game.rows; i++) {
                    bool isFull2 = true;
                    for(bool nums : game.grid[i]) {
                        if(!nums) {
                            isFull2 = false;
                            break;
                        }
                    }
                    if(isFull2){
                        isFull = true;
                        fullCount++;
                        fullRows.push_back(i);
                    }
                }


                if(isFull) {
                    game.current_score += game.cols * fullCount;
                    for (int fullRow: fullRows) {
                        for (int i = fullRow; i > 0; --i) {
                            game.grid[i] = game.grid[i - 1];
                        }
                    }
                }
                game.gravity_mode_on = true;
            } else {
                game.gravity_mode_on = false;
            }
        } else if (line == "ROTATE_RIGHT") {
            game.active_rotation = game.active_rotation->right_rotation;
        } else if (line == "ROTATE_LEFT") {
            game.active_rotation = game.active_rotation->left_rotation;
        } else {
            cout << "Unknown command: " << line << endl;
        }
    }
    cout << "GAME FINISHED!\n"
            "No more commands.\n"
            "Final grid and score:" << endl;
    cout << endl;
    cout << "Score: " << game.current_score << endl;
    cout << "High Score: " << game.leaderboard.head_leaderboard_entry->score << endl;
    for (int i = 0; i < game.rows; ++i) {
        for (int j = 0; j < game.cols; ++j) {
            if (game.grid[i][j]) {
                cout << occupiedCellChar;
            } else {
                cout << unoccupiedCellChar;
            }
        }
        cout << endl;
    }
    cout << endl;
    cout << endl;
    cout << "Leaderboard\n"
            "-----------" << endl;
    game.leaderboard.print_leaderboard();
    game.leaderboard.write_to_file(game.leaderboard_file_name);
    return true;
}