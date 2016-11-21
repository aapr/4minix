//#include <semaphore.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <minix_sem.h>
#include <errno.h>
#include <glo.h>
#include <signal.h>

#define SEM_INIT 1
#define SEM_MAX 30
#define LIMIT 20

typedef struct {
	int id; //identificador del semaforo
	int estado; // 1 si esta activo, 0 si no lo esta	
	int proc_list[LIMIT];// una lista de los procesos que lo an llamado 
}mysem;

static mysem sem_array[SEM_MAX];

void init_sem(void){
	int i;
	for ( i=0; i< SEM_MAX; i++){
		sem_array[i].id = 0;
		sem_array[i].estado = 0;
	}
}

//  Crea un sem´aforo con identificador id. Retorna
// 0 si lo pudo crear, -1 si ya existe. El valor inicial de un sem´aforo
// ser´a siempre 1.
int sem_create(int id){
	int j; // se asegura de que el id no exista
	for ( j=0; j< SEM_MAX; ;++){
		if (sem_array[j].id == id)
			return -1;
	}

	int i; // se asegura de que aun queda espacio
	for ( i=0; i< SEM_MAX; i++){
		if (sem_array[i].id == 0)
			break;
	}

	if ( i == SEM_MAX){
		errno = ENOSPC;
	}

	if ( errno == 0){
		sem_array[i].id = id;
		sem_array[i].estado = SEM_INIT;
		return 0;
	}
	return -1;
}


// Destruye el sem´aforo con identificador id.
// Retorna 0 si lo pudo eliminar, y -1 si este no existe.
int sem_terminate(int id){
	int i; // "destruye" 
	for ( i=0; i<= SEM_MAX; i++){
		if (sem_array[i].id == id){
			sem_array[i].id = 0;
			memset(sem_array[i].proc_list, 0, sizeof(sem_array[i].proc_list));
			return 0;
		}
	}
	return -1;
}


// Si el valor del sem´aforo id es 1, lo decrementa
// y el proceso que hizo la llamada se adue˜na del sem´aforo. En cambio, si
// el valor es 0, entonces el proceso que hizo la llamada debe ser bloquiado
// hasta que este se libere. Retorna 0 si no hubo problema, o -1 en caso de
// intentar acceder a un sem´aforo inv´alido.
int sem_down(int id){
	int i; // 
	int j; 
	for ( i=0; i< SEM_MAX; i++){
		if (sem_array[i].id == id){
			if (sem_array[i].estado == 1){
				sem_array[i].estado = 0;
				return 0;
			}
			for ( j=0; j < LIMIT; j++){
				if ( sem_array[i].proc_list[j] == 0){
					sem_array[i].proc_list[j] = &mp;	// mp es una variable global de glo.h, ptr al slot del proceso
					kill(&mp, SIGSTOP);
					return 0;// this dosnt work as intended
				}
			}
			
		}
	}
	return -1;
}

// Incrementa el valor del sem´aforo identificado por
// el par´ametro. El (los) procesos que estaban bloqueados esperando este
// sem´aforo pueden competir para obtenerlo. Retorna 0 si todo ocurrio sin
// problemas. Retorna -1 si se intenta acceder a un sem´aforo inv´alido, o a
// un sem´aforo ocupado por otro proceso.
int sem_up(int id){
	int i; 
	int j;//  
	for ( i=0; i< SEM_MAX; i++){
		if (sem_array[i].id == id){
			if (sem_array[i].estado == 0){
				sem_array[i].estado = 1;
				for ( j=0; j< LIMIT; j++){
					if (sem_array[i].proc_list[j] == &mp ){
						sem_array[i].proc_list[j] = 0
						break;
					}
					
				}
				for (j=0;j<LIMIT;j++){
					if(sem_array[i].proc_list[j] != 0){
						kill(sem_array[i].proc_list[j], SIGCONT);
						break;
					}
				}
				return 0;
			}
			
		}
	}
	return -1;
}
