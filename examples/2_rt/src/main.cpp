#include "app.hpp"

#include <iostream>

int main() {
    App app;
    int return_code = app.run();
    std::cout << "app finished with value " << return_code << std::endl;
}
