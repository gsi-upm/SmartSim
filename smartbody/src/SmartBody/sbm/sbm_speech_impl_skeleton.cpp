/*
 *  sbm_speech_impl_skeleton.cpp - part of SmartBody-lib
 *  Copyright (C) 2008  University of Southern California
 *
 *  SmartBody-lib is free software: you can redistribute it and/or
 *  modify it under the terms of the Lesser GNU General Public License
 *  as published by the Free Software Foundation, version 3 of the
 *  license.
 *
 *  SmartBody-lib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  Lesser GNU General Public License for more details.
 *
 *  You should have received a copy of the Lesser GNU General Public
 *  License along with SmartBody-lib.  If not, see:
 *      http://www.gnu.org/licenses/lgpl-3.0.txt
 *
 *  CONTRIBUTORS:
 *      Andrew n marshall, USC
 */

#include "sbm_speech_impl_skeleton.hpp"  // Replace with actual implementation header

using namespace SmartBody;

// Constructor / Destructor
SpeechImplementation::SpeechImplementation() {
	// TODO when fields are added
}

SpeechImplementation::~SpeechImplementation() {
	// TODO when fields are added
}


//  Override SpeechInterface methods (see sbm_speech.hpp)

/**
 *  Requests audio for a speech by agentName as specified in XML node.
 *  If node is a DOMElement, use the child nodes.
 *  If node is a text node, is the value a string.
 *  If node is an attribute, use the attribute value as a string.
 *  Returns a unique request identifier.
 */
RequestId SpeechImplementation::requestSpeechAudio( const char* agentName, const DOMNode* node, const char* callbackCmd ) {
	// TODO
	return 0; // increment per request
}

/**
 *  Requests audio for a speech char[] text by agentName.
 *  Returns a unique request identifier.
 */
RequestId SpeechImplementation::requestSpeechAudio( const char* agentName, const char* text, const char* callbackCmd ) {
	// TODO
	return 0; // increment per request
}

/**
 *  If the request has been processed, returns the time ordered vector 
 *  of VisemeData for the requestId.  Otherwise return NULL.
 *
 *  Visemes in this list are actually morph targets, and multiple
 *  visemes with different weights can be added together.  Because of
 *  this, the returned viseme list should include zero weighted
 *  VisemeData instances of when to cancel previous visemes (change
 *  of viseme, and end of words).
 */
//const std::vector<VisemeData *>* SpeechImplementation::getVisemes( RequestId requestId ) {
//	// TODO
//	return NULL;
//}

/**
 *  Returns the sbm command used to play the speech audio.
 */
char* SpeechImplementation::getSpeechPlayCommand( RequestId requestId ) {
	// TODO
	return NULL;
}

/**
 *  Returns the sbm command used to stop the speech audio.
 */
char* SpeechImplementation::getSpeechStopCommand( RequestId requestId ) {
	// TODO
	return NULL;
}

/**
 *  Returns the filename of the audio.
 */
char* SpeechImplementation::getSpeechAudioFilename( RequestId requestId ) {
	// TODO
	return NULL;
}

/**
 *  Returns the timing for a synthesis bookmark,
 *  or -1 if the markId is not recognized.
 */
float SpeechImplementation::getMarkTime( RequestId requestId, const XMLCh* markId ) {
	// TODO
	return -1;
}

/**
 *  Signals that requestId processing is complete and its data can be 
 *  discarded.
 */
void SpeechImplementation::requestComplete( RequestId requestId ) {
	// TODO
}
