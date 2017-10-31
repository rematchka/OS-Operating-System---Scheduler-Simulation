#include "clkUtilities.h"
#include "queueUtilities.h"
#include <unistd.h>
#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <stdio.h>
#include <sys/wait.h>
#include<stdlib.h>
using namespace std;

void sleepplease(int x)
{
  int clll=getClk();
   int time=clll+x;
   while(time>getClk()){}
   printf("clcok in parent%d\n",getClk());
}

class logger 
{
 FILE * fptr;
 const char* fname; 

public:

    logger(const char* fn1ame)
    {  fname=fn1ame;
    }
    
    void logProcess(processData process,int curtime)
    {
       // int pid,arrtime,runtime,remain,wait;
         int wait;
         double wta;
         int ta;
         const char * states[4]={"started","resumed","stopped","finished"};
        printf("printing logs\n");
        printf("state %d\n",process.state);
         switch(process.state)
         {
            case 0:
            {
                //state="started";
                wait=process.startOfExecution-process.arrivalTime;
                break;
            }
            case 1:
            {
                //state="resumed";
                 wait=process.startOfExecution-process.finishTime;
                break;
            }
            case 2:
            {
                //state="stopped";
                wait=0; //TODO: REVISE THAT CONDITION
                break;
            }
           case 3:
            {
                //state="finished";
                printf("in case 3 \n");
                wait=0;
               printf("%d %d\n",process.finishTime,process.arrivalTime);
                ta=process.finishTime-process.arrivalTime;
                wta=double(ta)/process.runTime;
                break;
            }
            default:
             break;
         }

//printf("hello file name is %s\n",fname);
   int remin=process.runTime-(curtime-process.startOfExecution);
     fptr=fopen(fname,"a");
         if(fptr!=NULL&&process.state==3)

      {  
	rewind(fptr);
 fprintf(fptr, "At time %d process %d %s arr %d total %d remain %d wait %d TA %d WTA %.2f \n",process.finishTime,process.pid,states[process.state],process.arrivalTime,process.runTime,remin,wait,ta,wta);

 fclose(fptr);
printf("almost there \n");
}
          else if(fptr!=NULL)
          {printf("almost there 2\n");
 rewind(fptr);
  fprintf(fptr, "At time %d process %d %s arr %d total %d remain %d wait %d \n",process.startOfExecution,process.pid,states[process.state],process.arrivalTime,process.runTime,remin,wait);
printf("almost there 2\n");
 fclose(fptr);
}

    }

    ~logger()
    {    
       // fclose(fptr);
       printf("closed el bta3 el file\n");
    }

};


logger schedulerLogger("RR.txt");
struct processData pD;
 bool running_process=false;
    struct processData current_running;
    queue<processData>pq;

    struct sigaction sa;
int quant;
void my_sigchld_handler(int sig)
{
    pid_t p;
    int status;

    while ((p=waitpid(-1, &status, WNOHANG)) != -1)
    {
      
     
      current_running.stopTime= getClk();     
       pD=current_running;
      //do some calculation and printing 
      if(p!=0)
      {running_process=false;
        current_running.state=3;
        current_running.finishTime=getClk();  
       printf("child is dead %did %d\n",p,current_running.id);
      // schedulerLogger.logProcess(current_running,getClk());
}
     else { printf("child is waiting /resumed %d id %d\n",p,current_running.id);
           }
       break;
      

    }
}


void SigIntHandler(int sig)
{

  //int msg=Recmsg(pD);
 // printf("received a msg form pg %d\n",msg);
  signal(SIGILL, SIG_IGN);
  

}




