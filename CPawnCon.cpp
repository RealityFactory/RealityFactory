/************************************************************************************//**
 * @file CPawnCon.cpp
 * @brief Pawn Conversation
 * @author Ralph Deane
 *//*
 * Copyright (c) Ralph Deane; All Rights Reserved.
 ****************************************************************************************/

#include "RabidFramework.h"

extern geSound_Def *SPool_Sound(const char *SName);
// changed Nout 12/15/05
extern geBitmap *TPool_Bitmap(const char *DefaultBmp, const char *DefaultAlpha,
							  const char *BName, const char *AName);
// end change

#include "Simkin\\skScriptedExecutable.h"
#include "Simkin\\skRValue.h"
#include "Simkin\\skRValueArray.h"

// #define BACKCLEAR	GE_TRUE
#define BACKCLEAR	GE_FALSE

/* ------------------------------------------------------------------------------------ */
//	ScriptConverse class
/* ------------------------------------------------------------------------------------ */
ScriptedConverse::ScriptedConverse(const char *fileName) : skScriptedExecutable(fileName,CCD->GetskContext()) //update simkin
{
	M_CameraRect.Left	= 0;
	M_CameraRect.Right	= CCD->Engine()->Width() - 1;
	M_CameraRect.Top	= 0;
	M_CameraRect.Bottom = CCD->Engine()->Height() - 1;

	m_Streams			= NULL;
	m_Streams_reply		= NULL;
	m_Streams_click		= NULL;
	m_Streams_mouseover = NULL;
// changed Nout 12/15/05
	Background			= NULL;
	Icon				= NULL;
	rBackground			= NULL;
	rIcon				= NULL;
	ReplyMenuBar		= NULL;

	SpeachWidth		= 0;
	SpeachHeight	= 0;
	ReplyWidth		= 0;
	ReplyHeight		= 0;


	for(int i=0; i<9; i++)
	{
		BGBitmap[i] = NULL;
		MouseOverBitmap[i] = NULL;
	}
// end change
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
/* ------------------------------------------------------------------------------------ */
ScriptedConverse::~ScriptedConverse()
{
	// make sure we didn't leave anything alive
	if(m_Streams)
	{
		m_Streams->Delete();
		delete m_Streams;
	}
	m_Streams = NULL;

	if(m_Streams_reply)
	{
		m_Streams_reply->Delete();
		delete m_Streams_reply;
	}
	m_Streams_reply = NULL;

	if(m_Streams_click)
	{
		m_Streams_click->Delete();
		delete m_Streams_click;
	}
	m_Streams_click = NULL;

	if(m_Streams_mouseover)
	{
		m_Streams_mouseover->Delete();
		delete m_Streams_mouseover;
	}
	m_Streams_mouseover = NULL;
}

/* ------------------------------------------------------------------------------------ */
//	getValue
/* ------------------------------------------------------------------------------------ */
bool ScriptedConverse::getValue(const skString &fieldName, const skString &attribute, skRValue &value)
{
/*
	if (fieldName == "m_param")
	{
		// copy the value
		value = m_param;

		// we found the variable
		return true;
	}
	else */
	{
		return skScriptedExecutable::getValue(fieldName, attribute, value);
	}

}

/* ------------------------------------------------------------------------------------ */
//	sets a field's value
/* ------------------------------------------------------------------------------------ */
bool ScriptedConverse::setValue(const skString &fieldName, const skString &attribute, const skRValue &value)
{
/*
	if (fieldName == "m_param")
	{
		// assign the new value
		m_param = value.intValue();

		// we found the variable
		return true;
	}
	else */
	{
		return skScriptedExecutable::setValue(fieldName, attribute, value);
	}
}

/* ------------------------------------------------------------------------------------ */
//	calls a method in this object
/* ------------------------------------------------------------------------------------ */
bool ScriptedConverse::method(const skString &methodName, skRValueArray &arguments,
							  skRValue &returnValue, skExecutableContext &ctxt)
{
	char cparam0[128];
	char cparam1[128];
	bool bparam0;
	int iparam0;

	cparam0[0] = '\0';
	cparam1[0] = '\0';

	if(IS_METHOD(methodName, "random"))
	{
		if(arguments[0].intValue() < arguments[1].intValue())
			returnValue = EffectC_rand(arguments[0].intValue(), arguments[1].intValue());
		else
			returnValue = EffectC_rand(arguments[1].intValue(), arguments[0].intValue());

		return true;
	}
	else if(IS_METHOD(methodName, "Speak"))
	{
		strcpy(cparam0, arguments[0].str());
		Text = CCD->Pawns()->GetText(cparam0);
		strcpy(cparam0, arguments[1].str());

		if(!EffectC_IsStringNull(cparam0))
		{
			char music[256];
			strcpy(music, CCD->GetDirectory(kAudioStreamFile));
			strcat(music, "\\");
			strcat(music, cparam0);
			m_dsPtr = (LPDIRECTSOUND)geSound_GetDSound();
			m_Streams = new StreamingAudio(m_dsPtr);

			if(m_Streams)
			{
				if(!m_Streams->Create(music))
				{
					delete m_Streams;
					m_Streams = NULL;
				}
			}
		}

		return true;
	}
	else if(IS_METHOD(methodName, "Reply"))
	{
		strcpy(cparam0, arguments[1].str());
		iparam0 = arguments[0].intValue();

		if(iparam0 > 9 || iparam0 < 1)
			return true;

		std::ostringstream oss;
		oss << iparam0 << " : ";
		Reply[iparam0-1] = oss.str();
		Reply[iparam0-1] += CCD->Pawns()->GetText(cparam0);
		TrimRight(Reply[iparam0-1]);
		TrimLeft(Reply[iparam0-1]);

		replyflg[iparam0-1] = true;
		return true;
	}
// Nout begin 20-8-2003
// Sound Conversation
// Begin add Nout 23/09/2003
	else if(IS_METHOD(methodName, "SpeakShowTime"))
	{
		SpeakShowTime = arguments[0].intValue();
		return true;
	}
	else if(IS_METHOD(methodName, "ReplyShowTime"))
	{
		ReplyShowTime = arguments[0].intValue();
		return true;
	}
// End add Nout 23/09/2003
	else if(IS_METHOD(methodName, "SoundReply"))
	{
		strcpy(cparam0, arguments[1].str());
		iparam0 = arguments[0].intValue();
		strcpy(cparam1, arguments[2].str());

		if(iparam0 > 9 || iparam0 < 1)
			return true;

		std::ostringstream oss;
		oss << iparam0 << " : ";
		Reply[iparam0-1] = oss.str();
		Reply[iparam0-1] += CCD->Pawns()->GetText(cparam0);
		TrimRight(Reply[iparam0-1]);
		TrimLeft(Reply[iparam0-1]);

		ReplySoundFileName[iparam0-1] = cparam1;
		TrimRight(ReplySoundFileName[iparam0-1]);
		TrimLeft(ReplySoundFileName[iparam0-1]);

		replyflg[iparam0-1] = true;

// changed Nout 12/15/05
		ShowSelectedReply = true;

		if(arguments.entries() > 7)
		{
			iparam0 = arguments[3].intValue();
			if(iparam0>=0)
				ReplyX = iparam0;

			iparam0 = arguments[4].intValue();
			if(iparam0>=0)
				ReplyY = iparam0;

			iparam0 = arguments[5].intValue();

			if(iparam0>=0)
				MyReplyWidth = iparam0;

			iparam0 = arguments[6].intValue();

			if(iparam0>=0)
				MyReplyHeight = iparam0;

			iparam0 = arguments[7].intValue();
			if(iparam0>=0)
				ReplyFont = iparam0;
		}
// end change

		return true;
	}
// changed Nout 12/15/05
	// MenuReply(ReplyNr, ReplyTextName, ReplySound, MenuFontNr, ReplyFontNr,
	//			 ReplyMenuBarBitmap, ReplyMenuBarAlphaBitmap, GIFnr, GifX, GifY);
	// To keep this function general, no autonumbering is added to the reply-lines.
	// Numbers can be added to the conversation.txt strings itself
	else if(IS_METHOD(methodName, "MenuReply"))
	{
		iparam0 = arguments[0].intValue();
		iparam0--;
		strcpy(cparam0, arguments[1].str());
		strcpy(cparam1, arguments[2].str());

		if(iparam0 > 8 || iparam0 < 0)
			return true;

		if(arguments[1].str() == "")
			Reply[iparam0] = " ";
		else
		{
			Reply[iparam0] = CCD->Pawns()->GetText(cparam0);
			TrimRight(Reply[iparam0]);
			TrimLeft(Reply[iparam0]);
		}

		ReplySoundFileName[iparam0] = cparam1;
		TrimRight(ReplySoundFileName[iparam0]);
		TrimLeft(ReplySoundFileName[iparam0]);

	    replyflg[iparam0] = true;
		MouseReply = true;
		ShowSelectedReply = false;

		if(arguments.entries() > 9)
		{
			GifX = arguments[8].intValue();
			GifY = arguments[9].intValue();
		}
		else
		{
			CCD->Pawns()->GetGifXY(&GifX, &GifY);
		}

		ReplyGifNr[iparam0] = -1;
		if(arguments.entries() > 7)
		{
			if(arguments[7].intValue() > 0 && arguments[7].intValue() < 10)
				ReplyGifNr[iparam0] = arguments[7].intValue();
		}

		if(arguments.entries() > 6)
		{
			strcpy(cparam0, arguments[5].str());

			if(strlen(cparam0) > 0)
			{
				geBitmap *Icon = CCD->Pawns()->GetCache(cparam0);

				if(Icon)
				{
					ReplyMenuBar = Icon;
					strcpy(cparam0, arguments[6].str());

					if(strlen(cparam0) > 0)
					{
						geBitmap *TIcon = CCD->Pawns()->GetCache(cparam0);

						if(TIcon)
							geBitmap_SetAlpha(Icon, TIcon);
						else
						{
							sprintf(cparam1, "[WARNING] Order %s, MenuReplyBarAlphaBitmap: AlphaBitmap %s not found", Order, cparam0);
							CCD->ReportError(cparam1, false);
						}
					}
				}
				else
				{
					sprintf(cparam1, "[WARNING] Order %s, MenuReplyBarBitmap: Bitmap %s not found", Order, cparam0);
					CCD->ReportError(cparam1, false);
				}
			}
			else
			{
				ReplyMenuBar = NULL;
			}
		}

		if(arguments.entries() > 4)
		{
			iparam0 = arguments[4].intValue();
			if(iparam0 >= 0)
				ReplyMenuFont = iparam0;
		}

		if(arguments.entries() > 3)
		{
			iparam0 = arguments[3].intValue();
			if(iparam0 >= 0)
				ReplyFont = iparam0;
		}

		return true;
	}
	// RestoreBackground(<boolean>);
	// Defines if the screen is cleared between a speak and a reply command (only 1 visible or both)
	else if(IS_METHOD(methodName, "RestoreBackground"))
	{
		RestoreBackground = arguments[0].boolValue();
		return true;
	}
	// ShowSelectedReply(<boolean>);
	// Defines if the selected reply is displayed or not after the selection is made
	else if(IS_METHOD(methodName, "ShowSelectedReply"))
	{
		ShowSelectedReply = arguments[0].boolValue();
		return true;
	}
	// SoundConversation(CharProSec, ShowHUD)
	else if(IS_METHOD(methodName, "SoundConversation"))
	{
		bparam0 = true;

		if(arguments.entries() > 1)
			bparam0 = arguments[1].boolValue();

		returnValue = DoSoundConversation(arguments[0].intValue(), bparam0);
		return true;
	}
	// AttachSpeakToPawn(PawnName, OffsetX, OffsetY, TextAreaWidth, TextAreaHeight, TextFont)
	// Attaches the speak window + text to a defined pawn. Use "Player" as PawnName to identify the player
	else if(IS_METHOD(methodName, "AttachSpeakToPawn"))
	{
		geVec3d Pos, ScreenPos;
		strcpy(cparam0, arguments[0].str());

		if(strlen(cparam0) > 0)
		{
			if(!stricmp(cparam0, "Player"))
				Pos = CCD->Player()->Position();
			else
				CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(cparam0), &Pos);

			geCamera_TransformAndProject(CCD->CameraManager()->Camera(), &Pos, &ScreenPos);
			ScreenPos.Z = 0.0f;

			SpeachX = int(ScreenPos.X) + arguments[1].intValue();
			SpeachY = int(ScreenPos.Y) + arguments[2].intValue();

			iparam0 = arguments[3].intValue();
			if(iparam0 >= 0)
				SpeachWidth = iparam0;

			iparam0 = arguments[4].intValue();
			if(iparam0 >= 0)
				SpeachHeight = iparam0;

			iparam0 = arguments[5].intValue();
			if(iparam0 >= 0)
				SpeachFont = iparam0;

			DrawBackground = false; // enforce no speak background
		}

		return true;
	}
	// AttachReplyToPawn(PawnName, TextOffsetX, TextOffsetY, TextAreaWidth, TextAreaHeight, TextFont)
	// Attaches the reply window + text to a defined pawn, use as PawnName "Player" to identify the player
	else if(IS_METHOD(methodName, "AttachReplyToPawn"))
	{
		geVec3d Pos, ScreenPos;
		strcpy(cparam0, arguments[0].str());

		if(strlen(cparam0) > 0)
		{
			if(!stricmp(cparam0, "Player"))
				Pos = CCD->Player()->Position();
			else
				CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(cparam0), &Pos);

			geCamera_TransformAndProject(CCD->CameraManager()->Camera(), &Pos, &ScreenPos);
			ScreenPos.Z = 0.0f;

			ReplyX = int(ScreenPos.X) + arguments[1].intValue();
			ReplyY = int(ScreenPos.Y) + arguments[2].intValue();

			iparam0 = arguments[3].intValue();
			if(iparam0 >= 0)
				MyReplyWidth = iparam0;

			iparam0 = arguments[4].intValue();
			if(iparam0 >= 0)
				MyReplyHeight = iparam0;

			iparam0 = arguments[5].intValue();
			if(iparam0 >= 0)
				ReplyFont = iparam0;

			ReplyInSpeakWindow = false; // enforce the reply in the reply window, not in the speak window
			DrawrBackground = false; // enforce to show no reply background
		}

		return true;
	}
	// GetMouseClick(ReplyNr, BGBitmap, PosX, PosY, Width, Height, SoundAtClick, SoundMouseOver, MouseOverBitmap)
	// Adds the option to mouse-click the reply messages
	else if(IS_METHOD(methodName, "GetMouseClick"))
	{
		int dPosX, dPosY, Index;
		char delim[] = ":";
		char *result = NULL;
		bool MultiLoop = false;
		bool MultiReply = false; // Indicates if a multiple reply menu is created or not
		Index = arguments[0].intValue() - 1;

		if(Index < 0 || !replyflg[Index])
		{
			sprintf(cparam1, "GetMouseClick with ReplyNr=%d: no Reply() or SoundReply() command found before this command with ReplyNumber=%d", Index);
			MessageBox(NULL, cparam1, "Error", MB_OK);
			return true;
		}

		strcpy(cparam0, arguments[1].str());
		geBitmap *TIcon = CCD->Pawns()->GetCache(cparam0);

		if(TIcon)
			BGBitmap[Index] = TIcon;
		else
		{
			BGBitmap[Index] = NULL;
			sprintf(cparam1, "Script Command GetMouseClick: Bitmap %s not found", cparam0);
			CCD->ReportError(cparam1, false);
		}

		strcpy(cparam0, arguments[2].str());
		result = strtok(cparam0, delim);
		MouseRepPosX[Index] = atoi(result);
		result = strtok(NULL, delim);
		if(result != NULL)
			dPosX = atoi(result);
		else
			dPosX = 0;

		strcpy(cparam0, arguments[3].str());
		result = strtok(cparam0, delim);
		MouseRepPosY[Index] = atoi(result);
		result = strtok(NULL, delim);
		if(result != NULL)
			dPosY = atoi(result);
		else
			dPosY = 0;

		MouseRepWidth[Index] = arguments[4].intValue();
		MouseRepHeight[Index] = arguments[5].intValue();

		strcpy(cparam0, arguments[6].str());
		if(!EffectC_IsStringNull(cparam0))
			SoundAtClick[Index] = cparam0;
		else
			SoundAtClick[Index] = "";

		strcpy(cparam0, arguments[7].str());
		if(!EffectC_IsStringNull(cparam0))
			SoundMouseOver[Index] = cparam0;
		else
			SoundMouseOver[Index] = "";

		strcpy(cparam0, arguments[8].str());
		geBitmap *SIcon = CCD->Pawns()->GetCache(cparam0);
		if(SIcon)
			MouseOverBitmap[Index] = SIcon;
		else
		{
			MouseOverBitmap[Index] = NULL;
			sprintf(cparam1, "[WARNING] Script Command GetMouseClick: bitmap %s not found", cparam0);
			CCD->ReportError(cparam1, false);
		}

		MouseMenu = true;
		return true;
	}
