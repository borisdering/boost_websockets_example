#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/strand.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include "../include/Websocket.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

// Report a failure
void fail(beast::error_code ec, char const *what) {
    std::cerr << what << ": " << ec.message() << "\n";
}

Websocket::Session::Session(net::io_context &ioc) : m_resolver(net::make_strand(ioc)), m_ws(net::make_strand(ioc)) {}

void Websocket::Session::run(const char *host, const char *port, const char *text) {
    // Save these for later
    m_host = host;
    m_text = text;

    // Look up the domain name
    m_resolver.async_resolve(host, port, beast::bind_front_handler(&Websocket::Session::onResolve, shared_from_this()));
}

void Websocket::Session::onMessage(const std::function<void(const void*, size_t)> callback) {
    m_onMessageCallback = callback;
}

void Websocket::Session::send(const char* message, size_t length) {
    m_ws.async_write(net::buffer(message, length), beast::bind_front_handler(&Websocket::Session::onWrite, shared_from_this()));
}

void Websocket::Session::close() {
    // Close the WebSocket connection
    m_ws.async_close(websocket::close_code::normal,beast::bind_front_handler(&Websocket::Session::onClose,shared_from_this()));
}

void Websocket::Session::onResolve(beast::error_code ec, tcp::resolver::results_type results) {
    if (ec)
        return fail(ec, "resolve");

    // Set the timeout for the operation
    beast::get_lowest_layer(m_ws).expires_after(std::chrono::seconds(30));

    // Make the connection on the IP address we get from a lookup
    beast::get_lowest_layer(m_ws).async_connect(results,beast::bind_front_handler( &Websocket::Session::onConnect,shared_from_this()));
}

void Websocket::Session::onConnect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep)
{
    if(ec)
        return fail(ec, "connect");

    // Turn off the timeout on the tcp_stream, because
    // the websocket stream has its own timeout system.
    beast::get_lowest_layer(m_ws).expires_never();

    // Set suggested timeout settings for the websocket
    m_ws.set_option(
        websocket::stream_base::timeout::suggested(
                beast::role_type::client));

    // Set a decorator to change the User-Agent of the handshake
    m_ws.set_option(websocket::stream_base::decorator(
        [](websocket::request_type& req)
        {
            req.set(http::field::user_agent,
                    std::string(BOOST_BEAST_VERSION_STRING) +
                    " websocket-client-async");
        }));

    // Update the host_ string. This will provide the value of the
    // Host HTTP header during the WebSocket handshake.
    // See https://tools.ietf.org/html/rfc7230#section-5.4
    m_host += ':' + std::to_string(ep.port());

    // Perform the websocket handshake
    m_ws.async_handshake(m_host, "/",
                    beast::bind_front_handler(
                            &Websocket::Session::onHandshake,
                            shared_from_this()));
}

void Websocket::Session::onHandshake(beast::error_code ec)
{
    if(ec)
        return fail(ec, "handshake");

    // Send the message
    m_ws.async_write(net::buffer(m_text), beast::bind_front_handler(&Websocket::Session::onWrite, shared_from_this()));
}

void Websocket::Session::onWrite(beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    if(ec)
        return fail(ec, "write");

    // Read a message into our buffer
    m_ws.async_read(m_buffer, beast::bind_front_handler(&Websocket::Session::onRead, shared_from_this()));
}

void Websocket::Session::onRead(beast::error_code ec, std::size_t bytes_transferred)
{
    if(ec)
        return fail(ec, "read");

    void* message = m_buffer.data().data();
    size_t size = m_buffer.size();
    m_buffer.consume(bytes_transferred);

    if (m_onMessageCallback)
        m_onMessageCallback(message, bytes_transferred);

    // read the actual message and notify subscribers
    m_ws.async_read(m_buffer, beast::bind_front_handler(&Websocket::Session::onRead, shared_from_this()));
}

void Websocket::Session::onClose(beast::error_code ec)
{
    if(ec)
        return fail(ec, "close");

    std::cout << "ON CLOSE" << std::endl;

    // If we get here then the connection is closed gracefully

    // The make_printable() function helps print a ConstBufferSequence
    std::cout << beast::make_printable(m_buffer.data()) << std::endl;
}
