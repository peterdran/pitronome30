

class metronome
{
public:
	void start_timing()
	{
		m_timing = true;
		beat_samples = 0;
		
	}

	void stop_timing()
	{
		m_timing = false;
		
	}

	void tap()
	{
		//somehow keep track of time per press?
	}

	size_t get_bpm() const
	{
		size_t bpm = 0;
		for(int i = 0; !m_timing && i < beat_samples; i++)
		{
			//how's it timed?
		}
		return (bpm / beat_samples);
	}

};
