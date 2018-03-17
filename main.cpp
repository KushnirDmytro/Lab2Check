#include <iostream>
#include <fstream>
#include <cmath>

int main() {
    std::cout << "Hello, World!" << std::endl;
    std::ofstream myfile;
    
    for (int i = 0; i < 10; i++){
        myfile.open("program_result.csv");
        myfile << 'a' << "," << i << std::endl << std::flush ;
        myfile.close();
    }
    return 0;
}