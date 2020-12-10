#pragma once


#include <Windows.h>


/*this class is for simple time recording in game*/
/*one use this timer must call it every loop*/
class GameTimer
{
public:
	GameTimer()
	{
		reset();
	}
	virtual ~GameTimer() {}
	GameTimer(const GameTimer&) = delete;
	GameTimer& operator=(const GameTimer&) = delete;

public:
	/**reset all state of this timer*/
	void reset();

	/**start record time*/
	void start();

	/**end record time and reset all state*/
	void end();

	/**pause time*/
	void pause();

	/**resume time*/
	void resume();

	/**get total elapsed time by this timer*/
	double totalTime() const;

	/**get delta time between two frame*/
	double deltaTime() const;

	/**update timer state, must be invoked every loop*/
	void updata();

private:
	bool mIsActive;
	bool mIsPause;
	__int64 mPerformanceFrequency;
	__int64 mInitialCounter;
	__int64 mCurrentCounter;

	__int64 mElapsedCounter;
	__int64 mDeltaCounter;
};




