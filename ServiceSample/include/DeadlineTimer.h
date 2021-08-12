#pragma once

#include <chrono>
#include <thread>

using namespace std;

template<typename IOContextType>
class DeadlineTimer
{

public:

	DeadlineTimer(IOContextType& io_ctx)
		:_io_ctx{ io_ctx } 
	{
	}

	template<typename WaitHandlerType>
	void async_wait(WaitHandlerType&& wait_handler) 
	{
		_io_ctx.send([&, wait_handler_{
			std::forward<WaitHandlerType>(wait_handler) }] () 
			{
				while (chrono::system_clock::now() - _start < _expiry_time)
				{
					this_thread::sleep_for(chrono::milliseconds(1));
				}
				wait_handler_();
			}
		);
	}

	void expires_from_now(
		const chrono::milliseconds& expiry_time)
	{
		_start = chrono::system_clock::now();
		_expiry_time = expiry_time;
	}

private:

	IOContextType& _io_ctx;
	chrono::milliseconds _expiry_time;
	chrono::system_clock::time_point _start;

};