#include <uwebsockets/App.h>
#include <iostream>
#include <regex>

using namespace std;



const unsigned long FIRST_ID = 10;
const unsigned long BOT_ID = 1;


struct UserConnection
{
    string name;
    unsigned long user_id;
};

// Client => Server
// SET_NAME=Mike
// MESSAGE_TO=11,Hello to you, mr Ivan
// MESSAGE_TO=10,Hello to you, dear Mile


// Server => Client
// NEW_USER=19,Mike


//TASK 2,3
bool good_name(string name)
{
    if (name.find(",") != string::npos) {
        cout << name.find(",") << name << endl;
        return false;
    }
    if (name.length() > 255) {
        cout << "255" << endl;
        return false;
    }
    return true;
}

//TASK4
bool exist_id(unsigned long latest_id, unsigned long id) {
    return (id < latest_id) and (id >= FIRST_ID) or (id == BOT_ID);
}

void newUserConnected(uWS::WebSocket<false,true>* ws, string name ,unsigned long user_id)
{
    string_view message("NEW_USER" + to_string(user_id) + "," + name);
    ws->publish("broadcast", message);
}


int main()
{
    int port = 8888;
    unsigned long latest_user_id = FIRST_ID;
    unsigned long total_connected = 0;
    map<unsigned long, string> online_users;

    uWS::App().ws<UserConnection>("/*", {
        .open = [&latest_user_id, &total_connected, &online_users](auto * ws) {
            UserConnection* data = (UserConnection*) ws->getUserData();
            data->user_id = latest_user_id++;
            data->name = "UNNAMED";
            cout << "New user connected ID = " << data->user_id << endl;
            online_users[data->user_id] = data->name;
            ws->subscribe("broadcast");
            ws->subscribe("user#" + to_string(data->user_id));
            //TASK 1
            cout << "Total user connected: " << ++ total_connected << endl;
        },
        .message = [&latest_user_id, &online_users](auto* ws, std::string_view message, uWS::OpCode opCode) {
            UserConnection* data = (UserConnection*)ws->getUserData();
            const auto& input = string(message);

            const regex SET_NAME("(SET_NAME)=(.*)");
            const regex MESSAGE_TO("(MESSAGE_TO)=(\\d+),(.*)");

            smatch match_result;

            cout << "New message received = " << input << endl;
       
            if (regex_match(input,match_result, SET_NAME))
            {
                string new_name = match_result[2].str();
                string user_id = to_string(data->user_id);
                if (good_name(new_name))
                {
                    data->name = new_name;
                    ws->publish("broadcast", "Welcome: " + new_name);
                    cout << "User" << user_id << " sets their name to " << new_name << endl;
                    ws->publish("broadcast", "NEW_USER = " + user_id + "," + data->name);
                    online_users[data->user_id] = data->name;
                }
                else
                {
                    cout << "User" << user_id << " tried to use unallowed name: " << new_name << endl;
                    ws->publish("user#" + user_id, "Sorry, you can't use this name, try different one!");
                }
            }
            if (regex_match(input, match_result, MESSAGE_TO))
            {
                string id_to = match_result[2].str();
                string message_to_send = match_result[3].str();
                string user_id = to_string(data->user_id);
                if (exist_id(latest_user_id,std::stoul(id_to)))
                {
                    ws->publish("user#" + id_to, message_to_send);
                    cout << "User" << user_id << " seends private message to User" << id_to << endl;
                }
                else
                {
                    ws->publish("user#" + user_id, "Error, there is no user with ID = " + id_to);
                    cout << "User" << data->user_id << " tried to send message to unexisting ID = " << id_to << endl;
                }
                
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

