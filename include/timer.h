#pragma once

#include "keys.h"

namespace windowing
{
	class Timer
	{

	public:

		Timer();

		Timer(bool start);

		float stop_time(bool debug = false);

		void start_time();

		double current_time_ms() const;

		double current_time_s() const;

		double reset_ms(bool debug = false);
		
		double reset_s(bool debug = false);

		bool get_state() const;

		double get_delta_time_ms(bool debug = false);
		
		double get_delta_time_s(bool debug = false);

		~Timer();

	private:

		std::chrono::high_resolution_clock::time_point _timeStarted;
		std::chrono::high_resolution_clock::time_point _timeEnded;

		bool _state;

		bool _isStopped;

	};

}

