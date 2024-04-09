
* To see the skeleton's code of a linux daemon: [__Basic skeleton of a linux daemon written in C__](https://github.com/pasce/daemon-skeleton-linux-c)

# Basic skeleton of a linux daemon written in C

Daemons work in the background and (usually...) don't belong to a TTY that's why you can't use stdout/stderr in the way you probably want.
Usually a syslog daemon (_syslogd_) is used for logging messages to files (debug, error,...).

## Compile and run
 - Compile the code: `gcc -o thisIsNotAKeylogger daemon-keylogger.c`
 - Start the daemon: `./keylogger-daemon`
 - Check if everything is working properly: `ps -xj | grep keylogger-daemon`

## Test the output
 - The output should be similar to this one:
<pre>
+------+------+------+------+-----+-------+------+------+------+-----+
| PPID | PID  | PGID | SID  | TTY | TPGID | STAT | UID  | TIME | CMD |
+------+------+------+------+-----+-------+------+------+------+-----+
|    1 | 3387 | 3386 | 3386 | ?   |    -1 | S    | 1000 | 0:00 | ./  |
+------+------+------+------+-----+-------+------+------+------+-----+
</pre>

## How to run the server

__What you should see here is:__

 - The daemon has no controlling terminal (__TTY = ?__)
 - The parent process ID (__PPID__) is __1__ (The init process)
 - The __PID != SID__ which means that our process is NOT the session leader<br>
   (because of the second fork())
 - Because PID != SID our process __can't take control of a TTY again__

__Reading the syslog:__

 - Locate your syslog file. Mine is here: `/var/log/syslog`
 - Do a: `grep firstdaemon /var/log/syslog`

 - The output should be similar to this one:
<pre>
<time> <user> firstdaemon[3387]: First daemon started.
<time> <user> firstdaemon[3387]: First daemon terminated.
</pre>
