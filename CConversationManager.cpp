/************************************************************************************//**
 * @file CConversationManager.cpp
 * @brief ConversationManager class implementation
 *
 * This file contains the class implementation for the ConversationManager.
 * @author Daniel Queteschiner
 * @date August 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"
#include "CFileManager.h"
#include "IniFile.h"
#include "CDialogState.h"
#include "CGUIManager.h"
#include "CLanguageManager.h"
#include "CScriptManager.h"
#include "CLevel.h"
#include "CPawnManager.h"
#include "CConversation.h"
#include "RFSX/CEGUI/sxCEGUIWindow.h"
#include "RFSX/sxVector.h"

#include "Simkin/skExecutable.h"
#include "CConversationManager.h"



class ConversationSnippet : public skExecutable
{
public:
	ConversationSnippet(const std::string& textName, const std::string& sound = "",
						float textScrollSpeed = 0.f, float textDisplayTime = 0.f, const geVec3d *soundPos = NULL) :
		m_Active(false),
		m_Time(0.f),
		m_TextScrollSpeed(textScrollSpeed),
		m_TextDisplayTime(textDisplayTime),
		m_Audio(NULL),
		m_SoundPos(NULL),
		m_TextName(textName)
	{
		if(!sound.empty())
		{
			char filename[MAX_PATH];
			// TODO: Move DubbingDirectory to FileManager?
			strcpy(filename, CCD->LanguageManager()->GetActiveLanguage()->GetDubbingDirectory().c_str());
			strcat(filename, "\\");
			strcat(filename, sound.c_str());

			m_Audio = new StreamingAudio((LPDIRECTSOUND)geSound_GetDSound());

			if(m_Audio)
			{
				if(!m_Audio->Create(filename))
				{
					SAFE_DELETE(m_Audio);
				}
			}
		}

		if(soundPos)
		{
			m_SoundPos = new geVec3d;
			geVec3d_Copy(soundPos, m_SoundPos);
		}
	}

	~ConversationSnippet()
	{
		if(m_Audio)
		{
			m_Audio->Delete();
			delete m_Audio;
		}

		delete m_SoundPos;
	}

	bool IsActive() const	{ return m_Active; }
	void Activate()			{ m_Active = true; }

	float GetTime() const	{ return m_Time; }
	void ResetTime()		{ m_Time = 0; }
	void AdvanceTime(float timeElapsed) { m_Time += timeElapsed; ModifyAudio3D(); }

	float GetTextDisplayTime() const	{ return m_TextDisplayTime; }
	float GetTextScrollSpeed() const	{ return m_TextScrollSpeed; }

	const std::string& GetTextName() const	{ return m_TextName; }
	StreamingAudio*	GetAudio()				{ return m_Audio; }

private:
	void ModifyAudio3D()
	{
		if(m_Audio && m_Active && m_SoundPos)
		{
			m_Audio->Modify3D(m_SoundPos, CCD->Level()->GetAudibleRadius());
		}
	}
private:
	bool			m_Active;
	float			m_Time;
	float			m_TextScrollSpeed;
	float			m_TextDisplayTime;
	StreamingAudio*	m_Audio;
	geVec3d*		m_SoundPos;
	std::string		m_TextName;
};



#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxConversationManager

#ifdef SX_METHOD_ARGS
#undef SX_METHOD_ARGS
#endif
#define SX_METHOD_ARGS sxConversationMethodArgs

typedef struct SX_METHOD_ARGS
{
	bool (SX_IMPL_TYPE::*method) (float timeElapsed, skRValueArray& args);
	skRValueArray args;
} SX_METHOD_ARGS;


SX_IMPL_TYPE* SX_IMPL_TYPE::m_ConversationManager = NULL;


SX_IMPL_TYPE::SX_IMPL_TYPE() :
	m_Conversation(NULL),
	m_TextWindow(NULL),
	m_IconWindow(NULL),
	m_TextDisplayTime(0.f),
	m_TextScrollSpeed(0.f),
	m_IsSpeaking(false),
	m_IsConversing(false),
	m_Skip(false)
{
}


SX_IMPL_TYPE::~SX_IMPL_TYPE()
{
	ScriptManager::RemoveGlobalVariable("Conversation");

	DequeueAll();
}


void SX_IMPL_TYPE::Initialize()
{
	LoadConfiguration(CCD->LanguageManager()->GetActiveLanguage()->GetConversationConfigFilename());
	LoadText(CCD->LanguageManager()->GetActiveLanguage()->GetConversationTextFilename());
}


void SX_IMPL_TYPE::StartConversation(CConversation* conversation)
{
	DequeueAll();

	if(conversation == NULL)
		return;

	if(conversation->GetLayout())
	{
		m_Conversation = conversation;
		conversation->GetLayout()->show();
		m_IsConversing = true;
	}
	else if(m_Layouts["default"])
	{
		m_Conversation = conversation;
		m_Layouts["default"]->show();
		m_IsConversing = true;
	}
	else
	{
		m_Conversation = NULL;
	}
}


const std::string& SX_IMPL_TYPE::GetText(const std::string& name)
{
	if(m_Text.find(name) != m_Text.end())
		return m_Text[name];

	static std::string empty;
	return empty;
}


void SX_IMPL_TYPE::Update(float timeElapsed)
{
	while(!m_MethodQueue.empty())
	{
		if((this->*(m_MethodQueue.front()->method))(timeElapsed, m_MethodQueue.front()->args))
		{
			delete m_MethodQueue.front();
			m_MethodQueue.pop();
		}
		else
		{
			break;
		}
	}
}


void SX_IMPL_TYPE::Enqueue(SX_METHOD_ARGS* ma)
{
	if(ma)
	{
		m_MethodQueue.push(ma);
	}
}


void SX_IMPL_TYPE::Dequeue()
{
	if(!m_MethodQueue.empty())
	{
		delete m_MethodQueue.front();
		m_MethodQueue.pop();
	}
}


void SX_IMPL_TYPE::DequeueAll()
{
	if(m_TextWindow)
	{
		m_TextWindow->setText("");
	}

	if(m_IconWindow)
	{
		if(m_IconWindow->isPropertyPresent("Image"))
		{
			m_IconWindow->setProperty("Image", m_IconWindow->getPropertyDefault("Image"));
		}
	}

	m_Skip = false;

	while(!m_MethodQueue.empty())
	{
		delete m_MethodQueue.front();
		m_MethodQueue.pop();
	}
}


void SX_IMPL_TYPE::LoadText(const std::string& conversationtxt)
{
	geVFile *MainFS;
	char szInputLine[256];
	std::string readinfo, keyname, text;

	m_Text.clear();

	if(!CFileManager::GetSingletonPtr()->OpenRFFile(&MainFS, kInstallFile, conversationtxt.c_str(), GE_VFILE_OPEN_READONLY))
		return;

	while(geVFile_GetS(MainFS, szInputLine, 256) == GE_TRUE)
	{
		if(strlen(szInputLine) <= 1)
			readinfo.clear();
		else
			readinfo = szInputLine;

		if(!readinfo.empty() && readinfo[0] != ';')
		{
			TrimRight(readinfo);

			// if trim operation results in empty string...
			if(readinfo.length() <= 1)
				continue;

			if(readinfo[0] == '[' && readinfo[readinfo.length()-1] == ']')
			{
				if(!keyname.empty() && !text.empty())
				{
					m_Text[keyname] = text;
				}

				keyname = readinfo;
				TrimLeft(keyname, "[");
				TrimRight(keyname, "]");
				text.clear();
			}
			else
			{
				if(readinfo == "<CR>")
				{
					text += '\n';
				}
				else
				{
					if(!text.empty() && text[text.length()-1] != '\n')
						text += " ";

					text += readinfo;
				}
			}
		}
	}

	if(!keyname.empty() && !text.empty())
	{
		m_Text[keyname] = text;
	}

	geVFile_Close(MainFS);
}


bool SX_IMPL_TYPE::LoadConfiguration(const std::string& filename)
{
	CIniFile iniFile(filename);

	if(!iniFile.ReadFile())
	{
		sxLog::GetSingletonPtr()->Warning("Failed to open " + filename + " file.");
		return false;
	}

	// destroy all windows
	stdext::hash_map<std::string, CEGUI::Window*>::iterator iter = m_Layouts.begin();
	for(; iter!=m_Layouts.end(); ++iter)
	{
		CCD->GUIManager()->DestroyWindow(iter->second);
	}

	CCD->GUIManager()->CleanDeadWindowPool();

	m_Layouts.clear();

	std::string keyName = iniFile.FindFirstKey();
	std::string value;

	// Check if default screen is defined
	{
		if(iniFile.GetValue("Layouts", "default").empty())
		{
			sxLog::GetSingletonPtr()->Error("No default conversation screen defined in " + filename + " file");
			return false;
		}
	}

	// Schemes
	{
		std::string name = iniFile.FindFirstName("Schemes");

		while(!name.empty())
		{
			value = iniFile.GetValue("Schemes", name);
			CCD->GUIManager()->LoadScheme(value);

			name = iniFile.FindNextName();
		}
	}

	// Layouts
	{
		std::string name = iniFile.FindFirstName("Layouts");

		while(!name.empty())
		{
			if(m_Layouts.find(name) == m_Layouts.end())
			{
				value = iniFile.GetValue("Layouts", name);

				if(!value.empty())
				{
					sxLog::GetSingletonPtr()->Debug("Load conversation layout: " + name + " " + value);
					CEGUI::Window *win = CCD->GUIManager()->LoadWindowLayout(value);
					m_Layouts[name] = win;

					CDialogState::GetSingletonPtr()->GetDefaultWindow()->addChildWindow(m_Layouts[name]);

					m_Layouts[name]->hide();
				}
			}

			name = iniFile.FindNextName();
		}
	}

	// Scripts
	{
		std::string name = iniFile.FindFirstName("Scripts");

		while(!name.empty())
		{
			value = iniFile.GetValue("Scripts", name);
			CEGUI::System::getSingleton().executeScriptFile(value);

			name = iniFile.FindNextName();
		}
	}

	value = iniFile.GetValue("Windows", "textwindow");
	if(!value.empty())
	{
		m_TextWindow = CCD->GUIManager()->GetWindow(value);
	}

	value = iniFile.GetValue("Windows", "iconwindow");
	if(!value.empty())
	{
		m_IconWindow = CCD->GUIManager()->GetWindow(value);
	}

	return true;
}


void SX_IMPL_TYPE::SetTextWindow(const std::string& windowname)
{
	m_TextWindow = CCD->GUIManager()->GetWindow(windowname);
}


void SX_IMPL_TYPE::SetIconWindow(const std::string& windowname)
{
	m_IconWindow = CCD->GUIManager()->GetWindow(windowname);
}


SX_IMPL_TYPE* SX_IMPL_TYPE::GetSingletonPtr()
{
	if(!m_ConversationManager)
	{
		m_ConversationManager = new sxConversationManager();
#ifdef _DEBUG
		InitMHT();
#endif
		ScriptManager::AddGlobalVariable("Conversation", m_ConversationManager);
	}

	return m_ConversationManager;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */

Q_METHOD_IMPL(Run)
{
	if(!m_Conversation->GetOrder().empty())
		CEGUI::System::getSingleton().executeScriptGlobal(m_Conversation->GetOrder());
	return true;
}


Q_METHOD_IMPL(Call)
{
	CEGUI::System::getSingleton().executeScriptGlobal(args[0].str().c_str());
	return true;
}


Q_METHOD_IMPL(SetIcon)
{
	if(m_IconWindow)
	{
		if(m_IconWindow->isPropertyPresent("Image"))
		{
			m_IconWindow->setProperty("Image", (const CEGUI::utf8*)(args[0].str().c_str()));
		}
	}

	return true;
}


Q_METHOD_IMPL(Speak)
{
	ConversationSnippet* snippet = static_cast<ConversationSnippet*>(args[0].obj());

	if(snippet->IsActive())
	{
		// skip snippet as requested
		if(m_Skip)
		{
			if(m_TextWindow)
				m_TextWindow->setText("");

			if(snippet->GetAudio())
				snippet->GetAudio()->Stop();

			m_IsSpeaking = false;

			m_Skip = false;
			CCameraManager::GetSingletonPtr()->ReachScriptSyncPoint();
			return true; // done, remove from queue
		}

		float textTime = 0.f; // milliseconds

		// update text
		if(m_TextWindow && snippet->GetTextScrollSpeed() > 0.f)
		{
			std::string text = GetText(snippet->GetTextName());
			Replace(text, "<Player>", CCD->Player()->GetPlayerName());
			size_t chars = static_cast<size_t>(snippet->GetTime() * snippet->GetTextScrollSpeed() + 0.5f);

			// need to convert to CEGUI::String to get number of codepoints (instead of 8bit units)
			CEGUI::String str(reinterpret_cast<const CEGUI::utf8*>(text.c_str()));

			textTime = str.length() / snippet->GetTextScrollSpeed();

			if(chars < str.length())
				str = str.substr(0, chars);

			m_TextWindow->setText(str);
		}

		// check if displaying of text is done
		bool textDone = true;

		if(m_TextWindow)
		{
			if(m_TextDisplayTime > 0.f)
				textTime += snippet->GetTextDisplayTime();

			if(snippet->GetTime() < textTime)
				textDone = false;
		}

		if(textDone)
		{
			// check if audio is still playing
			if(snippet->GetAudio())
			{
				if(snippet->GetAudio()->IsPlaying())
				{
					snippet->AdvanceTime(timeElapsed);
					return false;
				}
			}

			// we're done with this snippet
			if(m_TextWindow)
				m_TextWindow->setText("");

			m_IsSpeaking = false;
			CCameraManager::GetSingletonPtr()->ReachScriptSyncPoint();
			return true; // done, remove from queue
		}
	}
	else // activate
	{
		snippet->ResetTime();

		if(m_TextWindow && snippet->GetTextScrollSpeed() <= 0)
		{
			std::string text = GetText(snippet->GetTextName());
			Replace(text, "<Player>", CCD->Player()->GetPlayerName());
			m_TextWindow->setText(reinterpret_cast<const CEGUI::utf8*>(text.c_str()));
		}

		if(snippet->GetAudio())
		{
			snippet->GetAudio()->Play(false);
		}

		m_IsSpeaking = true;
		snippet->Activate();
	}

	snippet->AdvanceTime(timeElapsed);

	return false;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */


bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return skExecutable::method(m_name, args, r_val, ctxt);
}


