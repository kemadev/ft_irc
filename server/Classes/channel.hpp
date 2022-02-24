#ifndef _Channel_HPP
 #define _Channel_HPP

#include <sys/types.h>
#include <iostream>

using namespace std;

class WrongChannelName : public exception
{
	public:
		virtual const string	info() const throw()
		{
			return ("Channel should only contain alphanumeric characters");
		}
};

class WrongRoleName : public exception
{
	public:
		virtual const string	info() const throw()
		{
			return ("Role should either be 'user' or 'operator'");
		}
};

class Channel {
	private:
	string 			_name;
	string			_pass_hash;
	vector<string>	_msg_hist;

	public:
	Channel(string name, string pass);
	~Channel();
}

#endif