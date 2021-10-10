#include "metronome.hpp"
#include <chrono>

using namespace std::chrono;

void metronome::start_timing()
{
	this->m_timing = true;
	this->beat_samples = 0;
	this->temp_start_time = high_resolution_clock::now();
	
}

void metronome::stop_timing()
{
	this->m_timing = false;
	this->temp_stop_time = high_resolution_clock::now();
}

void metronome::tap()
{
	this->temp_stop_time = high_resolution_clock::now();
	this->m_beats[beat_samples] = duration_cast<milliseconds>(this->temp_stop_time - this->temp_start_time);
	this->beat_samples++;
	this->temp_start_time = high_resolution_clock::now();
}

size_t metronome::get_bpm() const
{
	size_t bpm = 0;
	for(int i = 0; !this->m_timing && i < this->beat_samples; i++)
	{
		bpm += m_beats[i];
	}
	return (bpm / this->beat_samples); //note: integer division
}

