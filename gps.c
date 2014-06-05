#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <string.h>
#include <time.h>
#define FALSE -1
#define TRUE 0
#define lianpai 3

void set_speed(int fd,int speed);
int OpenDev (char *Dev);
int set_Parity(int fd,int databits,int stopbits,int parity);

int main(int argc,char **argv) 
{
	unsigned char rbuff[512];
	int fd,i,ii,iii,tmpi;
	int nread,nnread;
	char *dev ="/dev/ttyS1";
	fd = OpenDev (dev);
	set_speed(fd,4800); //4800
	if (set_Parity(fd,8,1,'N')==FALSE)
	{
		printf("Set Parity Error\n");
		exit(0);
	} 
	else 
		printf("open com1 successfully\n");
	usleep(5000);

       char tm_hour[2],tm_min[2],tm_sec[3],tm_day[3];
       char tm_mon[2],tm_year[5],latitude[11],longitude[11],NS,EW;//latitude jingdu
       latitude[11]='\0';
       longitude[11]='\0';
       tm_year[1]='2';
       tm_year[2]='0';

while(1){
  char gpsdata[512];
  nread=read(fd,rbuff,512);
  tmpi=0;
  for (iii=0;iii<8;iii++)
  {
  for (i=0;i<nread;i++)
  {
	  gpsdata[tmpi+i]=rbuff[i];
	  if (tmpi+i==510)
	  { 
		  break;
	  }
  }
  tmpi=tmpi+nread;
  nread=read(fd,rbuff,512);
  }
  gpsdata[511]='\0';
 //output the gpsdata
 //2012/12/20 11:31:09 E12025.7496 N3607.2785
//     for (ii=0;ii<strlen(gpsdata);ii++){
//  	       printf("%c",gpsdata[ii]);
//  }
        
  for (i=0;i<strlen(gpsdata);i++)
  {
	  if((0x24==gpsdata[i])&&(0x43==gpsdata[i+5]))
	  {  //C 0x43 $ 0x24
		  tm_hour[0]=gpsdata[i+7];
		  tm_hour[1]=gpsdata[i+8];
        	// hour +8  beijing time
        
			if((tm_hour[1]+8) > '9') {tm_hour[0]=tm_hour[0]+1;
        	tm_hour[1]=tm_hour[1]+8-10;
        	}
        	else (tm_hour[1]=tm_hour[1]+8);
        	 				tm_min[0]=gpsdata[i+9];
        	 				tm_min[1]=gpsdata[i+10];
        	 				tm_sec[0]=gpsdata[i+11];
        	 				tm_sec[1]=gpsdata[i+12];
        	 				ii=get_comma_position(gpsdata,i,9);
        	 				tm_year[3]=gpsdata[ii+5];
        	 				tm_year[4]=gpsdata[ii+6];
        	 				tm_mon[1]=gpsdata[ii+3];
        	 				tm_mon[2]=gpsdata[ii+4];
        	 				tm_day[1]=gpsdata[ii+1];
        	 				tm_day[2]=gpsdata[ii+2];
        	 				EW=gpsdata[get_comma_position(gpsdata,i,6)+1];
        	 				ii=get_comma_position(gpsdata,i,5);
        	 				for (tmpi=1;tmpi<=10;tmpi++) {
        	 				latitude[tmpi]=gpsdata[ii+tmpi];}
        	 				NS=gpsdata[get_comma_position(gpsdata,i,4)+1];
        	 				ii=get_comma_position(gpsdata,i,3);
        	 				for (tmpi=1;tmpi<=9;tmpi++) 
							{
								longitude[tmpi]=gpsdata[ii+tmpi];
							}
        //output the gpsdata needed
							for(i=1;i<=4;i++)
							{
								printf("%c",tm_year[i]);
							}
        	 				printf("/");
        	 				for (i=1;i<3;i++)
							{
								printf("%c",tm_mon[i]);
							}
        	 				printf("/");
        	 				for (i=1;i<3;i++)
							{
								printf("%c",tm_day[i]);
							}
        	 				printf(" ");
        	 				for (i=0;i<2;i++)
							{
								printf("%c",tm_hour[i]);
							}
        	 			    printf(":");
        	 			    for (i=0;i<2;i++)
							{
								printf("%c",tm_min[i]);
							}
        	 			     printf(":");
        	 			    for (i=0;i<2;i++)
							{
								printf("%c",tm_sec[i]);
							}
                            printf(" ");
              			printf("%C",EW);
              			for (i=1;i<=10;i++) 
						{
							printf("%c",latitude[i]);
						}
              			printf(" ");
              			printf("%C",NS);
              			for (i=1;i<=9;i++)
						{
							printf("%c",longitude[i]);
						}
              			printf("\n");

                        break;
        	 			}

        	 	}
        	 	tcflush(fd,TCIOFLUSH);
}
   close(fd);
   return 0;
   return EXIT_SUCCESS;
}

