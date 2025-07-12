
#include "timer.h"

namespace windowing
{

	Timer::Timer()
	{
		_timeStarted = std::chrono::steady_clock::now();

		_isStopped = false;
	}

	Timer::Timer(bool start)
	{
		if (start)
		{
			_state = true;
			_timeStarted = std::chrono::steady_clock::now();
		}
		else
		{
			_state = false;
			_timeStarted = std::chrono::steady_clock::time_point();
		}

		_isStopped = false;
	}



	float Timer::stop_time(bool debug)
	{
		_state = true;
		_isStopped = true;

		_timeEnded = std::chrono::steady_clock::now();

		std::chrono::duration duration = _timeEnded - _timeStarted;

		float milliseconds = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());

		if (debug)
		{
			std::cout << "Timer stops at " << milliseconds << " ms \n";
		}

		return milliseconds;
	}

	void Timer::start_time()
	{
		_state = true;
		_isStopped = false;

		_timeStarted = std::chrono::steady_clock::now();
	}

	double Timer::current_time_ms() const 
	{
		return static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _timeStarted).count());
	}

	double Timer::current_time_s() const
	{
		return current_time_ms() / 1000.0;
	}

	double Timer::reset_ms(bool debug)
	{
		if (_isStopped)
		{
			if (debug)
			{
				std::cout << "Timer is stopped\n";
			}
			return 0.0f;
		}
		_timeEnded = std::chrono::steady_clock::now();

		std::chrono::duration duration = _timeEnded - _timeStarted;

		_timeStarted = std::chrono::steady_clock::now();

		double milliseconds = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());

		if (debug)
		{
			std::cout << milliseconds << " ms \n";
		}

		return milliseconds;
	}

	double Timer::reset_s(bool debug)
	{
		return reset_ms(debug) / 1000.0;
	}

	bool Timer::get_state() const
	{
		return _state;
	}

	double Timer::get_delta_time_ms(bool debug)
	{
		double time = reset_ms(debug);
		if (time < 0.005)
		{
			time = 0.005;
		}
		return time;
	}

	double Timer::get_delta_time_s(bool debug)
	{
		return get_delta_time_ms(debug) / 1000.0;
	}


	Timer::~Timer()
	{
		_state = false;

		if (!_isStopped)
		{
			stop_time(false);
		}
	}


} // namespace windowing

