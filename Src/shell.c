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

  if (strlen(token) == 3 && strncmp(token, "imu", 1) == 0)
  {
    IMUecho(TRUE);
  }
  else if (strlen(token) == 3 && strncmp(token, "gps", 1) == 0)
  {
    GPSecho(TRUE);
  }
  else if (strlen(token) == 3 && strncmp(token, "env", 1) == 0)
  {
    ENVecho(TRUE);
  }
  else if (strlen(token) == 6 && strncmp(token, "matlab", 1) == 0)
  {
    MATLABecho(TRUE);
  }
  else if (strlen(token) == 5 && strncmp(token, "range", 1) == 0)
  {
    RangeEcho(TRUE);
  }
  else if (strlen(token) > 0)
  {
    printf("command not found: %s\r\n", string);
  }
}
