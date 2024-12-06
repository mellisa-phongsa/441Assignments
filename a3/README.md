CPSC 441 Assignment 3: TCP and UDP
    The goal of this assignment was to get some experience with client-server socket programming over both TCP and UDP.

How to run:
    In your terminal navigate to the directory of where you stored "vowelizerServer.cpp" compile the code with the command:
    "g++ -o vowelizerServer.exe vowelizerServer.cpp" and run the code with the command:
    "./vowelizerServer.exe"
    The server is now running and waiting to accept a connection from the client.
    In a new terminal navigate to the directory of where you stored "vowelizerClient.cpp" complie the code with the command:
    "g++ -o vowelizerClient.exe vowelizerClient.cpp" and run the code with the command:
    "./vowelizerClient.exe"
    The client is now running and should have succesfully connected to the server.

    A menu will appear if socket connection was succesfull, enter the digit of the menu option youd like.

Testing:
    testing was done at home, I used SSH:cslinux.ucalgary.ca to connect to the ucalgary linux machines virtually.
    Split Basic and Split Advanced functions work. TCP communication works, but UDP communication does not.
    Merge Basic and Merge Advanced do no work.

