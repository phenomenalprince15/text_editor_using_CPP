#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <sys/stat.h>
#include <ncurses.h>
#include <vector>

using namespace std;

enum Mode {
    COMMAND, // where I can enter commands in below line.
    NORMAL // where I can NORMAL characters and do some stuff
};

/*
Steps:
1. Open a file, if file exist, create a file and save
*/

bool fileExists(const std::string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

void printLines(const std::vector<std::string>& lines) {
    int debugRow = LINES - 2;  // Reserve space for showing the vector's content
    for (size_t i = 0; i < lines.size(); i++) {
        mvprintw(debugRow - lines.size() + i, 0, "Line %ld: %s", i, lines[i].c_str());
    }
}


int main(int argc, char** argv) {
    std::string filename;

    // Check for filename in command-line arguments
    if (argc > 1) {
        filename = argv[1];
    } else {
        // Default filename if none provided
        filename = "untitled.txt";
    }

    // Check if file exists, open/create accordingly
    std::fstream file;
    if (fileExists(filename)) {
        file.open(filename, std::ios::in | std::ios::out);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return 1;
        }
    } else {
        file.open(filename, std::ios::out); // Create a new file
        if (!file.is_open()) {
            std::cerr << "Error creating file: " << filename << std::endl;
            return 1;
        }
    }

    // Start ncurses
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, true);

    Mode currentMode = COMMAND;
    int ch;

    int x=0, y=0;
    string line = "";
    vector<string> lines = {};
    // printw("%d", COLS);

    // Main loop
    while (true) {
        if (currentMode == NORMAL) {
            mvprintw(LINES-1, 0, "-- NORMAL Mode --");
        } else if(currentMode == COMMAND) {
            mvprintw(LINES-1, 0, "-- COMMAND MODE --");
        }

        ch = getch();
        move(LINES-1, 0);
        printw("\t\t\t%c", ch);

        if (currentMode == NORMAL) {
            mvaddch(x, y, ' ');  // Clear the previous vertical bar
        }

        switch (currentMode) {
            case COMMAND:
                if (ch == 'i') {
                    currentMode = NORMAL;
                    // clear();
                } else if (ch == 'q') {
                    // Quit only in NORMAL mode
                    endwin();  // Close ncurses
                    file.close();
                    return 0;
                }
                break;

            case NORMAL:
                if (ch == 27) {  // ESC key to switch back to NORMAL mode
                    currentMode = COMMAND;
                } else if(ch == 127 || ch == KEY_BACKSPACE) {
                    // if (x < 0 && y<0) move(0, 0);
                    if (y > 0) {
                        y--;
                        move(x, y);
                        addch(' ');
                        line.pop_back();
                    } else if (x > 0) {
                        x--;
                        y = lines[x].length();
                        line = lines[x];  // Load previous line content into `line`
                        move(x, y);
                        addch(' ');
                        if (!lines[x].empty()) {
                            lines[x].pop_back();  // Remove last character of the previous line
                        }
                        // If the previous line becomes empty, remove it from `lines`
                        if (lines[x].empty()) {
                            lines.erase(lines.begin() + x);
                        }
                    }
                } else if (ch == '\n') {// Handle Enter key
                    lines.push_back(line);
                    line.clear();
                    if(x < LINES-1) {
                        y = 0;
                        x++;
                    }
                } else {
                    move(x, y);
                    line += ch;
                    addch(ch);
                    y++;

                    if (y >= COLS) { 
                        lines.push_back(line);
                        line.clear();
                        y = 0;
                        if(x < LINES-1){
                            x++;
                        }
                    }
                }
                break;
        }
        if (currentMode == NORMAL) {
            mvaddch(x, y, '|');
        }
        refresh();  // Refresh the screen
    }


    // Close ncurses
    endwin();

    // Close the file
    file.close();

    return 0;
}
