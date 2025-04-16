#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "InputManager.h"

void InputManager::Update() {
	glfwPollEvents();
	double xPosition = 0;
	double yPosition = 0;
	GetMousePosition(&xPosition, &yPosition);
	//std::cout << "Mouse Position: (" << xPosition << "," << yPosition << ")" << std::endl;
}

bool InputManager::KeyIsPressed(int keyVal) {
	if (glfwGetKey(thisWindow, keyVal) == GLFW_PRESS) {
		return true;
	}
	return false;
}

void InputManager::SetWindow(GLFWwindow* newWindow) {
	thisWindow = newWindow;
}

void InputManager::GetMousePosition(double* xPos, double* yPos) {
	glfwGetCursorPos(thisWindow, xPos, yPos);
}