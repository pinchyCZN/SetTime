#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>


int getkey()
{
	int i=0;
	i=getch();
	if(i==0 || i==0xE0)
	{
		i=getch();
	}
	return i&0xFF;

}
int getkey2()
{
	int i=0;
	if(kbhit())
	{
		i=getch();
		if(i==0 || i==0xE0)
			i=getch();
	}
	return i&0xFF;

}

show_window()
{
	byte Title[200]; 
	byte ClassName[200]; 
	LPTSTR  lpClassName = ClassName; 
	HANDLE hConWnd; 

	GetConsoleTitle( Title, sizeof(Title));
	hConWnd = FindWindow( NULL, Title);
	GetClassName( hConWnd,lpClassName,120);
	ShowWindow( hConWnd,SW_SHOWNORMAL); 
	SetForegroundWindow(hConWnd);
}

minimize_window()
{
	byte Title[200]; 
	byte ClassName[200]; 
	LPTSTR  lpClassName = ClassName; 
	HANDLE hConWnd; 

	GetConsoleTitle( Title, sizeof(Title));
	hConWnd = FindWindow( NULL, Title);
	GetClassName( hConWnd,lpClassName,120);
	ShowWindow( hConWnd,SW_MINIMIZE); 
}
int get_ip_from_unknown(char *server,char *ipout)
{
    struct addrinfo *result = NULL;
    struct addrinfo *ptr = NULL;
//    struct addrinfo hints;

//	memset(&hints,0,sizeof(hints));
//	hints.ai_family = AF_UNSPEC;
//	hints.ai_socktype = SOCK_STREAM;
//	hints.ai_protocol = IPPROTO_TCP;
	return 0;
}
int get_month_int(char *str)
{
	char monstr[80];
	strcpy(monstr,str);
	strupr(monstr);
	if(strstr(monstr,"JAN"))
		return 1;
	if(strstr(monstr,"FEB"))
		return 2;
	if(strstr(monstr,"MAR"))
		return 3;
	if(strstr(monstr,"APR"))
		return 4;
	if(strstr(monstr,"MAY"))
		return 5;
	if(strstr(monstr,"JUN"))
		return 6;
	if(strstr(monstr,"JUL"))
		return 7;
	if(strstr(monstr,"AUG"))
		return 8;
	if(strstr(monstr,"SEP"))
		return 9;
	if(strstr(monstr,"OCT"))
		return 10;
	if(strstr(monstr,"NOV"))
		return 11;
	if(strstr(monstr,"DEC"))
		return 12;
	return 0;
}
int get_dword(char *buffer)
{
	int i;
	BYTE a;
	DWORD data=0;
	for(i=0;i<4;i++)
	{
		a=buffer[i];
		data|=a<<((3-i)*8);
	}
	return data;
}
int get_ntp_win_epoch_diff(LARGE_INTEGER *epochdiff)
{
	SYSTEMTIME ntpepoch;
	FILETIME ft;
	ntpepoch.wDay=1;
	ntpepoch.wDayOfWeek=1; //doesnt matter
	ntpepoch.wHour=0;
	ntpepoch.wMinute=0;
	ntpepoch.wSecond=0;
	ntpepoch.wMilliseconds=0;
	ntpepoch.wMonth=1;
	ntpepoch.wYear=1900;
	SystemTimeToFileTime(&ntpepoch,&ft);
	epochdiff->HighPart=ft.dwHighDateTime;
	epochdiff->LowPart=ft.dwLowDateTime;
	return TRUE;
}
int get_ntp_time_from_systime(LARGE_INTEGER *ntp,SYSTEMTIME *systime)
{
	FILETIME ft;
	LARGE_INTEGER epoch;
	SystemTimeToFileTime(systime,&ft);
	ntp->HighPart=ft.dwHighDateTime;
	ntp->LowPart=ft.dwLowDateTime;
	get_ntp_win_epoch_diff(&epoch);
	ntp->QuadPart-=epoch.QuadPart;
	ntp->QuadPart/=(1000000000/100);
	return TRUE;
}
int convert_ntp_to_systime(LARGE_INTEGER *ntp,SYSTEMTIME *systime)
{
	FILETIME ft;
	LARGE_INTEGER epoch;
	get_ntp_win_epoch_diff(&epoch);
	epoch.QuadPart+=(ntp->QuadPart)*(1000000000/100);
	ft.dwHighDateTime=epoch.HighPart;
	ft.dwLowDateTime=epoch.LowPart;
	FileTimeToSystemTime(&ft,systime);
	return TRUE;

}
int get_time_ntp(char *timeserver,int port,int update)
{
	WSADATA ws;
	int tcp_socket;
	struct sockaddr_in peer;
	char recvbuffer[255];
	int packet_size;
	LARGE_INTEGER timestamp;
	SYSTEMTIME time;

	BYTE time_packet[] = {
	0xe3, 0x00, 0x04, 0xfa, 0x00, 0x01, 0x00, 0x00, 
	0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; //time stamp blank

	GetSystemTime(&time);
	if(WSAStartup(0x0101,&ws)!=0)
	{
		printf("unable to WSAStartup()\n");
		return FALSE;
	}

	peer.sin_family = AF_INET;
	peer.sin_port = htons((u_short)port);
	peer.sin_addr.s_addr = inet_addr(timeserver);




	tcp_socket = socket(AF_INET, SOCK_DGRAM, 0);
	printf("connecting to %s port:%i\n",timeserver,port);
	if(connect(tcp_socket, (struct sockaddr*)&peer, sizeof(peer)) != 0)
	{
		closesocket(tcp_socket);
		printf("unable to connect\n");
		return FALSE;

	}
	printf("sending token\n");
	send(tcp_socket, (char*)time_packet, sizeof(time_packet),0);

	memset(recvbuffer,0,sizeof(recvbuffer));
	packet_size=recv(tcp_socket, recvbuffer, sizeof(recvbuffer),0); //format Fri Feb   6 16:40:28 2009
	closesocket(tcp_socket);
	timestamp.QuadPart=0;
	if(packet_size>=0x30)
	{
		timestamp.LowPart=get_dword(recvbuffer+0x20);
		if(timestamp.QuadPart!=0)
		{
			printf("old time %02i:%02i:%02i  %02i/%02i/%04i\n",time.wHour,time.wMinute,time.wSecond,
				time.wMonth,time.wDay,time.wYear);

			convert_ntp_to_systime(&timestamp,&time);
			printf("new time %02i:%02i:%02i  %02i/%02i/%04i\n",time.wHour,time.wMinute,time.wSecond,
				time.wMonth,time.wDay,time.wYear);
			if(update)
			{
				if(SetSystemTime(&time)!=0)
					printf("time updated\n");
			}
		}
		else
			printf("recv ntp timestamp of zero\n");

	}
	else
	{
		printf("ntp packet size not >= 0x30 (%X)\n",packet_size);
	}
	return TRUE;




}

int get_time_daytime_protocol(char *timeserver,int port,int update)
{
	int i;
	SYSTEMTIME time;
	int hour,min,sec,day,year,month_int=0;
	char dayofweek[20],month[20];
	WSADATA ws;
	int tcp_socket;
	struct sockaddr_in peer;
	char recvbuffer[255];

	if(WSAStartup(0x0101,&ws)!=0)
	{
		printf("unable to WSAStartup()\n");
		return FALSE;
	}

	peer.sin_family = AF_INET;
	peer.sin_port = htons((u_short)port);
	peer.sin_addr.s_addr = inet_addr(timeserver);

	tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	printf("connecting to %s port:%i\n",timeserver,port);
	if(connect(tcp_socket, (struct sockaddr*)&peer, sizeof(peer)) != 0)
	{
		closesocket(tcp_socket);
		printf("unable to connect\n");
		return FALSE;
	}

	memset(recvbuffer,0,sizeof(recvbuffer));
	recv(tcp_socket, recvbuffer, sizeof(recvbuffer),0); //format Fri Feb   6 16:40:28 2009
	closesocket(tcp_socket);

	printf("Response:  %s\n",recvbuffer);
	if(strstr(recvbuffer,"NIST")!=0) //NIST format: JJJJJ YR-MO-DA HH:MM:SS TT L H msADV UTC(NIST) OTM.
	{
		if(6!=sscanf(recvbuffer,"%*s %2d-%2d-%2d %2d:%2d:%2d",&year,&month_int,&day,&hour,&min,&sec))
			printf("sscanf error\n");
		year=year+2000;
		printf("scanned as %4d-%02d-%02d %02d:%02d:%02d\n",year,month_int,day,hour,min,sec);
	}
	else
	{
		if(7!=sscanf(recvbuffer,"%s %s %d %2d:%2d:%2d %d",dayofweek,month,&day,&hour,&min,&sec,&year))
			printf("sscanf error\n");
		printf("scanned as %s %s  %i %02d:%02d:%02d %d\n",dayofweek,month,day,hour,min,sec,year);
	}

	memset(&time,0,sizeof(time));
	GetSystemTime(&time);

	printf("old time %02i:%02i:%02i  %02i/%02i/%04i\n",time.wHour,time.wMinute,time.wSecond,
		time.wMonth,time.wDay,time.wYear);
	time.wHour=hour;
	time.wMinute=min;
	time.wSecond=sec;
	time.wYear=year;
	time.wDay=day;
	if(month_int!=0)
		time.wMonth=month_int;
	else
	{
		i=get_month_int(month);
		if(i==0)
			printf("error determining month string, no change to month made\n");
		else
			time.wMonth=i;
	}
	printf("new time %02i:%02i:%02i  %02i/%02i/%04i\n",time.wHour,time.wMinute,time.wSecond,
		time.wMonth,time.wDay,time.wYear);

	if(update)
	{
		if(SetSystemTime(&time)!=0)
		{
			printf("time updated\n");
			return TRUE;
		}
		else
		{
			printf("time update failed\n");
			return FALSE;
		}
	}
	return TRUE;
}
int list_and_get_servers(char *server,int *port,int *use_ntp)
{
	char str[80];
	int selection=0;
	int i;
	typedef struct {char *name;char *descrip;int port;int ntp;} _servlist;
	static _servlist servlist[]={
		{"132.163.4.101","time-b.timefreq.bldrdoc.gov",13,FALSE},
		{"132.163.4.102","time-b.timefreq.bldrdoc.gov",13,FALSE},
		{"132.163.4.103","time-b.timefreq.bldrdoc.gov",13,FALSE},
		{"70.84.194.243","some yackity schmack place ",1313,FALSE},
	};
	printf("\n\nselect time server\n");
	for(i=0;i<sizeof(servlist)/sizeof(_servlist);i++)
	{
		printf("%i:%s :%s %i %s\n",i+1,servlist[i].descrip,servlist[i].name,servlist[i].port,servlist[i].ntp?"NTP":"");
	}
	gets(str);
	sscanf(str,"%d",&selection);
	if( (selection<=0) || (selection>(sizeof(servlist)/sizeof(_servlist))) )
		selection=1; 
	selection-=1;
	strcpy(server,servlist[selection].name);
	*port=servlist[selection].port;
	*use_ntp=servlist[selection].ntp;
	return 0;
}
int usage()
{
	printf("usage: settime [-server \"name\"] [-port #] [-ntp] [-noupdate] [-list]\n");
	return 0;
}
int main(int argc,char *argv[])
{

	int i;
	char timeserver[255];
	int port;
	int key;
	int use_ntp=FALSE;
	int update=TRUE;

	printf("build date %s %s\n",__DATE__,__TIME__);
	usage();
	port=13;

	if(argc>1)
	{
		for(i=1;i<argc;i++)
		{
			strlwr(argv[i]);
			if(strstr(argv[i],"-server")!=0)
			{
				if(argc>i+1)
				{
					strncpy(timeserver,argv[i+1],sizeof(timeserver)-1);
					i++;
				}
			}
			else if(strstr(argv[i],"-port")!=0)
			{
				if(argc>i+1)
				{
					sscanf(argv[i+1],"%i",&port);
					i++;
				}
			}
			else if(strstr(argv[i],"-ntp")!=0)
			{
				if(i==1)
				{
					strcpy(timeserver,"172.16.10.1");
					port=123;
				}
				use_ntp=TRUE;
			}
			else if(strstr(argv[i],"-noupdate")!=0)
			{
				update=FALSE;
			}
			else if(strstr(argv[i],"-list")!=0)
				list_and_get_servers(timeserver,&port,&use_ntp);
		}
	}
	else
	{
		strcpy(timeserver,"70.84.194.243");  
		port=1313;
	}
	printf("using server [%s] port [%i]\n",timeserver,port);
	if(use_ntp)
		printf("using NTP protocol\n");
	

	if(use_ntp)
		get_time_ntp(timeserver,port,update);
	else
		get_time_daytime_protocol(timeserver,port,update);


	if(getenv("PROMPT")==0){
		printf("press any key to exit\n");
		getkey();
	}

}