
#include "RabidFramework.h"
#include <body._h>



CPolyShadow::CPolyShadow()
{
	return;
}

CPolyShadow::~CPolyShadow()
{
	return;
}


bool CPolyShadow::DrawShadow(geActor *Actor)
{
		int NumFace;
		int Verts;
		int Normals;
		geVec3d Point;
		GE_Collision Collision;
		int fc,x,i;
		geBody_Index BoneIndex;
		geXForm3d Transform;
		geBody *aBody;
		GE_LVertex vertex[3];
		float scale;
		geVec3d Pos,NewPos;
		geVec3d LightRot, NewLPos;

		//set defaults once
		vertex[0].u = 0.f;
		vertex[0].v = 0.f;
		vertex[0].r = 0.f;
		vertex[0].g = 0.f;
		vertex[0].b = 0.f;
		vertex[0].a = 255.f;

		vertex[1].u = 0.f;
		vertex[1].v = 0.f;
		vertex[1].r = 0.f;
		vertex[1].g = 0.f;
		vertex[1].b = 0.f;
		vertex[1].a = 255.f;

		vertex[2].u = 0.f;
		vertex[2].v = 0.f;
		vertex[2].r = 0.f;
		vertex[2].g = 0.f;
		vertex[2].b = 0.f;
		vertex[2].a = 255.f;

		// get actor data
		aBody = geActor_GetBody(geActor_GetActorDef(Actor));
		geBody_GetGeometryStats(aBody, 0, &Verts, &NumFace, &Normals);
		CCD->ActorManager()->GetScale(Actor,&scale);
		CCD->ActorManager()->GetPosition(Actor,&Pos);

		//set defualt light source position
		NewLPos = Pos;
		NewLPos.Y += 512;
		NewLPos.Z += 256;

		//set collision position
		NewPos = Pos;
		NewPos.Y -= 512;

		// get the angle from the light source to the actor - Cheat a bit. Should be done for every vertex.
		CCD->PathFollower()->GetDirectionVector(NewLPos,Pos,&LightRot);

		//Get ground plane and distance
		BOOL Result = geWorld_Collision(CCD->Engine()->World(), NULL, NULL, &Pos, &NewPos,
			GE_CONTENTS_SOLID_CLIP , GE_COLLIDE_MESHES | GE_COLLIDE_MODELS, 0,
			NULL, NULL, &Collision);

		//draw shadow
		//cycle through faces
		for(i=0;i<NumFace;i++)
		{

			//cycle through the vertices of each face
			for(x=0;x<3;x++)
			{
				//get vertex pos and bone index for vertex
				fc = aBody->SkinFaces[0].FaceArray[i].VtxIndex[x];
				geVec3d_Copy(&(aBody->XSkinVertexArray[fc].XPoint), &(Point));
				BoneIndex = aBody->XSkinVertexArray[fc].BoneIndex;

				//scale the vertices by the RF defined scale
				geVec3d_Scale(&Point,scale,&Point);

				//transform the vertex by the bone position
				geActor_GetBoneTransformByIndex(Actor, BoneIndex, &Transform);
				geXForm3d_Transform(&Transform, &(Point), &(Point));

				// draw shadow against floor plane
				float t;
				geVec3d Intersection;
				t = (Collision.Plane.Dist - geVec3d_DotProduct(&Collision.Plane.Normal,&Point))/geVec3d_DotProduct(&Collision.Plane.Normal,&LightRot);
				geVec3d_AddScaled(&Point,&LightRot,t,&Intersection);
				vertex[x].X=Intersection.X + Collision.Plane.Normal.X;
				vertex[x].Y=Intersection.Y + Collision.Plane.Normal.Y;
				vertex[x].Z=Intersection.Z + Collision.Plane.Normal.Z;

			}

				//add poly
				geWorld_AddPolyOnce(CCD->Engine()->World(), vertex, 3, NULL, GE_GOURAUD_POLY,
					GE_RENDER_DO_NOT_OCCLUDE_OTHERS|GE_RENDER_DO_NOT_OCCLUDE_SELF , scale);
		}

		return true;
}



