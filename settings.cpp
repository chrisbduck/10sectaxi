//------------------------------------------------------------------------------
// settings: A class to retrieve global settings from a data file.
//
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#include "settings.h"

#include "useful.h"
#include <fstream>
#include <unordered_map>

//------------------------------------------------------------------------------
// Local class SettingsManager
//------------------------------------------------------------------------------

namespace
{
	const std::string kEmptyString;
	
	
	class SettingsManager
	{
	public:
		SettingsManager(const std::string& lrFileName);
		~SettingsManager();
		
		static SettingsManager& instance();
		
		int getInt(const std::string& lrName) const;
		float getFloat(const std::string& lrName) const;
		const std::string& getString(const std::string& lrName) const;
		std::vector<int> getIntVector(const std::string& lrName) const;
		std::vector<float> getFloatVector(const std::string& lrName) const;
		std::vector<std::string> getStringVector(const std::string& lrName) const;
		
		bool setGroup(const std::string& lrName);		// returns false (and traces) if the group does not exist
		
	private:
		
		typedef std::unordered_map<std::string, std::string> SettingsGroup;
		std::unordered_map<std::string, SettingsGroup*> mGroupMap;
		
		std::string		mCurrentGroupName;
		SettingsGroup*	mpCurrentGroup;
		
		SettingsGroup* getGroup(const std::string& lrName, const char* lpErrorFormat = "Group \"%s\" not found\n") const;
		
	};
	
	//------------------------------------------------------------------------------
	
	SettingsManager::SettingsManager(const std::string& lrFileName)
	{
		printf("Reading settings file '%s'\n", lrFileName.c_str());
		std::ifstream lSrc(lrFileName);
		if (!lSrc.is_open())
		{
			printf("Error opening file\n");
			return;
		}
		
		// Create a group for the unnamed entries
		SettingsGroup* lpDefaultGroup = new SettingsGroup;
		mpCurrentGroup = lpDefaultGroup;
		mGroupMap[mCurrentGroupName] = mpCurrentGroup;
		
		char lLineBuf[256];
		while (!lSrc.eof())
		{
			lSrc.getline(lLineBuf, sizeof(lLineBuf));
			std::string lLine(lLineBuf);
			
			// Remove any comment
			size_t lCommentPos = lLine.find('#');
			if (lCommentPos != std::string::npos)
				lLine.erase(lCommentPos, lLine.length());
			
			// Strip what's left, and skip the line if it's now empty
			strip(lLine);
			if (lLine.empty())
				continue;
			
			// Group
			if (*lLine.begin() == '[' && *(lLine.end() - 1) == ']')
			{
				mCurrentGroupName = stripped(lLine.substr(1, lLine.length() - 2));
				mpCurrentGroup = getGroup(mCurrentGroupName, nullptr);
				if (mpCurrentGroup != nullptr)
					;//printf("Switching to group: %s\n", mCurrentGroupName.c_str());
				else
				{
					//printf("Creating group: %s\n", mCurrentGroupName.c_str());
					mpCurrentGroup = new SettingsGroup;
					mGroupMap[mCurrentGroupName] = mpCurrentGroup;
					continue;
				}
			}
			
			// Setting
			size_t lEqualsPos = lLine.find('=');
			if (lEqualsPos == std::string::npos)
			{
				printf("Unrecognised line: \"%s\"\n", lLine.c_str());
				continue;
			}
			std::string lName = stripped(lLine.substr(0, lEqualsPos - 1));
			std::string lValue = stripped(lLine.substr(lEqualsPos + 1, lLine.length()));
			if (lName.empty() || lValue.empty())
			{
				printf("Invalid setting: \"%s\" = \"%s\"\n", lName.c_str(), lValue.c_str());
				continue;
			}
			//printf("Setting \"%s\" to \"%s\"\n", lName.c_str(), lValue.c_str());
			(*mpCurrentGroup)[lName] = lValue;
		}
		
		setGroup("");		// make sure we start on the default group, as far as users are concerned
		
		printf("Successfully loaded settings file '%s'.\n", lrFileName.c_str());
	}
	
	//------------------------------------------------------------------------------
	
	SettingsManager::~SettingsManager()
	{
		for (auto liGroup = mGroupMap.begin(); liGroup != mGroupMap.end(); ++liGroup)
			delete liGroup->second;
	}
	
	//------------------------------------------------------------------------------
	
	SettingsManager& SettingsManager::instance()
	{
		static SettingsManager sMgr("data/settings.cfg");
		return sMgr;
	}
	
	//------------------------------------------------------------------------------
	
	int SettingsManager::getInt(const std::string& lrName) const
	{
		const std::string& lrValue = getString(lrName);
		return atoi(lrValue.c_str());
	}
	
	//------------------------------------------------------------------------------
	
