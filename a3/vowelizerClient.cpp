//CPSC 441 Assignment 3: TCP and UDP
//Mellisa Phongsa

//create a vowelizer program that splits text into consonants and vowels

//vowels a, e, i, o ,u
//preserve caps, punctuation and spacing
//all communication should be over TCP channel

#include <iostream>
#include <map>
#include <functional>
#include <cstdlib>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cerrno>

#define MAX_BUFFER_SIZE 4096
std::map<int, std::function<void(int)>> menuOptions;

void sendVowelsUDP(const std::string& vowels) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error Creating UDP Socket! D:" << std::endl;
        close(sockfd);
        return;
    }

    const char* serverIp = "127.0.0.1"; //change later
    int serverPort = 8080;

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    serverAddr.sin_addr.s_addr = inet_addr(serverIp);

    int sendResult = sendto(sockfd, vowels.c_str(), vowels.length(), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    if (sendResult < 0) {
        std::cerr << "Error Sending Vowel String to Server! D:" << std::endl;
        close(sockfd);
        exit(1);
    }

    close(sockfd);
}

std::string receiveVowelsUDP() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error Creating UDP Socket! D:" << std::endl;
        close(sockfd);
        return "";
    
    }
    const char* serverIp = "127.0.0.1"; //change later
    int serverPort = 8080;

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    serverAddr.sin_addr.s_addr = inet_addr(serverIp);

    char buffer[MAX_BUFFER_SIZE];
    socklen_t serverAddrLen = sizeof(serverAddr);

    int recvLen = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&serverAddr, &serverAddrLen);
    close(sockfd);

    if (recvLen > 0) {
        buffer[recvLen] = '\0';
        return std::string(buffer);
    } else {
        std::cerr << "Error Receiving Vowel String! D:" << std::endl;
        return "";
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ask for user input
void decision(int clientSocket) {
    int userInput;
    std::cout << "What do you want to do? ";
    std::cin >> userInput;
    //if input is in menuoptions call specific function
    if (menuOptions.find(userInput) != menuOptions.end()) {
        menuOptions[userInput](clientSocket);
    //else throw error message
    } else {
        std::cout << "Invalid Input! D: Try Again..." << std::endl;
        decision(clientSocket);
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//display menu options to user
void menu(int clientSocket) {
    std::string options = R"(
Options:
1. Split (Basic Encoding)
2. Merge (Basic Encoding)
3. Split (Advanced Encoding)
4. Merge (Advanced Encoding)
5. Quit
    )";
    std::cout << options << std::endl;
    decision(clientSocket);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Split (Basic) function: asks user for input string, sends string to server, wait for server to split into two strings and send to client, display strings to user
void splitBasic(int clientSocket) {
    //ask for user input
    std::string inputString;
    std::cout << "Enter a string you would like to split! (Basic Encoding): ";
    //clear previous input
    std::cin.ignore();
    //store input in variable
    std::getline(std::cin, inputString);

    inputString = "BASIC_SPLIT: " + inputString;

    //send string to server
    if (send(clientSocket, inputString.c_str(), inputString.length(), 0) == -1) {
        std::cerr << "Error Sending String to Server! D: " << strerror(errno) << std::endl;
        close(clientSocket);
        exit(1); 
    }

    char consonantsBuffer[MAX_BUFFER_SIZE];
    int consonantsRecv = recv(clientSocket, consonantsBuffer, sizeof(consonantsBuffer), 0);
    if (consonantsRecv == -1) {
        std::cerr << "Error Receiving Consonants from Server! D: " << strerror(errno) << std::endl;
        close(clientSocket);
        exit(1);
    }
    std::string consonants(consonantsBuffer, consonantsRecv);

    char vowelsBuffer[MAX_BUFFER_SIZE];
    int vowelsRecv = recv(clientSocket, vowelsBuffer, sizeof(vowelsBuffer), 0);
    if (vowelsRecv == -1) {
        std::cerr << "Error Receiving Vowels from Server! D: " << strerror(errno) << std::endl;
        close(clientSocket);
        exit(1);
    }
    std::string vowels(vowelsBuffer, vowelsRecv);

    std::cout << "Split String: " << inputString << std::endl;
    std::cout << "Consonants: " << consonants << std::endl;
    std::cout << "Vowels: " << vowels << std::endl;

    menu(clientSocket);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Split (Advanced) function: asks user for input string, sends string to server, wait for server to split into two strings and send to client, display strings to user
void splitAdvanced(int clientSocket)  {
    std::string inputString;
    std::cout << "Please enter a string you would like to split! (Advanced Encoding): ";
    //ignore previous input
    std::cin.ignore();
    //store input in variable
    std::getline(std::cin, inputString);

    inputString = "ADVANCED_SPLIT: " + inputString;

    //send string to server
    if (send(clientSocket, inputString.c_str(), inputString.length(), 0) == -1) {
        std::cerr << "Error Sending String to Server! D: " << strerror(errno) << std::endl;
        close(clientSocket);
        exit(1); 
    }

    char consonantsBuffer[MAX_BUFFER_SIZE];
    int consonantsRecv = recv(clientSocket, consonantsBuffer, sizeof(consonantsBuffer), 0);
    if (consonantsRecv == -1) {
        std::cerr << "Error Receiving Consonants from Server! D: " << strerror(errno) << std::endl;
        close(clientSocket);
        exit(1);
    }
    std::string consonants(consonantsBuffer, consonantsRecv);

    char vowelsBuffer[MAX_BUFFER_SIZE];
    int vowelsRecv = recv(clientSocket, vowelsBuffer, sizeof(vowelsBuffer), 0);
    if (vowelsRecv == -1) {
        std::cerr << "Error Receiving Vowels from Server! D: " << strerror(errno) << std::endl;
        close(clientSocket);
        exit(1);
    }
    std::string vowels(vowelsBuffer, vowelsRecv);

    std::cout << "Split String: " << inputString << std::endl;
    std::cout << "Consonants: " << consonants << std::endl;
    std::cout << "Vowels: " << vowels << std::endl;

    menu(clientSocket);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Merge (basic) function: asks user for two input strings, sends strings to server, waits for server to merge and send string, display string to user 
void mergeBasic(int clientSocket) {
    //ask for user input
    std::string consonantString, vowelString;
    std::cout << "Please enter consonant string you would like to merge! (Basic Encoding): ";
    //clear previous input
    std::cin.ignore();
    //store input in variable
    std::getline(std::cin, consonantString);

    consonantString = "BASIC_MERGE: " + consonantString;

    //ask for user input
    std::cout << "Please enter vowel (a, e, i, o, u) string you would like to merge! (Basic Encoding): ";
    //clear previous input
    std::cin.ignore();
    //store input in variable
    std::getline(std::cin, vowelString);

    vowelString = "BASIC_MERGE: " + vowelString;

    //send strings to server
    if (send(clientSocket, consonantString.c_str(), consonantString.length(), 0) == -1) {
        std::cerr << "Error Sending String to Server! D: " << strerror(errno) << std::endl;
        close(clientSocket);
        exit(1); 
    }

    sendVowelsUDP(vowelString);

    //get merged string
    char buffer[MAX_BUFFER_SIZE];
    int bytesReceived = recv(clientSocket , buffer, sizeof(buffer), 0);
    //if recv was unsuccesful
    if (bytesReceived == -1) {
        std::cerr << "Error Recieving Merged String from Server! D:" << strerror(errno) << std::endl;
        close(clientSocket);
        exit(1);
    }
    //print strings
    std::string mergedString(buffer, bytesReceived);
    std::cout << "Strings merged: (consonants)" << consonantString << " (vowels)" << vowelString << std::endl;
    std::cout << "Merged string: " << mergedString << std::endl;

    menu(clientSocket);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Merge (advanced) function: asks user for two input strings, sends strings to server, waits for server to merge and send string, display string to user 
void mergeAdvanced(int clientSocket) {
    //ask for user input
    std::string consonantString, vowelString;
    std::cout << "Please enter consonant string you would like to merge! (Basic Encoding): ";
    //clear previous input
    std::cin.ignore();
    //store input in variable
    std::getline(std::cin, consonantString);

    consonantString = "ADVANCED_MERGE: " + consonantString;

    //ask for user input
    std::cout << "Please enter vowel (a, e, i, o, u) string you would like to merge! (Basic Encoding): ";
    //clear previous input
    std::cin.ignore();
    //store input in variable
    std::getline(std::cin, vowelString);

    vowelString = "ADVANCED_MERGE: " + vowelString;

    //send strings to server
    if (send(clientSocket, consonantString.c_str(), consonantString.length(), 0) == -1) {
        std::cerr << "Error Sending String to Server! D: " << strerror(errno) << std::endl;
        close(clientSocket);
        exit(1); 
    }
    
    sendVowelsUDP(vowelString);

    //get merged string ffrom server
    char buffer[MAX_BUFFER_SIZE];
    int bytesReceived = recv(clientSocket , buffer, sizeof(buffer), 0);
    //if recv was unsuccesful
    if (bytesReceived == -1) {
        std::cerr << "Error Recieving Merged String from Server! D: " << strerror(errno) << std::endl;
        close(clientSocket);
        exit(1);
    }
    //print strings
    std::string mergedString(buffer, bytesReceived);
    std::cout << "Strings merged: (consonants)" << consonantString << " (vowels)" << vowelString << std::endl;
    std::cout << "Merged string: " << mergedString << std::endl;

    menu(clientSocket);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Quit function: close program
void quit(int clientSocket) {
    std::cout << "Quitting..." << std::endl;
    close(clientSocket);
    std::exit(0);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Error Creating TCP Socket! D:" << strerror(errno) << std::endl;
        return 1;
    } else {
        std::cout << "TCP Socket Creation was Succesful!" << std::endl;
    }

    const char* serverIp = "127.0.0.1"; //change later
    int serverPort = 8080;
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(serverPort);
    serverAddress.sin_addr.s_addr = inet_addr(serverIp);

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error Connecting to the Server: " << strerror(errno) << std::endl;
        close(clientSocket);
        return 1;
    } else {
        std::cout << "Connection to Server was Succesful!" << std::endl;
    }

    std::string banner = R"( 
Welcome To The
 ____ ____ ____ ____ ____ ____ ____ ____ ____ 
||V |||O |||W |||E |||L |||I |||Z |||E |||R ||
||__|||__|||__|||__|||__|||__|||__|||__|||__||
|/__\|/__\|/__\|/__\|/__\|/__\|/__\|/__\|/__\|
    )";
    std::cout << banner << std::endl;

    //map user input to functions
    menuOptions[1] = splitBasic;
    menuOptions[2] = mergeBasic;
    menuOptions[3] = splitAdvanced;
    menuOptions[4] = mergeAdvanced;
    menuOptions[5] = quit;
    //display menu
    menu(clientSocket);

    //close client socket
    close(clientSocket);
    std::cout << "Connection to Server was Closed!" << std::endl;
    return 0;
}