// end change
// Nout begin 16-9-2003
	// SoundCustomSpeakBackground(SpeakBackgroundBitmap, SpeakBackgroundX, SpeakBackgroundY);
	// SoundCustomSpeakBackground(SpeakBackgroundBitmap, SpeakBackgroundX, SpeakBackgroundY, SpeakX, SpeakY);
	// SoundCustomSpeakBackground(SpeakBackgroundBitmap, SpeakBackgroundX, SpeakBackgroundY, SpeakX, SpeakY, SpeakWidth, SpeakHeight);
	// SoundCustomSpeakBackground(SpeakBackgroundBitmap, SpeakBackgroundX, SpeakBackgroundY, SpeakX, SpeakY, SpeakWidth, SpeakHeight, SpeakFont);
    else if(IS_METHOD(methodName, "SoundCustomSpeakBackground"))
	{
		strcpy(cparam0, arguments[0].str());

		if(strlen(cparam0) > 0) //if no .BMP name, then use no background
		{
			geBitmap *TIcon = CCD->Pawns()->GetCache(cparam0);

			if(TIcon)
			{
				Background = TIcon;

				if(Background)
					geBitmap_SetColorKey(Background, GE_TRUE, 255, GE_FALSE);
			}
			else
			{
				sprintf(cparam1, "[WARNING] %s: The bitmap name may not include capitals!", cparam0);
				CCD->ReportError(cparam1, false);
			}
		}
		else
			DrawBackground = false;

		iparam0 = arguments[1].intValue();
		BackgroundX = iparam0;
		iparam0 = arguments[2].intValue();
		BackgroundY = iparam0;

// changed Nout 12/15/05
		if(arguments.entries() > 4)
		{
			iparam0 = arguments[3].intValue();
			if(iparam0 >= 0)
				SpeachX = iparam0;
			iparam0 = arguments[4].intValue();
			if(iparam0 >= 0)
				SpeachY = iparam0;
		}

		if(arguments.entries() > 6)
		{
			iparam0 = arguments[5].intValue();
			if(iparam0 >= 0)
				SpeachWidth = iparam0;
			iparam0 = arguments[6].intValue();
			if(iparam0 >= 0)
				SpeachHeight = iparam0;
		}

		if(arguments.entries() > 7)
		{
			iparam0 = arguments[7].intValue();
			if(iparam0 >= 0)
				SpeachFont = iparam0;
		}
// end change

		return true;
	}
	// SoundCustomReplyBackground(ReplyBackgroundBitmap, ReplyBackgroundX, ReplyBackgroundY);
	// SoundCustomReplyBackground(ReplyBackgroundBitmap, ReplyBackgroundX, ReplyBackgroundY, ReplyX, ReplyY);
	// SoundCustomReplyBackground(ReplyBackgroundBitmap, ReplyBackgroundX, ReplyBackgroundY, ReplyX, ReplyY, ReplyWidth, ReplyHeight);
	// SoundCustomReplyBackground(ReplyBackgroundBitmap, ReplyBackgroundX, ReplyBackgroundY, ReplyX, ReplyY, ReplyWidth, ReplyHeight, ReplyFont);
	else if(IS_METHOD(methodName, "SoundCustomReplyBackground"))
	{
		strcpy(cparam0, arguments[0].str());

		if(strlen(cparam0) > 0) //if no .BMP name,then use no background
		{
			geBitmap *TIcon = CCD->Pawns()->GetCache(cparam0);

			if(TIcon)
			{
				rBackground = TIcon;

				if(rBackground)
					geBitmap_SetColorKey(rBackground, GE_TRUE, 255, GE_FALSE);
			}
			else
			{
				sprintf(cparam1, "[WARNING] %s: The bitmap name may not include capitals!", cparam0);
				CCD->ReportError(cparam1, false);
			}
		}
		else
			DrawrBackground = false;

		iparam0 = arguments[1].intValue();
		rBackgroundX = iparam0;
		iparam0 = arguments[2].intValue();
		rBackgroundY = iparam0;

// changed Nout 12/15/05
		if(arguments.entries() > 4)
		{
			iparam0 = arguments[3].intValue();
			if(iparam0 >= 0)
				ReplyX = iparam0;
			iparam0 = arguments[4].intValue();
			if(iparam0 >= 0)
				ReplyY = iparam0;
		}

		if (arguments.entries() > 6)
		{
			iparam0 = arguments[5].intValue();
			if(iparam0 >= 0)
				MyReplyWidth = iparam0;
			iparam0 = arguments[6].intValue();
			if(iparam0 >= 0)
				MyReplyHeight = iparam0;
		}

		if(arguments.entries() > 7)
		{
			iparam0 = arguments[7].intValue();
			if(iparam0 >= 0)
				ReplyFont = iparam0;
		}
// end change

		return true;
	}
	else if(IS_METHOD(methodName, "ReplyInSpeakWindow"))
	{
		ReplyInSpeakWindow = arguments[0].boolValue();
		return true;
	}
