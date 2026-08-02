
#include "OBJECT/o_spring.h"
#include "task.h"


extern int spring_cnkdraw;

void DrawSpring(task *task);
void DrawSpringGC(task *task);
void ObjectSpringDestruct(task *task);
void VacumePlayer_0(task*, Uint8 flag);

extern Bool CheckRangeOut(task *t);
extern void CCL_Init();
extern void CCL_Entry(task *);
extern task *CCL_IsHitPlayer();
extern void GetPlayerRunningSpeed(Sint8, Float *);

extern task *playertp[2];
extern struct playerwk *playerpwp[2];
extern taskwk *playertwp[2];
extern int lbl_13_data_253714[4];

inline float qFabsf(float f) {
  Uint32 *i = (Uint32 *)&f;
  *i &= 0x7FFFFFFF;
  return *(float *)i;
}

inline float fabsf(float f) { return __fabsf(f); }
inline float sqrtf(float f) {}

enum {
  MODE_SPRING_0,
  MODE_SPRING_1,
  MODE_SPRING_2,
  MODE_SPRING_3,
};

#define GetV1(task) (*(Float *)&task->awp)
#define GetV2(task) (*(Float *)&task->mwp)
#define GetV3(task) (*(Float *)&task->fwp)

void ObjectSpringA(task *t) {
  taskwk *twp = t->twp;
  if (CheckRangeOut(t)) {
    return;
  }

  switch (twp->mode) {

  case MODE_SPRING_0: {
    t->dest = ObjectSpringDestruct;
    if (spring_cnkdraw) {
      t->disp = DrawSpring;
    } else {
      t->disp = DrawSpringGC;
    }
    CCL_Init(t, lbl_13_data_253714, 1, ARRAY_COUNT(lbl_13_data_253714));
    twp->cwp->flag |= 0x40;
    twp->scl.x = 180.f;
    twp->scl.z = 0.0f;
    GetV1(t) = 0.5f;
    twp->mode = MODE_SPRING_1;
  } break;

  case MODE_SPRING_1: {
    if (twp->wtimer != 0) {
      twp->wtimer--;
    }
    GetV2(t) = -qFabsf(GetV1(t) * njSin(twp->scl.x * (0x8000 / 180.f)));

    twp->scl.x += twp->scl.z;

    if (twp->scl.x >= 290.f && twp->scl.x < twp->scl.z + 290.f) {
      twp->scl.z = 18.f;
    }

    GetV1(t) = njCos((twp->scl.x - 180.0f) * 0.1f * (0x8000 / 180.0f)) * 0.6f;

    if (GetV1(t) < 0.0f) {
      twp->scl.x = 180.0f;
      twp->scl.z = 0.0f;
    }
    twp->btimer++;
    if ((Uint8)twp->btimer > 2) {
      task *hitPlayer = CCL_IsHitPlayer(t);
      if (hitPlayer != NULL) {
        Uint8 i;
        for (i = 0; i < ARRAY_COUNT(playertp); i++) {
          if (hitPlayer == playertp[i]) {
            GetV2(t) = 0.1f;
            twp->smode = i;
            twp->scl.z = 20.f;
            twp->scl.x = 270.f;
            twp->mode = 2;
            twp->btimer = 0;
            break;
          }
        }
      }
    } else if (CCL_IsHitPlayer(t)) {
      twp->btimer = 0;
    }
    if (twp->mode != 2) {
      CCL_Entry(t);
    }
    if ((Uint8)twp->btimer > 50) {
      twp->btimer = 50;
    }
  } break;

  case MODE_SPRING_2: {
    SE_Call(0x1000, 0, 0, 0);
    twp->btimer = 0;
    VacumePlayer_0(t, 0);
  } break;
  
  case MODE_SPRING_3: {
    DeadOut(t);
  } break;
  } // end switch

}

void VacumePlayer_0(task *t, Uint8 flag) {
  taskwk *twp = t->twp;
  NJS_POINT3 sp28 = {0.0f, 5.0f, 0.0f};
  NJS_POINT3 sp1c;
  NJS_POINT3 sp10;
  taskwk *r29 = playertwp[twp->smode];
  sp28.y = 5.0f + twp->scl.y;
  njPushMatrixEx();
  njUnitMatrix(NULL);
  njRotateZ(NULL, twp->ang.z);
  njRotateX(NULL, twp->ang.x);
  njRotateY(NULL, twp->ang.y);
  njCalcPoint(NULL, &sp28, &sp10);
  njPopMatrixEx();
  sp1c.x = sp10.x + twp->pos.x;
  sp1c.y = sp10.y + twp->pos.y;
  sp1c.z = sp10.z + twp->pos.z;
  if (r29 != NULL) {
    float sp34;
    Sint8 r26 = twp->smode;
    float f28 = twp->pos.x - sp1c.x;
    float f31 = twp->pos.y - sp1c.y;
    float f30 = twp->pos.z - sp1c.z;
    float unused = fabsf(f31);
    float f29 = sqrtf(f28*f28 + f31*f31);
    GetPlayerRunningSpeed(r26, &sp34);
    if ((1.0f > sp34 || GetV2(t) != 0.1f) && 14.0f > f29 && f29 >= 2.0f) {
      if (playertp)
      {

      }
      func();
    }
    (void)sp1c;
  }
}

void ObjectSpringDestruct(task *t) {
  t->mwp = NULL;
  t->fwp = NULL;
  t->awp = NULL;
}

void ObjectSpringBDestruct(task *t) {
  t->mwp = NULL;
  t->fwp = NULL;
  t->awp = NULL;
}

#undef GetV1
#undef GetV2
#undef GetV3
