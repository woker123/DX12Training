#include "GameTimer.h"

void GameTimer::reset()
{
    mIsActive = false;
    mIsPause = false;
    QueryPerformanceFrequency((LARGE_INTEGER*)&mPerformanceFrequency);
    QueryPerformanceFrequency((LARGE_INTEGER*)&mInitialCounter);
    mCurrentCounter = mInitialCounter;
    mElapsedCounter = 0;
    mDeltaCounter = 0;
}

void GameTimer::start()
{
    if (mIsActive == false)
    {
        mIsActive = true;
        QueryPerformanceFrequency((LARGE_INTEGER*)&mPerformanceFrequency);
        QueryPerformanceCounter((LARGE_INTEGER*)&mInitialCounter);
        mCurrentCounter = mInitialCounter;
    }   
}

void GameTimer::end()
{
    reset();
}

void GameTimer::pause()
{
    mIsPause = true;
}

void GameTimer::resume()
{
    mIsPause = false;
}

double GameTimer::totalTime() const
{
    return (double)mElapsedCounter / (double)mPerformanceFrequency;
}

double GameTimer::deltaTime() const
{
    return (double)mDeltaCounter / (double)mPerformanceFrequency;
}

void GameTimer::updata()
{
    if (mIsActive)
    {
        __int64 newCounter;
        QueryPerformanceCounter((LARGE_INTEGER*)&newCounter);
        mDeltaCounter = newCounter - mCurrentCounter;
        mCurrentCounter = newCounter;
        if (!mIsPause)
        {
            mElapsedCounter += mDeltaCounter;
        }
    }
}
