#ifndef _PCXX_CONFIG_H__2004_03_21_
#define _PCXX_CONFIG_H__2004_03_21_

#include <utility>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <algorithm> //find

class Conf
{
public:
	//type define
	typedef std::string string;
	typedef string Section;
	typedef string Key;
	typedef string Value;
	typedef std::pair<Key, Value> KeyValue;

	//set/get method
	void setValue(const string& section, const string& key, const string& value)
	{
		m_mapItem[makeIndex(section, key)] = value;

		Index index(section, key);
		RecordList::iterator it;
		for (it=m_list.begin(); it!=m_list.end(); ++it)
        {
            if (index == (*it).first)
			{
				(*it).second = value;
				return ;
			}
        }
		m_list.push_back(Record(index, value));
	}
	string getValue(const string& section, const string& key)
	{
	    StringMap::iterator it = m_mapItem.find( makeIndex(section, key) );
	    if (it != m_mapItem.end())
		    return (*it).second;
	    return string();
	}

	//modify success : return true; other return false
	bool modiValue(const string& section, const string& key, const string& value)
	{
	    if (value == getValue(section, key))
		    return false;
	    setValue(section, key, value);
	    return true;
	}
	//clear method
	void clear()
	{
	    m_mapItem.clear();
	    m_list.clear();
	}
	bool empty() const
	{
	    return m_mapItem.empty();
	}


	//query method
	bool querySection(const string& section)
	{
	    RecordList::iterator it;
	    for (it=m_list.begin(); it!=m_list.end(); ++it)
        {
            if ((*it).first.first == section)
			    return true;
        }
	    return false;
	}

	bool queryKey(const string& section, const string& key)
	{
		return m_mapItem.find( makeIndex(section, key) ) != m_mapItem.end();
	}
		

	//enum method
	template <typename _list_or_vector>
	void enumSection(_list_or_vector &sections)
	{
	    sections.clear();
	    string tmp;
	    RecordList::iterator it;
	    for (it=m_list.begin(); it!=m_list.end(); ++it)
        {
            if ((*it).first.first != tmp)
		    {
		        tmp = (*it).first.first;
		        sections.push_back(tmp);
		    }
        }
	}
	template <typename _list_or_vector>
	void enumKeyValue(const string& section, _list_or_vector &keyValues)
	{
	    keyValues.clear();
	    RecordList::iterator it;
	    for (it=m_list.begin(); it!=m_list.end(); ++it)
	    {
	        if ((*it).first.first == section)
		        keyValues.push_back(KeyValue((*it).first.second, (*it).second));
	    }
	}

protected:
	typedef std::map<string, string> StringMap;
	StringMap m_mapItem;

	typedef std::pair<Section, Key> Index;
	typedef std::pair<Index, Value> Record;
	typedef std::list<Record> RecordList;
	RecordList m_list;

	string makeIndex(const string& section, const string& key)
	{
		return section + key; 
	}
};


class ConfFile : public Conf
{
public:
	bool loadFromFile(const string& fileName)
	{
		FILE* file = fopen(fileName.c_str(), "rt");
		if (NULL == file) return false;
		//ScopeGuard fileGuard = MakeGuard(fclose, file);
		char line[1024];
		string section("__g_header__");
		string key;
		string value;
		int noteId = 0;
		for (;;)
		{
			if (NULL == fgets(line, sizeof(line), file))
				if (ferror(file) == 0)
					break;
				else
					return false;
			string str(line);
			trim(str);
			string::size_type strsize = str.size();
			if (strsize < 3)
			{
				if ((strsize == 0) || (str[0] == ';') || (str[0] == '#') || (str == "//"))
					setValue(section, "__m_note" + intToStr(noteId++), str);
				else
					continue;
			}
			else if ((str[0] == ';') || (str[0] == '#') || (str.substr(0, 2) == "//"))
				setValue(section, "__m_note" + intToStr(noteId++), str);
			else if ((str[0] == '[') && (*(str.end()-1) == ']'))
			{
				section = str.substr(1, str.size() - 2);
				trim(section);
				noteId = 0;
			}
            else
			{
				string::size_type pos = str.find('=');
				if (pos == string::npos)
					continue;
				key = str.substr(0, pos);
				value = str.substr(pos + 1);
				setValue(section, trim(key), trim(value));
			}
		}
		return true;
	}
	
	bool saveToFile(const string& fileName)
	{
		FILE* file = fopen(fileName.c_str(), "wt");
		if (NULL == file) return false;

		typedef std::list<string> StringList;
		typedef std::list<KeyValue> KeyValueList;
		StringList sections;
		KeyValueList keyValues;

		enumSection(sections);

		char line[1024];
		for (StringList::iterator it = sections.begin(); it != sections.end(); ++it)
		{
			string& section = *it;
			enumKeyValue(section, keyValues);

			if (section != "__g_header__")
			{
				sprintf(line, "[%s]\n", section.c_str());
				/*ASSERT(*/fputs(line, file)/* != EOF)*/;
			}

			for (KeyValueList::iterator it1 = keyValues.begin(); it1 != keyValues.end(); ++it1)
			{
				if ((*it1).first.substr(0, 8) == "__m_note")
					sprintf(line, "%s\n", (*it1).second.c_str());
				else
					sprintf(line, "%s = %s\n", (*it1).first.c_str(), (*it1).second.c_str());
				/*ASSERT(*/fputs(line, file)/* != EOF)*/;
			}
		}
		return true;
	}	

protected:

	//helper
	string intToStr(int a)
	{
		char buf[20];
		sprintf(buf, "%d", a);
		return string(buf);
	}
	string& trim(string& str)
	{
		string::size_type idx;
		//trim left
		idx = str.find_first_not_of(" \t\r\n");
		if (0 == idx)
			;
		else if (idx != std::string::npos)
			str.erase(0,idx);
		else
			str.erase();
		//trim right
		idx = str.find_last_not_of(" \t\r\n");
		if ((idx+(size_t)1) == str.size())
			;
		else if (idx != std::string::npos)
			str.erase(++idx);
		else
			str.erase();
		return str;
	}
};

#endif

