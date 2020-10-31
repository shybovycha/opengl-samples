#include "Score.h"

Score::Score() : targetsEliminated(0), currentTime(0), totalTimeUsed(0)
{
}

void Score::targetEliminated()
{
    targetsEliminated++;
}

void Score::resetTargetEliminated()
{
    targetsEliminated = 0;
}

void Score::timeUsed(unsigned long time)
{
    totalTimeUsed += time;
}

void Score::resetTimeUsed()
{
    totalTimeUsed = 0;
}

const unsigned int Score::getTargetsEliminated() const
{
    return targetsEliminated;
}

const unsigned long Score::getCurrentTime() const
{
    return currentTime;
}

void Score::tick()
{
    if (currentTime < 1)
    {
        return;
    }

    currentTime--;
}

void Score::resetCurrentTime(const unsigned long maxTime)
{
    currentTime = maxTime;
}

const unsigned long Score::getTotalTime() const
{
    return totalTimeUsed;
}
