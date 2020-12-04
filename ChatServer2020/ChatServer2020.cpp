#include <uwebsockets/App.h>
#include <iostream>
using namespace std;


struct UserConnection
{
    string name;
    unsigned long user_id;
};

// SET_NAME=Mike
// MESSAGE_TO=11,Hello to you, mr Ivan
// MESSAGE_TO=10,Hello to you, dear Mile


int main()
{
    int port = 8888;
    unsigned long latest_user_id = 10;
    uWS::App().ws<UserConnection>("/*", {
        .open = [&latest_user_id](auto * ws) {
            UserConnection* data = (UserConnection*) ws->getUserData();
            data->user_id = latest_user_id++;
            cout << "New user connected ID =  << " << data->user_id << endl;
        },
        .message = [](auto* ws, std::string_view message, uWS::OpCode opCode) {
            cout << "New message received = " << message << endl;
        }
        }).listen(port, [port](auto* token) {
            if (token)
            {
                cout << "Server started successfully on port " << port << endl;
            }
            else
            {
                cout << "Server failed to start" << endl;
            }
        }).run();
}

