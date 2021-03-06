#include "Fishman.h"
#include "Simon.h"
#include "Water.h"
#include "SplashEffect.h"

CFishman::CFishman()
{
	CMonster::CMonster();
	health = 1;
	damage = 1;
	AddAnimation(18000);
	AddAnimation(18001);
	AddAnimation(18002);
	AddAnimation(18003);
	AddAnimation(18004);
	AddAnimation(18005);
	bullet = new CBullet();
	bullet->SetDamage(1);
	i = 0;
	j = 0;
	fire = false;
	int random;
	wait1 = true;
	wait2 = true;
	isAddBulletToCell = false;
	//srand(time(NULL));
	random = rand() % 10;
	if (random <= 1)
		items = new CHeart(HEART_BIG);
	else if (random <= 3)
		items = new CHeart(HEART_SMALL);
	else
		items = NULL;
}

CFishman::CFishman(float width, float height, float a_x, float a_y)
{
	CMonster::CMonster();
	health = 1;
	damage = 1;
	AddAnimation(18000);
	AddAnimation(18001);
	AddAnimation(18002);
	AddAnimation(18003);
	AddAnimation(18004);
	AddAnimation(18005);
	bullet = new CBullet();
	i = 0;
	j = 0;
	fire = false;
	int random;
	wait1 = true;
	wait2 = true;
	isAddBulletToCell = false;
	//srand(time(NULL));
	random = rand() % 10;
	if (random <= 1)
		items = new CHeart(HEART_BIG);
	else if (random <= 3)
		items = new CHeart(HEART_SMALL);
	else
		items = NULL;

	SetStateAppear(FISHMAN_STATE_JUMP);
	SetPostionAppear(a_x, a_y);
	SetSize(width, height);
}

void CFishman::GetBoundingBox(float &left, float &top, float &right, float &bottom)
{
	left = x;
	top = y;
	if (state == MONSTER_STATE_SLEEPING)
	{
		left = x;
		top = y;
		right = x + weight;
		bottom = y + height;
	}
	else if (state == FISHMAN_STATE_FIRE)
	{
		right = x + FISHMAN_BBOX_FIRE_WIDTH;
		bottom = y + FISHMAN_BBOX_FIRE_HEIGHT;
	}
	else
	{
		right = x + FISHMAN_BBOX_WIDTH;
		bottom = y + FISHMAN_BBOX_HEIGHT;
	}
}

