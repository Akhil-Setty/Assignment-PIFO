#include <map>
#include <queue>
#include <chrono>
#include <cstdlib>

// slack time are in microseconds
#define BASE_SLACK_TIME 50*1000
#define SLACK_TIME_RANGE 100*1000
#define NUM_PACKETS 100

// sleep time is in microseconds
#define MIN_SLEEP_TIME 10
#define SLEEP_TIME_RANGE 20 

using namespace std;

typedef struct {
	uint64_t slack; // slack in ms
	uint64_t arrival_time; // arrival time in ms 
}LstfPacketInfo;

void get_system_time(chrono::microseconds& ms)
{
	ms = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch());
}

void initialize_slack_times(uint32_t numFlows, map<uint32_t, uint64_t>& slack_times)
{
	for (auto i = 0; i < numFlows; i++)
	{
		slack_times[i] = static_cast<uint64_t>(BASE_SLACK_TIME + (rand() % SLACK_TIME_RANGE));
	}
}
