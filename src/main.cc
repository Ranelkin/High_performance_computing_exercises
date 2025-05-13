#include "cli.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <algorithm>
#include <iterator>

enum commands {
    CREATE, 
    LOAD, 
    SAVE, 
    PRINT, 
    DELAY, 
    STABILITY, 
    RUN, 
    SET, 
    GET, 
    GLIDER, 
    TOAD, 
    BEACON, 
    METHUSELAH, 
    RANDOM,
    HELP,
    EXIT
};

// Trim leading and trailing whitespace
std::string trim(std::string str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, last - first + 1);
}

// Split string into tokens
std::vector<std::string> split(const std::string& input) { 
    std::istringstream buffer(input);
    std::vector<std::string> ret;
    std::copy(std::istream_iterator<std::string>(buffer), 
              std::istream_iterator<std::string>(),
              std::back_inserter(ret));
    return ret;
}

// Map command strings to enum values
std::map<std::string, commands> command_map = {
    {"create", CREATE},
    {"load", LOAD},
    {"save", SAVE},
    {"print", PRINT},
    {"delay", DELAY},
    {"stability", STABILITY},
    {"run", RUN},
    {"set", SET},
    {"get", GET},
    {"glider", GLIDER},
    {"toad", TOAD},
    {"beacon", BEACON},
    {"methuselah", METHUSELAH},
    {"random", RANDOM},
    {".help", HELP},
    {".exit", EXIT}
};