// Nout end 16-9-2003
	else if(IS_METHOD(methodName, "TransparentBackground"))
	{
		ClearScreen = !arguments[0].boolValue();
		return true;
	}
	else if(IS_METHOD(methodName, "SetAttribute"))
	{
		strcpy(cparam0, arguments[0].str());
		iparam0 = arguments[1].intValue();
// changed QD 12/15/05
		//CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
		CPersistentAttributes *theInv;

		if(arguments.entries() > 2)
		{
			strcpy(cparam1, arguments[2].str());

			if(!stricmp(cparam1, "Player"))
				theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
			else
				theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(cparam1));
		}
		else
			theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
// end change
		returnValue = (int)theInv->Set(cparam0, iparam0);
		return true;
	}
	else if(IS_METHOD(methodName, "GetEventState"))
	{
		strcpy(cparam0, arguments[0].str());
		returnValue = (bool)GetTriggerState(cparam0);
		return true;
	}
	else if(IS_METHOD(methodName, "SetEventState"))
	{
		strcpy(cparam0, arguments[0].str());
		bparam0 = arguments[1].boolValue();
		CCD->Pawns()->AddEvent(cparam0, bparam0);
		return true;
	}
// Nout end 20-8-2003
	else if(IS_METHOD(methodName, "Conversation"))
	{
		returnValue = DoConversation(arguments[0].intValue());
		return true;
	}
	else if(IS_METHOD(methodName, "NewConversation"))
	{
		strcpy(cparam0, arguments[0].str());
		bparam0 = arguments[1].boolValue();
		strcpy(Order, cparam0);
		ConvFlag = !bparam0;
		return true;
	}
	else if(IS_METHOD(methodName, "CustomIcon"))
	{
		strcpy(cparam0, arguments[0].str());

		iparam0 = arguments[1].intValue();
		if(iparam0 >= 0)
		{
			IconX = iparam0;
		}

		iparam0 = arguments[2].intValue();
		if(iparam0 >= 0)
		{
			IconY = iparam0;
		}

		geBitmap *CustomIcon = CCD->Pawns()->GetCache(cparam0);
		if(CustomIcon)
		{
			Icon = CustomIcon;
		}
// begin add Nout 16/09/2003
		else
		{
			sprintf(cparam1, "[WARNING] %s: The bitmap name may not include capitals!", cparam0);
			CCD->ReportError(cparam1, false);
		}
//end add Nout 16/09/2003

		return true;
	}
	else if(IS_METHOD(methodName, "CustomBackground"))
	{
		strcpy(cparam0, arguments[0].str());
		geBitmap *CustomBackground = CCD->Pawns()->GetCache(cparam0);

		if(CustomBackground)
		{
			Background = CustomBackground;
		}
//begin add Nout 16/09/2003
		else
		{
			sprintf(cparam1, "[WARNING] %s: The bitmap name may not include capitals!", cparam0);
			CCD->ReportError(cparam1, false);
		}
//end add Nout 16/09/2003

		return true;
	}
	else if(IS_METHOD(methodName, "CustomSpeak"))
	{
		iparam0 = arguments[0].intValue();
		if(iparam0 >= 0)
		{
			SpeachX = iparam0;
		}

		iparam0 = arguments[1].intValue();
		if(iparam0 >= 0)
		{
			SpeachY = iparam0;
		}

		iparam0 = arguments[2].intValue();
		if(iparam0 >= 0)
		{
			SpeachWidth = iparam0;
		}

		iparam0 = arguments[3].intValue();
		if(iparam0 >= 0)
		{
			SpeachHeight = iparam0;
		}

		iparam0 = arguments[4].intValue();
		if(iparam0 >= 0)
		{
			SpeachFont = iparam0;
		}

		return true;
	}
	else if(IS_METHOD(methodName, "CustomReply"))
	{
		iparam0 = arguments[0].intValue();
		if(iparam0 >= 0)
		{
			ReplyX = iparam0;
		}

		iparam0 = arguments[1].intValue();
		if(iparam0 >= 0)
		{
			ReplyY = iparam0;
		}

		iparam0 = arguments[2].intValue();
		if(iparam0 >= 0)
		{
			MyReplyWidth = iparam0;
		}

		iparam0 = arguments[3].intValue();
		if(iparam0 >= 0)
		{
			MyReplyHeight = iparam0;
		}

		iparam0 = arguments[4].intValue();
		if(iparam0 >= 0)
		{
			ReplyFont = iparam0;
		}

		return true;
	}
	else if(IS_METHOD(methodName, "SetFlag"))
	{
		iparam0 = arguments[0].intValue();
		if(iparam0 >= MAXFLAGS)
		{
			return true;
		}

		bparam0 = arguments[1].boolValue();
		CCD->Pawns()->SetPawnFlag(iparam0, bparam0);
		return true;
	}
	else if(IS_METHOD(methodName, "GetFlag"))
	{
		iparam0 = arguments[0].intValue();
		if(iparam0 >= MAXFLAGS)
		{
			return true;
		}

		returnValue = CCD->Pawns()->GetPawnFlag(iparam0);
		return true;
	}
	else if(IS_METHOD(methodName, "GetAttribute"))
	{
		strcpy(cparam0, arguments[0].str());
		CPersistentAttributes *theInv;

		if(arguments.entries() > 1)
		{
			strcpy(cparam1, arguments[1].str());

			if(!stricmp(cparam1, "Player"))
				theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
			else
				theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(cparam1));
		}
		else
			theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());

		returnValue = theInv->Value(cparam0);
		return true;
	}
	else if(IS_METHOD(methodName, "ModifyAttribute"))
	{
		strcpy(cparam0, arguments[0].str());
		iparam0 = arguments[1].intValue();
		CPersistentAttributes *theInv;

		if(arguments.entries() > 2)
		{
			strcpy(cparam1, arguments[2].str());

			if(!stricmp(cparam1, "Player"))
				theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
			else
				theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(cparam1));
		}
		else
			theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());

		theInv->Modify(cparam0, iparam0);
		return true;
	}
// changed QD 12/15/05
	else if(IS_METHOD(methodName, "AddAttribute"))
	{
		// USAGE:	AddAttribute(char *Attribute)
		//			AddAttribute(char *Attribute, char *EntityName)
		//			AddAttribute(char *Attribute, int LowValue, int HighValue)
		//			AddAttribute(char *Attribute, int LowValue, int HighValue, char *EntityName)
		strcpy(cparam0, arguments[0].str());

		CPersistentAttributes *theInv;

		if(arguments.entries() == 2 || arguments.entries() == 4)
		{
			strcpy(cparam1, arguments[arguments.entries()-1].str());

			if(!stricmp(cparam1, "Player"))
				theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
			else
				theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(cparam1));
		}
		else
			theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());

		returnValue = (int)theInv->Add(cparam0);

		if(arguments.entries() > 2)
		{
			theInv->SetValueLimits(cparam0, arguments[1].intValue(), arguments[2].intValue());
		}

		return true;
	}
