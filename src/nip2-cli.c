/* nip2-cli.c ... run the nip2 executable, connecting stdin and stdout to the
 * console
 */

/*

    Copyright (C) 2008 Imperial College, London

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 */

/*

    These files are distributed with VIPS - http://www.vips.ecs.soton.ac.uk

 */

/* Adapted from sample code by Leo Davidson, with the author's permission.
 */

/* Windows does not let a single exe run in both command-line and GUI mode. To
 * run nip2 in command-line mode, we run this CLI wrapper program instead,
 * which starts the main nip2 exe, connecting stdin/out/err appropriately.
 */

/*
#define DEBUG
 */

#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <ctype.h>

#include <glib.h>

/* The largest command-line we can build.
 */
#define MAX_COMMAND (2048)

int
main (int argc, char **argv)
{
  char *dirname;
  char command[MAX_COMMAND];
  gboolean quote;
  int i, j;

  HANDLE hChildStdoutRd;
  HANDLE hChildStdoutWr;
  SECURITY_ATTRIBUTES saAttr;

  PROCESS_INFORMATION processInformation;
  STARTUPINFO startUpInfo;

  DWORD dwRead;
  CHAR buf[1024];

  /* we run the nip2.exe in the same directory as this exe: swap the last 
   * pathname component for nip2.exe
   * we change the argv[0] pointer, probably not a good idea
   */
  dirname = g_path_get_dirname (argv[0]);
  argv[0] = g_build_filename (dirname, "nip2.exe", NULL);
  g_free (dirname);

  if (_access (argv[0], 00))
    {
      fprintf (stderr, "Cannot access \"%s\"\n", argv[0]);
      exit (1);
    }

  /* build the command string ... we have to quote items containing spaces
   */
  command[0] = '\0';
  for (i = 0; i < argc; i++)
    {
      quote = FALSE;
      for (j = 0; argv[i][j]; j++)
	{
	  if (isspace (argv[i][j]))
	    {
	      quote = TRUE;
	      break;
	    }
	}
      if (i > 0)
	{
	  strncat (command, " ", MAX_COMMAND - 1);
	}
      if (quote)
	{
	  strncat (command, "\"", MAX_COMMAND - 1);
	}
      strncat (command, argv[i], MAX_COMMAND - 1);
      if (quote)
	{
	  strncat (command, "\"", MAX_COMMAND - 1);
	}
    }

  if (strlen (command) == MAX_COMMAND - 1)
    {
      fprintf (stderr, "Command too long\n");
      exit (1);
    }

#ifdef DEBUG
  printf ("running: %s\n", command);
#endif /*DEBUG*/

  /* Create a pipe for the child process's STDOUT. 
   */
  hChildStdoutRd = NULL;
  hChildStdoutWr = NULL;
  saAttr.nLength = sizeof (SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = TRUE;
  saAttr.lpSecurityDescriptor = NULL;
  if (!CreatePipe (&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0))
    {
      fprintf (stderr, "CreatePipe failed: %d\n", GetLastError ());
      exit (1);
    }

  /* run command
   */
  startUpInfo.cb = sizeof (STARTUPINFO);
  startUpInfo.lpReserved = NULL;
  startUpInfo.lpDesktop = NULL;
  startUpInfo.lpTitle = "nip2 (Batch)";
  startUpInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
  startUpInfo.hStdOutput = hChildStdoutWr;
  startUpInfo.hStdError = hChildStdoutWr;
  startUpInfo.cbReserved2 = 0;
  startUpInfo.lpReserved2 = NULL;
  startUpInfo.wShowWindow = SW_SHOWNORMAL;
  if (!CreateProcess (NULL, command, NULL,	/* default security */
		      NULL,			/* default thread security */
		      TRUE,			/* inherit handles */
		      CREATE_DEFAULT_ERROR_MODE | NORMAL_PRIORITY_CLASS | 
		      	DETACHED_PROCESS, 
		      NULL,			/* use default environment */
		      NULL,			/* set default directory */
		      &startUpInfo, &processInformation))
    {
      fprintf (stderr, "Failed to run command \"%s\", error %d\n", command,
	       GetLastError ());
      exit (1);
    }

  /* Close the write end of the pipe before reading from the read end.
   */
  CloseHandle (hChildStdoutWr);
  hChildStdoutWr = NULL;

  while (ReadFile (hChildStdoutRd, buf, sizeof (buf) - 1, &dwRead, NULL) &&
	 dwRead > 0)
    {
      buf[dwRead] = '\0';
      printf ("%s", buf);
    }

  /* WaitForSingleObject(processInformation.hProcess, INFINITE); */
  CloseHandle (hChildStdoutRd);
  hChildStdoutRd = NULL;

  if (hChildStdoutRd)
    CloseHandle (hChildStdoutRd);
  if (hChildStdoutWr)
    CloseHandle (hChildStdoutWr);

  return (0);
}
