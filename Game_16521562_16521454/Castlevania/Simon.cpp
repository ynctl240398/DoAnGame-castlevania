#include "Simon.h"
#include "debug.h"
#include "Candle.h"
#include "Game.h"
#include "HidenObject.h"
#include "EntranceLevel.h"

CSimon *CSimon::__instance = NULL;
void CSimon::Update(DWORD dt, vector<LPGAMEOBJECT>* coObject)
{
	CGameObject::Update(dt);
	CEntranceLevel *level1 = CEntranceLevel::GetInstance();
	CGame *game = CGame::GetInstance();
	float widthofmap;
	float heightofmap;
	level1->GetSizeOfMap(widthofmap, heightofmap);
	float temp_x;
	if (stair == 0)
		vy += SIMON_GRAVITY;
	if (state != SIMON_STATE_KNEE && previousstate == SIMON_STATE_KNEE)
	{
		previousstate = NULL;
		SetPosition(x, y - 9);
	}
	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;
	coEventsResult.clear();
	if (stair != 2)
		stair = 0;
	if (state != SIMON_STATE_DIE)
		CalcPotentialCollisions(coObject, coEvents);

	if (fight == true)
	{
		whip->SetPosition(x, y);
		DWORD t = GetTickCount() - whip->GetFrameWhip();
		if (t >= 300)
		{
			whip->fight = true;
			if (t >= 450)
			{
				fight = false;
				whip->fight = false;
			}
		}
		whip->Update(dt, coObject);
	}
	if (skill)
	{
		weapon->Update(dt);
	}
	if (coEvents.size() == 0)
	{
		x += dx;
		y += dy;
	}
	else
	{

		float min_tx, min_ty, nx = 0, ny;

		for (UINT i = 0; i < coEvents.size(); i++)
		{
			LPCOLLISIONEVENT e = coEvents[i];
			if (dynamic_cast<CCandle *> (e->obj))
				coEvents.erase(coEvents.begin() + i);
		}
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny);
		x += min_tx * dx + nx * 0.4f;
		y += min_ty * dy + ny * 0.4f;


		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
			if (dynamic_cast<CHidenObject *> (e->obj))
			{
				CHidenObject *hidenobject = dynamic_cast<CHidenObject *>(e->obj);
				if (nx != 0) vx = 0;
				if (ny != 0) {
					vy = 0; jump = true;
				}
				if (hidenobject->GetState() == HIDENOBJECT_STATE_STAIR_UP)
				{
					x += dx;
				}
				if (hidenobject->GetState() == HIDENOBJECT_STATE_STAIR_DOWN)
				{
					x += dx;
				}
			}
			else if (dynamic_cast<CHeart *>(e->obj))
			{
				CItems *items = dynamic_cast<CHeart *>(e->obj);
				x += dx;
				y += dy;
				items->SetState(ITEM_STATE_DELETE);
			}
			else if (dynamic_cast<CDagger *>(e->obj))
			{
				OnSkill = true;
				CItems *items = dynamic_cast<CDagger *>(e->obj);
				weapon = new CDagger();
				weapon->LoadData();
				x += dx;
				items->SetState(ITEM_STATE_DELETE);
			}
			else if (dynamic_cast<CDoor *>(e->obj))
			{
				level1->SetScene(SCENE_2);
				SetPosition(10.0f, 80.0f);
				game->SetCamera(0.0f, 0.0f);
			}
		}
		coEventsResult.clear();
		FilterCollisionImmediately(coEvents, coEventsResult);
		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
			if (dynamic_cast<CHidenObject *> (e->obj))
			{
				CHidenObject *hidenobject = dynamic_cast<CHidenObject *>(e->obj);
				if (hidenobject->GetState() == HIDENOBJECT_STATE_STAIR_UP)
				{
					stair_x = hidenobject->x;
					if (stair != 2)
						stair = 1;
					if (stair == 2 && IsDown != 2)
						IsDown = 1;
				}
				if (hidenobject->GetState() == HIDENOBJECT_STATE_STAIR_DOWN)
				{
					stair_x = hidenobject->x;
					if (stair != 2)
						stair = 3;
					if (stair == 2 && IsUp != 2)
						IsUp = 1;
				}
			}
		}
	}
	if (state_auto == 1)
	{
		if (x > stair_x)
		{
			SetState(SIMON_STATE_WALKING_LEFT);
			state_auto = 2;
		}
		else if (x < stair_x)
		{
			state_auto == 3;
			SetState(state = SIMON_STATE_WALKING_RIGHT);
		}
	}
	else if (state_auto == 2)
	{
		if (x <= stair_x)
		{
			if (stair == 1)
			{
				SetState(SIMON_STATE_STAIR);
				x = stair_x;
				nx = 1;
				x += 8;
			}
			else
			{
				SetState(SIMON_STATE_STAIR);
				x = stair_x;
				nx = -1;
				x -= 8;
			}
			SetFrameStair();
			GetPosition(simon_x, simon_y);
			stair = 2;
			state_auto = 0;
			if (nx < 0)
				y += 7;
		}
	}
	else if (state_auto == 3)
	{
		if (x >= stair_x)
		{
			if (stair == 1)
			{
				SetState(SIMON_STATE_STAIR);
				x = stair_x;
				nx = 1;
				x += 8;
			}
			else
			{
				SetState(SIMON_STATE_STAIR);
				x = stair_x;
				nx = -1;
				x -= 8;
			}
			SetFrameStair();
			GetPosition(simon_x, simon_y);
			stair = 2;
			state_auto = 0;
			if (nx < 0)
				y += 7;
		}
	}
	if (state == SIMON_STATE_STAIR)
	{
		DWORD now = GetTickCount();
		test_stair = now - FrameStair;
	}
	else
		test_stair = 0;
	if (state != SIMON_STATE_STAIR)
	{
		float cx, cy;
		game->GetCamera(cx, cy);
		if (x > 100.0f)
		{
			game->SetCamera(x - 100.0f, 0.0f);
			if (cx < 0) game->SetCamera(0.0f, 0.0f);
			if (cx + 256.0f >= widthofmap)
			{
				game->SetCamera(cx, 0.0f);
				if (x < 620.0f) game->SetCamera(x - 100.0f, 0.0f);
			}
		}
	}
	else
	{
		float x1;
		if (nx > 0)
			x1 = simon_x + 0.02*test_stair;
		else
			x1 = simon_x - 0.02*test_stair;
		float cx, cy;
		game->GetCamera(cx, cy);
		if (x1 > 100.0f)
		{
			game->SetCamera(x1 - 100.0f, 0.0f);
			if (cx < 0) game->SetCamera(0.0f, 0.0f);
			if (cx + 256.0f >= widthofmap)
			{
				game->SetCamera(cx, 0.0f);
				if (x1 < 620.0f) game->SetCamera(x1 - 100.0f, 0.0f);
			}
		}
	}
	DebugOut(L"x=%f\n", x);

	//DebugOut(L"y=%f\n", y);

	//DebugOut(L"IsUp=%d\n", IsUp);

	//sau nay se sua thanh CDoor
	//if (x >= 704.0f)
	//{
	//	
	//	level1->SetScene(SCENE_2);
	//	SetPosition(10.0f, 80.0f);
	//	game->SetCamera(0.0f, 0.0f);
	//	//level1->Update();
	//}
}

