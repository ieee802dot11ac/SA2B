#ifndef __SA2B_TYPES_H_
#define __SA2B_TYPES_H_

#include "samt/core.h"
#include "samt/ninja/ninja.h"

#define OFFSET_OF(type, field) ((uintptr_t)&(((type*)(NULL))->field))

#define artificial_padding(lastOffset, nextOffset, typeOfLastMember) \
u8 __padding##lastOffset[(nextOffset) - (lastOffset) - sizeof(typeOfLastMember)]

#endif // !__SA2B_TYPES_H_
