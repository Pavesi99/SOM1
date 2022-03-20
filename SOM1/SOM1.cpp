
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "SOM1.h"
#include <thread>
#include <chrono>
#include <windows.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include	<pthread.h>

using namespace std;
using namespace std::chrono;

#pragma region  Processos - execução e tempo de operação


double time1, timedif;   /* use doubles to show small values */
void exercicio1(void)
{
	time1 = (double)clock();            /* get initial time */
	time1 = time1 / CLOCKS_PER_SEC;      /*    in seconds    */

	/* call clock a second time */
	timedif = (((double)clock()) / CLOCKS_PER_SEC) - time1;
	printf("The elapsed time is %f seconds\n", timedif);
}

#define NSEC_PER_SEC    (1000000000) 	// Numero de nanosegundos em um milissegundo
void exercicio2()
{
	struct timespec t;
	int periodo = 500000000; 	// 500ms

	// Le a hora atual, coloca em t
	clock_gettime(CLOCK_MONOTONIC, &t);

	// Tarefa periodica iniciarÃ¡ em 1 segundo
	t.tv_sec++;

	while (1) {
		// Espera ateh inicio do proximo periodo
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);

		// Realiza seu trabalho
		printf("Passou um periodo !\n");

		// Calcula inicio do proximo periodo
		t.tv_nsec += periodo;
		while (t.tv_nsec >= NSEC_PER_SEC) {
			t.tv_nsec -= NSEC_PER_SEC;
			t.tv_sec++;
		}
	}
}

#define	N_AMOSTRAS	100		// Numero de amostras (medicoes) coletadas

long atraso_inicio[N_AMOSTRAS];		// M

void exercicio3()
{
	struct timespec t, t_inicio;
	int amostra = 0;		// Amostra corrente
	int periodo = 200000000; 	// 200ms

	// Le a hora atual, coloca em t
	clock_gettime(CLOCK_MONOTONIC, &t);

	// Tarefa periodica iniciarÃ¡ em 1 segundo
	t.tv_sec++;

	while (amostra < N_AMOSTRAS) {
		// Espera ateh inicio do proximo periodo
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);

		// Le a hora atual, coloca em t_inicio
		clock_gettime(CLOCK_MONOTONIC, &t_inicio);

		// Calcula atraso observado em microsegundos
		atraso_inicio[amostra++] = 1000000 * (t_inicio.tv_sec - t.tv_sec) + (t_inicio.tv_nsec - t.tv_nsec) / 1000;

		// Realiza seu trabalho
		printf("Coletada a amostra %d\n", amostra);

		// Calcula inicio do proximo periodo
		t.tv_nsec += periodo;
		while (t.tv_nsec >= NSEC_PER_SEC) {
			t.tv_nsec -= NSEC_PER_SEC;
			t.tv_sec++;
		}
	}

	printf("Atrasos ateh o inicio da execucao (overhead,release jitter, interferencias\n");
	for (int i = 0; i < N_AMOSTRAS; ++i)
		printf("%3ld us\n", atraso_inicio[i]);

}

#define	N_AMOSTRAS	100		// Numero de amostras (medicoes) coletadas

long atraso_inicio[N_AMOSTRAS];		// Medicoes do atraso ateh inicio em microsegundos
long atraso_fim[N_AMOSTRAS];		// Medicoes do atraso ateh o fim em microsegundos

