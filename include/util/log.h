#define FMT_HEADER_ONLY
#include <string>
#include <fmt/format.h>
#include <mqtt/client.h>

#define LOGD(str, ...) logger::log(fmt::format(str, ##__VA_ARGS__))

namespace logger {
    void bind_client(mqtt::client &client);

    void log(std::string s);
}