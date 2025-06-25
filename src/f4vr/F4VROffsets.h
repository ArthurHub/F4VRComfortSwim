#pragma once

namespace f4vr
{
    // not sure why RE::Relocation doesn't work here, so using raw address
    inline auto iniLeftHandedMode = reinterpret_cast<bool*>(REL::Offset(0x37d5e48).address());
}
