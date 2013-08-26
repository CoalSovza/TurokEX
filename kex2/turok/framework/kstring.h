// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// Copyright(C) 2013 Samuel Villarreal
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
//
//-----------------------------------------------------------------------------

#ifndef __KSTRING_H__
#define __KSTRING_H__

#include <string.h>
#include "common.h"
#include "keywds.h"
#include "array.h"

class kexStr {
public:
                        kexStr(void);
                        kexStr(const char *string);
                        kexStr(const kexStr &string);
                        ~kexStr(void);

    void                CheckSize(int size, bool bKeepString);
    int                 IndexOf(const char *pattern) const;
    kexStr              &Concat(const char *string);
    kexStr              &Concat(const char *string, int len);
    kexStr              &NormalizeSlashes(void);
    kexStr              &StripPath(void);
    kexStr              &StripExtension(void);
    kexStr              &Copy(const kexStr &src, int len);
    kexStr              &Copy(const kexStr &src);
    kexStr              &ToUpper(void);
    kexStr              &ToLower(void);
    int                 Hash(void);
    kexStr              Substr(int start, int len) const;
    int                 Length(void) const { return length; }
    const char          *c_str(void) const { return charPtr; }

    kexStr              &operator=(const kexStr &str);
    kexStr              &operator=(const char *str);
    kexStr              &operator=(const bool b);
    kexStr              operator+(const kexStr &str);
    kexStr              operator+(const char *str);
    kexStr              operator+(const bool b);
    kexStr              &operator+=(const kexStr &str);
    kexStr              &operator+=(const char *str);
    kexStr              &operator+=(const bool b);

    operator            const char *(void) const { return c_str(); }
    operator            const char *(void) { return c_str(); }
    
    static bool         Compare(const kexStr &a, const kexStr &b);
    static int          IndexOf(const char *string, const char *pattern);

private:
    void                Resize(int size, bool bKeepString);
    void                CopyNew(const char *string, int len);

protected:
    void                Init(void);

    static const int    STRING_DEFAULT_SIZE = 32;

    char                *charPtr;
    char                defaultBuffer[STRING_DEFAULT_SIZE];
    int                 length;
    int                 bufferLength;
};

typedef kexArray<kexStr>    kexStrList;
typedef kexArray<kexStr*>   kexStrListMem;

#endif