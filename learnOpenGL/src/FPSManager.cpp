#include "FPSManager.h"

#include <thread>


void FPSManager::startNewFrame() {
	std::chrono::time_point<std::chrono::high_resolution_clock> frameEndTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = frameEndTime - thisFrameStartTime;
	if (duration < minFrameDuration) {
		std::this_thread::sleep_for(minFrameDuration - duration);
	}
	lastFrameStartTime = thisFrameStartTime;
	thisFrameStartTime = std::chrono::high_resolution_clock::now();
	lastFrameDuration = thisFrameStartTime - lastFrameStartTime;
}