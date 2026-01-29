#pragma once
#include "Common/Common.h"

namespace Wanted
{
	// 커스텀 RTTI를 제공하는 클래스의 최상위 클래스
	class NAOMI_API RTTI
	{
	public:
		virtual const size_t& GetType() const = 0;

		virtual bool Is(RTTI* const rtti) const
		{
			return false;
		}

		virtual bool Is(const size_t id) const
		{
			return false;
		}

		template<typename T>
		T* As()
		{
			if (Is(T::TypeIdClass()))
			{
				return (T*)this;
			}

			return nullptr;
		}

		template<typename T>
		const T* As() const
		{
			if (Is(T::TypeIdClass()))
			{
				return (T*)this;
			}

			return nullptr;
		}
	};

}

// RTTI를 선언할 클래스에 추가할 매크로.
// 아래 코드에서 Type, ParentType이 실제 타입으로 변환되어 복사/붙여넣기 됨.
#define RTTI_DECLARATIONS(Type, ParentType)												\
friend class RTTI;																		\
protected:																				\
	static const size_t TypeIdClass()													\
	{																					\
		static int runTimeTypeId = 0;													\
		return reinterpret_cast<size_t>(&runTimeTypeId);								\
	}																					\
public:																					\
	virtual const size_t& GetType() const override { return Type::TypeIdClass(); }		\
	using super = ParentType;															\
	virtual bool Is(const size_t id) const override										\
	{																					\
		if (id == TypeIdClass())														\
		{																				\
			return true;																\
		}																				\
		else																			\
		{																				\
			return ParentType::Is(id);													\
		}																				\
	}																					\
	virtual bool Is(RTTI* const rtti) const override									\
	{																					\
		return Is(rtti->GetType());														\
	}