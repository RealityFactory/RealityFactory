/*
	CInventory.h:		Inventory class handler

	(c) 2000 Ralph Deane
	All Rights Reserved

	This file contains the class declaration for Inventory
handling.
*/

#ifndef __RGF_CINVENTORY_H_
#define __RGF_CINVENTORY_H_

typedef enum
{
	INVWEAPON = 0,
	INVUSE,
	INVSTATIC,
	INVLOAD,
	INVSAVE,
	INVMODIFY
};

typedef struct InvItem
{
	InvItem *next;
	InvItem *prev;
	int catagory;
	char Attribute[64];
	geBitmap *Image;
	int Amount;
	char Modify[64];
	char text[64];
	bool Decrease;
} InvItem;

class CInventory : public CRGFComponent
{
public:
  CInventory();
  ~CInventory();
  void Display();
private:
	void Blit();
	geBitmap *GetImage(int index);
	int GetAmount(int index);
	int GetCatagory(int index);
	char *GetAttribute(int index);
	char *GetModified(int index);
	int GetModifiedAmount(int index);
	bool GetDecrease(int index);
	char *GetText(int index);
	void GenerateList();
	bool active;
	geBitmap *background;
	geBitmap *highlight;
	int backgroundx, backgroundy;
	int leftx, lefty;
	int middlex, middley;
	int rightx, righty;
	int font;
	int txtfont;
	InvItem *Bottom;
	InvItem *ZBottom;
	int ZMaxItems;
	int Selected;
	int MaxItems;
	geSound_Def *keyclick;
};

#endif
