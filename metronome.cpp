

class metronome
{
public:
	void start_timing()
	{
		m_timing = true;
		beat_samples = 0;
		temp_start_time = high_resolution_clock::now();
		
	}

	void stop_timing()
	{
		m_timing = false;
		temp_stop_time = high_resolution_clock::now();
	}

	void tap()
	{
		temp_stop_time = high_resolution_clock::now();
		m_beats[beat_samples] = duration_cast<milliseconds>(stop - start);
		beat_samples++;
		temp_start_time = high_resolution_clock::now();
	}

	size_t get_bpm() const
	{
		size_t bpm = 0;
		for(int i = 0; !m_timing && i < beat_samples; i++)
		{
			bpm += m_beats[i];
		}
		return (bpm / beat_samples);
	}

};

