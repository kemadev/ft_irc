#include "Reply.hpp"

string header_1(Server* server, ssize_t id, string rplyNb) {
	string reply;

	reply = ":" + server->userDB->search(id)->getHostName() + " " + rplyNb + " " + server->userDB->search(id)->getNickName() + " ";
	return (reply);
}

string header_2(Server* server, ssize_t id, string rplyNb) {
	string reply;
	if (rplyNb.empty())
		reply = ":" + server->userDB->search(id)->getNickName() + "!" + server->userDB->search(id)->getUserName() + "@" + server->userDB->search(id)->getHostName() + " ";
	else
		reply = ":" + server->userDB->search(id)->getNickName() + "!" + server->userDB->search(id)->getUserName() + "@" + server->userDB->search(id)->getHostName() + " " + rplyNb + " ";
	return (reply);
}

void reply_privmsg(Server* serv, string replyNb, string args, string reply) {
	if (replyNb == PRVMSG_U) {
		cout << "socket =" << serv->userDB->search(args)->getSocket() << endl;
		cout << reply << endl;
		send(serv->userDB->search(args)->getSocket(), reply.c_str(), strlen(reply.c_str()), MSG_DONTWAIT);
		return;
	}
	if (replyNb == PRVMSG_C) {
		vector<string> list = serv->chanDB->search(args)->getNickLst();
		vector<string>::iterator it = list.begin();
		vector<string>::iterator ite = list.end();
		while (it != ite) {
			cout << reply << endl;
			cout << "socket =" << serv->userDB->search(*it)->getSocket() << endl;
			send(serv->userDB->search(*it)->getSocket(), reply.c_str(), strlen(reply.c_str()), MSG_DONTWAIT);
			it++;
		}
		return;
	}
}

void reply_2(Server* serv, ssize_t uid, string replyNb, string args, string args2) {
	string reply;

	switch (atoi(replyNb.c_str())) {
		case 4546:
			reply = header_2(serv, uid, "");
			reply += "PART " + serv->chanDB->search(args)->getName() + " :" + args2 + "\r\n";
			break;
		case 4547:
			reply = header_2(serv, uid, "");
			reply += "PRIVMSG " + serv->userDB->search(args)->getNickName() + " :" + args2 + "\r\n";
			reply_privmsg(serv, replyNb, args, reply);
			return;
			break;
		case 4548:
			reply = header_2(serv, uid, "");
			reply += "PRIVMSG " + serv->chanDB->search(args)->getName() + " " + args2 + "\r\n";
			reply_privmsg(serv, replyNb, args, reply);
			return;
			break;
		case 4551:
			reply = header_2(serv, uid, "");
			reply += "TOPIC " + args + " :" + args2 + "\r\n";
			break;
		default:
			break;
	}
	cout << reply << endl;
	// TODO Send this message to all clients
	send((serv->userDB->search(uid))->getSocket(), reply.c_str(), strlen(reply.c_str()), 0);
	return;
}

void reply(Server* serv, ssize_t uid, string replyNb, string args) {
	string reply;

	switch (atoi(replyNb.c_str())) {
		// ANCHOR Improve the 0 in the fourth first messages
		case 001:
			reply = header_1(serv, uid, RPL_WELCOME);
			reply += " : Welcome to the Internet Relay Network " + serv->userDB->search(uid)->getNickName() + "\r\n";
			break;
		case 002:
			reply = header_1(serv, uid, RPL_YOURHOST);
			reply += " : Your host is " + serv->userDB->search(uid)->getHostName() + ", running on version [42.42]\r\n";
			break;
		case 003:
			reply = header_1(serv, uid, RPL_CREATED);
			reply += " : This server was created " + getTime() + "\r\n";
			break;
		case 004:
			reply = header_1(serv, uid, RPL_MYINFO);
			reply += " : " + serv->userDB->search(uid)->getHostName() + " version [42.42]. Available user MODE : +Oa . Avalaible channel MODE : none.\r\n";
			break;
		case 331:
			reply = header_1(serv, uid, RPL_TOPIC);
			reply += serv->chanDB->search(args)->getName() + " :No topic is set\r\n";
			break;
		case 332:
			reply = header_1(serv, uid, RPL_TOPIC);
			reply += serv->chanDB->search(args)->getName() + " :" + serv->chanDB->search(args)->getTopic() + "\r\n";
			break;
		case 353: {
			reply = header_1(serv, uid, RPL_NAMEREPLY);
			reply += "= " + serv->chanDB->search(args)->getName() + " :";
			vector<string> tmp = serv->chanDB->search(args)->getNickLst();
			vector<string>::iterator it = tmp.begin();
			vector<string>::iterator ite = tmp.end();
			while (it != ite) {
				if (serv->userDB->isOper(*it) == true)
					reply += "@";
				reply += ((*it) + " ");
				it++;
			}
			reply += "\r\n";
		} break;
		case 366:
			reply = header_1(serv, uid, RPL_ENDOFNAMES);
			reply += serv->chanDB->search(args)->getName() + " :End of NAMES list\r\n";
			break;
		case 4545:
			reply = header_2(serv, uid, "");
			reply += "JOIN " + serv->chanDB->search(args)->getName() + "\r\n";
			break;
		case 4549:
			reply = header_2(serv, uid, "");
			reply += "NICK " + args + "\r\n";
		case 4550:
			// TODO
			break;
		default:
			break;
	}
	cout << reply << endl;
	// TODO Send this message to all clients
	// if (replyNb == || )
	// {
	//     vector<string> list = params->irc_serv->chanDB->search(args)->getNickLst();
	//     vector<string>::iterator it = list.begin();
	//     vector<string>::iterator ite = list.end();
	//     while (it != ite)
	//     {
	//         send(params->irc_serv->userDB->search(*it)->getSocket(), reply.c_str(), strlen(reply.c_str()), MSG_DONTWAIT);
	//         it++;
	//     }
	// }
	// else
	send((serv->userDB->search(uid))->getSocket(), reply.c_str(), strlen(reply.c_str()), MSG_DONTWAIT);
	return;
}
