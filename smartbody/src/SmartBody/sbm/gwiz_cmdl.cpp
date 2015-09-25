#include	<sys/types.h>
#include	<stdio.h>
#include    <stdlib.h>
#include	<string.h>
#include	<iostream>
#include	<list>
#include	<sstream>

#ifdef WIN32
#include	<windows.h>
#include	<conio.h>
#else
#include	<fcntl.h>
#include	<termios.h>
#include	<unistd.h>
#include	<sys/ioctl.h>
#include	<sys/time.h>
#endif

#include "gwiz_cmdl.h"

#define ENABLE_LINUX_STDIN_NONBLOCK 	0
#define ENABLE_KBD_POLL_BUG 			0

///////////////////////////////////////////////////////////////////////////

int GWIZ_cmdl_count_global( int inc = 0 )	{

	static int c = 0;
	if( inc > 0 )	{
		c++;
	}
	else
	if( inc < 0 )	{
		c--;
	}
	if( c < 0 ) {
		fprintf( stderr, "GWIZ_cmdl_count_global ERR: negative count: %d\n", c );
	}
	return( c );
}

int GWIZ_query_console_width( void )	{
#ifdef WIN32
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo; 
	HANDLE hstdout;
	hstdout = GetStdHandle( STD_OUTPUT_HANDLE );
	GetConsoleScreenBufferInfo( hstdout, &csbiInfo );
//	fprintf( stdout, "Size: %d by %d", csbiInfo.dwSize.X, csbiInfo.dwSize.Y );
//	fprintf( stdout, "Pos: %d by %d", csbiInfo.dwCursorPosition.X, csbiInfo.dwCursorPosition.Y );
	return( (int)( csbiInfo.dwSize.X ) );
#else
    struct winsize w;
    ioctl( 0, TIOCGWINSZ, &w );
//	fprintf( stdout, "lines %d columns %d\n", w.ws_row, w.ws_col );
	return( (int)( w.ws_col ) );
#endif
}

#ifndef WIN32

inline void GWIZ_linux_enable_kbd_poll( void )	{

	struct termios attr;
	tcgetattr( STDIN_FILENO, &attr );
	attr.c_lflag &= ~( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSANOW, &attr );

#if ENABLE_LINUX_STDIN_NONBLOCK
	int flags = fcntl( STDIN_FILENO, F_GETFL, 0 );
	flags |= O_NONBLOCK;
	fcntl( STDIN_FILENO, F_SETFL, flags );
#endif
}

inline void GWIZ_linux_disable_kbd_poll( void )	{

	struct termios attr;
	tcgetattr( STDIN_FILENO, &attr );
	attr.c_lflag |= ( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSANOW, &attr );

#if ENABLE_LINUX_STDIN_NONBLOCK
	int flags = fcntl( STDIN_FILENO, F_GETFL, 0 );
	flags &= ~( O_NONBLOCK );
	fcntl( STDIN_FILENO, F_SETFL, flags );
#endif
}

inline bool GWIZ_linux_query_kbhit( void )	{

	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	fd_set rdfs;
	FD_ZERO( &rdfs );
	FD_SET( STDIN_FILENO, &rdfs );
	
	int ret = select( STDIN_FILENO + 1, &rdfs, NULL, NULL, &tv );
	if( ret > 0 )	{
		int set = FD_ISSET( STDIN_FILENO, &rdfs );
		if( set > 0 )	{
			return( true );
		}
#if 0
		fprintf( stdout, "GWIZ_linux_query_kbhit WARN: select() returned %d\n", ret );
#endif
	}
#if 0
	int set = FD_ISSET( STDIN_FILENO, &rdfs );
	if( set != 0 )	{
		fprintf( stdout, "GWIZ_linux_query_kbhit WARN: select:%d FD_ISSET:%d\n", ret, set );
	}
#endif
	return( false );
}
#endif

inline int GWIZ_query_kbhit_stdin( void )	{

#ifdef WIN32
	return( _kbhit() );
#else
	return( GWIZ_linux_query_kbhit() );
#endif
}

