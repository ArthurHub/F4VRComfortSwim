#pragma once

namespace comfort_swim
{
    class ComfortSwim
    {
    public:
        static void init();
        static void onFrameUpdate();

    private:
    };

    // The ONE global to rule them ALL
    inline ComfortSwim g_comfortSwim;
}
