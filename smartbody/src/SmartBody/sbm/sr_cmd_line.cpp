#include <sstream>

#ifdef WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#endif

#include <vhcl_log.h>
#include "sr_cmd_line.h"
#include <sbm/sbm_constants.h>


#define ENABLE_NEW_PROMPT		1

#define ENABLE_KBD_POLL_BUG 	0
#define DISABLE_KBD_POLL_ECHO 	0

///////////////////////////////////////////////////////////////////////////

char *realloc_char_buffer( char *old_buff_p, int usage, int new_len )	{

	if( ( old_buff_p == NULL )&&( usage > 0 ) )   {
		return( NULL );
	}
	if( new_len < ( usage + 1 ) ) {
		return( old_buff_p );
	}
	char *new_buff = new char[ new_len ];
	if( new_buff == NULL )	{
		return( NULL );
	}
//	printf( "REALLOC: u:%d l:%d\n", usage, new_len );
	if( old_buff_p && ( usage > 0 ) )	{
		memcpy( new_buff, old_buff_p, usage + 1 );
		delete [] old_buff_p;
	}
	else	{
		new_buff[ 0 ] = '\0';
	}
	return( new_buff );
}

void edit_char_buffer( char *buff_p, int usage, int pos, char c ) {

	int tail_size = usage - pos;
	char *tmp_p = new char[ tail_size + 1 ];
	
	if( c )	{
//printf( "<ins>\n" );
		memcpy( tmp_p, buff_p + pos, tail_size + 1 );
		buff_p[ pos ] = c;
		memcpy( buff_p + pos + 1, tmp_p, tail_size + 1 );
	}
	else	{
//printf( "<del>\n" );
		memcpy( tmp_p, buff_p + pos + 1, tail_size );
		memcpy( buff_p + pos, tmp_p, tail_size );
		buff_p[ usage ] = 0;
	}
	delete [] tmp_p;
}

#ifndef WIN32

inline void linux_enable_kbd_poll( void )	{

	struct termios attr;
	tcgetattr( STDIN_FILENO, &attr );
	attr.c_lflag &= ~( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSANOW, &attr );
}

inline void linux_disable_kbd_poll( void )	{

	struct termios attr;
	tcgetattr( STDIN_FILENO, &attr );
	attr.c_lflag |= ( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSANOW, &attr );
}

inline int linux_query_kbhit( void )	{

	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	fd_set rdfs;
	FD_ZERO( &rdfs );
	FD_SET( STDIN_FILENO, &rdfs );
	select( STDIN_FILENO + 1, &rdfs, NULL, NULL, &tv );
	int set = FD_ISSET( STDIN_FILENO, &rdfs );
	return( set );
}
#endif

inline int query_kbhit_stdin( void )	{

#ifdef WIN32
	return( _kbhit() );
#else
	return( linux_query_kbhit() );
#endif
}

inline char read_char_stdin( void )	{

#ifdef WIN32
	return( (char)_getch() );
#else
	return( (char)getchar() );
#endif
}

///////////////////////////////////////////////////////////////////////////

// ascii table:
// http://www.robelle.com/smugbook/ascii.html

// Linux up-arrow sequence: { KBD_ESC, KBD_OPEN_BRK, 'A' }
// Linux arrow keys: { A, B, C, D } -> { up, dn, rt, lt }

// Win32 up-arrow sequence: { KBD_WIN32_ESC_SGN, 'H' }
// Win32 arrow keys: { H, P, M, K } -> { up, dn, rt, lt }

#define KBD_NULL		0

#define KBD_BS			8	// backspace (win32: delete)
#define KBD_HT			9
#define KBD_LF			10
#define KBD_CR			13

#define KBD_ESC 		27
#define KBD_HOME_ESC	79	// { home, end } -> { 70, 72 } == { F, H }
#define KBD_OPEN_BRK	91

#define KBD_DEL 		127	// delete (linux: backspace)

#define KBD_WIN32_ESC		224
#define KBD_WIN32_ESC_SGN	-32

#define KBD_WIN32_ESC_F11	-123
#define KBD_WIN32_ESC_F12	-122

#define NULL_MODE		0
#define LIN_ARROW_MODE	1
#define LIN_FUNCT_MODE	2
#define WIN_ARROW_MODE	3
#define WIN_FUNCT_MODE	4

