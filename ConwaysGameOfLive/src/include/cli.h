#ifndef CLI_H
#define CLI_H 
#include <string>
#include "World.h"
#include <chrono>

class CLI {
    /*
        CLI class for interaction with cellular automaton
    */
public: 
    CLI();

    // Create world given height and width
    void create(int height, int width); 

    // Load world from specified file 
    void load(std::string f_path); 
    
    // Save current state of the world to a file
    void save(std::string filename); 

    // Enable/disable printing of the world
    void print(int setting);
    
    // Set delay for printing (in ms)
    void delay(int ms); 

    // Enable/disable stability check
    void stability(int x); 

    // Run for n generations and return execution time
    double run(int gen); 

    // Set cell state at (x, y)
    void set(int x, int y, int alive); 

    // Set cell state at index
    void set(int index, int alive); 

    // Get cell state at (x, y)
    void get(int x, int y); 

    // Get cell state at index
    void get(int index); 

    // Add glider pattern at (x, y)
    void glider(int x, int y); 

    // Add toad pattern at (x, y)
    void toad(int x, int y); 

    // Add beacon pattern at (x, y)
    void beacon(int x, int y); 

    // methuselah pattern at (x, y)
    void methuselah(int x, int y); 

    // Add n random patterns
    void random(int n); 

private: 
    bool print_world = false; 
    bool check_stability = false; 
    int print_delay = 100; 
    World world; 
}; 

#endif