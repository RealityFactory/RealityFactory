/************************************************************************************//**
 * @file sxCore.h
 * @brief Core component declarations for RFSX.
 * @author D.R. Gunter
 *//*
 * Copyright (c) 2008 D.R. Gunter; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_CORE_H_
#define _SX_CORE_H_

#include <vector>

namespace RFSX
{

class sxMHT_init
{
public:
	sxMHT_init();
	~sxMHT_init();

	void AddMHT(void (*)(void));
	void execute();

private:
	std::vector<void (*)(void)> MHT_Inits;
};


// Class defining skString-to-function pointer hashtable for Method functions
template <typename T> class sxMHT
{
public:
	sxMHT<T>(USize size, void (*)(void));

protected:
	inline bool operator()(T* caller, const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	inline void Add(skString m_name, SX_P_FUNC_IT(m,T));

private:
	static void InstallMHT(void (*pfunc)(void));

	skTSHashTable<SX_P_FUNC_T(T)> MHT;

	friend T;
};

// Type safety functions
template<typename T> T* CheckType(skRValue& val, int type);
extern bool CheckParams(int pmin, int pmax, skRValueArray& args, skRValue::RType types[]);

void Install(skInterpreter* Interp);
extern sxMHT_init inits;

} // namespace RFSX

#include "sxMHT.inl"

#endif // _SX_CORE_H_
