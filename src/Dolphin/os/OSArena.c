#include "Dolphin/OS/OSArena.h"

static void* __OSArenaHi = (void*)0x00000000;
static void* __OSArenaLo = (void*)0xffffffff;

#define ROUND(n, a) (((u32)(n) + (a)-1) & ~((a)-1))
#define TRUNC(n, a) (((u32)(n)) & ~((a)-1))
/**
 * @TODO: Documentation
 */
void* OSGetArenaHi(void)
{
	return __OSArenaHi;
}

/**
 * @TODO: Documentation
 */
void* OSGetArenaLo(void)
{
	return __OSArenaLo;
}

/**
 * @TODO: Documentation
 */
void OSSetArenaHi(void* arena_hi)
{
	__OSArenaHi = arena_hi;
	return;
}

/**
 * @TODO: Documentation
 */
void OSSetArenaLo(void* arena_lo)
{
	__OSArenaLo = arena_lo;
	return;
}


void* OSAllocFromArenaLo(u32 size, u32 align) {
  void* ptr;
  u8* arenaLo;

  ptr = OSGetArenaLo();
  arenaLo = ptr = (void*)ROUND(ptr, align);
  arenaLo += size;
  arenaLo = (u8*)ROUND(arenaLo, align);
  OSSetArenaLo(arenaLo);
  return ptr;
}

void* OSAllocFromArenaHi(u32 size, u32 align) {
  void* ptr;
  u8* arenaHi;

  arenaHi = OSGetArenaHi();
  arenaHi = (u8*)TRUNC(arenaHi, align);
  arenaHi -= size;
  arenaHi = ptr = (void*)TRUNC(arenaHi, align);
  OSSetArenaHi(arenaHi);
  return ptr;
}
