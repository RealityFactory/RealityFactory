/************************************************************************************//**
 * @file CNetBuffer.h
 * @brief CNetBuffer class declaration
 ****************************************************************************************/

#ifndef __RGF_CNETBUFFER_H_
#define __RGF_CNETBUFFER_H_

/**
 * @brief NetBuffer class
 */
class NetBuffer
{
public:
	// variables
	/**
	 * @brief Current position in the buffer
	 */
	unsigned char *dataptr;
    int Pos;
	/**
	 * @brief Current size of the buffer
	 */
	int Size;
	/**
	 * @brief Starting position of data in the buffer
	 */
	unsigned char *Data;

public:
	// functions
	NetBuffer(int value);
	virtual ~NetBuffer();

	void *Expand(int addsize);
	void *CheckSize(int size);

	int GetInt();
	unsigned char GetChar();
	float GetFloat();
	geVec3d GetVec3d();
	geXForm3d GetXForm3d();
	char *GetString();
	void GetRaw(void *pointer, int len);

	void *Add(int value);
	void *Add(unsigned char value);
	void *AddRaw(void *data, int len);
	void *Add(geFloat value);
	void *Add(geVec3d vect3d);
	void *Add(geXForm3d xform);
	void *AddString(const char *message, int len);
	void *AddLen();
	void PosBack(int len);

protected:
	// variables
	/** @brief Maximum size of the buffer (before expanding) */
	int MaxSize;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
