#ifdef RF063

#ifndef __RGF_CENEMY_H__
#define __RGF_CENEMY_H__

typedef struct EnemyList
{
	EnemyList *next;
	EnemyList *prev;
	geActor	*Actor;
	char *AIName;
} EnemyList;

class CEnemy : public CRGFComponent
{
public:
	CEnemy();										// Constructor
	~CEnemy();									// Destructor
	void BuildBase(Enemy *pSource);
	void BuildAI(Enemy *pSource);
	bool AINextState(void *VData, int state, int input, char *key, CString KeyName);
	void Tick(float dwTicks);					// Increment animation time
	void Spawn(Enemy *pSource);
	void Animate(Enemy *pSource, float dwTicks);
	void ProcessAI(Enemy *pSource, float dwTicks);
	void ProcessStateMachine(Enemy *pSource);
	void RotateToGoal(Enemy *pSource, bool CanRotate, float dwTicks);
	void CoffeeMove(Enemy *pSource, float dwTicks);
	bool MoveScan(Enemy *pSource, geVec3d dir, float dist);
	void CheckTarget(Enemy *pSource);
	bool InRange(geActor *Actor, geActor *Target, float distance, float FOV);
	bool AquireGoal(Enemy *pSource);
	void ClearTrack(Enemy *pSource);
	Track *FindTrack(Enemy *pSource, int32 TrackArr[]);
	Track *FindTrackToGoal(Enemy *pSource);
	bool FinishTrack(Enemy *pSource);
	bool InitFindMultiTrack(Enemy *pSource, geVec3d *DestPos);
	bool InitFindMultiTrackAway(Enemy *pSource, geVec3d *DestPos);
	bool PastPoint(geVec3d *Pos, geVec3d *MoveVector, geVec3d *TgtPos);
	void Debug();
	int LocateEntity(char *szName, void **pEntityData);
private:
	CIniFile AttrFile;
	EnemyList *Bottom;
};

#endif

#endif