void exercicio4()
{
	struct timespec t, t_inicio, t_fim;
	int amostra = 0;		// Amostra corrente
	int periodo = 200000000; 	// 200ms

	// Le a hora atual, coloca em t
	clock_gettime(CLOCK_MONOTONIC, &t);

	// Tarefa periodica iniciarÃ¡ em 1 segundo
	t.tv_sec++;

	while (amostra < N_AMOSTRAS) {
		// Espera ateh inicio do proximo periodo
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);

		// Le a hora atual, coloca em t_inicio
		clock_gettime(CLOCK_MONOTONIC, &t_inicio);

		// Calcula atraso observado em microsegundos
		atraso_inicio[amostra] = 1000000 * (t_inicio.tv_sec - t.tv_sec) + (t_inicio.tv_nsec - t.tv_nsec) / 1000;

		// Realiza seu trabalho
		printf("Coletada a amostra %d\n", amostra);

		// Le a hora atual, coloca em t_fim
		clock_gettime(CLOCK_MONOTONIC, &t_fim);

		// Calcula o tempo de resposta observado em microsegundos
		atraso_fim[amostra++] = 1000000 * (t_fim.tv_sec - t.tv_sec) + (t_fim.tv_nsec - t.tv_nsec) / 1000;

		// Calcula inicio do proximo periodo
		t.tv_nsec += periodo;
		while (t.tv_nsec >= NSEC_PER_SEC) {
			t.tv_nsec -= NSEC_PER_SEC;
			t.tv_sec++;
		}
	}

	printf("Atrasos ateh o inicio da execucao incluem overhead,release jitter, interferencias\n");
	printf("Tempo de resposta vai desde a chegada ateh a conclusao\n\n");
	printf("Atraso inicio           Tempo de resposta\n");

	for (int i = 0; i < N_AMOSTRAS; ++i)
		printf("Atraso inicio=%4ldus     Tempo de resposta=%4ldus\n", atraso_inicio[i], atraso_fim[i]);

}

#pragma endregion

#pragma region Processos
int value = 5;
int exercicio5()
{
	pid_t pid;

	pid = fork();

	if (pid == 0) { /* child process */
		value += 15;
		return 0;
	}
	else if (pid > 0) { /* parent process */
		wait(NULL);
		printf("PARENT: value = %d\n", value); /* LINE A */
		return 0;
	}
}

int exercicio6()
{
	fork();

	fork();

	fork();

	return 0;
}

int exercicio7()
{
	pid_t pid, pid1;

	/* fork a child process */
	pid = fork();
	if (pid < 0) {
		/* error occurred */
		fprintf(stderr, "Fork Failed");
		return 1;
	}
	else if (pid == 0) { /* child process */
		pid1 = getpid();
		printf("child: pid = %d\n", pid); /* A */
		printf("child: pid1 = %d\n", pid1); /* B */
	}
	else { /* parent process */
		pid1 = getpid();
		printf("parent: pid = %d\n", pid); /* C */
		printf("parent: pid1 = %d\n", pid1); /* D */
		wait(NULL);
	}

	return 0;
}

int exercicio8()
{
	pid_t pid, pid1;

	/* fork a child process */
	pid = fork();
	if (pid < 0) {
		/* error occurred */
		fprintf(stderr, "Fork Failed");
		return 1;
	}
	else if (pid == 0) { /* child process */
		pid1 = getpid();
		printf("child: pid = %d\n", pid); /* A */
		printf("child: pid1 = %d\n", pid1); /* B */
	}
	else { /* parent process */
		pid1 = getpid();
		printf("parent: pid = %d\n", pid); /* C */
		printf("parent: pid1 = %d\n", pid1); /* D */
		wait(NULL);
	}

	return 0;
}

int exercicio9()
{
	const int SIZE = 4096;
	const char* name = "OS";
	const char* message0 = "Studying ";
	const char* message1 = "Operating Systems ";
	const char* message2 = "Is Fun!";

	int shm_fd;
	void* ptr;

	/* create the shared memory segment */
	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);

	/* configure the size of the shared memory segment */
	ftruncate(shm_fd, SIZE);

	/* now map the shared memory segment in the address space of the process */
	ptr = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (ptr == MAP_FAILED) {
		printf("Map failed\n");
		return -1;
	}

	/**
	 * Now write to the shared memory region.
	 *
	 * Note we must increment the value of ptr after each write.
	 */
	sprintf(ptr, "%s", message0);
	ptr += strlen(message0);
	sprintf(ptr, "%s", message1);
	ptr += strlen(message1);
	sprintf(ptr, "%s", message2);
	ptr += strlen(message2);

	return 0;
}


