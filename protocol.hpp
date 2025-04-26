#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <string>

namespace Protocol {
void send_request(const std::string& query);
void receive_response();
}

#endif
