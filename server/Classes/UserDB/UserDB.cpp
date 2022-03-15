#include "../UserDB/UserDB.hpp"

using namespace std;

ssize_t	UserDB::add(User& usr) {
	_db.push_back(usr);
	log(string(LIGHT_MAGENTA) + string("User ") + string(GREEN) + string(usr.getUserName()) + string(LIGHT_BLUE) + string(" has been added to ") + string(LIGHT_MAGENTA) + string("userDB ") + string(GREEN) + string(this->_name) + string(DEFAULT));
	return usr.getUid();
}

User*	UserDB::search(User& usr) {
	try {
		vector<User>::iterator it = _db.begin(), end = _db.end();
		ssize_t	id = usr.getUid();
		while (it != end) {
			if (it->getUid() == id) {
				return &(*it);
			}
			it++;
		}
		throw NoSuchUser();
	}
	catch (exception& e) {
		logError(string("Searching user"), usr.getUserName(), e.what());
		throw UserAddFail();
	}
	return nullptr;
}

User*	UserDB::search(ssize_t id) {
	try {
		vector<User>::iterator it = _db.begin(), end = _db.end();
		while (it != end) {
			if (it->getUid() == id) {
				return &(*it);
			}
			it++;
		}
		throw NoSuchUser();
	}
	catch (exception& e) {
		logError(string("Searching user"), to_string(id), e.what());
		throw UserAddFail();
	}
	return nullptr;
}

void	UserDB::chkDuplicate(string username, string fullname, string nickname) {
	try {
		vector<User>::iterator it = _db.begin(), end = _db.end();
		while (it != end) {
			if (it->getUserName() == username) {
				throw DuplicateUsername();
			}
			if (it->getFullName() == fullname) {
				throw DuplicateFullname();
			}
			if (it->getNickName() == nickname) {
				throw DuplicateNickname();
			}
			it++;
		}
	}
	catch (exception& e) {
		logError(string("Checking user duplicate"), username, e.what());
		throw UserDuplicate();
	}
}