int OpenDev (char *Dev)
	{
		int fd = open (Dev, O_RDWR );
		if (-1==fd)
		{
		  perror("Can't Open Serial Port");
		  return -1;
		}
		else
			return fd;
	}

int speed_arr[]={B38400,B19200,B9600,B4800,B38400,B19200,B9600,B4800};
int name_arr[]={38400,19200,9600,4800,38400,19200,9600,4800};
void set_speed(int fd,int speed)
{
	int i;
	int status;
	struct termios Opt;
	tcgetattr(fd,&Opt);
	for (i=0;i<sizeof(speed_arr)/sizeof(int);i++)
	{
		if (speed==name_arr[i])
		{
			tcflush(fd,TCIOFLUSH);
			cfsetispeed(&Opt,speed_arr[i]);
			cfsetospeed(&Opt,speed_arr[i]);
			status=tcsetattr(fd,TCSANOW,&Opt);
			if (status!=0)
			{
				perror("tcsetattr fd");
				return;
			}
			tcflush(fd,TCIOFLUSH);
		}
	}
}

int set_Parity (int fd,int databits,int stopbits,int parity)
{
	struct termios options;
	if(tcgetattr(fd,&options)!= 0)
	{
		perror("SetupSerial 1");
		return(FALSE);
	}
	options.c_cflag &=~CSIZE;
	switch(databits)
	{
	case 7: options.c_cflag |=CS7;break;
	case 8: options.c_cflag |=CS8;break;
	default: fprintf(stderr,"Unsupported data size\n");return(FALSE);
	}

	switch(parity)
	{
	case 'n':
	case 'N':options.c_cflag &=~PARENB;  //clear parity enable
	         options.c_iflag &=~INPCK; //enable parity checking
	         break;
	case 'o':
	case 'O':
		      options.c_cflag |= (PARODD | PARENB);
	          options.c_iflag |= INPCK;  //disable parity checking
	          break;
	case 'e':
	case 'E': options.c_cflag |= PARENB;  //enable parity
	          options.c_cflag &= ~PARODD;
	          options.c_iflag |= INPCK;  //disable parity checking
	          break;
	case 'S':
	case 's':      //no parity
		  options.c_cflag &=~PARENB;
		  options.c_cflag&=~CSTOPB;
		  break;
	default:
		  fprintf(stderr,"Unsupported parity \n");
		  return(FALSE);
	}

	switch(stopbits){
	case 1: options.c_cflag &= ~CSTOPB;break;
	case 2: options.c_cflag |= CSTOPB;break;
	default:fprintf(stderr,"Unsupported stop bits \n");
	       return(FALSE);

	}
	//set input parity option
	if (parity !='n')
    options.c_iflag |=INPCK;
    options.c_cflag|= (CLOCAL|CREAD);
    options.c_lflag&=~(ICANON|ECHO|ECHOE|ISIG);
    options.c_oflag&=~OPOST;
    options.c_oflag&=~(ONLCR|OCRNL);
    options.c_iflag&=~(ICRNL|INLCR);
    options.c_iflag&=~(IXON|IXOFF|IXANY);

	tcflush(fd,TCIFLUSH);
	options.c_cc[VTIME]=100;//10 jie shou yi ge zijie dengdai chaoshi
	options.c_cc[VMIN]=68;  //68 in example zuishao zifu duqu shu
	if (tcsetattr(fd,TCSANOW,&options)!=0)
	{
		perror("Setup Serial 1");
		return(FALSE);
	}
	return(TRUE);
}
int get_comma_position(char gpsdata[512],int i,int n)
{
	int tmpi,commanum=0;
for (tmpi=1;tmpi<=80;tmpi++)
{   //find the nineth comma
 		if (gpsdata[i+tmpi]==','){commanum+=1;}
 		if (commanum==n) {break;};
}
return (i+tmpi);
}
