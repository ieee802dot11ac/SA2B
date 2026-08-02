
#include "OBJECT/o_spring.h"
#include "samt/sonic/c_colli.h"
#include "samt/sonic/player.h"

extern int spring_cnkdraw;

extern Bool CheckRangeOut(task *t);
extern task *CCL_IsHitPlayer();
extern void GetPlayerRunningSpeed(Sint8, Float *);

extern void SetInputP(Sint8, int, int);
extern void SetSpringVelocityP(Uint8, Float, Float, Float);
extern void SetVelocityYAndRotationAndNoconTimeP(Uint8, Float, Sint32*, Sint32);
extern void fn_8002FB2C(Sint8, int, int, int); 
extern void fn_13_390DC(Sint8, int); 

inline float qFabsf(float f) {
  Uint32 *_f = (Uint32 *)&f;
  *_f &= 0x7FFFFFFF;
  return f;
}

inline float fabsf(float f) { return __fabsf(f); }
inline float sqrtf(float f) {}

// ^ extern
// v in this file

extern NJS_TEXLIST lbl_13_data_2510DC;
extern NJS_OBJECT  lbl_13_data_2513F4;
extern NJS_OBJECT  lbl_13_data_2516C0;
extern NJS_OBJECT  lbl_13_data_251D44;
extern NJS_OBJECT  lbl_13_data_251A78;
 
extern GJS_OBJECT  lbl_13_data_252F44;
extern GJS_OBJECT  lbl_13_data_2521A4;
extern GJS_OBJECT  lbl_13_data_252554;
extern GJS_MODEL   lbl_13_data_252A70;
 
extern GJS_OBJECT  lbl_13_data_2532F4;
extern GJS_MODEL   lbl_13_data_2536F0;
extern CCL_INFO    lbl_13_data_253714[4];
extern CCL_INFO    lbl_13_data_253744[4];
extern NJS_MODEL*  lbl_13_data_253774[][4];

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
        for (i = 0; i < 2; i++) {
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
  Float stack_pad[2];
  Sint8 smode_;
  taskwk *twp = t->twp;
  Float runningSpeed;
  NJS_POINT3 inPoint = {0.0f, 5.0f, 0.0f};
  NJS_POINT3 offset;
  NJS_POINT3 outPoint;
  taskwk *player = playertwp[twp->smode];
  
  inPoint.y = 5.0f + twp->scl.y;
  njPushMatrixEx();
  njUnitMatrix(NULL);
  njRotateZ(NULL, twp->ang.z);
  njRotateX(NULL, twp->ang.x);
  njRotateY(NULL, twp->ang.y);
  njCalcPoint(NULL, &inPoint, &outPoint);
  njPopMatrixEx();
  offset.x = twp->pos.x + outPoint.x;
  offset.y = twp->pos.y + outPoint.y;
  offset.z = twp->pos.z + outPoint.z;
  if (player != NULL) {
    Sint8 smode = twp->smode;
    float diffX = offset.x - player->pos.x;
    float diffY = offset.y - player->pos.y;
    float diffZ = offset.z - player->pos.z;
    float distance;
    diffY = fabsf(diffY); // unused?
    distance = sqrtf(diffX * diffX + diffZ * diffZ);
    smode_ = smode;
    GetPlayerRunningSpeed(smode_, &runningSpeed);
    if (1.0f > runningSpeed || GetV2(t) != 0.1f) {
      if (14.0f > distance && distance >= 2.0f) {
        if ((playerpwp[twp->smode]->item & 0x4000) == 0) {
          SetInputP(smode_, 9, 0);
          player->pos.x += 2.0f * (diffX / distance);
          player->pos.z += 2.0f * (diffZ / distance);
          SetInputP(smode_, 15, 0);
        }
        if (flag == 0) {
          SetSpringVelocityP(smode_, outPoint.x, outPoint.y, outPoint.z);
        } else {
          player->pos.x = offset.x;
          player->pos.y = twp->pos.y + outPoint.y * 0.8f;
          player->pos.z = offset.z;
          if (twp->scl.x <= 0.0f) {
            twp->scl.x = 0.0f;
          }
          SetVelocityYAndRotationAndNoconTimeP(smode_, twp->scl.y + 5.0f,
                                               &twp->ang.x, (Sint32)twp->scl.x);
        }
        twp->mode = MODE_SPRING_1;
      } else {
        if (flag == 0) {
          SetSpringVelocityP(smode_, outPoint.x, outPoint.y, outPoint.z);
        } else {
          player->pos.x = offset.x;
          player->pos.y = twp->pos.y + outPoint.y * 0.8f;
          player->pos.z = offset.z;
          if (twp->scl.x <= 0.f) {
            twp->scl.x = 0;
          }
          SetVelocityYAndRotationAndNoconTimeP(smode_, twp->scl.y + 5.0f,
                                               &twp->ang.x, (Sint32)twp->scl.x);
        }
        twp->mode = MODE_SPRING_1;
      }
    } else {
      if ((playerpwp[twp->smode]->item & 0x4000) == 0) {
        SetInputP(smode_, 9, 0);
        player->pos = offset;
        SetInputP(smode_, 15, 0);
      }

      if (flag == 0) {
        SetSpringVelocityP(smode_, outPoint.x, outPoint.y, outPoint.z);
      } else {
        if (twp->scl.x <= 0.f) {
          twp->scl.x = 0.f;
        }
        SetVelocityYAndRotationAndNoconTimeP(smode_, twp->scl.y + 5.0f,
                                             &twp->ang.x, (Sint32)twp->scl.x);
      }
      twp->mode = MODE_SPRING_1;
    }
    GetV2(t) = 0.1f;
    fn_8002FB2C(smode_, 4, 15, 0);
    fn_13_390DC(smode_, 1);
  }
}

