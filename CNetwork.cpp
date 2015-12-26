/************************************************************************************//**
 * @file CNetwork.cpp
 * @brief NetPlayer and NetPlayerMgr class
 ****************************************************************************************/

#include "RabidFramework.h"
#include "CAttribute.h"
#include "CAutoDoors.h"
#include "CDamage.h"
#include "CElectric.h"
#include "CLogic.h"
#include "CMovingPlatforms.h"
#include "CStaticEntity.h"
#include "CTeleporter.h"
#include "CTriggers.h"

#include <crtdbg.h>
#define DEBUG_NW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NW

/* ------------------------------------------------------------------------------------ */
// NetPlayer Constructor
/* ------------------------------------------------------------------------------------ */
NetPlayer::NetPlayer()
{
	Actor = NULL;
	ActorName[0] = '\0';
	Animation[0] = '\0';
	PlayerName[0] = '\0';
	Scale = 1.0f;
	AnimTime = 0.0f;

	BaseRotation.X = BaseRotation.Y = BaseRotation.Z = 0.0f;
	localRotation.X = localRotation.Y = localRotation.Z = 0.0f;
	localTranslation.X = localTranslation.Y = localTranslation.Z = 0.0f;
}

/* ------------------------------------------------------------------------------------ */
// Destructor
/* ------------------------------------------------------------------------------------ */
NetPlayer::~NetPlayer()
{
	if(Actor)
	{
		CCD->ActorManager()->RemoveActor(Actor);
		geActor_Destroy(&Actor);
		Actor = NULL;
	}
}

/* ------------------------------------------------------------------------------------ */
// Create
//
// add netplayer actor to level
/* ------------------------------------------------------------------------------------ */
void NetPlayer::Create(char *actorname)
{
	Actor = CCD->ActorManager()->SpawnActor(actorname, BaseRotation, BaseRotation,
											Animation, Animation, NULL);
	CCD->ActorManager()->Position(Actor, localTranslation);
	CCD->ActorManager()->Rotate(Actor, localRotation);
	CCD->ActorManager()->SetAnimationTime(Actor, AnimTime);
	CCD->ActorManager()->SetScale(Actor, Scale);
}

/* ------------------------------------------------------------------------------------ */
// NetPlayerMgr Constructor
/* ------------------------------------------------------------------------------------ */
NetPlayerMgr::NetPlayerMgr()
{
	for(int i=0; i<MAXPLAYERS; ++i)
		Player[i] = NULL;

	Buffer			= NULL;
	outbuffer		= NULL;
	inbuffer		= NULL;
	updatebuffer	= NULL;
	isServer		= false;
	clientnum		= 0;
	serverstop		= true;
	clientrun		= false;
}

/* ------------------------------------------------------------------------------------ */
// Destructor
/* ------------------------------------------------------------------------------------ */
NetPlayerMgr::~NetPlayerMgr()
{
	if(!isServer)
		SendDelete();

	if(Buffer)
		delete Buffer;

	if(outbuffer)
		delete outbuffer;

	if(inbuffer)
		delete inbuffer;

	if(updatebuffer)
		delete updatebuffer;

	if(!serverstop && isServer)
	{
		nlClose(serversock);
		serverstop = true;
		CCD->ReportError("NetManager:  Shutting Down Server", false);
	}

	if(clientrun)
	{
		nlClose(sock);
		clientrun = false;
	}
}

