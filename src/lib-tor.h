#include <curl/curl.h>
#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <thread>
#include <chrono>




inline bool ssl_verification(std::string url){

    CURL* curl = curl_easy_init();

    if (curl == nullptr){
        throw  std::runtime_error("CURL err : curl == nullptr");
    }


    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_PROXY, "socks5h://127.0.0.1:9050");

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    // Enable certificate information retrieval
    curl_easy_setopt(curl, CURLOPT_CERTINFO, 1L);

    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);  // HEAD request only

    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 90L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 120L);

    CURLcode res_perform = curl_easy_perform(curl);

    if (res_perform != CURLE_OK){

        curl_easy_cleanup(curl);
        throw std::runtime_error("CURL error : curl != CURLE_OK");
    }   

    char* scheme = nullptr;
    
    struct curl_certinfo* certinfo = nullptr;

    if (curl_easy_getinfo(curl, CURLINFO_CERTINFO, &certinfo) == CURLE_OK &&
        certinfo != nullptr &&
        certinfo->num_of_certs > 0)
    {
        curl_easy_cleanup(curl);
        return true;
    }

    curl_easy_cleanup(curl);
    return false;

}



inline bool https_verification(std::string url){

    CURL* curl = curl_easy_init();

    if (curl == nullptr){
        throw std::runtime_error("CURL error : curl == nullptr");
    }


    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_PROXY, "socks5h://127.0.0.1:9050");

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    // Enable certificate information retrieval
    curl_easy_setopt(curl, CURLOPT_CERTINFO, 1L);

    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);  // HEAD request only

    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 90L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 120L);

    CURLcode res_perform = curl_easy_perform(curl);

    if (res_perform != CURLE_OK){

        curl_easy_cleanup(curl);
        throw std::runtime_error("CURL error : curl != CURLE_OK");
    }   

    char* scheme = nullptr;
    
    CURLcode res = curl_easy_getinfo(curl, CURLINFO_SCHEME, &scheme);

    if (res == CURLE_OK && scheme != nullptr){

        if (std::string(scheme) == "HTTPS" || std::string(scheme) == "https"){

            curl_easy_cleanup(curl);
            return true;
            
        }

    }

    curl_easy_cleanup(curl);
    return false;

}



// Callback function
static inline size_t manage_data(char* data, size_t size, size_t nmemb, void* userdata){

    // Fill the output variable with the return content
    std::string* resultat = static_cast<std::string*>(userdata);
    resultat->append(data, size * nmemb);

    return size*nmemb;
}



inline std::string tor_curl(const std::string& url, std::string user_agent, bool redirection){

    // Initialize curl
    CURL* curl = curl_easy_init();


    if (curl == nullptr){
        throw std::runtime_error(std::string("CURL error: \033[31m curl == nullptr \033[0m"));
    }


    // Initialize SOCKS5 with 'h' option for DNS resolution
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_PROXY, "socks5h://127.0.0.1:9050");

    // Define user-agent
    if (!user_agent.empty()){
        curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent.c_str());
    }

    // Register callback function "manage_data" on call
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, manage_data);

    // Retrieve curl result in the result variable
    std::string resultat{""};
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resultat);

    // 0L option is set to avoid redirects, while 1L accepts them
    if (redirection == false){

        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0L); 
    }else{

        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    } 

    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30L);

    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 120L);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK){
        throw std::runtime_error(std::string("CURL error: ") + curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        return "err";
    }

    curl_easy_cleanup(curl);

    return resultat;
}




inline bool creat_service(std::string name, std::string public_port, std::string local_port){

    // Create and configure directories related to hidden service


    std::string command_hidden_service = "sudo mkdir -p /var/lib/tor/" + name + " && ";
    command_hidden_service += "sudo chown debian-tor:debian-tor /var/lib/tor/" + name + " && ";
    command_hidden_service += "sudo chmod 700 /var/lib/tor/" + name;

    if (std::system(command_hidden_service.c_str()) != 0 ){
        std::cerr << "File creation was not completed\n";
        return false;
    }

    // Modify the /etc/tor/torrc configuration file to configure the hidden service
    std::ofstream torcc("/etc/tor/torrc", std::ios::app);


    if (!torcc.is_open()){
        std::cerr << "torrc file could not be opened\n";
        return false;
    }

    std::string command_torcc = "\nHiddenServiceDir /var/lib/tor/" + name + "\nHiddenServicePort " + public_port + " 127.0.0.1:" + local_port + "\n";
    torcc.write(command_torcc.c_str(), command_torcc.size());

    torcc.close();

    if (std::system("sudo systemctl restart tor") != 0 || std::system("sudo systemctl is-active --quiet tor") != 0){
        std::cerr << "Tor failed to restart\n";
        return false;
    }


    // We need this wait time, otherwise we read the file before it is created
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::ifstream hostname("/var/lib/tor/" + name + "/hostname");

    if (!hostname.is_open()){
        std::cerr << "File could not be opened\n" << "/var/lib/tor/" + name +"/hostname\n";
        return false;
    }

    std::string content = "";
    if(!std::getline(hostname, content)){
        std::cerr << "Unable to read file\n";
        return false;
    }


    std::cout <<"onion : " << content << std::endl;

    return true;
}
