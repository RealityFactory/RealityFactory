
#include "RabidFramework.h"

extern geBitmap *TPool_Bitmap(char *DefaultBmp, char *DefaultAlpha, char *BName, char *AName);

CNPCPathPoint::CNPCPathPoint()
{
	void				*ClassData;
	geEntity_EntitySet	*ClassSet;
	geEntity			*Entity;
	NPCPathPoint		*Point;
	
	int pointnum=0;
	TrackPt *track_pt;
	
	memset(CCD->Track()->TrackList,0,sizeof(CCD->Track()->TrackList[0])*MAX_TRACKS);
	CCD->Track()->TrackCount = 0;
	
	// Look for the class name in the world
	ClassSet = geWorld_GetEntitySet(CCD->World(), "NPCPathPoint");
	
    if (!ClassSet)
        return;
	
	Entity = NULL;
	
	while (1)
	{
		Entity = geEntity_EntitySetGetNextEntity(ClassSet, Entity);
		
		if (!Entity)
			break;
		
		ClassData = geEntity_GetUserData(Entity);
		Point = (NPCPathPoint*)ClassData;
		
		assert(Point);
		
		// initialize translated position
		Point->Pos = Point->origin;
		
		if (Point->PathType <= 0)
			continue;
		
		//save off header info
		CCD->Track()->TrackList[CCD->Track()->TrackCount].Type = Point->PathType;
        pointnum = 0;
		
		//move through points setting up the list
		do
		{
			//save off point info
			track_pt = &CCD->Track()->TrackList[CCD->Track()->TrackCount].PointList[pointnum];
			// initialize translated position
			Point->Pos = Point->origin;
			Point->OriginOffset = Point->origin;
			if(Point->MoveWithModel)
			{
				geVec3d ModelOrigin;
				geWorld_GetModelRotationalCenter(CCD->World(), Point->MoveWithModel, &ModelOrigin);
				geVec3d_Subtract(&Point->origin, &ModelOrigin, &Point->OriginOffset);
			}
			track_pt->Pos = &Point->Pos;
			if (Point->WatchPoint)
				track_pt->WatchPos = &Point->WatchPoint->origin;
			
			track_pt->Action = Point->ActionType;
			track_pt->Time = Point->Time;
            track_pt->ActionDir = Point->Direction;
			track_pt->Dist = Point->Dist;
			track_pt->VelocityScale = Point->VelocityScale;
			
			// automatic setup for doors
			if (CCD->Track()->TrackList[CCD->Track()->TrackCount].Type == TRACK_TYPE_TRAVERSE_DOOR)
			{
				if (pointnum == 0)
				{
					track_pt->ActionDir = 1;
					track_pt->Action = POINT_TYPE_WAIT_POINT_VISIBLE;
					track_pt->WatchPos = &Point->Next->origin;
				}
				else
					if (pointnum == 1)
					{
						track_pt->ActionDir = -1;
						track_pt->Action = POINT_TYPE_WAIT_POINT_VISIBLE;
						track_pt->WatchPos = (track_pt-1)->Pos;
					}
			}
			
			pointnum++;
			CCD->Track()->TrackList[CCD->Track()->TrackCount].PointCount = pointnum;

            assert(pointnum < MAX_TRACK_POINTS);
			
			Point = Point->Next;
			
			// Found a type
			if (Point && Point->PathType >= 0)
			{
				track_pt = &CCD->Track()->TrackList[CCD->Track()->TrackCount].PointList[pointnum];
				SET(track_pt->Flags, BIT(0));
			}
			
		}while(Point && Point != (NPCPathPoint*)ClassData);
		
		CCD->Track()->TrackCount++;
		assert(CCD->Track()->TrackCount < MAX_TRACKS);
	}
	
	Entity = NULL;
	
	CCD->Track()->Track_LinkTracks();
	
	PathPointTexture = TPool_Bitmap("corona.bmp", "corona.bmp", NULL, NULL);
}

CNPCPathPoint::~CNPCPathPoint()
{
}