inline char GWIZ_read_char_stdin( bool verbose = false )	{

#ifdef WIN32
	char c = (char)_getch();
#else
	char c = (char)getc( stdin );
#endif
	if( verbose ) fprintf( stdout, "<%d>\n", (int)c );
	return( c );
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
#define EDIT_MODE		2
#define ARROW_UP_MODE	3
#define ARROW_DN_MODE	4
#define ARROW_LT_MODE	5
#define ARROW_RT_MODE	6
#define FUNCTION_MODE	7

/*
	ANSI...
	http://cboard.cprogramming.com/cplusplus-programming/74EOF567-do-arrow-keys-have-two-ascii-values.html

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

///////////////////////////////////////////////////////////////////////////

bool gwiz::line_editor::query_stdin( char *cp, int *mode_p, bool v )	{

// NOTE: split into query()/read(): WIN32?

	if( ( cp == NULL )||( mode_p == NULL ) ) return( false );

	bool set = false;
	if( GWIZ_query_kbhit_stdin() )	{
	
		char c = GWIZ_read_char_stdin( v );
		if( c == -1 ) { // EOF: for non-blocking read
#if ( !ENABLE_LINUX_STDIN_NONBLOCK )
			fprintf( stdout, "<E>" ); // screws up cursor position...
#endif
			return( false );
		}
		if( c == KBD_NULL )	{

			c = GWIZ_read_char_stdin( v );
			*mode_p = WIN_FUNCT_MODE;
		}
		else
		if( c == KBD_ESC )	{

			c = GWIZ_read_char_stdin( v );
			if( c == KBD_OPEN_BRK )	{

				c = GWIZ_read_char_stdin( v );
				*mode_p = LIN_ARROW_MODE;
			}
			else	{

				fprintf( stdout, "<UNK:%d>\n", (int)c ); // reset pos?
				*mode_p = NULL_MODE;
			}
		}
		else
		if( c == KBD_WIN32_ESC_SGN )	{

			c = GWIZ_read_char_stdin( v );
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
	GWIZ_linux_disable_kbd_poll();
#endif
#endif
	return( set );
}

int gwiz::line_editor::edit( char key, int key_mode, bool v_tab )	{

	prev_len = line.length();
	prev_pos = pos;

	if( key_mode == LIN_ARROW_MODE ) { // Linux arrow keys: { A, B, C, D } -> { up, dn, rt, lt }

		int special = NULL_MODE;
		if( key == 'A' )	{
			special = ARROW_UP_MODE;
		}
		else
		if( key == 'B' )	{
			special = ARROW_DN_MODE;
		}
		else
		if( key == 'C' )	{
			special = ARROW_RT_MODE;
		}
		else
		if( key == 'D' )	{
			special = ARROW_LT_MODE;
		}
		return( special );
	}
	if( key_mode == WIN_ARROW_MODE ) { // Win32 arrow keys: { H, P, M, K } -> { up, dn, rt, lt }

		int special = NULL_MODE;
		if( key == 'H' )	{
			special = ARROW_UP_MODE;
		}
		else
		if( key == 'P' )	{
			special = ARROW_DN_MODE;
		}
		else
		if( key == 'M' )	{
			special = ARROW_RT_MODE;
		}
		else
		if( key == 'K' )	{
			special = ARROW_LT_MODE;
		}
		return( special );
	}

	if( key == 0 )	{
		fprintf( stdout, "<0>" ); // screws up cursor position...
		return( NULL_MODE );
	}
	if( key == KBD_HT )	{	// horizontal tab
		if( tab_callback_fp )	{

			fprintf( stdout, "\n" );
			std::string ret_str = tab_callback_fp( line );

			if( ret_str == line ) {
				if( v_tab )	{
					fprintf( stdout, "< unchanged >" );
				}
				fprintf( stdout, "\n" );
			}
			else	{
				if( v_tab )	{
					fprintf( stdout, "< edited: '%s':='%s' >", line.c_str(), ret_str.c_str() );
				}
				fprintf( stdout, "\n" );
				prev_len = line.length();
				line = ret_str;
			}
		}
		else	{
			fprintf( stdout, "<tab>\n" );
			render_stats();
		}
		return( FUNCTION_MODE ); // EDIT_MODE?
	}
	if( ( key == KBD_BS )||( key == KBD_DEL ) )	{ 

		// DELETION
		if( pos > 0 )	{
			line.erase( pos - 1, 1 );
			pos--;
		}
		else	{
			fprintf( stdout, "%c", 7 ); // bell();
		}
		return( EDIT_MODE );
	}
	if( ( key == KBD_LF )||( key == KBD_CR )||( key == KBD_NULL ) ) { // END OF LINE

		fprintf( stdout, "\n" ); // line entry complete...
		return( STRING_MODE );
	}

	// INSERTION
	line.insert( pos, 1, key );
	pos++;
	return( EDIT_MODE );
}

int gwiz::line_editor::query( bool v_key, bool v_tab )	{

	char key;
	int key_mode;
	if( query_stdin( &key, &key_mode, v_key ) ) {
		
		return( edit( key, key_mode, v_tab ) );
	}
	return( NULL_MODE );
}

void gwiz::line_editor::render_stats( void ) {

	fprintf( stdout, "=< [%d:%d]:=[%d:%d] '%s' >\n", 
		prev_pos,
		(int)( prev_len ),
		pos, 
		(int)( line.length() ),
		line.c_str() 
	);
	prev_pos = 0;
}

//void gwiz::line_editor::render_line( bool diagnostic, bool partial_redraw = false ) {
void gwiz::line_editor::render_line( bool diagnostic ) {
	int i, n;

//	int console_wid = GWIZ_query_console_width();

	// rewind
	n = prev_pos;
//	n = prev_pos % console_wid;
	for( i = 0; i < n; i++ )	{
		fprintf( stdout, "\b" );
	}
	// clear
	n = prev_len;
//	n = prev_len % console_wid;
	for( i = 0; i < n; i++ )	{
		if( diagnostic )	{
			fprintf( stdout, "." );
		}
		else	{
			fprintf( stdout, " " );
		}
	}
	// rewind
	for( i = 0; i < n; i++ )	{
		fprintf( stdout, "\b" );
	}
	// rewrite
	n = line.length();
//	n = line.length() % console_wid;
	for( i = 0; i < n; i++ )	{
		if( diagnostic && ( line[ i ] == ' ' ) )	{
			fprintf( stdout, "_" );
		}
		else	{
			fprintf( stdout, "%c", line[ i ] );
		}
	}
	// rewind
	n = line.length() - pos;
//	n = line.length() - pos;
	for( i = 0; i < n; i++ )	{
		fprintf( stdout, "\b" );
	}
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

gwiz::cmdl::cmdl( void )	{
	
	verbose_key = false;
	verbose_line = false;
	verbose_tab = false;
	diagnostic_chars = false;

//	blocking = false;

	max_cmdlines = 100;
	cmds = new std::list< std::string >;
	iter = cmds->end();
	
	nest_level = GWIZ_cmdl_count_global();
	int c = GWIZ_cmdl_count_global( 1 );
#ifndef WIN32
	if( c == 1 )	{
		GWIZ_linux_enable_kbd_poll(); // move to line_editor?
	}
#endif
}

gwiz::cmdl::~cmdl( void )	{

	int c = GWIZ_cmdl_count_global( -1 );
#ifndef WIN32
	if( c == 0 )	{
		GWIZ_linux_disable_kbd_poll();
	}
#endif
}

void gwiz::cmdl::render_prompt( std::string prompt ) {

	for( int i=0; i<nest_level; i++ )	{
		fprintf( stdout, "+" );
	}
	if( diagnostic_chars )	{
		fprintf( stdout, "^= " );
	}
	else	{
		if( prompt != "" )	{
			fprintf( stdout, "%s", prompt.c_str() );
		}
		else	{
			fprintf( stdout, "<> " );
		}
	}
}

///////////////////////////////////////////////////////////////////////////

bool gwiz::cmdl::pending( const std::string& prompt )	{

	int mode = query( verbose_key, verbose_tab );
	fflush( stdout );

	if( mode )	{

		if( mode == STRING_MODE )	{

			int len = line.length();
			if( len )	{
				if( cmds->size() >= max_cmdlines )	{
					cmds->pop_front();
				}
				cmds->push_back( line );
				iter = cmds->end();
			}

			render_prompt( prompt );
			return( true );
		}
		if( mode == FUNCTION_MODE )	{

			pos = line.length();
			render_prompt( prompt );
			prev_pos = 0;
		}
		else	{

			if( !cmds->empty() )	{

				if( mode == ARROW_UP_MODE )	{

					if( iter == cmds->begin() )	{
						bell();
					}
					else	{
						iter--;
						line = (*iter);
						pos = line.length();
					}
				}
				else
				if( mode == ARROW_DN_MODE )	{

					if( iter == cmds->end() )	{
						bell();
					}
					else	{

						iter++;
						if( iter == cmds->end() )	{
							line.erase();
							pos = 0;
						}
						else	{
							line = (*iter);
							pos = line.length();
						}
					}
				}
			}

			if( !line.empty() )	{

				if( mode == ARROW_LT_MODE )	{

					if( pos > 0 )	{
						pos--;
					}
					else	{
						bell();
					}
				}
				else
				if( mode == ARROW_RT_MODE )	{

					if( pos < (int)line.length() )	{
						pos++;
					}
					else	{
						bell();
					}
				}
			}
		}

		if( verbose_line ) {
			render_stats();
			render_prompt( prompt );
		}

		render_line( diagnostic_chars );
		fflush( stdout );
	}
	return( false );
}

///////////////////////////////////////////////////////////////////////////////

std::string gwiz::cmdl::test_query( std::string src_str )	{
	bool quit = false;
	std::string prompt( "q> " );

	set_callback( prompt_callback );
	render_prompt( prompt );
	write( src_str );

	while( !quit ) {

		int mode = query( verbose_key, verbose_tab );
		if( mode )	{

			if( verbose_line ) {
				render_stats();
//				render_prompt( prompt );
			}
			if( mode == STRING_MODE )	{

				std::string str = read();
				
				if( str == "t" )	{

					render_prompt( prompt );
					write( std::string( "test" ) ); // ... render_line ...
				}
				else
				if( str == "k" )	{
					verbose_key = !verbose_key;
					render_prompt( prompt );
				}
				else
				if( str == "l" )	{
					verbose_line = !verbose_line;
					render_prompt( prompt );
				}
				else
				if( str == "t" )	{
					verbose_tab = !verbose_tab;
					render_prompt( prompt );
				}
				else
				if( str == "d" )	{
					diagnostic_chars = !diagnostic_chars;
					render_prompt( prompt );
				}
				
				else
				if( str[ 0 ] == 'q' )	{
					quit = true;
				}
				else	{
					render_prompt( prompt );
				}
			}
			else	{

				if( mode == FUNCTION_MODE )	{

					pos = line.length();
					render_prompt( prompt );
					prev_pos = 0;
					render_line( diagnostic_chars );
				}
				else
				if( mode == ARROW_UP_MODE )	{
					if( verbose_line )	{
						render_prompt( prompt );
					}
					write( std::string( "<U>" ), true ); // ... render_line ...
				}
				else
				if( mode == ARROW_DN_MODE )	{
					if( verbose_line )	{
						render_prompt( prompt );
					}
					write( std::string( "<D>" ), true ); // ... render_line ...
				}
				else
				if( mode == ARROW_LT_MODE )	{
					if( verbose_line )	{
						render_prompt( prompt );
					}
					write( std::string( "<L>" ), true ); // ... render_line ...
				}
				else
				if( mode == ARROW_RT_MODE )	{
					if( verbose_line )	{
						render_prompt( prompt );
					}
					write( std::string( "<R>" ), true ); // ... render_line ...
				}
				else {
					if( verbose_line )	{
						render_prompt( prompt );
					}
					render_line( diagnostic_chars );
				}
				if( verbose_line )	{
//					render_prompt( prompt );
				}
			}
		}
		fflush( stdout );
	}
	
	fprintf( stdout, "<q:exit>" ); 
	fprintf( stdout, "\n" ); 
	return( std::string( "q" ) );
}

std::string gwiz::cmdl::test_prompt( std::string src_str )	{
	bool quit = false;
	std::string prompt( "P> " );

	set_callback( editor_callback );
	render_prompt( prompt );
	write( src_str );

	while( !quit ) {
		if( pending( prompt ) )	{

			std::string str = read();

			if( str == "T" )	{
				write( std::string( "test" ) );
			}
			else
			if( str == "k" )	{
				verbose_key = !verbose_key;
			}
			else
			if( str == "l" )	{
				verbose_line = !verbose_line;
			}
			else
			if( str == "t" )	{
				verbose_tab = !verbose_tab;
			}
			else
			if( str == "d" )	{
				diagnostic_chars = !diagnostic_chars;
			}
			else
			if( str[ 0 ] == 'q' )	{
				quit = true;
			}
		}
	}
	
	fprintf( stdout, "<P:exit>" ); 
	fprintf( stdout, "\n" ); 
	return( std::string( "q" ) );
}

std::string gwiz::cmdl::test_editor( std::string src_str )	{

	bool edit = true;
	bool edited = false;
	bool quit = false;

	std::string new_str = src_str;
	std::string menu( "{restore/cont/save}? r|c|[save]" );
	std::string prompt( "E: " );

	fprintf( stdout, "<< Mini-edit: '%s':\n", src_str.c_str() );

	set_callback( query_callback );
	render_prompt( prompt );
	write( src_str );

	while( !quit ) {
		if( pending( prompt ) )	{

			std::string str = read();

			if( edit )	{

				fprintf( stdout, ">< '%s':='%s' %s: ", src_str.c_str(), str.c_str(), menu.c_str() );
				new_str = str;
				edit = false;
				edited = true;
			}
			else
			if( edited )	{
				if( str == "r" )	{

					write( src_str );
					edited = false;
					edit = true;
				}
				else
				if( str == "c" )	{

					write( new_str );
					edit = true;
				}
				else	{
					if( edited )	{
						src_str = new_str;
					}
					quit = true;
				}
			}
			else	{
				quit = true;
			}
		}
		fflush( stdout );
	}

	fprintf( stdout, ">< Mini-exit: '%s' >>", src_str.c_str() ); 
	fprintf( stdout, "\n" ); 
	return( src_str );
}
