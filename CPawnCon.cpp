/************************************************************************************//**
 * @file CPawnCon.cpp
 * @brief Pawn Conversation
 * @author Ralph Deane
 *//*
 * Copyright (c) Ralph Deane; All Rights Reserved.
 ****************************************************************************************/

#include "RabidFramework.h"

extern geSound_Def *SPool_Sound(char *SName);
// changed Nout 12/15/05
extern geBitmap *TPool_Bitmap(char *DefaultBmp, char *DefaultAlpha, char *BName, char *AName);
// end change

#include "Simkin\\skScriptedExecutable.h"
#include "Simkin\\skRValue.h"
#include "Simkin\\skRValueArray.h"

//not commented, from picklses newsource.zip, nouts soundconversation...
// #define BACKCLEAR	GE_TRUE
#define BACKCLEAR	GE_FALSE

/* ------------------------------------------------------------------------------------ */
//	ScriptConverse class
/* ------------------------------------------------------------------------------------ */
ScriptedConverse::ScriptedConverse(char *fileName) : skScriptedExecutable(fileName,CCD->GetskContext()) //update simkin
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

	SpeachWidth = 0;
	SpeachHeight = 0;
	ReplyWidth = 0;
	ReplyHeight = 0;


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
	char param0[128];
	int param1;
	bool param2;

// not commented, from picklses newsource.zip, nouts soundconversation...
	char param3[128];

	param0[0] = '\0';

