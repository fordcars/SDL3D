#ifndef SIMPLETIMER_H_
#define SIMPLETIMER_H_

class SimpleTimer
{
private:
	int startTicks; // The time when the timer was started
	int endTicks; // The time when the timer has stopped

public:
	SimpleTimer();
	~SimpleTimer();

	void start();
	int getTicks();
};

#endif /* SIMPLETIMER_H_ */