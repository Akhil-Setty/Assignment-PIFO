#include "pifo.hpp"
#include "lstf.hpp"
#include <iostream>
#include <vector>

#include <thread>
#include <mutex>

using namespace std;

// queue is shared among all hosts and the switch, so we need a mutex to synchronize 
mutex queueMutex;
map<uint32_t, queue<LstfPacketInfo>> rankStore;

void packet_generator(uint32_t flowId, uint64_t slackTime)
{
	/* initialize random seed: */
	srand (time(NULL));
	long sleep_time = MIN_SLEEP_TIME + (rand() % SLEEP_TIME_RANGE);
	this_thread::sleep_for(chrono::microseconds(sleep_time));

	for (auto i = 0; i < NUM_PACKETS; i++)
	{
		LstfPacketInfo new_packet;
		new_packet.slack = slackTime + i;
		chrono::microseconds ms;
		get_system_time(ms);
		new_packet.arrival_time = static_cast<uint64_t>(ms.count());
		// transmit packet and sleep for random value in range(refer lstf.hpp)
		{
			lock_guard<std::mutex> lock(queueMutex);
			rankStore[flowId].push(new_packet);
		}
		sleep_time = MIN_SLEEP_TIME + (rand() % SLEEP_TIME_RANGE);
		this_thread::sleep_for(chrono::microseconds(sleep_time));
	}
	
	// cout << "Flow " << flowId << " completed." <<endl;
}

void pifo_pipeline(uint32_t numFlows)
{
	PIFO pifo;
	double average_wait_time = 0;
	uint32_t num_packets_ontime = 0;
	uint32_t num_packets_remaining = numFlows * NUM_PACKETS;

	uint32_t flowId = 0;
	while (num_packets_remaining > 0)
	{ 
		// dequeue followed by enqueue (avoiding round robin, can swap these operations as well)
		if (pifo.size() > 0)
		{
			uint64_t priority;
			string flow_id; 
			pifo.dequeue(flow_id, priority);
			flowId = static_cast<uint32_t>(std::stoul(flow_id));
			LstfPacketInfo dq_packet;
			{
				lock_guard<std::mutex> lock(queueMutex);
				if (rankStore.find(flowId) != rankStore.end())
				{
					if (rankStore[flowId].size() > 0)
					{
						dq_packet = rankStore[flowId].front();
						rankStore[flowId].pop();
					}
				}
			}
			chrono::microseconds ms;
			get_system_time(ms);
			uint64_t deq_time = static_cast<uint64_t>(ms.count());
			// cout << wait_time << "=" << priority << "-" << static_cast<uint64_t>(ms.count()) << endl;
			if (priority >= deq_time)
				num_packets_ontime++;
			average_wait_time = average_wait_time + (priority >= deq_time ? priority - deq_time: deq_time - priority);
			num_packets_remaining--;
			if (num_packets_remaining <= 0)
				break;
		}
		else {
			flowId = static_cast<uint32_t>(rand() % numFlows);
		}
		// enqueue operation
		{
			lock_guard<std::mutex> lock(queueMutex);
			if (rankStore.find(flowId) != rankStore.end())
			{
				if (rankStore[flowId].size() > 0)
				{
					LstfPacketInfo& enq_packet = rankStore[flowId].front();
					// rank computation
					uint64_t priority = enq_packet.slack + enq_packet.arrival_time;
					pifo.enqueue(to_string(flowId), priority);
					// cout << priority << "=" << enq_packet.slack << "+" << enq_packet.arrival_time << endl;
				}
			}
		}
	}
	cout << "LSTF: " << (num_packets_ontime * 100) / (NUM_PACKETS * numFlows) << "% packets met deadline" << endl;
	cout << "LSTF: average wait time is " << average_wait_time / (NUM_PACKETS * numFlows) << endl;
}

void lstf_packet_scheduling()
{
	/* initialize random seed: */
	srand (time(NULL));

	uint32_t numFlows = 1024;
	map<uint32_t, uint64_t> slack_times;
	initialize_slack_times(numFlows, slack_times);

	vector<thread*> host_threads;
	for(auto i = 0; i < numFlows; i++)
	{
		rankStore[i] = queue<LstfPacketInfo>(); 
		thread* host = new thread(packet_generator, i, slack_times[i]);
		host_threads.push_back(host);
	}

	uint64_t sleep_time = static_cast<uint64_t>(MIN_SLEEP_TIME + (rand() % SLEEP_TIME_RANGE));
	this_thread::sleep_for(std::chrono::microseconds(sleep_time));

	pifo_pipeline(numFlows);

	// cleanup all the created threads
	auto it = host_threads.begin();
	while(it !=  host_threads.end())
	{
		(*it)->join();
		delete(*it);
		host_threads.erase(it);
	}
}

int main()
{
	srand(time(NULL));
	chrono::microseconds ms;
	get_system_time(ms);
	uint64_t start_time = static_cast<uint64_t>(ms.count());
	uint64_t slack_time = static_cast<uint64_t>(BASE_SLACK_TIME + rand() % SLACK_TIME_RANGE);
	get_system_time(ms);
	uint64_t end_time = static_cast<uint64_t>(ms.count());
	cout << slack_time << " + " << start_time << " = " << slack_time + start_time << " | end_time: " << end_time << endl;

	lstf_packet_scheduling();
}
