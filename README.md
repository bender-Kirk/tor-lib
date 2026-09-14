# tor-lib

`tor-lib` is a C++ library that can connect to the Tor network and create hidden services.

## Installation

Clone the repository and run the installation script:

```bash
git clone https://github.com/bender-Kirk/tor-lib
cd tor-lib
sudo bash install.sh
```
## Exemples : 

```cpp
#include "tor_sock.h"

int main(){

    std::string url = "http://2gzyxa5ihm7nsggfxnu52rck2vv4rvmdlkiu3zzui5du4xyclen53wid.onion/";
    std::string res = tor_curl(url, "", true);
    std::cout << res << std::endl;

    return 0;
}
``