int main(int argc, char* argv[]) {
    initQueue(false);
    initClk();
    printf("my PID %d\n",getpid());
   

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = my_sigchld_handler;

    sigaction(SIGCHLD, &sa, NULL);
    signal(SIGILL,SigIntHandler);

    quant=atoi(argv[1]);
    
   int prevclock;
    
   printf("run ronbin\n");
  
  int y=Recmsg(pD);
  while(y<=0)
  {  
        int x= getClk();
        
       
       y=Recmsg(pD);
       while(y==0)
	{  x= getClk();
	  printf("current received data %d priority %d\n",pD.id,pD.priority);
       
	
	  pq.push(pD);printf("waiting in queue\n");
	  y=Recmsg(pD);
		
      }
    
    

    if(!running_process&&!pq.empty())
      {   printf("clock %d\n",getClk());
        printf("state %d\n",running_process);
          current_running=pq.front();
          pq.pop(); 
          running_process=true;
            if(current_running.state==2)
             {  kill(current_running.pid,SIGCONT);
               current_running.state=0;
                current_running.laststopTime=x;	
		printf("resuming child %d \n", current_running.id);
               
   		
             
             }
         else  
             { 

                   current_running.state=0;
                    int id=fork();
                    if(id==0)
                        {   stringstream strs;
			    strs << current_running.runTime;
			   string temp_str = strs.str();
	        	   char const *pchar = temp_str.c_str();
                           execl("./process.out","./process.out",pchar);
				     perror("execl() failure!\n\n");
                        }
			else 
				{  
				  
                                     current_running.pid=id;
				     current_running.state=0;
                                     current_running.startOfExecution=x;                                  
                                     printf("running child process with id  %d\n",current_running.id);
				}
		}
      }
  //printf("here\n");
 if(!running_process)sleepplease(1);
  if(running_process)
  {    int prevclk=getClk();
           printf("quant %d\n",quant); 
           printf("clock in lseep %d\n",getClk());
	   sleepplease(quant);
   int y=getClk();
  //printf("remain time %d\n",quant+prevclk);
  //printf("clock %d\n",y);
          if(running_process&&y==quant+prevclk)
            { 
	        {  kill(current_running.pid,SIGSTOP);
                  running_process=false; 
                  current_running.state=2;                     
		   current_running.stopTime=x;
		   pq.push(current_running);
		   printf("stop child %d \n", current_running.id);	
		   
	        
                 }	
}else if(running_process){sleepplease(quant+prevclk-y);kill(current_running.pid,SIGSTOP);
                  running_process=false; 
                  current_running.state=2;                     
		   current_running.stopTime=x;
		   pq.push(current_running);
		   printf("stop child %d \n", current_running.id);} 
        
		                                
    printf("clock %d\n",y);
        printf("state %d\n",running_process);
    
	  
            
    }
 
  
  //printf("clock %d\n",getClk());
   }
   

  while(running_process||!pq.empty())
    {  int x= getClk();
       // printf("im in loop with clck%d\n",x);  
        
       
	if(!running_process)
            {     current_running=pq.front();
                  pq.pop();     
                 running_process=true;
                 if(current_running.state==2)
                    {   kill(current_running.pid,SIGCONT);
                        current_running.state=0;
                       current_running.laststopTime=x;	
               
                        
                        printf("resuming child %d \n", current_running.id);
                       // sleep(1); 
		
                    }
                  else{   int id=fork();
                             if(id==0)
                             {  stringstream strs;
			       strs << current_running.runTime;
			       string temp_str = strs.str();
	        	       char const *pchar = temp_str.c_str();
                               execl("./process.out","./process.out",pchar);
				     perror("execl() failure!\n\n");
		             }
			    else  
				{  
				  current_running.pid=id;
				    current_running.state=0;
                                    current_running.startOfExecution=x;
                                  
                                    printf("running child process with id  %d\n",current_running.id);
				}
                    }
		
            }
      
   if(running_process)
  {    int prevclk=getClk();
           printf("quant %d\n",quant); 
           printf("clock in lseep %d\n",getClk());
	   sleepplease(quant);
   int y=getClk();
          if(y==quant+prevclk)
            {   if(running_process)
	        {  kill(current_running.pid,SIGSTOP);
                  running_process=false; 
                  current_running.state=2;                     
		   current_running.stopTime=x;
		   pq.push(current_running);
		   printf("stop child %d \n", current_running.id);	
		   
	        
                 }	
}else if(running_process){sleepplease(quant+prevclk-y);if(running_process){kill(current_running.pid,SIGSTOP);
                  running_process=false; 
                  current_running.state=2;                     
		   current_running.stopTime=x;
		   pq.push(current_running);
		   printf("stop child %d \n", current_running.id);}	} 
        
		                                
    printf("clock %d\n",y);
        printf("state %d\n",running_process);
    
	  
            
    } 
   
    } 


 int x= getClk();
 printf("at end clck%d\n",x); 

  

    return 0;
    
}
