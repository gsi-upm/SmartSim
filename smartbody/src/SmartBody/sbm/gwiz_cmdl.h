#ifndef GWIZ_CMDL_H
#define GWIZ_CMDL_H

#include <sb/SBTypes.h>

#include	<string>
#include	<list>

namespace gwiz{

class line_editor	{
	
	public:

		typedef std::string (*callback_fp)( std::string );

		line_editor( void ) {
			pos = 0;
			prev_pos = 0;
			tab_callback_fp = NULL;
		}
		virtual ~line_editor( void ) {}

		inline std::string *
		read( std::string *io_p, bool clear = true ) {

			int len = line.length();
			if( len ) {
				*io_p = line;
				if( clear ) {
					prev_len = len;
					line.erase();
					pos = 0;
				}
			}
			else	{
				*io_p = "";
			}
			return( io_p );
		}

		inline std::string 
		read( bool clear = true ) {

			std::string tmp;
			std::string *tmp_p = read( &tmp, clear ); // for illustration
			return( *tmp_p );
		}

		inline void 
		write( const std::string *in_p, bool append = false ) {

			prev_len = line.length();
			prev_pos = pos;
			if( append )	{
				line += ( *in_p );
			}
			else	{
				line = ( *in_p );
			}
			pos = line.length();

			render_line( false ); // false: diagnostic ignored
			prev_pos = pos;
		}

		inline void 
		write( std::string in, bool append = false ) {

			write( &in, append );
		}

		void set_callback( callback_fp fp )	{
			tab_callback_fp = fp;
		}

		int edit( char key, int key_mode, bool v_tab = false );
		int query( bool v_key = false, bool v_tab = false );

		void render_stats( void );
		SBAPI void render_line( bool diagnostic = false );

		std::string line;
		int prev_len;

		int pos;
		int prev_pos;

	private:
		callback_fp tab_callback_fp;

		bool query_stdin( char *cp, int *mode_p, bool v = false );
};

class cmdl : public line_editor	{
	
	public:

		SBAPI cmdl( void );
		SBAPI virtual ~cmdl( void );

		void set_verbose( bool key, bool line, bool tab, bool diagnostic )	{
			verbose_key = key;
			verbose_line = line;
			verbose_tab = tab;
			diagnostic_chars = diagnostic;
		}

		SBAPI bool pending( const std::string& prompt = "> " );

		std::string test_query( std::string src_str = "" );
		std::string test_prompt( std::string src_str = "" );
		SBAPI std::string test_editor( std::string src_str = "" );

		static std::string query_callback( std::string src_str )	{
			cmdl cl; return( cl.test_query( src_str ) );
		}
		static std::string prompt_callback( std::string src_str ) {
			cmdl cl; return( cl.test_prompt( src_str ) );
		}
		static std::string editor_callback( std::string src_str )	{
			cmdl cl; return( cl.test_editor( src_str ) );
		}

		SBAPI void render_prompt( std::string prompt = "> " );

	private:

		bool verbose_key;
		bool verbose_line;
		bool verbose_tab;
		bool diagnostic_chars;

		int nest_level;
//		bool blocking;

		void bell( void )	{
			fprintf( stdout, "%c", 7 );
		}

		unsigned int max_cmdlines;
		std::list< std::string >			*cmds;
		std::list< std::string >::iterator	iter;
};

} // namespace gwiz

#endif
