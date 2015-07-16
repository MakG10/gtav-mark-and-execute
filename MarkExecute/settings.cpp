/*
  Author: MakG (http://www.gta-mods.pl)
*/

#include <sys/stat.h>
#include <stdlib.h>
#include <map>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include "settings.h"

using namespace std;

Settings::Settings()
{

}

Settings::Settings(string filename)
{
	this->load(filename);
}
	
map<string, map<string, string> > Settings::load(string filename)
{
	this->filename = filename;

	struct stat buffer;
	if (stat(this->filename.c_str(), &buffer) == 0)
	{
		read_ini(filename, this->propertyTree);

		try
		{
			for (auto& section : this->propertyTree)
			{
// 					this->options[section.first] = new map<string, string>;
					
				for (auto& option : section.second)
					this->options[section.first][option.first] = option.second.get_value<std::string>();
			}
		} catch(...) {
			// Pusta sekcja
		}
	}
		
	return this->options;
}
	
void Settings::save(string filename, map<string, map<string, string> > options)
{
	boost::property_tree::ptree newPropertyTree;
		
	for(auto &section : options)
		for(auto &option : section.second)
			newPropertyTree.put(section.first +  "." + option.first, option.second);
			
	write_ini(filename, this->propertyTree);
}
	
void Settings::save(string filename)
{
	write_ini(filename, this->propertyTree);
}
	
void Settings::save()
{
	write_ini(this->filename, this->propertyTree);
}
	
void Settings::setOption(string section, string name, string value)
{
	this->options[section][name] = value;
	this->propertyTree.put(section +  "." + name, value);
}
	
string Settings::getOption(string name)
{
	return this->getOption(name, this->options.begin()->first);
}
	
string Settings::getOption(string name, string section)
{
	if (this->options[section].find(name) != this->options[section].end())
		return this->options[section][name];
	else
		return "NULL";
}
	
bool Settings::getOptionBool(string name)
{
	return this->getOptionBool(name, this->options.begin()->first);
}
	
bool Settings::getOptionBool(string name, string section)
{
	string option = this->getOption(name, section);
		
	if(option == "true" || option == "1" || option == "on")
	{
		return true;
	} else {
		return false;
	}
}

map<string, map<string, string> > Settings::getOptions()
{
	return this->options;
}

map<string, string> Settings::getOptions(string section)
{
	return this->options[section];
}