void CSimon::Render()
{
	int ani;

	if (stair == 2 || state_auto == 4)
	{
		DWORD now = GetTickCount();
		if (state == SIMON_STATE_STAIR)
		{
			if (nx > 0)
				ani = SIMON_ANI_STAIR_RIGHT;
			else
				ani = SIMON_ANI_STAIR_LEFT;
		}
		else
		{
			if (skill == true && now - FrameWeapon < 450)
			{
				if (nx > 0)
					ani = SIMON_ANI_STAIR_FIGHT_RIGHT;
				else
					ani = SIMON_ANI_STAIR_FIGHT_LEFT;
				animations[ani]->Render(x, y, 255);
			}
			else
			{
				if (fight == true)
				{
					if (nx > 0)
						ani = SIMON_ANI_STAIR_FIGHT_RIGHT;
					else
						ani = SIMON_ANI_STAIR_FIGHT_LEFT;
					whip->Render();
				}
				else
				{
					if (nx > 0)
						ani = SIMON_ANI_STAIR_IDLE_RIGHT;
					else
						ani = SIMON_ANI_STAIR_IDLE_LEFT;
				}
			}

		}
		if (state == SIMON_STATE_STAIR)
		{
			if (animations[ani]->GetCureentFrame() == 0 && test == 0)
			{
				test = 1;
				if (nx > 0)
				{
					x += 8;
					y -= 8;
				}
				else
				{
					x -= 8;
					y += 8;
				}
			}
			else if (animations[ani]->GetCureentFrame() == 1 && test == 1)
			{
				test = 0;
			}
		}
		if (skill)
		{
			if (now - FrameWeapon > 500)	skill = false;
			else weapon->Render();
		}
		animations[ani]->Render(x, y, 255);
	}
	else
	{
		if (state == SIMON_STATE_UPDATE)
		{

			if (state_update == SIMON_STATE_WALKING_LEFT)
				ani = SIMON_ANI_WALKING_LEFT;
			else
				ani = SIMON_ANI_WALKING_RIGHT;
			if (alpha == 255)
			{
				animations[ani]->Render_now(x, y, 255, alpha, 28, 36);
				alpha = 237;
			}
			else
			{
				animations[ani]->Render_now(x, y);
				alpha = 255;
			}
		}
		else
		{
			DWORD now = GetTickCount();
			if (skill == true && now - FrameWeapon < 450)
			{
				if (nx > 0)
					ani = SIMON_ANI_FIGHT_RIGHT;
				else
					ani = SIMON_ANI_FIGHT_LEFT;
				animations[ani]->Render(x, y, 255);
			}
			else
			{
				if (fight == true)
				{
					if (vx == 0)
					{
						if (mx == 1)
						{
							if (nx > 0)
								ani = SIMON_ANI_KNEE_FIGHT_RIGHT;
							else
								ani = SIMON_ANI_KNEE_FIGHT_LEFT;
						}
						else
						{
							if (nx > 0)
								ani = SIMON_ANI_FIGHT_RIGHT;
							else
								ani = SIMON_ANI_FIGHT_LEFT;
						}
					}
					/*				whip->Render();
									animations[ani]->Render(x, y, 255);*/
					if (nx > 0)whip->SetStateWhip(WHIP_STATE_RIGHT);
					else whip->SetStateWhip(WHIP_STATE_LEFT);
					animations[ani]->Render(x, y, 255);
					whip->Render();
				}
				else
				{
					if (vx == 0)
					{
						if (mx == 1)
						{
							if (nx > 0)
								ani = SIMON_ANI_JUMP_RIGHT; //SIMON_ANI_KNEE_RIGHT
							else
								ani = SIMON_ANI_JUMP_LEFT; //SIMON_ANI_KNEE_LEFT
						}
						else
						{
							if (nx > 0)
								ani = SIMON_ANI_IDLE_RIGHT;
							else
								ani = SIMON_ANI_IDLE_LEFT;
						}
					}
					else
					{
						if (nx > 0)
							ani = SIMON_ANI_WALKING_RIGHT;
						else
							ani = SIMON_ANI_WALKING_LEFT;
					}
					if (vy < 0 && nx < 0)
						ani = SIMON_ANI_JUMP_LEFT;
					else if (vy < 0 && nx > 0)
						ani = SIMON_ANI_JUMP_RIGHT;
					animations[ani]->Render(x, y, 255);
				}
			}
			if (skill)
			{
				if (now - FrameWeapon > 500)	skill = false;
				else weapon->Render();
			}
		}
	}
	DebugOut(L"ani=%d\n", ani);
}

