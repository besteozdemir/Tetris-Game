#include "BlockFall.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

BlockFall::BlockFall(string grid_file_name, string blocks_file_name, bool gravity_mode_on,
                     const string &leaderboard_file_name, const string &player_name) : gravity_mode_on(
        gravity_mode_on), leaderboard_file_name(leaderboard_file_name), player_name(player_name) {
    this->gravity_mode_on = gravity_mode_on;
    this->leaderboard_file_name = leaderboard_file_name;
    this->player_name = player_name;

    initialize_grid(grid_file_name);
    read_blocks(blocks_file_name);
    leaderboard.read_from_file(leaderboard_file_name);

}

vector<string> readInputFile(const string &fileName) {
    vector<string> lines;
    ifstream inputFile(fileName);

    if (!inputFile.is_open()) {
        cerr << "Error: Unable to open file: " << fileName << endl;
        exit(-1);
    }

    string line;
    while (getline(inputFile, line)) {
        lines.push_back(line);
    }

    inputFile.close();
    return lines;
}

std::vector<std::string> splitLines(const std::vector<std::string>& lines) {
    std::vector<std::string> splitResult;
    std::stringstream ss;

    bool emptyLineDetected = false;

    for (const std::string& line : lines) {
        if (line.empty()) {
            if (!emptyLineDetected) {
                if (!ss.str().empty()) {
                    splitResult.push_back(ss.str());
                    ss.str("");
                }
            }
            emptyLineDetected = true;
        } else {
            ss << line << '\n';
            emptyLineDetected = false;
        }
    }

    if (!ss.str().empty()) {
        splitResult.push_back(ss.str());
    }

    return splitResult;
}


vector<vector<bool>> convertToBoolMatrix(const string &element) {
    vector<vector<bool>> boolMatrix;
    vector<bool> currentVector;

    for (char ch: element) {
        if (ch == '1') {
            currentVector.push_back(true);
        } else if (ch == '0') {
            currentVector.push_back(false);
        } else if (ch == '\n') {
            boolMatrix.push_back(currentVector);
            currentVector.clear();
        }
    }
    return boolMatrix;
}

bool isHorizontalSymmetric(vector<vector<bool>> matrix) {
    int rows = matrix.size();

    if (rows <= 1) {
        return true;
    }

    int cols = matrix[0].size();
    if(rows == cols) {
        for (int i = 0; i < rows; ++i) {
            vector<bool> reversedRow = matrix[i];
            std::reverse(reversedRow.begin(), reversedRow.end());

            if (matrix[i] != reversedRow) {
                return false;
            }
        }
    } else {
        return false;
    }

    return true;
}

bool isVerticalSymmetric(vector<vector<bool>> matrix) {
    int rows = matrix.size();
    int cols = matrix[0].size();

    if (cols <= 1) {
        return true;
    }
    if(rows == cols) {
        for (int j = 0; j < cols; ++j) {
            vector<bool> reversedCol;
            for (int i = 0; i < rows; ++i) {
                reversedCol.push_back(matrix[i][j]);
            }
            std::reverse(reversedCol.begin(), reversedCol.end());

            for (int i = 0; i < rows; ++i) {
                if (matrix[i][j] != reversedCol[i]) {
                    return false;
                }
            }
        }
    } else {
        return false;
    }



    return true;
}

bool isSymmetric(vector<vector<bool>> matrix) {
    return isHorizontalSymmetric(matrix) && isVerticalSymmetric(matrix);
}

vector<vector<bool>> rotateClockwise(vector<vector<bool>> matrix) {
    int rows = matrix.size();
    int cols = matrix[0].size();

    vector<vector<bool>> rotatedMatrix(cols, vector<bool>(rows, 0));
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            rotatedMatrix[j][i] = matrix[i][j];
        }
    }

    for (int i = 0; i < cols; ++i) {
        reverse(rotatedMatrix[i].begin(), rotatedMatrix[i].end());
    }

    return rotatedMatrix;
}

void BlockFall::read_blocks(const string &input_file) {
    // TODO: Read the blocks from the input file and initialize "initial_block" and "active_rotation" member variables
    // TODO: For every block, generate its rotations and properly implement the multilevel linked list structure
    //       that represents the game blocks, as explained in the PA instructions.
    // TODO: Initialize the "power_up" member variable as the last block from the input file (do not add it to the linked list!)

    vector<string> lines = readInputFile(input_file);
    vector<string> splitResult = splitLines(lines);
    Block *prev = nullptr;
    for (const string &str: splitResult) {
        if (str.empty()) break;
        vector<vector<bool>> boolMatrix = convertToBoolMatrix(str);

        Block *pBlock = new Block();
        pBlock->shape = boolMatrix;

        if (!initial_block) {
            initial_block = pBlock;
            active_rotation = pBlock;
            rotateRight(pBlock);
            prev = initial_block;
        } else {
            prev->next_block = pBlock;
            rotateRight(pBlock);
            prev = pBlock;
        }
    }
    Block *current = initial_block;
    Block *previous = nullptr;

    while (current->next_block) {
        previous = current;
        current = current->next_block;
    }
    if (!previous) {
        power_up = current->shape;
        delete current;
    } else {
        power_up = current->shape;
        previous->next_block = nullptr;
        delete current;
    }
}

void BlockFall::rotateRight(Block *pBlock) {
    if(isSymmetric(pBlock->shape)) {
        pBlock->right_rotation = pBlock;
        pBlock->left_rotation = pBlock;
    } else {
        Block *secondBlock = new Block();
        secondBlock->shape = rotateClockwise(pBlock->shape);
        pBlock->right_rotation = secondBlock;
        secondBlock->left_rotation = pBlock;

        if (rotateClockwise(secondBlock->shape) == pBlock->shape) {
            secondBlock->right_rotation = pBlock;
            pBlock->left_rotation = secondBlock;
        } else {
            Block *thirdBlock = new Block();
            Block *fourthBlock = new Block();
            thirdBlock->shape = rotateClockwise(secondBlock->shape);
            fourthBlock->shape = rotateClockwise(thirdBlock->shape);
            secondBlock->right_rotation = thirdBlock;
            thirdBlock->left_rotation = secondBlock;
            thirdBlock->right_rotation = fourthBlock;
            fourthBlock->left_rotation = thirdBlock;
            fourthBlock->right_rotation = pBlock;
            pBlock->left_rotation = fourthBlock;
        }
    }
}

void BlockFall::initialize_grid(const string &input_file) {
    // TODO: Initialize "rows" and "cols" member variables
    // TODO: Initialize "grid" member variable using the command-line argument 1 in main

    vector<string> lines = readInputFile(input_file);

    for (const string &line: lines) {
        vector<int> intLine;
        for (char c: line) {
            if (c == '0') {
                intLine.push_back(0);
            } else if (c == '1') {
                intLine.push_back(1);
            } else {
                continue;
            }
        }
        grid.push_back(intLine);
    }

    rows = grid.size();
    cols = grid[0].size();
}

BlockFall::~BlockFall() {
    // TODO: Free dynamically allocated memory used for storing game blocks
    Block* current = initial_block;

    while (current) {
        Block* rotation = current->right_rotation;
        while (rotation && rotation != current && rotation->right_rotation != current && rotation->right_rotation->right_rotation->right_rotation != current) {
            Block* nextRotation = rotation->right_rotation;
            delete rotation;
            rotation = nextRotation;
        }

        Block* next = current->next_block;
        delete current;
        current = next;
    }
}