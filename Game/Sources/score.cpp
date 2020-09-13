#include "Score.h"

Score::Score() : targetsEliminated(0), currentTime(0) {}

void Score::targetEliminated() {
    targetsEliminated++;
}

void Score::resetTargetEliminated() {
    targetsEliminated = 0;
}

void Score::timeUsed(unsigned long time) {
    currentTime += time;
}

void Score::resetTimeUsed() {
    currentTime = 0;
}

const unsigned int Score::getTargetsEliminated() const {
    return targetsEliminated;
}

const unsigned long Score::getCurrentTime() const {
    return targetsEliminated;
}
