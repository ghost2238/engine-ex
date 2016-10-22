#pragma once

#ifdef DECL_OFFSETS
	#define OFFSET_ORIG(__name)				FuncPtr Orig_##__name;
	#define OFFSET(__name, __offset)		void* Offset_##__name = (void*)##__offset; OFFSET_ORIG(__name);
	#define OFFSET_STATIC(__name, __offset) int Static_##__name = *(int*)##__offset;
	#define DECL_PTR(__ptr)					__ptr;
#else
	#define OFFSET_ORIG(__name)				extern FuncPtr Orig_##__name;
	#define OFFSET(__name, __offset)		extern void* Offset_##__name; OFFSET_ORIG(__name);
	#define OFFSET_STATIC(__name, __offset) extern int Static_##__name;
	#define DECL_PTR(__ptr)					extern __ptr;
#endif

#define GET_OFFSET(__name)				Offset_##__name
#define GET_OFFSET_ORIG(__name)         Orig_##__name
