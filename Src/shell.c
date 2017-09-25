/*
 * shell.c
 *
 *  Created on: Sep 24, 2017
 *      Author: jsjolund
 */

#include "shell.h"

void ShellExecute(char* string)
{
  // Parse a user command
  char buffer[101];
  sprintf(buffer, string);
  char *token = strtok(buffer, " \t");

  uint8_t fail = 1;

  if (strlen(token) == 0)
  {
    fail = 0;
  }
  else if (strlen(token) == 3 && strncmp(token, "imu", 1) == 0)
  {
    fail = 0;
    IMUecho(TRUE);
  }
  else if (strlen(token) == 3 && strncmp(token, "gps", 1) == 0)
  {
    fail = 0;
    GPSecho(TRUE);
  }
  if (fail)
  {
    printf("command not found: %s\r\n", string);
  }
}
