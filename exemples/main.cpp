#include "tor_sock.h"


int main(){

    std::string url = "http://2gzyxa5ihm7nsggfxnu52rck2vv4rvmdlkiu3zzui5du4xyclen53wid.onion/";
    std::string res = tor_curl(url, "", true);
    std::cout << res << std::endl;

    return 0;
}