// not commented, from picklses newsource.zip, nouts soundconversation...
	param3[0] = '\0';

	if(IS_METHOD(methodName, "random"))
	{
		if(arguments[0].floatValue() <= arguments[1].floatValue())
			returnValue = (int)EffectC_Frand(arguments[0].floatValue(), arguments[1].floatValue());
		else
			returnValue = (int)EffectC_Frand(arguments[1].floatValue(), arguments[0].floatValue());

		return true;
	}
	else if(IS_METHOD(methodName, "Speak"))
	{
		strcpy(param0, arguments[0].str());
		Text = CCD->Pawns()->GetText(param0);
		strcpy(param0, arguments[1].str());

		if(!EffectC_IsStringNull(param0))
		{
			char music[256];
			strcpy(music, CCD->GetDirectory(kAudioStreamFile));
			strcat(music, "\\");
			strcat(music, param0);
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
		strcpy(param0, arguments[1].str());
		param1 = arguments[0].intValue();

		if(param1 > 9 || param1 < 1)
			return true;

		ostringstream oss;
		oss << param1 << " : ";
		Reply[param1-1] = oss.str();
		Reply[param1-1] += CCD->Pawns()->GetText(param0);
		TrimRight(Reply[param1-1]);
		TrimLeft(Reply[param1-1]);

		replyflg[param1-1] = true;
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
		strcpy(param0, arguments[1].str());
		param1 = arguments[0].intValue();
		strcpy(param3, arguments[2].str());

		if(param1 > 9 || param1 < 1)
			return true;

		ostringstream oss;
		oss << param1 << " : ";
		Reply[param1-1] = oss.str();
		Reply[param1-1] += CCD->Pawns()->GetText(param0);
		TrimRight(Reply[param1-1]);
		TrimLeft(Reply[param1-1]);

		ReplySoundFileName[param1-1] = param3;
		TrimRight(ReplySoundFileName[param1-1]);
		TrimLeft(ReplySoundFileName[param1-1]);

		replyflg[param1-1] = true;

// changed Nout 12/15/05
		ShowSelectedReply = true;

		if(arguments.entries() > 7)
		{
			param1 = arguments[3].intValue();
			if(param1>=0)
				ReplyX = param1;

			param1 = arguments[4].intValue();
			if(param1>=0)
				ReplyY = param1;

			param1 = arguments[5].intValue();

			if(param1>=0)
				MyReplyWidth = param1;

			param1 = arguments[6].intValue();

			if(param1>=0)
				MyReplyHeight = param1;

			param1 = arguments[7].intValue();
			if(param1>=0)
				ReplyFont = param1;
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
		param1 = arguments[0].intValue();
		param1--;
		strcpy(param0, arguments[1].str());
		strcpy(param3, arguments[2].str());

		if(param1 > 8 || param1 < 0)
			return true;

		if(arguments[1].str() == "")
			Reply[param1] = " ";
		else
		{
			Reply[param1] = CCD->Pawns()->GetText(param0);
			TrimRight(Reply[param1]);
			TrimLeft(Reply[param1]);
		}

	    ReplySoundFileName[param1] = param3;
		TrimRight(ReplySoundFileName[param1]);
		TrimLeft(ReplySoundFileName[param1]);

	    replyflg[param1] = true;
		MouseReply = true;
		ShowSelectedReply = false;

		if(arguments.entries() > 9)
		{
			GifX = arguments[8].intValue();
			GifY = arguments[9].intValue();
		}
		else
			GifX = GifY = 0;

		if(arguments.entries() > 7)
			ReplyGifNr[param1] = arguments[7].intValue();
		else
			ReplyGifNr[param1] = -1;

		if(arguments.entries() > 6)
		{
			strcpy(param0, arguments[5].str());

			if(strlen(param0) > 0)
			{
				geBitmap *Icon = CCD->Pawns()->GetCache(param0);

				if(Icon)
				{
					ReplyMenuBar = Icon;
					strcpy(param0, arguments[6].str());

					if(strlen(param0) > 0)
					{
						geBitmap *TIcon = CCD->Pawns()->GetCache(param0);

						if(TIcon)
							geBitmap_SetAlpha(Icon, TIcon);
						else
						{
							sprintf(param3, "*WARNING* Order %s, MenuReplyBarAlphaBitmap: AlphaBitmap %s not found", Order, param0);
							CCD->ReportError(param3, false);
						}
					}
				}
				else
				{
					sprintf(param3, "*WARNING* Order %s, MenuReplyBarBitmap: Bitmap %s not found", Order, param0);
					CCD->ReportError(param3, false);
				}
			}
			else
			{
				ReplyMenuBar = NULL;
			}
		}

		if(arguments.entries() > 4)
		{
			param1 = arguments[4].intValue();
			if(param1 >= 0)
				ReplyMenuFont = param1;
		}

		if(arguments.entries() > 3)
		{
			param1 = arguments[3].intValue();
			if(param1 >= 0)
				ReplyFont = param1;
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
		param2 = true;

		if(arguments.entries() > 1)
			param2 = arguments[1].boolValue();

		returnValue = DoSoundConversation(arguments[0].intValue(), param2);
		//returnValue = DoSoundConversation(arguments[0].intValue());
		return true;
	}
	// AttachSpeakToPawn(PawnName, OffsetX, OffsetY, TextAreaWidth, TextAreaHeight, TextFont)
	// Attaches the speak window + text to a defined pawn. Use as PawnName "Player" to itentify the player
	else if(IS_METHOD(methodName, "AttachSpeakToPawn"))
	{
		geVec3d Pos, ScreenPos;
		strcpy(param0, arguments[0].str());

		if(strlen(param0) > 0)
		{
			if(!stricmp(param0, "Player"))	//arguments[0].str()=="Player")
				Pos = CCD->Player()->Position();
			else
				CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(param0), &Pos);

			geCamera_Transform(CCD->CameraManager()->Camera(), &Pos, &ScreenPos);

			ScreenPos.X = (float)(CCD->Engine()->Width()/2 - 1) * (1.0f - ScreenPos.X / ScreenPos.Z);
			ScreenPos.Y = (float)(CCD->Engine()->Height()/2 - 1) * (1.0f + ScreenPos.Y / ScreenPos.Z);
			ScreenPos.Z = 0.0f;

			SpeachX = int(ScreenPos.X) + arguments[1].intValue();
			SpeachY = int(ScreenPos.Y) + arguments[2].intValue();

			param1 = arguments[3].intValue();
			if(param1 >= 0)
				SpeachWidth = param1;

			param1 = arguments[4].intValue();
			if(param1 >= 0)
				SpeachHeight = param1;

			param1 = arguments[5].intValue();
			if(param1 >= 0)
				SpeachFont = param1;

			DrawBackground = false; //enforce no speak background
		}

		return true;
	}
	// AttachReplyToPawn(PawnName, TextOffsetX, TextOffsetY, TextAreaWidth, TextAreaHeight, TextFont)
	// Attaches the reply window + text to a defined pawn, use as PawnName "Player" to identify the player
	else if(IS_METHOD(methodName, "AttachReplyToPawn"))
	{
		geVec3d Pos, ScreenPos;
		strcpy(param0, arguments[0].str());

		if(strlen(param0) > 0)
		{
			if(!stricmp(param0, "Player"))	//arguments[0].str()=="Player")
				Pos = CCD->Player()->Position();
			else
				CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(param0), &Pos);

			geCamera_Transform(CCD->CameraManager()->Camera(), &Pos, &ScreenPos);

			ScreenPos.X = (float)(CCD->Engine()->Width()/2 -1) * (1.0f - ScreenPos.X / ScreenPos.Z);
			ScreenPos.Y = (float)(CCD->Engine()->Height()/2 -1) * (1.0f + ScreenPos.Y / ScreenPos.Z);
			ScreenPos.Z = 0.0f;

			ReplyX = int(ScreenPos.X) + arguments[1].intValue();
			ReplyY = int(ScreenPos.Y) + arguments[2].intValue();

			param1 = arguments[3].intValue();
			if(param1 >= 0)
				MyReplyWidth = param1;

			param1 = arguments[4].intValue();
			if(param1 >= 0)
				MyReplyHeight = param1;

			param1 = arguments[5].intValue();
			if(param1 >= 0)
				ReplyFont = param1;

			ReplyInSpeakWindow = false; //enforce the reply in the reply window, not in the speak window
			DrawrBackground = false; //enforce to show no reply background
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
		bool MultiReply = false; //Indicates if a multiple reply menu is created or not
		Index = arguments[0].intValue() - 1;

		if(Index < 0 || !replyflg[Index])
		{
			sprintf(param3, "GetMouseClick with ReplyNr=%d: no Reply() or SoundReply() command found before this command with ReplyNumber=%d", Index);
			MessageBox(NULL, param3, "Error", MB_OK);
			return true;
		}

		strcpy(param0, arguments[1].str());
		geBitmap *TIcon = CCD->Pawns()->GetCache(param0);

		if(TIcon)
			BGBitmap[Index] = TIcon;
		else
		{
			BGBitmap[Index] = NULL;
			sprintf(param3, "Script Command GetMouseClick: Bitmap %s not found", param0);
			CCD->ReportError(param3, false);
		}

		strcpy(param0, arguments[2].str());
		result = strtok(param0, delim);
		MouseRepPosX[Index] = atoi(result);
		result = strtok(NULL, delim);
		if(result != NULL)
			dPosX = atoi(result);
		else
			dPosX = 0;

		strcpy(param0, arguments[3].str());
		result = strtok(param0, delim);
		MouseRepPosY[Index] = atoi(result);
		result = strtok(NULL, delim);
		if(result != NULL)
			dPosY = atoi(result);
		else
			dPosY = 0;

		MouseRepWidth[Index] = arguments[4].intValue();
		MouseRepHeight[Index] = arguments[5].intValue();

		strcpy(param0, arguments[6].str());
		if(!EffectC_IsStringNull(param0))
			SoundAtClick[Index] = param0;
		else
			SoundAtClick[Index] = "";

		strcpy(param0, arguments[7].str());
		if(!EffectC_IsStringNull(param0))
			SoundMouseOver[Index] = param0;
		else
			SoundMouseOver[Index] = "";

		strcpy(param0, arguments[8].str());
		geBitmap *SIcon = CCD->Pawns()->GetCache(param0);
		if(SIcon)
			MouseOverBitmap[Index] = SIcon;
		else
		{
			MouseOverBitmap[Index] = NULL;
			sprintf(param3, "*WARNING* Script Command GetMouseClick: bitmap %s not found", param0);
			CCD->ReportError(param3, false);
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
		strcpy(param0, arguments[0].str());

		if(strlen(param0) > 0) //if no .BMP name, then use no background
		{
			geBitmap *TIcon = CCD->Pawns()->GetCache(param0);

			if(TIcon)
			{
				Background = TIcon;

				if(Background)
					geBitmap_SetColorKey(Background, GE_TRUE, 255, GE_FALSE);
			}
			else
			{
				sprintf(param3, "*WARNING* %s: The bitmap name may not include capitals!", param0);
				CCD->ReportError(param3, false);
			}
		}
		else
			DrawBackground = false;

		param1 = arguments[1].intValue();
		BackgroundX = param1;
		param1 = arguments[2].intValue();
		BackgroundY = param1;

// changed Nout 12/15/05
		if(arguments.entries() > 4)
		{
			param1 = arguments[3].intValue();
			if(param1 >= 0)
				SpeachX = param1;
			param1 = arguments[4].intValue();
			if(param1 >= 0)
				SpeachY = param1;
		}

		if(arguments.entries() > 6)
		{
			param1 = arguments[5].intValue();
			if(param1 >= 0)
				SpeachWidth = param1;
			param1 = arguments[6].intValue();
			if(param1 >= 0)
				SpeachHeight = param1;
		}

		if(arguments.entries() > 7)
		{
			param1 = arguments[7].intValue();
			if(param1 >= 0)
				SpeachFont = param1;
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
		strcpy(param0, arguments[0].str());

		if(strlen(param0) > 0) //if no .BMP name,then use no background
		{
			geBitmap *TIcon = CCD->Pawns()->GetCache(param0);

			if(TIcon)
			{
				rBackground = TIcon;

				if(rBackground)
					geBitmap_SetColorKey(rBackground, GE_TRUE, 255, GE_FALSE);
			}
			else
			{
				sprintf(param3, "*WARNING* %s: The bitmap name may not include capitals!", param0);
				CCD->ReportError(param3, false);
			}
		}
		else
			DrawrBackground = false;

		param1 = arguments[1].intValue();
		rBackgroundX = param1;
		param1 = arguments[2].intValue();
		rBackgroundY = param1;

// changed Nout 12/15/05
		if(arguments.entries() > 4)
		{
			param1 = arguments[3].intValue();
			if(param1 >= 0)
				ReplyX = param1;
			param1 = arguments[4].intValue();
			if(param1 >= 0)
				ReplyY = param1;
		}

		if (arguments.entries() > 6)
		{
			param1 = arguments[5].intValue();
			if(param1 >= 0)
				MyReplyWidth = param1;
			param1 = arguments[6].intValue();
			if(param1 >= 0)
				MyReplyHeight = param1;
		}

		if(arguments.entries() > 7)
		{
			param1 = arguments[7].intValue();
			if(param1 >= 0)
				ReplyFont = param1;
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
		strcpy(param0, arguments[0].str());
		param1 = arguments[1].intValue();
// changed QD 12/15/05
		//CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
		CPersistentAttributes *theInv;

		if(arguments.entries() > 2)
		{
			strcpy(param3, arguments[2].str());

			if(!stricmp(param3, "Player"))
				theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
			else
				theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(param3));
		}
		else
			theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
// end change
		returnValue = (int)theInv->Set(param0, param1);
		return true;
	}
	else if(IS_METHOD(methodName, "GetEventState"))
	{
		strcpy(param0, arguments[0].str());
		returnValue = (bool)GetTriggerState(param0);
		return true;
	}
	else if(IS_METHOD(methodName, "SetEventState"))
	{
		strcpy(param0, arguments[0].str());
		param2 = arguments[1].boolValue();
		CCD->Pawns()->AddEvent(param0, param2);
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
		strcpy(param0, arguments[0].str());
		param2 = arguments[1].boolValue();
		strcpy(Order, param0);
		ConvFlag = !param2;
		return true;
	}
	else if(IS_METHOD(methodName, "CustomIcon"))
	{
		strcpy(param0, arguments[0].str());
		param1 = arguments[1].intValue();

		if(param1 >= 0)
			IconX = param1;

		param1 = arguments[2].intValue();

		if(param1 >= 0)
			IconY = param1;

		geBitmap *TIcon = CCD->Pawns()->GetCache(param0);

		if(TIcon)
		{
			Icon = TIcon;

// begin add Nout 16/09/2003
// Changed by Nout 16/09/2003
//			if(Icon)
//				geBitmap_SetColorKey(Icon, GE_TRUE, 0x00ff00fe, GE_TRUE);
		}
// begin add Nout 16/09/2003
		else
		{
			sprintf(param3, "*WARNING* %s: The bitmap name may not include capitals!", param0);
			CCD->ReportError(param3, false);

		}
//end add Nout 16/09/2003

		return true;
	}
	else if(IS_METHOD(methodName, "CustomBackground"))
	{
		strcpy(param0, arguments[0].str());
		geBitmap *TIcon = CCD->Pawns()->GetCache(param0);

		if(TIcon)
			Background = TIcon;
//begin add Nout 16/09/2003
		else
		{
			sprintf(param3, "*WARNING* %s: The bitmap name may not include capitals!", param0);
			CCD->ReportError(param3, false);
		}
//end add Nout 16/09/2003

		return true;
	}
	else if(IS_METHOD(methodName, "CustomSpeak"))
	{
		param1 = arguments[0].intValue();

		if(param1 >= 0)
			SpeachX = param1;
		param1 = arguments[1].intValue();

		if(param1 >= 0)
			SpeachY = param1;

		param1 = arguments[2].intValue();

		if(param1 >= 0)
			SpeachWidth = param1;

		param1 = arguments[3].intValue();

		if(param1 >= 0)
			SpeachHeight = param1;

		param1 = arguments[4].intValue();

		if(param1 >= 0)
			SpeachFont = param1;

		return true;
	}
	else if(IS_METHOD(methodName, "CustomReply"))
	{
		param1 = arguments[0].intValue();

		if(param1 >= 0)
			ReplyX = param1;

		param1 = arguments[1].intValue();

		if(param1 >= 0)
			ReplyY = param1;

		param1 = arguments[2].intValue();
		if(param1 >= 0)
			MyReplyWidth = param1;

		param1 = arguments[3].intValue();
		if(param1 >= 0)
			MyReplyHeight = param1;

		param1 = arguments[4].intValue();

		if(param1 >= 0)
			ReplyFont = param1;

		return true;
	}
	else if(IS_METHOD(methodName, "SetFlag"))
	{
		param1 = arguments[0].intValue();

		if(param1 >= MAXFLAGS)
			return true;

		param2 = arguments[1].boolValue();
		CCD->Pawns()->SetPawnFlag(param1, param2);
		return true;
	}
	else if(IS_METHOD(methodName, "GetFlag"))
	{
		param1 = arguments[0].intValue();

		if(param1 >= MAXFLAGS)
			return true;

		returnValue = CCD->Pawns()->GetPawnFlag(param1);

		return true;
	}
	else if(IS_METHOD(methodName, "GetAttribute"))
	{
		strcpy(param0, arguments[0].str());
// changed QD 12/15/05
		//CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
		CPersistentAttributes *theInv;

		if(arguments.entries() > 1)
		{
			strcpy(param3, arguments[1].str());

			if(!stricmp(param3, "Player"))
				theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
			else
				theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(param3));
		}
		else
			theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
// end change
		returnValue = theInv->Value(param0);
		return true;
	}
	else if(IS_METHOD(methodName, "ModifyAttribute"))
	{
		strcpy(param0, arguments[0].str());
		param1 = arguments[1].intValue();
// changed QD 12/15/05
		//CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
		CPersistentAttributes *theInv;

		if(arguments.entries() > 2)
		{
			strcpy(param3, arguments[2].str());

			if(!stricmp(param3, "Player"))
				theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
			else
				theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(param3));
		}
		else
			theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
// end change
		theInv->Modify(param0, param1);
		return true;
	}
// changed QD 12/15/05
	else if(IS_METHOD(methodName, "AddAttribute"))
	{
		// USAGE:	AddAttribute(char *Attribute)
		//			AddAttribute(char *Attribute, char *EntityName)

		// changed QD 07/15/06 - add optional arguments
		//			AddAttribute(char *Attribute, int LowValue, int HighValue)
		//			AddAttribute(char *Attribute, int LowValue, int HighValue, char *EntityName)
		strcpy(param0, arguments[0].str());

		CPersistentAttributes *theInv;

		if(arguments.entries() == 2 || arguments.entries() == 4)
		{
			strcpy(param3, arguments[arguments.entries()-1].str());

			if(!stricmp(param3, "Player"))
				theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
			else
				theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(param3));
		}
		else
			theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());

		returnValue = (int)theInv->Add(param0);

		if(arguments.entries() > 2)
		{
			theInv->SetValueLimits(param0, arguments[1].intValue(), arguments[2].intValue());
		}
		// end change QD 07/15/06

		return true;
	}
