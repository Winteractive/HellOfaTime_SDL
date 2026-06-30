#include "entity.h"

bool IsMoving(Entity* e){
    return e->x != e->x_prev || e->y != e->y_prev;
}
