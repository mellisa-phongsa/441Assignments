#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <thread>

//design a web proxy using HTTP
//Goals: build a web proxy for simple web pages, and then use the proxy to alter certian content items before they are delivered to the browser
//two main functionalities: 
    //1. handle HTTP requests and responses by forwarding them between client and server (transparent proxy)
    //2. parse and modify HTTP requests and responses
        //(a). rewriting content in HTTP responses before displaying to web browser
            //- parse, understand and return to the client a possibly modified version of the HTTP response that the web server provides to the proxy
        //(b). rewriting content in HTTP requests so that they request a different object that it is normally retrieved from the web page
            //- parse, understand and forward to the web server a possibly modified version of the client HTTP request 
//Your proxy should be able to do two things:
    //1. replace all occurences of the word "frog" (regardless of caps) with the word "fred" in an HTTP response
        //- parse HTTP response, detect the word "frog" and replace with "fred"
    //2. replace all JPG image files on a given web page with an image of a frog instead (any frog photo you like)
        //- parse HTTP requests, identify what content is being requested, and substitute the original image with a frog image if a JPG file is requested
//Most important commmand for your web proxy to handle is the "GET" request, which specifies the URL for an object to be retrieved (Using either HTTP/1.0 or HTTP/1.1)
    //proxy should also handle:
        //- 200 (OK)
        //- 206 (Partial Content)
        //- 301 (Moved Permanently)
        //- 302 (Found)
        //- 304 (Not Modified)
        //- 403 (Forbidden)
        //- 404 (Not Found)
//need at least one TCP socket for client-proxy communication and at least one additional TCP socket for each web server that your proxy talks to during proxy-server communication

#define PROXY_PORT 8080
#define MAX_BUFFER_SIZE 8192
#define MAX_URL_LENGTH 1024

void handleResponse() {
    char buffer[MAX_BUFFER_SIZE];
    int totalBytes = 0;

}

void handleRequest(int clientSocket, int webServerSocket) {
    char buffer[MAX_BUFFER_SIZE];
    int httpRequest;

    httpRequest = recv(clientSocket, buffer, MAX_BUFFER_SIZE, 0);

    if (httpRequest <= 0) {

    } else {
        int sendRequest = send(webServerSocket, buffer, httpRequest, 0);
        if (sendRequest == -1) {
            //Sending request to web server failed
        }
    }
}

//FUNCTION: create additional TCP sockets for each web server for porxy-server communication
/*1. Read client HTTP requests from clientSocket
 *2. GET the url from client request
 *3. get hostname from URL
 *4. resolve hostname to an IP address
 *5. create web server socket 
 *6. connect to web server
 *7. call function(s) to handle requests and responses
 *8. close sockets 
*/
void initWebServer(int clientSocket) {
    char buffer[MAX_BUFFER_SIZE];
    memset(buffer, 0, MAX_BUFFER_SIZE);

    //Step 1: read request from client socket into a buffer
    int httpRequest = recv(clientSocket, buffer, MAX_BUFFER_SIZE, 0);
    if (httpRequest <= 0) {
        //request empty, and or catch error
        close(clientSocket);
        return;
    }

    //Step 2:
    //create a string called httpString using buffer (char array)
    std::string httpString(buffer);
    std::string url;

    size_t start = httpString.find("GET ");
    if (start != std::string::npos) {
        size_t end1 = httpString.find(" HTTP/1.1", start);
        size_t end2 = httpString.find(" HTTP/1.0", start);
        if (end1 != std::string::npos) {
            url = httpString.substr(start + 4, end1 - (start +4));
        } else if (end2 != std::string::npos) {
            url = httpString.substr(start + 4, end2 - (start +4));
        }
    }

    if (url.empty()) {
        //url empty, and or catch error
        close(clientSocket);
        return;
    }

    //Step 3:
    std::string hostname;
    if (url.find("HTTP://") == 0) {
        size_t hostStart = url.find("HTTP://") + 7;
        size_t hostEnd = url.find("/", hostStart);
        if (hostEnd != std::string::npos) {
            hostname = url.substr(hostStart, hostEnd - hostStart);
        }
    }

    if (hostname.empty()) {
        //hostname empty and or catch error
        close(clientSocket);
        return;
    }

    //Step 4:
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int resolveResult = getaddrinfo(hostname.c_str(), "80", &hints, &res);
    if (resolveResult != 0) {
        //handle host name resolution error
        close(clientSocket);
        return;
    }

    //Step 5:
    int webServerSocket;
    struct sockaddr_in webServerAddr;
    
    webServerSocket = socket(AF_INET, SOCK_STREAM, 0);
    webServerAddr.sin_family = AF_INET;
    webServerAddr.sin_port = htons(80);
    webServerAddr.sin_addr = ((struct sockaddr_in *)(res->ai_addr))->sin_addr;

    //Step 6:
    if (connect(webServerSocket, (struct sockaddr*)&webServerAddr, sizeof(webServerAddr)) == -1) {
        perror("Oh No! There was an Error Connecting to Web Server! D:");
        close(clientSocket);
        close(webServerSocket);
        return;
    }

    //Step 7:
    handleRequest(clientSocket, webServerSocket);

    //Step 8:
    close(clientSocket);
    close(webServerSocket);

    freeaddrinfo(res);
    
}

//FUNCTION: create a TCP socket for client-proxy communication and additional sockets for each web server
/*1. create a socket for the proxy server
 *2. set up proxy's address info, including IP address and port number, and bind the socket to this address  
 *3. listen for incoming connections on the specified port
 *4. accept client connections and create a separate socket for communication with the client
*/
void initProxyServer() {
    int proxySocket, clientSocket;
    struct sockaddr_in proxyAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    //Step 1:
    proxySocket = socket(AF_INET, SOCK_STREAM, 0);
    if (proxySocket == -1) {
        perror("Oh No! There was an Error Creating the Socket! D:");
        exit(1);
    }

    //Step 2:
    proxyAddr.sin_family = AF_INET;
    proxyAddr.sin_port = htons(PROXY_PORT);
    proxyAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(proxySocket, (struct sockaddr*)&proxyAddr, sizeof(proxyAddr)) == -1) {
        perror("Oh No! There was an Error Binding the Socket! D:");
        exit(1);
    }
    //Step 3:
    if (listen(proxySocket, 10) == -1) {
        perror("Oh No! There was an Error Listening for Incoming Connections! D:");
        exit(1);
    }

    //Print message to show that socket creating and binding was successful!
    std::cout << "The Proxy Server is running and listening on Port " << PROXY_PORT << std::endl;

    //Step 4:
    while (true) {
        clientSocket = accept(proxySocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == -1) {
            perror("Oh No! There was an Error Acception Client Connection! D:");
            continue;
        }

        std::thread(initWebServer, clientSocket).detach();
        
        close(clientSocket);
    }
    close(proxySocket);
}

//Function(s): HTTP Request handling and status codes
    //recieve http requests
    //parse and understand requests, extract the requested url
    //modify request (replace JPG image with a frog photo)

    //Handle common HTTP status codes (e.g., 301, 302, 404, 200) and act accordingly
    //Handle redirections by updating the client's request with the new URL

//Function(s): Web Server communication 
    //forwarding requests: 
    //open a new socket to connect to the target web server based on the url extracted from request
    //forward the clients HTTP request to the web server

    //recieving and parsing responses:
    //recieve http response from web server
    //parse and understand responses, including status codes
    //modify responses (replace "frog" with "fred")

    //forwarding response:
    //Forward the modified or unmodified response back to the client

//Main Function

