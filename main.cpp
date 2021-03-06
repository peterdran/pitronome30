#include <chrono>
#include <thread>
using namespace std::chrono_literals;

#include <cpprest/http_msg.h>
#include <wiringPi.h>

#include "metronome.hpp"
#include "rest.hpp"

// ** Remember to update these numbers to your personal setup. **
//#define LED_RED   17
#define LED_RED   20
//#define LED_GREEN 27
#define LED_GREEN 21
//#define BTN_MODE  23
#define BTN_MODE  6
//#define BTN_TAP   24
#define BTN_TAP   5

// Mark as volatile to ensure it works multi-threaded.
volatile bool btn_mode_pressed = false; 
volatile bool btn_tap_pressed = false;
volatile bool mode = false;//mode: false is play, true is learn

//metronome *m;

volatile size_t bpm = 60; //default 1s
volatile size_t min_bpm = 0;
volatile size_t max_bpm = 1;

// Run an additional loop separate from the main one.
void blink() 
{
	bool on = false;
	// ** This loop manages LED blinking. **
	while (true) {
		// The LED state will toggle once every second.
		std::this_thread::sleep_for(900ms);

		// Perform the blink if we are pressed,
		// otherwise just set it to off.
		if (!mode)
			on = !on;
		else
			on = false;

		// HIGH/LOW probably equal 1/0, but be explicit anyways.
		digitalWrite(LED_RED, (on) ? HIGH : LOW);
		std::this_thread::sleep_for(100ms); //make it actually blink
		digitalWrite(LED_RED, LOW);
	}
}

// This is called when a GET request is made to "/answer".
void get42(web::http::http_request msg) 
{
	msg.reply(200, web::json::value::number(42));
}

void bpm_base_get(web::http::http_request msg)
{
	msg.reply(200, web::json::value::number(bpm));
}

void bpm_base_put(web::http::http_request msg)
{
	auto json = msg.extract_json();
	//bpm = json.as_integer(); //TODO handle exception
	msg.reply(200);
}

void bpm_min_get(web::http::http_request msg)
{
	msg.reply(200, web::json::value::number(min_bpm));
}


void bpm_min_delete(web::http::http_request msg)
{
	min_bpm = 0;
	msg.reply(200);
}


void bpm_max_get(web::http::http_request msg)
{
	msg.reply(200, web::json::value::number(max_bpm));
}


void bpm_max_delete(web::http::http_request msg)
{
	max_bpm = 0;
	msg.reply(200);
}

void mode_button_ISR()
{
	btn_mode_pressed = true;
}

void tap_button_ISR()
{
	btn_tap_pressed = true;
}

// This program shows an example of input/output with GPIO, along with
// a dummy REST service.
// ** You will have to replace this with your metronome logic, but the
//    structure of this program is very relevant to what you need. **
int main() 
{
	// WiringPi must be initialized at the very start.
	// This setup method uses the Broadcom pin numbers. These are the
	// larger numbers like 17, 24, etc, not the 0-16 virtual ones.
	wiringPiSetupGpio();

	// Set up the directions of the pins.
	// Be careful here, an input pin set as an output could burn out.
	pinMode(LED_RED, OUTPUT);
	pinMode(BTN_MODE, INPUT);
	// Note that you can also set a pull-down here for the button,
	// if you do not want to use the physical resistor.
	//pullUpDnControl(BTN_MODE, PUD_DOWN);

	// Configure the rest services after setting up the pins,
	// but before we start using them.
	// ** You should replace these with the BPM endpoints. **
	auto get42_rest = rest::make_endpoint("/answer");
	auto bpm_base = rest::make_endpoint("/bpm");
	auto bpm_min = rest::make_endpoint("/bpm/min");
	auto bpm_max = rest::make_endpoint("/bpm/max");

	get42_rest.support(web::http::methods::GET, get42);
	bpm_base.support(web::http::methods::GET, bpm_base_get);
	bpm_base.support(web::http::methods::PUT, bpm_base_put);
	bpm_min.support(web::http::methods::GET, bpm_min_get);
	bpm_min.support(web::http::methods::DEL, bpm_min_delete);
	bpm_max.support(web::http::methods::GET, bpm_max_get);
	bpm_max.support(web::http::methods::DEL, bpm_max_delete);

	// Start the endpoints in sequence.
	get42_rest.open().wait();
	bpm_base.open().wait();
	bpm_min.open().wait();
	bpm_max.open().wait();

	// Use a separate thread for the blinking.
	// This way we do not have to worry about any delays
	// caused by polling the button state / etc.
	std::thread blink_thread(blink);
	blink_thread.detach();

	//create interrupt handler for the mode button
	wiringPiISR(BTN_MODE, INT_EDGE_RISING, mode_button_ISR);
	wiringPiISR(BTN_TAP, INT_EDGE_RISING, tap_button_ISR);

	metronome m;// = new metronome();

	// ** This loop manages reading button state. **
	while (true) 
	{
		// We are using a pull-down, so pressed = HIGH.
		// If you used a pull-up, compare with LOW instead.
		//btn_mode_pressed = (digitalRead(BTN_MODE) == HIGH);
		// Delay a little bit so we do not poll too heavily.
		std::this_thread::sleep_for(10ms);

		// ** Note that the above is for testing when the button
		// is held down. For detecting "taps", or momentary
		// pushes, you need to check for a "rising edge" -
		// this is when the button changes from LOW to HIGH... **
		
		//interrupt sets flag to switch modes. this loop waits for said flag and executes metronome methods
		
		if(m.is_timing())
		{
			if(btn_mode_pressed)
			{
				m.stop_timing();
				btn_mode_pressed = false;
				bpm = m.get_bpm();
				
			}
			else if(btn_tap_pressed)
			{
				m.tap();
				btn_tap_pressed = false;
			}
			else
			{
				//something?
			}

		}
		else if(btn_mode_pressed)
		{
			m.start_timing();
			mode = true;
			btn_mode_pressed = false;

		}
		else
		{
			//something?
		}
		
		
		min_bpm = (min_bpm > bpm) ? bpm : min_bpm;
		max_bpm = (max_bpm < bpm) ? bpm : max_bpm;
	}

	return 0;
}

