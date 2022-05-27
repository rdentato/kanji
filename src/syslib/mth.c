#include "val.h"
#include "math.h"

val_t mth.sin(val_t arg)
{
  if (valisdbl(arg)) return val(sin(valtodbl(arg)));
  return valnil;
}
