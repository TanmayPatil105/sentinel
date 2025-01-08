/* main.c
 *
 * Copyright 2025 Tanmay Patil <tanmaynpatil105@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ptrace.h>


#define ISWATCHFLAG(arg) \
        (strcmp (arg, "-w") == 0)

/* Default to bash */
#define SHELL_EXEC_PATH "/bin/bash"

/* Colors */
#define GREEN "\033[0;32m"
#define RESET "\033[0;m"

static void
print_pid ()
{
  pid_t self;

  self = getpid ();

  printf ("PID: %s%u%s\n", GREEN, self, RESET);
}

static pid_t
watch_who (char *arg)
{
  pid_t who;

  who = strtol (arg, NULL, 0);

  return who;
}

static bool
watch_and_wait (pid_t counterpart)
{
  int status = -1;
  int ret;
  bool succeeded = false;

  /* Attach process */
  ret = ptrace (PTRACE_ATTACH, counterpart,
                NULL, NULL);
  if (ret < 0)
    return succeeded;

  /* wait for process to exit */
  while (waitpid (counterpart, &status, 0) >= 0)
    {
      if (WIFEXITED (status))
        {
          if (WEXITSTATUS (status) == 0)
            succeeded = true;
          break;
        }
      else if (WIFSTOPPED (status))
        {
          ptrace (PTRACE_CONT, counterpart,
                  NULL, NULL);
        }
      else if (WIFSIGNALED (status))
        {
          succeeded = false;
          break;
        }
    }

  /* Detach process and let it exit */
  ret = ptrace (PTRACE_DETACH, counterpart,
                NULL, NULL);

  return succeeded;
}

static void
execute_cmd (char *bash_cmd)
{
  char *argv[4] = {SHELL_EXEC_PATH, "-c",
                   NULL, NULL};

  argv[2] = bash_cmd;

  /* replace current process */
  execv (argv[0], argv);
}

int
main (int   argc,
      char *argv[])
{
  bool watch = false;
  char *bash_cmd = NULL;
  pid_t counterpart;

  for (int i = 1; i < argc; i++)
    {
      if (ISWATCHFLAG (argv[i]))
        {
          i++;
          if (i < argc)
            {
               counterpart = watch_who (argv[i]);
               watch = true;
            }
        }
      else
        {
          bash_cmd = argv[i];
        }
    }

  if (bash_cmd == NULL)
    {
      printf ("Provide a command to execute\n");

      return 1;
    }

  /* Print self PID
   * This helps in creating a chain of dependent
   * process without waiting for someone to complete */
  print_pid ();

  /*
   * Watch the couterpart process and only proceed
   * if it succeeds */
  if (watch && !watch_and_wait (counterpart))
    {
      printf ("process %u exited unexpectedly\n", counterpart);

      return 1;
    }

  execute_cmd (bash_cmd);

  /* We will never reach here */
  return 0;
}
