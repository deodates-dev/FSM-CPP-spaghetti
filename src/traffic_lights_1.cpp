/**
\file test_traffic_lights_1.cpp
\brief a simple traffic light example, build using boost::asio as timer backend
*/


//#include "udp_server.hpp"

//#define SPAG_GENERATE_DOT
//#define SPAG_PRINT_STATES
#include "spaghetti.hpp"

#include "asio_wrapper.hpp"


#include <memory>

//-----------------------------------------------------------------------------------
enum EN_States { st_Init=0, st_Red, st_Orange, st_Green, st_BlinkOn, st_BlinkOff, NB_STATES };
enum EN_Events { ev_Reset=0, ev_WarningOn, NB_EVENTS };


SPAG_DECLARE_FSM_TYPE( fsm_t, EN_States, EN_Events, AsioWrapper, std::string );

//-----------------------------------------------------------------------------------
void callback( std::string v )
{
	std::cout << "cb, value=" << v << '\n';
}
//-----------------------------------------------------------------------------------
int main( int, char* argv[] )
{
	std::cout << argv[0] << ": " << fsm_t::buildOptions() << '\n';

	fsm_t fsm;

	std::cout << "fsm: nb states=" << fsm.nbStates() << " nb_events=" << fsm.nbEvents() << "\n";
	fsm.assignTimeOut( st_Init,   3, st_Red    ); // if state st_Init and time out of 5s occurs, then switch to state st_Red
	fsm.assignTimeOut( st_Red,    4, st_Green  );
	fsm.assignTimeOut( st_Green,  4, st_Orange );
	fsm.assignTimeOut( st_Orange, 1, st_Red   );

	fsm.assignTransitionAlways( ev_Reset,      st_Init ); // if reception of message ev_Reset, then switch to state st_Red, whatever the current state is
	fsm.assignTransitionAlways( ev_WarningOn, st_BlinkOn );

	fsm.assignTimeOut( st_BlinkOn,  1, st_BlinkOff );
	fsm.assignTimeOut( st_BlinkOff, 1, st_BlinkOn );

	fsm.assignCallback( st_Red,    callback, std::string("RED") );
	fsm.assignCallback( st_Orange, callback, std::string("ORANGE") );
	fsm.assignCallback( st_Green,  callback, std::string("GREEN") );

	fsm.printConfig( std::cout );
	fsm.writeDotFile( "test1.dot" );

	AsioWrapper<EN_States,EN_Events,std::string> asio;
	fsm.assignTimer( &asio );

	fsm.start();
}