void CFishman::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
{
	CMonster::Update(dt, coObjects);
	vy += FISHMAN_GRAVITY * dt;
	if (health <= 0 && state == MONSTER_STATE_DELETE)
	{
		float cx, cy, tx, ty;
		GetPositionAppear(cx, cy);
		GetPosition(tx, ty);
		SetPostionAppear(tx, ty);
		SetPosition(cx, cy);
		SetState(MONSTER_STATE_SLEEPING);
		health = 1;
	}
	if (state != MONSTER_STATE_SLEEPING)
	{
		if (state != MONSTER_STATE_DISAPPEAR && state != MONSTER_STATE_DELETE)
		{
			vector<LPCOLLISIONEVENT> coEvents;
			vector<LPCOLLISIONEVENT> coEventsResult;
			coEventsResult.clear();
			CalcPotentialCollisions(coObjects, coEvents);
			for (UINT i = 0; i < coEvents.size(); i++)
			{
				LPCOLLISIONEVENT e = coEvents[i];
				if (dynamic_cast<CHidenObject *> (e->obj))
				{
					CHidenObject *hidenobject = dynamic_cast<CHidenObject *>(e->obj);
					if (hidenobject->GetState() != HIDENOBJECT_STATE_NORMAL)
					{
						coEvents.erase(coEvents.begin() + i);
						i--;
					}
				}
				else if (dynamic_cast<CWater *>(e->obj))
					continue;
				else
				{
					coEvents.erase(coEvents.begin() + i);
					i--;
				}
			}
			if (coEvents.size() == 0)
			{
				x += dx;
				y += dy;
			}
			else
			{
				float min_tx, min_ty, nx = 0, ny;
				FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny);
				x += min_tx * dx + nx * 0.4f;
				if (ny < 0)
				{
					y += min_ty * dy + ny * 0.4f;




					if (this->nx < 0 && this->x >= FISHMAN_TURN) SetState(FISHMAN_STATE_WALKING_LEFT);
					else SetState(FISHMAN_STATE_WALKING_RIGHT);


					if (wait1)
						StartWait(wait1, start_wait1);

					if (GetTickCount() - start_wait1 > 2000)
					{
						if (fire == false)
						{
							bullet->SetPosition(this->x, this->y);
							bullet->Setnx(this->nx);
							if (this->nx > 0) bullet->SetState(BULLET_STATE_RIGHT);
							else bullet->SetState(BULLET_STATE_LEFT);

							CSimon *simon = CSimon::GetInstance();
							CCells *cells = simon->map->GetCell();
							cells->InitCells(bullet);
							simon->map->SetCell(cells);
							isAddBulletToCell = true;
						}
						SetState(FISHMAN_STATE_FIRE);

						start_wait1 = 0;

						if (wait2)
							StartWait(wait2, start_wait2);

						if (GetTickCount() - start_wait2 > 1000)
						{
							fire = false;
							start_wait2 = 0;
							wait1 = true;
							wait2 = true;
						}
					}

				}
				else
					y += dy;

				for (UINT i = 0; i < coEventsResult.size(); i++)
				{
					LPCOLLISIONEVENT e = coEventsResult[i];
					if (dynamic_cast<CWater *> (e->obj))
					{
						CWater *water = dynamic_cast<CWater *> (e->obj);
						water->SetIsUpEffect(true);
						water->SetWait(true);
						vector<CSplashEffect *> spEffect = water->GetSplashEffect();
						if (e->ny > 0)
						{
							for (UINT i = 0; i < spEffect.size(); i++)
								spEffect[i]->SetPosition(x + 5.0f, y);
						}
						else 
						{
							for (UINT i = 0; i < spEffect.size(); i++)
								spEffect[i]->SetPosition(x + 5.0f, y + FISHMAN_BBOX_HEIGHT);
						}
						y += dy;
						//wait = true;
					}
				}
			}
		}
	}
}

void CFishman::Render()
{
	int ani;
	if (state == FISHMAN_STATE_FIRE)
	{
		if (nx < 0) ani = FISHMAN_ANI_FIRE_LEFT;
		else ani = FISHMAN_ANI_FIRE_RIGHT;
		Time_Fishman_HitEffect = GetTickCount();
		animations[ani]->Render(x, y);
		//bullet->Render();
	}
	else if (state == FISHMAN_STATE_WALKING_LEFT)
	{
		ani = FISHMAN_ANI_WALKING_LEFT;
		animations[ani]->Render(x, y);
		Time_Fishman_HitEffect = GetTickCount();
	}
	else if (state == FISHMAN_STATE_WALKING_RIGHT)
	{
		ani = FISHMAN_ANI_WALKING_RIGHT;
		animations[ani]->Render(x, y);
		Time_Fishman_HitEffect = GetTickCount();
	}
	else if (state == FISHMAN_STATE_JUMP)
	{
		if (nx < 0) ani = FISHMAN_ANI_JUMP_LEFT;
		else ani = FISHMAN_ANI_JUMP_RIGHT;
		Time_Fishman_HitEffect = GetTickCount();
		animations[ani]->Render(x, y);
		//bullet->Render();
	}
	//RenderBoundingBox(200);
}

void CFishman::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case FISHMAN_STATE_WALKING_RIGHT:
		vx = FISHMAN_WALKING_SPEED;
		nx = 1;
		break;
	case FISHMAN_STATE_WALKING_LEFT:

		vx = -FISHMAN_WALKING_SPEED;
		nx = -1;
		break;
	case FISHMAN_STATE_JUMP:
		vy = -FISHMAN_JUMP_SPEED_Y;
		break;
	case FISHMAN_STATE_FIRE:
		vx = 0;
		fire = true;
		break;
	}
}
