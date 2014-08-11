/*************************************************************************
	> File Name: gps_main.h
	> Author: yzliu
# Copyright  (C), date, Hongdian Tech. Co., Ltd.
	> Created Time: 2014年06月06日 星期五 16时41分31秒
 ************************************************************************/

#include<termios.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<time.h>
#include <string.h>

#include<signal.h>
//#include "Python.h"
#include "debug.h"


#ifndef _GPS_H
#define _GPS_H

#define dev "/dev/ttymxc0"
#define GPS_TS_FILE "/mnt/disk/.gps_timestamp"
#define GPS_DATA_FILE "/mnt/disk/gps_data"
#define GPS_DATA_FILE_OLD "/mnt/disk/gps_data.old"
#define GPS_REPORT_DATA_FILE "/tmp/gps_data"

//struct tm *gmtime(const time_t *timep);
//struct tm *localtime(const time_t *timep);


typedef struct{
	//date_time D;
	struct tm D;
	double latitude;  
	double longitude;
	char NS;    //南北纬
	char EW;    //东西经
	char STATUS; //GPS状态
	float SPEED; //速度
	float height;//高度
}GPS_INFO;

void gps_parse(char *line,GPS_INFO *GPS);
void show_gps(GPS_INFO *GPS);
FILE *save_gps(char  *filepath ,char *buf,char *mode); 

#endif



