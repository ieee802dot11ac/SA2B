/*
*   SAMT for Sonic Adventure 2 (PC, 2012) - '/sonic/task/anywk.h'
*
*   Description:
*     Task 'Any Work'.
*/
#ifndef H_SA2B_TASK_ANYWK
#define H_SA2B_TASK_ANYWK

EXTERN_START

/********************************/
/*  Structures                  */
/********************************/
/****** Work ************************************************************************************/
#define TO_ANYWK(p)         ((anywk*)(p))

typedef struct anywk
{
    union
    {
        Uint8     ub[16];           /* unsigned bytes                                           */
        Sint8     sb[16];           /* signed bytes                                             */
        Uint16    uw[8];            /* unsigned words                                           */
        Sint16    sw[8];            /* signed words                                             */
        Uint32    ul[4];            /* unsigned longs                                           */
        Sint32    sl[4];            /* signed longs                                             */
        Float     f[4];             /* real numbers                                             */
        void*     ptr[4];           /* pointers                                                 */
    }
    work;                           /* inline work                                              */
}
anywk;

EXTERN_END

#endif/*H_SA2B_TASK_ANYWK*/
