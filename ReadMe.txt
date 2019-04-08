========================================================================
       WIN32 APPLICATION :	NetTrafficMon 1.05
	   Author:				Marius Samoila
	   Contact:				msamoila2004@yahoo.com
						    https://sites.google.com/site/msamoilaweb/
========================================================================

This program may be used to monitor the Internet traffic on a computer.
It shows how many bytes were uploaded / downloaded. 
It might be useful to avoid extra charge if the Internet Service Provider sets a monthly traffic limit.


In order to automatically start the program in background a shortcut may be added in StartUp menu.
To start the program without user interface, the switch /s must be used. In this case only the icon
in the taskbar's status area will be visible. The main window still can be displayed if user clicks
on this icon.

The counters are reset daily, monthly or on specific date.
When monthly reset is configured user must specify the day of the month to reset.
If user chooses a date which is too big for the current month (such as 31st for February)
then for the current month the day will be adjusted to the last day in month.

May 03th 2015 version 1.05.3

1. Update the developer contact information in the "About ..." dialog box and in this ReadMe file.

February 26th 2014 version 1.05.2

1. Use 3 decimal points when displaying numbers between 1MB and 2MB and 2 decimal points for munbers between 2MB and 20MB.

February 24th 2011 version 1.05

1. Ability to log traffic data into a comma separated values (CSV) file for further processing in Excel.
2. Fix: counters were not always reset at due date.

October 12nd 2006 version 1.04

1. If the network connection is lost, then recovered while NetTrafficMon is running, then
the "TOTAL" values were screwed up
(suddenly the traffic values jump to gigabytes as soon as the network connection is restored)

August 6th, 2006 version 1.03

1. Update About box.

November 15th 2005
Fixes for version 1.02:
1. The "IN" and "OUT" limits were not saved properly if user tries to set different values in "Settings" dialog.
2. If the bytes number goes over 1GB, then two decimals are displayed (ex: 1.23 GB instead of 1 GB).


October 4th 2005
Fixes for version 1.01:

1. A memory leak while getting the IP address
2. The pathname for log file not updated in settings dialog after 'open file' dialog is closed.

