//
// Created by jul3x on 22.03.21.
//

#include <R3E/utils/Misc.h>


namespace r3e::utils {
    uint64_t timeSinceEpochMillisec()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
    }
} // namespace r3e::utils
