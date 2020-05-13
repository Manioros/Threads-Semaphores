/*	Ioannis Manioros
	A.M.: 3347
	username: csd3347
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#define MAXCUSTOMERS 100000

int n,customers;
int chairs;/*katilimenes*/
int customersleft = 0;
double sum_time = 0;

sem_t semcust, semgroc;
pthread_mutex_t mymutex, cmutex;


void *grocer_function(void *idp){
	char * pname;

	pname = (char *) idp;
	while(1) {
		/*printf("gtrocer %s\n", pname);*/
		sem_wait(&semcust);
		
		pthread_mutex_lock(&mymutex);
		usleep((rand() % 400)*1000);
		pthread_mutex_unlock(&mymutex);
		
		sem_post(&semgroc);
	}
}

void *customer_function(void *idp){
	clock_t start, end;
	double cust_time;
	/*printf("customer chairs:%d\n", chairs);*/
	if(chairs<n) {

		start = clock();
		pthread_mutex_lock(&cmutex);
		chairs++;
		pthread_mutex_unlock(&cmutex);
		sem_wait(&semgroc);
		pthread_mutex_lock(&cmutex);
		chairs--;
		pthread_mutex_unlock(&cmutex);
		end = clock();

		cust_time = ((double) (end - start)) / CLOCKS_PER_SEC;
		sum_time += cust_time;

		sem_post(&semcust);
		/*printf("finished\n");*/
	}
	else{
		/*printf("leaving\n");*/
		pthread_mutex_lock(&cmutex);
		customersleft++;
		pthread_mutex_unlock(&cmutex);
	}
}

int main(int argc, char *argv[]){
	int i;
	pthread_t g0, g1;
	pthread_t c[MAXCUSTOMERS];
	/*char cname[MAXCUSTOMERS][10];*/

	time_t t;
	srand((unsigned) time(&t));

	if(argc != 3 ){
		printf("error number of parameters.\n");
		exit(EXIT_FAILURE);
	}

	n = atoi(argv[1]);
	customers = atoi(argv[2]);

	chairs=0;

	pthread_mutex_init(&mymutex,NULL);
	pthread_mutex_init(&cmutex,NULL);
	sem_init(&semcust, 0, 0 );
	sem_init(&semgroc, 0, 2 );
	pthread_create(&g0, NULL, grocer_function, "g0");
	pthread_create(&g1, NULL, grocer_function, "g1");
	for(i=0;i<customers;i++){
		usleep(100*1000);
		pthread_create(&c[i], NULL, customer_function, NULL);
	}
	printf("average waiting time: %f\n",sum_time/(customers-customersleft));
	printf("customers left: %d\n", customersleft);
	return 0;
}