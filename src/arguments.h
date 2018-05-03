#pragma once
#include <map>
#include <string>
#include <sstream>
#include <cstring>
using namespace std;
//
// very simple command line argument handler
// Huw Lloyd 2017
//
class Arguments
{
	map< string, string> args;

	bool IsArg( char *token )
	{
		return ( strlen(token) > 2 && token[0] == '-' && token[1] == '-' );
	}
	void ProcessArgs( int argc, char *argv[] )
	{
		for ( int i = 1; i < argc; i++ )
		{
			if ( IsArg( argv[i] ) )
			{
				string argument = string(argv[i]+2); // +2 removes the '--'
				string value;
				if ( i < argc-1 && !IsArg(argv[i+1]) )
				{
					value = string(argv[i+1]);
				}
				else
				{
					// write a '1' in here - this is a boolean argument so make it true if found
					value = string("1");
				}
				args[argument] = value;
			}
		}
	}
public:
	Arguments( int argc, char *argv[] )
	{
		ProcessArgs( argc, argv );
	}
	template<class T> T GetArg(const string &arg, const T& defaultValue ) 
	{
		T retVal = defaultValue;
		if (args.find(arg) != args.end())
		{
			stringstream ss(args[arg]);
			ss >> retVal; 
		}
		return retVal;
	}
};
