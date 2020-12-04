#include <uwebsockets/App.h>
#include <iostream>
#include <regex>

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
            data->name = "UNNAMED";
            cout << "New user connected ID =  << " << data->user_id << endl;
            ws->subscribe("broadcast");
            ws->subscribe("user#" + data->user_id);
        },
        .message = [](auto* ws, std::string_view message, uWS::OpCode opCode) {
            string SET_NAME("SET_NAME=");
            string MESSAGE_TO("MESSAGE_TO=");
            UserConnection* data = (UserConnection*)ws->getUserData();
            cout << "New message received = " << message << endl;
            if (message.find(SET_NAME)==0)
            {
                cout << "User sets their name" << endl;
                data->name = message.substr(SET_NAME.length());
            }
            if (message.find("MESSAGE_TO=") == 0)
            {
                cout << "User sends private message" << endl;
                auto rest = message.substr(MESSAGE_TO.length());
                int comma_position = rest.find(",");
                auto ID = rest.substr(0, comma_position);
                auto text = rest.substr(comma_position+1);
                ws->publish("user#" + string(ID), text);
            }
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

