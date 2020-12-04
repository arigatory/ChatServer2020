#include <uwebsockets/App.h>
#include <iostream>
using namespace std;


struct UserConnection
{
    string name;
    unsigned long user_id;
};

int main()
{
    uWS::App().ws<UserConnection>("/*", {
        //settings
    });
}