// end change
// changed QD 07/15/06
	else if(IS_METHOD(methodName, "SetAttributeValueLimits"))
	{
		// USAGE:	SetAttributeValueLimits(char* Attribute, int LowValue, int HighValue),
		//			SetAttributeValueLimits(char* Attribute, int LowValue, int HighValue, char* EntityName)

		strcpy(cparam0, arguments[0].str());

		CPersistentAttributes *theInv;

		if(arguments.entries() > 3)
		{
			strcpy(cparam1, arguments[3].str());

			if(!stricmp(cparam1, "Player"))
				theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
			else
				theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(cparam1));
		}
		else
			theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());

		theInv->SetValueLimits(cparam0, arguments[1].intValue(), arguments[2].intValue());

		return true;
	}
// end change QD 07/15/06
	else
	{
		return skScriptedExecutable::method(methodName, arguments, returnValue,ctxt);
	}
}

/* ------------------------------------------------------------------------------------ */
//	DoConversation
/* ------------------------------------------------------------------------------------ */
int ScriptedConverse::DoConversation(int charpersec)
{
	int choice;
	char *temp = new char[Text.length()+1];
	char *temp1 = NULL;
	DWORD OldTime = CCD->FreeRunningCounter();
	unsigned int counter = 1;

	if(charpersec == 0)
	{
		counter = Text.length();
	}

	DWORD ElapsedTime;
	int startline = 1;
	int replyline = 1;
	strcpy(temp, Text.substr(0, counter).c_str());

	std::string Reply1 = "";

	for(int j=0; j<9; j++)
	{
		if(replyflg[j])
		{
			if(Reply1 != "")
			{
				Reply1 += " ";
				Reply1[Reply1.length()-1] = (char)1;
			}

			Reply1 += Reply[j];
		}
	}

	if(Reply1 != "")
	{
		temp1 = new char[Reply1.length()+1];
		strcpy(temp1, Reply1.c_str());
	}

	// display scrolling text
	if(m_Streams)
	{
		m_Streams->Play(false);
	}

	if(CCD->GetHasFocus())
	{
// changed Nout - soundconversation...
		geEngine_BeginFrame(CCD->Engine()->Engine(), M_Camera, ClearScreen);

		geEngine_EndFrame(CCD->Engine()->Engine());
	}

	if(charpersec != 0)
	{
		while(counter < Text.length())
		{
			MSG msg;

			while(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
			{
				GetMessage(&msg, NULL, 0, 0);
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if(CCD->GetHasFocus())
			{
// changed Nout - soundconversation...
				geEngine_BeginFrame(CCD->Engine()->Engine(), M_Camera, ClearScreen);

				geEngine_DrawBitmap(CCD->Engine()->Engine(), Background, NULL, BackgroundX, BackgroundY);

				if(Icon)
					geEngine_DrawBitmap(CCD->Engine()->Engine(), Icon, NULL, BackgroundX+IconX, BackgroundY+IconY);

				ElapsedTime = (DWORD)(CCD->FreeRunningCounter() - OldTime);

				if(ElapsedTime > (DWORD)(1000/charpersec))
				{
					OldTime = CCD->FreeRunningCounter();
					counter++;
					strcpy(temp, Text.substr(0, counter).c_str());
				}

				TextDisplay(temp, SpeachWidth, SpeachFont);
// changed Nout 12/15/05
				if(DrawBackground)
					startline = TextOut(0, SpeachHeight, SpeachFont, BackgroundX+SpeachX, BackgroundY+SpeachY);
				else
// end change
					startline = TextOut(0, SpeachHeight, SpeachFont, SpeachX, SpeachY);

				geEngine_EndFrame(CCD->Engine()->Engine());
			}
		}
	}

	// display text and allow scrolling
	int key = -1;
	bool keyup = false;
	bool keydwn = false;
	bool ckeyup = false;
	bool ckeydwn = false;

	for(;;)
	{
		MSG msg;

		while(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			GetMessage(&msg, NULL, 0, 0);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(!CCD->GetHasFocus())
			continue;

		bool kup = false;
		bool kdwn = false;
		bool ckup = false;
		bool ckdwn = false;

// changed Nout - soundconversation...
		geEngine_BeginFrame(CCD->Engine()->Engine(), M_Camera, ClearScreen);

		geEngine_DrawBitmap(CCD->Engine()->Engine(), Background, NULL, BackgroundX, BackgroundY);

		if(Icon)
			geEngine_DrawBitmap(CCD->Engine()->Engine(), Icon, NULL, BackgroundX+IconX, BackgroundY+IconY);

		if(key == KEY_PAGEUP && startline != 0)
		{
			kup = true;

			if(!keyup)
			{
				keyup = true;

				if(startline > 1)
					startline -= 1;
			}
		}

		if(key == KEY_PAGEDOWN && startline != 0)
		{
			kdwn = true;

			if(!keydwn)
			{
				keydwn = true;
				startline += 1;
			}
		}

		TextDisplay(temp, SpeachWidth, SpeachFont);
// changed Nout 12/15/05
		if(DrawBackground)
			startline = TextOut(startline, SpeachHeight, SpeachFont, BackgroundX+SpeachX, BackgroundY+SpeachY);
		else
// end change
			startline = TextOut(startline, SpeachHeight, SpeachFont, SpeachX, SpeachY);

		if(Reply1 != "")
		{
			if(key == KEY_UP && replyline != 0)
			{
				ckup = true;

				if(!ckeyup)
				{
					ckeyup = true;

					if(replyline > 1)
						replyline -= 1;
				}
			}

			if(key == KEY_DOWN && replyline != 0)
			{
				ckdwn = true;

				if(!ckeydwn)
				{
					ckeydwn = true;
					replyline += 1;
				}
			}

			if(MyReplyWidth > 0)
				ReplyWidth = MyReplyWidth;

			if(MyReplyHeight > 0)
				ReplyHeight = MyReplyHeight;

			TextDisplay(temp1, ReplyWidth, ReplyFont);
// changed Nout 12/15/05
			if(DrawrBackground)
				replyline = TextOut(replyline, ReplyHeight, ReplyFont, rBackgroundX+ReplyX, rBackgroundY+ReplyY);
			else
// end change
				replyline = TextOut(replyline, ReplyHeight, ReplyFont, ReplyX, ReplyY);
		}

		geEngine_EndFrame(CCD->Engine()->Engine());

		if(Reply1 != "")
		{
			if(key == KEY_1 && replyflg[0])
			{
				choice = 1;
				while(CCD->Input()->GetKeyboardInputNoWait() != -1);
				break;
			}

			if(key == KEY_2 && replyflg[1])
			{
				choice = 2;
				while(CCD->Input()->GetKeyboardInputNoWait() != -1);
				break;
			}

			if(key == KEY_3 && replyflg[2])
			{
				choice = 3;
				while(CCD->Input()->GetKeyboardInputNoWait() != -1);
				break;
			}

			if(key == KEY_4 && replyflg[3])
			{
				choice = 4;
				while(CCD->Input()->GetKeyboardInputNoWait() != -1);
				break;
			}

			if(key == KEY_5 && replyflg[4])
			{
				choice = 5;
				while(CCD->Input()->GetKeyboardInputNoWait() != -1);
				break;
			}

			if(key == KEY_6 && replyflg[5])
			{
				choice = 6;
				while(CCD->Input()->GetKeyboardInputNoWait() != -1);
				break;
			}

			if(key == KEY_7 && replyflg[6])
			{
				choice = 7;
				while(CCD->Input()->GetKeyboardInputNoWait() != -1);
				break;
			}

			if(key == KEY_8 && replyflg[7])
			{
				choice = 8;
				while(CCD->Input()->GetKeyboardInputNoWait() != -1);
				break;
			}

			if(key == KEY_9 && replyflg[8])
			{
				choice = 9;
				while(CCD->Input()->GetKeyboardInputNoWait() != -1);
				break;
			}
		}
		else
		{
			if(key == KEY_ESC)
			{
				choice = -1;
				while(CCD->Input()->GetKeyboardInputNoWait() != -1);
				break;
			}
		}

		if(!kup)
			keyup = false;

		if(!kdwn)
			keydwn = false;

		if(!ckup)
			ckeyup = false;

		if(!ckdwn)
			ckeydwn = false;

		key = CCD->Input()->GetKeyboardInputNoWait();
	}

	if(m_Streams)
	{
		m_Streams->Delete();
		delete m_Streams;
		m_Streams = NULL;
	}

	delete temp;

	if(temp1)
		delete temp1;

	return choice;
}

//begin add Nout 16/09/2003
/* ------------------------------------------------------------------------------------ */
//	DoSoundConversation
/* ------------------------------------------------------------------------------------ */
int ScriptedConverse::DoSoundConversation(int charpersec, bool RenderHUD)
{
// changed Nout 12/15/05
	RECT client;
	POINT RectPos;

	if(!CCD->Engine()->FullScreen())
	{
		GetClientRect(CCD->Engine()->WindowHandle(),&client);
		RectPos.x = client.left;
		RectPos.y = client.top;
		ClientToScreen(CCD->Engine()->WindowHandle(),&RectPos);
	}
	else
	{
		RectPos.x = 0;
		RectPos.y = 0;
	}


	POINT MousePos;
	bool SoundWasPlayed = false;
// end change
	int key = -1;
	int choice;
	char *temp = new char[1024];
	char *temp1 = NULL;
	DWORD OldTime = CCD->FreeRunningCounter();
	DWORD ElapsedTime;
	int startline = 1;
	int replyline = 1;
	unsigned int counter = 1;

	if(charpersec == 0)
	{
		counter = Text.length();
	}

	strcpy(temp, Text.substr(0, counter).c_str());
	std::string Reply1 = "";

	for(int j=0; j<9; j++)
	{
		if(replyflg[j])
		{
			if(Reply1 != "")
			{
				Reply1 += " ";
				Reply1[Reply1.length()-1] = (char)1;
			}

			Reply1 += Reply[j];
// changed Nout 12/15/05
			temp1 = new char[Reply[j].length()+1];
			strcpy(temp1, Reply[j].c_str());

			if(CCD->MenuManager()->FontWidth(ReplyMenuFont, temp1) > ReplyWidth)
				ReplyWidth = CCD->MenuManager()->FontWidth(ReplyMenuFont, temp1)+2;

			delete(temp1);

			ReplyHeight += CCD->MenuManager()->FontHeight(ReplyMenuFont)+3;
// end change
		}
	}

	if(Reply1 != "")
	{
		temp1 = new char[Reply1.length()+1];
		strcpy(temp1, Reply1.c_str());
	}

	if(m_Streams)
		m_Streams->Play(false);

	// display scrolling text
	if(CCD->GetHasFocus())
	{
// changed Nout 12/15/05 - Render 1 frame to restore the background
		if(RestoreBackground || !ClearScreen)
		{
			CCD->Engine()->BeginFrame();	// Start up rendering for this frame
			CCD->RenderComponents();		// Render the RGF components
			CCD->Weapons()->Display();
			CCD->Engine()->RenderWorld();
			// changed QD 07/15/06
			CCD->Meshes()->Tick(0.f);

			if(RenderHUD)
				CCD->HUD()->Render();
		}
		else
// end change
			geEngine_BeginFrame(CCD->Engine()->Engine(), M_Camera, ClearScreen);

		geEngine_EndFrame(CCD->Engine()->Engine());
	}

	if(charpersec != 0)
	{
		while(counter < Text.length())
		{
			MSG msg;

			while(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
			{
				GetMessage(&msg, NULL, 0, 0);
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if(CCD->GetHasFocus())
			{
// changed QD 12/15/05
				if(RestoreBackground || !ClearScreen)
				{
					CCD->Engine()->BeginFrame();	// Start up rendering for this frame
					CCD->RenderComponents();		// Render the RGF components
					CCD->Weapons()->Display();
					CCD->Engine()->RenderWorld();
					// changed QD 07/15/06
					CCD->Meshes()->Tick(0.f);

					if(RenderHUD)
						CCD->HUD()->Render();
				}
				else
// end change
					geEngine_BeginFrame(CCD->Engine()->Engine(), M_Camera, ClearScreen);

				if(DrawBackground)
				{
					geEngine_DrawBitmap(CCD->Engine()->Engine(), Background, NULL, BackgroundX, BackgroundY);

					if(Icon)
						geEngine_DrawBitmap(CCD->Engine()->Engine(), Icon, NULL, BackgroundX+IconX, BackgroundY+IconY);
				}

				ElapsedTime = (DWORD)(CCD->FreeRunningCounter() - OldTime);

				if(ElapsedTime > (DWORD)(1000/charpersec))
				{
					OldTime = CCD->FreeRunningCounter();
					counter += 1+int(charpersec/20); //print more characters to make a faster speed
					strcpy(temp, Text.substr(0, counter).c_str());
				}

				TextDisplay(temp, SpeachWidth, SpeachFont);
// changed Nout 12/15/05
				if(DrawBackground)
					startline = TextOut(0, SpeachHeight, SpeachFont, BackgroundX+SpeachX, BackgroundY+SpeachY);
				else
// end change
					startline = TextOut(0, SpeachHeight, SpeachFont, SpeachX, SpeachY);

				geEngine_EndFrame(CCD->Engine()->Engine());

                // force fast displaying
				key = CCD->Input()->GetKeyboardInputNoWait();

				if(key == KEY_SPACE && counter < Text.length()-1)
				{
					while(CCD->Input()->GetKeyboardInputNoWait() != -1);
					counter = Text.length() - 1;
				}
			}
		}
	}

	// display text and allow scrolling
	key = -1;
	bool keyup = false;
	bool keydwn = false;
	bool ckeyup = false;
	bool ckeydwn = false;
// changed Nout 12/15/05
	int MouseChoice = -1;
// end change
	bool bShowCursor = CCD->MenuManager()->GetShowCursor();

	for(;;)
	{
		MSG msg;
		while(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			GetMessage(&msg, NULL, 0, 0);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(!CCD->GetHasFocus())
			continue;

		bool kup = false;
		bool kdwn = false;
		bool ckup = false;
		bool ckdwn = false;

// changed QD 12/15/05
		if(RestoreBackground || !ClearScreen)
		{
			CCD->Engine()->BeginFrame();	// Start up rendering for this frame
			CCD->RenderComponents();		// Render the RGF components
			CCD->Weapons()->Display();
			CCD->Engine()->RenderWorld();
			// changed QD 07/15/06
			CCD->Meshes()->Tick(0.f);

			if(RenderHUD)
				CCD->HUD()->Render();
		}
		else
// end change
			geEngine_BeginFrame(CCD->Engine()->Engine(), M_Camera, ClearScreen);

		if(DrawBackground)
		{
			geEngine_DrawBitmap(CCD->Engine()->Engine(), Background, NULL, BackgroundX, BackgroundY);

			if(Icon)
				geEngine_DrawBitmap(CCD->Engine()->Engine(), Icon, NULL, BackgroundX+IconX, BackgroundY+IconY);
		}

		if(key == KEY_UP && startline != 0)
		{
			kup = true;

			if(!keyup)
			{
				keyup = true;

				if(startline > 1)
					startline -= 1;
			}
		}

		if(key == KEY_DOWN && startline != 0)
		{
			kdwn = true;

			if(!keydwn)
			{
				keydwn = true;
				startline += 1;
			}
		}

		TextDisplay(temp, SpeachWidth, SpeachFont);
// changed Nout 12/15/05
		if(DrawBackground)
			startline = TextOut(startline, SpeachHeight, SpeachFont, BackgroundX+SpeachX, BackgroundY+SpeachY);
		else
// end change
			startline = TextOut(startline, SpeachHeight, SpeachFont, SpeachX, SpeachY);

		//Display the reply options and allow also here scrolling up and down
		if(Reply1 != "" && replyflg[1])
		{
			if(DrawrBackground)
				geEngine_DrawBitmap(CCD->Engine()->Engine(), rBackground, NULL, rBackgroundX, rBackgroundY);

			if(key == KEY_PAGEUP && replyline != 0)
			{
				ckup = true;

				if(!ckeyup)
				{
					ckeyup = true;

					if(replyline > 1)
						replyline -= 1;
				}
			}

			if(key == KEY_PAGEDOWN && replyline != 0)
			{
				ckdwn = true;

				if(!ckeydwn)
				{
					ckeydwn = true;
					replyline += 1;
				}
			}

// changed Nout 12/15/05
			if(MyReplyWidth > 0)
				ReplyWidth = MyReplyWidth;

			if(MyReplyHeight > 0)
				ReplyHeight = MyReplyHeight;
// end change

			TextDisplay(temp1, ReplyWidth, ReplyFont);
// changed Nout 12/15/05
			if(DrawrBackground)
				replyline = TextOut(replyline, ReplyHeight, ReplyFont, rBackgroundX+ReplyX+4, rBackgroundY+ReplyY);
			else
// end change
				replyline = TextOut(replyline, ReplyHeight, ReplyFont, ReplyX+4, ReplyY);
		}

// changed Nout 12/15/05
		// MenuReply
		// When menureply options exist, draw the menu (code allows both sound reply and
		// ..menureplay although this makes no sense)
		if(MouseReply)
		{
			CCD->MenuManager()->ShowCursor(true);
			GetCursorPos(&MousePos);

			//Compensate mouse-pos for windowed screens
			MousePos.x -= RectPos.x;
			MousePos.y -= RectPos.y;

			int FontHeight = (CCD->MenuManager()->FontHeight(ReplyFont)+2);
			int m=0;

			if(ReplyMenuBar)
			{
				if(geBitmap_Width(ReplyMenuBar) > ReplyWidth)
					ReplyWidth = geBitmap_Width(ReplyMenuBar);

				if(geBitmap_Height(ReplyMenuBar) > ReplyHeight)
					ReplyHeight = geBitmap_Height(ReplyMenuBar);
			}

			do
			{
				if(replyflg[m])
				{
					MouseRepPosX[m] = rBackgroundX+ReplyX;
					MouseRepPosY[m] = rBackgroundY+ReplyY+m*FontHeight;
					MouseRepWidth[m] = ReplyWidth;
					MouseRepHeight[m] = (CCD->MenuManager()->FontHeight(ReplyFont));

					if((MousePos.x > MouseRepPosX[m])
						&& (MousePos.x < (MouseRepPosX[m] + MouseRepWidth[m]))
						&& (MousePos.y > MouseRepPosY[m])
						&& (MousePos.y < (MouseRepPosY[m] + MouseRepHeight[m])))
					{
						if(ReplyMenuBar)
							geEngine_DrawBitmap(CCD->Engine()->Engine(), ReplyMenuBar, NULL, MouseRepPosX[m] ,MouseRepPosY[m]);

						//Check if we moved up or down by keys
						if((key == KEY_UP) && (m > 0))
						{
							if(replyflg[m-1])
							{
								SetCursorPos(MousePos.x+RectPos.x+1,rBackgroundY+ReplyY+(m-1)*FontHeight+RectPos.y+1);
								while(CCD->Input()->GetKeyboardInputNoWait()!=-1);
								key = NULL;
							}
						}
						else if((key == KEY_DOWN) && (replyflg[m+1]))
						{
							SetCursorPos(MousePos.x+RectPos.x+1,rBackgroundY+ReplyY+(m+1)*FontHeight+RectPos.y+1);
							while(CCD->Input()->GetKeyboardInputNoWait()!=-1){}
							key = NULL;
						}

						// Use ReplyMenuFont rather then ReplyFont to enable a different font/colour
						// for the font of the text on then active menubar
						if(DrawrBackground)
							replyline = TextOutLine(m, ReplyMenuFont, rBackgroundX+ReplyX+4, rBackgroundY+ReplyY);
						else
							replyline = TextOutLine(m, ReplyMenuFont, ReplyX+4, ReplyY);

						// Check for GIF to display
						if(ReplyGifNr[m]>0)
						{
							rIcon = CCD->Pawns()->GifFile[ReplyGifNr[m]-1]->NextFrame(true);
							if(rIcon)
								CCD->MenuManager()->DrawBitmap(rIcon, NULL, rBackgroundX+GifX, rBackgroundY+GifY+m*FontHeight);
						}

						// Check if mouse or key pressed
						if (!((GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0) ||
							(key == KEY_SPACE) || (key == KEY_RETURN)) //Mouse was clicked or SPACE or ENTER was pressed
						{
							while (!((GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0) &&
								(CCD->Input()->GetKeyboardInputNoWait()!=-1))
							{}	//Wait till the mouse is released  }

							MouseChoice = m+1; //Select the clicked reply message
						}
					}
				}

				m++;
			} while(MouseChoice < 0 && m < 9);

			CCD->MenuManager()->DisplayCursor();

			if(MouseChoice >= 0)
			{
				choice = MouseChoice;
				key = KEY_SPACE;
				MouseChoice = -1;
				break;
			}
		}
// end change

		geEngine_EndFrame(CCD->Engine()->Engine());
// Sound Conversation
// Begin add Nout 23092003
		ElapsedTime = (DWORD)(CCD->FreeRunningCounter() - OldTime);

		if((ElapsedTime > (DWORD)(SpeakShowTime)) && SpeakShowTime > 0)
		{
			OldTime = CCD->FreeRunningCounter();
			choice = 1;
			key = KEY_1;
			break;
		}
//End add Nout 23092003

		if(Reply1 != "" && replyflg[0])
		{
			if(key == KEY_SPACE
				&& replyflg[0]
				&& !replyflg[1]
				&& !replyflg[2]
				&& !replyflg[3]
				&& !replyflg[4]
				&& !replyflg[5]
				&& !replyflg[6]
				&& !replyflg[7]
				&& !replyflg[8])
			{
				choice = 1;
				while(CCD->Input()->GetKeyboardInputNoWait() != -1);
				key = KEY_1;
				break;
			}

			if(key == KEY_1 && replyflg[0])
			{
				choice = 1;
				while(CCD->Input()->GetKeyboardInputNoWait() != -1);
				break;
			}

			if(key == KEY_2 && replyflg[1])
			{
				choice = 2;
				while(CCD->Input()->GetKeyboardInputNoWait() != -1);
				break;
			}

			if(key == KEY_3 && replyflg[2])
			{
				choice = 3;
				while(CCD->Input()->GetKeyboardInputNoWait() != -1);
				break;
			}

			if(key == KEY_4 && replyflg[3])
			{
				choice = 4;
				while(CCD->Input()->GetKeyboardInputNoWait() != -1);
				break;
			}

			if(key == KEY_5 && replyflg[4])
			{
				choice = 5;
				while(CCD->Input()->GetKeyboardInputNoWait() != -1);
				break;
			}

			if(key == KEY_6 && replyflg[5])
			{
				choice = 6;
				while(CCD->Input()->GetKeyboardInputNoWait() != -1);
				break;
			}

			if(key == KEY_7 && replyflg[6])
			{
				choice = 7;
				while(CCD->Input()->GetKeyboardInputNoWait() != -1);
				break;
			}

			if(key == KEY_8 && replyflg[7])
			{
				choice = 8;
				while(CCD->Input()->GetKeyboardInputNoWait() != -1);
				break;
			}

			if(key == KEY_9 && replyflg[8])
			{
				choice = 9;
				while(CCD->Input()->GetKeyboardInputNoWait() != -1);
				break;
			}
		}
		else
		{
			if(key == KEY_SPACE)
			{
				choice = -1;
				while(CCD->Input()->GetKeyboardInputNoWait() != -1);
				key = -1;
				break;
			}
		}

		if(!kup)
			keyup = false;

		if(!kdwn)
			keydwn = false;

		if(!ckup)
			ckeyup = false;

		if(!ckdwn)
			ckeydwn = false;

		key = CCD->Input()->GetKeyboardInputNoWait();
	}

	if(m_Streams)
	{
		m_Streams->Delete();
		delete m_Streams;
		m_Streams = NULL;
	}

	// play the reply sound file
// changed Nout 12/15/05
	//if(Reply1 != "")
	if(Reply1 != "" && ReplySoundFileName[choice-1] != "")
	{
		char music[256];

		strcpy(music, CCD->GetDirectory(kAudioStreamFile));
		strcat(music, "\\");
		strcat(music, ReplySoundFileName[choice-1].c_str());
		m_dsPtr = (LPDIRECTSOUND)geSound_GetDSound();
		m_Streams_reply = new StreamingAudio(m_dsPtr);

		if(m_Streams_reply)
		{
			if(!m_Streams_reply->Create(music))
			{
				delete m_Streams_reply;
				m_Streams_reply = NULL;
			}
		}

		if(m_Streams_reply)
		{
			// changed Nout 12/15/05
			SoundWasPlayed = true;
			m_Streams_reply->Play(false);
		}
	}

	// show selected reply text
	key = -1;
	keyup = false;
	keydwn = false;
	ckeyup = false;
	ckeydwn = false;

	if(choice > 0)
	{
		Text = Reply[choice-1];
		if(Text.length() > 4)
			Text = Text.substr(4);
		else
			Text = " ";
	}
	else
		Text = " ";

	counter = 1;

	if(charpersec == 0)
		counter = Text.length();

	strcpy(temp, Text.substr(0, counter).c_str());
	startline = 1;

// changed Nout 12/15/05
	//if(charpersec != 0)
	if(charpersec != 0 && Text.length() > 0 && ShowSelectedReply)
	{
		while(counter < Text.length())
		{
			MSG msg;

 			while(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
			{
				GetMessage(&msg, NULL, 0, 0);
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if(CCD->GetHasFocus())
			{
// changed QD 12/15/05
				if(RestoreBackground || !ClearScreen)
				{
					CCD->Engine()->BeginFrame();	// Start up rendering for this frame
					CCD->RenderComponents();		// Render the RGF components
					CCD->Weapons()->Display();
					CCD->Engine()->RenderWorld();
					// changed QD 07/15/06
					CCD->Meshes()->Tick(0.f);

					if(RenderHUD)
						CCD->HUD()->Render();
				}
				else
// end change
					geEngine_BeginFrame(CCD->Engine()->Engine(), M_Camera, ClearScreen);

				if(ReplyInSpeakWindow)
				{
					if(DrawBackground)
					{
						geEngine_DrawBitmap(CCD->Engine()->Engine(), Background, NULL, BackgroundX, BackgroundY);

						if(Icon)
							geEngine_DrawBitmap(CCD->Engine()->Engine(), Icon, NULL, BackgroundX+IconX, BackgroundY+IconY);
					}

					ElapsedTime = (DWORD)(CCD->FreeRunningCounter() - OldTime);

					if(ElapsedTime > (DWORD)(1000/charpersec))
					{
						OldTime = CCD->FreeRunningCounter();
						counter += 1+int(charpersec/20); //print more characters to make a faster speed
						strcpy(temp, Text.substr(0, counter).c_str());
					}

					TextDisplay(temp, SpeachWidth, SpeachFont);
// changed Nout 12/15/05
					if(DrawBackground)
						startline = TextOut(0, SpeachHeight, SpeachFont, BackgroundX+SpeachX, BackgroundY+SpeachY);
					else
// end change
						startline = TextOut(0, SpeachHeight, SpeachFont, SpeachX, SpeachY);
				}
				else
				{
					if(DrawrBackground)
					{
						geEngine_DrawBitmap(CCD->Engine()->Engine(), rBackground, NULL, rBackgroundX, rBackgroundY);

						if(Icon)
							geEngine_DrawBitmap(CCD->Engine()->Engine(), rIcon, NULL, rBackgroundX+IconX, rBackgroundY+IconY);
					}

					ElapsedTime = (DWORD)(CCD->FreeRunningCounter() - OldTime);

					if(ElapsedTime > (DWORD)(1000/charpersec))
					{
						OldTime = CCD->FreeRunningCounter();
						counter += 1+int(charpersec/20); //print more characters to make a faster speed
						strcpy(temp, Text.substr(0, counter).c_str());
					}

// changed Nout 12/15/05
					if(MyReplyWidth > 0)
						ReplyWidth = MyReplyWidth;

					if(MyReplyHeight > 0)
						ReplyHeight = MyReplyHeight;
// end change
					TextDisplay(temp, ReplyWidth, ReplyFont);
// changed Nout 12/15/05
					if(DrawrBackground)
						startline = TextOut(0, ReplyHeight, ReplyFont, rBackgroundX+ReplyX, rBackgroundY+ReplyY);
					else
// end change
						startline = TextOut(0, ReplyHeight, ReplyFont, ReplyX, ReplyY);
				}

				geEngine_EndFrame(CCD->Engine()->Engine());

				// force fast displaying when SPACE pressed
				key = CCD->Input()->GetKeyboardInputNoWait();

				if(key == KEY_SPACE && counter < Text.length()-1)
				{
    				while(CCD->Input()->GetKeyboardInputNoWait() != -1);
					counter = Text.length() - 1;
				}
			}
		}
	}

	key = -1;

// changed Nout 12/15/05
	OldTime = CCD->FreeRunningCounter();
	if(ShowSelectedReply && Text.length() > 0 && replyflg[1])
	{
// end change
		for(;;)
		{
			MSG msg;

			while(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
			{
				GetMessage(&msg, NULL, 0, 0);
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if(!CCD->GetHasFocus())
				continue;

			bool kup = false;
			bool kdwn = false;
			bool ckup = false;
			bool ckdwn = false;

// changed Nout 12/15/05
			if(RestoreBackground || !ClearScreen)
			{
				CCD->Engine()->BeginFrame();
				CCD->RenderComponents();
				CCD->Weapons()->Display();
				CCD->Engine()->RenderWorld();
				// changed QD 07/15/06
				CCD->Meshes()->Tick(0.f);

				if(RenderHUD)
					CCD->HUD()->Render();
			}
			else
				geEngine_BeginFrame(CCD->Engine()->Engine(), M_Camera, ClearScreen);
// end change

			if(ReplyInSpeakWindow)
			{
				// changed Nout 12/15/05
				//geEngine_BeginFrame(CCD->Engine()->Engine(), M_Camera, ClearScreen);

				if(DrawBackground)
				{
					geEngine_DrawBitmap(CCD->Engine()->Engine(), Background, NULL, BackgroundX, BackgroundY);

					if(Icon)
						geEngine_DrawBitmap(CCD->Engine()->Engine(), Icon, NULL, BackgroundX+IconX, BackgroundY+IconY);
				}
			}
			else
			{
				// changed Nout 12/15/05
				//geEngine_BeginFrame(CCD->Engine()->Engine(), M_Camera, ClearScreen);

				if(DrawrBackground)
				{
					geEngine_DrawBitmap(CCD->Engine()->Engine(), rBackground, NULL, rBackgroundX, rBackgroundY);

					if(Icon)
						geEngine_DrawBitmap(CCD->Engine()->Engine(), rIcon, NULL, rBackgroundX+IconX, rBackgroundY+IconY);
				}
			}

			if(key == KEY_UP && startline != 0)
			{
				kup = true;

				if(!keyup)
				{
					keyup = true;

					if(startline > 1)
						startline -= 1;
				}
			}

			if(key == KEY_DOWN && startline != 0)
			{
				kdwn = true;

				if(!keydwn)
				{
					keydwn = true;
					startline += 1;
				}
			}

			if(ReplyInSpeakWindow)
			{
				TextDisplay(temp, SpeachWidth, SpeachFont);
// changed Nout 12/15/05
				if(DrawBackground)
					startline = TextOut(startline, SpeachHeight, SpeachFont, BackgroundX+SpeachX, BackgroundY+SpeachY);
				else
// end change
					startline = TextOut(startline, SpeachHeight, SpeachFont, SpeachX, SpeachY);
			}
			else
			{
// changed Nout 12/15/05
				if(MyReplyWidth > 0)
					ReplyWidth = MyReplyWidth;

				if(MyReplyHeight > 0)
					ReplyHeight = MyReplyHeight;
// end change
				TextDisplay(temp, ReplyWidth, ReplyFont);
// changed Nout 12/15/05
				if(DrawrBackground)
					startline = TextOut(startline, ReplyHeight, ReplyFont, rBackgroundX+ReplyX, rBackgroundY+ReplyY);
				else
// end change
					startline = TextOut(startline, ReplyHeight, ReplyFont, ReplyX, ReplyY);
			}

			geEngine_EndFrame(CCD->Engine()->Engine());

// Sound Conversation
// Begin add Nout 23/09/2003
			ElapsedTime = (DWORD)(CCD->FreeRunningCounter() - OldTime);

			if((ElapsedTime > (DWORD)(ReplyShowTime)) && ReplyShowTime > 0)
			{
				OldTime = CCD->FreeRunningCounter();
				choice = 1;
				key = KEY_1;
				break;
			}
// End add Nout 23/09/2003

			// changed Nout 12/15/05
			//if(key == KEY_SPACE)
			if(key == KEY_SPACE || (MouseReply && (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0))
			{
				while(CCD->Input()->GetKeyboardInputNoWait() != -1) {}
				break;
			}

			if(!kup)
				keyup = false;

			if(!kdwn)
				keydwn = false;

			if(!ckup)
				ckeyup = false;

			if(!ckdwn)
				ckeydwn = false;

			key = CCD->Input()->GetKeyboardInputNoWait();
		}
	}

// changed Nout 12/15/05
	if(SoundWasPlayed)
	{
		if(m_Streams_reply)
		{
			m_Streams_reply->Delete();
			delete m_Streams_reply;
			m_Streams_reply = NULL;
		}
	}

	if(temp)
    	delete temp;

	if(temp1)
		delete temp1;

// changed Nout 12/15/05
	CCD->MenuManager()->ShowCursor(bShowCursor);
	LastReplyNr = choice;
// end change

	return choice;
}
// end add Nout 16-9-2003

/* ------------------------------------------------------------------------------------ */
//	TextDisplay
/* ------------------------------------------------------------------------------------ */
void ScriptedConverse::TextDisplay(const char *Text, int Width, int Font)
{
	std::string WText = Text;
	std::string Line, Line1;
	int i, width, width1;
	char *temp = new char[strlen(Text)+1];
	int Index = 0;

	TextLines.resize(0);

	while(1)
	{
		Line = WText.substr(Index);
		strcpy(temp, Line.c_str());
		width = CCD->MenuManager()->FontWidth(Font, temp);

		if(width <= Width)
		{
			if(Line.find((char)1) == -1)
			{
				TextLines.push_back(Line);
				break;
			}
		}

		i = 1;

		while(1)
		{
			Line1 = Line.substr(0, i);
			strcpy(temp, Line1.c_str());

			if(Line1[Line1.length()-1] == 1)
			{
				Line1 = Line.substr(0, i-1);
				TextLines.push_back(Line1);
				Index += i;
				break;
			}

			width1 = CCD->MenuManager()->FontWidth(Font, temp);

			if(width1 > Width)
			{
				if(Line1[Line1.length()-1] == ' ')
				{
					Line1 = Line.substr(0, i-1);
					TextLines.push_back(Line1);
					Index += i;
					break;
				}

				Line1 = Line.substr(0, i-1);
				int space = Line1.rfind(' ');

				if(space == -1)
				{
					Line1 = Line.substr(0, i-1);
					TextLines.push_back(Line1);
					Index += i;
					break;
				}

				Line1 = Line.substr(0, space);
				TextLines.push_back(Line1);
				Index += (space+1);
				break;
			}

			i += 1;
		}
	}

	delete temp;
}

/* ------------------------------------------------------------------------------------ */
//	TextOut
/* ------------------------------------------------------------------------------------ */
int ScriptedConverse::TextOut(int startline, int Height, int Font, int X, int Y)
{
	int sline = -1, eline;
	int THeight;
	int size = TextLines.size();

	if(size > 0)
	{
		if(startline == 0)
		{
			sline = 0;
			eline = size;
			THeight = (size-sline) * (CCD->MenuManager()->FontHeight(Font)+2);

			while(THeight > Height)
			{
				sline += 1;
				THeight = (size-sline) * (CCD->MenuManager()->FontHeight(Font)+2);
			}
		}
		else
		{
			sline = startline-1;
			eline = size;
			THeight = (eline-sline) * (CCD->MenuManager()->FontHeight(Font)+2);

			if(sline > 0 && THeight < Height)
			{
				while(THeight < Height)
				{
					sline -= 1;
					if(sline < 0)
						break;

					THeight = (eline-sline) * (CCD->MenuManager()->FontHeight(Font)+2);
				}

				sline += 1;
			}
			else
			{
				while(THeight > Height)
				{
					eline -= 1;
					THeight = (eline-sline) * (CCD->MenuManager()->FontHeight(Font)+2);
				}
			}
		}

		int YOffset = Y;

		for(int i=sline; i<eline; i++)
		{
			std::string Text = TextLines[i];
			char *temp = new char[Text.length()+1];
			strcpy(temp, Text.c_str());
// changed Nout 12/15/05
			//CCD->MenuManager()->FontRect(temp, Font, BackgroundX+X, BackgroundY+YOffset);
			CCD->MenuManager()->FontRect(temp, Font, X, YOffset);
// end change
			YOffset += (CCD->MenuManager()->FontHeight(Font)+2);
			delete temp;
		}
	}

	return sline+1;
}

// changed Nout 12/15/05
/* ------------------------------------------------------------------------------------ */
//	TextOutLine
/* ------------------------------------------------------------------------------------ */
int ScriptedConverse::TextOutLine(int startline, int Font, int X, int Y)
{
	int THeight;
	int size = TextLines.size();

	if(startline > -1 && size > startline) //size > 0
	{
		THeight = CCD->MenuManager()->FontHeight(ReplyFont)+2;
		int YOffset = Y + (startline * (CCD->MenuManager()->FontHeight(ReplyFont)+2));

		{
			std::string Text = TextLines[startline];//(i);
			char *temp = new char[Text.length()+1];
			strcpy(temp, Text.c_str());
			CCD->MenuManager()->FontRect(temp, Font, X, YOffset);
			YOffset += (CCD->MenuManager()->FontHeight(ReplyFont)+2);
			delete temp;
		}

		return startline+1;
	}

	return 0;
}
// end change

/* ------------------------------------------------------------------------------------ */
//	PreLoadC
//
//	Preload conversation scripts
/* ------------------------------------------------------------------------------------ */
void CPawn::PreLoadC(const char *filename)
{
	geVFile *fdInput = NULL;
	char szInputString[1024] = {""};
	std::string str;
	int i, j;
	char file[256];
	int bmp, tga, jpg;

	if(!CCD->OpenRFFile(&fdInput, kScriptFile, filename, GE_VFILE_OPEN_READONLY))
		return;

	while(geVFile_GetS(fdInput, szInputString, 1024) == GE_TRUE)
	{
		if(szInputString[0] == ';' || strlen(szInputString) <= 1)
			continue;

		str = szInputString;
		TrimRight(str);
		MakeLower(str);

		i = 0;

		bmp = str.find(".bmp");
		if(bmp > 0)
			i = bmp;

		tga = str.find(".tga");
		if(tga > 0 && (tga < i || i == 0))
			i = tga;

		jpg = str.find(".jpg");
		if(jpg > 0 && (jpg < i || i == 0))
			i = jpg;

		while(i > 0 && i < (int)str.length())
		{
			j = i-1;

			while(!(str[j] == '"' || str[j] == '[') && j >= 0)
				j--;

			if(j >= 0)
			{
				strcpy(file, "conversation\\");
				strcat(file, str.substr(j+1, i-j+3).c_str());

				BitmapCache.resize(BitmapCache.size()+1);

				int keynum = BitmapCache.size()-1;

				BitmapCache[keynum].Name = str.substr(j+1, i-j+3);
				BitmapCache[keynum].Bitmap = CreateFromFileName(file);

				if(BitmapCache[keynum].Bitmap == (geBitmap *)NULL)
				{
					char szError[256];
					sprintf(szError, "[WARNING] CPawnCon: Failed to create image %s", file);
					CCD->ReportError(szError, false);
				}
				else
				{
					geEngine_AddBitmap(CCD->Engine()->Engine(), BitmapCache[keynum].Bitmap);
				}
			}

			if((int)str.size() > i+6) str = str.substr(i+6);
			else break;

			i = 0;

			bmp = str.find(".bmp");
			if(bmp > 0)
				i = bmp;

			tga = str.find(".tga");
			if(tga > 0 && (tga < i || i == 0))
				i = tga;

			jpg = str.find(".jpg");
			if(jpg > 0 && (jpg < i || i == 0))
				i = jpg;
		}
	}

	geVFile_Close(fdInput);
}

/* ------------------------------------------------------------------------------------ */
//	LoadConv
/* ------------------------------------------------------------------------------------ */
void CPawn::LoadConv(const char *convtxt)
{
	geVFile *MainFS;
	char szInputLine[256];
	std::string readinfo, keyname, text;

	Text.resize(0);

// changed QD Language Menu
	if(!CCD->OpenRFFile(&MainFS, kInstallFile, convtxt, GE_VFILE_OPEN_READONLY))
		return;

	keyname = "";
	text = "";

	while(geVFile_GetS(MainFS, szInputLine, 256) == GE_TRUE)
	{
		if(strlen(szInputLine) <= 1)
			readinfo = "";
		else
			readinfo = szInputLine;

		if(readinfo != "" && readinfo[0] != ';')
		{
			TrimRight(readinfo);

// changed QD 07/15/06 - if trim operation results in empty string...
			if(readinfo.length()<=1)
				continue;
// end change QD 07/15/06

			if(readinfo[0] == '[' && readinfo[readinfo.length()-1] == ']')
			{
				if(keyname != "" && text != "")
				{
					Text.resize(Text.size()+1);
					int keynum = Text.size()-1;
					Text[keynum].Name = keyname;
					Text[keynum].Text = text;
					Replace(Text[keynum].Text, "<Player>", CCD->Player()->GetPlayerName());
				}

				keyname = readinfo;
				TrimLeft(keyname, "[");
				TrimRight(keyname, "]");
				text = "";
			}
			else
			{
				if(readinfo == "<CR>")
				{
					text += " ";
					text[text.length()-1] = (char)1;
				}
				else
				{
					if(text != "" && text[text.length()-1] != 1)
						text += " ";

					text += readinfo;
				}
			}
		}
	}

	if(keyname != "" && text != "")
	{
		Text.resize(Text.size()+1);
		int keynum = Text.size()-1;
		Text[keynum].Name = keyname;
		Text[keynum].Text = text;
		Replace(Text[keynum].Text, "<Player>", CCD->Player()->GetPlayerName());
	}

	geVFile_Close(MainFS);
}

/* ------------------------------------------------------------------------------------ */
//	GetText
/* ------------------------------------------------------------------------------------ */
std::string CPawn::GetText(const char *Name)
{
	int size = Text.size();

	if(size < 1)
		return "";

	for(int i=0; i<size; i++)
	{
		if(Text[i].Name == Name)
			return Text[i].Text;
	}

	return "";
}

/* ------------------------------------------------------------------------------------ */
//	RunConverse
/* ------------------------------------------------------------------------------------ */
void CPawn::RunConverse(ScriptedConverse *Converse, const char *szName, geBitmap *OIcon)
{
	skRValueArray args;
	skRValue ret;

	if(!EffectC_IsStringNull(Converse->Order))
	{
		bool methoderror = false;
		try
		{
			Converse->CreateCamera();

			do
			{
				Converse->Background = Background;
				Converse->Icon = OIcon;
				Converse->BackgroundX = BackgroundX;
				Converse->BackgroundY = BackgroundY;
				Converse->IconX = IconX;
				Converse->IconY = IconY;
				Converse->SpeachX = SpeachX;
				Converse->SpeachY = SpeachY;
				Converse->SpeachWidth = SpeachWidth;
				Converse->SpeachHeight = SpeachHeight;
				Converse->SpeachFont = SpeachFont;
				Converse->ReplyX = ReplyX;
				Converse->ReplyY = ReplyY;
				Converse->ReplyWidth = ReplyWidth;
				Converse->ReplyHeight = ReplyHeight;
				Converse->ReplyFont = ReplyFont;
				Converse->ConvFlag = true;

// begin add Nout 16/09/2003
				Converse->rBackground = rBackground;
				Converse->SpeachWindowX = SpeachWindowX;
				Converse->SpeachWindowY = SpeachWindowY;
				Converse->ReplyWindowX = ReplyWindowX;
				Converse->ReplyWindowY = ReplyWindowY;
				Converse->rBackgroundX = rBackgroundX;
				Converse->rBackgroundY = rBackgroundY;
                Converse->ClearScreen = false;
				Converse->DrawBackground = true;
		        Converse->DrawrBackground = true;
				Converse->ReplyInSpeakWindow = true;
// end add Nout 16/09/2003

// Sound Conversation
// Begin add Nout 23092003
				Converse->SpeakShowTime = 0;
				Converse->ReplyShowTime = 0;
// End add Nout 23092003
// changed Nout 12/15/05
				Converse->RestoreBackground = true;
				Converse->MouseReply = false;
				Converse->ReplyMenuBar = ReplyMenuBar;
				Converse->ReplyMenuFont = ReplyMenuFont;
				Converse->GifX = GifX;
				Converse->GifY = GifY;
				Converse->ShowSelectedReply = false;

				for(int i=0; i<9; i++)
				{
					Converse->replyflg[i] = false;
					Converse->BGBitmap[i] = NULL;
					Converse->MouseOverBitmap[i] = NULL;
					Converse->MouseRepPosX[i] = false;
					Converse->MouseRepPosY[i] = false;
					Converse->MouseRepWidth[i] = false;
					Converse->MouseRepHeight[i] = false;
					Converse->SoundAtClick[i] = "";
					Converse->SoundMouseOver[i] = "";
				}
// end change
				Converse->method(skString(Converse->Order), args, ret, CCD->GetskContext()); // change simkin

			} while (!Converse->ConvFlag);
		}
		catch (...)
		{
			char szBug[256];
			sprintf(szBug, "Conversation Script Error for %s in Order %s", szName, Converse->Order);
			CCD->ReportError(szBug, false);
			methoderror = true;
		}

		Converse->DestroyCamera();
// changed Nout 12/15/05 - remove black fillrect
		//CCD->Engine()->ResetSystem();
		CCD->ResetClock();
// end change
	}
}

/* ------------------------------------------------------------------------------------ */
//	Converse
/* ------------------------------------------------------------------------------------ */
bool CPawn::Converse(const geActor *pActor)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	skRValueArray args;// change simkin
	skRValue ret;

	// Ok, check to see if there are Pawns in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "Pawn");

	if(pSet)
	{
		for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
			pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
		{
			Pawn *pSource = (Pawn*)geEntity_GetUserData(pEntity);

			if(pSource->Data)
			{
				ScriptedObject *Object = (ScriptedObject *)pSource->Data;

				if(Object->Actor == pActor && Object->active && Object->UseKey)
				{
					if(pSource->Converse)
					{
						ScriptedConverse *Converse = (ScriptedConverse*)pSource->Converse;
						RunConverse(Converse, Object->szName, Object->Icon);
					}

					return true;
				}
			}
		}
	}

	return false;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
