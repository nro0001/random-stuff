//Created by: Nick Openshaw
//Auburn University - COMP3500

#include <unistd.h>     // getpid(), getcwd()
#include <sys/types.h>  // type definitions, e.g., pid_t
#include <sys/wait.h>   // wait()
#include <signal.h>     // signal name constants and kill()
#include <sys/stat.h>

#include <fcntl.h> 
#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <string>
#include <cstring>
using namespace std;

//Comment for Main method
int main()
{
  while ( true )
    {
      // Show prompt.
      //cout << get_current_dir_name () << "]$ " ;
      char buffer[100];
      char *answer = getcwd(buffer, sizeof(buffer));
      string currdir;
      if (answer)
        {
           currdir = answer;
        }
      cout << currdir << "]$ " ;
      char command[256];
      cin.getline( command, 256 );
      
      vector<char*> args;
      char* prog = strtok( command, " " );
      char* tmp = prog;
      int numstrings = 0;
      while ( tmp != NULL )
	{
	  args.push_back( tmp );
	  tmp = strtok( NULL, " " );
          numstrings++;
	}
      
      char** argv = new char*[args.size()+1];
      for ( int k = 0; k < args.size(); k++ )
	argv[k] = args[k];
      
      argv[args.size()] = NULL;
      
      //Check entered command for exit condition
      if ( strcmp( command, "exit" ) == 0 )
	{
	  return 0;
	}

      //Continue if not exiting
      else
	{
          //if(numstrings >= 2)
               //cout << "Second to last element in ARGV: " << argv[numstrings - 2] << "\n";
          
          //Check first argument for "cd" - change directory
	  if (strcmp (prog, "cd") == 0)
	    {
	      if (argv[1] == NULL)
		{
		  chdir ("/");
		}
	      else 
		{
		  int status = chdir (argv[1]);
                  if(status != 0)
                      perror (command);
//                currdir = $CWD + argv[1];
                  
		}    
	    }

          //Check for Type 1 line form
          else if (numstrings >= 2 && strcmp(argv[numstrings - 2], ">") == 0)
            {
              //cout << "Taking Type 1 Command Format\n";
              
              //Fork for Child Process 1
              pid_t kidpid = fork();

	      if (kidpid < 0)
		{
		  perror( "Internal error: cannot fork." );
		  return -1;
		}
	      else if (kidpid == 0)
		{
		  // I am the child. Execute the inputted program here.
                  //cout << "Executing a child 1......before execvp\n";
                  //cout << "Executing from command......" << *prog << "\n";

                  //char** newargs = new char*[numstrings];
                  //for ( int k = 0; k < numstrings - 3; k++ )
                  //     newargs[k] = argv[k+1];

		  //for ( int i = 0; i < numstrings; i++ )
                  //    cout << "New Arguments to pass to child 1 process: " << newargs[i] << "\n";
		  

                  //ofstream myfile;
                  //myfile.open ("output.txt");
                  //myfile << "Writing this to a file.\n";
                  //myfile.close();

                  string filename = argv[numstrings-1];

                  int fd = open(filename.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                  
                  if (fd != -1)
                    {
                        // make stdout go to file
                        dup2(fd, 1);   
                        // make stderr go to file
                        dup2(fd, 2);   
                
                        close(fd);

                        execvp (prog, argv);
		        cout << "Executing a child 1......after execvp\n";
                     }
                  else
                     {
		        perror( command );
                     }
		}
	      else
		{
		  // I am the parent.  Wait for the child to finish before showing "$".
		  //cout << "Executing a parent.....before wait\n";
                  if ( waitpid( kidpid, 0, 0 ) < 0 )
		    {
		      perror( "Internal error: cannot wait for child." );
		      return -1;
		    }
                  //cout << "Executing a parent.....after wait\n";
		}
              
            }

          //Check for Type 2 line form
	  else if (prog != NULL)
	    {
	      //cout << "Taking Type 2 Command Format\n";

              //Fork for Child Process 2
              pid_t kidpid = fork();

	      if (kidpid < 0)
		{
		  perror( "Internal error: cannot fork." );
		  return -1;
		}
	      else if (kidpid == 0)
		{
		  // I am the child. Execute the inputted program here.
                  //cout << "Executing a child 2......before execvp\n";
                  //cout << "Executing from command......" << *prog << "\n";

                  //char** newargs = new char*[numstrings];
                  //for ( int k = 0; k < numstrings; k++ )
                  //     newargs[k] = argv[k+1];

		  //for ( int i = 0; i < numstrings; i++ )
                  //     cout << "New Arguments to pass to child 2 process: " << newargs[i] << "\n";

		  execvp (prog, argv);
		  //cout << "Executing a child 2......after execvp\n";

		  // The following lines should not happen (normally).
		  perror( command );
		  return -1;
		}
	      else
		{
		  // I am the parent.  Wait for the child to finish before showing "$".
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