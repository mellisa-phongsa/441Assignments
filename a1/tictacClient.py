#CPSC 441 Assignment 1: Protocols and Sockets
#Mellisa Phongsa
#136.159.5.25
#6969

import os
import socket
import sys

#first cmd ln arg is the IP address of server
HOST = sys.argv[1]
#second cmd ln arg is servers port number
PORT = int(sys.argv[2])

#just to show what user typed
print("\nIP Address: ", HOST)
print("Port Number: ", PORT, "\n")

userScore = 0
serverScore = 0
draw = 0

userChar = ""
load = False

#banner from https://manytools.org/hacker-tools/ascii-banner/ #######################################################
def welcomeBanner():
    banner = """
    Welcome To
     ███████████ █████   █████████     ███████████   █████████     █████████     ███████████    ███████    ██████████
    ░█░░░███░░░█░░███   ███░░░░░███   ░█░░░███░░░█  ███░░░░░███   ███░░░░░███   ░█░░░███░░░█  ███░░░░░███ ░░███░░░░░█
    ░   ░███  ░  ░███  ███     ░░░    ░   ░███  ░  ░███    ░███  ███     ░░░    ░   ░███  ░  ███     ░░███ ░███  █ ░ 
        ░███     ░███ ░███                ░███     ░███████████ ░███                ░███    ░███      ░███ ░██████   
        ░███     ░███ ░███                ░███     ░███░░░░░███ ░███                ░███    ░███      ░███ ░███░░█   
        ░███     ░███ ░░███     ███       ░███     ░███    ░███ ░░███     ███       ░███    ░░███     ███  ░███ ░   █
        █████    █████ ░░█████████        █████    █████   █████ ░░█████████        █████    ░░░███████░   ██████████
       ░░░░░    ░░░░░   ░░░░░░░░░        ░░░░░    ░░░░░   ░░░░░   ░░░░░░░░░        ░░░░░       ░░░░░░░    ░░░░░░░░░░
       0 = X                             1 = O                                     2 = Empty space
    """
    print(banner)
    
def menu():
    print("""\nLOADING GAME MENU...
██████████████]99%""")
    print("\nGame Menu:\n", "1. New Game\n", "2. Load Game\n", "3. Show Score\n", "4. Exit")
    options()
#####################################################################################################################

#take BORD message from server and render a tic tac toe board #######################################################
def render(board):
    #check if the game is over be looking at the character at position 5:6
    if board[5:6] in ("N", "S", "C"):
        #if the game is over spilt the string in two, store first half in msg and second half in data ex. msg = OVER:S, data = ...
        msg, data = board.split(board[:7])
        #create a list of strings
        boardState = data.split(",")
        
    #game is ongoing
    else:
        #if the game is ongoing split the string in two where there is a colon, store first half in msg, second half in data
        msg, data = board.split(":")
        #create list of strings
        boardState = data.split(",")
    
    #create dictionary to map characters 0, 1, 2 to X, O and empty space
    symbols = {
        '0': 'X',
        '1': 'O',
        '2': ' '
    }
    
    #set formatted board to be an empty string
    formattedBoard = ""
    
    #for loop 0 to 9 increment by 3
    for i in range(0, 9, 3):
        #create rows of game, for each element in the list from i to i + 3 convert the symbols
        row = [symbols[cell] for cell in boardState[i:i+3]]
        #join | after each symbol in the row (makes columns)
        formattedBoard += " | ".join(row) + "\n"
        # seperate each row with "-"
        if i < 6:
            formattedBoard += "---------\n"

    #return formatted board
    return formattedBoard
#####################################################################################################################

#function allows user to make moves #################################################################################
def play(board):
    #update global variables
    global userScore
    global serverScore
    global draw
    global userChar
    global load
    
    #see what char the server and user are
    if board[5:6] == "0" and load == False:
        print("Server is X     User is O")
        userChar = "O"
    elif board[5:6] == "2" and load == False:
        print("Server is O     User is X")
        userChar = "X"
    
    #call render function to render board
    formattedBoard = render(board)
    #print board to terminalc
    print(formattedBoard)
    print("\nEnter: 'row(0-2) column(0-2)' to Move          'g g' to End Game          's s' to Save Game")
    
    #try catch block to catch invalid number of inputs
    try:
        #user enters coordinates of move or g g
        row, column = input("Move: ").split()
        
        #keep asking for user input until user enters g g
        while (board[:5] != "OVER:" and "UWIN:" and row and column != "g"):
            #if user enters s s
            if row == 's' and column == 's':
                #get the current board
                boardState = board[5:]
                #concatenate user charcter and board
                game = userChar + "," + boardState
                #pass the game through the save function
                saveGame(game)
                #break out of the loop
                break
            
            #user makes a move
            else:
                #get board from server
                board = message(f"MOVE:{row},{column}")
                
                #call render function to render board
                formattedBoard = render(board)
                #print board to terminal
                print(formattedBoard)
                
                #if the game is not over yet ask for user input
                if board[:5] != "OVER:" and "UWIN:":
                    #ask for user input
                    row, column = input("\nMove: ").split()
                    
                #check if server won  
                elif board[:6] == "OVER:S":
                    print("Server wins! D:")
                    #update score
                    serverScore = serverScore + 1
                    load = False
                    #call menu
                    menu()
                    
                #check if it was a draw 
                elif board[:6] == "OVER:N":
                    print("Its a draw! -_-")\
                    #update score
                    draw = draw + 1
                    load = False
                    menu()
                    
                #check if user won
                else:
                    print("You win! :D")
                    #update score
                    userScore = userScore + 1
                    load = False
                    menu()
                    
        #if the user enters g g call menu function
        if row and column == "g":
            endGame()
            
    #catch invalid number of input error
    except ValueError:
        print("\nInvalid Number of Inputs... Try Again!")
        #call play function to allow user to try again
        play(board)
    #other errors
    except Exception as e:
        print(f"\nUh Oh Something Went Wrong!: {e}")
        #call exit function to close connection
        menu()
