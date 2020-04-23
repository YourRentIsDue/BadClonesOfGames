#include <iostream> //For output
#include <Windows.h> //Also for output
#include <thread>//To set refresh rate of game loop (it's literally only used once)
#include <cstdlib>//To make rand() work
#include <string>//Not to actually use String objects, but rather the methods asociated with them.

using namespace std; // so I don't have to write std:: before 90% of this code

const int SCREENWIDTH = 120; 
const int SCREENHEIGHT = 30;

class Player { //Player class stores info/methods asociated with player

    int x = 10;
    double y = 29;

public:

    int getX() { return x; }
    int getY() { return y; }

    void moveUp() {
        
        y--;
    }

    void moveDown() {

        y++;
    }
};

class Obs { //Obstacle class stores info/methods asociated with obstacles

    int x = 120;
    const int y = 29;
    int v = 1;
    bool spawned = 0;

public:

    int getX() { return x; }
    int getY() { return y; }
    bool getSpawn() { return spawned; }
    void setSpawn(bool set) { spawned = set; }

    void updatePosition(int gameSpeed) {

        x -= gameSpeed + v;

        if (x <= 0) {

            x = 120;
            setSpawn(0);
        }
    }
};

int main()
{
    Player p1;

    Obs o1[5]; //Change the size of this array and length of loops asociated with obstacles to set how many obstacles can appear on screen at any one time

    wchar_t* screen = new wchar_t[SCREENWIDTH * SCREENHEIGHT]; //Screen array of wchars. If you're on linux change wchar_t to char (might break WriteConsoleOutputCharacter())

    for (int i = 0; i < SCREENWIDTH * SCREENHEIGHT; i++) //Setting the screen to be empty
        screen[i] = ' '; 

    HANDLE console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL); //Declaring a Handle for the console buffer
    SetConsoleActiveScreenBuffer(console); //Setting the active console buffer to the handle that we just created
    DWORD dwBytesWritten = 0; //Useless variable microsoft insists on having

    bool gameOver = 0; 
    bool jumping = 0;
    bool falling = 0;
    int score = 0;
    double gameSpeed = 1;
    string scoreDisplay;

    while (!gameOver) {

        this_thread::sleep_for(50ms); //Refresh rate of while loop, set to a bigger number to make the game run slower

        scoreDisplay = to_string(score); //Parsing the score integer into a String

        jumping = GetAsyncKeyState(0x20); //Checks whether or not spacebar is pressed down on an interrupt basis 

        for (int i = 0; i < SCREENWIDTH; i++) //Putting hashtags at the top of the screen
            screen[i] = '#';

        for (int i = 1; i < SCREENHEIGHT; i++) //Hashtags on left side
            screen[i * 120] = '#';

        for (int i = 0; i < SCREENWIDTH; i++) //Hashtags on bottom
            screen[3480 + i] = '#';

        for (int i = 0; i < 4; i++)  //Removing old drawing of obstacle when it has moved
            if (o1[i].getSpawn()) { 
                screen[(o1[i].getY() - 1) * SCREENWIDTH + o1[i].getX()] = ' ';
                screen[(o1[i].getY() - 2) * SCREENWIDTH + o1[i].getX()] = ' ';
            }
        
        for (int i = 0; i < 4; i++) { //loop for spawning obstacles

            int spawn = rand() % 100; //Random number between 0 and 100

            if (!o1[i].getSpawn() && spawn > 65) { //if obstacle[i] is not spawned and RNG wills it into being start thinking about spawning it. change the number spawn has to be more than to adjust spawn rate of obstacles

                if (i == 0) { //if it's the first obstacle being spawned in this loop, then draw it and set spawned variable to true

                    screen[(o1[i].getY() - 1) * SCREENWIDTH + o1[i].getX()] = '#';
                    screen[(o1[i].getY() - 2) * SCREENWIDTH + o1[i].getX()] = '#';

                    o1[i].setSpawn(1);
                }

                else if ((o1[i].getX() - o1[i - 1].getX()) >= 20) { //if it's not the first one spawned this loop, make sure there's at least 20 units of space between it and the last one, then draw it and set it to spawned

                    screen[(o1[i].getY() - 1) * SCREENWIDTH + o1[i].getX()] = '#';
                    screen[(o1[i].getY() - 2) * SCREENWIDTH + o1[i].getX()] = '#';

                    o1[i].setSpawn(1);
                }
            }
        }

        for (int i = 0; i < 4; i++) //if obstacle is spawned make it move left according to gamespeed
            if (o1[i].getSpawn()) {

                o1[i].updatePosition(gameSpeed);

                screen[(o1[i].getY() - 1) * SCREENWIDTH + o1[i].getX()] = '#';
                screen[(o1[i].getY() - 2) * SCREENWIDTH + o1[i].getX()] = '#';
            }

        if (jumping && !falling) // if player is jumping (holding space) and not falling 
            if (!(p1.getY() <= 24)) { //and if the player hasn't reached the top of it's jump (adjust number to change jump height)

                screen[(p1.getY() - 1) * SCREENWIDTH + p1.getX()] = ' '; //undraw it from where it was
                p1.moveUp(); // and move it up
            }

        if (p1.getY() < 29 && !jumping) { //if above ground and not jumping 

            falling = 1; //set falling to true
        }
        
        if (falling) { //if falling

            screen[(p1.getY() - 1) * SCREENWIDTH + p1.getX()] = ' '; //Undraw where player was
            p1.moveDown();//and move down
        }

        if (p1.getY() == 29) { //if player has reached the ground

            falling = 0; //set falling to false
        }
        
        screen[(p1.getY() - 1) * SCREENWIDTH + p1.getX()] = 'S'; //Draw player


        for (int i = 0; i < 4; i++)
            if (p1.getY() == o1[i].getY() && (p1.getX() == o1[i].getX() || p1.getX() == (o1[i].getX() + 1) || p1.getX() == (o1[i].getX() - 1))) //This hit detection system is garbage, if anyone knows a better way to do it pls fix
                gameOver = 1;

        for (int i = 0; i < scoreDisplay.length(); i++) { //draw score at top of screen

            screen[2 * SCREENWIDTH + (SCREENWIDTH - scoreDisplay.size()) / 2 + i] = scoreDisplay[i];
        }

        WriteConsoleOutputCharacter(console, screen, SCREENWIDTH * SCREENHEIGHT, { 0,0 }, &dwBytesWritten); //does the actual writing to console 

        gameSpeed += 0.005; //add small amount to gamespeed
        score+=10; // add score
    }

    if (gameOver)
        cout << "RIP" << endl << "here's ur score btw: " << scoreDisplay;

    return 0;
}

