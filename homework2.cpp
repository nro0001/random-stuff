#include <unistd.h>     // getpid(), getcwd()
#include <sys/types.h>  // type definitions, e.g., pid_t
#include <sys/wait.h>   // wait()
#include <signal.h>     // signal name constants and kill()
#include <iostream>
#include <vector>
#include <string>
using namespace std;


int main()
{
  while ( true )
    {
      // Show prompt.
      cout << get_current_dir_name () << "$ " ;
      char command[256];
      cin.getline( command, 256 );
      
      vector<char*> args;
      char* prog = strtok( command, " " );
      char* tmp = prog;
      while ( tmp != NULL )
	{
	  args.push_back( tmp );
	  tmp = strtok( NULL, " " );
	}
      
      char** argv = new char*[args.size()+1];
      for ( int k = 0; k < args.size(); k++ )
	argv[k] = args[k];
      
      argv[args.size()] = NULL;
      
      if ( strcmp( command, "exit" ) == 0 )
	{
	  return 0;
	}
      else
	{
	  if (!strcmp (prog, "cd"))
	    {
	      if (argv[1] == NULL)
		{
		  chdir ("/");
		}
	      else
		{
		  chdir (argv[1]);
		}
	      perror (command);
	    }
	  else
	    {
	      pid_t kidpid = fork();

	      if (kidpid < 0)
		{
		  perror( "Internal error: cannot fork." );
		  return -1;
		}
	      else if (kidpid == 0)
		{
		  // I am the child.
		  execvp (prog, argv);
		  
		  // The following lines should not happen (normally).
		  perror( command );
		  return -1;
		}
	      else
		{
		  // I am the parent.  Wait for the child.
		  if ( waitpid( kidpid, 0, 0 ) < 0 )
		    {
		      perror( "Internal error: cannot wait for child." );
		      return -1;
		    }
		}
	    }
	}
    }
  
  return 0;
}