SX_METHOD_IMPL(Run)
{
	SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
	ma->method = Q_P_FUNC(Run);

	caller->Enqueue(ma);
	return true;
}


SX_METHOD_IMPL(Call)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
	ma->method = Q_P_FUNC(Call);
	ma->args = args;

	caller->Enqueue(ma);
	return true;
}

// Speak(string textSection, [string soundFile, float textScrollSpeed, float textDisplayTime, sxVector pos])
SX_METHOD_IMPL(Speak)
{
	if(args.entries() < 1 || args.entries() > 5)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	std::string sound;

	if(args.entries() > 1)
	{
		if(args[1].type() == skRValue::T_String)
			sound = args[1].str().c_str();
		else if(!RFSX::IS_VECTOR(args[1]))
			return false;
	}

	float textScrollSpeed = caller->GetTextScrollSpeed();

	if(args.entries() > 2)
	{
		if(args[2].type() == skRValue::T_Int || args[2].type() == skRValue::T_Float)
			textScrollSpeed = args[2].floatValue() * 0.001f;
		else if(!RFSX::IS_VECTOR(args[2]))
			return false;
	}

	float textDisplayTime = caller->GetTextDisplayTime();

	if(args.entries() > 3)
	{
		if(args[3].type() == skRValue::T_Int || args[3].type() == skRValue::T_Float)
			textDisplayTime = args[3].floatValue() * 1000.f;
		else if(!RFSX::IS_VECTOR(args[3]))
			return false;
	}

	geVec3d *soundPos = NULL;
	RFSX::sxVector *vec = RFSX::IS_VECTOR(args[args.entries()-1]);
	if(vec) soundPos = vec->Vector();

	SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
	ma->method = Q_P_FUNC(Speak);
	ma->args.append(skRValue(new ConversationSnippet(args[0].str().c_str(), sound, textScrollSpeed, textDisplayTime, soundPos), true));//false));//

	caller->Enqueue(ma);
	CCameraManager::GetSingletonPtr()->InsertScriptSyncPoint();
	return true;
}


