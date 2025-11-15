#include "cradle.hpp"
#include <windows.h>

namespace cradle
{

    static bool isRunning = false;

    bool initialize()
    {
        isRunning = true;
        return true;
    }

    void run()
    {
        while (isRunning)
        {
            Sleep(100);
        }
    }

    void cleanup()
    {
        isRunning = false;
    }

}
