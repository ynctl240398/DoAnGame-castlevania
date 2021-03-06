#include "Map.h"
#include "Candle.h"
#include "Simon.h"
#include "Ghoul.h"
#include "BossBat.h"
#include "Bat.h"
#include "Fishman.h"
#include "Bullet.h"
#include "SpiritDracula.h"
#include "Dracula.h"
#include "HidenObject.h"
CMap::CMap()
{

	textures = CTextures::GetInstance();
	sprites = CSprites::GetInstance();
	animations = CAnimations::GetInstance();
	tilemap = new CTileMap();
	prev_scene = 0;
	scene = 0;
}

CMap::~CMap()
{
}




void CMap::GetUpdateObjects(vector<LPGAMEOBJECT>* objects)
{
	float cam_x, cam_y;
	CGame *game = CGame::GetInstance();
	game->GetCamera(cam_x, cam_y);
	ChangeCellOfObject(cam_x, cam_y);
	cells->GetListOfObjects(objects, cam_x, cam_y);
	cellsobject->GetListOfObjects(objects, cam_x, cam_y);
}

void CMap::ChangeCellOfObject(float cam_x, float cam_y)
{
	int xs, ys;
	int xe, ye;
	int i, j, k;
	xs = (int)cam_x / CELL_WIDTH;
	ys = (int)cam_y / CELL_HEIGHT;

	xe = (int)(cam_x + 256.0f) / CELL_WIDTH;
	ye = (int)(cam_y + 185.0f) / CELL_HEIGHT;
	for (i = xs; i <= xe; i++)
		for (j = ys; j <= ye; j++)
		{
			if (cells->cells[i][j].GetObjects().size() != 0)
				for (k = 0; k < (int)cells->cells[i][j].GetObjects().size(); k++)
				{
					LPGAMEOBJECT e = cells->cells[i][j].GetObjects()[k];
					if (dynamic_cast <CBone *> (e))
					{
						if (e->state == BONE_STATE_DELETE)
							cells->cells[i][j].XoaObject(k);
					}
					if (dynamic_cast <CCandle *> (e))
					{
						if (e->state == CANDLE_STATE_DELETE)
							cells->cells[i][j].XoaObject(k);
					}
					if (dynamic_cast <CItems *> (e))
					{
						if (e->state == ITEM_STATE_DELETE)
							cells->cells[i][j].XoaObject(k);
					}
					else if (dynamic_cast <CDracula *> (e))
					{
						CDracula *dracula = dynamic_cast<CDracula *>(e);
						if (dracula->state == DRACULA_STATE_DIE && dracula->GetIsSpirit() == false)
						{
							cells->cells[i][j].XoaObject(k);
						}
					}
					else if (dynamic_cast <CBossBat *> (e))
					{
						CBossBat *bossbat = dynamic_cast<CBossBat *>(e);
						if (bossbat->state == MONSTER_STATE_DELETE)
						{
							cells->cells[i][j].XoaObject(k);
						}
					}
					else if (dynamic_cast<CMonster *> (e))
					{
						int a = (int)e->x / CELL_WIDTH;
						int b = (int)e->y / CELL_HEIGHT;
						if (a != i || b != j)
						{
							cells->cells[i][j].XoaObject(k);
							if (xs <= a && a <= xe && ys <= b && b <= ye)
								cells->cells[a][b].SetObjects(e);
						}
					}
					else if (dynamic_cast<CBullet *> (e))
					{
						int a = (int)e->x / CELL_WIDTH;
						int b = (int)e->y / CELL_HEIGHT;
						if (a != i || b != j)
						{
							cells->cells[i][j].XoaObject(k);
							if (xs <= a && a <= xe)
								if (ys <= b && b <= ye)
									cells->cells[i][j].SetObjects(e);
						}
					}
					else if (dynamic_cast<CBrick *>(e))
					{
						CBrick *brick = dynamic_cast<CBrick *>(e);
						if (brick->state == BRICK_STATE_DELETE)
							cells->cells[i][j].XoaObject(k);
					}
				}
		}
}

void CMap::GetSizeOfMap(float &width, float &height)
{
	tilemap->GetSize(width, height);
}

void CMap::LoadMap()
{

}

void CMap::Render()
{

}


