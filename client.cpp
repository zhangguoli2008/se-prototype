#include "protocol.hpp"
#include "crypto.hpp"
#include <iostream>

void run_client() {
    std::string query;
    std::cout << "[Client] Enter keyword to search: ";
    std::cin >> query;
    std::string trapdoor = Crypto::encrypt(query);
    Protocol::send_request(trapdoor);
    Protocol::receive_response();
}
