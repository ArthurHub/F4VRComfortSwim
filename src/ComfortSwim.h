#pragma once

namespace comfort_swim
{
    class ComfortSwim
    {
    public:
        static void init();

    private:
        static void testing();
    };

    // The ONE global to rule them ALL
    inline ComfortSwim g_comfortSwim;
}