int exercicio10()
{
	const char* name = "OS";
	const int SIZE = 4096;

	int shm_fd;
	void* ptr;
	int i;

	/* open the shared memory segment */
	shm_fd = shm_open(name, O_RDONLY, 0666);
	if (shm_fd == -1) {
		printf("shared memory failed\n");
		exit(-1);
	}

	/* now map the shared memory segment in the address space of the process */
	ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
	if (ptr == MAP_FAILED) {
		printf("Map failed\n");
		exit(-1);
	}

	/* now read from the shared memory region */
	printf("%s", (char*)ptr);

	/* remove the shared memory segment */
	if (shm_unlink(name) == -1) {
		printf("Error removing %s\n", name);
		exit(-1);
	}

	return 0;
}

int exercicio10() {
	pid_t pid;

	/* fork a child process */
	pid = fork();

	if (pid < 0) { /* error occurred */
		fprintf(stderr, "Fork Failed\n");
		return 1;
	}
	else if (pid == 0) { /* child process */
		printf("I am the child %d\n", pid);
		execlp("/bin/ls", "ls", NULL);
	}
	else { /* parent process */
		/* parent will wait for the child to complete */
		printf("I am the parent %d\n", pid);
		wait(NULL);

		printf("Child Complete\n");
	}

	return 0;
}


#pragma endregion


#pragma region  Threads

/*************************************************************************
*	Monitor responsavel pelo acesso ao valor total
*
*	Uma unica funcao, usada para somar 'delta' no valor total
***/

int valor_total = 0;
pthread_mutex_t em = PTHREAD_MUTEX_INITIALIZER;

int alteraValor(int delta) {
	int novo_valor;

	pthread_mutex_lock(&em);
	valor_total += delta;
	novo_valor = valor_total;
	pthread_mutex_unlock(&em);
	return novo_valor;
}

void defineValor(int novo) {
	pthread_mutex_lock(&em);
	valor_total = novo;
	pthread_mutex_unlock(&em);
}
/*************************************************************************/

/*************************************************************************
*	Monitor responsavel pelo acesso a tela
*
***/

pthread_mutex_t tela = PTHREAD_MUTEX_INITIALIZER;

void aloca_tela(void) {
	pthread_mutex_lock(&tela);
}

void libera_tela(void) {
	pthread_mutex_unlock(&tela);
}

/*************************************************************************/

/***
*	Thread que mostra status na tela
***/
void thread_mostra_status(void) {
	int ultimo;
	while (1) {
		sleep(1);
		aloca_tela();
		ultimo = alteraValor(+1);
		printf("Ultimo lido foi %d, digite <enter> para alterar\n", ultimo);
		libera_tela();
	}
}

/***
*	Thread que le teclado
***/
void thread_le_teclado(void) {
	int novo;
	char teclado[1000];
	while (1) {
		fgets(teclado, 1000, stdin);
		aloca_tela();
		printf("Digite novo valor:\n");
		fgets(teclado, 1000, stdin);
		defineValor(atoi(teclado));
		libera_tela();
	}
}

int exercicio11()
{
	pthread_t t1, t2;

	pthread_create(&t1, NULL, (void*)thread_mostra_status, NULL);
	pthread_create(&t2, NULL, (void*)thread_le_teclado, NULL);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
}

/*************************************************************************
*	Monitor responsavel pelo acesso ao valor total
*
*	Uma unica funcao, usada para somar 'delta' no valor total
***/

int valor_total = 0;
pthread_mutex_t em = PTHREAD_MUTEX_INITIALIZER;

/*************************************************************************
*	Monitor responsavel pelo acesso ao valor total
*
*	Uma unica funcao, usada para somar 'delta' no valor total
***/

int valor_total = 0;
pthread_mutex_t em = PTHREAD_MUTEX_INITIALIZER;

int alteraValor(int delta) {
	int novo_valor;

	pthread_mutex_lock(&em);
	valor_total += delta;
	novo_valor = valor_total;
	printf("Somando %d ficarÃ¡ %d\n", delta, novo_valor);
	pthread_mutex_unlock(&em);
	return novo_valor;
}
/*************************************************************************/

/***
*	Codigo de thread que incrementa
***/
void thread_incrementa(void) {
	while (1) {
		sleep(1);
		alteraValor(+1);
	}
}

void thread_decrementa(void) {
	while (1) {
		sleep(2);
		alteraValor(-1);
	}
}

