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
#include <cctype>
#define MAX_BUFFER_SIZE 4096

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string parseMessage(const std::string& message, const std::string& prefix) {
    size_t prefixPosition = message.find(prefix);
    if (prefixPosition != std::string::npos) {
        //Extract the substring after the prefix
        return message.substr(prefixPosition + prefix.length());
    }
    return "";
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void sendVowelsUDP(const std::string& vowels) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error Creating UDP Socket! D:" << strerror(errno) << std::endl;
        close(sockfd);
        return;
    }

    int serverPort = 8080;

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Error Binding UDP Socket! D:" << strerror(errno) << std::endl;
        close(sockfd);
        return;
    }  

    int sendResult = sendto(sockfd, vowels.c_str(), vowels.length(), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    if (sendResult < 0) {
        std::cerr << "Error Sending Vowel String to Server! D:" << strerror(errno) << std::endl;
        close(sockfd);
        exit(1);
    }

    close(sockfd);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string receiveVowelsUDP(int port, const std::string& prefix) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error Creating UDP Socket! D:" << std::endl;
        close(sockfd);
        return "";
    }

    struct sockaddr_in serverAddr, clientAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    memset(&clientAddr, 0, sizeof(clientAddr));

    serverAddr.sin_family = AF_INET;
    //Accept connections from any IP
    serverAddr.sin_addr.s_addr = INADDR_ANY; 
    serverAddr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Error Binding UDP Socket! D:" << strerror(errno) << std::endl;
        close(sockfd);
        return "";
    }

    char buffer[MAX_BUFFER_SIZE];
    socklen_t clientAddrLen = sizeof(clientAddr);

    int recvLen = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&clientAddr, &clientAddrLen);
    close(sockfd);

    if (recvLen > 0) {
        buffer[recvLen] = '\0';
        std::string receivedString(buffer);
        std::string parsedString = parseMessage(receivedString, prefix);
        return parsedString;
    } else {
        std::cerr << "Error Receiving Vowel String! D:" << strerror(errno) << std::endl;
        return "";
    }

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//split basic encoding
void splitBasic(int clientSocket, const std::string& splitString) {
    //strings to store consonants and vowels
    std::string consonants;
    std::string vowels;
    //boolean value to see if prev char was a vowel so that the consonant/vowel string uses spaces as place holders
    //so that both strings have the same number of chars
    bool prevIsVowel = false;
    //for every char int string sent from the client
    for (char ch : splitString) {
        //Hello World!
        //is the char in the alphabet
        if (isalpha(ch)) {
            //VOWELS
            //"_e__o__o____" 12 chars
            if (ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u' ||
                ch == 'A' || ch == 'E' || ch == 'I' || ch == 'O' || ch == 'U') {
                //add char to vowel string
                vowels += ch;
                //set boolean to true for next char
                prevIsVowel = true;
            //CONSONANTS
            //"H_ll_ W_rld!" 12 chars
            } else {
                //if the prev char was a vowel
                if(prevIsVowel = true) {
                    //add a space as place holder
                    consonants += ' ';
                } else {
                    //if the prev was not a vowel add space as place holder in vowel string
                    vowels += ' ';
                }
                //add char to consonants string
                consonants += ch;
                //set boolean to false
                prevIsVowel = false;
            }
        //PUNCTUATION
        } else {
            //add place holders to correct string
            if (prevIsVowel = true) {
                consonants += ' ';
            } else {
                vowels += ' ';
            }
            //add non-alphabet char to consonants string
            consonants += ch;
            //set boolean to false
            prevIsVowel = false;
        }
    }
    //print strings
    std::cout << "Consonants: " << consonants << std::endl;
    std::cout << "Vowels: " << vowels << std::endl;
    //send consonants
    if (send(clientSocket, consonants.c_str(), consonants.length(), 0) == -1) {
        //error message if send was unsuccessful
        std::cerr << "Error Sending Consonant String to Server! D:" << strerror(errno) << std::endl;
        //close connection
        close(clientSocket);
        exit(1); 
    }

    //send vowels
    if (send(clientSocket, vowels.c_str(), vowels.length(), 0) == -1) {
        //error message if send was unsuccessful
        std::cerr << "Error Sending Vowel String to Server! D:" << strerror(errno) << std::endl;
        //close connection
        close(clientSocket);
        exit(1); 
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//split advanced encoding
void splitAdvanced(int clientSocket, const std::string& splitString) {
    //split strings
    std::string consonants;
    std::string vowels;
    //count how many chars to skip to place next vowel in merge operation
    int skipCount = 0;
    for (char ch : splitString) {
        //Hello World!
        if (isalpha(ch)) {
            //VOWELS
            //"1e2o2o"
            if (ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u' || 
                ch == 'A' || ch == 'E' || ch == 'I' || ch == 'O' || ch == 'U') {
                //add counter to string
                vowels += std::to_string(skipCount);
                //add char to string
                vowels += ch;
                //set counter back to zero
                skipCount = 0;   
            //CONSONANTS
            //"Hll Wrld!"
            } else {
                //add char to string
                consonants += ch;
                //increase counter
                skipCount ++;
            }
        //PUNCTUATION
        } else {
            //add char to string
            consonants += ch;
            //increase counter
            skipCount ++;
        }
    }
    //print strings
    std::cout << "Consonants: " << consonants << std::endl;
    std::cout << "Vowels: " << vowels << std::endl;
    //send consonants
    if (send(clientSocket, consonants.c_str(), consonants.length(), 0) == -1) {
        //error message
        std::cerr << "Error Sending String to Client! D:" << strerror(errno) << std::endl;
        close(clientSocket);
        exit(1); 
    }
    //send vowels
    if (send(clientSocket, vowels.c_str(), vowels.length(), 0) == -1) {
        //error message if send was unsuccessful
        std::cerr << "Error Sending Vowel String to Server! D:" << strerror(errno) << std::endl;
        //close connection
        close(clientSocket);
        exit(1); 
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//merge basic encoding
void mergeBasic(int clientSocket, const std::string& consonants, const std::string& vowels) {
    //make string
    std::string mergedString;
    //reserve space for string
    mergedString.reserve(consonants.length());

    //for the length of the consonants string
    for (size_t i = 0; i < consonants.length(); i++) {
        //if char at index i in consonants string is a space and i is within the range of the vowel string
        if (consonants[i] == ' ' && i < vowels.length()) {
            //add vowel at index i in vowel string to merged string
            mergedString += vowels[i];
        } else {
            //add char at index i in consonant string to merged string
            mergedString += consonants[i];
        }
    }

    //print merged string
    std::cout << "Merged String: " << mergedString << std::endl;
    //send merged string to client
    if (send(clientSocket, mergedString.c_str(), mergedString.length(), 0) == -1) {
        //error message
        std::cerr << "Error sending merged string to Client! D:" << strerror(errno) << std::endl;
        //close connection
        close(clientSocket);
        exit(1);
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//merge advanced encoding
void mergeAdvanced(int clientSocket, const std::string& consonants, const std::string& vowels) {
    //set merged string to be the consonants string
    std::string mergedString = consonants;
    //counters
    //position of where the vowel should be inserted into the string
    size_t consonantsIndex = 0;
    size_t skipCount = 0;

    //for every char in vowels string
    for (char ch : vowels) {
        //if char is a digit
        if (isdigit(ch)) {
            //converts char representing a digit into corresponding int value
            //ex. ch = '5' ascii value = 53, '0' ascii value = 48
            //53 - 48 = 5
            //converts char to number of chars to skip when merging strings
            skipCount = ch - '0';
        } else {
            //while skip count is greater than 0 and consonants index is less than the length of the merged string
            while (skipCount > 0 && consonantsIndex < mergedString.length()) {
                //increment consonantsIndex
                consonantsIndex++;
                //decrement skipCount
                skipCount--;
            }
            //if consonants index is less than the lenght of the string
            if (consonantsIndex < mergedString.length()) {
                //insert vowel into merged string at position, consonantsIndex
                //1, specifies number of times char should be inserted
                //ch, is the char to insert
                mergedString.insert(consonantsIndex, 1, ch);
                //increment index for next vowel insertion
                consonantsIndex++;
            }
        }
    }
    //print merged string
    std::cout << "Merged String: " << mergedString << std::endl;
    //send merged string to client
    if (send(clientSocket, mergedString.c_str(), mergedString.length(), 0) == -1) {
        //error message
        std::cerr << "Error sending merged string to Client! D:" << strerror(errno) << std::endl;
        //close connection
        close(clientSocket);
        exit(1);
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main() {
    const char* serverIp = "127.0.0.1"; //change later
    int serverPort = 8080;
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error Creating Socket: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(serverPort);
    server_address.sin_addr.s_addr = inet_addr(serverIp);

    if (bind(serverSocket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        std::cerr << "Error Binding Socket: " <<strerror(errno) << std::endl;
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Error Listening on Socket: " << strerror(errno) << std::endl;
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server is listening on " << serverIp << ":" << serverPort << std::endl;
    
    while (true) {
        struct sockaddr_in clientAddress;
        socklen_t clientAddress_len = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddress_len);
        if (clientSocket == -1) {
            std::cerr << "Error Accepting Connection: " << strerror(errno) << std::endl;
            continue;
        }

        std::cout << "Accepting Connection from " << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << std::endl;

        //client communication here
        char buffer[MAX_BUFFER_SIZE];
        int bytesReceived = recv(clientSocket, buffer, MAX_BUFFER_SIZE, 0);
        if (bytesReceived <= 0) {
            //handle error or connection closure
        } else {
            std::string receivedString(buffer, bytesReceived);
            //call appropriate function to perform split/merge and store result
            if (receivedString.find("BASIC_SPLIT: ") == 0) {
                std::string prefix = "BASIC_SPLIT: ";
                std::string parsedString = parseMessage(receivedString, prefix);
                splitBasic(clientSocket, parsedString);

            } else if (receivedString.find("ADVANCED_SPLIT: ") == 0) {
                std::string prefix = "ADVANCED_SPLIT: ";
                std::string parsedString = parseMessage(receivedString, prefix);
                splitAdvanced(clientSocket, parsedString);

            } else if (receivedString.find("BASIC_MERGE: ") == 0) {
                std::string prefix = "BASIC_MERGE: ";
                std::string consonantString = parseMessage(receivedString, prefix);
                std::string vowelString = receiveVowelsUDP(serverPort, prefix);
                mergeBasic(clientSocket, consonantString, vowelString);

            } else if (receivedString.find("ADVANCED_MERGE: ") == 0) {
                std::string prefix = "ADVANCED_MERGE: ";
                std::string consonantString = parseMessage(receivedString, prefix);
                std::string vowelString = receiveVowelsUDP(serverPort, prefix);
                mergeAdvanced(clientSocket, consonantString, vowelString);
            }
        }
    }
    close(serverSocket);
    return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////