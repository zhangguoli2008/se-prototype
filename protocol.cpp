#include "protocol.hpp"
#include <iostream>

namespace Protocol {
void send_request(const std::string& query) {
    std::cout << "[Client] Sending search request: " << query << "\n";
}

void receive_response() {
    std::cout << "[Client] Receiving search results...\n";
}
}
