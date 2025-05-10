#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

class World {
    private: 
        int height; 
        int width; 

        std::vector<std::vector<int>> curr_state;
    
    
    public: 
        World(int height, int width): height(height), width(width) {
            curr_state.resize(height, std::vector<int>(width, 0)); // Initialize with zeros
        }

        World(std::string f_path) {
            std::string height_str; 
            std::string width_str; 
            std::ifstream f(f_path); 
            std::string line; 
            // construct World from input file 
            if (f.is_open()){
                // read in height / width of world 
                if (std::getline(f, height_str)){
                    height = std::stoi(height_str); 
                }
            
                if (std::getline(f, width_str)){
                    width = std::stoi(width_str); 
                }
                while(getline(f, line)){
                    std::vector<int> line_vec; 
                    std::istringstream iss(line); 
                    int val; 

                    while (iss >> val) {
                        line_vec.push_back(val); 
                    }
                    curr_state.push_back(line_vec); 
                }
                // Output generated matrix 
                std::cout << "Created world from file: " << std::endl;
                for (const auto& row : curr_state) {
                    for (const auto& cell : row) {
                        std::cout << cell << " ";
                    }
                    std::cout << std::endl;
                }
            } else {
                std::cout << "error opening file" << std::endl; 
            }
        };


        int get_height(){
            return height; 
        }

        int get_width(){
            return width; 
        }

        int world_size(){
            return width*height; 
        }

        void evolve(){}

        bool is_stable(){

            return false; 
        }

        void print(){
            for (const auto& row : curr_state) {
                for (const auto& cell : row) {
                    std::cout << cell << " ";
                }
                std::cout << std::endl;
            }
        }

        void load(std::string f_path){

        }

        void save(){
            std::ofstream File("GameState.txt"); 
            File << height << std::endl; 
            File << width << std::endl; 
            for (size_t i = 0; i < curr_state.size(); i++){
                for (size_t j = 0; j < curr_state[i].size(); j++){
                    File << curr_state[i][j] << " "; 
                }
                File << std::endl; 
            }
        }
}; 


int main(){
    
    //std::string filepath; 
    int height = 10; 
    int width = 10; 
    World world = World(height, width);
    world.print(); 
    world.save(); 
    return 0; 
}