/************************************************************************************//**
 * @file CNetwork.h
 * @brief NetPlayer and NetPlayerMgr class
 ****************************************************************************************/

#ifndef __RGF_CNETWORK_H_
#define __RGF_CNETWORK_H_

#include "HawkNL\\nl.h"
#include "CNetBuffer.h"

#define TIMERINTERVAL  50		///< # of milliseconds between calls to server/client cycle

#define CONNECTIONPACKETS  4	///< # of packets sent during connection handshake

#define WRITEDATA(type, a, b, c, d) CCD->NetPlayerManager()->WriteData(type, (void*)a, b, c, d)
#define READDATA(type, a, b, c, d) CCD->NetPlayerManager()->ReadData(type, (void*)a, b, c, d)

#define MAXPLAYERS 10

typedef enum
{
	CONNECTIONACCEPT = 0,
// commands sent to server
	PLAYERINFO,
	PLAYERUPDATE,
	PLAYERDELETE,
// commands sent to client
	DELETECLIENT,
	CLIENTUPDATE,
	CLIENTLIST,
	WORLDLIST1,
	WORLDLIST2,
	DONECONNECT,
	CLIENTADD,
// information delimiters
	ROTATION,
	TRANSLATION,
	ANIMATION,
	ENDCMD
};

/**
 * client states during connection to server
 */
typedef enum
{
	ACCEPTED = 0,
	PLAYERSETUP,
	OTHERPLAYERS,
	FINISHED
};

/**
 * @brief NetPlayer class
 */
class NetPlayer
{
public:
	NetPlayer();
	~NetPlayer();

	void Create(char *actorname);
	int GetId() const									{ return Id;						}
	void SetId(int id)									{ Id = id;							}
	void SetBaseRotation(const geVec3d &Rotation)		{ BaseRotation = Rotation;			}
	void SetlocalRotation(const geVec3d &Rotation)		{ localRotation = Rotation;			}
	void SetlocalTranslation(const geVec3d &Translation){ localTranslation = Translation;	}

public:
	geActor *Actor;
	char ActorName[128];
	char Animation[128];
	char PlayerName[64];
	float Scale;
	float AnimTime;
	int Id;
	geVec3d localTranslation;
	geVec3d localRotation;
	geVec3d oldTranslation;
	geVec3d oldRotation;
	geVec3d BaseRotation;

private:

};

class NetPlayerMgr
{
public:
	NetPlayerMgr();
	~NetPlayerMgr();

	bool Initialize(bool server, char *serverip);
	void Tick(geFloat dwTicks);
	void ServerClientCycle();
	int AddNewPlayer(NetBuffer *Buff);
	void SendWorldInfo(NetBuffer *Buff, NLsocket sock);
	void DeletePlayers();
	void BuildPlayer(NetBuffer *Buff, int index, int Id);
	void ClientUpdate();
	void ServerUpdateClient(NetBuffer *Buff);
	void ProcessClientUpdate(NetBuffer *Buff);
	void DeletePlayer(NetBuffer *Buff);
	void SendDelete();
	void WriteData(bool type, void *data, int size, int amount, FILE *SaveFD);
	void ReadData(bool type, void *data, int size, int amount, FILE *RestoreFD);

private:
	bool CreatePlayer(int Id);
	int GetIndexFromId(int Id);
	int ReadBuffer(NetBuffer *Buff, NLsocket socket);

private:
	NetPlayer	*Player[MAXPLAYERS];
	int			PlayerId;
	NLsocket	sock;
	NetBuffer	*Buffer;
	bool		isServer;
	char		ServerIP[64];
	int			clientstage;
	int			connectstate;
// Server
	NLsocket	serversock;
	NLint		group;
	NLsocket	s[MAXPLAYERS];
	int			clientnum;

	NetBuffer	*inbuffer;
	NetBuffer	*outbuffer;
	NetBuffer	*updatebuffer;

	bool		clientrun;
	bool		serverstop;
	bool		clientready;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
