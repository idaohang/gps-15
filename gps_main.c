/*************************************************************************
  > File Name: gps_main.c
  > Author: yzliu
# Copyright  (C), date, Hongdian Tech. Co., Ltd.
> Created Time: 2014年06月06日 星期五 16时41分26秒
 ************************************************************************/

#include "gps_main.h"
#include <fcntl.h>
#include <sys/stat.h>



#define BAUDRATE B9600



char GPS_BUF[4096];
GPS_INFO gps_info;
static int baud = BAUDRATE;
volatile int fd;
int flag ;

void touch_gps_timestamp()
{
    int fd_ts;

    fd_ts = open(GPS_TS_FILE, O_CREAT | O_RDWR);
    futimens(fd_ts, NULL);
    close(fd_ts);
}


time_t get_gps_timestamp()
{
    struct stat s;
    if(stat(GPS_TS_FILE, &s)) {
        return 0;
    }
    return s.st_ctime;
}

double Convertlatilongtude(double parameter) {
  double value =0.0;
  double tempvalue =0.0;
  int  degree =0;
  
  degree =(int)parameter;
  tempvalue = parameter - (double)degree;
  tempvalue = (double)(tempvalue*1.66666667);
  
  value =(double)degree + tempvalue;
  
  return value;
}

int receive(GPS_INFO * GPS)
{
    int i = 0, res;
    char buf[4096],c;
    FILE *fp = NULL,*fp2 = NULL;
    char cmdbuf[200],pbuf[100];
    time_t report_ts;
		report_ts = time(NULL);
    //report_ts = system_time_get();
		touch_gps_timestamp();
    printf("read gps date\n");

    bzero((void*)buf, sizeof(buf));
    while (1) {
				//bzero((void*)buf, sizeof(buf));
				bzero((void*)cmdbuf, sizeof(cmdbuf));
				bzero((void*)pbuf, sizeof(pbuf));
       
        c = 0;
        if((res = read(fd, &c, 1 )) == -1) {
            printf("Could not read serial\n");
            system("echo 2000 > /dev/h895x_led");
            msg(M_INFO, "serial read error!\n");
            //exit(-1);
        }
        
        if (res > 0)
            buf[i++] = c;
        //printf("1.1,i = %d, buf = %s\n", i,buf);
        if(c == '\n'){

            printf("%s", buf);
            if (buf[0] == '$') {
                gps_parse(buf, &gps_info);    //分离原始数据
                i = 0;
                bzero((void*)buf, sizeof(buf));

               // show_gps(&gps_info);    //显示分离后的
                sprintf(cmdbuf,
                        "{'status': '%c', 'latitude': %2.14f, 'east_west': '%c', 'date': '%d-%02d-%02d %02d:%02d:%02d', 'speed': '%3.2f', 'north_south': '%c', 'longitude': %2.14f, 'height': '%4.1f'}\n",
                        GPS->STATUS, Convertlatilongtude(GPS->latitude),
                        GPS->EW, GPS->D.tm_year,
                        GPS->D.tm_mon, GPS->D.tm_mday,
                        GPS->D.tm_hour, GPS->D.tm_min,
                        GPS->D.tm_sec,
                        (GPS->SPEED) * 1.852, GPS->NS,
                        Convertlatilongtude(GPS->longitude), 
                        GPS->height);
                printf("%s", cmdbuf);
                sprintf(pbuf,
                        "date -s '%d-%02d-%02d  %02d:%02d:%02d'",
                        GPS->D.tm_year,GPS->D.tm_mon,
                        GPS->D.tm_mday,GPS->D.tm_hour,
                        GPS->D.tm_min,GPS->D.tm_sec);
 								if (gps_info.STATUS == 'V') {
                    system("echo 2002 > /dev/h895x_led");
                    msg(M_INFO, "LED turn FAST ,GPS_DATA INVAILD\n");
                } else if (gps_info.STATUS == 'A') {
                    system("echo 2001 > /dev/h895x_led");
                   
                    //save_gps(GPS_REPORT_DATA_FILE ,cmdbuf,"w+");
                }

                if(time(NULL) - report_ts > 5 * 60) {
                    
                    // report every 5 minutes
                    fp = fopen(GPS_DATA_FILE, "a+");
                    if (NULL == fp) {
                        perror("unable to open  gps_data");
                        //fclose(fp);
                        return -1;
                    }
  
                    fputs(cmdbuf, fp);
                    fclose(fp);
        
                   // system("python /root/wifibox_code/gps/gps_report.py");
                   printf("start sync time:%s \n",pbuf);
                    system(pbuf);
                    printf("start to sve gps_data per 5min!\n");
                 if (gps_info.STATUS == 'A') {
                    save_gps(GPS_REPORT_DATA_FILE ,cmdbuf,"w+");
                }
                    report_ts = system_time_get();
                }
								printf("system_time_get:%ld get_gps_timestamp:%ld\n",time(NULL),get_gps_timestamp());
                if( time(NULL) - get_gps_timestamp() > 90 * 86400) {
                    // rotate 
                    // log when time exceeds 90 days,
                    // remove old log
                    touch_gps_timestamp();
                    //fclose(fp);
                    unlink(GPS_DATA_FILE_OLD);
                    rename(GPS_DATA_FILE, GPS_DATA_FILE_OLD);
                    printf("start backup old gps_data and restart to write\n");
                    #if 0
                    fp = fopen(GPS_DATA_FILE, "a+");
                    if (NULL == fp) {

                        perror("unable to open second save gps_data");
                        return -1;

                    }
										#endif
                }
                bzero((void*)buf, sizeof(buf));
            }
            else
            {
                i = 0;
                bzero((void*)buf, sizeof(buf));
            }
            
        }		
    }
    //fclose(fp);
    printf("exit from reading gps\n");

    return 0;
}

void sighandle()
{
	
    flag = 1;
    msg(M_INFO, " gps  abort!\n");
   // system("echo 2000 > /dev/h895x_led");
    printf("gps  abort!\n");
    
    _exit(0);
	}
int main()
{
		int ret;
    struct termios options;
    struct tm *gmtime(const time_t * timep);
    
    openmsg1(MSG_EASY, "/var/log/temp/gps_err.log", "gps", 90628);
    fd = open(dev, O_RDWR);
    if (fd < 0) {
        
        system("echo 2000 > /dev/h895x_led");
        perror(dev);
        msg(M_INFO, " serial open error!");
        printf("serial open error!\n");
        exit(-1);
    }
    tcgetattr(fd, &options);
    options.c_cflag = baud | CRTSCTS | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    options.c_cc[VMIN] = 1;
    options.c_cc[VTIME] = 0;

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &options);
    signal(SIGINT,sighandle);
    if(flag == 1)
    {
    
     system("echo 2000 > /dev/h895x_led");
    }
    ret = receive(&gps_info);
    //exit(receive(&gps_info));
    exit(ret);
    closemsg1();
}