int main() {
    CLI cli;
    std::string input_str;

    std::cout << "Cellular Automaton CLI. Type '.help' for commands." << std::endl;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input_str);
        input_str = trim(input_str);

        if (input_str.empty()) {
            continue;
        }

        std::vector<std::string> tokens = split(input_str);
        std::string command = tokens[0];

        // Convert command to lowercase for case-insensitive matching
        std::transform(command.begin(), command.end(), command.begin(), ::tolower);

        // Check if command exists
        auto it = command_map.find(command);
        if (it == command_map.end()) {
            std::cout << "Unknown command: " << command << ". Type '.help' for commands." << std::endl;
            continue;
        }

        commands cmd = it->second;

        try {
            switch (cmd) {
                case CREATE: {
                    if (tokens.size() != 3) {
                        throw std::runtime_error("Usage: create <height> <width>");
                    }
                    int height = std::stoi(tokens[1]);
                    int width = std::stoi(tokens[2]);
                    if (height <= 0 || width <= 0) {
                        throw std::runtime_error("Height and width must be positive");
                    }
                    cli.create(height, width);
                    std::cout << "Created " << height << "x" << width << " world" << std::endl;
                    break;
                }
                case LOAD: {
                    if (tokens.size() != 2) {
                        throw std::runtime_error("Usage: load <filename>");
                    }
                    cli.load(tokens[1]);
                    std::cout << "Loaded world from " << tokens[1] << std::endl;
                    break;
                }
                case SAVE: {
                    if (tokens.size() != 2) {
                        throw std::runtime_error("Usage: save <filename>");
                    }
                    cli.save(tokens[1]);
                    std::cout << "Saved world to " << tokens[1] << std::endl;
                    break;
                }
                case PRINT: {
                    if (tokens.size() != 2) {
                        throw std::runtime_error("Usage: print <0|1>");
                    }
                    int setting = std::stoi(tokens[1]);
                    if (setting != 0 && setting != 1) {
                        throw std::runtime_error("Setting must be 0 or 1");
                    }
                    cli.print(setting);
                    std::cout << "Print setting: " << (setting ? "enabled" : "disabled") << std::endl;
                    break;
                }
                case DELAY: {
                    if (tokens.size() != 2) {
                        throw std::runtime_error("Usage: delay <ms>");
                    }
                    int ms = std::stoi(tokens[1]);
                    if (ms < 0) {
                        throw std::runtime_error("Delay must be non-negative");
                    }
                    cli.delay(ms);
                    std::cout << "Set delay to " << ms << " ms" << std::endl;
                    break;
                }
                case STABILITY: {
                    if (tokens.size() != 2) {
                        throw std::runtime_error("Usage: stability <0|1>");
                    }
                    int setting = std::stoi(tokens[1]);
                    if (setting != 0 && setting != 1) {
                        throw std::runtime_error("Setting must be 0 or 1");
                    }
                    cli.stability(setting);
                    std::cout << "Stability check: " << (setting ? "enabled" : "disabled") << std::endl;
                    break;
                }
                case RUN: {
                    if (tokens.size() != 2) {
                        throw std::runtime_error("Usage: run <generations>");
                    }
                    int gen = std::stoi(tokens[1]);
                    if (gen < 0) {
                        throw std::runtime_error("Generations must be non-negative");
                    }
                    double time = cli.run(gen);
                    std::cout << "Ran " << gen << " generations in " << time << " seconds" << std::endl;
                    break;
                }
                case SET: {
                    if (tokens.size() == 3) {
                        int index = std::stoi(tokens[1]);
                        int alive = std::stoi(tokens[2]);
                        if (index < 0) {
                            throw std::runtime_error("Index must be non-negative");
                        }
                        cli.set(index, alive);
                        std::cout << "Set cell at index " << index << " to " << (alive ? "alive" : "dead") << std::endl;
                    } else if (tokens.size() == 4) {
                        int x = std::stoi(tokens[1]);
                        int y = std::stoi(tokens[2]);
                        int alive = std::stoi(tokens[3]);
                        if (x < 1 || y < 1) {
                            throw std::runtime_error("Coordinates must be positive");
                        }
                        cli.set(x, y, alive);
                        std::cout << "Set cell at (" << x << ", " << y << ") to " << (alive ? "alive" : "dead") << std::endl;
                    } else {
                        throw std::runtime_error("Usage: set <x> <y> <0|1> or set <index> <0|1>");
                    }
                    break;
                }
                case GET: {
                    if (tokens.size() == 2) {
                        int index = std::stoi(tokens[1]);
                        if (index < 0) {
                            throw std::runtime_error("Index must be non-negative");
                        }
                        std::cout << "Cell at index " << index << ": ";
                        cli.get(index);
                    } else if (tokens.size() == 3) {
                        int x = std::stoi(tokens[1]);
                        int y = std::stoi(tokens[2]);
                        if (x < 1 || y < 1) {
                            throw std::runtime_error("Coordinates must be positive");
                        }
                        std::cout << "Cell at (" << x << ", " << y << "): ";
                        cli.get(x, y);
                    } else {
                        throw std::runtime_error("Usage: get <x> <y> or get <index>");
                    }
                    break;
                }
                case GLIDER: {
                    if (tokens.size() != 3) {
                        throw std::runtime_error("Usage: glider <x> <y>");
                    }
                    int x = std::stoi(tokens[1]);
                    int y = std::stoi(tokens[2]);
                    if (x < 1 || y < 1) {
                        throw std::runtime_error("Coordinates must be positive");
                    }
                    cli.glider(x, y);
                    std::cout << "Added glider at (" << x << ", " << y << ")" << std::endl;
                    break;
                }
                case TOAD: {
                    if (tokens.size() != 3) {
                        throw std::runtime_error("Usage: toad <x> <y>");
                    }
                    int x = std::stoi(tokens[1]);
                    int y = std::stoi(tokens[2]);
                    if (x < 1 || y < 1) {
                        throw std::runtime_error("Coordinates must be positive");
                    }
                    cli.toad(x, y);
                    std::cout << "Added toad at (" << x << ", " << y << ")" << std::endl;
                    break;
                }
                case BEACON: {
                    if (tokens.size() != 3) {
                        throw std::runtime_error("Usage: beacon <x> <y>");
                    }
                    int x = std::stoi(tokens[1]);
                    int y = std::stoi(tokens[2]);
                    if (x < 1 || y < 1) {
                        throw std::runtime_error("Coordinates must be positive");
                    }
                    cli.beacon(x, y);
                    std::cout << "Added beacon at (" << x << ", " << y << ")" << std::endl;
                    break;
                }
                case METHUSELAH: {
                    if (tokens.size() != 3) {
                        throw std::runtime_error("Usage: methuselah <x> <y>");
                    }
                    int x = std::stoi(tokens[1]);
                    int y = std::stoi(tokens[2]);
                    if (x < 1 || y < 1) {
                        throw std::runtime_error("Coordinates must be positive");
                    }
                    cli.methuselah(x, y);
                    std::cout << "Added methuselah at (" << x << ", " << y << ")" << std::endl;
                    break;
                }
                case RANDOM: {
                    if (tokens.size() != 2) {
                        throw std::runtime_error("Usage: random <n>");
                    }
                    int n = std::stoi(tokens[1]);
                    if (n < 0) {
                        throw std::runtime_error("Number of patterns must be non-negative");
                    }
                    cli.random(n);
                    std::cout << "Added " << n << " random patterns" << std::endl;
                    break;
                }
                case HELP: {
                    std::cout << "Available commands:\n"
                              << "  create <height> <width> : Create a new world\n"
                              << "  load <filename> : Load world from file\n"
                              << "  save <filename> : Save world to file\n"
                              << "  print <0|1> : Enable/disable printing\n"
                              << "  delay <ms> : Set print delay in milliseconds\n"
                              << "  stability <0|1> : Enable/disable stability check\n"
                              << "  run <generations> : Run simulation for n generations\n"
                              << "  set <x> <y> <0|1> : Set cell state at (x, y)\n"
                              << "  set <index> <0|1> : Set cell state at index\n"
                              << "  get <x> <y> : Get cell state at (x, y)\n"
                              << "  get <index> : Get cell state at index\n"
                              << "  glider <x> <y> : Add glider pattern at (x, y)\n"
                              << "  toad <x> <y> : Add toad pattern at (x, y)\n"
                              << "  beacon <x> <y> : Add beacon pattern at (x, y)\n"
                              << "  methuselah <x> <y> : Add methuselah pattern at (x, y)\n"
                              << "  random <n> : Add n random patterns\n"
                              << "  .help : Show this help\n"
                              << "  .exit : Exit the program\n";
                    break;
                }
                case EXIT: {
                    std::cout << "Exiting..." << std::endl;
                    return 0;
                }
            }
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
    }

    return 0;
}