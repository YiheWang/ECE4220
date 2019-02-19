/*
 ============================================================================
 Name        : Lab2_1.c
 Author      : YIhe Wang
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#define MAX_LINE 1024

int **matrix;
int static rows;
int static columns;

//int static case2Count = 0;//count the times of search number
int static case2ThreadCount = 0;//count the number of thread created


//int static case3Count = 0;
int static case3ThreadCount = 0;

int static case4ThreadCount = 0;

void ThreadEntireMatrix(void *ptr)
{
	int *a;
    a = (int *)ptr;
    int i,j;
    int *count = (int *)malloc(sizeof(int));
    count[0] = 0;
    //printf("Rows: %d, Columns: %d",rows,columns);
    for(i=0;i<rows;++i){
    	for(j=0;j<columns;++j){
    		if(a[0] == matrix[i][j]){
    			++count[0];
    		}
    	}
    }
    //printf("Number %d appeared %d times in the whole matrix\n",a[0],count);

    pthread_exit((void *)count);
}

void ThreadEachRow(void *ptr)
{
	++case2ThreadCount;
	int *a;
	a = (int *)ptr;
	int i;
	int *count = (int *)malloc(sizeof(int));
	count[0] = 0;
	for(i=0;i<columns;++i){
		if(matrix[case2ThreadCount][i] == a[0]){
			++count[0];//count the number appeared times
		}//one thread just search one row
	}

	//printf("%d\n",count[0]);
	pthread_exit((void *)count);
}

void ThreadEachColumn(void *ptr)
{
	++case3ThreadCount;
	int *a;
	a = (int *)ptr;
	int i;
	int *count = (int *)malloc(sizeof(int));
	count[0] = 0;
	for(i=0;i<rows;++i){
		if(matrix[i][case3ThreadCount] == a[0]){
			++count[0];//count the number appeared times
		}//one thread just search one column
	}


	pthread_exit((void *)count);
}

void ThreadEachElement(void *ptr)
{
	++case4ThreadCount;
	int i,j;
	i = case4ThreadCount/columns;// get the row number
	j = case4ThreadCount%columns - 1;// get the column number
	int *a;
	a = (int *)ptr;
	int *count = (int *)malloc(sizeof(int));
	count[0] = 0;
	if(matrix[i][j] == a[0]){
		++count[0];//count the number appeared times
	}//one thread just compare one number

	pthread_exit((void *)count);
}

int main() {
                int line1[2];
                FILE *file;
                char fileName[50];
                strcpy(fileName,"2x100.txt");
                clock_t start, finish;
                double totalTime;


                file = fopen(fileName,"r");
                if(file == NULL){
                        printf("File open failed!\n");
                        return 0;
                }//open the file and error check

                {
                int i;
                for(i=0;i<2;++i){
                        fscanf(file,"%d",line1+i);
                }
                }//read the first line

                rows = line1[0];
                columns = line1[1];
                //get the rows and columns by reading the first line

                printf("rows: %d, columns: %d\n",rows,columns);

                matrix = (int**) malloc(sizeof(int *)*rows);
                if(matrix == NULL){
                        printf("Malloc failed!\n");
                        return 0;
                }

                {
                int i;
                for(i=0;i<rows;++i){
                        matrix[i] = (int*) malloc(sizeof(int)*columns);
                        if(matrix[i] == NULL){
                                printf("Malloc failed!\n");
                                return 0;
                        }
                }//malloc space for the global pointer according to the rows and columns
                }

                int i,j;
                for(i=0;i<rows;++i){
                        for(j=0;j<columns;++j){
                                fscanf(file,"%d",&matrix[i][j]);
                        }//load the numbers in the matrix
                }

                {
                int i,j;
                for(i=0;i<rows;++i){
                         for(j=0;j<columns;++j){
                                  printf("%d ",matrix[i][j]);
                         }//load the numbers in the matrix
                         printf("\n");
                }
                }

                /*
                if(file != NULL){
                	fclose(file);  ??? There is always an error here so I commented out
                }*/

                int a;
                printf("PLease enter a specific number you want to search:");
                scanf("%d",&a);

                //case 1, search the entire matrix
                printf("Case 1 start:\n");
                pthread_t threadEntireMatrix;

                int *count;
                start = clock();//time start
                pthread_create(&threadEntireMatrix,NULL,(void*)&ThreadEntireMatrix,(void *)&a);
                pthread_join(threadEntireMatrix,(void **)&count);
                finish = clock();//time finish
                totalTime = (double)(finish - start)/CLOCKS_PER_SEC;//count the total execution time
                printf("The Number %d appeared %d times in the matrix\n",a,count[0]);
                printf("The execution time of case 1 is %f ms\n\n",totalTime*1000);



                {
                	//case 2, search each row of the matrix
                	printf("Case 2 start:\n");
                	pthread_t *pthreads;
                	pthreads = (pthread_t *)malloc(sizeof(pthread_t)*rows);
                	//create a pointer to point to a list of threads

                	int *countEachRow;
                	int case2Count = 0;
                	start = clock();//time start
                	int i;
                	for(i=0;i<rows;++i){
                		pthread_create((pthreads+i),NULL,(void *)&ThreadEachRow,(void *)&a);
                	}//create multiple threads

                	for(i=0;i<rows;++i){
                		pthread_join(*(pthreads+i),(void **)&countEachRow);
                		case2Count = case2Count + countEachRow[0];
                	}
                	finish = clock();//time finish
                	totalTime = (double)(finish - start)/CLOCKS_PER_SEC;//count the total execution time
                	printf("Number %d appeared %d times in the whole matrix\n",a,case2Count);
                	printf("The execution time of case 2 is %f ms\n\n",totalTime*1000);
                	free(pthreads);
                }


                {
                	//case 3, search each row of the matrix
                	printf("Case 3 start:\n");
                	pthread_t *pthreads;
                	pthreads = (pthread_t *)malloc(sizeof(pthread_t)*columns);
                	//create a pointer to point to a list of threads

                	int *countEachColumn;
                	int case3Count = 0;
                	start = clock();//time start
                	int i;
                	for(i=0;i<columns;++i){
                		pthread_create((pthreads+i),NULL,(void *)&ThreadEachColumn,(void *)&a);
                	}//create multiple threads

                	for(i=0;i<columns;++i){
                	    pthread_join(*(pthreads+i),(void **)&countEachColumn);
                	    case3Count = case3Count + countEachColumn[0];
                	}
                	finish = clock();//time finish
                	totalTime = (double)(finish - start)/CLOCKS_PER_SEC;//count the total execution time
                	printf("Number %d appeared %d times in the whole matrix\n",a,case3Count);
                	printf("The execution time of case 3 is %f ms\n\n",totalTime*1000);
                	free(pthreads);
                }


                {
                    //case 4, search each row of the matrix
                    printf("Case 4 start:\n");
                    int threadNumber = rows*columns;
                    //printf("%d\n",threadNumber);
                    pthread_t *pthreads;
                    pthreads = (pthread_t *)malloc(sizeof(pthread_t)*threadNumber);

                    int *countEachElement;
                    int case4Count = 0;
                    start = clock();//time start
                    int i,j;

                    for(i=0;i<threadNumber;++i){
                        pthread_create((pthreads+i),NULL,(void *)&ThreadEachElement,(void *)&a);
                    }//create multiple threads

                    for(j=0;j<threadNumber;++j){
                    	pthread_join(*(pthreads+j),(void **)&countEachElement);
                    	case4Count = case4Count + countEachElement[0];
                    }

                    finish = clock();//time finish
                    totalTime = (double)(finish - start)/CLOCKS_PER_SEC;//count the total execution time
                    printf("Number %d appeared %d times in the whole matrix\n",a,case4Count);
                    printf("The execution time of case 4 is %f ms\n\n",totalTime*1000);
                    free(pthreads);
                 }



                return 0;
}
