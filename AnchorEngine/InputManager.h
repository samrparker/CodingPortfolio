
#include <iostream>

class InputManager {
public:
    void Update();
    bool KeyIsPressed(int keyVal);
    void SetWindow(GLFWwindow* newWindow);

    void GetMousePosition(double* xPos, double* yPos);

    GLFWwindow* thisWindow;
};