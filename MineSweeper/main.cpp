#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>
using namespace std;
using namespace sf;

void openBoard(ifstream& myFile, vector<vector<bool>>& checkBoardTemp, unsigned int row, unsigned int col) {
    string input;
    int valRow = 0;
    int valCol = 0;

    for (unsigned int i = 0; i < col; i++) {
        for (unsigned int j = 0; j < row; j++) {
            checkBoardTemp[i][j] = false;
        }
    }

    while (getline(myFile, input)) {
        for (char& c : input) {
            if (c == '1') {
                checkBoardTemp[valCol][valRow] = true;
            }
            else
                checkBoardTemp[valCol][valRow] = false;
            valCol++;
            if (valCol == col)
                valCol = 0;
        }
        valRow++;
    }
}


void revealedTile(vector<vector<int>>& saveLocation, vector<vector<int>>& saveLocationTemp, vector<vector<bool>>& isRevealed, unsigned int thisCol, unsigned int thisRow, unsigned int col, unsigned int row) {

    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            int value_X = thisCol + i;
            int value_Y = thisRow + j;
            if (value_X > -1 && value_X < col && value_Y > -1 && value_Y < row) {
                int neighbor = saveLocationTemp[value_X][value_Y];
                if (neighbor != 9 && isRevealed[value_X][value_Y] == false) {
                    saveLocation[value_X][value_Y] = neighbor;
                    isRevealed[value_X][value_Y] = true;
                    if (neighbor == 0)
                        revealedTile(saveLocation, saveLocationTemp, isRevealed, value_X, value_Y, col, row);
                }
            }

        }
    }
}

String conversion(int value) {
    string result = "";
    if (value < 0)
        value = abs(value);
    if (value >= 10) {
        result.append("0");
        result.append(to_string(value));
    }
    if (value >= 100) {
        return to_string(value);
    }
    else if (value < 10) {
        result.append("0");
        result.append("0");
        result.append(to_string(value));
    }
    return result;
}

