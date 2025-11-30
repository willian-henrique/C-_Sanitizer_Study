#include <iostream>
#include <chrono>

int *pointer;

void define_pointer(int **parm_p){

    int a[20] = {10, 25, 30, 40, 50, 60, 70, 80, 90, 100,
                  110, 120, 130, 140, 150, 160, 170, 180, 190, 200};
    *parm_p = &a[5];
    std::cout << **parm_p << std::endl;
    std::cout << *parm_p << std::endl;
}
std::chrono::microseconds elapsed_time(std::chrono::_V2::system_clock::time_point start){
    auto end = std::chrono::high_resolution_clock::now(); // End time
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start);
}
int main(){
    auto start = std::chrono::high_resolution_clock::now(); // Start time

    define_pointer(&pointer);
    *pointer = 80;
    std::cout << "Value pointed by pointer " << *pointer << std::endl;
    std::cout << "Address stored in pointer " << pointer << std::endl;
    
    auto duration = elapsed_time(start);
    std::cout << "Execution time: " << duration.count() << " microseconds" << std::endl;

}