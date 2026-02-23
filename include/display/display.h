//
// Created by german on 25/1/26.
//

#ifndef MYPROCESSMANAGER_DISPLAY_H
#define MYPROCESSMANAGER_DISPLAY_H

#include <memory>
#include <spdlog/spdlog.h>
#include <termios.h>
#include <unistd.h>

#include "common/errors.h"
#include "core/activeProcesses.h"
#include "core/systemMonitor.h"

namespace myProc {
    class Display {
    public:
        struct ANSI {
            enum class Color {
                RED = 31,
                BLUE = 34,
                GREEN = 32,
                YELLOW = 33,
                WHITE = 37,
                BLACK = 30,
                BRIGHT_GREEN = 92,
                BRIGHT_RED = 91,
                RESET = 0
            };


            enum class BackgroundColor {
                RED = 41,
                BLUE = 44,
                YELLOW = 43,
                WHITE = 47,
                BLACK = 40
            };

            enum class Style {
                NORMAL = 0,
                BOLD = 1,
                DIM = 2,
                UNDERLINE = 4,
                REVERSE = 7 //invert colors
            };

            struct Positioning {
                //display reference for inner to outer access
                Display &parent;

                //constructor for references
                explicit Positioning(Display &d) : parent(d) {}

                //ANSI
                static constexpr std::string_view RESET = "\x1B[0m"; //Reset colors
                static constexpr std::string_view SHOW_CURSOR = "\x1B[?25h";
                static constexpr std::string_view HIDE_CURSOR = "\x1B[2J\x1B[?25l";
                static constexpr std::string_view HOME = "\x1B[H"; //Move cursor to home(top/left)
                static constexpr std::string_view CLEAR = "\x1B[2J"; //Clear all screen
                static constexpr std::string_view CLEAR_SCROLL_BACK_BUFF = "\x1B[3J";
                static constexpr std::string_view CLEAR_CURSOR = "\x1B[K"; //clear from cursor to eol
                static constexpr std::string_view ASK_CURSOR_POSITION = "\x1B[6n"; //Ask the cursos position
                static constexpr std::string_view MOVE_TO_MAX_SCREEN = "\x1B[999;999H"; //overflow screen positioning

                //	Move cursor to row, col (1-indexed)
                static std::string moveCursorToRow(const int row, const int col) {
                    return "\x1B[" + std::to_string(row) + ";" + std::to_string(col) + "H";
                }

                std::pair<int, int> getEndTerminal2() const {
                    try {
                        //TODO: does this make sense?
                        const int tty = parent.tty;
                        if (tty == -1) {
                            parent.console->error("Not a real terminal");
                            //TODO: Change this exception
                            throw ProcessError("Error en terminal");
                        }

                        //Save current terminal settings
                        //save old to restore later
                        termios oldt, newt;
                        if (tcgetattr(tty, &oldt) == -1) { //reads current terminal config
                            throw std::runtime_error("getattr failed for termios");
                        }
                        newt = oldt;

                        //disable canonical mode so we can read characters
                        newt.c_lflag &= ~(ICANON | ECHO);
                        tcsetattr(tty, TCSANOW, &newt);

                        //Move the cursor and ask position
                        write(tty, MOVE_TO_MAX_SCREEN.data(), MOVE_TO_MAX_SCREEN.size());
                        write(tty, ASK_CURSOR_POSITION.data(), ASK_CURSOR_POSITION.size());


                        char ch;
                        std::string response;

                        //read character by character the terminal response, one byte a time
                        while (read(tty, &ch, 1) == 1) {
                            response += ch;
                            if (ch == 'R') break; //stop condition
                        }

                        //now restore old terminal config
                        tcsetattr(tty, TCSANOW, &oldt);
                        //close(tty);//TODO: check if we need to close this

                        //We dont have a split method
                        response.erase(0,2); //Erase ESC character
                        response.pop_back(); //Erase final R char
                        const size_t semiColonPos = response.find(';'); //Find position of semicolo and extract substrings
                        std::string first = response.substr(0, semiColonPos);
                        std::string second = response.substr(semiColonPos + 1, response.length() - 1);
                        std::pair positionPair(std::stoi(first), std::stoi(second));
                        return positionPair;
                    } catch (std::runtime_error &err) {
                        parent.console->error(err.what());
                        throw DisplayOutputError("Error in display - Cannot read end of terminal");
                    } catch (std::invalid_argument &err) {
                        parent.console->error(err.what());
                        throw DisplayOutputError("Error parsing terminal position, value not valid");
                    }
                }
            };

            static std::string buildAnsiCodes(std::optional<Color> fg, std::optional<BackgroundColor> bg,
                std::optional<Style> s) {
                std::string result = "\x1B[";
                if (fg.has_value()) result += std::to_string(static_cast<int>(fg.value())) + ";";
                if (bg.has_value()) result += std::to_string(static_cast<int>(bg.value())) + ";";
                if (s.has_value()) result += std::to_string(static_cast<int>(s.value())) + ";";
                return result.substr(0, result.length() - 1) + "m";
            }
        };

        /**
         * Constructor, initializes the Monitor anc process container
         */
        Display(SystemMonitor &system_monitor, ActiveProcesses &active_processes, const bool &active_logs);

        /**
         * Main render method to call in main
         */
        void render();

        void cleanup() const;

    private:
        //Logger
        std::shared_ptr<spdlog::logger> console;
        //Processes & Monitor
        ActiveProcesses &current_active_processes;
        SystemMonitor &current_system_monitor;
        //Terminal file descriptor
        int tty;
        std::pair<int, int> terminalCoordinates;
        static constexpr int MAX_CELL_SIZE = 25;

        /**
         * Outputs the Global system metrics
         */
        void renderSystemMetrics() const;

        void renderProcessesList() const;

        /**
         * Outputs the Header of the manager
         */
        void renderHeader() const;

        /**
         * Outputs the footer of the Manager
         */
        void renderFooter() const;

        void init() const;

        void displayGlobalMetrics() const;

        void displayProcessMetricsHeader() const;

        void displayProcessMetrics() const;

        [[nodiscard]] int openTerminal() const;

        void writeToTerminal(std::string_view msg) const;

        [[nodiscard]] std::pair<int,int> getEndTerminal() const;

    };
}

#endif //MYPROCESSMANAGER_DISPLAY_H
