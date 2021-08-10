#pragma once

#include <thread>
#include <mutex>
#include <chrono>
#include <queue>
#include <functional>

using namespace std;

class Active {

public:

	using MessageType = function<void()>;

	Active() {
		_thd = thread([&]() {
			while (!_is_stopped)
			{
				lock_guard<recursive_mutex> lock{ _mutex };
				if (_msg_queue.size() == 0)
				{
					this_thread::sleep_for(
						chrono::milliseconds(100));
				}
				else
				{
					_msg_queue.front()();
					_msg_queue.pop();
				}
			}
			});
	}

	~Active() {
		_thd.join();
	}

	void send(MessageType m) {
		lock_guard<recursive_mutex> lock{ _mutex };
		_msg_queue.push(m);
	}

	void stop() {
		if (_is_stopped)
		{
			return;
		}

		_msg_queue.push([&]() {
			_is_stopped = true;
			});
		_thd.join();
	}

private:

	thread _thd;
	queue<MessageType> _msg_queue;
	bool _is_stopped{false};
	recursive_mutex _mutex;

};