#####################################################################################################################

#this function calls the message function to cause the server to initialize a new game board ######################## 
def newGame():
    print("""\nCREATING A NEW GAME...  
██████████████]99%\n""")
    #send message to server to get new board
    board = message("NEWG")
    #call function to play game
    play(board)
#####################################################################################################################

#this function asks user for filename and sends the saved game to the server, for the server to load the game #######
def loadGame():
    global userChar
    global load
    #ask user for filename
    filename = input("\nEnter filename: ")
    
    #try catch block 
    try:
        load = True
        #define subdirectory
        subdirectory = "savedGames"
        #define location of save file
        filepath = os.path.join(subdirectory, filename)
        
        #open and read the file at this location
        with open(filepath, 'r') as file:
            savedGame = file.read()
            
        print("""\nLOADING GAME...         
██████████████]99%\n""")
        
        userChar = savedGame[:1]
        #send loaded game to file
        loadedGame = message(f"LOAD:{savedGame}")
        #call play function to play with loaded game board
        play(loadedGame)
        
    #catch file not found error
    except FileNotFoundError:
        print("File", filename, "not found.")
        menu()
    #catch cant read file error
    except Exception as e:
        print(f"Error Loading Saved Game: {e}")
        menu()
#####################################################################################################################

#this function asks user for a filename to save the current board at a subdirectory of the location the client script
#is run from
def saveGame(board):
    #ask user for filename
    filename = input("\nEnter filename: ")
    
    #hardcoded subdirectory to save file to
    subdirectory = "savedGames"
    
    #if path doesnt exist make it
    if not os.path.exists(subdirectory):
        os.makedirs(subdirectory)
        
    #make file path by joining file name and subdirectory
    filepath = os.path.join(subdirectory, filename)
    print("""\nSAVING... 
██████████████]99%\n""")
    
    #try catch block
    try:
        #open file and write to it
        with open(filepath, 'w') as file:
            #write game board on file
            file.write(board)
            
        print("Game Saved to ", filepath)
    
    #catch any errors
    except Exception as e:
        print(f"Error Saving Game: {e}")
    
    #call menu function 
    menu()
#####################################################################################################################

#function shows score ###############################################################################################
def showScore():
    #print score
    print("""\nLOADING SCORE...
██████████████]99%""")
    print("User Score:", userScore, "     Server Score:", serverScore, "     Draws:", draw)
    menu()
#####################################################################################################################

#function sends message to server to close the connection ###########################################################
def exit():
    #close connection to server
    print("\nClosing Connection to Server...")
    return message("CLOS")
#####################################################################################################################

#function ends current game #########################################################################################
def endGame():
    #end current game
    print("\nENDING GAME... ")
    reply = message("ENDG")
    formattedBoard = render(reply)
    print("No Winner! -_-")
    print(formattedBoard)
    menu()
#####################################################################################################################

#function sends and recieves messages to and from server ############################################################
def message(msg):
    #send message to server convert to bits
    s.sendall(msg.encode())
    #recieve message from server convert from bits
    reply = s.recv(1024).decode()
    #return message from server
    return reply
#####################################################################################################################

#function takes input from user and looks at dictionary to see if user input is in there, if yes call appropriate
#function
def options():
    #get user input
    decision = input("Option: ")
    
    #if user input is in dictionary to call function
    if decision in menuOptions:
        #call function
        menuOptions[decision]()
        
    #if user enters a value that is not in the dictionary ask user to try again
    else:
        print("Invalid Input... Try Again!")
        #call function
        options()
#####################################################################################################################

#dictionary to map users decision to functions ######################################################################
menuOptions = {
    #dictionary of user inputs mapped to functions
    '1' : newGame,
    '2' : loadGame,
    '3' : showScore,
    '4' : exit
}
#####################################################################################################################

#create socket using IPv4 and TCP ###################################################################################
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    #try to connect to server
    try:
        #connect to server
        s.connect((HOST, PORT))
        #display welcome banner
        welcomeBanner()
        #call menu function
        menu()
        
    #catch connection/socket errors
    except socket.error as se:
        print(f"Socket Error: {se}")
    except Exception as e:
        print(f"An error occurred: {e}")
#####################################################################################################################
    
