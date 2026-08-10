#include "stg13_cityescape/o_ce_board.h"
#include "samt/sonic/player.h"
#include "samt/ninja/njdraw.h"

extern NJS_TEXLIST lbl_13_data_105088;
extern NJS_OBJECT lbl_13_data_1060EC;
extern NJS_OBJECT lbl_13_data_1057C0;

extern NJS_TEXLIST lbl_13_data_107268;
extern NJS_OBJECT lbl_13_data_107F10;
extern NJS_OBJECT lbl_13_data_10B56C;

extern NJS_TEXLIST lbl_13_data_107F64;
extern NJS_OBJECT lbl_13_data_108C1C;
extern NJS_OBJECT lbl_13_data_10C3EC;

extern NJS_TEXLIST lbl_13_data_106588;
extern NJS_OBJECT lbl_13_data_107214;
extern NJS_OBJECT lbl_13_data_10A72C;

extern NJS_TEXLIST lbl_13_data_108C70;
extern NJS_OBJECT lbl_13_data_109918;
extern NJS_OBJECT lbl_13_data_10D22C;

extern void *lbl_13_data_106534;

typedef struct {
  int _0;
  int _4;
} lbl_801E5D28_arr;
struct {
  u8 pad[4];
  lbl_801E5D28_arr _4[300];
} lbl_801E5D28;

typedef struct{
    u32 _0;
    u32 _4;
    float _8;
    float _C;
} fn_8011E19C_p2;

void fn_8011E19C(NJS_OBJECT *, fn_8011E19C_p2 *, Float);

// ^ extern
// v in this file

NJS_MATRIX lbl_13_bss_0;

static void o_ce_board_1(Sint32 s, NJS_TEXLIST **r4, NJS_OBJECT **r5,
                  NJS_OBJECT **r6) {
  if (!lbl_801CC168.TWO_PLAYER) {
    *r4 = &lbl_13_data_105088;
    *r6 = &lbl_13_data_1060EC;
    *r5 = &lbl_13_data_1057C0;
  } else {
    playerwk *pwp = playerpwp[s];
    switch (pwp->character) {
    case PLNO_AMY:
      *r4 = &lbl_13_data_107268;
      *r5 = &lbl_13_data_107F10;
      *r6 = &lbl_13_data_10B56C;
      break;
    case PLNO_METAL_SONIC:
      *r4 = &lbl_13_data_107F64;
      *r5 = &lbl_13_data_108C1C;
      *r6 = &lbl_13_data_10C3EC;
      break;
    case PLNO_SHADOW:
      *r4 = &lbl_13_data_106588;
      *r5 = &lbl_13_data_107214;
      *r6 = &lbl_13_data_10A72C;
      break;
    default:
      *r4 = &lbl_13_data_108C70;
      *r5 = &lbl_13_data_109918;
      *r6 = &lbl_13_data_10D22C;
      break;
    }
  }
}

void ObjectBoard(task *tp) {
  taskwk *twp = tp->twp;
  twp->smode = tp->twp->ang.x & 1;
  tp->work.ptr = (void *)syCalloc(1, 0x58);
  if (tp->work.ptr) {
    fn_13_189EC(tp->work.ptr, 0x40);
    tp->exec = ObjectBoardExec;
    tp->dest = ObjectBoardDest;
    tp->disp = ObjectBoardDisp;
    tp->disp_shad = ObjectBoardDispShad;
  }
}

void ObjectBoardExec(task *tp) {
    taskwk* twp = tp->twp;
    motionwk* mwp = tp->mwp;
    
    taskwk* p_twp = playertwp[twp->smode];
    motionwk* p_mwp = playermwp[twp->smode];
    playerwk *p_pwp = playerpwp[twp->smode];
    if (p_twp) {
      if (p_pwp->m.action < 0x79 || p_pwp->m.action > 0x90) {
        if (twp->mode == 1) {
          twp->mode = 2;
        }
        if (twp->mode == 2) {
            fn_800DE3F8(tp);
            CheckRangeOut(tp);
        }
      } else {
        twp->pos = p_twp->pos;
        twp->ang.x = p_twp->ang.x;
        twp->ang.y = 0x8000 - p_twp->ang.y;
        twp->ang.z = p_twp->ang.z;
        mwp->spd = p_mwp->spd;
        twp->mode = 1;
        if (p_pwp->m.mtnmode > 2) {
            mwp->ang_aim.x = p_pwp->m.action;
            *(f32*)&mwp->ang_aim.y = p_pwp->m.nframe;
        }
      }
    }
}

void o_ce_board_4() { njGetMatrix(&lbl_13_bss_0); }

void ObjectBoardDisp(task *tp) {
  taskwk *twp = tp->twp;
  motionwk *mwp = tp->mwp;
  playerwk *p_pwp = playerpwp[twp->smode];
  taskwk *p_twp = playertwp[twp->smode];
  if (p_twp) {
    NJS_OBJECT *sp34;
    NJS_TEXLIST *sp30;
    NJS_OBJECT *sp2C;
    u32 r27;
    o_ce_board_1(twp->smode, &sp30, &sp34, &sp2C);
    r27 = p_pwp->m.action;
    fn_8011C3A0(o_ce_board_4);
    njSetTexture(sp30);
    njPushMatrixEx();
    if (twp->mode != 1) {
      njTranslateEx(&twp->pos);
      njRotateZ(NULL, twp->ang.z);
      njRotateX(NULL, twp->ang.x);
      njRotateY(NULL, twp->ang.y);
      fn_8011DF44(sp34);
      njRotateEx(&sp34->ang, 0);
      njGetMatrix(&lbl_13_bss_0);
    } else {
      int r26 = mwp->ang_aim.x + 24;
      r27 = lbl_801E5D28._4[r27+0x18]._0;
      if (r27) {
        njTranslateEx(&twp->pos);
        njRotateZ(NULL, twp->ang.z);
        njRotateX(NULL, twp->ang.x);
        njRotateY(NULL, twp->ang.y);
        if (p_pwp->m.mtnmode != 2) {
          fn_8011E214(sp34, r27, p_pwp->m.nframe);
        } else if (r26 >= 0x91 && r26 <= 0xa8) {
          fn_8011E19C_p2 sp14;
          sp14._0 = lbl_801E5D28._4[r26]._0;
          sp14._4 = r27;
          sp14._8 = *(f32 *)&mwp->ang_aim.y;
          sp14._C = 0.0f;
          fn_8011E19C(sp34, &sp14, p_pwp->m.nframe);
        } else {
          fn_8011DF44(sp34);
          njRotateEx(&sp34->ang, 0);
          njGetMatrix(&lbl_13_bss_0);
        }
      }
    }
    fn_8002B2E8(0x2400);
    njSetMatrix(NULL, &lbl_13_bss_0);
    fn_8011E244(&lbl_13_data_106534);
    fn_8002B2D8(0x2400);
    njPopMatrixEx();
    fn_8011C3A0(0);
  }
}

void ObjectBoardDispShad(task *tp) {}

void ObjectBoardDest(task *tp) {
    fn_13_18AA8(tp->work.ptr);
    Free(tp->work.ptr);
    tp->work.ptr = NULL;
}
