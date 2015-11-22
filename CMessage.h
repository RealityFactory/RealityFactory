/*
	CMessage.h:		Message class handler

	(c) 2001 Ralph Deane
	All Rights Reserved

	This file contains the class declaration for Message
handling.
*/

#ifndef __RGF_CMESSAGE_H_
#define __RGF_CMESSAGE_H_

class CMessage : public CRGFComponent
{
public:
  CMessage();
  ~CMessage();
  void Tick(float dwTicks);
  void Display();
  int LocateEntity(char *szName, void **pEntityData);
  int ReSynchronize();
private:

};

#endif

// --------------------------------------------------------------------------
