#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

void main()
{
        key_t key = 1234;
	int *value;       
	int i,j,k;
    	int matA[4][3];
    	int matB[3][6];
    	int res[4][6];
    	int order=0;

        int shmid = shmget(key, sizeof(int)*4*6, IPC_CREAT | 0666);
        int *value;
        value = shmat(shmid, NULL, 0);

    
    	printf("\nInput elements of matrix 4x3:\n");
    	for(i=0;i<4;i++){
        	for(j=0;j<3;j++){
            		scanf("%d",&matA[i][j]);
        	}
    	} 
    
    	printf("\nInput the elements of matrix 3x6:\n");
    	for(i=0;i<3;i++){
        	for(j=0;j<6;j++){
            		scanf("%d",&matB[i][j]);
        	}
    	}
    
    	for(i=0;i<4;i++){
        	for(j=0;j<6;j++){
            		res[i][j]=0;
            		for(k=0;k<3;k++){
                		res[i][j] = res[i][j] + matA[i][k]*matB[k][j];  
            		}
        	}
    	}
    	printf("\nResult:\n");
    	for(i=0;i<4;i++){
       		for(j=0;j<6;j++){
            		printf("%d \t",res[i][j]);
            		value[order]=res[i][j];
            		order++;
        	}
        	printf("\n");
    	}
    	
    	sleep(20);

        shmdt(value);
        shmctl(shmid, IPC_RMID, NULL);
}
