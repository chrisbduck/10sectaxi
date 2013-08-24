//------------------------------------------------------------------------------
// settings: A class to retrieve global settings from a data file.
//
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

//------------------------------------------------------------------------------

namespace Settings
{
	// Parameters are all retrieved from the current group by default - and the default group is "" (none, or empty).
	// A group name can be specified when retrieving a parameter, using the form "group/param".  This is always an
	// absolute path, as there is only one level of groups at present.
	
	int getInt(const std::string& lrName);
	float getFloat(const std::string& lrName);
	const std::string& getString(const std::string &lrName);
	
	bool setGroup(const std::string& lrName);		// returns false (and traces) if the group does not exist
}

//------------------------------------------------------------------------------

#endif // SETTINGS_H
