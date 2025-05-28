#include "json.hpp"
using json=nlohmann::json;
#include <iostream>
#include <vector>
#include <string>

using namespace std;

namespace test{
    string method1(){
        json js;
        js["id"]={1,2,3,4,5};
        js["msg"]["from"]="A";
        js["msg"]["to"]="B";
        js["msg"]["content"]="hello ubuntu!";
        return js.dump();
    }
}

int main(){
    cout<<test::method1()<<endl;
    return 0;
}