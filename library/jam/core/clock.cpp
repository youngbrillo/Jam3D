#include "clock.hpp"

void jam::Clock::SetState(TIME_STATE _state, float _playbackSpeed)
{
	state = _state;
	switch (state)
	{
	case jam::TIME_STATE_PAUSED:
		playbackSpeed = 0.0f;
		break;
	case jam::TIME_STATE_PLAY:
		playbackSpeed = 1.0f;
		break;
	case jam::TIME_STATE_HALF:
		playbackSpeed = 0.5f;
		break;
	case jam::TIME_STATE_FAST_FORWARD_X2:
		playbackSpeed = 2.0f;
		break;
	case jam::TIME_STATE_FAST_FORWARD_X4:
		playbackSpeed = 4.0;
		break;
	case jam::TIME_STATE_CUSTOM:
		playbackSpeed = _playbackSpeed;
		break;
	default:
		break;
	}
}
