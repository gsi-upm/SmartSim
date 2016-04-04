//
// Copyright (c) 2009-2010 Shawn Singh, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __UTIL_GENERIC_EXCEPTION_H__
#define __UTIL_GENERIC_EXCEPTION_H__

/// @file GenericException.h
/// @brief Defines the Util::GenericException class, the base Exception used by %SteerLib.

#include <string>
#include <exception>
#include "Globals.h"

namespace Util {
 
	/**
	 * @brief The base exception type used by %SteerLib.
	 *
	 * This class is the base exception type all of %SteerLib.  It allows you\
	 * to specify a std::string error message as a constructor.
	 *
	 * <h3> Examples </h3>
	 * Example of throwing a GenericException:
	 * \code
	 * throw GenericException("Arg number " + toString(argIndex) + " is an unrecognized option.");
	 * \endcode
	 *
	 * Example of catching GenericException objects and standard exceptions at the toplevel main() function:
	 * \code
	 * int main( int argc, char ** argv )
	 * {
	 *     try {
	 *         // put all your code here
	 *     }
	 *     catch (std::exception &e) {
	 *         // print the message contained in the exception.
	 *         std::cerr << "\n\nERROR: exception caught in main:\n" << e.what() << "\n";
	 *         return 1;  // treat exception as error
	 *     }
	 *     return 0; // no errors occurred
	 * }
	 * \endcode
	 *
	 * @see
	 *  - The toString() helper function converts other data types into strings, nice for dynamic helpful
	 *    error messages.
	 */
	class GenericException : public std::exception
	{
	public:
		/// When throwing the exception, a useful error message must be specified.
		GenericException(const std::string & errorMessage) throw() { _errorMessage = errorMessage; }
		~GenericException() throw() { }

		/// Returns a C string containing the error message of the exception.
		virtual const char * what() const throw() { return _errorMessage.c_str(); }

	protected:
		std::string _errorMessage;
	};

} // end namespace Util

#endif
