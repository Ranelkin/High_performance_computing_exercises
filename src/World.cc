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

        std::vector<std::vector<int>> curr_state;
        std::vector<std::vector<int>> future_state; 
        
    
    public: 
        World(int height, int width): height(height), width(width) {
            curr_state.resize(height+2, std::vector<int>(width+2, 0)); // Initialize with zeros
            future_state.resize(height+2, std::vector<int>(width+2, 0)); 
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
                //Add blank line first 
                std::vector<int> v1(width+2, 0); 
                curr_state.push_back(v1); 
                future_state.push_back(v1); 
                //Construct current & future state from file
                while(getline(f, line)){

                    std::vector<int> line_vec; 
                    std::istringstream iss(line); 
                    int val; 
                    line_vec.push_back(0); 
                    while (iss >> val) {
                        line_vec.push_back(val); 
                    }
                    line_vec.push_back(0); 

                    curr_state.push_back(line_vec); 
                    future_state.push_back(line_vec); 
                }

                //Add blank line after added matrix 
                std::vector<int> v2(width+2, 0); 
                curr_state.push_back(v2); 
                future_state.push_back(v2); 

                // Output generated matrix 
                std::cout << "Created world from file: " << std::endl;
                int n_height = height+2; 
                int n_width = width+2; 
                for (int i=1; i< n_height; i++) {
                    for (int j=1; j<n_width; j++) {
                        std::cout << curr_state[i][j] << " ";
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

        void evolve(){
            int n_height = height+2; 
            int n_width = width+2; 
            for (int i=1; i<n_height-1; i++){
                for (int j=1; j<n_width-1; j++){
                    int n_sum = 0; //sum of neighbours
                    n_sum += curr_state[i-1][j-1]+ curr_state[i-1][j] + curr_state[i-1][j+1]
                    + curr_state[i][j-1] + curr_state[i][j+1] + curr_state[i+1][j-1] + curr_state[i+1][j] + curr_state[i+1][j+1]; 

                    if (curr_state[i][j]==1){
                        bool alive = n_sum == 2 || n_sum == 3; 
                        future_state[i][j] = alive ? 1 : 0; 
                        
                    } else {
                        bool alive = n_sum == 3; 
                        future_state[i][j] = alive ? 1 : 0; 
                    }
                }
            }
            std::swap(curr_state, future_state); 
        }

        bool is_stable(){

            return false; 
        }

        void print() {
            std::cout << "\033[2J\033[H"; // Clear screen
            int n_height = height+1;
            int n_width = width+1;
            
            for (int i=1; i< n_height; i++) {
                for (int j=1; j<n_width; j++) {
                    std::string cell = curr_state[i][j] == 1 ? "\033[1m\033[32m\u2593\u2593\033[0m\033" : "\033[1m\033[90m\u2591\u2591\033[0m";
                    std::cout << cell << " ";
                }
                std::cout << std::endl;
            }
            
            std::cout << std::endl;
        }

        void load(std::string f_path){
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
        // Generates grid with random 0 1 occurences
        void random(double probability = 0.3) {
        std::random_device rd;
        std::mt19937 gen(rd()); 
        std::bernoulli_distribution dist(probability); 

        // Populate inner grid 
        for (int i = 1; i <= height; ++i) {
            for (int j = 1; j <= width; ++j) {
                curr_state[i][j] = dist(gen); // Randomly set to 0 or 1
            }
        }
    }

    bool is_stable(){
        std::vector<std::vector<int>> copy = curr_state; 
        evolve(); 
        evolve(); 
        return curr_state == copy; 
    }
}; 


int main(){
    
    //std::string filepath; 
    
    int height = 30; 
    int width = 30; 
    std::string filepath = "../p67_snark_loop.txt"; 
    World world = World(height, width);
    world.random(0.3); 
    
    while(true){
        world.evolve(); // Compute next state
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Wait for 1 second
        world.print(); // Print new state
    }
   
    return 0; 
}