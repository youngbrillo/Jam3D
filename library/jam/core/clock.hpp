#pragma once

#include <stdint.h>

namespace jam
{
    enum TIME_STATE {
        TIME_STATE_PAUSED,
        TIME_STATE_PLAY,
        TIME_STATE_HALF,
        TIME_STATE_FAST_FORWARD_X2,
        TIME_STATE_FAST_FORWARD_X4,
        TIME_STATE_CUSTOM
    };


    struct Clock
    {
        int state = TIME_STATE::TIME_STATE_PLAY;
        float playbackSpeed = 1.0f;
        //elapsed time
        float time = 0.0f;
        //time (in seconds) since last frame
        float deltaTime;
        //frames since app started
        uint32_t frameCount = 0;
        //fixed update time interval
        float fixedTimeStep = 1.0f / 60.0f;
        float fixedTimer = 0.0f;

        void SetState(TIME_STATE state, float playbackSpeed = 1.0f);
    };
} // namespace jam
