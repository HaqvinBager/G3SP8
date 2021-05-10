#pragma once
#include "WinInclude.h"
#include <Windowsx.h>
#include <bitset>
#include <memory>
#include <stack>

#define INPUT Input::GetInstance()

//#define INPUT_AXIS_USES_FALLOFF

class Input {
public:

	static Input* GetInstance();

	enum class EMouseButton {
		Left = 0,
		Right = 1,
		Middle = 2,
		Mouse4 = 3,
		Mouse5 = 4

	};//regular enum was easier to use.

	enum class EAxis {
		Horizontal = 0,
		Vertical = 1
	};

	Input();

	bool UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam);
	void Update();


	bool MoveLeft();
	bool MoveRight();
	bool MoveUp();
	bool MoveDown();

	DirectX::SimpleMath::Vector2 GetAxisRaw();


	float GetAxis(const EAxis& aAxis);
	bool IsKeyDown(WPARAM wParam);
	bool IsKeyPressed(WPARAM wParam);
	bool IsKeyReleased(WPARAM wParam);

	int MouseX();//x coordiantes in app window
	int MouseY();//y coordiantes in app window
	int MouseScreenX();
	int MouseScreenY();
	int MouseDeltaX();
	int MouseDeltaY();
	int MouseRawDeltaX();
	int MouseRawDeltaY();
	int MouseWheel();
	void SetMouseScreenPosition(int x, int y);
	bool IsMouseDown(EMouseButton aMouseButton);
	bool IsMousePressed(EMouseButton aMouseButton);
	bool IsMouseReleased(EMouseButton aMouseButton);

private:
	void UpdateAxisUsingFallOff();
	void UpdateAxisUsingNoFallOff();
	const float GetAxisUsingFallOff(const EAxis& anAxis);
	const float GetAxisUsingNoFallOff(const EAxis& anAxis);

private:
	std::bitset<5> myMouseButtonLast;
	std::bitset<5> myMouseButton;

	std::bitset<256> myKeyDownLast;
	std::bitset<256> myKeyDown;

	int myMouseX;
	int myMouseY;
	int myMouseScreenX;
	int myMouseScreenY;
	int myMouseXLast;
	int myMouseYLast;
	int myMouseRawXLast;
	int myMouseRawYLast;
	int myMouseWheel;//positive = away from user, negative = towards user

	bool myHorizontalPressed;
	bool myVerticalPressed;
	float myHorizontal;
	float myVertical;
};