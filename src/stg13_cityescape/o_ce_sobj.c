#include "stg13_cityescape/o_ce_sobj.h"
#include "sa2b_types.h"
#include "samt/ninja/njmatrix.h"
#include "samt/shinobi/sg_maloc.h"
#include "samt/sonic/c_colli.h"
#include "samt/sonic/game.h"
#include "samt/sonic/player.h"
#include "samt/sonic/task.h"
#include "set.h"

extern u8 fn_80065388(task *tp);
extern void fn_8006539C(task *tp, u8 smode);
extern f32 fabsf(f32);
extern NJS_POINT3 lbl_13_data_141294;

// ^ extern
// v in this file

void ObjectCesobj(task *tp) {
  taskwk *twp = tp->twp;
  tp->exec = ObjectCesobjExec;
  tp->dest = ObjectCesobjDest;
  twp->ang.x &= 0xFF;
  // i think mwp is being used as a loose ptr to some struct here

  CCL_Init(tp, NULL, 0, CID_OBJECT);

  tp->work.ptr = syMalloc(sizeof(NJS_MATRIX));
  if (tp->work.ptr) {
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&twp->pos);
    njRotateY(NULL, twp->ang.y);
    njGetMatrix(tp->work.ptr);
    njPopMatrixEx();
  }
  if (tp->ocp) {
    twp->smode = fn_80065388(tp);
  }
}

void ObjectCesobjDest(task *tp) {
  taskwk *twp = tp->twp;
  if (tp->work.ptr) {
    syFree(tp->work.ptr);
    tp->work.ptr = NULL;
  }
  tp->mwp = NULL;
  tp->awp = NULL;
  tp->fwp = NULL;
  if (tp->ocp) {
    fn_8006539C(tp, twp->smode);
  }
}

void ObjectCesobjExec(task *tp) {
  taskwk *twp = tp->twp;
  if (!CheckRangeOut(tp)) {
    // if
    // elif twp->0x2c {CCL_Entry(tp);}
    twp->mode = 1;
    if (playertwp[0]) {
      NJS_POINT3 xfmed_pos;
      njPushMatrixEx();
      njUnitMatrix(NULL);
      njRotateY(NULL, -twp->ang.y);
      njTranslate(NULL, -twp->pos.x, -twp->pos.y, -twp->pos.z);
      njCalcPoint(NULL, &playertwp[0]->pos, &xfmed_pos);
      njPopMatrixEx();
      if (fabsf(xfmed_pos.x) < lbl_13_data_141294.x &&
          fabsf(xfmed_pos.z) < lbl_13_data_141294.z &&
          fabsf(xfmed_pos.y) < lbl_13_data_141294.y) {
        twp->mode = 0;
      }
    }
  }
}
