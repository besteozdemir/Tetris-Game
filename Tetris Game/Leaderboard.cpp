#include "Leaderboard.h"
#include <iostream>
#include <fstream>

void Leaderboard::insert_new_entry(LeaderboardEntry *new_entry) {
    // TODO: Insert a new LeaderboardEntry instance into the leaderboard, such that the order of the high-scores
    //       is maintained, and the leaderboard size does not exceed 10 entries at any given time (only the
    //       top 10 all-time high-scores should be kept in descending order by the score).
    if (!head_leaderboard_entry || new_entry->score > head_leaderboard_entry->score) {
        new_entry->next_leaderboard_entry = head_leaderboard_entry;
        head_leaderboard_entry = new_entry;
    } else {
        LeaderboardEntry *curr = head_leaderboard_entry;
        while (curr->next_leaderboard_entry &&
               new_entry->score <= curr->next_leaderboard_entry->score) {
            curr = curr->next_leaderboard_entry;
        }
        new_entry->next_leaderboard_entry = curr->next_leaderboard_entry;
        curr->next_leaderboard_entry = new_entry;
    }

    LeaderboardEntry *prev = nullptr;
    LeaderboardEntry *curr = head_leaderboard_entry;
    int count = 0;
    while (curr && count < MAX_LEADERBOARD_SIZE) {
        prev = curr;
        curr = curr->next_leaderboard_entry;
        count++;
    }

    if (prev) {
        prev->next_leaderboard_entry = nullptr;
    }
}

void Leaderboard::write_to_file(const string &filename) {
    // TODO: Write the latest leaderboard status to the given file in the format specified in the PA instructions
    ofstream file(filename);
    LeaderboardEntry *curr = head_leaderboard_entry;
    while (curr) {
        file << curr->score << " " << curr->last_played << " " << curr->player_name << endl;
        curr = curr->next_leaderboard_entry;
    }
    file.close();
}

void Leaderboard::read_from_file(const string &filename) {
    // TODO: Read the stored leaderboard status from the given file such that the "head_leaderboard_entry" member
    //       variable will point to the highest all-times score, and all other scores will be reachable from it
    //       via the "next_leaderboard_entry" member variable pointer.
    ifstream file(filename);
    LeaderboardEntry *curr;
    LeaderboardEntry *prev = nullptr;

    unsigned long score;
    time_t lastPlayed;
    string playerName;

    while (file >> score >> lastPlayed >> playerName) {
        curr = new LeaderboardEntry(score, lastPlayed, playerName);
        if (head_leaderboard_entry) {
            prev->next_leaderboard_entry = curr;
        } else {
            head_leaderboard_entry = curr;
        }
        prev = curr;
    }

    file.close();
}


void Leaderboard::print_leaderboard() {
    // TODO: Print the current leaderboard status to the standard output in the format specified in the PA instructions
    LeaderboardEntry *curr = head_leaderboard_entry;
    int rank = 1;

    while (curr) {
        std::tm *timeinfo = std::localtime(&(curr->last_played));
        char buffer[80];
        std::strftime(buffer, 80, "%H:%M:%S/%d.%m.%Y", timeinfo);

        cout << rank << ". " << curr->player_name << " " << curr->score << " " << buffer << endl;
        curr = curr->next_leaderboard_entry;
        rank++;
    }
}

Leaderboard::~Leaderboard() {
    // TODO: Free dynamically allocated memory used for storing leaderboard entries
    LeaderboardEntry *curr = head_leaderboard_entry;
    LeaderboardEntry *next;

    while (curr) {
        next = curr->next_leaderboard_entry;
        delete curr;
        curr = next;
    }
}
