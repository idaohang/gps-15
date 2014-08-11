#!/usr/bin/env python
#-*- coding: utf-8 -*-
import sys
import os
import serial
import time
import subprocess

def get_parent_dir(filepath, level = 1):
    """
    Get parent directory with given return level.
    
    @param filepath: Filepath.
    @param level: Return level, default is 1
    @return: Return parent directory with given return level. 
    """
    parent_dir = os.path.realpath(filepath)
    while level > 0:
        parent_dir = os.path.dirname(parent_dir)
        level -= 1

    return parent_dir


sys.path.append(get_parent_dir(__file__, 2))
from server import send_message


def report_gps():
	file =open("/tmp/gps_data")
    SAVE_SYS_LOG = TRUE
    import syslog
    syslog.openlog('wifibox-gps',syslog.LOG_PID)
    def save_syslog(msg)
        if SAVE_SYSLOG:
            syslog.syslog(msg)
	log_info =file.readline()
	
    while log_info:
		#print log_info,
		log_file =file.readline()

	if log_info:
		try:
			send_message('gps', log_info)
            save_syslog('send gps-heartbeat')
		except:
			print 'append log for gps heartbeat failed\n'
            save_syslog('send gps-heartbeat failed...')
	else:
		try:
			send_message('gps', {'gps': 'get gps info failed'})
            save_syslog('send get gps info failed')
		except:
			print 'append log for gps exception failed\n'
            save_syslog('send gps exception failed')
    syslog.closelog()
		file.close()


if __name__ == '__main__':
    report_gps()

