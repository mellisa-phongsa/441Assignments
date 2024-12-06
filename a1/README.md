CPSC 441 Assignment 1: Protocols and Sockets
    The goal of assignment 1 was to create a client: tictacClient.py that communicates with a server that can be found on the university cpsc server csx.cpsc.ucalgary.ca (IP 136.159.5.25) on port 6969, to play tic tac toe with an AI opponent. The server handles keeping track of the game state and making moves for the AI opponent.

Features of tictacClient.py:
    1. Tic Tac Toe game loop: for repeated user input and rendering
    2. Board rendering: for user readability, takes "BORD:..." message from server and renders a tictactoe board
    3. Menu Options:
        -New Game: creates a new game. Client sends "NEWG" message to server to create a new game, the return message is the game board.
        -Load Game: asks for filename of saved game. Client sends "LOAD:savedGame" message to server to load a saved game, where the saved game is contained in a file. The return message is the game board of the loaded game
            ex. savedGame: X,2,2,2,2,2,2,2,2,2 "LOAD:savedGame"
        -Show Score: shows number of times the server won, the client won and draws
        -Exit: closes connection to server, sends "CLOS" message to server.
        To use menu options user should enter 1-4 when asked.
            ex. Game Menu:
                1.
                ...
                4.
                Option: 1
    4. Save Game: if user enters "s s" asks user for a filename to save the current state of the board
    5. End Game: if user enters "g g" end the current game
    6.Move: asks where user wants to move and sends "MOVE:..." message to server to show that the user has moved and returns the game board. User input should be seperated by a space.
        ex. Move: 1 1

Running the Program:
    In your terminal navigate to where "tictacClient.py" is stored. The program runs in terminal and takes two command line arguments, python3 tictacClient.py 36.159.5.25 6969

Known Bugs:
    If user moves to a spot that has already been taken the current game ends.
      

