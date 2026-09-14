#include <cstddef>
#include <curl/curl.h>
#include <curl/easy.h>
#include <iostream>
#include <string>
#include <stdexcept>




inline bool ssl_verification(std::string url){

    CURL* curl = curl_easy_init();

    if (curl == nullptr){
        throw  std::runtime_error("CURL err : curl == nullptr");
        return false;
    }


    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_PROXY, "socks5h://127.0.0.1:9050");

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    // Active la récupération des informations du certificat
    curl_easy_setopt(curl, CURLOPT_CERTINFO, 1L);

    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);  // HEAD request seulement

    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 90L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 120L);

    CURLcode res_perform = curl_easy_perform(curl);

    if (res_perform != CURLE_OK){

        curl_easy_cleanup(curl);
        return false;
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
        return false;
    }


    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_PROXY, "socks5h://127.0.0.1:9050");

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    // Active la récupération des informations du certificat
    curl_easy_setopt(curl, CURLOPT_CERTINFO, 1L);

    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);  // HEAD request seulement

    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 90L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 120L);

    CURLcode res_perform = curl_easy_perform(curl);

    if (res_perform != CURLE_OK){

        curl_easy_cleanup(curl);
        return false;
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



//fonction callback
static inline size_t manage_data(char* data, size_t size, size_t nmemb, void* userdata){

    //permet de remplire la variable de sortie avec le contenut de retour
    std::string* resultat = static_cast<std::string*>(userdata);
    resultat->append(data, size * nmemb);

    return size*nmemb;
}



inline std::string tor_curl(const std::string& url, std::string user_agent, bool redirection){

    //initialisation de curl
    CURL* curl = curl_easy_init();


    if (curl == nullptr){
        curl_easy_cleanup(curl);
        throw std::runtime_error(std::string("CURL error: \033[31m curl == nullptr \033[0m"));
    }


    //initialisation du socket5 avec l'option h pour la résolution dns
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_PROXY, "socks5h://127.0.0.1:9050");

    //deifinition de l'user-agent
    if (!user_agent.empty()){
        curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent.c_str());
    }

    //inscription de la fonction callback "manage_data" au moment de l'apelle
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, manage_data);

    //permet de récuprer le resultat de curl dans la variable resultat
    std::string resultat{nullptr};
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resultat);

    //l'option 0L est mise pour eviter les redirection tandis que le 1L lui les accepte
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
