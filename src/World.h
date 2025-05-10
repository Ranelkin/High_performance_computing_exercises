#ifndef WORLD_H
#define WORLD_H
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>
#include <thread>
#include <random>

class World {
    /* The array holds an additional border of 0 around to allow efficient checking 
    without edge cases*/
    private: 

        int height; 
        int width; 

        std::vector<std::vector<int>> state1;
        std::vector<std::vector<int>> state2; 
        std::vector<std::vector<int>> state3; 
    
    public: 
        World(int height, int width): height(height), width(width) {}

        World(std::string f_path) {};


        int get_height(); 

        int get_width(); 

        int world_size(); 

        void evolve(); 

        bool is_stable(); 

        void print(); 

        void load(std::string f_path); 

        void save(); 

        bool is_stable(); 
        // Generates grid with random 0 1 occurences
        void random(double probability = 0.3); 

}; 

#endif