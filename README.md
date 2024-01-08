# Multiplayer Dice Game

This repository contains the source code for a simple multiplayer dice game implemented in C. The game allows multiple players to connect to a server, make bets, and guess whether the sum of two dice rolls will be even or odd.

## Table of Contents

- [Game Instruction](#game-instruction)
- [Game Rules](#game-rules)
- [Client Implementation](#client-implementation)
- [Server Implementation](#server-implementation)
- [Usage](#usage)
- [Dependencies](#dependencies)
- [Acknowledgments](#acknowledgments)

## Game Instruction

The `gameInstruction` function provides the basic instructions for playing the game. Players can choose to start the game, add more money to their balance, or quit. After adding money, players have to wait for the next round.

```c
void gameInstruction(void){
    // Display game instructions
}
```

## Game Rules

1. **Adding Money:**
    - Players can add more money to their initial balance by pressing 'a'.
    - After adding money, players must wait until the next round to participate.

2. **Starting the Round:**
    - Players start the round by pressing 'y'.
    - Each player must input the amount of money they want to bet for that round.

3. **Guessing Even or Odd:**
    - After betting, players guess whether the sum of two dice rolls will be even ('e') or odd ('o').

4. **Determining the Winner:**
    - The server generates two random numbers representing the outcomes of two dice rolls.
    - If a player's guess matches the sum of the dice rolls, they win; otherwise, they lose.

5. **Winning and Losing:**
    - Players win if their guess matches the dice outcome and receive a share of the total money bet by all players who guessed wrong.
    - Players lose the amount they bet if their guess is incorrect.

6. **Quitting the Game:**
    - Players can quit the game at any time by pressing 'q'.

## Client Implementation

The `main` function in the client code initializes the client, connects to the server, and enters a loop where players can make bets and guesses. Players can add money to their balance, guess whether the sum of the dice rolls is even or odd, and quit the game.

```c
int main(int argc, char *argv[]){
    // Main client function
}
```

## Server Implementation

The server code consists of functions to initialize the server, accept connections from clients, and manage the game loop. Players connect to the server, make their bets, and the server determines the outcome, distributing or deducting money accordingly.

```c
int main(int argc, char *argv[]){
    // Main server function
}

void clientInit(struct client *client){
    // Initialize client data
}
```

## Usage

### Client

To run the client, provide the server's IP address and port number as command-line arguments.

```bash
./client <Server IP> <port>
```

### Server

To run the server, provide the desired port number and the maximum number of clients as command-line arguments.

```bash
./server <port> <maxClients>
```

Note: The port number must be greater than 1024.

## Dependencies

- `unistd.h`
- `header.h`

## Acknowledgments

This project was inspired by the desire to create a simple multiplayer dice game using socket programming in C. Special thanks to the contributors and developers who provided valuable insights.

Feel free to explore, modify, and enhance the code for your own projects or educational purposes. Enjoy the game!