#pragma once

class Score {
public:
    Score();

    void targetEliminated();

    void resetTargetEliminated();

    void timeUsed(unsigned long time);

    void resetTimeUsed();

    const unsigned int getTargetsEliminated() const;

    const unsigned long getCurrentTime() const;

private:
    unsigned int targetsEliminated;
    unsigned long currentTime;
};
