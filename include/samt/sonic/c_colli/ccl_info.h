/*
*   SAMT for Sonic Adventure 2 (PC, 2012) - '/sonic/c_colli/ccl_info.h'
*
*   Description:
*       Definition for the CCL_INFO struct type.
*/
#ifndef _SA2B_CCOLLI_INFO_H_
#define _SA2B_CCOLLI_INFO_H_

/************************/
/*  Includes            */
/************************/
#include <samt/ninja/njcommon.h>

/************************/
/*  Structures          */
/************************/
typedef struct
{
    Sint8      kind;
    Uint8      form;
    Sint8      push;
    Sint8      damage;
    Uint32     attr;
    NJS_POINT3 center;
    Float        a;
    Float        b;
    Float        c;
    Float        d;
    Sint32       angx;
    Sint32       angy;
    Sint32       angz;
}
CCL_INFO;

#endif/*_SA2B_CCOLLI_INFO_H_*/
