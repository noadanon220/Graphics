// ============== main.cpp ==============
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include "Game.h"
#include "glut.h"


Game* game;
bool showVisibility = false;
int visibilityTeam = 0;

void init() {
    srand(time(0));
    glClearColor(0.2, 0.2, 0.2, 0);
    glOrtho(0, MAP_SIZE, 0, MAP_SIZE, -1, 1);

    game = new Game();
    game->Initialize();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (showVisibility) {
        game->ShowVisibilityMap(visibilityTeam);
    }
    else {
        game->Show();
    }

    game->ShowGameStats();

    glutSwapBuffers();
}

void idle() {
    game->Update();

    if (game->CheckGameEnd()) {
        std::cout << "Game Over!" << std::endl;
        // Don't exit, just display winner
    }

    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 27: // ESC
        exit(0);
        break;
    case 'v':
    case 'V':
        showVisibility = !showVisibility;
        break;
    case '1':
        visibilityTeam = 0;
        break;
    case '2':
        visibilityTeam = 1;
        break;
    case 'r':
    case 'R':
        delete game;
        game = new Game();
        game->Initialize();
        break;
    }
}

void menu(int option) {
    switch (option) {
    case 1: // New Game
        delete game;
        game = new Game();
        game->Initialize();
        break;
    case 2: // Toggle Visibility
        showVisibility = !showVisibility;
        break;
    case 3: // Show Team 1 Visibility
        showVisibility = true;
        visibilityTeam = 0;
        break;
    case 4: // Show Team 2 Visibility
        showVisibility = true;
        visibilityTeam = 1;
        break;
    case 5: // Exit
        exit(0);
        break;
    }
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(900, 900);
    glutInitWindowPosition(200, 100);
    glutCreateWindow("AI Tactical Combat Game");

    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);

    // Create menu
    glutCreateMenu(menu);
    glutAddMenuEntry("New Game", 1);
    glutAddMenuEntry("Toggle Visibility", 2);
    glutAddMenuEntry("Show Team 1 Vision", 3);
    glutAddMenuEntry("Show Team 2 Vision", 4);
    glutAddMenuEntry("Exit", 5);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    init();

    std::cout << "=== AI Tactical Combat Game ===" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "V - Toggle visibility map" << std::endl;
    std::cout << "1/2 - Switch between team visibility" << std::endl;
    std::cout << "R - Restart game" << std::endl;
    std::cout << "Right click - Menu" << std::endl;
    std::cout << "ESC - Exit" << std::endl;

    glutMainLoop();
    return 0;
}