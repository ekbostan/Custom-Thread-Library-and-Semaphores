#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "queue.h"

void FCFS();
void RR();
void output();

struct process {
    char *name; //the names of the process
    int runtime; // its corresponding runtimes
    double prob; // probability
    int cputime; // total time process spent on cpu
    int cpu_disp; // number of cpu dispatches
    int wallclock; // wall clock
    int num_block; // number of times process was blocked
    int blocktime; // time for block
    int isBlock; // checks if process is blocked
    int iotime; // total time process spent on IO
    int remiotime; // remaining time on IO        
    int runtimeconst;
    int quantum; // for RR
};

struct cpustats {
    int busytime;
    int idletime;
    int dispatches;
    int num;
};

queue_t ready_q;
queue_t io_q;
queue_t stats_q;

struct cpustats* cpuInfo;
struct cpustats* ioInfo;

struct process* cpu = NULL;
struct process* io = NULL;
struct process* garbage = NULL;

int wall_clock;
int ioBlock = 0;

// user input prob and rand generated prob 
// rand generated prob < input prob then (block)  (0 < p < 1)
// otherwise passes 

int main (int argc, char *argv[]) {
    (void) srandom(12345);
    ready_q = queue_create();
    io_q = queue_create();
    stats_q = queue_create();

    cpuInfo = (struct cpustats*)malloc(sizeof(struct cpustats*));  
    ioInfo = (struct cpustats*)malloc(sizeof(struct cpustats*));  

    // check if a file has been passed.
    if (argc != 3) 
        perror("Need a file \n");

    FILE *ptr = fopen(argv[2], "r");
    int n = 0, count;
    char line[256];
    while (fgets(line, sizeof(line), ptr)) {
        n++;        
    }
    count = n; // count = valid processes
    fclose(ptr);
    // find number of elements in file
    // Create three lists. Index = order that we get processes
    char names[n][11];
    int runtimes[n];
    double probs[n];

    // Put valid processes into lists
    char* name; 
    char* prob;
    char* run;
    int runtime;
    double p;
    
    const char delim[]=" \n\r\t";
    ptr = fopen(argv[2], "r");
    for (int i = 0; i < n; i++) {
        //int count += delim;
        fgets(line, sizeof(line), ptr);
        name = strtok(line, delim);

        if(name != NULL) {
            run = strtok(NULL, delim);
            if(run != NULL) {
                runtime = atof(run);
                if(runtime == 0.0) {
                    fprintf(stderr, "Malformed line %s(%d)\n", argv[2], (i+1));
                    exit(1);
                }
                prob = strtok(NULL, delim);
                if(prob != NULL) {
                    p = atof(prob);
                    if(p == 0.0) {
                        fprintf(stderr, "Malformed line %s(%d)\n", argv[2], (i+1));
                        exit(1);
                    }
                } else {
                    fprintf(stderr, "Malformed line %s(%d)\n", argv[2], (i+1));
                    exit(1);
                }                
            } else {
                fprintf(stderr, "Malformed line %s(%d)\n", argv[2], (i+1));
                exit(1);
            }
        } else {
            fprintf(stderr, "Malformed line %s(%d)\n", argv[2], (i+1));
            exit(1);
        }
        if ((strcmp(argv[1], "-r") != 0) && (strcmp(argv[1], "-f") != 0)) { 
            fprintf(stderr, "Usage: %s [-r | -f] file\n", argv[0]);
            exit(1);
        }
            
        if (strlen(name) > 10) { 
            perror("name is too long"); 
        } else if (runtime < 1.0F) { //runtime needs to be greater than 1 
            perror("runtime is not positive integer");
        } else if (p <= 0 || p >= 1) { //probability: 0 < p < 1
            perror("probability < 0 or > 1 ");
        } else if (strlen(prob) != 4) { // strlen(prob) != 4
            perror("this prob");
        }
        for (int k = 0; k < 11; k++) { 
            if (k <= strlen(name)) {
                names[i][k] = name[k];
            } else {
                names[i][k] = '\0';
                break;
            }
        runtimes[i] = runtime;
        probs[i] = atof(prob);
        }
    }

    for (int k = 0; k < n; k++) {
        struct process* map = (struct process*)malloc(sizeof(struct process));   
        map->name = names[k];
        map->runtime = runtimes[k];
        map->runtimeconst = runtimes[k];
        map->prob = probs[k];
        map->cputime = 0;
        map->cpu_disp = 0;
        map->wallclock = 0;
        map->num_block = 0;
        map->blocktime = 0;
        map->isBlock = 0;
        map->iotime = 0;
        map->remiotime = 0;
        map->quantum = 0;

        queue_enqueue(ready_q, map);
    }

    cpuInfo->busytime = 0;
    cpuInfo->idletime = 1;
    cpuInfo->dispatches = 0;
    cpuInfo->num = queue_length(ready_q);
    
    ioInfo->busytime = 0;
    ioInfo->idletime = 0;
    ioInfo->dispatches = 0;
    ioInfo->num = queue_length(ready_q);

    printf("Processes:\n\n");
	printf("   name     CPU time  when done  cpu disp  i/o disp  i/o time\n");
    
    if (strcmp(argv[1], "-r") == 0) { 
        RR();
    } else if (strcmp(argv[1], "-f") == 0) {
        FCFS();
    }

    //need parameters for this one 
    output();
    fclose(ptr);
    return 0;
}