void ObjectSpringDestruct(task *t) {
  t->mwp = NULL;
  t->fwp = NULL;
  t->awp = NULL;
}

void DrawSpringGC(task *t) {
  taskwk *twp = t->twp;
  GJS_MODEL* model;
  njSetTexture(&lbl_13_data_2510DC);
  njPushMatrixEx();
  njTranslateEx(&twp->pos);
  njRotateZ(NULL, twp->ang.z);
  njRotateX(NULL, twp->ang.x);
  njRotateY(NULL, twp->ang.y);
  if (twp->wtimer != 0) {
    Float f = 1.f / (Float)twp->wtimer;
    njScale(NULL, f, f, f);
  }
  model = &lbl_13_data_252A70;
  gjDrawModel(model);
  njTranslateEx(&lbl_13_data_252554.pos);
  njScale(NULL, 0.98f, 1.0f + GetV2(t), 0.98f);
  gjDrawModel(lbl_13_data_252554.model);
  njTranslateEx(&lbl_13_data_2521A4.pos);
  njTranslate(NULL, 0.0f, 4.f * GetV2(t), 0.0f);
  gjDrawModel(lbl_13_data_2521A4.model);
  njPopMatrixEx();
}

void DrawSpring(task *t) {
  taskwk *twp = t->twp;
  njSetTexture(&lbl_13_data_2510DC);
  njPushMatrixEx();
  njTranslateEx(&twp->pos);
  njRotateZ(NULL, twp->ang.z);
  njRotateX(NULL, twp->ang.x);
  njRotateY(NULL, twp->ang.y);
  if (twp->wtimer != 0) {
    Float f = 1.f / (Float)twp->wtimer;
    njScale(NULL, f, f, f);
  }
  _rename_dsDrawModelClip(lbl_13_data_253774[0][3]); // model_bane_banebottom_banebottom
  njTranslateEx(&lbl_13_data_2516C0.pos);
  njScale(NULL, 0.98f, 1.0f + GetV2(t), 0.98f);
  _rename_dsDrawModelClip(lbl_13_data_253774[1][3]); // model_bane_banebottom_spring
  njTranslateEx(&lbl_13_data_2513F4.pos);
  njTranslate(NULL, 0.0f, 4.f * GetV2(t), 0.0f);
  _rename_dsDrawModelClip(lbl_13_data_253774[2][3]); // model_bane_banebottom_banehead
  njPopMatrixEx();
}

