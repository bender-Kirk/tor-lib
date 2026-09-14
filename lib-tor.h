#include <curl/curl.h>
#include <curl/easy.h>
#include <iostream>
#include <string>
using namespace std;




bool ssl_verification(string url){

    CURL* curl = curl_easy_init();

    if (curl == nullptr){
        curl_easy_cleanup(curl);
        return false;
    }


    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_PROXY, "socks5h://127.0.0.1:9050");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);  // HEAD request seulement
    
    CURLcode res = curl_easy_perform(curl);

    curl_easy_cleanup(curl);
    return res == CURLE_OK;

}



//fonction callback
static size_t manage_data(char* data, size_t size, size_t nmemb, void* userdata){

    //permet de remplire la variable de sortie avec le contenut de retour
    string* resultat = static_cast<string*>(userdata);
    resultat->append(data, size * nmemb);

    return size*nmemb;
}



string tor_curl(const string& url, string user_agent, bool redirection){

    //initialisation de curl
    CURL* curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);


    if (curl == nullptr){
        curl_easy_cleanup(curl);
        return "err : curl = nullptr";
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
    string resultat{""};
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
        cerr << "code d'erreur : " << curl_easy_strerror(res) << endl;
        curl_easy_cleanup(curl);
        return "err";
    }

    curl_easy_cleanup(curl);
    return resultat;
}