int main() {

    //Getting board dimension and number of mines
    ifstream file;
    file.open("boards/config.cfg");
    vector<string> values;
    string input;
    if (file.is_open()) {
        while (getline(file, input)) {
            values.push_back(input);
        }
    }
    int col = 25;
    int row = 16;
    int numMinesRandom = 50;
    int numMinesTemp = numMinesRandom;
    int tileCount = col * row;

    file.close();

    vector<vector<int>> saveLocation;
    vector<vector<int>> saveLocationTemp;
    vector<vector<bool>> checkBoardTemp;
    vector<vector<bool>> checkBoard;
    vector<vector<bool>> isRevealed;

    for (unsigned int i = 0; i < col; i++) {
        vector<int> tempAdding;
        vector<bool> tempCheck;
        for (unsigned int j = 0; j < row; j++) {
            tempAdding.push_back(-1);
            tempCheck.push_back(false);
        }
        saveLocationTemp.push_back(tempAdding);
        saveLocation.push_back(tempAdding);
        checkBoardTemp.push_back(tempCheck);
        checkBoard.push_back(tempCheck);
        isRevealed.push_back(tempCheck);
    }

    int width = 32;
    int windowWidth = col * 32;
    int windowHeight = (row * 32) + 88;

    RenderWindow window(VideoMode(windowWidth, (row * 32) + 88), "MineSweeper");

    //Tile images (revealed, hidden)
    Texture t;
    t.loadFromFile("images/tile_hidden.png");
    Sprite tile(t);

    Texture tr;
    tr.loadFromFile("images/tile_revealed.png");
    Sprite tileRevealed(tr);

    //Face images
    Texture f;
    f.loadFromFile("images/face_happy.png");
    Sprite face(f);

    Texture hh;
    hh.loadFromFile("images/face_win.png");
    Sprite winFace(hh);

    Texture s;
    s.loadFromFile("images/face_lose.png");
    Sprite sadFace(s);

    //Each mine
    Texture m;
    m.loadFromFile("images/mine.png");
    Sprite mine(m);

    //Flag Image
    Texture fl;
    fl.loadFromFile("images/flag.png");
    Sprite flag(fl);

    //All mines showed
    Texture b;
    b.loadFromFile("images/debug.png");
    Sprite bomb(b);

    //Test board
    Texture t1, t2, t3;
    t1.loadFromFile("images/test_1.png");
    t2.loadFromFile("images/test_2.png");
    t3.loadFromFile("images/test_3.png");
    Sprite testOne(t1);
    Sprite testTwo(t2);
    Sprite testThree(t3);

    //Digit Image
    Texture d;
    d.loadFromFile("images/digits.png");
    Sprite digit(d);

    //Numbers
    Texture n1, n2, n3, n4, n5, n6, n7, n8;
    n1.loadFromFile("images/number_1.png");
    n2.loadFromFile("images/number_2.png");
    n3.loadFromFile("images/number_3.png");
    n4.loadFromFile("images/number_4.png");
    n5.loadFromFile("images/number_5.png");
    n6.loadFromFile("images/number_6.png");
    n7.loadFromFile("images/number_7.png");
    n8.loadFromFile("images/number_8.png");
    Sprite numOne(n1);
    Sprite numTwo(n2);
    Sprite numThree(n3);
    Sprite numFour(n4);
    Sprite numFive(n5);
    Sprite numSix(n6);
    Sprite numSeven(n7);
    Sprite numEight(n8);

    sf::Vector2u size = window.getSize();
    unsigned int w = size.x;
    unsigned int h = size.y;

    //There are 2 board, one is actual and another is for hidden showed
    vector<vector<Sprite>> myBoard;
    vector<vector<Sprite>> actualBoard;
    vector<vector<Sprite>> showBomb;
    vector<vector<Sprite>> example;
    vector<vector<Sprite>> middleGraph;

    for (unsigned int i = 0; i < col; i++) {                     //Hidden board
        vector<Sprite> tempAdding;
        for (unsigned int j = 0; j < row; j++) {
            tempAdding.push_back(tile);
        }
        example.push_back(tempAdding);
        middleGraph.push_back(tempAdding);
    }


    for (unsigned int i = 0; i < col; i++) {                     //Hidden board
        vector<Sprite> tempAdding;
        vector<Sprite> forMainBoard;
        vector<Sprite> forActualBoard;
        for (unsigned int j = 0; j < row; j++) {
            forMainBoard.push_back(tile);
            if (rand() % col == 0 && numMinesTemp > 0) {
                forActualBoard.push_back(mine);
                tempAdding.push_back(mine);
                checkBoard[i][j] = true;
                numMinesTemp--;
            }
            else {
                forActualBoard.push_back(tile);
                tempAdding.push_back(tile);
                checkBoard[i][j] = false;
            }
        }
        actualBoard.push_back(forActualBoard);
        myBoard.push_back(forMainBoard);
        showBomb.push_back(tempAdding);
        //Check if mines have not filled with total number of mines
        if (i == col - 1 && numMinesTemp != 0) {
            for (unsigned int j = numMinesTemp; j > 0;) {
                unsigned int tempRow = rand() % row;
                unsigned int tempCol = rand() % col;
                if (checkBoard[tempCol][tempRow] == false) {
                    actualBoard[tempCol][tempRow] = mine;
                    showBomb[tempCol][tempRow] = mine;
                    checkBoard[tempCol][tempRow] = true;
                    j--;
                }
            }
        }

    }

    for (unsigned int i = 0; i < col; i++) {                     //Actual board
        for (unsigned int j = 0; j < row; j++) {

            unsigned int val = 0;
            if (checkBoard[i][j] == true) {
                saveLocationTemp[i][j] = 9;
                continue;
            }

            if (j == 0) {
                if (i == 0) {
                    if (checkBoard[i + 1][j + 1] == true) val++;
                    if (checkBoard[i][j + 1] == true) val++;
                    if (checkBoard[i + 1][j] == true) val++;
                }
                else if (i == col - 1) {
                    if (checkBoard[i - 1][j] == true) val++;
                    if (checkBoard[i - 1][j + 1] == true) val++;
                    if (checkBoard[i][j + 1] == true) val++;
                }
                else {
                    if (checkBoard[i - 1][j] == true) val++;
                    if (checkBoard[i + 1][j] == true) val++;
                    if (checkBoard[i][j + 1] == true) val++;
                    if (checkBoard[i - 1][j + 1] == true) val++;
                    if (checkBoard[i + 1][j + 1] == true) val++;
                }
            }

            else if (j == row - 1) {
                if (i == 0) {
                    if (checkBoard[i + 1][j - 1] == true) val++;
                    if (checkBoard[i][j - 1] == true) val++;
                    if (checkBoard[i + 1][j] == true) val++;
                }
                else if (i == col - 1) {
                    if (checkBoard[i - 1][j] == true) val++;
                    if (checkBoard[i - 1][j - 1] == true) val++;
                    if (checkBoard[i][j - 1] == true) val++;
                }
                else {
                    if (checkBoard[i - 1][j] == true) val++;
                    if (checkBoard[i + 1][j] == true) val++;
                    if (checkBoard[i][j - 1] == true) val++;
                    if (checkBoard[i - 1][j - 1] == true) val++;
                    if (checkBoard[i + 1][j - 1] == true) val++;
                }
            }

            else if (i == 0 && j != 0) {
                if (checkBoard[i + 1][j + 1] == true) val++;
                if (checkBoard[i + 1][j - 1] == true) val++;
                if (checkBoard[i][j + 1] == true) val++;
                if (checkBoard[i][j - 1] == true) val++;
                if (checkBoard[i + 1][j] == true) val++;
            }

            else if (i == col - 1 && j != row - 1) {
                if (checkBoard[i - 1][j + 1] == true) val++;
                if (checkBoard[i - 1][j - 1] == true) val++;
                if (checkBoard[i][j + 1] == true) val++;
                if (checkBoard[i][j - 1] == true) val++;
                if (checkBoard[i - 1][j] == true) val++;
            }

            else {  //Mid point
                if (checkBoard[i - 1][j + 1] == true) val++;
                if (checkBoard[i][j + 1] == true) val++;
                if (checkBoard[i][j - 1] == true) val++;
                if (checkBoard[i + 1][j - 1] == true) val++;
                if (checkBoard[i - 1][j - 1] == true) val++;
                if (checkBoard[i + 1][j + 1] == true) val++;
                if (checkBoard[i - 1][j] == true) val++;
                if (checkBoard[i + 1][j] == true) val++;
            }

            if (val == 0) actualBoard[i][j] = tileRevealed;
            else if (val == 1) actualBoard[i][j] = numOne;
            else if (val == 2) actualBoard[i][j] = numTwo;
            else if (val == 3) actualBoard[i][j] = numThree;
            else if (val == 4) actualBoard[i][j] = numFour;
            else if (val == 5) actualBoard[i][j] = numFive;
            else if (val == 6) actualBoard[i][j] = numSix;
            else if (val == 7) actualBoard[i][j] = numSeven;
            else if (val == 8) actualBoard[i][j] = numEight;
            saveLocationTemp[i][j] = val;

        }
    }

    bool cheatClick = true;
    bool isLose = false;
    int mineLeft = numMinesRandom;
    int isWin = tileCount - numMinesRandom;
    int tempFlag = 0;
    bool isFlagThere = false;

    vector<vector<bool>> isFlag;
    vector<vector<bool>> isClicked;
    for (unsigned int i = 0; i < col; i++) {
        vector<bool> tempAdding;
        vector<int> adj;
        for (unsigned int j = 0; j < row; j++) {
            tempAdding.push_back(false);
        }
        isFlag.push_back(tempAdding);
        isClicked.push_back(tempAdding);
    }

    while (window.isOpen()) {

        Vector2i getOnClick = Mouse::getPosition(window);
        int x_pos = getOnClick.x / 32;
        int y_pos = getOnClick.y / 32;

        Event event;
        while (window.pollEvent(event)) {

            if (event.type == Event::Closed)
                window.close();

            if (event.type == Event::MouseButtonPressed) {
                // If Left click

                if (event.mouseButton.button == Mouse::Left && y_pos <= row - 1 && isFlag[x_pos][y_pos] != true && isClicked[x_pos][y_pos] != true) {     //Each tile on click (above the board)

                    if (checkBoard[x_pos][y_pos] == true && !isLose) {                          //If the tile is the bomb show all the bombs
                        isLose = true;

                        isWin = tileCount - numMinesTemp;
                        for (unsigned int i = 0; i < col; i++) {
                            for (unsigned int j = 0; j < row; j++) {
                                if (checkBoard[i][j] == true) {
                                    if (isFlag[i][j]) {
                                        myBoard[i][j] = mine;
                                        middleGraph[i][j] = flag;
                                        example[i][j] = tileRevealed;
                                    }
                                    else {
                                        middleGraph[i][j] = tileRevealed;
                                        myBoard[i][j] = mine;
                                    }
                                        
                                }
                                    
                                isClicked[i][j] = true;
                            }
                        }
                    }

                    //Adjacent tile input here
                    if (!isLose) {
                        //Check if click on revealed tile
                        if (saveLocationTemp[x_pos][y_pos] == 0) {

                            for (int i = 0; i < col; i++) {
                                for (int j = 0; j < row; j++) {
                                    saveLocation[i][j] = 10;
                                    isRevealed[i][j] = false;
                                }
                            }

                            revealedTile(saveLocation, saveLocationTemp, isRevealed, x_pos, y_pos, col, row);

                            for (unsigned int i = 0; i < col; i++) {
                                for (unsigned int j = 0; j < row; j++) {
                                    if (saveLocation[i][j] == 0) myBoard[i][j] = tileRevealed;
                                    else if (saveLocation[i][j] == 1) myBoard[i][j] = numOne;
                                    else if (saveLocation[i][j] == 2) myBoard[i][j] = numTwo;
                                    else if (saveLocation[i][j] == 3) myBoard[i][j] = numThree;
                                    else if (saveLocation[i][j] == 4) myBoard[i][j] = numFour;
                                    else if (saveLocation[i][j] == 5) myBoard[i][j] = numFive;
                                    else if (saveLocation[i][j] == 6) myBoard[i][j] = numSix;
                                    else if (saveLocation[i][j] == 7) myBoard[i][j] = numSeven;
                                    else if (saveLocation[i][j] == 8) myBoard[i][j] = numEight;
                                    if (saveLocation[i][j] <= 8 && isFlag[i][j] == false && !isClicked[i][j]) {
                                        isClicked[i][j] = true;
                                        middleGraph[i][j] = tileRevealed;
                                        isWin--;
                                    }

                                }
                            }

                        }
                        //Else do this
                        else {
                            myBoard[x_pos][y_pos] = actualBoard[x_pos][y_pos];              //Else shows the numbers that is valid
                            middleGraph[x_pos][y_pos] = tileRevealed;
                            if (!isClicked[x_pos][y_pos]) {
                                isWin--;
                            }
                            isClicked[x_pos][y_pos] = true;
                        }
                    }

                }

                else if (event.mouseButton.button == Mouse::Left && y_pos > row - 1) {    //Below the board on click
                    auto mousePosition = Mouse::getPosition();
                    mousePosition = Mouse::getPosition(window);

                    FloatRect bombShow = bomb.getGlobalBounds();
                    FloatRect faceShow = face.getGlobalBounds();
                    FloatRect theTestOne = testOne.getGlobalBounds();
                    FloatRect theTestTwo = testTwo.getGlobalBounds();
                    FloatRect theTestThree = testThree.getGlobalBounds();

                    Vector2f mousePosFloat(mousePosition.x, mousePosition.y);

                    if (bombShow.contains(mousePosFloat)) {                           //Bomb pic on click cheat sheet

                        //Hidden Texture, Flag on top, Mine on top
                        if (cheatClick == true && isLose == false) {
                            for (unsigned int i = 0; i < col; i++) {
                                for (unsigned int j = 0; j < row; j++) {
                                    if (checkBoard[i][j]) {
                                        if (isFlag[i][j]) {
                                            middleGraph[i][j] = flag;
                                            myBoard[i][j] = mine;
                                        }
                                        else {
                                            middleGraph[i][j] = tile;
                                            myBoard[i][j] = mine;
                                        }
   
                                    }
                                }
                            }
                            cheatClick = false;
                        }

                        else if (cheatClick == false && isLose == false){
                            for (unsigned int i = 0; i < col; i++) {
                                for (unsigned int j = 0; j < row; j++) {
                                    if (checkBoard[i][j] == true && !isFlag[i][j])
                                        myBoard[i][j] = tile;
                                    else if (checkBoard[i][j] && isFlag[i][j])
                                        myBoard[i][j] = flag;
                                }
                            }
                            cheatClick = true;
                        }

                     
                    }

                    else if (faceShow.contains(mousePosFloat)) {                      //Face button

                        mineLeft = numMinesRandom;
                        isLose = false;
                        numMinesTemp = numMinesRandom;
                        isWin = tileCount - numMinesRandom;

                        for (unsigned int i = 0; i < col; i++) {                     //Hidden board

                            for (unsigned int j = 0; j < row; j++) {
                                myBoard[i][j] = tile;
                                isClicked[i][j] = false;
                                isFlag[i][j] = false;
                                middleGraph[i][j] = tile;
                                if (rand() % col == 0 && numMinesTemp > 0) {
                                    actualBoard[i][j] = mine;
                                    showBomb[i][j] = mine;
                                    checkBoard[i][j] = true;
                                    numMinesTemp--;
                                }
                                else {
                                    actualBoard[i][j] = tile;
                                    showBomb[i][j] = tile;
                                    checkBoard[i][j] = false;
                                }
                            }

                            //Check if mines have not filled with total number of mines
                            if (i == col - 1 && numMinesTemp != 0) {
                                for (unsigned int j = numMinesTemp; j > 0;) {
                                    unsigned int tempRow = rand() % row;
                                    unsigned int tempCol = rand() % col;
                                    if (checkBoard[tempCol][tempRow] == false) {
                                        actualBoard[tempCol][tempRow] = mine;
                                        showBomb[tempCol][tempRow] = mine;
                                        checkBoard[tempCol][tempRow] = true;
                                        j--;
                                    }
                                }
                            }

                        }

                        for (unsigned int i = 0; i < col; i++) {                     //Actual board

                            for (unsigned int j = 0; j < row; j++) {

                                unsigned int val = 0;
                                if (checkBoard[i][j] == true) {
                                    saveLocationTemp[i][j] = 9;
                                    continue;
                                }

                                if (j == 0) {                                            //edge on the right
                                    if (i == 0) {
                                        if (checkBoard[i + 1][j + 1] == true) val++;
                                        if (checkBoard[i][j + 1] == true) val++;
                                        if (checkBoard[i + 1][j] == true) val++;
                                    }
                                    else if (i == col - 1) {
                                        if (checkBoard[i - 1][j] == true) val++;
                                        if (checkBoard[i - 1][j + 1] == true) val++;
                                        if (checkBoard[i][j + 1] == true) val++;
                                    }
                                    else {
                                        if (checkBoard[i - 1][j] == true) val++;
                                        if (checkBoard[i + 1][j] == true) val++;
                                        if (checkBoard[i][j + 1] == true) val++;
                                        if (checkBoard[i - 1][j + 1] == true) val++;
                                        if (checkBoard[i + 1][j + 1] == true) val++;
                                    }
                                }

                                else if (j == row - 1) {                                   //edge on the left
                                    if (i == 0) {
                                        if (checkBoard[i + 1][j - 1] == true) val++;
                                        if (checkBoard[i][j - 1] == true) val++;
                                        if (checkBoard[i + 1][j] == true) val++;
                                    }
                                    else if (i == col - 1) {
                                        if (checkBoard[i - 1][j] == true) val++;
                                        if (checkBoard[i - 1][j - 1] == true) val++;
                                        if (checkBoard[i][j - 1] == true) val++;
                                    }
                                    else {
                                        if (checkBoard[i - 1][j] == true) val++;
                                        if (checkBoard[i + 1][j] == true) val++;
                                        if (checkBoard[i][j - 1] == true) val++;
                                        if (checkBoard[i - 1][j - 1] == true) val++;
                                        if (checkBoard[i + 1][j - 1] == true) val++;
                                    }
                                }

                                else if (i == 0 && j != 0) {                             //Side top
                                    if (checkBoard[i + 1][j + 1] == true) val++;
                                    if (checkBoard[i + 1][j - 1] == true) val++;
                                    if (checkBoard[i][j + 1] == true) val++;
                                    if (checkBoard[i][j - 1] == true) val++;
                                    if (checkBoard[i + 1][j] == true) val++;
                                }

                                else if (i == col - 1 && j != row - 1) {                     //Side bottom
                                    if (checkBoard[i - 1][j + 1] == true) val++;
                                    if (checkBoard[i - 1][j - 1] == true) val++;
                                    if (checkBoard[i][j + 1] == true) val++;
                                    if (checkBoard[i][j - 1] == true) val++;
                                    if (checkBoard[i - 1][j] == true) val++;
                                }

                                else {  //Mid point
                                    if (checkBoard[i - 1][j + 1] == true) val++;
                                    if (checkBoard[i][j + 1] == true) val++;
                                    if (checkBoard[i][j - 1] == true) val++;
                                    if (checkBoard[i + 1][j - 1] == true) val++;
                                    if (checkBoard[i - 1][j - 1] == true) val++;
                                    if (checkBoard[i + 1][j + 1] == true) val++;
                                    if (checkBoard[i - 1][j] == true) val++;
                                    if (checkBoard[i + 1][j] == true) val++;
                                }

                                if (val == 0) actualBoard[i][j] = tileRevealed;

                                else if (val == 1) actualBoard[i][j] = numOne;
                                else if (val == 2) actualBoard[i][j] = numTwo;
                                else if (val == 3) actualBoard[i][j] = numThree;
                                else if (val == 4) actualBoard[i][j] = numFour;
                                else if (val == 5) actualBoard[i][j] = numFive;
                                else if (val == 6) actualBoard[i][j] = numSix;
                                else if (val == 7) actualBoard[i][j] = numSeven;
                                else if (val == 8) actualBoard[i][j] = numEight;
                                saveLocationTemp[i][j] = val;
                            }
                        }

                        if (cheatClick == false) {
                            for (unsigned int i = 0; i < col; i++) {                      //Print board
                                for (unsigned int j = 0; j < row; j++) {
                                    if (checkBoard[i][j]) {
                                        myBoard[i][j] = mine;
                                        middleGraph[i][j] = tile;
                                    }
                                }
                            }
                        }

                    }

                    else if (theTestOne.contains(mousePosFloat)) {
                        isLose = false;
                        for (unsigned int i = 0; i < col; i++) {                      //Print board
                            for (unsigned int j = 0; j < row; j++) {
                                isClicked[i][j] = false;
                                isFlag[i][j] = false;
                                middleGraph[i][j] = tile;
                            }
                        }
                        int coutingOne = 0;
                        file.open("boards/testboard1.brd");
                        openBoard(file, checkBoardTemp, row, col);
                        for (unsigned int i = 0; i < col; i++) {
                            for (unsigned int j = 0; j < row; j++) {
                                if (checkBoardTemp[i][j] == true) {
                                    myBoard[i][j] = tile;
                                    checkBoard[i][j] = true;
                                    coutingOne++;
                                }
                                else {
                                    myBoard[i][j] = tile;
                                    checkBoard[i][j] = false;
                                }
                            }
                        }
                        mineLeft = coutingOne;
                        isWin = tileCount - coutingOne;
                        file.close();
                        for (unsigned int i = 0; i < col; i++) {                     //Actual board

                            for (unsigned int j = 0; j < row; j++) {

                                unsigned int val = 0;
                                if (checkBoard[i][j] == true) {
                                    saveLocationTemp[i][j] = 9;
                                    continue;
                                }

                                if (j == 0) {                                            //edge on the right
                                    if (i == 0) {
                                        if (checkBoard[i + 1][j + 1] == true) val++;
                                        if (checkBoard[i][j + 1] == true) val++;
                                        if (checkBoard[i + 1][j] == true) val++;
                                    }
                                    else if (i == col - 1) {
                                        if (checkBoard[i - 1][j] == true) val++;
                                        if (checkBoard[i - 1][j + 1] == true) val++;
                                        if (checkBoard[i][j + 1] == true) val++;
                                    }
                                    else {
                                        if (checkBoard[i - 1][j] == true) val++;
                                        if (checkBoard[i + 1][j] == true) val++;
                                        if (checkBoard[i][j + 1] == true) val++;
                                        if (checkBoard[i - 1][j + 1] == true) val++;
                                        if (checkBoard[i + 1][j + 1] == true) val++;
                                    }
                                }

                                else if (j == row - 1) {                                   //edge on the left
                                    if (i == 0) {
                                        if (checkBoard[i + 1][j - 1] == true) val++;
                                        if (checkBoard[i][j - 1] == true) val++;
                                        if (checkBoard[i + 1][j] == true) val++;
                                    }
                                    else if (i == col - 1) {
                                        if (checkBoard[i - 1][j] == true) val++;
                                        if (checkBoard[i - 1][j - 1] == true) val++;
                                        if (checkBoard[i][j - 1] == true) val++;
                                    }
                                    else {
                                        if (checkBoard[i - 1][j] == true) val++;
                                        if (checkBoard[i + 1][j] == true) val++;
                                        if (checkBoard[i][j - 1] == true) val++;
                                        if (checkBoard[i - 1][j - 1] == true) val++;
                                        if (checkBoard[i + 1][j - 1] == true) val++;
                                    }
                                }

                                else if (i == 0 && j != 0) {                             //Side top
                                    if (checkBoard[i + 1][j + 1] == true) val++;
                                    if (checkBoard[i + 1][j - 1] == true) val++;
                                    if (checkBoard[i][j + 1] == true) val++;
                                    if (checkBoard[i][j - 1] == true) val++;
                                    if (checkBoard[i + 1][j] == true) val++;
                                }

                                else if (i == col - 1 && j != row - 1) {                     //Side bottom
                                    if (checkBoard[i - 1][j + 1] == true) val++;
                                    if (checkBoard[i - 1][j - 1] == true) val++;
                                    if (checkBoard[i][j + 1] == true) val++;
                                    if (checkBoard[i][j - 1] == true) val++;
                                    if (checkBoard[i - 1][j] == true) val++;
                                }

                                else {  //Mid point
                                    if (checkBoard[i - 1][j + 1] == true) val++;
                                    if (checkBoard[i][j + 1] == true) val++;
                                    if (checkBoard[i][j - 1] == true) val++;
                                    if (checkBoard[i + 1][j - 1] == true) val++;
                                    if (checkBoard[i - 1][j - 1] == true) val++;
                                    if (checkBoard[i + 1][j + 1] == true) val++;
                                    if (checkBoard[i - 1][j] == true) val++;
                                    if (checkBoard[i + 1][j] == true) val++;
                                }

                                if (val == 0) actualBoard[i][j] = tileRevealed;
                                else if (val == 1) actualBoard[i][j] = numOne;
                                else if (val == 2) actualBoard[i][j] = numTwo;
                                else if (val == 3) actualBoard[i][j] = numThree;
                                else if (val == 4) actualBoard[i][j] = numFour;
                                else if (val == 5) actualBoard[i][j] = numFive;
                                else if (val == 6) actualBoard[i][j] = numSix;
                                else if (val == 7) actualBoard[i][j] = numSeven;
                                else if (val == 8) actualBoard[i][j] = numEight;
                                saveLocationTemp[i][j] = val;
                            }
                        }

                        if (cheatClick == false) {
                            for (unsigned int i = 0; i < col; i++) {                      //Print board
                                for (unsigned int j = 0; j < row; j++) {
                                    if (checkBoard[i][j]) {
                                        myBoard[i][j] = mine;
                                        middleGraph[i][j] = tile;
                                    }
                                }
                            }
                        }
                    }

                    else if (theTestTwo.contains(mousePosFloat)) {
                        isLose = false;
                        for (unsigned int i = 0; i < col; i++) {                      //Print board
                            for (unsigned int j = 0; j < row; j++) {
                                isClicked[i][j] = false;
                                isFlag[i][j] = false;
                                middleGraph[i][j] = tile;
                            }
                        }
                        int countingTwo = 0;
                        file.open("boards/testboard2.brd");
                        openBoard(file, checkBoardTemp, row, col);
                        for (unsigned int i = 0; i < col; i++) {
                            for (unsigned int j = 0; j < row; j++) {
                                if (checkBoardTemp[i][j] == true) {
                                    countingTwo += 1;
                                    myBoard[i][j] = tile;
                                    checkBoard[i][j] = true;
                                }
                                else {
                                    myBoard[i][j] = tile;
                                    checkBoard[i][j] = false;
                                }
                            }
                        }
                        mineLeft = countingTwo;
                        isWin = tileCount - countingTwo;
                        file.close();
                        for (unsigned int i = 0; i < col; i++) {                     //Actual board

                            for (unsigned int j = 0; j < row; j++) {

                                unsigned int val = 0;
                                if (checkBoard[i][j] == true) {
                                    saveLocationTemp[i][j] = 9;
                                    continue;
                                }

                                if (j == 0) {                                            //edge on the right
                                    if (i == 0) {
                                        if (checkBoard[i + 1][j + 1] == true) val++;
                                        if (checkBoard[i][j + 1] == true) val++;
                                        if (checkBoard[i + 1][j] == true) val++;
                                    }
                                    else if (i == col - 1) {
                                        if (checkBoard[i - 1][j] == true) val++;
                                        if (checkBoard[i - 1][j + 1] == true) val++;
                                        if (checkBoard[i][j + 1] == true) val++;
                                    }
                                    else {
                                        if (checkBoard[i - 1][j] == true) val++;
                                        if (checkBoard[i + 1][j] == true) val++;
                                        if (checkBoard[i][j + 1] == true) val++;
                                        if (checkBoard[i - 1][j + 1] == true) val++;
                                        if (checkBoard[i + 1][j + 1] == true) val++;
                                    }
                                }

                                else if (j == row - 1) {                                   //edge on the left
                                    if (i == 0) {
                                        if (checkBoard[i + 1][j - 1] == true) val++;
                                        if (checkBoard[i][j - 1] == true) val++;
                                        if (checkBoard[i + 1][j] == true) val++;
                                    }
                                    else if (i == col - 1) {
                                        if (checkBoard[i - 1][j] == true) val++;
                                        if (checkBoard[i - 1][j - 1] == true) val++;
                                        if (checkBoard[i][j - 1] == true) val++;
                                    }
                                    else {
                                        if (checkBoard[i - 1][j] == true) val++;
                                        if (checkBoard[i + 1][j] == true) val++;
                                        if (checkBoard[i][j - 1] == true) val++;
                                        if (checkBoard[i - 1][j - 1] == true) val++;
                                        if (checkBoard[i + 1][j - 1] == true) val++;
                                    }
                                }

                                else if (i == 0 && j != 0) {                             //Side top
                                    if (checkBoard[i + 1][j + 1] == true) val++;
                                    if (checkBoard[i + 1][j - 1] == true) val++;
                                    if (checkBoard[i][j + 1] == true) val++;
                                    if (checkBoard[i][j - 1] == true) val++;
                                    if (checkBoard[i + 1][j] == true) val++;
                                }

                                else if (i == col - 1 && j != row - 1) {                     //Side bottom
                                    if (checkBoard[i - 1][j + 1] == true) val++;
                                    if (checkBoard[i - 1][j - 1] == true) val++;
                                    if (checkBoard[i][j + 1] == true) val++;
                                    if (checkBoard[i][j - 1] == true) val++;
                                    if (checkBoard[i - 1][j] == true) val++;
                                }

                                else {  //Mid point
                                    if (checkBoard[i - 1][j + 1] == true) val++;
                                    if (checkBoard[i][j + 1] == true) val++;
                                    if (checkBoard[i][j - 1] == true) val++;
                                    if (checkBoard[i + 1][j - 1] == true) val++;
                                    if (checkBoard[i - 1][j - 1] == true) val++;
                                    if (checkBoard[i + 1][j + 1] == true) val++;
                                    if (checkBoard[i - 1][j] == true) val++;
                                    if (checkBoard[i + 1][j] == true) val++;
                                }

                                if (val == 0) actualBoard[i][j] = tileRevealed;
                                else if (val == 1) actualBoard[i][j] = numOne;
                                else if (val == 2) actualBoard[i][j] = numTwo;
                                else if (val == 3) actualBoard[i][j] = numThree;
                                else if (val == 4) actualBoard[i][j] = numFour;
                                else if (val == 5) actualBoard[i][j] = numFive;
                                else if (val == 6) actualBoard[i][j] = numSix;
                                else if (val == 7) actualBoard[i][j] = numSeven;
                                else if (val == 8) actualBoard[i][j] = numEight;
                                saveLocationTemp[i][j] = val;
                            }
                        }

                        if (cheatClick == false) {
                            for (unsigned int i = 0; i < col; i++) {                      //Print board
                                for (unsigned int j = 0; j < row; j++) {
                                    if (checkBoard[i][j]) {
                                        myBoard[i][j] = mine;
                                        middleGraph[i][j] = tile;
                                    }
                                }
                            }
                        }
                    }

                    else if (theTestThree.contains(mousePosFloat)) {
                        isLose = false;
                        for (unsigned int i = 0; i < col; i++) {                      //Print board
                            for (unsigned int j = 0; j < row; j++) {
                                isClicked[i][j] = false;
                                isFlag[i][j] = false;
                                middleGraph[i][j] = tile;
                            }
                        }
                        int countingThree = 0;
                        file.open("boards/testboard3.brd");
                        openBoard(file, checkBoardTemp, row, col);
                        for (unsigned int i = 0; i < col; i++) {
                            for (unsigned int j = 0; j < row; j++) {
                                if (checkBoardTemp[i][j] == true) {
                                    myBoard[i][j] = tile;
                                    checkBoard[i][j] = true;
                                    countingThree++;
                                }
                                else {
                                    myBoard[i][j] = tile;
                                    checkBoard[i][j] = false;
                                }
                            }
                        }
                        mineLeft = countingThree;
                        isWin = tileCount - countingThree;
                        file.close();
                        for (unsigned int i = 0; i < col; i++) {                     //Actual board

                            for (unsigned int j = 0; j < row; j++) {

                                unsigned int val = 0;
                                if (checkBoard[i][j] == true) {
                                    saveLocationTemp[i][j] = 9;
                                    continue;
                                }

                                if (j == 0) {                                            //edge on the right
                                    if (i == 0) {
                                        if (checkBoard[i + 1][j + 1] == true) val++;
                                        if (checkBoard[i][j + 1] == true) val++;
                                        if (checkBoard[i + 1][j] == true) val++;
                                    }
                                    else if (i == col - 1) {
                                        if (checkBoard[i - 1][j] == true) val++;
                                        if (checkBoard[i - 1][j + 1] == true) val++;
                                        if (checkBoard[i][j + 1] == true) val++;
                                    }
                                    else {
                                        if (checkBoard[i - 1][j] == true) val++;
                                        if (checkBoard[i + 1][j] == true) val++;
                                        if (checkBoard[i][j + 1] == true) val++;
                                        if (checkBoard[i - 1][j + 1] == true) val++;
                                        if (checkBoard[i + 1][j + 1] == true) val++;
                                    }
                                }

                                else if (j == row - 1) {                                   //edge on the left
                                    if (i == 0) {
                                        if (checkBoard[i + 1][j - 1] == true) val++;
                                        if (checkBoard[i][j - 1] == true) val++;
                                        if (checkBoard[i + 1][j] == true) val++;
                                    }
                                    else if (i == col - 1) {
                                        if (checkBoard[i - 1][j] == true) val++;
                                        if (checkBoard[i - 1][j - 1] == true) val++;
                                        if (checkBoard[i][j - 1] == true) val++;
                                    }
                                    else {
                                        if (checkBoard[i - 1][j] == true) val++;
                                        if (checkBoard[i + 1][j] == true) val++;
                                        if (checkBoard[i][j - 1] == true) val++;
                                        if (checkBoard[i - 1][j - 1] == true) val++;
                                        if (checkBoard[i + 1][j - 1] == true) val++;
                                    }
                                }

                                else if (i == 0 && j != 0) {                             //Side top
                                    if (checkBoard[i + 1][j + 1] == true) val++;
                                    if (checkBoard[i + 1][j - 1] == true) val++;
                                    if (checkBoard[i][j + 1] == true) val++;
                                    if (checkBoard[i][j - 1] == true) val++;
                                    if (checkBoard[i + 1][j] == true) val++;
                                }

                                else if (i == col - 1 && j != row - 1) {                     //Side bottom
                                    if (checkBoard[i - 1][j + 1] == true) val++;
                                    if (checkBoard[i - 1][j - 1] == true) val++;
                                    if (checkBoard[i][j + 1] == true) val++;
                                    if (checkBoard[i][j - 1] == true) val++;
                                    if (checkBoard[i - 1][j] == true) val++;
                                }

                                else {  //Mid point
                                    if (checkBoard[i - 1][j + 1] == true) val++;
                                    if (checkBoard[i][j + 1] == true) val++;
                                    if (checkBoard[i][j - 1] == true) val++;
                                    if (checkBoard[i + 1][j - 1] == true) val++;
                                    if (checkBoard[i - 1][j - 1] == true) val++;
                                    if (checkBoard[i + 1][j + 1] == true) val++;
                                    if (checkBoard[i - 1][j] == true) val++;
                                    if (checkBoard[i + 1][j] == true) val++;
                                }

                                if (val == 0) actualBoard[i][j] = tileRevealed;
                                else if (val == 1) actualBoard[i][j] = numOne;
                                else if (val == 2) actualBoard[i][j] = numTwo;
                                else if (val == 3) actualBoard[i][j] = numThree;
                                else if (val == 4) actualBoard[i][j] = numFour;
                                else if (val == 5) actualBoard[i][j] = numFive;
                                else if (val == 6) actualBoard[i][j] = numSix;
                                else if (val == 7) actualBoard[i][j] = numSeven;
                                else if (val == 8) actualBoard[i][j] = numEight;
                                saveLocationTemp[i][j] = val;
                            }
                        }

                        if (cheatClick == false) {
                            for (unsigned int i = 0; i < col; i++) {                      //Print board
                                for (unsigned int j = 0; j < row; j++) {
                                    if (checkBoard[i][j]) {
                                        myBoard[i][j] = mine;
                                        middleGraph[i][j] = tile;
                                    }
                                }
                            }
                        }

                    }

                }

                else if (event.mouseButton.button == Mouse::Right && y_pos <= row - 1) {
                    if (!isFlag[x_pos][y_pos] && isClicked[x_pos][y_pos] != true) {
                        mineLeft--;
                        middleGraph[x_pos][y_pos] = tile;
                        myBoard[x_pos][y_pos] = flag;
                        tempFlag = saveLocationTemp[x_pos][y_pos];
                        saveLocationTemp[x_pos][y_pos] = -1;
                        isFlag[x_pos][y_pos] = true;
                    }

                    else if (isFlag[x_pos][y_pos] && isClicked[x_pos][y_pos] != true) {
                        mineLeft++;
                        isFlag[x_pos][y_pos] = false;
                        saveLocationTemp[x_pos][y_pos] = tempFlag;
                        myBoard[x_pos][y_pos] = tile;
                    }
                }

            }
        }

        window.clear(Color::White);

        if (isLose == false) {
            face.setPosition(windowWidth / 2 - 32, row * width);
            window.draw(face);
        }

        else if (isLose == true) {
            sadFace.setPosition(windowWidth / 2 - 32, row * width);
            window.draw(sadFace);
        }
        if (isWin == 0) {

            winFace.setPosition(windowWidth / 2 - 32, row * width);
            window.draw(winFace);
            mineLeft = 0;
            for (int i = 0; i < col; i++) {
                for (int j = 0; j < row; j++) {
                    middleGraph[i][j] = tileRevealed;
                    if (checkBoard[i][j] && cheatClick == true) {
                        myBoard[i][j] = flag;
                        middleGraph[i][j] = tile;
                        isLose = true;
                    }
                    else if (checkBoard[i][j] && cheatClick == false) {
                        myBoard[i][j] = mine;
                        middleGraph[i][j] = flag;
                    }
                    isClicked[i][j] = true;
                }
            }
        }

        for (unsigned int i = 0; i < col; i++) {
            for (unsigned int j = 0; j < row; j++) {
                //myBoard[i][j].setTextureRect(IntRect(0,0,32,32));
                myBoard[i][j].setPosition(i * 32, j * 32);
                example[i][j].setPosition(i * 32, j * 32);
                middleGraph[i][j].setPosition(i * 32, j * 32);
                window.draw(example[i][j]);
                window.draw(middleGraph[i][j]);
                window.draw(myBoard[i][j]);
            }
        }

        string valueString = conversion(mineLeft);

        string valuePlug(1, valueString.at(0));
        string valuePlugTwo(1, valueString.at(1));
        string valuePlugThree(1, valueString.at(2));

        if (mineLeft < 0) {
            digit.setTextureRect(IntRect(10 * 21, 0, 21, 32));
            digit.setPosition(0, row * width);
            window.draw(digit);

            digit.setTextureRect(IntRect(stoi(valuePlug) * 21, 0, 21, 32));
            digit.setPosition(21, row * width);
            window.draw(digit);

            digit.setTextureRect(IntRect(stoi(valuePlugTwo) * 21, 0, 21, 32));
            digit.setPosition(21 * 2, row * width);
            window.draw(digit);

            digit.setTextureRect(IntRect(stoi(valuePlugThree) * 21, 0, 21, 32));
            digit.setPosition(21 * 3, row * width);
            window.draw(digit);
        }

        else {
            digit.setTextureRect(IntRect(stoi(valuePlug) * 21, 0, 21, 32));
            digit.setPosition(0, row * width);
            window.draw(digit);

            digit.setTextureRect(IntRect(stoi(valuePlugTwo) * 21, 0, 21, 32));
            digit.setPosition(21, row * width);
            window.draw(digit);

            digit.setTextureRect(IntRect(stoi(valuePlugThree) * 21, 0, 21, 32));
            digit.setPosition(21 * 2, row * width);
            window.draw(digit);
        }

        bomb.setPosition(windowWidth / 2 + 96, row * width);
        window.draw(bomb);

        testOne.setPosition(windowWidth / 2 + 96 + 64, row * width);
        window.draw(testOne);

        testTwo.setPosition(windowWidth / 2 + 96 + 64 * 2, row * width);
        window.draw(testTwo);

        testThree.setPosition(windowWidth / 2 + 96 + 64 * 3, row * width);
        window.draw(testThree);

        window.display();
    }

    return 0;
}
