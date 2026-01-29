#pragma once
#pragma warning(disable: 4251)
#pragma warning(disable: 4172)

//이렇게 활성화도 가능
//#define ENGINE_BUILD_DLL 1 -> 사용하면 1
//#define ENGINE_BUILD_DLL 0 -> 사용 안 하면 0

#define DLLEXPORT __declspec( dllexport )
#define DLLIMPORT __declspec( dllimport )

#if ENGINE_BUILD_DLL
#define NAOMI_API DLLEXPORT
#else
#define NAOMI_API DLLIMPORT
#endif
