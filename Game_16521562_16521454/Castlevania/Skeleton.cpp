#include "Skeleton.h"
#include "Simon.h"
#include "Candle.h"
#include "HidenObject.h"
#include "Door.h"
#include "debug.h"

CSkeleton::CSkeleton()
{
}

CSkeleton::CSkeleton(float x, float y)
{
	health = 1;
	damage = 4;
	SetPosition(x, y);
	AddAnimation(220011); //ani left
	AddAnimation(220012); // ani right




	number = 0;

	jump = false;

	is_back = false;

	start_to_throw = GetTickCount();
}

void CSkeleton::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vy += SKELETON_GRAVITY;
	CSimon *simon = CSimon::GetInstance();
	CMonster::Update(dt, coObjects);
	if (state != MONSTER_STATE_DELETE && state != MONSTER_STATE_DISAPPEAR)
	{
		DWORD now = GetTickCount();

		min = simon->x - SKELETON_DISTANCE_MIN;
		max = simon->x - SKELETON_DISTANCE_MAX;
		if (state != SKELETON_STATE_JUMP)
		{
			if (x < min)
				SetState(SKELETON_STATE_WALKING_RIGHT);
			else if (x > max)
				SetState(SKELETON_STATE_WALKING_LEFT);
		}
		CGameObject::Update(dt);
		vector<LPCOLLISIONEVENT> coEvents;
		vector<LPCOLLISIONEVENT> coEventsResult;
		coEventsResult.clear();
		CalcPotentialCollisions(coObjects, coEvents);
		FilterCollisionImmediately(coEvents, coEventsResult);
		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
			if (dynamic_cast<CHidenObject *>(e->obj))
			{
				CHidenObject *hobj = dynamic_cast<CHidenObject *>(e->obj);
				if (hobj->GetState() == HIDENOBJECT_STATE_JUMP)
				{
					if (state != SKELETON_STATE_JUMP)
					{
						jump = true;
						jump_time = GetTickCount();
					}
				}
			}
		}
		coEventsResult.clear();

		for (UINT i = 0; i < coEvents.size(); i++)//Xoa cac doi tuong khong can thiet
		{
			LPCOLLISIONEVENT e = coEvents[i];
			if (dynamic_cast<CHidenObject *>(e->obj))
			{
				CHidenObject *hobj = dynamic_cast<CHidenObject *>(e->obj);
				if (hobj->GetState() != HIDENOBJECT_STATE_NORMAL
					&& hobj->GetState() != HIDENOBJECT_STATE_JUMP)
				{
					coEvents.erase(coEvents.begin() + i);
					i--;
				}
			}
			else
			{
				coEvents.erase(coEvents.begin() + i);
				i--;
			}
		}

		bool test = true;

		if (coEvents.size() != 0)
		{
			float min_tx, min_ty, nx = 0, ny;
			FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny);

			if (ny != 0) vy = 0;

			for (UINT i = 0; i < coEventsResult.size(); i++)
			{
				LPCOLLISIONEVENT e = coEventsResult[i];
				if (dynamic_cast<CHidenObject *>(e->obj))
				{
					CHidenObject *hobj = dynamic_cast<CHidenObject *>(e->obj);
					if (hobj->GetState() == HIDENOBJECT_STATE_NORMAL)
					{
						this->x += min_tx * dx + nx * 0.4f;
						this->y += min_ty * dy + ny * 0.4f;
						test = false;
						if (state == SKELETON_STATE_JUMP)
						{
							if (vx > 0)
								SetState(SKELETON_STATE_WALKING_LEFT);
							else
								SetState(SKELETON_STATE_WALKING_RIGHT);
						}
					}
				}
			}
		}

		if (now - start_to_throw >= TIME_TO_THROW_BONES)
		{
			CCells* cell = simon->map->GetCell();
			CBone* bone = new CBone();
			bone->SetSpeed(0.02f, -0.2f);
			bone->SetPosition(this->x + 5.0f, this->y + 10.0f);
			bone->InitHeight();
			bone->SetState(BONE_STATE_NORMAL);
			if (this->nx > 0) bone->nx = 1;
			else bone->nx = -1;
			cell->InitCells(bone);
			simon->map->SetCell(cell);
			start_to_throw = GetTickCount();
		}
		if (jump == true)
		{
			SetState(SKELETON_STATE_JUMP);
			jump = false;
		}

		if (test == true)
		{
			this->x += dx;
			this->y += dy;
		}
		if (y >= 190)
			SetState(MONSTER_STATE_DELETE);
	}
	DebugOut(L"state= %d\n", state);
}

void CSkeleton::Render()
{
	CMonster::Render();
	if (state != MONSTER_STATE_DELETE && state != MONSTER_STATE_DISAPPEAR)
	{
		if (nx > 0)
			animations[SKELETON_ANI_WALKING_RIGHT]->Render(x, y);
		else
			if (nx < 0)animations[SKELETON_ANI_WALKING_LEFT]->Render(x, y);
	}
	//RenderBoundingBox(200);
}

void CSkeleton::GetBoundingBox(float & left, float & top, float & right, float & bottom)
{
	left = x;
	top = y;
	right = x + SKELETON_BBOX_WIDTH;
	bottom = y + SKELETON_BBOX_HEIGHT;
}

void CSkeleton::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case SKELETON_STATE_WALKING_LEFT:
		vx = -SKELETON_WALKING_SPEED;
		break;
	case SKELETON_STATE_WALKING_RIGHT:
		vx = SKELETON_WALKING_SPEED;
		break;
	case SKELETON_STATE_JUMP:
		vy = -SKELETON_JUMP_SPEED_Y;
		break;
	}
}

CSkeleton::~CSkeleton()
{
}