void ObjectSpringB(task *t) {
  taskwk *twp = t->twp;
  if (CheckRangeOut(t)) {
    return;
  }

  switch (twp->mode) {

  case MODE_SPRING_0: {
    t->dest = ObjectSpringBDestruct;
    if (spring_cnkdraw) {
      t->disp = DrawSpringB;
    } else {
      t->disp = DrawSpringBGC;
    }
    CCL_Init(t, lbl_13_data_253744, 1, ARRAY_COUNT(lbl_13_data_253744));
    twp->cwp->flag |= 0x40;
    GetV3(t) = 180.f;
    twp->scl.z = 0.0f;
    GetV1(t) = 0.5f;
    twp->btimer = 0;
    twp->mode = MODE_SPRING_1;
  } break;

  case MODE_SPRING_1: {
    if (twp->wtimer != 0) {
      twp->wtimer--;
    }
    GetV2(t) = -qFabsf(GetV1(t) * njSin(GetV3(t) * (0x8000 / 180.f)));

    GetV3(t) += twp->scl.z;

    if (GetV3(t) >= 290.f && GetV3(t) < twp->scl.z + 290.f) {
      twp->scl.z = 15.f;
    }

    GetV1(t) = njCos((GetV3(t) - 180.0f) * 0.1f * (0x8000 / 180.0f)) * 0.6f;

    if (GetV1(t) < 0.0f) {
      GetV3(t) = 180.0f;
      twp->scl.z = 0.0f;
    }
    twp->btimer++;
    if ((Uint8)twp->btimer > 2) {
      task *hitPlayer = CCL_IsHitPlayer(t);
      if (hitPlayer != NULL) {
        Uint8 i;
        for (i = 0; i < 2; i++) {
          if (hitPlayer == playertp[i]) {
            GetV2(t) = 0.1f;
            twp->smode = i;
            twp->scl.z = 20.f;
            GetV3(t) = 270.f;
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
    twp->btimer = 0;
    VacumePlayer_0(t, 1);
    SE_Call(0x1000, 0, 0, 0);
  } break;
  
  case MODE_SPRING_3: {
    DeadOut(t);
  } break;
  } // end switch
}


void DrawSpringBGC(task *t) {
  NJS_POINT3 STACK_PAD;
  taskwk *twp = t->twp;
  GJS_MODEL* model;
  njSetTexture(&lbl_13_data_2510DC);
  njPushMatrixEx();
  njTranslateEx(&twp->pos);
  njRotateZ(NULL, twp->ang.z);
  njRotateX(NULL, twp->ang.x);
  njRotateY(NULL, twp->ang.y);
  if (twp->wtimer != 0) {
    Float f = 1.f / (Float)twp->wtimer;
    njScale(NULL, f, f, f);
  }
  gjDrawModel(&lbl_13_data_2536F0);
  njPushMatrixEx();
  njTranslateEx(&lbl_13_data_2532F4.pos);
  njPushMatrixEx();
  njTranslateEx(&lbl_13_data_252F44.pos);
  njTranslate(NULL, 0.0f, 4.f * GetV2(t), 0.0f);
  gjDrawModel(lbl_13_data_252F44.model);
  njPopMatrixEx();
  njScale(NULL, 0.98f, 1.0f + GetV2(t), 0.98f);
  gjDrawModel(lbl_13_data_2532F4.model);
  njPopMatrix(2);
}

void DrawSpringB(task *t) {
  float STACK_PAD[4];
  taskwk *twp = t->twp;
  njSetTexture(&lbl_13_data_2510DC);
  njPushMatrixEx();
  njTranslateEx(&twp->pos);
  njRotateZ(NULL, twp->ang.z);
  njRotateX(NULL, twp->ang.x);
  njRotateY(NULL, twp->ang.y);
  if (twp->wtimer != 0) {
    Float f = 1.f / (Float)twp->wtimer;
    njScale(NULL, f, f, f);
  }
  _rename_dsDrawModelClip(lbl_13_data_253774[3][3]);  // model_bane_type_b_type_b_type_b
  njPushMatrixEx();
  njTranslateEx(&lbl_13_data_251D44.pos);
  njPushMatrixEx();
  njTranslateEx(&lbl_13_data_251A78.pos);
  njTranslate(NULL, 0.0f, 4.f * GetV2(t), 0.0f); // model_bane_type_b_type_b_banehead
  _rename_dsDrawModelClip(lbl_13_data_253774[5][3]);
  njPopMatrixEx();
  njScale(NULL, 0.98f, 1.0f + GetV2(t), 0.98f);
  _rename_dsDrawModelClip(lbl_13_data_253774[4][3]); // model_bane_type_b_type_b_spring
  njPopMatrix(2);
}

void ObjectSpringBDestruct(task *t) {
  t->mwp = NULL;
  t->fwp = NULL;
  t->awp = NULL;
}

#undef GetV1
#undef GetV2
#undef GetV3
