# spaghetti
A C++ library useful for simple and easy Finite State Machine (FSM) building

## Key points
- Status: alpha (currently in dev. mode), API is not stabilized yet, so don't use until advised...
- Licence: GPL v3
- Audience: C++ developper having to implement a FSM
- Webpage: https://github.com/skramm/spaghetti
- Language: C++11
- Author: S. Kramm, LITIS, University of Rouen, France - 2018

## General information

### What is this ?
 This library doesn't provide any highend algorithm, you can consider it as a container holding all that is required to implement
 a FSM.
 However, it **does not** provide the main event loop. This is up to the user code.
 The rationale behind this is that this loop may vary greatly depending on the requirements
 (need for timeout or not, need to acquire hardware external events,...).

 This library provides an easy way to specify states and events, and how and when it will switch from one state to another.
 It also has some additional facilities, such as logging and easy timeout handling.


### Main features

 - single file header-only library, just fetch the file spaghetti.hpp and store it on your machine somewhere accessible by your compiler
 - C++11, no dependencies other than standard library
 - ease of use, and performance
 - runtime logging capability
 - provided with some sample programs, see folder src and or src/html/index.html

 ### Why/when should I use / not use this ?
  - pros: it is pretty simple to get running (sample programs provided)
  - cons: it is inherently not type safe, as states and events are provided as enum values.
  If you can live with that, then you may go ahead.

## Showcase 1: Hello World for FSM