static void DrawLine3d(const geVec3d *p1, const geVec3d *p2, int r, int g, int b, int r1, int g1, int b1)
{
	GE_LVertex	v[4];
	int			i;
	geVec3d		perp;
	geVec3d		in;
	
	geXForm3d M = CCD->CameraManager()->ViewPoint();
	
	v[0].u = 0.0f;
	v[0].v = 0.0f;
	v[1].u = 1.0f;
	v[1].v = 0.0f;
	v[2].u = 1.0f;
	v[2].v = 1.0f;
	v[3].u = 0.0f;
	v[3].v = 1.0f;
	
	for	(i = 0; i < 4; i++)
	{
		v[i].r = (geFloat)r;
		v[i].g = (geFloat)g;
		v[i].b = (geFloat)b;
		v[i].a = 255.0f;
	}
	
	geVec3d_Subtract(p1, p2, &perp);
	geXForm3d_GetIn(&M, &in);
	geVec3d_CrossProduct(&perp, &in, &perp);
	geVec3d_Normalize(&perp);
	geVec3d_Scale(&perp, 2.0f / 2.0f, &perp);
	
	v[0].X = p2->X - perp.X;
	v[0].Y = p2->Y - perp.Y;
	v[0].Z = p2->Z - perp.Z;
	v[1].X = p2->X + perp.X;
	v[1].Y = p2->Y + perp.Y;
	v[1].Z = p2->Z + perp.Z;
	v[2].X = p1->X + perp.X;
	v[2].Y = p1->Y + perp.Y;
	v[2].Z = p1->Z + perp.Z;
	v[3].X = p1->X - perp.X;
	v[3].Y = p1->Y - perp.Y;
	v[3].Z = p1->Z - perp.Z;
	
	v[0].r = (float)r1;
	v[0].g = (float)g1;
	v[0].b = (float)b1;
	v[1].r = (float)r1;
	v[1].g = (float)g1;
	v[1].b = (float)b1;
	v[2].r = (float)r1;
	v[2].g = (float)g1;
	v[2].b = (float)b1;
	v[3].r = (float)r1;
	v[3].g = (float)g1;
	v[3].b = (float)b1;
	
	geWorld_AddPolyOnce(CCD->World(),
		v,
		4,
		NULL,
		GE_GOURAUD_POLY,
		GE_RENDER_DO_NOT_OCCLUDE_OTHERS,
		1.0f);
}

void CNPCPathPoint::Tick()
{
	geEntity_EntitySet *	Set;
	geEntity *		Entity;
	
	Set = geWorld_GetEntitySet(CCD->World(), "NPCPathPoint");
	if (Set == NULL)
		return;
	
	Entity = geEntity_EntitySetGetNextEntity(Set, NULL);
	
	while	(Entity)
	{
		NPCPathPoint *Point;
		
		Point = (NPCPathPoint *)geEntity_GetUserData(Entity);
		
		Point->origin = Point->OriginOffset;
		SetOriginOffset(NULL, NULL, Point->MoveWithModel, &(Point->origin));
		
		Entity = geEntity_EntitySetGetNextEntity(Set, Entity);
	}
}

void CNPCPathPoint::Render()
{
	int	i,v;
	geEntity_EntitySet *	Set;
	geVec3d *sp, *ep;
	
	Set = geWorld_GetEntitySet(CCD->World(), "NPCPathPoint");
	if (Set == NULL)
		return;
	
	// Now walk the track list, drawing lines to connect them all.  Draw red
	// lines to things that are being watched.
	
	for	(i = 0; i < CCD->Track()->TrackCount; i++)
	{
		Track *	T;
		
		T = &CCD->Track()->TrackList[i];
		assert(T->PointCount > 0);
		
		for (v = 0; v<T->PointCount - 1; v++)
		{
			sp = T->PointList[v].Pos;
			ep = T->PointList[v+1].Pos;
			DrawLine3d(sp,ep,255, 255, 255,0, 0, 255);
		}
	}
	
	GE_Collision	Collision;
	geBoolean	Visible;
	geXForm3d XForm = CCD->CameraManager()->ViewPoint();
	
	for	(i = 0; i < CCD->Track()->TrackCount; i++)
	{
		int		j;
		Track *	T;
		
		T = &CCD->Track()->TrackList[i];
		assert(T->PointCount > 0);
		
		for	(j = 0; j < T->PointCount; j++)
		{
			GE_LVertex	Vert;
			float Radius;
			
			if	(!geWorld_Collision(CCD->World(),
				NULL,
				NULL,
				T->PointList[j].Pos,
				&XForm.Translation,
				GE_CONTENTS_CANNOT_OCCUPY,
				GE_COLLIDE_MODELS,
				0xffffffff, NULL, NULL, 
				&Collision))
			{
				Visible = GE_TRUE;
			}
			else
			{
				Visible = GE_FALSE;
			}
			
			
			if (Visible)
			{
				Vert.X = T->PointList[j].Pos->X;
				Vert.Y = T->PointList[j].Pos->Y;
				Vert.Z = T->PointList[j].Pos->Z;
				
				if (j != 0 && TEST(T->PointList[j].Flags, BIT(0)))
				{
					Vert.g = 0.0f;
					Vert.b = 0.0f;
					Radius = 0.1f;
				}
				else
				{
					Vert.g = 255.0f;
					Vert.b = 255.0f;
					Radius = 0.05f;
				}
				
				Vert.r = 255.0f;
				Vert.a = 255.0f;
				Vert.u = Vert.v = 0.0f;
				
				geWorld_AddPolyOnce(CCD->World(),
					&Vert,
					1,
					PathPointTexture,
					GE_TEXTURED_POINT,
					GE_RENDER_DO_NOT_OCCLUDE_OTHERS | GE_RENDER_DO_NOT_OCCLUDE_SELF,
					Radius);
			}
			
			
			if (j < T->PointCount - 1)
			{
				DrawLine3d(T->PointList[j].Pos,
					T->PointList[j + 1].Pos,
					255, 255, 255,
					0, 255, 0);
			}
			
			if	(T->PointList[j].WatchPos)
			{
				DrawLine3d(T->PointList[j].Pos,
					T->PointList[j].WatchPos,
					255, 255, 255,
					255, 0, 0);
			}
		}
	}
}