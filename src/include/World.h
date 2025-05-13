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
        
    
    public: 
       
        World();
        World(int height, int width);
        World(std::string f_path);

        int get_height(); 

        int get_width(); 

        int world_size(); 

        void evolve(); 

        bool is_stable(); 

        void print(); 

        void load(std::string f_path); 

        void save(); 

        // Generates grid with random 0 1 occurences
        void random(double probability = 0.3); 

        void set(int x, int y); 

        void set(int index); 

        int get(int x, int y); 

        int get(int index); 
}; 

#endif