Once you have downloaded the file ```spaghetti.hpp```, you are able to make the [turnstyle (WP link)](https://en.wikipedia.org/wiki/Finite-state_machine#Example:_coin-operated_turnstile) example run, which is the "Hello World" for Finite State Machines.

First, create enums for states and events:

```C++
#include "spaghetti.hpp"

enum States { st_Locked, st_Unlocked, NB_STATES };
enum Events { ev_Push, ev_Coin, NB_EVENTS };
```

Then, create the FSM data type:
```C++
SPAG_DECLARE_FSM_TYPE_NOTIMER( fsm_t, States, Events, bool );
```
This means you declare the type ```fsm_t```, using ```States``` and ```Events```, with callback functions having a bool as an argument.
(Alternatively, you could also use a ```typedef```, but lets say this is easier at present.)

Now, you can instanciate the fsm:

```C++
int main()
{
	fsm_t fsm;
```

Now, you need to configure your FSM, that is define what event in what state will trigger switching to what state.
With this simple example, you just do:
```C++
fsm.assignTransition( st_Locked,   ev_Coin, st_Unlocked );
fsm.assignTransition( st_Unlocked, ev_Push, st_Locked );
```
Ok, and also tell the FSM what is to be done when a state gets triggered. This is done by providing a callback function:

```C++
	fsm.assignCallback( st_Locked,   cb_func, true );
	fsm.assignCallback( st_Unlocked, cb_func, false );
```

Alternatively (and that is useful if you have a lot of states), you can provide the callback function for all the states, and separately set the argument value:

```C++
	fsm.assignGlobalCallback( cb_func );
	fsm.assignCallbackValue( st_Unlocked, false );
	fsm.assignCallbackValue( st_Locked,   true );
```

And of course you must provide that callback function:

```C++
void cb_func( bool b )
{
	if( b )
		std::cout << "State: Locked\n";
	else
		std::cout << "State: Unlocked\n";
}
```
Done for configuration.
Now, how to run this. (Here, you don't really need to because there are no timeout, but its recommended to do so, to avoid forgetting it afterwards. Just call ```run()``` ).

Here the events will be triggered by the keyboard, so lets do this:
```C++
fsm.run();
do
{
	char key;
	std::cout << "Enter command: ";
	std::cin >> key;
	switch( key )
	{
		case 'A':
			std::cout << "Event: push\n";
			fsm.processEvent( ev_Push );
		break;

		case 'B':
			std::cout << "Event: coin\n";
			fsm.processEvent( ev_Coin );
		break;
	}
}
while( 1 );
```

That's it!
All of this is given as a sample program,
see file ```src/turnstyle_1.cpp``` and/or just clone repo end enter
```make demo -j4``` followed by ```bin/turnstyle_1```.


## Showcase 2: let's add a timer !

Say your want to add something to your turnstyle: if a coin is given and nobody pushes within 10 seconds, then you want to lock it back.
This means adding a timeout to the "unlocked" state, leading back to the "locked" state.

Now you need to provide a timer class, that will handle this.
This class has only on one requirements: it must provide two functions:
```timerStart()``` and ```timerCancel()```.


And the declaration of the data type of the FSM will become:
```C++
SPAG_DECLARE_FSM_TYPE( fsm_t, States, Events, MyTimer, bool );
```

Then we can add the timeout to the configuration step:
```C++
fsm.assignTimeOut( st_Unlocked, 10, st_Locked );
```

The time out functions need to be defined, with the following form:
```C++
template<>
void
MyTimer<States, Events>::timerStart( const fsm_t* p_fsm )
{
	... start the timer
}

template<>
void
MyTimer<States, Events>::timerCancel()
{
	... cancel the timer
}
```

The latter function is needed because your fsm handles both "hardware" events (keypress) and timeouts.
In another sample program (```src/traffic_lights_1.cpp```), there is only timeout events handled, to there is no need for
```timerCancel()```.


## Additional facilities

- Printing the configuration:

- Printing runtime data:

TO BE CONTINUED

## Build options

Several symbols can change the behavior of the library and/or add additional capabilities, you can define them either by adding them in your makefile
(with GCC, its \c -DSPAG_SOME_SYMBOL ), or by hardcoding in your program, BEFORE including the library file, like this:

```C++
#define SPAG_SOME_SYMBOL
#include "spaghetti.hpp"
```

They all start with these 5 characters: ```SPAG_```

The available options/symbols are:
- ```SPAG_PRINT_STATES``` : will print on stdout the steps, useful only for debugging your SpagFSM
- ```SPAG_ENABLE_LOGGING``` : will enable logging of dynamic data (see spag::SpagFSM::printLoggedData() )
- ```SPAG_FRIENDLY_CHECKING```: A lot of checking is done to ensure no nasty bug will crash your program.
However, in case of incorrect usage of the library by your client code (say, invalid index value),
the default behavior is to spit a standard error message that can be difficult to understand.
So if you define this symbol at build time, instead of getting this:
```
myfile: /usr/local/include/spaghetti.hpp:236: void spag::SpagFSM<STATE, EVENT, TIM>::assignTransitionMat(const std::vector<std::vector<T> >&) [with STATE = SERSTAT; EVENT = EN_EVENTS; TIM = AsioWrapper]: Assertion `mat.size() == EVENT::NB_EVENTS' failed.
Aborted
```
you will get this:
```
Spaghetti: runtime error in func: assignTransitionMat(), values are not equal:
 - mat.size() value=7
 - EVENT::NB_EVENTS value=8
Exiting...
```
If this symbol is not defined, regular checking is done with the classical ```assert()```.
As usual, this checking can be removed by defining the symbol ```NDEBUG```.

- ```SPAG_ENUM_STRINGS``` : this enables the usage of enum-string mapping, for events only.
You can provide a string either individually with
```C++
	fsm.assignString2Event( std::make_pair(EV_MyEvent, "something happened" );
```
or globally, by providing a vector of pairs(enum values, string). For example:
```C++
	std::vector<std::pair<EVENT,std::string>> v_str = {
		{ EV_RESET,       "Reset" },
		{ EV_WARNING_ON,  "Warning On" },
		{ EV_WARNING_OFF, "Warning Off" }
	};
	fsm.assignStrings2Events( v_str );
```
These strings will then be printed out when calling the ```printConfig()``` member function.


## FAQ

- Q: Why that name ?
- A: Naming is hard. But, lets see: Finite State Machine = FSM = Flying Spaghetti Monster
(see [WP](https://en.wikipedia.org/wiki/Flying_Spaghetti_Monster)).
So you got it.
(and certainly not related to [this](https://en.wikipedia.org/wiki/Spaghetti_code), hopefully!)

- Q: What if I have more that a single argument to pass to my callback function ?
- A: then, you'll need to "pack it" in some class, or use a ```std::pair```, or ```std::tuple```.

## References

https://en.wikipedia.org/wiki/Finite-state_machine

More info to come...
