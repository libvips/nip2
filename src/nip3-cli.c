/* nip2-cli.c ... run the nip2 executable, connecting stdin and stdout to the
 * console
 *
 * 11/12/09
 * 	- use SetHandleInformation() to stop the child inheriting the read
 * 	  handle (thanks Leo)
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

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

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

#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <ctype.h>

#include <glib.h>

void
print_last_error ()
{
  char *buf;

  if (FormatMessageA (FORMAT_MESSAGE_ALLOCATE_BUFFER |
		      FORMAT_MESSAGE_IGNORE_INSERTS |
		      FORMAT_MESSAGE_FROM_SYSTEM,
		      NULL,
		      GetLastError (),
		      MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
		      (LPSTR) & buf, 0, NULL))
    {
      fprintf (stderr, "%s", buf);
      LocalFree (buf);
    }
}

int
main (int argc, char **argv)
{
  char *dirname;
  char command[2048];
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
      fprintf (stderr, "cannot access \"%s\"\n", argv[0]);
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
	  strncat (command, " ", sizeof (command) - 1);
	}
      if (quote)
	{
	  strncat (command, "\"", sizeof (command) - 1);
	}
      strncat (command, argv[i], sizeof (command) - 1);
      if (quote)
	{
	  strncat (command, "\"", sizeof (command) - 1);
	}
    }

  if (strlen (command) == sizeof (command) - 1)
    {
      fprintf (stderr, "command too long\n");
      exit (1);
    }

  /* Create a pipe for the child process's STDOUT. 
   */
  hChildStdoutRd = NULL;
  hChildStdoutWr = NULL;
  saAttr.nLength = sizeof (SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = TRUE;
  saAttr.lpSecurityDescriptor = NULL;
  if (!CreatePipe (&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0))
    {
      fprintf (stderr, "CreatePipe failed: ");
      print_last_error ();
      fprintf (stderr, "\n");
      exit (1);
    }

  /* Ensure the read handle to the pipe for STDOUT is not inherited.
   */
  if (!SetHandleInformation(hChildStdoutRd, HANDLE_FLAG_INHERIT, 0))
    {
      fprintf (stderr, "SetHandleInformation failed: ");
      print_last_error ();
      fprintf (stderr, "\n");
      exit (1);
    }

  /* Run command.
   */
  startUpInfo.cb = sizeof (STARTUPINFO);
  startUpInfo.lpReserved = NULL;
  startUpInfo.lpDesktop = NULL;
  startUpInfo.lpTitle = NULL;
  startUpInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
  startUpInfo.hStdOutput = hChildStdoutWr;
  startUpInfo.hStdError = hChildStdoutWr;
  startUpInfo.cbReserved2 = 0;
  startUpInfo.lpReserved2 = NULL;
  startUpInfo.wShowWindow = SW_SHOWNORMAL;
  if (!CreateProcess (NULL, command, NULL,	/* default security */
		      NULL,	/* default thread security */
		      TRUE,	/* inherit handles */
		      CREATE_DEFAULT_ERROR_MODE | DETACHED_PROCESS, NULL,	/* use default environment */
		      NULL,	/* set default directory */
		      &startUpInfo, &processInformation))
    {
      fprintf (stderr, "error running \"%s\": ", command);
      print_last_error ();
      fprintf (stderr, "\n");
      exit (1);
    }

  /* Close the write end of the pipe before reading from the read end.
   */
  CloseHandle (hChildStdoutWr);

  while (ReadFile (hChildStdoutRd, buf, sizeof (buf) - 1, &dwRead, NULL) &&
	 dwRead > 0)
    {
      buf[dwRead] = '\0';
      printf ("%s", buf);
    }

  CloseHandle (hChildStdoutRd);

  return (0);
}