/* ------------------------------------------------------------------------------------ */
// Delet Players
//
// Delete all active players
/* ------------------------------------------------------------------------------------ */
void NetPlayerMgr::DeletePlayers()
{
	for(int i=0; i<MAXPLAYERS; ++i)
	{
		if(Player[i])
		{
			delete Player[i];
			Player[i] = NULL;
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// CreatePlayer
//
// Create a new player from its ID#
/* ------------------------------------------------------------------------------------ */
bool NetPlayerMgr::CreatePlayer(int Id)
{
	int i;

	for(i=0; i<MAXPLAYERS; ++i)
	{
		if(Player[i])
		{
			// exists already
			if(Player[i]->GetId()==Id)
				return false;
		}
	}

	for(i=0; i<MAXPLAYERS; ++i)
	{
		if(!Player[i])
		{
			// make new player
			Player[i] = new NetPlayer;
			Player[i]->SetId(Id);
			return true;
		}
	}

	return false;
}

/* ------------------------------------------------------------------------------------ */
// GetIndexFromId
//
// Get player number from ID#
/* ------------------------------------------------------------------------------------ */
int NetPlayerMgr::GetIndexFromId(int Id)
{
	int i;

	for(i=0; i<MAXPLAYERS; ++i)
	{
		if(Player[i])
		{
			if(Player[i]->GetId()==Id)
				return i;
		}
	}

	return -1;
}

/* ------------------------------------------------------------------------------------ */
// Initialize
//
// Initialize networking for multiplayer game
/* ------------------------------------------------------------------------------------ */
bool NetPlayerMgr::Initialize(bool server, char *serverip)
{
	// test for working network TCP\IP
	serverstop = true;

	if(!nlSelectNetwork(NL_IP))
	{
		// initialize server if hosting
		if(server)
		{
			isServer = true;
			// open on port 25000
			serversock = nlOpen(25000, NL_RELIABLE);

			if(serversock == NL_INVALID)
			{
				serverstop = true;
				CCD->ReportError("[WARNING] Server Error - Open Socket Failed", false);
				return false;
			}

			if(!nlListen(serversock))	/* let's listen on this socket */
			{
				nlClose(serversock);
				serverstop = true;
				CCD->ReportError("[WARNING] Server Error - Listen on Socket Failed", false);
				return false;
			}

			// create the servers client group
			group = nlGroupCreate();
			clientnum = 0;
			serverstop = false;
			CCD->ReportError("Server Started", false);

			// create buffers for server
			outbuffer = new NetBuffer(0);
			inbuffer = new NetBuffer(0);
			updatebuffer = new NetBuffer(0);
		}
		else
		{
			// save ip of server if joining game
			strcpy(ServerIP, serverip);
		}

		clientrun = false;
		clientready = false;

		NLaddress addr;

		// open client socket on system set port
		sock = nlOpen(0, NL_RELIABLE);

		if(sock == NL_INVALID)
		{
			return false;
		}

		// connect locally if hosting, otherwise to server ip
		if(isServer)
			nlGetLocalAddr(sock, &addr);
		else
			nlStringToAddr(ServerIP, &addr);

		nlSetAddrPort(&addr, 25000);

		if(!nlConnect(sock, &addr))
		{
			nlClose(sock);
			return false;
		}

		// create client buffer
		Buffer = new NetBuffer(0);

		// used by join game client only to get ID # and level name
		if(!isServer)
		{
			float Time = CCD->FreeRunningCounter_F();

			while(1)
			{
				// wait for acceptance packet from server
				if(ReadBuffer(Buffer, sock)==NL_TRUE)
				{
					if(Buffer->GetChar() == static_cast<unsigned char>(CONNECTIONACCEPT))
					{
						// get your Id#
						PlayerId = Buffer->GetInt();
						// get name of level being used
						char *level = Buffer->GetString();
						CCD->MenuManager()->SetLevelName(level);
						free(level);
						clientready = true;
						clientstage = ACCEPTED;
						break;
					}
				}

				// wait for 30 secs for acceptance then abort
				float deltaTime = 0.001f * (static_cast<float>(CCD->FreeRunningCounter()) - Time);

				if(deltaTime > 30.0f)
				{
					CCD->ReportError("Client Timeout waiting for Acceptance", false);
					return false;
				}
			}
		}

		clientrun = true;

		return true;
	}

	return false;
}

/* ------------------------------------------------------------------------------------ */
// ReadBuffer
//
// Read packet into buffer from socket
/* ------------------------------------------------------------------------------------ */
int NetPlayerMgr::ReadBuffer(NetBuffer *Buff, NLsocket socket)
{
	char bufferlen[10];
	int readlen;

	// first 4 bytes are length of following data
	if(readlen = nlRead(socket, bufferlen, 4) > 0)
	{
		int rv;
		memcpy(&rv, bufferlen, sizeof(int)); // convert to integer value

		// read in rest of data
		Buff->PosBack(0);
		Buff->CheckSize(rv);
		nlRead(socket, Buff->Data, rv);

		// got packet
		return NL_TRUE;
	}

	if(readlen == NL_INVALID) // may have errored
		return NL_INVALID;

	// no packet yet
	return NL_FALSE;
}

/* ------------------------------------------------------------------------------------ */
// Tick
/* ------------------------------------------------------------------------------------ */
void NetPlayerMgr::Tick(geFloat /*dwTicks*/)
{

}

/* ------------------------------------------------------------------------------------ */
// ServerClientCycle
//
// This is the Server and Client processing
//
// this function is called every 0.05 second from the Windows Timer
/* ------------------------------------------------------------------------------------ */
void NetPlayerMgr::ServerClientCycle()
{
	// Server for receiving and redistributing information
	if(isServer && !serverstop)
	{
		// if not full then check for new connection
		if(clientnum<(MAXPLAYERS-1))
		{
			NLsocket newsock = nlAcceptConnection(serversock);

			if(newsock != NL_INVALID)
			{
				// add client to group
				nlGroupAddSocket(group, newsock);
				++clientnum;

				// send acceptance packet to new client only
				// packet has ID# and level name
				outbuffer->PosBack(0);
				outbuffer->Add(1);
				outbuffer->Add(static_cast<unsigned char>(CONNECTIONACCEPT));
				outbuffer->Add(newsock);
				const char *level = CCD->MenuManager()->GetLevelName();
				outbuffer->AddString(level, strlen(level));
				outbuffer->AddLen();
				nlWrite(newsock, outbuffer->Data, outbuffer->Size);
			}
			else
			{
				// test for error if no new connection
				if(nlGetError() == NL_SOCKET_ERROR)
				{
					nlClose(serversock);
					serverstop = true;
					CCD->ReportError("Server Stopped on Connection Error", false);
					return;
				}
			}
		}

		// check client group for incoming packets
		int count = nlPollGroup(group, NL_READ_STATUS, s, MAXPLAYERS, 0);

		// have something
		if(count > 0)
		{
			int i;
			bool update = false;

			// initalize client update buffer for possible use later
			updatebuffer->PosBack(0);
			updatebuffer->Add(1);
			updatebuffer->Add(static_cast<unsigned char>(CLIENTUPDATE));

			// loop through all packets
			for(i=0; i<count; ++i)
			{
				// read packet for socket s[] which is Client ID#
				int readlen;
				readlen = ReadBuffer(inbuffer, s[i]);

				// got the packet
				if(readlen == NL_TRUE)
				{
					// get packet type
					unsigned char Info = inbuffer->GetChar();
					int index;

					switch(Info)
					{
					// information from a new client after acceptance
					case PLAYERINFO:
						// create a new net player for client
						index = AddNewPlayer(inbuffer);
						// send info about current world to new client
						SendWorldInfo(outbuffer, s[i]);
						// notify all clients about new client being added
						if(index != -1)
						{
							outbuffer->PosBack(0);
							outbuffer->Add(1);
							outbuffer->Add(static_cast<unsigned char>(CLIENTADD));
							BuildPlayer(outbuffer, index, Player[index]->GetId());
							nlWrite(group, outbuffer->Data, outbuffer->Size);
						}
						break;
					// updated information from a client
					case PLAYERUPDATE:
						// build info in update buffer
						ServerUpdateClient(inbuffer);
						update = true;
						break;
					// process player leaving game
					case PLAYERDELETE:
						outbuffer->PosBack(0);
						outbuffer->Add(1);
						outbuffer->Add(static_cast<unsigned char>(DELETECLIENT));
						outbuffer->Add(s[i]);
						outbuffer->AddLen();

						nlGroupDeleteSocket(group, s[i]);
						nlClose(s[i]);
						// send to all other players
						nlWrite(group, outbuffer->Data, outbuffer->Size);
						--clientnum;
						break;
					default:
						break;
					}
				}
				// check if error reading packet
				else if(readlen == NL_INVALID)
				{
					NLenum err = nlGetError();

					// delete client if they dissappear
					if(err == NL_MESSAGE_END || err == NL_SOCK_DISCONNECT)
					{
						outbuffer->PosBack(0);
						outbuffer->Add(1);
						outbuffer->Add(static_cast<unsigned char>(DELETECLIENT));
						outbuffer->Add(s[i]);
						outbuffer->AddLen();

						nlGroupDeleteSocket(group, s[i]);
						nlClose(s[i]);

						nlWrite(group, outbuffer->Data, outbuffer->Size);

						--clientnum;
					}
				}
			}

			// if there were client updates then broadcast info to all clients
			if(update)
			{
				updatebuffer->Add(-1);
				updatebuffer->AddLen();
				nlWrite(group, updatebuffer->Data, updatebuffer->Size);
			}
		}
	}


	// Client
	//
	// used by client when hosting game only to get ID
	if(!clientready && clientrun)
	{
		// get acceptance packet from server to get ID#
		if(ReadBuffer(Buffer, sock) == NL_TRUE)
		{
			if(Buffer->GetChar() == static_cast<unsigned char>(CONNECTIONACCEPT))
			{
				PlayerId = Buffer->GetInt();
				clientready = true;
				clientstage = ACCEPTED;
			}
		}
	}

	// process info when client is up and going
	if(clientready && clientrun)
	{
		// Connection acceptance handshaking
		//  - create net player on own computer
		//  - send that data to server
		//  - get data back on other net players plus data on world state

		// client has been accepted by server
		if(clientstage == ACCEPTED) // create net player of client
		{
			CreatePlayer(PlayerId);
			int index = GetIndexFromId(PlayerId);

			strcpy(Player[index]->ActorName, CCD->Player()->GetActorName());
			strcpy(Player[index]->PlayerName, CCD->Player()->GetPlayerName());
			CCD->ActorManager()->GetScale(CCD->Player()->GetActor(), &Player[index]->Scale);

			CCD->ActorManager()->GetAligningRotation(CCD->Player()->GetActor(), &Player[index]->BaseRotation);
			CCD->ActorManager()->GetRotate(CCD->Player()->GetActor(), &Player[index]->localRotation);
			CCD->ActorManager()->GetPosition(CCD->Player()->GetActor(), &Player[index]->localTranslation);
			CCD->ActorManager()->GetRotate(CCD->Player()->GetActor(), &Player[index]->oldRotation);
			CCD->ActorManager()->GetPosition(CCD->Player()->GetActor(), &Player[index]->oldTranslation);

			strcpy(Player[index]->Animation, CCD->ActorManager()->GetMotion(CCD->Player()->GetActor()));
			Player[index]->AnimTime = CCD->ActorManager()->GetAnimationTime(CCD->Player()->GetActor());

			// got to next stage of connection next pass
			// skip all the handshaking if you are hosting the game
			if(!isServer)
				clientstage = PLAYERSETUP;
			else
				clientstage = FINISHED;
		}
		// client has created own net player
		else if(clientstage == PLAYERSETUP) // send player info to server
		{
			// send info about client net player to server
			int index = GetIndexFromId(PlayerId);
			Buffer->PosBack(0);
			Buffer->Add(1);
			Buffer->Add(static_cast<unsigned char>(PLAYERINFO));
			BuildPlayer(Buffer, index, PlayerId);
			nlWrite(sock, Buffer->Data, Buffer->Size);

			// got to next stage of connection next pass
			connectstate = 0;
			clientstage = OTHERPLAYERS;
		}
		// client has sent player info to server
		else if(clientstage == OTHERPLAYERS) // get other players info from server
		{
			// wait until server sends info about all other clients and world
			if(ReadBuffer(Buffer, sock) == NL_TRUE)
			{
				unsigned char Info = Buffer->GetChar();

				// got client list
				if(Info == CLIENTLIST)
				{
					// process all client info recieved
					while(Buffer->GetInt() != -1)
					{
						Buffer->PosBack(sizeof(int));
						AddNewPlayer(Buffer);
					}

					connectstate += 1;
				}

				// read in all world info here
				if(Info == WORLDLIST1)
				{
					CCD->Doors()->RestoreFrom(NULL, true);
					CCD->Platforms()->RestoreFrom(NULL, true);
					CCD->Props()->RestoreFrom(NULL, true);
					CCD->Teleporters()->RestoreFrom(NULL, true);
					CCD->Triggers()->RestoreFrom(NULL, true);
					CCD->Logic()->RestoreFrom(NULL, true);

					connectstate += 1;
				}

				if(Info == WORLDLIST2)
				{
					CCD->Attributes()->RestoreFrom(NULL, true);
					CCD->Damage()->RestoreFrom(NULL, true);
					CCD->ElectricEffects()->RestoreFrom(NULL, true);
					CCD->ModelManager()->RestoreFrom(NULL, true);

					connectstate += 1;
				}

				// got end of connection
				if(Info == DONECONNECT)
				{
					connectstate += 1;
				}

				// if have read in all info then end
				if(connectstate == CONNECTIONPACKETS)
				{
					// finished getting all connection data
					clientstage = FINISHED;
				}
			}
		}
		// client is connected properly to game
		else if(clientstage == FINISHED) // process normal info
		{
			// get a packet from server
			if(ReadBuffer(Buffer, sock)==NL_TRUE)
			{
				unsigned char Info = Buffer->GetChar();

				switch(Info)
				{
				// client to be deleted
				case DELETECLIENT:
					DeletePlayer(Buffer);
					break;
				// clients need updating
				case CLIENTUPDATE:
					ProcessClientUpdate(Buffer);
					break;
				// new client has joined game
				case CLIENTADD:
					AddNewPlayer(Buffer);
					break;
				default:
					break;
				}
			}

			// update client info if needed and send to server
			ClientUpdate();
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// AddNewPlayer
//
// add net player from info in buffer
/* ------------------------------------------------------------------------------------ */
int NetPlayerMgr::AddNewPlayer(NetBuffer *Buff)
{
	// ID# of net player
	int NewId = Buff->GetInt();

	// create player locally if doesn't already exist
	bool flag = CreatePlayer(NewId);
	int index = GetIndexFromId(NewId);

	char *name = Buff->GetString();
	char *playername = Buff->GetString();
	float scale = Buff->GetFloat();
	geVec3d BaseRotation = Buff->GetVec3d();
	geVec3d localRotation = Buff->GetVec3d();
	geVec3d localTranslation = Buff->GetVec3d();
	char *animname = Buff->GetString();
	float animtime = Buff->GetFloat();

	// doesn't exist yet
	if(flag)
	{
		strcpy(Player[index]->ActorName, name);
		strcpy(Player[index]->PlayerName, playername);
		Player[index]->Scale = scale;
		Player[index]->BaseRotation = BaseRotation;
		Player[index]->localRotation = localRotation;
		Player[index]->localTranslation = localTranslation;
		strcpy(Player[index]->Animation, animname);
		Player[index]->AnimTime = animtime;

		// add new actor to level
		Player[index]->Create(Player[index]->ActorName);
	}

	free(name);
	free(animname);

	return index;
}

/* ------------------------------------------------------------------------------------ */
// SendWorldInfo
//
// send info about current world to socket
/* ------------------------------------------------------------------------------------ */
void NetPlayerMgr::SendWorldInfo(NetBuffer *Buff, NLsocket sock)
{
	int j = 0;

	// send info about clients
	Buff->PosBack(0);
	Buff->Add(1);
	Buff->Add(static_cast<unsigned char>(CLIENTLIST));

	for(int i=0; i<MAXPLAYERS; ++i)
	{
		if(Player[i])
		{
			BuildPlayer(Buff, i, Player[i]->GetId());
			++j;
		}
	}

	Buff->Add(-1);
	Buff->AddLen();
	nlWrite(sock, Buff->Data, Buff->Size);

	// send info about rest of world here
	outbuffer->PosBack(0);
	outbuffer->Add(1);
	outbuffer->Add(static_cast<unsigned char>(WORLDLIST1));

	CCD->Doors()->SaveTo(NULL, true);
	CCD->Platforms()->SaveTo(NULL, true);
	CCD->Props()->SaveTo(NULL, true);
	CCD->Teleporters()->SaveTo(NULL, true);
	CCD->Triggers()->SaveTo(NULL, true);
	CCD->Logic()->SaveTo(NULL, true);

	outbuffer->AddLen();

	nlWrite(sock, outbuffer->Data, outbuffer->Size);

	outbuffer->PosBack(0);
	outbuffer->Add(1);
	outbuffer->Add(static_cast<unsigned char>(WORLDLIST2));

	CCD->Attributes()->SaveTo(NULL, true);
	CCD->Damage()->SaveTo(NULL, true);
	CCD->ElectricEffects()->SaveTo(NULL, true);
	CCD->ModelManager()->SaveTo(NULL, true);

	outbuffer->AddLen();
	nlWrite(sock, outbuffer->Data, outbuffer->Size);

	// total # of packets sent is defined by CONNECTIONPACKETS
	// currently is 4

	// send done connecting command
	Buff->PosBack(0);
	Buff->Add(1);
	Buff->Add(static_cast<unsigned char>(DONECONNECT));
	Buff->AddLen();
	nlWrite(sock, Buff->Data, Buff->Size);
}

/* ------------------------------------------------------------------------------------ */
// BuildPlayer
//
// put player info into buffer
/* ------------------------------------------------------------------------------------ */
void NetPlayerMgr::BuildPlayer(NetBuffer *Buff, int index, int Id)
{
	Buff->Add(Id);
	Buff->AddString(Player[index]->ActorName, strlen(Player[index]->ActorName));
	Buff->AddString(Player[index]->PlayerName, strlen(Player[index]->PlayerName));
	Buff->Add(Player[index]->Scale);
	Buff->Add(Player[index]->BaseRotation);
	Buff->Add(Player[index]->localRotation);
	Buff->Add(Player[index]->localTranslation);
	Buff->AddString(Player[index]->Animation, strlen(Player[index]->Animation));
	Buff->Add(Player[index]->AnimTime);
	Buff->AddLen();
}

/* ------------------------------------------------------------------------------------ */
// ClientUpdate
//
// check if client needs to send update to server
/* ------------------------------------------------------------------------------------ */
void NetPlayerMgr::ClientUpdate()
{
	// see if client has changed from last update
	int index = GetIndexFromId(PlayerId);

	// check rotation and position against last updated info
	CCD->ActorManager()->GetRotate(CCD->Player()->GetActor(), &Player[index]->localRotation);
	CCD->ActorManager()->GetPosition(CCD->Player()->GetActor(), &Player[index]->localTranslation);

	bool flag1 = geVec3d_Compare(&Player[index]->localRotation, &Player[index]->oldRotation, 0.0f);
	bool flag2 = geVec3d_Compare(&Player[index]->localTranslation, &Player[index]->oldTranslation, 0.0f);

	// see if animation has changed
	bool flag3 = !strcmp(Player[index]->Animation, CCD->ActorManager()->GetMotion(CCD->Player()->GetActor()));

	// something has changed
	if(!flag1 || !flag2 || !flag3)
	{
		// sent update of changed data
		Buffer->PosBack(0);
		Buffer->Add(1);
		Buffer->Add(static_cast<unsigned char>(PLAYERUPDATE));
		Buffer->Add(PlayerId);

		// rotation has changed
		if(!flag1)
		{
			Buffer->Add(static_cast<unsigned char>(ROTATION));
			Buffer->Add(Player[index]->localRotation);
			Player[index]->oldRotation = Player[index]->localRotation;
		}

		// position has changed
		if(!flag2)
		{
			Buffer->Add(static_cast<unsigned char>(TRANSLATION));
			Buffer->Add(Player[index]->localTranslation);
			Player[index]->oldTranslation = Player[index]->localTranslation;
		}

		// animation has changed
		if(!flag3)
		{
			strcpy(Player[index]->Animation, CCD->ActorManager()->GetMotion(CCD->Player()->GetActor()));
			Player[index]->AnimTime = CCD->ActorManager()->GetAnimationTime(CCD->Player()->GetActor());
			Buffer->Add(static_cast<unsigned char>(ANIMATION));
			Buffer->AddString(Player[index]->Animation, strlen(Player[index]->Animation));
			Buffer->Add(Player[index]->AnimTime);
		}

		// end of changes so send to server
		Buffer->Add(static_cast<unsigned char>(ENDCMD));
		Buffer->AddLen();
		nlWrite(sock, Buffer->Data, Buffer->Size);
	}
}

/* ------------------------------------------------------------------------------------ */
// ServerUpdateClient
//
// server builds update buffer from incoming packets
/* ------------------------------------------------------------------------------------ */
void NetPlayerMgr::ServerUpdateClient(NetBuffer *Buff)
{
	unsigned char cmd;
	geVec3d Rotation;
	geVec3d Translation;
	char *animname;
	float animtime;

	// add client ID#
	int NewId = Buff->GetInt();
	updatebuffer->Add(NewId);

	while(1)
	{
		// transfer change data to update buffer
		cmd = Buff->GetChar();
		updatebuffer->Add(cmd);
		// end of change data

		if(cmd == static_cast<unsigned char>(ENDCMD))
			break;

		if(cmd == static_cast<unsigned char>(ROTATION))
		{
			Rotation = Buff->GetVec3d();
			updatebuffer->Add(Rotation);
		}

		if(cmd == static_cast<unsigned char>(TRANSLATION))
		{
			Translation = Buff->GetVec3d();
			updatebuffer->Add(Translation);
		}

		if(cmd == static_cast<unsigned char>(ANIMATION))
		{
			animname = Buff->GetString();
			animtime = Buff->GetFloat();
			updatebuffer->AddString(animname, strlen(animname));
			updatebuffer->Add(animtime);
			free(animname);
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// ProcessClientUpdate
//
// client processes other clients update data
/* ------------------------------------------------------------------------------------ */
void NetPlayerMgr::ProcessClientUpdate(NetBuffer *Buff)
{
	int NewId;
	unsigned char cmd;
	geVec3d Rotation;
	geVec3d Translation;
	char *animname;
	float animtime;

	while(1)
	{
		// get client ID#
		NewId = Buff->GetInt();

		// end of list
		if(NewId == -1)
			break;

		int index = GetIndexFromId(NewId);

		while(1)
		{
			// process all changes for client
			cmd = Buff->GetChar();

			// end current client data
			if(cmd == static_cast<unsigned char>(ENDCMD))
				break;

			if(cmd == static_cast<unsigned char>(ROTATION))
			{
				Rotation = Buff->GetVec3d();

				// ignore if self
				if(NewId != PlayerId && index != -1)
				{
					Player[index]->oldRotation = Player[index]->localRotation;
					Player[index]->localRotation = Rotation;
					CCD->ActorManager()->Rotate(Player[index]->Actor, Rotation);
				}
			}

			if(cmd == static_cast<unsigned char>(TRANSLATION))
			{
				Translation = Buff->GetVec3d();

				// ignore if self
				if(NewId != PlayerId && index != -1)
				{
					Player[index]->oldTranslation = Player[index]->localTranslation;
					Player[index]->localTranslation = Translation;
					CCD->ActorManager()->Position(Player[index]->Actor, Translation);
				}
			}

			if(cmd == static_cast<unsigned char>(ANIMATION))
			{
				animname = Buff->GetString();
				animtime = Buff->GetFloat();

				// ignore if self
				if(NewId != PlayerId && index != -1)
				{
					strcpy(Player[index]->Animation, animname);
					Player[index]->AnimTime = animtime;
					CCD->ActorManager()->SetMotion(Player[index]->Actor, animname);
					CCD->ActorManager()->SetAnimationTime(Player[index]->Actor, animtime);
				}

				free(animname);
			}
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// DeletePlayer
//
// Client delete player from game
/* ------------------------------------------------------------------------------------ */
void NetPlayerMgr::DeletePlayer(NetBuffer *Buff)
{
	int NewId = Buff->GetInt();
	int index = GetIndexFromId(NewId);

	if(index != -1)
	{
		delete Player[index];
		Player[index] = NULL;
	}
}

/* ------------------------------------------------------------------------------------ */
// SendDelete
//
// send message to server you are leaving
/* ------------------------------------------------------------------------------------ */
void NetPlayerMgr::SendDelete()
{
	// do only if connected
	if(clientrun)
	{
		Buffer->PosBack(0);
		Buffer->Add(1);
		Buffer->Add(static_cast<unsigned char>(PLAYERDELETE));
		Buffer->AddLen();
		nlWrite(sock, Buffer->Data, Buffer->Size);
		nlClose(sock);
		clientrun = false;
		// delete all player data
		DeletePlayers();
	}
}

/* ------------------------------------------------------------------------------------ */
// WriteData
//
// direct save data to file (Save) or buffer (build world data by server)
/* ------------------------------------------------------------------------------------ */
void NetPlayerMgr::WriteData(bool type, void *data, int size, int amount, FILE *SaveFD)
{
	if(!type)
	{
		fwrite(data, size, amount, SaveFD);
	}
	else
	{
		outbuffer->AddRaw(data, size*amount);
	}
}

/* ------------------------------------------------------------------------------------ */
// ReadData
//
// read data from file (Load) or from buffer (read world data by client)
/* ------------------------------------------------------------------------------------ */
void NetPlayerMgr::ReadData(bool type, void *data, int size, int amount, FILE *RestoreFD)
{
	if(!type)
	{
		fread(data, size, amount, RestoreFD);
	}
	else
	{
		Buffer->GetRaw(data, size*amount);
	}
}


/* ----------------------------------- END OF FILE ------------------------------------ */
