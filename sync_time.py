#!/usr/bin/env python
#-*- coding: utf-8 -*-
import subprocess
import gps_report
if __name__ == '__main__':
    try:
        log_info = gps_report.get_gps_data()
        time = log_info['date']
    except:
        pass
    if time:
        cmd = 'date -s "%s"' % time
        subprocess.Popen(cmd, shell=True)

