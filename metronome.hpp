#pragma once

#include <cstddef>
#include <chrono>



using namespace std::chrono;

class metronome
{
public:
	enum { beat_samples = 3 };

	metronome()
	: m_timing(false), m_beat_count(0) {}
	~metronome() {}

	// Call when entering "learn" mode
	void start_timing();
	// Call when leaving "learn" mode
	void stop_timing();

	// Should only record the current time when timing
	// Insert the time at the next free position of m_beats
	void tap();

	bool is_timing() const { return m_timing; }
	// Calculate the BPM from the deltas between m_beats
	// Return 0 if there are not enough samples
	size_t get_bpm() const;

private:
	bool m_timing;

	// Insert new samples at the end of the array, removing the oldest
	size_t m_beats[beat_samples]; //entry in ms
	size_t m_beat_count;
//	high_resolution_clock
	high_resolution_clock::time_point temp_start_time;
	high_resolution_clock::time_point temp_stop_time;
	
};