#define STRING_MODE		1
#define TAB_MODE		2
#define ARROW_UP_MODE	3
#define ARROW_DN_MODE	4
#define ARROW_LT_MODE	5
#define ARROW_RT_MODE	6

/*
	ANSI...
	http://cboard.cprogramming.com/cplusplus-programming/74567-do-arrow-keys-have-two-ascii-values.html

	getch() and other keyboard input functions return two integers for all 
	special keys, such as arrow keys and function keys (F1, F2, ... F10). 
	When getch() returns 0 or 224 as the first value, then a special key was 
	pressed and the program must call getch() again to get the key's integer 
	value. That integer value duplicates one of the standard ascii values of 
	other keys and your program may have to should encode it to make it 
	distingushable from normal ascii keys. I normally make it a negative 
	value, but other programmers may add 255 to it.

	int key;
	key = getch();
	if(key == 0 || key == 224)
	{
	   key = getch() + 255;
	}
*/

///////////////////////////////////////////////////////////////////////////////

bool keyboard_query_stdin( char *cp, int *mode_p )	{
	bool set = false;

	if( cp == NULL ) return( false );
	if( mode_p == NULL ) return( false );

#ifdef WIN32
	if( _kbhit() )
#else
#if ENABLE_KBD_POLL_BUG
	linux_enable_kbd_poll();
#endif
	if( linux_query_kbhit() )
#endif
	{
		char c = read_char_stdin();
		if( c == KBD_NULL )	{

			c = read_char_stdin();
			*mode_p = WIN_FUNCT_MODE;
		}
		else
		if( c == KBD_ESC )	{

			c = read_char_stdin();
			if( c == KBD_OPEN_BRK )	{

				c = read_char_stdin();
				*mode_p = LIN_ARROW_MODE;
			}
			else	{

				fprintf( stdout, "<UNK:%d>", (int)c );
				*mode_p = NULL_MODE;
			}
		}
		else
		if( c == KBD_WIN32_ESC_SGN )	{

			c = read_char_stdin();
			*mode_p = WIN_ARROW_MODE;
		}
		else	{
			*mode_p = NULL_MODE;
		}
		*cp = c;
		set = true;
	}
#ifndef WIN32
#if ENABLE_KBD_POLL_BUG
	linux_disable_kbd_poll();
#endif
#endif
	return( set );
}

