#include <iostream>
#include <cstdlib>
#include <memory>
#include <boost/program_options.hpp>
#include "include/Websocket.h"

namespace asio = boost::asio;
namespace po = boost::program_options;

void on_message(const void* session, const void* message, size_t size) {
    std::cout << (char*) message << std::endl;
}

int main(int argc, char **argv) {

    std::string host;
    int port;
    std::string text;

    po::options_description options_description("Allowed options");
    options_description.add_options()
            ("help", "shows help")
            ("host", po::value<std::string>(&host)->default_value("127.0.0.1"), "websockets host e.g. 127.0.0.1 by default")
            ("port", po::value<int>(&port)->default_value(8080), "websockets port e.g 8080 by default")
            ("message", po::value<std::string>(&text)->default_value("hello world!"), "text to use for the first message e.g hello world! by default");

    po::variables_map options_map;
    po::store(po::parse_command_line(argc, argv, options_description), options_map);
    po::notify(options_map);


    // The io_context is required for all I/O
    asio::io_context ioc;

    // Launch the asynchronous operation
    std::shared_ptr<Websocket::Session> websocket = std::make_shared<Websocket::Session>(ioc);
    websocket->run(host.c_str(), std::to_string(port).c_str(), text.c_str());

    websocket->on_message(on_message);

    // Run the I/O service. The call will return when
    // the socket is closed.
    ioc.run();

    return EXIT_SUCCESS;
}
