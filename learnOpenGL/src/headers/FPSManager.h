#pragma once

#include <chrono>

class FPSManager {
private:

	const std::chrono::time_point<std::chrono::high_resolution_clock> managerBeginningTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> thisFrameStartTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> lastFrameStartTime;
	std::chrono::duration<double> minFrameDuration;
	std::chrono::duration<double> lastFrameDuration;

public:

	/**
		Creates a new FPSManager.
		@param fps The wanted frames per second.
	*/
	inline FPSManager(double fps) :
		minFrameDuration(1.0 / fps),
		managerBeginningTime(std::chrono::high_resolution_clock::now()),
		thisFrameStartTime(managerBeginningTime),
		lastFrameStartTime(managerBeginningTime),
		lastFrameDuration(0.0)
		{
	}

	/**
		Gets the fps of the
	*/
	inline double getFPS() {
		return 1000.0 / minFrameDuration.count();
	}

	/**
		Gets the minimum duration of a frame. This minimum duration is determined by the wanted FPS.
	*/
	inline double getMinFrameDuration() {
		return minFrameDuration.count();
	}


	/**
		returns this frame's start time from the point the fps manager was created.
	*/
	inline double getThisFrameStartTime() {
		std::chrono::duration<double> duration = thisFrameStartTime - managerBeginningTime;
		return duration.count();
	}

	/**
		returns the last frame's start time from the point the fps manager was created.
	*/
	inline double getLastFrameStartTime() {
		std::chrono::duration<double> duration = lastFrameStartTime - managerBeginningTime;
		return duration.count();
	}

	/**
		Gets the duration of the last frame.
	*/
	inline double getLastFrameDuration() {
		return lastFrameDuration.count();
	}

	// TODO
	inline void setFPS(double fps) {
		minFrameDuration = std::chrono::duration<double>(1.0 / fps);
	}

	/**
		Ends the current frame and starts a new one.
	*/
	void startNewFrame();
};