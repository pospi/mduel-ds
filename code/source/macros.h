/*
 * Marshmallow Duel DS v2
 * Copyright © 2007 Sam Pospischil http://pospi.spadgos.com
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

// started off as some simple macros, has sortof turned into my definitions file now.

//#ifndef __MDDEBUG		//debug mode - attempt log output (slow!)
//#define __MDDEBUG
//#endif

#ifndef __WITHSOUND		//enable or disable sound easily
#define __WITHSOUND
	//#ifndef __WITHMENUTRACK	//<-- i took this out cos it roughly doubled the rom size
	//#define __WITHMENUTRACK
	//#endif
#endif

#ifndef __WITHSAVING	//enable or disable SRAM support
#define __WITHSAVING
#endif

///x screen position (in pixels) for hidden sprites (ie offscreen somewhere)
#define OFFX -50
///y screen position (in pixels) for hidden sprites (ie offscreen somewhere)
#define OFFY -256

#ifdef __MDDEBUG
	///screen y position (in text tiles) to print log messages at
	#define LOGPOS 1
	///screen y position (in text tiles) to print error messages at
	#define ERRORPOS 4
	///text to print out for critical errors
	#define ERRTEXT "ERR"
#endif

#ifndef _MDUELMACROS_
#define _MDUELMACROS_

#include <PA9.h>
#include <string>
using namespace std;

#ifdef __MDDEBUG
	#include <fstream>
	#include <fat.h>
	#define LOGFILE "mduelLog.txt"
#endif

/**
 * Contains generic utility functions that many other places use.
 * This actually began as a debugging namespace, but other global functions have found their
 * way here as well.
 * 
 * @author pospi
 */
namespace macros
{
		#ifdef __MDDEBUG
		/**
		 * Output some debugging information. If this is a critical debug (as evidenced by ERRORPOS
		 * and ERRTEXT), then the game is paused so that it can be read before continuing. This function
		 * tries to output to a file for flashcarts that support it. Otherwise, the message is just printed
		 * on the screen. Be forewarned - file output seriously slows down execution of the game.
		 * 
		 * @param	className	the name of the class which is outputting the text
		 * @param	outText	the messag to output
		 * @param	yPos	the y screen position to output the text on
		 */
		inline string debugMessage(string className, string outText, u8 yPos = LOGPOS)
		{
			string ret = className+string(": ")+outText+string(" \n");
			
			if (yPos == LOGPOS)
			{
				FILE* logFile = fopen(LOGFILE, "a+");
				if (logFile)	//this probably only works for supported flashcarts, so just draw onscreen if failed.
					fprintf(logFile, ret.c_str());
				else
					PA_OutputText(0, 0, 0, const_cast<char *>(ret.c_str()));
				fclose(logFile);
			} else {
				PA_OutputText(0, 0, yPos, const_cast<char *>(ret.c_str()));
				//pause for critical errors to be read
				if (className == ERRTEXT && yPos == ERRORPOS)
					PA_WaitFor(Stylus.Newpress);
			} 
				
			return ret;
		}
		#endif
		
		/**
		 * Parse player names string and return player name for a specific number.
		 * 
		 * @param	playerNum	the player number to retrieve the name for (between 1 and 6)
		 * @param	playerNames	the string of player names to interpret
		 * @param	leftPadding	if true, will pad player names 1-3 from the right, and names 4-6 from the left.
		 */
		inline string getPlayerName(u8 playerNum, string playerNames, bool leftPadding = true)
		{
			playerNum -= 1;	//1 indexed players
			string ret;
			for (u8 i=playerNum*8; i < (playerNum+1)*8; ++i)
				ret += playerNames[i];
			
			if (leftPadding)
			{
				if (playerNum >= 4)	//need left padding
				{
					while (ret.at(ret.length()-1) == ' ')
					{
						for (s8 i=ret.length()-2; i>=0; i--)
							ret.at(i+1) = ret.at(i);
						ret.at(0) = ' ';
					}
				}
			}
			return ret;
		}
};

#endif
