#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define NUMBER_OF_THREADS 11 /* Hint */
#define PUZZLE_SIZE 9

/* example puzzle */
int puzzle[PUZZLE_SIZE+1][PUZZLE_SIZE+1] = {
			{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
			{-1,0,0,0,0,0,0,0,0,0},
			{-1,0,0,0,0,0,0,0,0,0},
			{-1,0,0,0,0,0,0,0,0,0},
			{-1,0,0,0,0,0,0,0,0,0},
			{-1,0,0,0,0,0,0,0,0,0},
			{-1,0,0,0,0,0,0,0,0,0},
			{-1,0,0,0,0,0,0,0,0,0},
			{-1,0,0,0,0,0,0,0,0,0},
			{-1,0,0,0,0,0,0,0,0,0}
		};

/* data structure for passing data to threads */
typedef struct
{
	int thread_number;
	int x;
	int y;
} parameters;

//sum
int sum = 0;

/* print puzzle */ 
#define LINE "====================== \n"
#define COLLINE "\n___________________________________ \n"
#define NAME "||  SUM   CHECKER  || \n"
void print_grid(int grid[10][10])
{
    int i,j;
    printf(LINE);
    printf(NAME);
    printf(LINE);

	for (i = 1; i < 10; i++)
	{
		for (j = 1; j < 10; j++)
		{
	        printf("|%1d |",grid[i][j]);
		}
        printf(COLLINE);
	}
    printf("\n");
}
// read file to check sudoku
void SetPuzzle(char filename[]){
    FILE *file = fopen(filename,"r");
    int i,j,ch,num;
    for (i=0;i<=9;i++){
        for(j=0;j<=9;j++){
            while(((ch = getc(file)) != EOF)){
                if(ch == EOF) break;
                if(ch == ',') break;
                if(ch == '\n') break;
                ungetc(ch,file);
                fscanf(file,"%d",&num);
                if(num!=-1)
                    puzzle[i][j] = num;            
            } 
        }
    }
    print_grid(puzzle);
    return;
}

void *row_child(){
  
  for (int i = 1; i <= 9; i ++)
  {    
      int check = 0;
      for (int j = 1; j <= 9; j ++)
      {     
          if (puzzle[i][j] > 9 || puzzle[i][j] < 1)
              pthread_exit((void *)0);
          check += puzzle[i][j];
      }
      if (check != sum)
          pthread_exit((void *)0);
  }
  
  pthread_exit((void *)1);

}

void *col_child(){
 
  for (int j = 1; j <= 9; j ++)
  {    
      int check = 0;
      for (int i = 1; i <= 9; i ++)
      {
          if (puzzle[i][j] > 9 || puzzle[i][j] < 1)
              pthread_exit((void *)0);
          check += puzzle[i][j];
      }
      if (check != sum)
          pthread_exit((void *)0);
  }

  pthread_exit((void *)1);
 
}

void *sub_child(void *sub_data){
  
  parameters *dt = (parameters*) sub_data;
  
  int check = 0;
  for (int i = dt->x; i < (dt->x) + 3; i ++)
  {
      for (int j = dt->y; j < (dt->y) + 3; j ++)
      {
          if (puzzle[i][j] > 9 || puzzle[i][j] < 1)
              pthread_exit((void *)0);
          check += puzzle[i][j];
      }
  }

  if (check != sum)  pthread_exit((void *)0);
  else  pthread_exit((void*)1);
  
}

int main(int argc, char* argv[])
{

	int rv = 1; // flag to check answer
	// input the sudoku file
	SetPuzzle("test.txt");
	//Your code here
  
  void *ret;
	pthread_t t[NUMBER_OF_THREADS];
  parameters data;
  
  //SUM
  for (int k = 1; k <= 9; k ++)
      sum += puzzle[1][k];
  
  
  for (int k = 0; k < NUMBER_OF_THREADS; k ++)
  {   
      //ROW  
      if(k == 0)    pthread_create(&t[k], NULL, row_child, NULL);   
  
      //COL
      else if(k == 1)    pthread_create(&t[k], NULL, col_child, NULL);   
      
      //SUB GRID
      else 
      {
        	for (int i = 1; i < 10; i += 3)
        	{
            		for (int j = 1; j < 10; j += 3)
            		{
                    data.x = i;
                    data.y = j;
   
              			pthread_create(&t[k], NULL, sub_child, (void *) &data);   
                    k ++;
            		}
        	}
      }
  }
  
  //JOIN
  for (int k = 0; k < NUMBER_OF_THREADS; k ++)
  {
      pthread_join(t[k], &ret);
      rv = rv && ret;
  }
  
	if (rv == 1)
		printf("Successful :) \n");
	else
		printf("Must check again! :( \n");

	return 0;
}