// end change
// changed QD 07/15/06
	else if(IS_METHOD(methodName, "SetAttributeValueLimits"))
	{
		// USAGE:	SetAttributeValueLimits(char* Attribute, int LowValue, int HighValue),
		//			SetAttributeValueLimits(char* Attribute, int LowValue, int HighValue, char* EntityName)

		strcpy(param0, arguments[0].str());

		CPersistentAttributes *theInv;

		if(arguments.entries() > 3)
		{
			strcpy(param3, arguments[3].str());

			if(!stricmp(param3, "Player"))
				theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
			else
				theInv = CCD->ActorManager()->Inventory(CCD->ActorManager()->GetByEntityName(param3));
		}
		else
			theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());

		theInv->SetValueLimits(param0, arguments[1].intValue(), arguments[2].intValue());

		return true;
	}
// end change QD 07/15/06
	else
	{
		return skScriptedExecutable::method(methodName, arguments, returnValue,ctxt);//change simkin
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
		counter = Text.length();

	DWORD ElapsedTime;
	int startline = 1;
	int replyline = 1;
	strcpy(temp, Text.substr(0, counter).c_str());

	string Reply1 = "";

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
// changed Nout 12/15/05
//int ScriptedConverse::DoSoundConversation(int charpersec)
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
		counter = Text.length();

	strcpy(temp, Text.substr(0, counter).c_str());
	string Reply1 = "";

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
			ShowCursor(TRUE);
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

						//Check for GIF to display
						if(ReplyGifNr[m]>0)
						{
							rIcon = CCD->Pawns()->GifFile[ReplyGifNr[m]]->NextFrame(true);
							if(rIcon)
								CCD->MenuManager()->DrawBitmap(rIcon, NULL, rBackgroundX+GifX, rBackgroundY+GifY+m*FontHeight);
						}

						//Check if mouse or key pressed
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
	ShowCursor(FALSE); //hide mouse pointer
	LastReplyNr = choice;
// end change

	return choice;
}
// end add Nout 16-9-2003

