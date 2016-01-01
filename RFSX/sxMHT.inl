/************************************************************************************//**
 * @file sxMHT.inl
 * @brief Inline template for Method Hashtable
 * @author D.R. Gunter
 *//*
 * Copyright (c) 2008 D.R. Gunter; All rights reserved.
 ****************************************************************************************/

#ifndef _SXMHT_INL_
#define _SXMHT_INL_


namespace RFSX
{
//////////////////////////////////////////////////////////////////////////////////////////
// sxMHT implementation
//////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
inline sxMHT<T>::sxMHT(USize size, void (*pfunc)(void)): MHT(size)
{
	InstallMHT(pfunc);
}

template<typename T>
inline void sxMHT<T>::Add(skString m_name, SX_P_FUNC_IT(p_Func, T))
{
	MHT.insertKeyAndValue(m_name, new (SX_P_FUNC_T(T))(p_Func));
}

template<typename T>
inline bool sxMHT<T>::operator()(T* caller, const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	SX_P_FUNC_IT(*m, T) = 0;
	if(m = MHT.value(m_name))
		return (*(m))(caller, args, r_val, ctxt);
	return false;
}

template<typename T>
inline void sxMHT<T>::InstallMHT(void (*pfunc)(void))
{
	static bool called = false;
	if(called) return;

	inits.AddMHT(pfunc);
	called = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Object type-check and upcast
//////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
inline T* CheckType(skRValue& val, int type)
{
	if(val.type() == skRValue::T_Object)
		if(val.obj()->executableType() == type)
			return (T*)(val.obj());
	return NULL;
}

} // namespace RFSX

#endif _SXMHT_INL_
