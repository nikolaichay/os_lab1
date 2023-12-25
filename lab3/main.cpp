#include "Tester.h"

int main() {
    Tester tester;

    std::cout << "Write test:\t";
    if (tester.testWrite()) {
        std::cout << "success";
    }
    else {
        std::cout << "fail";
    }
    std::cout << std::endl;

    std::cout << "Read test:\t";
    if (tester.testRead()) {
        std::cout << "success";
    }
    else {
        std::cout << "fail";
    }
    std::cout << std::endl;

    std::cout << "Common test:\t";
    if (tester.testCommonSpeed()) {
        std::cout << "success";
    }
    else {
        std::cout << "fail";
    }
    std::cout << std::endl;

    std::cout << "Speed test:" << std::endl;
    if (tester.testSpeed()) {
        std::cout << "success";
    }
    else {
        std::cout << "fail";
    }
    std::cout << std::endl;

    return 0;
}