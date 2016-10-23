#pragma once

#ifdef DECL_OFFSETS
	#define OFFSET(__name, __offset)		void* Offset_##__name = (void*)##__offset;
	#define OFFSET_STATIC(__name, __offset) int Static_##__name = *(int*)##__offset;
	#define DECL_PTR(__ptr)					__ptr;
#else
	#define OFFSET(__name, __offset)		extern void* Offset_##__name;
	#define OFFSET_STATIC(__name, __offset) extern int Static_##__name;
	#define DECL_PTR(__ptr)					extern __ptr;
#endif

#define GET_OFFSET(__name)				Offset_##__name
#define GET_OFFSET_ORIG(__name)         Orig_##__name