void CSimon::GetBoundingBox(float & left, float & top, float & right, float & bottom)
{
	left = x;
	top = y;
	if (vx == 0 && mx == 1)
	{
		right = x + SIMON_BBOX_KNEE_WIDTH;
		bottom = y + SIMON_BBOX_KNEE_HEIGHT;
	}
	else
	{
		right = x + SIMON_BBOX_IDLE_WIDTH;
		bottom = y + SIMON_BBOX_IDLE_HEIGHT;
	}


}

int CSimon::GetPreviousState()
{
	return previousstate;
}

void CSimon::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case SIMON_STATE_WALKING_LEFT:
		if (mx == 0)
			vx = -SIMON_WALKING_SPEED;
		nx = -1;
		break;
	case SIMON_STATE_WALKING_RIGHT:
		if (mx == 0)
			vx = SIMON_WALKING_SPEED;
		nx = 1;
		break;
	case SIMON_STATE_JUMP:
		vy = -SIMON_JUMP_SPEED;
		break;
	case SIMON_STATE_IDLE:
		vx = 0;
		mx = 0;
		break;
	case SIMON_STATE_KNEE:
		if (previousstate != SIMON_STATE_KNEE)
			SetPosition(x, y + 9);
		previousstate = SIMON_STATE_KNEE;
		vx = 0;
		mx = 1;
		break;
	case SIMON_STATE_UPDATE:
		vx = 0;
		break;
	case SIMON_STATE_STAIR:
		vx = 0;
		vy = 0;
		break;
	}
}

CSimon * CSimon::GetInstance()
{
	if (__instance == NULL) __instance = new CSimon();
	return __instance;
}