void FCFS() {
    while((queue_length(ready_q) > 0) || (queue_length(io_q) > 0) || (cpu != NULL) || (io != NULL)) {
        wall_clock++;
        
        if(queue_length(ready_q) > 0) {
            if(cpu == NULL) {
                struct process* ptr = ready_q->head->value;

                if(ptr->runtime < 2) {
                    ptr->isBlock = 0;
                } else {
                    double r = (double)random() / (double)RAND_MAX; 

                    if(ptr->prob > r) {
                        ptr->isBlock = 1;
                        ptr->blocktime = ((int)random())%(ptr->runtime) + 1;
                    } else {
                        ptr->isBlock = 0;
                    }
                }
                ptr->quantum = 0;
                cpu = ptr;
                (cpu->cpu_disp)++;
                queue_dequeue(ready_q, (void**)&garbage);
            }            
        }

        if(cpu != NULL) {
            if(cpu->runtime == 0) {
                printf("%-10s %6d     %6d    %6d    %6d    %6d\n", cpu->name, cpu->cputime, wall_clock, cpu->cpu_disp, cpu->num_block, cpu->iotime);
                cpuInfo->busytime += cpu->cputime;
                cpuInfo->dispatches += cpu->cpu_disp;
                ioInfo->busytime += cpu->iotime;
                ioInfo->dispatches += cpu->num_block;
                free(cpu);
                cpu = NULL;

                if(queue_length(ready_q) > 0 || queue_length(io_q) > 0 || io != NULL) {
                    (cpuInfo->idletime)++;
                }
            } else {
                (cpu->runtime)--;
                (cpu->cputime)++;
                (cpu->blocktime)--;
                if((cpu->isBlock == 1) && (cpu->blocktime == 0)) { //IF WE HAVE TO BLOCK
                    (cpu->num_block)++;
                    queue_enqueue(io_q, cpu);
                    cpu = NULL;
                    ioBlock = 1;
                }                
            }
        } else {
            (cpuInfo->idletime)++;
        }

        if((ioBlock == 1) && (io == NULL) && (queue_length(io_q) == 1)) { //CHECK IF BLOCK STATUS
            ioBlock = 0;
            (ioInfo->idletime)++;
        } else {
            ioBlock = 0;
            if(queue_length(io_q) > 0) {
                if(io == NULL) {
                    struct process* ptr = io_q->head->value;
                    int ioblocktime = 1;
                    if(ptr->runtime > 0) {
                        ioblocktime = ((int) random())%30 + 1;
                    }
                    ptr->iotime += ioblocktime;
                    ptr->remiotime = ioblocktime;
                    io = ptr;
                    queue_dequeue(io_q, (void**)&garbage);
                }
            }
            if(io != NULL) {
                (io->remiotime)--;
                if (io->remiotime <= 0) {
                    queue_enqueue(ready_q, io);
                    io = NULL;
                }
            } else {
                (ioInfo->idletime)++;
            }
        }
    }
}
 
