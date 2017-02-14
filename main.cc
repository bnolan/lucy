#include <memory>
#include "node.h"
#include "script.h"
#include "vendor/sole.hpp"
#include "vendor/server_ws.hpp"

using namespace std;

typedef SimpleWeb::SocketServer<SimpleWeb::WS> WsServer;

std::string generateUUID () {
  return sole::uuid4().str();
}

Node::ptr world(new Node("a-entity"));

void startServer () {
    WsServer server;

    server.config.port=8080;
    
    //Example 1: echo WebSocket endpoint
    //  Added debug messages for example use of the callbacks
    //  Test with the following JavaScript:
    //    var ws=new WebSocket("ws://localhost:8080/echo");
    //    ws.onmessage=function(evt){console.log(evt.data);};
    //    ws.send("test");
    auto& echo=server.endpoint["^/lucy/?$"];
    
    echo.on_message=[&server](shared_ptr<WsServer::Connection> connection, shared_ptr<WsServer::Message> message) {
        //WsServer::Message::string() is a convenience function for:
        //stringstream data_ss;
        //data_ss << message->rdbuf();
        //auto message_str = data_ss.str();
        auto message_str=message->string();
        
        startScript(world, message_str);

        cout << "Server: Message received: \"" << message_str << "\" from " << (size_t)connection.get() << endl;
                
        // cout << "Server: Sending message \"" << message_str <<  "\" to " << (size_t)connection.get() << endl;
        
        // auto send_stream=make_shared<WsServer::SendStream>();
        // *send_stream << message_str;
        // //server.send is an asynchronous function
        // server.send(connection, send_stream, [](const boost::system::error_code& ec){
        //     if(ec) {
        //         cout << "Server: Error sending message. " <<
        //         //See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
        //                 "Error: " << ec << ", error message: " << ec.message() << endl;
        //     }
        // });
    };
    
    echo.on_open=[&server](shared_ptr<WsServer::Connection> connection) {
        cout << "Server: Opened connection " << (size_t)connection.get() << endl;

        auto str=make_shared<WsServer::SendStream>();
        *str << "hi2ulolol\n";

        server.send(connection, str);
    };
    
    //See RFC 6455 7.4.1. for status codes
    echo.on_close=[](shared_ptr<WsServer::Connection> connection, int status, const string& /*reason*/) {
        cout << "Server: Closed connection " << (size_t)connection.get() << " with status code " << status << endl;
    };
    
    //See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
    echo.on_error=[](shared_ptr<WsServer::Connection> connection, const boost::system::error_code& ec) {
        cout << "Server: Error in connection " << (size_t)connection.get() << ". " << 
                "Error: " << ec << ", error message: " << ec.message() << endl;
    };

    thread server_thread([&server](){
        std::cout << "Listening on 0.0.0.0:8080\n";

        //Start WS-server
        server.start();
    });

    while (true) {
        for (auto a_connection: server.get_connections()) {
            auto send_stream=make_shared<WsServer::SendStream>();
            *send_stream << world->toString();

            //server.send is an asynchronous function
            server.send(a_connection, send_stream);
        }

        tickScript();

        this_thread::sleep_for(chrono::milliseconds(200));
    }

    server_thread.join();

}

int main () {
  world->setAttribute("uuid", generateUUID());

  Node::ptr a(new Node("a-box"));
  a->setAttribute("uuid", generateUUID());
  a->setAttribute("position", "1 2 -10");
  world->appendChild(a);

  Node::ptr b(new Node("a-box"));
  b->setAttribute("uuid", generateUUID());
  b->setAttribute("position", "3 2 -10");
  world->appendChild(b);

  Node::ptr c(new Node("a-box"));
  c->setAttribute("uuid", generateUUID());
  c->setAttribute("position", "6 2 -10");
  world->appendChild(c);

  // std::cout << "Scene state:\n\n" << world->toString() << "\n\n";

  startScript(world, "");
  startServer();
}