int commandline_query( 
	char **write_buffer_pp, 
	int *buff_len_p, 
	int *buff_use_p, 
	int *buff_pos_p, 
	bool verbose 
)	{
	
	char c;
	int mode;
	while( keyboard_query_stdin( &c, &mode ) ) {

		if( verbose )	{
			fprintf( stdout, "%c{%d}", 7, (int)c );
			fflush( stdout );
		}
		bool copy = true;

		if( mode == LIN_ARROW_MODE ) { // Linux arrow keys: { A, B, C, D } -> { up, dn, rt, lt }

			int special = NULL_MODE;
			if( c == 'A' )	{
				special = ARROW_UP_MODE;
			}
			else
			if( c == 'B' )	{
				special = ARROW_DN_MODE;
			}
			else
			if( c == 'C' )	{
				special = ARROW_RT_MODE;
			}
			else
			if( c == 'D' )	{
				special = ARROW_LT_MODE;
			}
			return( special );
		}
		else
		if( mode == WIN_ARROW_MODE ) { // Win32 arrow keys: { H, P, M, K } -> { up, dn, rt, lt }

			int special = NULL_MODE;
			if( c == 'H' )	{
				special = ARROW_UP_MODE;
			}
			else
			if( c == 'P' )	{
				special = ARROW_DN_MODE;
			}
			else
			if( c == 'M' )	{
				special = ARROW_RT_MODE;
			}
			else
			if( c == 'K' )	{
				special = ARROW_LT_MODE;
			}
			return( special );
		}
		else
		if( c == 0 )	{
			fprintf( stdout, "<0>" );
			fflush( stdout );
			return( NULL_MODE );
		}
		else
		if( c == KBD_HT )	{	// horizontal tab
			return( TAB_MODE );
		}
		else
		if( ( c == KBD_BS )||( c == KBD_DEL ) )	{ 
		
			// DELETION
#if 1
			if( ( *buff_pos_p ) > 0 )	{
				if( ( *buff_pos_p ) < ( *buff_use_p ) ) {
					edit_char_buffer( *write_buffer_pp, *buff_use_p, *buff_pos_p, 0 );
				}
				else	{
					( *write_buffer_pp )[ *buff_use_p ] = 0;
				}
#else
			if( ( *buff_use_p ) > 0 )	{
				( *write_buffer_pp )[ *buff_use_p ] = 0;
#endif
				( *buff_pos_p )--;
				( *buff_use_p )--;
				fprintf( stdout, "\b \b" );
			}
			else	{
#if 1
				fprintf( stdout, "%c", 7 );
				fprintf( stdout, "_\b" );
#else
				fprintf( stdout, "%c_\b", 7 );
#endif
			}
			fflush( stdout );
			return( NULL_MODE );
		}
		else
		if( ( c == KBD_LF )||( c == KBD_CR )||( c == KBD_NULL ) ) { // END OF LINE
			fprintf( stdout, "\n" ); // line entry complete...
			c = KBD_NULL;
		}
		else	{	
#if ( ! DISABLE_KBD_POLL_ECHO )
#if 1
#if 0
		// TEMPORARY
			int i;
			int n = *buff_use_p;
			for( i=0; i<n; i++ )	{
				fprintf( stdout, "\b" );
			}
			for( i=0; i<n; i++ )	{
				fprintf( stdout, "%c", (*write_buffer_pp)[ i ] );
			}
			fprintf( stdout, "%c", c );
			fflush( stdout );
#else
			fprintf( stdout, "%c", c ); // render character to prompt
#endif
#endif
#endif
		}

		// CHECK CAPACITY
		if( ( *buff_use_p ) >= ( ( *buff_len_p ) - 1 ) )	{ 
			if( ( *buff_len_p ) < 2 )	{
				( *buff_len_p ) = 2;
			}
			else	{
				( *buff_len_p ) = ( *buff_len_p ) * 2 + 1;
			}
			*write_buffer_pp = realloc_char_buffer( *write_buffer_pp, *buff_use_p, *buff_len_p );
		}

		// INSERTION
#if 1
		if( ( *buff_pos_p ) < ( *buff_use_p ) ) {
			if( c != KBD_NULL ) {
				edit_char_buffer( *write_buffer_pp, *buff_use_p, *buff_pos_p, c );
			}
		}
		else	{
			( *write_buffer_pp )[ *buff_pos_p ] = c;
		}
#else
		( *write_buffer_pp )[ *buff_use_p ] = c;
#endif
		if( c == KBD_NULL ) {
			fflush( stdout );
			return( STRING_MODE );
		}
		( *buff_use_p )++;
		( *buff_pos_p )++;
	}
	fflush( stdout );
	return( NULL_MODE );
}

///////////////////////////////////////////////////////////////////////////

srCmdLine::srCmdLine( void )
{
	cmd_buffer = NULL;
	buffer_len = 0;
	buffer_use = 0;
	buffer_pos = 0;
	
	max_cmdlines = 100;
	cmds = new std::list<std::string>;
	iter = cmds->end();
	
#ifndef WIN32
	linux_enable_kbd_poll();
#endif
}

srCmdLine::~srCmdLine( void )
{
	if( cmd_buffer )	{
		delete [] cmd_buffer;
	}
#ifndef WIN32
	linux_disable_kbd_poll();
#endif
}

char* srCmdLine::peek_cmd( void )
{
	return( cmd_buffer );
}

char* srCmdLine::read_cmd( void )
{
	buffer_use = 0;
	buffer_pos = 0;
	return( cmd_buffer );
}

///////////////////////////////////////////////////////////////////////////////

int srCmdLine::realloc_buffer( int len )
{
	char *new_buff = new char[ len ];
	if( new_buff == NULL )	{
		return( CMD_FAILURE );
	}
	if( len < ( buffer_use + 1 ) )	{
		buffer_use = len - 1;
	}
	if( cmd_buffer )	{
		memcpy( new_buff, cmd_buffer, buffer_use );
		delete [] cmd_buffer;
	}
	else	{
		new_buff[ 0 ] = '\0';
	}
	cmd_buffer = new_buff;
	buffer_len = len;
	return( CMD_SUCCESS );
}

///////////////////////////////////////////////////////////////////////////////

#ifndef LinWin_strcmp
#define LinWin_strcmp strcmp
#endif

void srCmdLine::test_prompt( void )	{
	bool quit = false;
	bool verbose = false;
	int buffer_cap = 0;
	char *cmd_buffer = NULL;
	
	fprintf( stdout, "> " ); fflush( stdout );
	
	int buffer_use = 0;
	while( !quit ) {

		int in_mode = commandline_query( &cmd_buffer, &buffer_cap, &buffer_use, &buffer_pos, verbose );
		if( in_mode == STRING_MODE )	{
			char *cmd = read_cmd();

			if( LinWin_strcmp( cmd, "v" ) == 0 )	{
				verbose = true;
			}
			else
			if( LinWin_strcmp( cmd, "s" ) == 0 )	{
				verbose = false;
			}
			else
			if( LinWin_strcmp( cmd, "q" ) == 0 )	{
				quit = true;
			}
			// parse...

			if( !quit ) {
				fprintf( stdout, "> " );
				fflush( stdout );
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////

#if ENABLE_NEW_PROMPT

int srCmdLine::pending_cmd( bool )	{

	int prev_buffer_use = -1;
		
	int in_mode = commandline_query( &cmd_buffer, &buffer_len, &buffer_use, &buffer_pos, false );
	if( in_mode )	{

//printf( "use:%d pos:%d\n", buffer_use, buffer_pos );

		if( in_mode == STRING_MODE )	{

			if( buffer_use!= 0 )	{
				std::stringstream stream;
				for(int i = 0 ; i < buffer_use; i++)
					stream << cmd_buffer[i];

				if( cmds->size() >= max_cmdlines )
					cmds->pop_front();

				cmds->push_back( stream.str() );
				iter = cmds->end();
			}
			fflush( stdout );
			return( TRUE );
		}
			
		prev_buffer_use = buffer_use;
		
		if( in_mode == TAB_MODE )	{
			fprintf( stdout, "%c<T>", 7 ); // bell
	//		fprintf( stdout, "%c", 7 ); // bell


		}
		else
		if( in_mode == ARROW_UP_MODE )	{

			if( !cmds->empty() )	{
				fprintf( stdout, "%c", 7 );
				
#if 0
				int i, n;
				n = buffer_use;
				for( int i = 0 ; i < n; i++ )	{
					fprintf( stdout, "\b" );
				}
				for( int i = 0 ; i < n; i++ )	{
					fprintf( stdout, " " );
				}
				for( int i = 0 ; i < n; i++ )	{
					fprintf( stdout, "\b" );
				}
				fflush( stdout );
#endif

				if( iter != cmds->begin() )	{
					iter--;
				}
//				fprintf( stdout, "> %s", (*iter).c_str() );
				
				memcpy( cmd_buffer, (*iter).c_str(), (*iter).size() );
//				buffer_use = (*iter).size() - 1;
				buffer_use = (*iter).size();
				buffer_pos = buffer_use;

				fprintf( stdout, "%s", cmd_buffer );
			}
		}
		else
		if( in_mode == ARROW_DN_MODE )	{

			if( iter != cmds->end() )	{
				fprintf( stdout, "%c", 7 );
#if 0
				for( int i = 0 ; i < ( buffer_use + 2 ); i++ )	{
					fprintf(stdout, "\b \b" );
				}
				fprintf( stdout, "> " );
#endif

				iter++;
				if( iter != cmds->end() )	{
					fprintf( stdout, "%s", (*iter).c_str() );
					memcpy( cmd_buffer, (*iter).c_str(), (*iter).size() );
					buffer_use = (*iter).size() - 1;
				}
				else	{
					memset( cmd_buffer, 0, buffer_use );
					buffer_use = 0;
				}
				buffer_pos = buffer_use;
			}
		}
		else
		if( in_mode == ARROW_LT_MODE )	{
			
			if( buffer_pos > 0 )	{
				buffer_pos--;
#if 0
				fprintf(stdout, "\b" );
#endif
			}
			else	{
				fprintf( stdout, "%c", 7 );
			}
//			fprintf( stdout, "<L>" );
		}
		else
		if( in_mode == ARROW_RT_MODE )	{

			if( buffer_pos < buffer_use )	{
				buffer_pos++;
#if 0
				int i;
				int n = buffer_use - buffer_pos;
				for( i = 0; i < n; i++ )	{
					fprintf( stdout, "%c", cmd_buffer[ buffer_pos + i ] );
				}
				for( i = 0; i < ( n - 1 ); i++ )	{
					fprintf(stdout, "\b" );	
				}
#endif
			}
			else	{
				fprintf( stdout, "%c", 7 );
			}
//			fprintf( stdout, "<R>" );
		}
		fflush( stdout );
	}

#if 1
	if( prev_buffer_use >= 0 )	{
		printf( "prv:%d use:%d pos:%d\n", prev_buffer_use, buffer_use, buffer_pos );

		int i, n;
		n = prev_buffer_use;
		for( i = 0; i < n; i++ )	{
			fprintf( stdout, "\b" );
		}
		n = buffer_use;
		for( i = 0; i < n; i++ )	{
			fprintf( stdout, "%c", cmd_buffer[ i ] );
		}
		n = buffer_use - buffer_pos;
		for( i = 0; i < n; i++ )	{
			fprintf(stdout, "\b" );	
		}
		fflush( stdout );
	}
	
#endif

	return( FALSE );
}

#else
int srCmdLine::pending_cmd( bool )	{

#ifdef WIN32
	while( _kbhit() )
#else
#if ENABLE_KBD_POLL_BUG
	linux_enable_kbd_poll();
#endif
	while( linux_query_kbhit() )
#endif
		{
		char c = read_char_stdin();

#define ENABLE_TAB_AND_ARROW 1
#if ENABLE_TAB_AND_ARROW

		if( c == '\x09' ) // tab - use auto completion for commands
		{
			
			srHashMapBase* map = NULL;

			// get the current partial command
			std::string partialCommand( cmd_buffer, buffer_use );
			std::string commandPrefix = "";

			// only use tab completion on the first word
			size_t index = partialCommand.find_first_of(" ");
			if( index != std::string::npos )
			{

				// if the command matches 'set', 'print' or 'test' use those maps
				std::string firstToken = partialCommand.substr(0, index);
				if (firstToken == "set")
				{
					map = &mcu.set_cmd_map.getHashMap();
					partialCommand = partialCommand.substr(index + 1);
					commandPrefix = "set ";
				}
				else if (firstToken == "print")
				{
					map = &mcu.print_cmd_map.getHashMap();
					partialCommand = partialCommand.substr(index + 1);
					commandPrefix = "print ";
				}
				else if (firstToken == "test")
				{
					map = &mcu.test_cmd_map.getHashMap();
					partialCommand = partialCommand.substr(index + 1);
					commandPrefix = "test ";
				}
				else
				{
					// transform tabs into a space
					fprintf( stdout, " " );
					cmd_buffer[ buffer_use++ ] = ' ';
					continue;
				}

			}

			// find a match against the current list of commands

			if( !map )
				map = &mcu.cmd_map.getHashMap();
			int numEntries = map->get_num_entries();
			map->reset();
			int numMatches = 0;
			char* key = NULL;
			int numChecked = 0;
			map->next( &key );
			std::vector<std::string> options;

			while( key )
			{
				bool match = false;
				std::string keyString = key;
				numChecked++;
				if( partialCommand.size() <= keyString.size() )
				{
					match = true;
					for( size_t a = 0; a < partialCommand.size() && a < keyString.size(); a++ )
					{
						if( partialCommand[ a ] != keyString[ a ] )
						{
							match = false;
							break;
						}
					}
					if( match )
					{
						options.push_back( keyString );
						numMatches++;
					}
				}
				map->next( &key );
				std::string nextKey = key;
				if( nextKey == keyString )
					break; // shouldn't map.next(key) make key == NULL? This doesn't seem to happen.
			}

			if( numMatches == 1 )
			{
				int numChars = buffer_use;
				std::string final = commandPrefix + options[0] + " ";
				
				
				// insert into command string, udate length
				strcpy( cmd_buffer, final.c_str() );
				buffer_use = final.size();
				
				for( size_t n = numChars; n < final.size(); n++ )
				{
					fprintf( stdout, "%c", final[ n ] );
				}
				continue;
			}
			else 
			if (numMatches > 1)
			{ // more than one match, show the options on the line below

				fprintf( stdout, "\n");
				std::sort( options.begin(), options.end() );
				for( size_t x = 0; x < options.size(); x++ )
				{
					fprintf( stdout, "%s ", options[x].c_str() );
				}
				fprintf( stdout, "\n> ");
				for( int x = 0; x < buffer_use; x++ )	{
					fprintf( stdout, "%c", cmd_buffer[x] );
				}
				continue;
			}
			else 
			if( numMatches == 0 )
			{
				// transform tabs into a space
				fprintf( stdout, " " );
				cmd_buffer[ buffer_use++ ] = ' ';
				continue;
			}
		}
		else 
		if( c == '\x0' || c == '\xE0' ) // non-char flag
		{
			char c_next = read_char_stdin();

			if( c_next == '\x48' ) { // uparrow

				for( int i = 0 ; i < buffer_use; i++ )
					fprintf(stdout, "\b \b" );

				if( !cmds->empty() )
				{
					if( iter != cmds->begin() )	iter--;
					fprintf( stdout, "%s", (*iter).c_str() );
					memcpy( cmd_buffer, (*iter).c_str(), (*iter).size() );
					buffer_use = (*iter).size();
				}
			}

			if( c_next == '\x50' ){ // downarrow?

				for( int i = 0 ; i < buffer_use; i++ )
					fprintf(stdout, "\b \b" );

				if( iter != cmds->end() )
				{
					iter ++;
					if( iter != cmds->end() )
					{
						fprintf( stdout, "%s", (*iter).c_str() );
						memcpy( cmd_buffer, (*iter).c_str(), (*iter).size());
						buffer_use = (*iter).size();
					}
					else
					{
						memset( cmd_buffer, 0, buffer_use);
						buffer_use = 0;
					}
				}
			}
			continue;
		}
#endif

#if 1
		if( buffer_use >= ( buffer_len - 1 ) )	{
//			int err = realloc_buffer( 2 * buffer_len );
			if( buffer_len < 2 )	{
				buffer_len = 2;
			}
			else	{
				buffer_len = buffer_len * 2 + 1;
			}
			cmd_buffer = realloc_char_buffer( cmd_buffer, buffer_use, buffer_len );
		}
#endif

		// check for end...
		if( 
			( c == '\r' )||
			( c == '\n' )||
			( c == '\0' ) )	{
			if( buffer_use!= 0 )
			{
				std::stringstream stream;
				for(int i = 0 ; i < buffer_use; i++)
					stream << cmd_buffer[i];

				if( cmds->size() >= max_cmdlines )
					cmds->pop_front();

				cmds->push_back( stream.str() );
			}

			iter = cmds->end();
			fprintf( stdout, "\n" );
			cmd_buffer[ buffer_use++ ] = '\0';
#ifndef WIN32
#if ENABLE_KBD_POLL_BUG
			linux_disable_kbd_poll();
#endif
#endif
			fflush( stdout );
			return( TRUE );
		}

		if( ( c == '\b' )||( c == KBD_BS )||( c == KBD_DEL ) )	{
			if( buffer_use > 0 )	{
				fprintf( stdout, "\b \b" );
				buffer_use--;
			}
#ifndef WIN32
#if ENABLE_KBD_POLL_BUG
			linux_disable_kbd_poll();
#endif
#endif
			fflush( stdout );
			return( FALSE );
		}

#if ( ! DISABLE_KBD_POLL_ECHO )
		fprintf( stdout, "%c", c );
#endif

		cmd_buffer[ buffer_use++ ] = c;
#if 0
		if( buffer_use == buffer_len )	{
			int err = realloc_buffer( 2 * buffer_len );
//			buffer_len *= 2;
//			cmd_buffer = realloc_char_buffer( cmd_buffer, buffer_use, buffer_len );
		}
#endif
	}

#ifndef WIN32
#if ENABLE_KBD_POLL_BUG
	linux_disable_kbd_poll();
#endif
#endif
	fflush( stdout );
	return( FALSE );
}
#endif