void RR() {
    while((queue_length(ready_q) > 0) || (queue_length(io_q) > 0) || (cpu != NULL) || (io != NULL)) {
        wall_clock++;
        if(queue_length(ready_q) > 0) {
            if(cpu == NULL) {
                struct process* ptr = ready_q->head->value;

                if(ptr->runtime < 2) {
                    /*
                    if(ptr->runtime <= 5 && ptr->runtime > 0) {
                        int rand = (int)random();
//printf("RAND %d\t", rand);
                        ptr->blocktime = (rand%ptr->runtime) + 1;
                    } else {
                        int rand = (int)random();
//printf("RAND %d\t", rand);
                        ptr->blocktime = (rand%5) + 1;
                    }                    
                    if(ptr->blocktime > ptr->runtime) {
                        ptr->blocktime = ptr->runtime;
                    } */
                    ptr->isBlock = 0;
                } else {
                    int rand = (int)random();
//printf("RAND %d\t", rand);
                    float r = (float)rand / RAND_MAX;  
                    ptr->isBlock = 0;
                    if(ptr->prob >= r) {
                       if(ptr->runtime <= 5 && ptr->runtime > 0) {
                           int rand = (int)random();
//printf("RAND %d\t", rand);
                            ptr->blocktime = (rand%ptr->runtime) + 1;
                        } else {
                            int rand = (int)random();
//printf("RAND %d\t", rand);
                            ptr->blocktime = (rand%5) + 1;
                        }                    
                    if(ptr->blocktime > ptr->runtime) {
                        ptr->blocktime = ptr->runtime;
                    } 
                    ptr->isBlock = 1;
                    }
                }

                cpu = ptr;
                (cpu->cpu_disp)++;
                //printf("this is the quantum %d\n", cpu->quantum);
                queue_dequeue(ready_q, (void**)&garbage);
            }            
        }

        if(cpu != NULL) {
            if(cpu->runtime == 0) {
                printf("%-10s %6d     %6d    %6d    %6d    %6d\n", cpu->name, cpu->cputime, wall_clock, cpu->cpu_disp, cpu->num_block, cpu->iotime);
                cpuInfo->busytime += cpu->cputime;
                cpuInfo->dispatches += cpu->cpu_disp;
                ioInfo->busytime += cpu->iotime;
                ioInfo->dispatches += cpu->num_block;
                free(cpu);
                cpu = NULL;

                if(queue_length(ready_q) > 0 || queue_length(io_q) > 0 || io != NULL) {
                    (cpuInfo->idletime)++;
                }
            } else {
                (cpu->runtime)--;
                (cpu->cputime)++;
                (cpu->blocktime)--;
                (cpu->quantum)++;
                //printf("New Stats: %s\t%d\t%d\t %d\t %d\n", cpu->name, cpu->runtime, cpu->blocktime, cpu->quantum, cpu->isBlock);

                if((cpu->isBlock == 1) && (cpu->blocktime == 0)) { //IF WE HAVE TO BLOCK, run for quntum time 
                    (cpu->num_block)++;
                    queue_enqueue(io_q, cpu); //io disp
                    cpu = NULL;
                    ioBlock = 1;

                } else if(((cpu->quantum == 5) || (cpu->runtime == 0)) && (cpu->isBlock == 0)) {
                    queue_enqueue(ready_q, cpu);
                    cpu = NULL;
                    if(queue_length(ready_q) > 0) {
                        if(cpu == NULL) {
                        struct process* ptr = ready_q->head->value;
                        /*
                        if(ptr->runtime < 2) {
                            if(ptr->runtime <= 5 && ptr->runtime > 0) {
                                int rand = (int)random();
                                printf("RAND %d\t", rand);
                                ptr->blocktime = (rand%ptr->runtime) + 1;
                            } else {
                                int rand = (int)random();
                                printf("RAND %d\t", rand);
                                ptr->blocktime = (rand%5) + 1;
                            }                    
                            if(ptr->blocktime > ptr->runtime) {
                                ptr->blocktime = ptr->runtime;
                            } 
                            ptr->isBlock = 0;
                        } else {
                        
                        int rand = (int)random();
                        float r = (float)random() / RAND_MAX;  //float
                        ptr->isBlock = 1;
                        if(ptr->prob >= r) {  
                            if(ptr->runtime <= 5 && ptr->runtime > 0) {
                                ptr->blocktime = (rand%ptr->runtime) + 1;
                            } else {
                                ptr->blocktime = (rand%5) + 1;
                            }                    
                            if(ptr->blocktime > ptr->runtime) {
                                ptr->blocktime = ptr->runtime;
                            } 
                            ptr->isBlock = 1;
                            }
                        }*/
                        ptr->quantum = 0;
                        cpu = ptr;
                        (cpu->cpu_disp)++;
                        queue_dequeue(ready_q, (void**)&garbage);
                        }
                    }
                }    
            }
        } else {
            (cpuInfo->idletime)++;
        }     

        if((ioBlock == 1) && (io == NULL) && (queue_length(io_q) == 1)) { //CHECK IF BLOCK STATUS
            ioBlock = 0;
            (ioInfo->idletime)++;
        } else {
            ioBlock = 0;
            if(queue_length(io_q) > 0) {
                if(io == NULL) {
                    struct process* ptr = io_q->head->value;
                    int ioblocktime = 1;
                    if(ptr->runtime > 0) {
                        int rand = (int)random();
//printf("RAND %d\t", rand);
                        ioblocktime = (rand%30) + 1;
                    }
                    ptr->iotime += ioblocktime;
                    ptr->remiotime = ioblocktime;
                    //printf("IO Block Time: %d\n", ioblocktime);
                    io = ptr;
                    queue_dequeue(io_q, (void**)&garbage);
                }
            }
            if(io != NULL) {
                (io->remiotime)--;
                if (io->remiotime <= 0) {
                    queue_enqueue(ready_q, io);
                    io = NULL;
                }
            } else {
                (ioInfo->idletime)++;
            }
        }
    }
}

void output() {
    printf("\n");
    printf("System:\n");
    printf("The wall clock time at which the simulation finished: %d\n\n", wall_clock);
    printf("CPU:\n");
    printf("Total time spent busy: %d\n", cpuInfo->busytime);
    printf("Total time spent idle: %d\n",cpuInfo->idletime);
    printf("CPU utilization: %.2f\n", (double)cpuInfo->busytime/(double)wall_clock);
    printf("Number of dispatches: %d\n", cpuInfo->dispatches);
    printf("Overall throughput: %.2f\n\n", (double)cpuInfo->num/(double)wall_clock);
    printf("I/O device:\n");
    printf("Total time spent busy: %d\n", ioInfo->busytime);
    printf("Total time spent idle: %d\n", ioInfo->idletime);
    printf("I/O utilization: %.2f\n", ((double)ioInfo->busytime)/(double)wall_clock);
    printf("Number of dispatches: %d\n", ioInfo->dispatches);
    printf("Overall throughput: %.2f\n", (double)ioInfo->num/(double)wall_clock);    
}
