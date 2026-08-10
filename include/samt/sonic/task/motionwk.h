/*
*   SAMT for Sonic Adventure 2 (PC, 2012) - '/sonic/task/motionwk.h'
*
*   Description:
*     Task 'Motion Work'.
*/
#ifndef H_SA2B_TASK_MOTIONWK
#define H_SA2B_TASK_MOTIONWK

/********************************/
/*  Includes                    */
/********************************/
/****** Ninja ***********************************************************************************/
#include <samt/ninja/njcommon.h>    /* ninja common                                             */

EXTERN_START

/********************************/
/*  Structures                  */
/********************************/
/****** Work Macro ******************************************************************************/
#define MOTIONWK struct { \
    NJS_VECTOR  spd;                /* 0x00 speed                                                    */ \
    NJS_VECTOR  acc;                /* 0x0C acceleration                                             */ \
    NJS_ANGLE3  ang_aim;            /* 0x18 angle aim                                                */ \
    NJS_ANGLE3  ang_spd;            /* 0x24 angle speed                                              */ \
    f32         force;              /* 0x30 force                                                    */ \
    f32         accel;              /* 0x34 forward acceleration                                     */ \
    f32         frict;              /* 0x38 friction                                                 */ \
                          \
    union {               \
        s8      b[4];               /* bytes                                                    */ \
        s16     w[2];               /* words                                                    */ \
        s32     l;                  /* long                                                     */ \
        f32     f;                  /* real                                                     */ \
        void*   ptr;                /* pointer                                                  */ \
    }                     \
    work;                           /* 0x3C inline work                                              */ \
}

/****** Work ************************************************************************************/
#define TO_MOTIONWK(p)              ((motionwk*)(p))

typedef struct motionwk
{
    MOTIONWK;
}
motionwk;

EXTERN_END

#endif/*H_SA2B_TASK_MOTIONWK*/
