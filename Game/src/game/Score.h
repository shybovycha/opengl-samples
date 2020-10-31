#pragma once

// max time per level; in milliseconds, not microseconds
const unsigned long MAX_TIME = 60 * 100;

class Score
{
public:
    Score();

    void targetEliminated();

    void resetTargetEliminated();

    void timeUsed(unsigned long time);

    void resetTimeUsed();

    void tick();

    void resetCurrentTime(const unsigned long maxTime = MAX_TIME);

    const unsigned int getTargetsEliminated() const;

    const unsigned long getCurrentTime() const;

    const unsigned long getTotalTime() const;

private:
    unsigned int targetsEliminated;
    unsigned long currentTime;
    unsigned long totalTimeUsed;
};
