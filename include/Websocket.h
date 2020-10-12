//
// Created by Boris Dering on 23.09.20.
//

#ifndef BOOST_WEBSOCKETS_EXAMPLE_WEBSOCKET_H
#define BOOST_WEBSOCKETS_EXAMPLE_WEBSOCKET_H

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

namespace Websocket {

    // Sends a WebSocket message and prints the response
    class Session : public std::enable_shared_from_this<Session>
    {
    private:
        tcp::resolver m_resolver;
        websocket::stream<beast::tcp_stream> m_ws;
        beast::flat_buffer m_buffer;
        std::string m_host;
        std::string m_text;

        std::function<void(const void *, const void*, size_t size)> m_onmessage_callback;

        void on_resolve(beast::error_code ec, tcp::resolver::results_type results);

        void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep);

        void on_handshake(beast::error_code ec);

        void on_write(beast::error_code ec, std::size_t bytes_transferred);

        void on_read(beast::error_code ec, std::size_t bytes_transferred);

        void on_close(beast::error_code ec);

    public:
        // Resolver and socket require an io_context
        explicit Session(net::io_context& ioc);

        /**
         * Run actual websocket.
         * @param host
         * @param port
         * @param text
         */
        void run(char const* host, char const* port, char const* text);

        /**
         * Register on message callback.
         * @param callback
         */
        void on_message(const std::function<void(const void*, const void*, size_t)> callback);

        /**
         * Send message over websockets.
         * @param message
         * @param length
         */
        void send(const char* message, size_t length);

        /**
         * Closes websocket connection.
         */
        void close();
    };
}

#endif //BOOST_WEBSOCKETS_EXAMPLE_WEBSOCKET_H
