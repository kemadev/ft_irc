#include "../Error/Error.hpp"
#include "../CommandHandler/CommandHandler.hpp"
#include "../ServerManip/ServerManip.hpp"

#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

typedef struct s_params
{
    int fd;
    UidPool uidPool;
}   t_params;

using namespace std;

void*	task1(void *);

static int connFd;
void	error(const char* msg)
{
	perror(msg);
	exit(1);
}

int	main(int argc, char** argv)
{
	try {
		if (argc != 3)
			throw(BadNumberArgs());
	}
	catch(const BadNumberArgs e) {
		std::cerr << e.info() << std::endl;
		return (BAD_NUMBER_ARGS);
	}
	try {
		if (std::atoi(argv[1]) < 1 || std::atoi(argv[1]) > 65535)
			throw(InvalidPort());
	}
	catch (const InvalidPort e) {
		std::cerr << e.info() << std::endl;
		return (INVALID_PORT);
	}

	int portNo, listenFd;
	struct sockaddr_in svrAdd, clntAdd;

	vector<pthread_t> threadV;
	portNo = atoi(argv[1]);

	//create socket
	listenFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    try {
        int enable = 1;
        if (setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
            throw(ErrorReusingSocket());
    }
    catch (const ErrorReusingSocket e) {
        std::cerr << e.info() << std::endl;
    }
	try {
		if(listenFd < 0)
			throw (ErrorInSocketCreation());
	}
	catch (const ErrorInSocketCreation e) {
		std::cerr << e.info() << std::endl;
		return (ERR_SOCKET_CREATION);
	}
	
	bzero((char*) &svrAdd, sizeof(svrAdd));

	svrAdd.sin_family = AF_INET;
	svrAdd.sin_addr.s_addr = INADDR_ANY;
	svrAdd.sin_port = htons(portNo);

	//bind socket
	try {
		if(bind(listenFd, (struct sockaddr *)&svrAdd, sizeof(svrAdd)) < 0)
			throw (ErrorInBinding());
	}
	catch (const ErrorInBinding e) {
		std::cerr << e.info() << std::endl;
		return (ERR_BIND);
	}
	
	listen(listenFd, 5);
    t_params params;

    socklen_t len = sizeof(clntAdd);
    UidPool	pool = UidPool();
    while(1)
    {
        try {
            cout << "Listening to port " << argv[1] << endl;
            connFd = accept(listenFd, (struct sockaddr *)&clntAdd, &len);
            if (connFd < 0)
                throw (CannotAcceptConnection());
            else  
            {    
                pthread_t  a;
                threadV.push_back(a);
                params.fd = connFd;
                params.uidPool = pool;
                pthread_create(&threadV.back(), NULL, task1, &params); 
                cout << "Connection established." << endl;
            }
        }
        catch (const CannotAcceptConnection e){
            std::cerr << e.info() << std::endl;
            return (ERR_CONNECTION);
        }
    }
}

void *task1 (void *dummyPt)
{
    t_params* params = static_cast<t_params*>(dummyPt);
    cout << "-----------------" << endl;
	char input[256];
    int security = 0;
    int nbError = 0;
	bool loop = false;

    send(params->fd, ":ratio 1 dOD: Welcome to the Internet Relay Network dOD\r\n", strlen(":ratio 1 Dod: Welcome to the Internet Relay Network dOD\r\n"), 0);
    send(params->fd, ":ratio 2 dOD: Your host is ratio, running on version [42.42]\r\n", strlen(":ratio 2 dOD: Your host is ratio, running on version [42.42]\r\n"), 0);
    send(params->fd, ":ratio 3 dOD: This server was created?\r\n", strlen(":ratio 3 dOD: This server was created?\r\n"), 0);
    send(params->fd, ":ratio 4 dOD: ratio version [42.42]. Available user MODE : +Oa . Avalaible channel MODE : none.\r\n", strlen(":ratio 4 dOD: ratio version [42.42]. Available user MODE : +Oa . Avalaible channel MODE : none.\r\n"), 0);

    // send(params->fd, "Connection established.\n", strlen("Connection established.\n"), MSG_DONTWAIT); 
	bzero(input, 256);
    int n = read(params->fd, input, 255);
    try {
            if (n < 0)
                throw(ReadImpossible());
    }
    catch (const ReadImpossible e){
        std::cerr << e.info() << std::endl;
    }
    std::string input_s = input;
    cout << "input_s = " << input_s << endl;
    // TODO Add password check
    if (input_s.find("PASS") != std::string::npos)
    {
        std::string password = input_s.substr(strlen("PASS") + 1 , input_s.length() - strlen("PASS") - (input_s.length() - (input_s.find("CAP") - 3)));
        input_s.erase(0, strlen("PASS ") + password.length() + 2);
        cout << "password = " << password << endl;
        cout << "Inputv2 = " << input_s << endl;
    }
    // ANCHOR xchat check
    if (input_s.find("CAP") != std::string::npos)
        input_s.erase(0, strlen("CAP LS\n\r"));
    cout << "Inputv3 = " << input_s << endl;
    User current_user = createUser(input_s, params->uidPool, params->fd);

    // TODO Create user with infos;
	while(!loop)
	{
		bzero(input, 256);	 
		int n = read(params->fd, input, 255);
        std::cout << "Input = " << input << std::endl;
        try {
            if (n < 0)
                throw(ReadImpossible());
        }
        catch (const ReadImpossible e){
            std::cerr << e.info() << std::endl;
        }
        try{
            security = command_check(input, params->fd);
            if (security == CLIENT_DISCONNECTED)
                nbError++;
            if (nbError >= 5)
                throw (ClientDisconnected());
        }
        catch (const ClientDisconnected e) {
            std::cerr << e.info() << std::endl;
            close(params->fd);
            exit(CLIENT_DISCONNECTED);
        }
	}
	cout << "\nClosing thread and connection." << endl;
	close(params->fd);
	return (NULL);
}
