#pragma once
#include "GameObject.h"
#include <vector>
using namespace std;

#define WHITE_WHIP	0
#define BLUE_WHIP	1
#define YELLOW_WHIP 2
//#define RED_WHIP 3

#define WHITE_WHIP_BBOX_WIDTH	22
#define WHITE_WHIP_BBOX_HEIGHT	8

#define BLUE_WHIP_BBOX_WIDTH	24
#define BLUE_WHIP_BBOX_HEIGHT	8

#define YELLOW_WHIP_BBOX_WIDTH	39
#define YELLOW_WHIP_BBOX_HEIGHT	8

//#define RED_WHIP_BBOX_WIDTH	   39
//#define RED_WHIP_BBOX_HEIGHT	8

#define WHIP_STATE_LEFT 0
#define WHIP_STATE_RIGHT 1

#define SHORT_WHIP 0
#define LONG_WHIP 1

#define FRAME_TIME_WHIP 150
class CWhip :
	public CGameObject
{
	int currentFrame;
	int state_whip;
	int size;
	DWORD FrameWhip;
	int damage;
	bool IsSubHP;
public:
	bool fight;
	CWhip();
	void Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects = NULL);
	void Render();
	void GetBoundingBox(float & left, float & top, float & right, float & bottom);
	void SetState(int state);
	void FindPosstion();
	void SetStateWhip(int state_whip) { this->state_whip = state_whip; }
	int GetSize() { return size; }
	DWORD GetFrameWhip() { return FrameWhip; }
	void SetFrameWhip(int FrameWhip) { this->FrameWhip = FrameWhip; }
	void SetCurrentFrame(int currentFrame) { this->currentFrame = currentFrame; }
	int GetCurrentFrame() { return currentFrame; }
};