void exercicio12(int argc, char* argv[])
{

	pthread_t t1, t2, t3, t4;

	pthread_create(&t1, NULL, (void*)thread_incrementa, NULL);
	pthread_create(&t2, NULL, (void*)thread_incrementa, NULL);
	pthread_create(&t3, NULL, (void*)thread_decrementa, NULL);
	pthread_create(&t4, NULL, (void*)thread_decrementa, NULL);

	printf("Digite enter para terminar o programa:\n");
	getchar();
}

#define NUMTHREADS 2
#define VETSIZE 5000

typedef struct {
	int fromidx, length;
}thread_arg, * ptr_thread_arg;

pthread_t threads[NUMTHREADS];
thread_arg arguments[NUMTHREADS];
int nums[VETSIZE];
int sum;

void* thread_func(void* arg) {
	ptr_thread_arg argument = (ptr_thread_arg)arg;
	int i, localsum = 0, endidx;

	endidx = argument->fromidx + argument->length;

	for (i = argument->fromidx; i < endidx; i++) {
		localsum += nums[i];
	}
	sum += localsum;
}

void exercicio13(int argc, char** argv)
{
	int i, length, remainder;
	sum = 0; length = VETSIZE / NUMTHREADS;
	remainder = VETSIZE % NUMTHREADS;
	for (i = 0; i < NUMTHREADS; i++) {
		arguments[i].fromidx = i * length;
		arguments[i].length = length;
		if (i == (NUMTHREADS – 1)) arguments[i].length += remainder;
		pthread_create(&(threads[i]), NULL, thread_func, &(arguments[i]));
	}
	for (i = 0; i < NUMTHREADS; i++) pthread_join(threads[i], NULL);
	printf(“A soma dos numeros do vetor eh% d\n”, sum);
}



// Let us create a global variable to change it in threads 
int g = 0;

// The function to be executed by all threads 
void* myThreadFun(void* vargp) {
	// Store the value argument passed to this thread 
	int* myid = (int*)vargp;

	// Let us create a static variable to observe its changes 
	static int s = 0;

	// Change static and global variables 
	++s; ++g;

	// Print the argument, static and global variables 
	printf("Thread ID: %d, Static: %d, Global: %d\n", *myid, ++s, ++g);
}

int exercicio14()
{
	int i;
	pthread_t tid;

	// Let us create three threads 
	for (i = 0; i < 3; i++)
		pthread_create(&tid, NULL, myThreadFun, (void*)&tid);

	pthread_join(tid, NULL);
	pthread_exit(NULL);
	return 0;
}

int sum; /* this data is shared by the thread(s) */

/**
 * The thread will begin control in this function
 */
void* runner(void* param)
{
	int i, upper = atoi(param);
	sum = 0;

	if (upper > 0) {
		for (i = 1; i <= upper; i++)
			sum += i;
	}

	pthread_exit(0);
}

int exercicio15(int argc, char* argv[])
{
	pthread_t tid; /* the thread identifier */
	pthread_attr_t attr; /* set of attributes for the thread */

	if (argc != 2) {
		fprintf(stderr, "usage: a.out <integer value>\n");
		/*exit(1);*/
		return -1;
	}

	if (atoi(argv[1]) < 0) {
		fprintf(stderr, "Argument %d must be non-negative\n", atoi(argv[1]));
		/*exit(1);*/
		return -1;
	}

	/* get the default attributes */
	pthread_attr_init(&attr);

	/* create the thread */
	pthread_create(&tid, &attr, runner, argv[1]);

	/* now wait for the thread to exit */
	pthread_join(tid, NULL);

	printf("sum = %d\n", sum);
}

/**
 * The thread will begin control in this function
 */
void* runner(void* param)
{
	int i, upper = atoi(param);
	sum = 0;

	if (upper > 0) {
		for (i = 1; i <= upper; i++)
			sum += i;
	}

	pthread_exit(0);
}

#pragma endregion


int main(int argc, char* argv[])
{
	exercicio1();
	exercicio2();
	exercicio3();
	exercicio4();
	exercicio5();
	exercicio6();
	exercicio7();
	exercicio8();
	exercicio9();
	exercicio10();
	exercicio11();
	exercicio12(argc, argv);
	exercicio13(argc, argv);
	exercicio14();
	exercicio15(argc, argv);

	return 0;
}