	float SettingsManager::getFloat(const std::string& lrName) const
	{
		const std::string& lrValue = getString(lrName);
		return atof(lrValue.c_str());
	}
	
	//------------------------------------------------------------------------------
	
	const std::string& SettingsManager::getString(const std::string& lrName) const
	{
		std::string lKeyName = lrName;
		std::string lGroupName = mCurrentGroupName;
		
		// Override the current group if there's a slash in the name
		SettingsGroup* lpGroup = mpCurrentGroup;
		size_t lSlashPos = lrName.find('/');
		if (lSlashPos != std::string::npos && lSlashPos > 0)
		{
			lGroupName = lrName.substr(0, lSlashPos);
			lpGroup = getGroup(lGroupName);
			if (lpGroup == nullptr)
				return kEmptyString;
			lKeyName = lrName.substr(lSlashPos + 1, lrName.length());
		}
		
		// Look for the key in the group we found
		auto liEntry = lpGroup->find(lKeyName);
		if (liEntry == lpGroup->end())
		{
			/*printf("Key \"%s\" not found in group \"%s\".\n", lKeyName.c_str(), lGroupName.c_str());
			printf("List of keys:\n");
			for (auto liEntry: *lpGroup)
				printf("    \"%s\"\n", liEntry.second.c_str());*/
			return kEmptyString;
		}
		
		return liEntry->second;
	}
	
	//------------------------------------------------------------------------------
	
	std::vector<std::string> SettingsManager::getStringVector(const std::string& lrName) const
	{
		return split(getString(lrName));
	}
	
	//------------------------------------------------------------------------------
	
	std::vector<int> SettingsManager::getIntVector(const std::string& lrName) const
	{
		std::vector<int> lOutput;
		
		const std::string& lrText = getString(lrName);
		std::vector<std::string> lSegments = split(lrText);
		for (const std::string& lrSegment: lSegments)
		{
			int lVal = atoi(lrSegment.c_str());
			if (lVal != 0 || (!lrSegment.empty() && lrSegment.front() == '0'))
				lOutput.push_back(lVal);
		}
		
		return lOutput;
	}
	
	//------------------------------------------------------------------------------
	
	std::vector<float> SettingsManager::getFloatVector(const std::string& lrName) const
	{
		std::vector<float> lOutput;
		
		const std::string& lrText = getString(lrName);
		std::vector<std::string> lSegments = split(lrText);
		for (const std::string& lrSegment: lSegments)
		{
			float lVal = atof(lrSegment.c_str());
			if (lVal != 0.0f || (!lrSegment.empty() && lrSegment.front() == '0'))
				lOutput.push_back(lVal);
		}
		
		return lOutput;
	}
	
	//------------------------------------------------------------------------------
	
	bool SettingsManager::setGroup(const std::string& lrName)
	{
		SettingsGroup* lpGroup = getGroup(lrName);
		if (lpGroup == nullptr)
			return false;
		
		mCurrentGroupName = lrName;
		mpCurrentGroup = lpGroup;
		return true;
	}
	
	//------------------------------------------------------------------------------
	
	SettingsManager::SettingsGroup* SettingsManager::getGroup(const std::string& lrName, const char* lpErrorFormat) const
	{
		auto liGroup = mGroupMap.find(lrName);
		if (liGroup == mGroupMap.end())
		{
			if (lpErrorFormat != nullptr)
				printf(lpErrorFormat, lrName.c_str());
			return nullptr;
		}
		return liGroup->second;
	}
	
	//------------------------------------------------------------------------------
}

//------------------------------------------------------------------------------
// Namespace Settings
//------------------------------------------------------------------------------

int Settings::getInt(const std::string& lrName)
{
	return SettingsManager::instance().getInt(lrName);
}

//------------------------------------------------------------------------------

float Settings::getFloat(const std::string& lrName)
{
	return SettingsManager::instance().getFloat(lrName);
}

//------------------------------------------------------------------------------

const std::string& Settings::getString(const std::string& lrName)
{
	return SettingsManager::instance().getString(lrName);
}

//------------------------------------------------------------------------------

std::vector<int> Settings::getIntVector(const std::string& lrName)
{
	return SettingsManager::instance().getIntVector(lrName);
}

//------------------------------------------------------------------------------

std::vector<float> Settings::getFloatVector(const std::string& lrName)
{
	return SettingsManager::instance().getFloatVector(lrName);
}

//------------------------------------------------------------------------------

std::vector<std::string> Settings::getStringVector(const std::string& lrName)
{
	return SettingsManager::instance().getStringVector(lrName);
}

//------------------------------------------------------------------------------

bool Settings::setGroup(const std::string& lrName)
{
	return SettingsManager::instance().setGroup(lrName);
}

//------------------------------------------------------------------------------
