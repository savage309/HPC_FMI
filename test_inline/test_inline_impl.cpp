
#include "test_inline_impl.h"
#include <cmath>
namespace Inline {
    float calcNoInline(float f) {
        if (f > 0)
            return .5f;
        else return cos(sin(atan(f)));

    }

}