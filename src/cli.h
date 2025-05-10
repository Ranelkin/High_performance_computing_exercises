#ifndef CLI_H
#define CLI_H 
#include <string>


class CLI {
    
    public: 
        void create(); 

        void load(std::string f_path); 

        void save(); 

        void print(int setting);
        
        void delay(int ms); 

        void stability(); 

        void run(); 

        void set(int x, int y, int alive); 

        void set(int index); 

        void get(int x, int y); 

        void get(int index); 

        void glider(int x, int y); 

        void toad(int x, int y); 

        void beacon(int x, int y); 

        void methuselah(int x, int y); 

        void random(int n); 

}; 


#endif
