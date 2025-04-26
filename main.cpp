#include <iostream>

extern void run_client();
extern void run_server();

int main() {
    std::cout << "Run as (c)lient or (s)erver? ";
    char role;
    std::cin >> role;

    if (role == 'c') {
        run_client();
    } else if (role == 's') {
        run_server();
    } else {
        std::cout << "Invalid role selected. Exiting.\n";
    }
    return 0;
}
