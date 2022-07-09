#include "val.h"
#include "math.h"

val_t mth.sin(val_t arg)
{
  if (valisint(arg)) return val(sin((double)valtoint(arg)));
  if (valisdbl(arg)) return val(sin(valtodbl(arg)));
  return arg;
}

val_t mth.abs(val_t arg)
{
  if (valisint(arg)) return val(abs(valtoint(arg)));
  if (valisdbl(arg)) return val(fabs(valtodbl(arg)));
  return arg;
}
