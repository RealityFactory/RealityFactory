/************************************************************************************//**
 * @file CNetBuffer.cpp
 * @brief CNetBuffer class implementation
 ****************************************************************************************/

#include <stdlib.h>
#include <malloc.h>
#include <memory>
#include <tchar.h>
#include "genesis.h"
#include "CNetBuffer.h"

/* ------------------------------------------------------------------------------------ */
//	Constructor
/* ------------------------------------------------------------------------------------ */
NetBuffer::NetBuffer(int value)
{
	Size = 0;

	if(value == 0)
	{
		Data = (uint8*)malloc(sizeof(uint8) * 1000);
		MaxSize = 1000;
		dataptr = Data;
	}
	else
	{
		Data = (uint8*)malloc(sizeof(uint8) * value);
		MaxSize = value;
		dataptr = Data;
	}

    Pos = 0;
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
/* ------------------------------------------------------------------------------------ */
NetBuffer::~NetBuffer()
{
	if(Data)
		free(Data);

	Data	= NULL;
	dataptr = NULL;
	Size	= 0;
	MaxSize = 0;
    Pos		= 0;
}

/* ------------------------------------------------------------------------------------ */
//	PosBack
/* ------------------------------------------------------------------------------------ */
void NetBuffer::PosBack(int len)
{
	if(len > 0)
	{
		dataptr -= len;
		Pos -= len;
	}
	else
	{
		Size = 0;
		Pos = 0;
		dataptr = Data;
	}
}

/* ------------------------------------------------------------------------------------ */
//	AddLen
/* ------------------------------------------------------------------------------------ */
void *NetBuffer::AddLen()
{
	int value = Size - sizeof(int);
	memcpy(Data, &value, sizeof(int));

	return (void*)Data;
}

/* ------------------------------------------------------------------------------------ */
//	Adds an integer (32bit) value to the buffer
//
//	@param value Integer that you want to add to the buffer
//	@returns A pointer to the data buffer
/* ------------------------------------------------------------------------------------ */
void *NetBuffer::Add(int value)
{
	if((Size + (int32)sizeof(int)) >= MaxSize)
		Expand(1000);

	memcpy(&(Data[Size]), &value, sizeof(int));
	Size += sizeof(int);

	return (void*)Data;
}

/* ------------------------------------------------------------------------------------ */
//	Add
/* ------------------------------------------------------------------------------------ */
void *NetBuffer::Add(unsigned char value)
{
	if((Size + (int32)sizeof(unsigned char)) >= MaxSize)
		Expand(1000);

	memcpy(&(Data[Size]), &value, sizeof(unsigned char));
	Size += sizeof(unsigned char);

	return (void*)Data;
}

/* ------------------------------------------------------------------------------------ */
//	AddRaw
/* ------------------------------------------------------------------------------------ */
void *NetBuffer::AddRaw(void *data, int len)
{
	if((Size + len) >= MaxSize)
		Expand(1000);

	memcpy(&(Data[Size]), data, len);
	Size += len;

	return (void*)Data;
}

/* ------------------------------------------------------------------------------------ */
//	Makes the buffer grow bigger
//
//	@param addsize Size to grow the buffer by
//	@returns A pointer to the data buffer
/* ------------------------------------------------------------------------------------ */
void *NetBuffer::Expand(int addsize)
{
	Data = (uint8*)realloc(Data, MaxSize+(addsize*sizeof(uint8)));

	if (Data)
		MaxSize += addsize;
	else
		MaxSize = 0;

	return (void*)Data;
}

/* ------------------------------------------------------------------------------------ */
//	CheckSize
/* ------------------------------------------------------------------------------------ */
void *NetBuffer::CheckSize(int size)
{
	if(MaxSize > size)
		return (void*)Data;

	Data = (uint8*)realloc(Data, size*sizeof(uint8));

	if(Data)
		MaxSize = size;
	else
		MaxSize = 0;

	return (void*)Data;
}

/* ------------------------------------------------------------------------------------ */
//	Adds an geVect3D value to the buffer
//
//	@param geVect3D The geVect3D that you want to add to the buffer
//	@returns A pointer to the data buffer
/* ------------------------------------------------------------------------------------ */
void *NetBuffer::Add(geVec3d vect3d)
{
	Add(vect3d.X);
	Add(vect3d.Y);
	Add(vect3d.Z);

	return (void*)Data;
}

/* ------------------------------------------------------------------------------------ */
//	Adds an XForm3d value to the buffer
//
//	@param xform The XForm3d that you want to add to the buffer
//	@returns A pointer to the data buffer
/* ------------------------------------------------------------------------------------ */
void* NetBuffer::Add(geXForm3d xform)
{
	Add(xform.AX);
	Add(xform.AY);
	Add(xform.AZ);
	Add(xform.BX);
	Add(xform.BY);
	Add(xform.BZ);
	Add(xform.CX);
	Add(xform.CY);
	Add(xform.CZ);
	Add(xform.Translation.X);
	Add(xform.Translation.Y);
	Add(xform.Translation.Z);

	return (void *)Data;
}

/* ------------------------------------------------------------------------------------ */
//	Adds a float value to the buffer
//
//	@param value The float that you want to add to the buffer
//	@returns A pointer to the data buffer
/* ------------------------------------------------------------------------------------ */
void* NetBuffer::Add(geFloat value)
{
	if((Size + (int32)sizeof(geFloat)) >= MaxSize)
		Expand(1000);

	memcpy(&(Data[Size]), &value, sizeof(geFloat));
	Size += sizeof(geFloat);
	return (void*)Data;
}

/* ------------------------------------------------------------------------------------ */
//	Gets an integer from the buffer
//
//	@returns The integer received from the buffer
/* ------------------------------------------------------------------------------------ */
int NetBuffer::GetInt()
{
	int rv;

    memcpy(&rv, dataptr, sizeof(int));
	dataptr += sizeof(int);
    Pos += sizeof(int);
	return rv;
}

/* ------------------------------------------------------------------------------------ */
//	GetChar
/* ------------------------------------------------------------------------------------ */
unsigned char NetBuffer::GetChar()
{
	unsigned char rv;

    memcpy(&rv, dataptr, sizeof(unsigned char));
	dataptr += sizeof(unsigned char);
    Pos += sizeof(unsigned char);

	return rv;
}

/* ------------------------------------------------------------------------------------ */
//	GetRaw
/* ------------------------------------------------------------------------------------ */
void NetBuffer::GetRaw(void *pointer, int len)
{
    memcpy(pointer, dataptr, len);
	dataptr += len;
    Pos += len;

	return;
}

/* ------------------------------------------------------------------------------------ */
//	Gets a float from the buffer
//
//	@returns The float received from the buffer
/* ------------------------------------------------------------------------------------ */
float NetBuffer::GetFloat()
{
	geFloat rv;

	memcpy(&rv, dataptr, sizeof(geFloat));
	dataptr += sizeof(geFloat);
    Pos += sizeof(geFloat);

	return (float)rv;
}

/* ------------------------------------------------------------------------------------ */
//	Gets a geVec3d from the buffer
//
//	@returns The geVec3d received from the buffer
/* ------------------------------------------------------------------------------------ */
geVec3d NetBuffer::GetVec3d()
{
	geVec3d Vec3d;

	Vec3d.X = (geFloat)GetFloat();
	Vec3d.Y = (geFloat)GetFloat();
	Vec3d.Z = (geFloat)GetFloat();

	return Vec3d;
}

/* ------------------------------------------------------------------------------------ */
//	Gets an XForm3d from the buffer
//
//	@returns The XForm3d received from the buffer
/* ------------------------------------------------------------------------------------ */
geXForm3d NetBuffer::GetXForm3d()
{
	geXForm3d XForm3d;

	XForm3d.AX = (geFloat)GetFloat();
	XForm3d.AY = (geFloat)GetFloat();
	XForm3d.AZ = (geFloat)GetFloat();
	XForm3d.BX = (geFloat)GetFloat();
	XForm3d.BY = (geFloat)GetFloat();
	XForm3d.BZ = (geFloat)GetFloat();
	XForm3d.CX = (geFloat)GetFloat();
	XForm3d.CY = (geFloat)GetFloat();
	XForm3d.CZ = (geFloat)GetFloat();
	XForm3d.Translation.X = (geFloat)GetFloat();
	XForm3d.Translation.Y = (geFloat)GetFloat();
	XForm3d.Translation.Z = (geFloat)GetFloat();

	return XForm3d;
}

/* ------------------------------------------------------------------------------------ */
//	Adds a string value to the buffer
//
//	@param message The string that you want to add to the buffer
//	@param len The string's length that you want to add to the buffer
//	@returns A pointer to the data buffer
/* ------------------------------------------------------------------------------------ */
void* NetBuffer::AddString(char *message, int len)
{
	if((Size + (int32)(len+4)) >= MaxSize)
		Expand(len);

	Add(len);
	memcpy(&(Data[Size]), message, len);
	Size += len;

	return (void*)Data;
}

/* ------------------------------------------------------------------------------------ */
//	Gets a String from the buffer
//
//	NOTE: You must free the returning string when finished with it.
//	@returns The string received from the buffer
/* ------------------------------------------------------------------------------------ */
char * NetBuffer::GetString()
{
	int len;
	char *rv;

	len = GetInt();
	rv=(char*)malloc(sizeof(char) * (len+1));

	memcpy(rv, dataptr, len);
	dataptr += len;
    Pos += len;
	rv[len] = 0;

	return rv;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
