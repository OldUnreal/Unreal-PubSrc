/*=============================================================================
	DataWrapper.h

	Semi-overrideable data type wrappers, with TMap key support.

	Revision history:
	* Created by Fernando Velazquez
=============================================================================*/

#pragma once 

//
// Basic Data Wrapper with comparison checks
//
template < typename T > class TDataWrapper
{
public:
	typedef T DataType;

	// Constructors
	TDataWrapper();
	TDataWrapper( const T& InValue);
	TDataWrapper( const TDataWrapper& Other);

	// Public accessors
	T&       GetValue();
	const T& GetValue() const;
	void     SetValue( const T& NewValue);
	void     SetValue( const TDataWrapper<T>& Other);

	// Operators
	bool operator==( const TDataWrapper& Other) const;
	bool operator!=( const TDataWrapper& Other) const;


protected:
	T Value;
};

template <typename T> struct TTypeInfo<TDataWrapper<T>> : public TTypeInfoBase<TDataWrapper<T>>
{
public:
	typedef T ConstInitType;
	static T ToInit( const TTypeInfo<TDataWrapper<T>>& In ) { return In.GetValue(); }
	static UBOOL NeedsDestructor() { return 0; }
};


//
// Basic Data wrapper with bitwise operation support
//
template < typename T > class TDataWrapperBitwise : public TDataWrapper<T>
{
public:
	using TDataWrapper<T>::TDataWrapper;

	T operator&( const T& Other) const;
	T operator&( const TDataWrapperBitwise<T>& Other) const;
	TDataWrapperBitwise<T>& operator&=( const T& Other);
	TDataWrapperBitwise<T>& operator&=( const TDataWrapperBitwise<T>& Other);

	T operator|( const T& Other) const;
	T operator|( const TDataWrapperBitwise<T>& Other) const;
	TDataWrapperBitwise<T>& operator|=( const T& Other);
	TDataWrapperBitwise<T>& operator|=( const TDataWrapperBitwise<T>& Other);
};


//
// Basic numeric auto-tag
//
template < typename T > class TNumericTag : public TDataWrapper<T>
{
public:
	TNumericTag();
	TNumericTag(ENoInit);

	void Increment();
	void Reset();
	bool CompareAndSet( const TNumericTag<T>& Other);

	TNumericTag<T>& operator++();
};

template < typename T > class TNumericTagNoInit : public TNumericTag<T>
{
public:
	TNumericTagNoInit();
	TNumericTagNoInit<T>& operator=( const TNumericTag<T>& Other);
};


/*-----------------------------------------------------------------------------
	TDataWrapper.
-----------------------------------------------------------------------------*/

//
// Constructors
//
template<typename T>
inline TDataWrapper<T>::TDataWrapper()
{
}

template<typename T>
inline TDataWrapper<T>::TDataWrapper( const T& InValue)
	: Value(InValue)
{
}

template<typename T>
inline TDataWrapper<T>::TDataWrapper( const TDataWrapper& Other)
	: Value(Other.GetValue())
{
}

//
// Public accessors
//
template<typename T>
inline T& TDataWrapper<T>::GetValue()
{
	return Value;
}

template<typename T>
inline const T& TDataWrapper<T>::GetValue() const
{
	return this->Value;
}

template<typename T>
inline void TDataWrapper<T>::SetValue( const T& NewValue)
{
	this->Value = NewValue;
}

template<typename T>
inline void TDataWrapper<T>::SetValue(const TDataWrapper<T>& Other)
{
	this->Value = Other.Value();
}

//
// Operators
//
template<typename T>
inline bool TDataWrapper<T>::operator==( const TDataWrapper& Other) const
{
	return Value == Other.Value;
}

template<typename T>
inline bool TDataWrapper<T>::operator!=( const TDataWrapper& Other) const
{
	return Value != Other.Value;
}


/*-----------------------------------------------------------------------------
	TDataWrapperBitwise.
-----------------------------------------------------------------------------*/

template<typename T>
inline T TDataWrapperBitwise<T>::operator&( const T& Other) const
{
	return this->GetValue() & Other;
}

template<typename T>
inline T TDataWrapperBitwise<T>::operator&( const TDataWrapperBitwise<T>& Other) const
{
	return this->GetValue() & Other.GetValue();
}

template<typename T>
inline TDataWrapperBitwise<T>& TDataWrapperBitwise<T>::operator&=(const T& Other)
{
	this->GetValue() &= Other;
	return *this;
}

template<typename T>
inline TDataWrapperBitwise<T>& TDataWrapperBitwise<T>::operator&=(const TDataWrapperBitwise<T>& Other)
{
	this->GetValue() &= Other.GetValue();
	return *this;
}

template<typename T>
inline T TDataWrapperBitwise<T>::operator|( const T& Other) const
{
	return this->GetValue() | Other;
}

template<typename T>
inline T TDataWrapperBitwise<T>::operator|( const TDataWrapperBitwise<T>& Other) const
{
	return this->GetValue() | Other.GetValue();
}

template<typename T>
inline TDataWrapperBitwise<T>& TDataWrapperBitwise<T>::operator|=(const T& Other)
{
	this->GetValue() |= Other;
	return *this;
}

template<typename T>
inline TDataWrapperBitwise<T>& TDataWrapperBitwise<T>::operator|=(const TDataWrapperBitwise<T>& Other)
{
	this->GetValue() |= Other.GetValue();
	return *this;
}


/*-----------------------------------------------------------------------------
	TNumericTag.
-----------------------------------------------------------------------------*/

template<typename T>
inline TNumericTag<T>::TNumericTag()
	: TDataWrapper<T>(0)
{
}

template<typename T>
inline TNumericTag<T>::TNumericTag(ENoInit)
	: TDataWrapper<T>()
{
}

template<typename T>
inline void TNumericTag<T>::Increment()
{
	this->GetValue() += 1;
}

template<typename T>
inline void TNumericTag<T>::Reset()
{
	this->GetValue() = 0;
}

template<typename T>
inline bool TNumericTag<T>::CompareAndSet( const TNumericTag<T>& Other)
{
	if ( this->GetValue() != Other.GetValue() )
	{
		this->GetValue() = Other.GetValue();
		return true;
	}
	return false;
}

template<typename T>
inline TNumericTag<T>& TNumericTag<T>::operator++()
{
	this->Increment();
	return *this;
}

template<typename T>
inline TNumericTagNoInit<T>::TNumericTagNoInit()
	: TNumericTag<T>(E_NoInit)
{
}

template<typename T>
inline TNumericTagNoInit<T>& TNumericTagNoInit<T>::operator=( const TNumericTag<T>& Other)
{
	this->GetValue() = Other.GetValue();
	return *this;
}
