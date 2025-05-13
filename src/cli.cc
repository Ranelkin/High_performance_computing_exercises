#include "cli.h"
#include <random>
#include <thread>

CLI::CLI() : print_delay(100), print_world(false), check_stability(false) {}

void CLI::create(int height, int width) { world = World(height, width); }

void CLI::load(std::string f_path) { world = World(f_path); }

void CLI::save(std::string f_path) {
   world.load(f_path); 
}

void CLI::print(int setting) {
    print_world = (setting == 1);
    if (print_world) {
        world.print();
    }
}

void CLI::delay(int ms) {
    print_delay = ms;
}

void CLI::stability(int x) {
    check_stability = (x == 1);
}

double CLI::run(int gen) {
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < gen; i++) {
        if (print_world) {
            world.print();
        }
        if (check_stability && world.is_stable()) {
            std::cout << "World is stable after " << i << " generations" << std::endl;
            break;
        }
        world.evolve();
        if (print_world) {
            std::this_thread::sleep_for(std::chrono::milliseconds(print_delay));
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    return duration.count();
}

void CLI::set(int x, int y, int alive) {
    int height = world.get_height();
    int width = world.get_width();
    // Toroidal wrapping
    int wrapped_x = (x - 1) % height + 1;
    int wrapped_y = (y - 1) % width + 1;
    if (wrapped_x < 1) wrapped_x += height;
    if (wrapped_y < 1) wrapped_y += width;
    if (alive > 0) {
        world.set(wrapped_x, wrapped_y);
    }
}

void CLI::set(int index, int alive) {
    if (alive > 0) {
        world.set(index);
    }
}

void CLI::get(int x, int y) {
    int height = world.get_height();
    int width = world.get_width();
    // Toroidal wrapping
    int wrapped_x = (x - 1) % height + 1;
    int wrapped_y = (y - 1) % width + 1;
    if (wrapped_x < 1) wrapped_x += height;
    if (wrapped_y < 1) wrapped_y += width;
    std::cout << world.get(wrapped_x, wrapped_y) << std::endl;
}

void CLI::get(int index) {
    std::cout << world.get(index) << std::endl;
}

void CLI::glider(int x, int y) {
    int height = world.get_height();
    int width = world.get_width();
    // Toroidal wrapping for anchor cell
    int base_x = (x - 1) % height + 1;
    int base_y = (y - 1) % width + 1;
    if (base_x < 1) base_x += height;
    if (base_y < 1) base_y += width;
    int coords[5][2] = {
        {0, 1}, 
        {1, 2}, 
        {2, 0}, 
        {2, 1},
        {2, 2}
    };
    for (const auto& coord : coords) {
        int dx = coord[0];
        int dy = coord[1];
        int new_x = (base_x + dx - 1) % height + 1;
        int new_y = (base_y + dy - 1) % width + 1;
        if (new_x < 1) new_x += height;
        if (new_y < 1) new_y += width;
        world.set(new_x, new_y);
    }
}

void CLI::toad(int x, int y) {
    int height = world.get_height();
    int width = world.get_width();
    int base_x = (x - 1) % height + 1;
    int base_y = (y - 1) % width + 1;
    if (base_x < 1) base_x += height;
    if (base_y < 1) base_y += width;
    // Toad pattern
    int coords[6][2] = {
        {0, 1}, {0, 2}, {0, 3}, 
        {1, 0}, {1, 1}, {1, 2}  
    };
    for (const auto& coord : coords) {
        int dx = coord[0];
        int dy = coord[1];
        int new_x = (base_x + dx - 1) % height + 1;
        int new_y = (base_y + dy - 1) % width + 1;
        if (new_x < 1) new_x += height;
        if (new_y < 1) new_y += width;
        world.set(new_x, new_y);
    }
}

void CLI::beacon(int x, int y) {
    int height = world.get_height();
    int width = world.get_width();
    int base_x = (x - 1) % height + 1;
    int base_y = (y - 1) % width + 1;
    if (base_x < 1) base_x += height;
    if (base_y < 1) base_y += width;
    // Beacon pattern
    int coords[6][2] = {
        {0, 0}, {0, 1},
        {1, 0},         
        {2, 3},         
        {3, 2}, {3, 3}  
    };
    for (const auto& coord : coords) {
        int dx = coord[0];
        int dy = coord[1];
        int new_x = (base_x + dx - 1) % height + 1;
        int new_y = (base_y + dy - 1) % width + 1;
        if (new_x < 1) new_x += height;
        if (new_y < 1) new_y += width;
        world.set(new_x, new_y);
    }
}

void CLI::methuselah(int x, int y) {
    int height = world.get_height();
    int width = world.get_width();
    int base_x = (x - 1) % height + 1;
    int base_y = (y - 1) % width + 1;
    if (base_x < 1) base_x += height;
    if (base_y < 1) base_y += width;
    // R-pentomino pattern
    int coords[5][2] = {
        {0, 1}, {0, 2}, 
        {1, 0}, {1, 1},
        {2, 1}          
    };
    for (const auto& coord : coords) {
        int dx = coord[0];
        int dy = coord[1];
        int new_x = (base_x + dx - 1) % height + 1;
        int new_y = (base_y + dy - 1) % width + 1;
        if (new_x < 1) new_x += height;
        if (new_y < 1) new_y += width;
        world.set(new_x, new_y);
    }
}

void CLI::random(int n) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> pattern_dist(0, 3); // 4 patterns
    std::uniform_int_distribution<> x_dist(1, world.get_height());
    std::uniform_int_distribution<> y_dist(1, world.get_width());

    for (int i = 0; i < n; i++) {
        int pattern = pattern_dist(gen);
        int x = x_dist(gen);
        int y = y_dist(gen);
        switch (pattern) {
            case 0: glider(x, y); break;
            case 1: toad(x, y); break;
            case 2: beacon(x, y); break;
            case 3: methuselah(x, y); break;
        }
    }
}