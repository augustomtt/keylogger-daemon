![keyloggerdaemon](https://github.com/augustomtt/keylogger-daemon/assets/65096407/c98b380d-d2b8-4e65-82ad-36c2c03691f1)

* To see the skeleton's code of a linux daemon: [__Basic skeleton of a linux daemon written in C__](https://github.com/pasce/daemon-skeleton-linux-c)

# Basic skeleton of a linux daemon keylogger written in C for academic purposes

Daemons work in the background and (usually...) don't belong to a TTY that's why you can't use stdout/stderr in the way you probably want.
Usually a syslog daemon (_syslogd_) is used for logging messages to files (debug, error,...).

Keystroke logging, often referred to as keylogging or keyboard capturing, is the action of recording (logging) the keys struck on a keyboard, typically covertly, so that a person using the keyboard is unaware that their actions are being monitored. Data can then be retrieved by the person operating the logging program. 

## Compile and run
 - Compile the code: `gcc daemon-keylogger.c -o keyloggerd`
 - Start the daemon: `sudo ./keyloggerd`
 - Check if everything is working properly: `ps -xj | grep keyloggerd`
 - A simplified way to check if it's running without using grep is `ps -e`, the process may be found at the bottom of the results

## Test the output
 - The output should be similar to this one:
<pre>
+------+------+------+------+-----+-------+------+------+------+-----+
| PPID | PID  | PGID | SID  | TTY | TPGID | STAT | UID  | TIME | CMD |
+------+------+------+------+-----+-------+------+------+------+-----+
|    1 | 3387 | 3386 | 3386 | ?   |    -1 | S    | 1000 | 0:00 | ./  |
+------+------+------+------+-----+-------+------+------+------+-----+
</pre>

__What you should see here is:__

 - The daemon has no controlling terminal (__TTY = ?__)
 - The parent process ID (__PPID__) is __1__ (The init process)
 - The __PID != SID__ which means that our process is NOT the session leader<br>
   (because of the second fork())
 - Because PID != SID our process __can't take control of a TTY again__
 
__Reading the syslog:__

 - Locate your syslog file. Mine is here: `/var/log/syslog`
 - Do a: `grep keyloggerd /var/log/syslog`
 - You may find a line `"Keylogger working correctly"` 

## How to start the keylogger daemon
- After compiling the code, run it using `sudo ./keylogger`
- All your keystrokes will be logged to a .txt file in the same directory.
- After a specific amount of keystrokes (configurable) an HTTP request will be sent to a local running server.

## How to run the server
- Install dependencies using `npm install`
- Run with `npm start`

# How the server works
- It saves the raw text sent by the daemon to a txt file located in the server's path
______
*This repository was created as a required project for our University, as stated before please keep in mind a keylogger is considered malicious software.*