SX_METHOD_IMPL(SetIcon)
{
	if(args.entries() < 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	SX_METHOD_ARGS *ma = new SX_METHOD_ARGS();
	ma->method = Q_P_FUNC(SetIcon);
	ma->args = args;
	caller->Enqueue(ma);
	return true;
}


SX_METHOD_IMPL(Skip)
{
	return true;
}


SX_METHOD_IMPL(SkipAll)
{
	return true;
}


SX_METHOD_IMPL(SetTextScrollSpeed)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int && args[0].type() != skRValue::T_Float)
		return false;

	// chars/second to chars/millisecond
	caller->SetTextScrollSpeed(args[0].floatValue() * 0.001f);
	return true;
}


SX_METHOD_IMPL(SetTextDisplayTime)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int && args[0].type() != skRValue::T_Float)
		return false;

	// seconds to milliseconds
	caller->SetTextDisplayTime(args[0].floatValue() * 1000.f);
	return true;
}


SX_METHOD_IMPL(SetTextWindow)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	caller->SetTextWindow(args[0].str().c_str());
	return true;
}


SX_METHOD_IMPL(SetIconWindow)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	caller->SetIconWindow(args[0].str().c_str());
	return true;
}


SX_METHOD_IMPL(GetOwner)
{
	r_val.assignObject(caller->GetConversation()->GetOwner());
	return true;
}