/* ------------------------------------------------------------------------------------ */
//	TextDisplay
/* ------------------------------------------------------------------------------------ */
void ScriptedConverse::TextDisplay(char *Text, int Width, int Font)
{
	string WText = Text;
	string Line, Line1;
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
			string Text = TextLines[i];
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
			string Text = TextLines[startline];//(i);
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
void CPawn::PreLoadC(char *filename)
{
// changed Nout 12/15/05
	//FILE *fdInput = NULL;
	geVFile *fdInput = NULL;
// end change
	char szInputString[1024] = {""};
	string str;
	int i, j;
	char file[256];
// changed Nout 12/15/05
	int bmp, tga, jpg, gif;
// end change

// changed Nout 12/15/05
//	fdInput = fopen(filename, "rt");
//
//	if(!fdInput)
//		return;
//
//	while(fgets(szInputString, 1024, fdInput) != NULL)

	if(!CCD->OpenRFFile(&fdInput, kScriptFile, filename, GE_VFILE_OPEN_READONLY))
		return;

	while(geVFile_GetS(fdInput, szInputString, 1024) == GE_TRUE)
// end change
	{
		if(szInputString[0] == ';' || strlen(szInputString) <= 1)
			continue;

		str = szInputString;
		TrimRight(str);
		MakeLower(str);

// changed Nout 12/15/05
/*
		i = str.Find(".bmp");

		if(i >= 0 && i < str.GetLength())
		{
			j = i-1;

			while(!(str.GetAt(j) == '"' || str.GetAt(j) == '[') && j >= 0)
			{
				j -= 1;
			}

			if(j >= 0)
			{
				strcpy(file, "conversation\\");
				strcat(file, str.Mid(j+1, i-j+3));
				Cache.SetSize(Cache.GetSize()+1);
				int keynum = Cache.GetSize()-1;
				Cache[keynum].Name = str.Mid(j+1, i-j+3);
				Cache[keynum].Bitmap = CreateFromFileName(file);
				geEngine_AddBitmap(CCD->Engine()->Engine(), Cache[keynum].Bitmap);
			}
		}
		else
		{
			i = str.Find(".tga");
			if(i >= 0 && i < str.GetLength())
			{
				j = i-1;

				while(!(str.GetAt(j) == '"' || str.GetAt(j) == '[') && j >= 0)
				{
					j -= 1;
				}

				if(j >= 0)
				{
					strcpy(file, "conversation\\");
					strcat(file, str.Mid(j+1, i-j+3));
					Cache.SetSize(Cache.GetSize()+1);
					int keynum = Cache.GetSize()-1;
					Cache[keynum].Name = str.Mid(j+1, i-j+3);
					Cache[keynum].Bitmap = CreateFromFileName(file);
					geEngine_AddBitmap(CCD->Engine()->Engine(), Cache[keynum].Bitmap);
				}
			}
		}
*/
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

		gif = str.find(".gif");
		if(gif > 0 && (gif < i || i == 0))
			i = gif;

		while(i > 0 && i < (int)str.length())
		{
			j = i-1;

			while(!(str[j] == '"' || str[j] == '[') && j >= 0)
				j--;

			if(j >= 0)
			{
				strcpy(file, "conversation\\");
				strcat(file, str.substr(j+1, i-j+3).c_str());

				Cache.resize(Cache.size()+1);

				int keynum = Cache.size()-1;

				Cache[keynum].Name = str.substr(j+1, i-j+3);
				Cache[keynum].Bitmap = CreateFromFileName(file);
				// changed QD 07/15/06
				if(Cache[keynum].Bitmap == (geBitmap *)NULL)
				{
					char szError[256];
					sprintf(szError, "*WARNING* CPawnCon: Failed to create image %s", file);
					CCD->ReportError(szError, false);
				}
				else
				{
					geEngine_AddBitmap(CCD->Engine()->Engine(), Cache[keynum].Bitmap);
				}
				// end change
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

			gif = str.find(".gif");
			if(gif > 0 && (gif < i || i == 0))
				i = gif;
		}
// end change
	}

// changed Nout 12/15/05
	//fclose(fdInput);
	geVFile_Close(fdInput);
// end change
}

/* ------------------------------------------------------------------------------------ */
//	LoadConv
/* ------------------------------------------------------------------------------------ */
void CPawn::LoadConv(char *convtxt)
{
	geVFile *MainFS;
	char szInputLine[256];
	string readinfo, keyname, text;

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
// changed QD 12/15/05
					Replace(Text[keynum].Text, "<Player>", CCD->Player()->GetPlayerName());
// end change
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
// changed QD 12/15/05
		Replace(Text[keynum].Text, "<Player>", CCD->Player()->GetPlayerName());
// end change
	}

	geVFile_Close(MainFS);
}

/* ------------------------------------------------------------------------------------ */
//	GetText
/* ------------------------------------------------------------------------------------ */
string CPawn::GetText(char *Name)
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
void CPawn::RunConverse(ScriptedConverse *Converse, char *szName, geBitmap *OIcon)
{
	skRValueArray args;// change simkin
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
bool CPawn::Converse(geActor *pActor)
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
