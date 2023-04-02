#include <util/log.h>

static mqtt::client * client;
static mqtt::message_ptr log_msg = mqtt::make_message("log", "");

void logger::bind_client(mqtt::client& _client) {
    client = &_client;
}

void logger::log(std::string s) {
    log_msg->set_payload(s);
    client->publish(log_msg);
    fmt::println(s);
}