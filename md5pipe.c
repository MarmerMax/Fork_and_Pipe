// compile: gcc ex2.c -o ex2
// we need to send argument word to program
// for running: ./ex2 hello (hello is word to md5 function) 

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include "md5.c"
#define _OPEN_SYS_ITOA_EXT

char buf1[32]; //father buffer
char buf2[32]; //child buffer
char answer[32];    //answer array
char completed[1]; //success buffer
int result = 0; //success coded





int check_length(char * str){
     int size = strlen(str);
     for(int i = 0; i < 32; i++){ //check that all characters not equals to '-'
          if(str[i] == '-'){
               return 0;
          }
     }
     if(size == 32){
          return 1;
     }
     return 0;
}

void signal_handler(int sig){
     result = check_length(answer); // the result was success
     signal(sig, SIG_IGN); //ignore the sigint
}

// int main(int argc, char **argv){
int main(){

     char msg[21];
     int len = 0;
     printf("plain test: ");
     while(scanf("%[^\n]s", msg) != EOF){
          len = strlen(msg);
          if(len > 20){
               printf("FATAL: Input longer than 20...\n");
               exit(1);
          }
          else{
               break;
          }
     }

     int mypipe1[2]; //define pipe1 - send to child input string
     int mypipe2[2]; //define pipe2 - send to father coded input string
     int mypipe3[2]; //define pipe3 - send to father if coded was success
     int pid; //pid for process 

     if (pipe (mypipe1) || pipe(mypipe2) || pipe(mypipe3)){ //create pipes and exit if it failed 
          printf("Pipe failed.\n");
          return EXIT_FAILURE;
     }

     memset(completed, '0', 1);
     memset(answer, '-', 32);

     pid = fork(); //create process


     if(pid != 0){//father
          signal(SIGINT, SIG_IGN);

          close(mypipe1[0]);// Close reading end of first pipe 
          write(mypipe1[1], msg, len); //write to child
          close(mypipe1[1]); //close writing end of first pipe

          wait(NULL);// Wait for child to send a string 

          close(mypipe2[1]);// Close writing end of second pipe 
          read(mypipe2[0], buf1, 32);  
          close(mypipe2[0]);

          close(mypipe3[1]);
          read(mypipe3[0], completed, 1);
          close(mypipe3[0]);
          

          // Read string from child, print it and close 
          // reading end. 
          if(completed[0] == '1'){
               printf("encrypted by process %d : ", getpid());
               for(int i = 0; i < 32; i++){
                    printf("%c", buf1[i]);
               }
               printf("\n");
               kill(pid, SIGINT);
          }
          else{
               printf("Encoding failed...\n");
          }
     }
     else{ 
          signal(SIGINT, signal_handler);
          close(mypipe1[1]);// Close writing end of first pipe 
          read(mypipe1[0], buf2, len); //read from father
          close(mypipe1[0]); 
          close(mypipe2[0]); 

          md5(buf2, len, answer); //do md5 and put result to answer

          write(mypipe2[1], answer, sizeof(answer));
          close(mypipe2[1]);

          kill(getpid(), SIGINT);

          if(result){//if check was success so send to pipe3 - '1' (true)
               char * success = "1";
               close(mypipe3[0]); 
               write(mypipe3[1], success, 1);
               close(mypipe3[1]);
          }
     }
     return 0;
}