
#include <stdio.h>
#include <cstdio>
#include <cmath>
#include <math.h>
#include <conio.h>
#include <iostream>
#include <winsock2.h>
#include <time.h>

using namespace std;

#define BUFLEN 512	//Max length of buffer
#define PORT 7777	//The port on which to listen for incoming data
#define IS_PASSWORDED 0 //Specifies whether the server has a password or not

#define SERVER_FILE "config/server.dat"

class HandlePackets
{
public:
        //Constructor
        SampSendUDP() { cout<<"Instatiating Constructor()"<<endl;};
        //Destructor
        ~SampSendUDP() { cout<<"Destroying Constructor()"<<endl; };
		void initSocket( void );
		//void sendReply(char buf[512], SOCKET s, int recv_len, int slen);
		void sendReply(unsigned int binaryAddress, unsigned short port, char* data, int length, SOCKET s);
		void setServerVars(char* hostname, char* gamemode, char* mapname);
		void SendRules(SOCKET s, char* data, const sockaddr_in* to, int tolen);
		int	getAmountOfRules( void );
private: //Private variables so they can only be accessed within the class
        struct sockaddr_in server, si_other;
		int slen , recv_len;
		char buf[BUFLEN];
		WSADATA wsa;
		struct savedaddress;
		char* szHostname;
		char* szGameMode;
		char* szMapName;
		WORD wRuleCount;
};
