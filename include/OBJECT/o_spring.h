#ifndef _O_SPRING_H_
#define _O_SPRING_H_

#include "sa2b_types.h"
#include "samt/sonic/task.h"

void DrawSpring(task *task);
void DrawSpringGC(task *task);

void DrawSpringB(task *task);
void DrawSpringBGC(task *task);

void ObjectSpringDestruct(task *task);
void ObjectSpringBDestruct(task *task);
void VacumePlayer_0(task*, Uint8 flag);

#endif // !_O_SPRING_H_
