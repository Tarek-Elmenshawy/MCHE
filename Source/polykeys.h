/**
 * @file polykeys.h
 * This file reslates to polyglot book format.
 * Refrence: http://hgm.nubati.net/book_format.html
 * 
 * @author Tarek Elmenshawy (tarek.elmenshawy1997@gmail.com)
 * Date: April 2020
 */

#ifndef POLYKEYS_H
#define POLYKEYS_H

#ifdef _MSC_VER
	#define U64_POLY(u) (u##ui64)		///< ui64 suffix to make an integer constant of type u32 long s32.
#else
	#define U64_POLY(u) (u##ULL)		///< ULL suffix to make an integer constant of type u32 long s32.
#endif

extern const u64 Random64Ploy[781];

#endif