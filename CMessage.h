/************************************************************************************//**
 * @file CMessage.h
 * @brief Message class handler
 *
 * This file contains the class declaration for Message handling.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All Rights Reserved.
 ****************************************************************************************/

#ifndef __RGF_CMESSAGE_H_
#define __RGF_CMESSAGE_H_


typedef struct MessageText
{
	std::string Text;
	std::string Name;

} MessageText;


typedef struct MessageData
{
	bool		active;
	float		ticks;
	int			type;
	int			posx;
	int			posy;
	bool		centerx;
	bool		centery;
	float		time;
	float		fadeintime;
	float		fadeouttime;
	int			font;
	float		fadetime;
	int			fadein;
	float		alpha;
	float		alphastep;
	geBitmap	**graphic;
	int			graphicposx;
	int			graphicposy;
	bool		graphiccenterx;
	bool		graphiccentery;
	int			graphicstyle;
	int			graphicframes;
	int			graphicspeed;
	float		graphicfadeintime;
	float		graphicfadeouttime;
	float		graphicfadetime;
	int			graphicfadein;
	float		graphicalpha;
	float		graphicalphastep;
	float		graphicticks;
	int			graphiccur;
	int			graphicdir;

} MessageData;


class CMessage : public CRGFComponent
{
public:
	CMessage();
	~CMessage();

	void Tick(geFloat dwTicks);
	void Display();

	/**
	 * @brief Given a name, locate the desired entity in the currently loaded
	 * level and return its user data.
	 */
	int LocateEntity(const char *szName, void **pEntityData);

	/**
	 * @brief Correct internal timing to match current time, to make up for time
	 * lost when outside the game loop (typically in "menu mode").
	 */
	int ReSynchronize();
	void LoadText(const char *messagetxt);

private:
	std::string GetText(const char *Name);

private:
	std::vector<MessageText> Text;
	CIniFile AttrFile;
	bool active;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
