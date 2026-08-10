#include "stg13_cityescape/o_ce_board_col.h"
#include "samt/sonic/player.h"
#include "set.h"

// ^ extern
// v in this file


void ObjectBoardColDest(task *tp) { return; }

void ObjectBoardColExec(task *tp) {
  int PAD_STACK[4];
  taskwk *twp = tp->twp;
  if (!CheckRangeOut(tp)) {
    // not a for loop, but it really looks like it
    int i = 0;
    taskwk *player = playertwp[i];
    if (player && njDistanceP2P(&player->pos, &twp->pos) < twp->scl.x + 10.f &&
        (twp->mode & (1 << i)) == 0) {
      if (twp->ang.x & 1) {
        twp->mode |= (1 << i);
        SetInputP(i, 0x22, 0);
      } else {
        twp->mode |= (1 << i);
        SetInputP(i, 0xf, 0);
      }
    }
    i++;
    player = playertwp[i];
    if (player && njDistanceP2P(&player->pos, &twp->pos) < twp->scl.x + 10.f &&
        (twp->mode & (1 << i)) == 0) {
      if (twp->ang.x & 1) {
        twp->mode |= (1 << i);
        SetInputP(i, 0x22, 0);
      } else {
        twp->mode |= (1 << i);
        SetInputP(i, 0xf, 0);
      }
    }

    if (lbl_801CC168.TWO_PLAYER) {
      Sint32 i;
      for (i = 0; i < 2; i++) {
        player = playertwp[i];
        if (player) {
          if (player->mode >= 0x4c && player->mode <= 0x52) {
            if (twp->ang.x & 1 && twp->mode & (1 << i)) {
              twp->mode &= ~(1 << i);
            }
          } else {
            if ((twp->ang.x & 1) == 0 && twp->mode & (1 << i)) {
              twp->mode &= ~(1 << i);
            }
          }
        }
      }
    }
  }
}

void ObjectBoardCol(task *tp) {
  if (!CheckRangeOut(tp)) {
    tp->exec = ObjectBoardColExec;
    tp->dest = ObjectBoardColDest;
  }
}