SX_METHOD_IMPL(GetText)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	std::string text = caller->GetText(args[0].str().c_str());
	Replace(text, "<Player>", CCD->Player()->GetPlayerName());
	r_val = skString(text.c_str());
	return true;
}


SX_METHOD_IMPL(SetEventState)
{
	skRValue::RType rtypes[] = { skRValue::T_String, skRValue::T_Bool };

	if(!RFSX::CheckParams(2, 2, args, rtypes))
		return false;

	CCD->Level()->Pawns()->AddEvent(args[0].str().c_str(), args[1].boolValue());
	return true;
}


SX_METHOD_IMPL(GetEventState)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	r_val = GetTriggerState(args[0].str().c_str());
	return true;
}


SX_METHOD_IMPL(SetFlag)
{
	skRValue::RType rtypes[] = { skRValue::T_Int, skRValue::T_Bool };

	if(!RFSX::CheckParams(2, 2, args, rtypes))
		return false;

	if(args[0].intValue() >= MAXFLAGS)
		return false;

	CCD->Level()->Pawns()->SetPawnFlag(args[0].intValue(), args[1].boolValue());
	return true;
}


SX_METHOD_IMPL(GetFlag)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	if(args[0].intValue() >= MAXFLAGS)
		return false;

	r_val = CCD->Level()->Pawns()->GetPawnFlag(args[0].intValue());
	return true;
}


RFSX::SX_INIT_MHT(SXCONVERSATIONMANAGER_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(Run);
	SX_ADD_METHOD(Call);
	SX_ADD_METHOD(Speak);
	SX_ADD_METHOD(SetIcon);
	SX_ADD_METHOD(Skip);
	SX_ADD_METHOD(SkipAll);
	SX_ADD_METHOD(SetTextScrollSpeed);
	SX_ADD_METHOD(SetTextDisplayTime);
	SX_ADD_METHOD(SetTextWindow);
	SX_ADD_METHOD(SetIconWindow);
	SX_ADD_METHOD(SetEventState);
	SX_ADD_METHOD(GetEventState);
	SX_ADD_METHOD(SetFlag);
	SX_ADD_METHOD(GetFlag);
	SX_ADD_METHOD(GetOwner);
	SX_ADD_METHOD(GetText);
}

#undef SX_METHOD_ARGS
#undef SX_IMPL_TYPE
