/*************************************************************************
	> File Name: gps.c
	> Author: yzliu
	> Copyright  (C), date, Hongdian Tech. Co., Ltd.
	> Created Time: 2014年06月06日 星期五 16时41分15秒
 ************************************************************************/

#include<stdio.h>
#include"gps_main.h"

static int GetComma(int num, char *str);
static void UTC2BTC(struct tm * GPS);
static double get_double_number(char *s);

void show_gps(GPS_INFO * GPS)
{

	printf("DATE	 :%d-%02d-%02d \n", GPS->D.tm_year, GPS->D.tm_mon,
	       GPS->D.tm_mday);
	printf("TIME	 :%02d:%02d:%02d \n", GPS->D.tm_hour, GPS->D.tm_min,
	       GPS->D.tm_sec);
	printf("Latitude :%2.14f %c\n", GPS->latitude, GPS->NS);
	printf("Longitude:%2.14f %c\n", GPS->longitude, GPS->EW);
	printf("status:%c\n", GPS->STATUS);
	printf("speed:%3.2f\n", (GPS->SPEED) * 1.852);
	printf("heihght:%4.1f\n", GPS->height);
  usleep(200000);
}




/*
 * **解释gps发出的数据
 * **0      7  0   4 6   0     6 8 0        90         0  3      0        9   
 * **$GPRMC,091400,A,3958.9870,N,11620.3278,E,000.0,000.0,120302,005.6,W*62  
 * **$GPRMC,022713.000,A,2239.0487,N,11406.5127,E,0.00,303.87,110714,,,A*64
 * */
void gps_parse(char *line, GPS_INFO * GPS)
{

	int i, tmp;
	char c;
	char *buf = line;
	time_t t;
	
	
	
	c = buf[5];
	struct tm *area, *gmt;
	
	

	if (c == 'C') {		//$GPRMC
		GPS->D.tm_hour = (buf[7] - '0') * 10 + (buf[8] - '0');	//原始数据串中第7位（从0计起）为时钟的十位，而8为个位
		GPS->D.tm_min = (buf[9] - '0') * 10 + (buf[10] - '0');	//  9               分钟          10
		GPS->D.tm_sec = (buf[11] - '0') * 10 + (buf[12] - '0');	//  11              秒钟          12
		tmp = GetComma(9, buf);
		GPS->D.tm_mday =
		    (buf[tmp + 0] - '0') * 10 + (buf[tmp + 1] - '0');
		GPS->D.tm_mon =
		    (buf[tmp + 2] - '0') * 10 + (buf[tmp + 3] - '0');
		GPS->D.tm_year =
		    (buf[tmp + 4] - '0') * 10 + (buf[tmp + 5] - '0') + 2000;
		GPS->STATUS = buf[GetComma(2, buf)];
		GPS->SPEED = get_double_number(&buf[GetComma(7, buf)]);
		GPS->latitude = get_double_number(&buf[GetComma(3, buf)]) / 100;
		GPS->NS = buf[GetComma(4, buf)];
		GPS->longitude = get_double_number(&buf[GetComma(5, buf)]) / 100;
		GPS->EW = buf[GetComma(6, buf)];

		UTC2BTC(&GPS->D);
		
	}
	
		if (c == 'A') {		//"$GPGGA"		
						if(buf[4] == 'G'){
					GPS->height = get_double_number(&buf[GetComma(9, buf)]);
				}
		}

}

/**
 * *获得在字符串s中第一个逗号前的数值，并转换为浮点数返回
 * */
static double get_double_number(char *s)
{
	char buf[128];
	int i;
	double rev;
	bzero((void*)buf, sizeof(buf));
	i = GetComma(1, s);
	strncpy(buf, s, i);
	buf[i] = '\n';
	rev = atof(buf);
	return rev;
}

/**
 * *返回str中第num个逗号的下一位置(从0起)
 * */
static int GetComma(int num, char *str)
{
	int i, j = 0;
	int len = strlen(str);
	for (i = 0; i < len; i++) {
		if (str[i] == ',')
			j++;
		if (j == num)
			return (i + 1);
	}
	return 0;
}

FILE *save_gps(char *filepath ,char *buf,char *mode){
	FILE *fp;
	fp = fopen(filepath, mode);
                    if (NULL == fp) {
                        perror("unable to open report gps_data");
                        msg(M_INFO, "file [%s] open error", GPS_DATA_FILE);
                        fclose(fp);
                        return -1;

                    }
                    fputs(buf, fp);
                    fclose(fp);
                    return fp;
	
	}

/**
 * ˀ½花¼嗪»»Ϊ±±¾©ʱ¼∠**/
static UTC2BTC(struct tm *GPS)
{

	GPS->tm_sec++;		//如果秒号先出,再出时间数据,则将时间数据+1秒
	if (GPS->tm_sec > 59) {
		GPS->tm_sec = 0;
		GPS->tm_min++;
		if (GPS->tm_min > 59) {
			GPS->tm_min = 0;
			GPS->tm_hour++;
		}
	}
	GPS->tm_hour += 8;
	if (GPS->tm_hour > 24) {
		GPS->tm_hour -= 24;
		GPS->tm_mday++;
		if (GPS->tm_mon == 2 || GPS->tm_mon == 4 || GPS->tm_mon == 6
		    || GPS->tm_mon == 9 || GPS->tm_mon == 11) {
			if (GPS->tm_mday > 30) {
				GPS->tm_mday = 1;
				GPS->tm_mon++;
			}
		} else {
			if (GPS->tm_mday > 31) {
				GPS->tm_mday = 1;
				GPS->tm_mon++;
			}

		}
		if ((GPS->tm_year % 4 == 0)
		    && (GPS->tm_year % 400 == 0 || GPS->tm_year % 100 != 0))
			if (GPS->tm_mday > 29 && GPS->tm_mon == 2) {
				GPS->tm_mday = 1;
				GPS->tm_mon++;
			}
	}
	if (GPS->tm_mon > 12) {
		GPS->tm_mon -= 12;
		GPS->tm_year++;
	}
}
