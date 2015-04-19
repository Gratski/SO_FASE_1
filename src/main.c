#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <sys/mman.h> //mmap
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <signal.h>
#include <limits.h>

#include "main.h"
#include "rececionista.h"
#include "instalador.h"
#include "cliente.h"
#include "memoria.h"
#include "prodcons.h"
#include "controlo.h"
#include "ficheiro.h"
#include "tempo.h"
#include "so.h"

struct indicadores Ind;     // indicadores do funcionamento do SOinstala
struct configuracao Config; // configuração da execução do SOinstala

/* main_cliente recebe como parâmetro o nº de clientes a criar */
void main_cliente(int quant)
{


    //==============================================
    // CRIAR PROCESSOS
    //
    // após a criação de cada processo, chamar cliente_executar
    // e guardar pid do filho no vetor Ind.pid_clientes[n], com n=0,1,...

    // index de arrays para criacao de processos
    int counter;
    for( counter = 0;  counter < quant; counter++)
    {
        int pid = fork();
        
        if(pid == -1)
        {
            perror("erro processo");
            exit(1);
        }else
        {
            if(pid == 0)
            {
                
                int ret = cliente_executar(counter);
                exit(ret);
            }else
            {
                Ind.pid_clientes[counter] = pid;
                
            }
        
        }
    }
    //so_main_cliente(quant);
    //==============================================
}

/* main_rececionista recebe como parâmetro o nº de rececionistas a criar */
void main_rececionista(int quant)
{

    //==============================================
    // CRIAR PROCESSOS
    //
    // após a criação de cada processo, chamar rececionista_executar 
    // e guardar pid do filho no vetor Ind.pid_rececionistas[n], com n=0,1,...

    // index de arrays para criacao de processos
    int counter;
    for( counter = 0;  counter < quant; counter++)
    {
        int pid = fork();
        
        if(pid == -1)
        {
            perror("erro processo");
            exit(1);
        }else
        {
            if(pid == 0)
            {
                
                int ret = rececionista_executar(counter);
                exit(ret);
            }else
            {
                Ind.pid_rececionistas[counter] = pid;
                
            }
        
        }
    }
    //so_main_rececionista(quant);
    //==============================================
}

/* main_instalador recebe como parâmetro o nº de instaladores a criar */
void main_instalador(int quant)
{

    //==============================================
    // CRIAR PROCESSOS
    //
    // após a criação de cada processo, chamar instalador_executar 
    // e guardar pid do filho no vetor Ind.pid_instaladores[n], com n=0,1,...

    // index de arrays para criacao de processos
    int counter;
    for( counter = 0;  counter < quant; counter++)
    {
        int pid = fork();
        
        if(pid == -1)
        {
            perror("erro processo");
            exit(1);
        }else
        {
            if(pid == 0)
            {
                
                int ret = instalador_executar(counter);
                exit(ret);
            }else
            {
                Ind.pid_instaladores[counter] = pid;
                
            }
        
        }
    }
    //so_main_instalador(quant);
    //==============================================
}

int main(int argc, char* argv[])
{
    int n,result;
    char *ficEntrada=NULL;
    char *ficSaida=NULL;
    char *ficLog=NULL;
    long intervalo=0;

    //==============================================
    // TRATAR PARÂMETROS DE ENTRADA
    // parâmetro obrigatório: ficheiro_configuracao
    // parâmetros opcionais:
    //   ficheiro_resultados
    //   -l ficheiro_log
    //   -t intervalo(us)    // us: microsegundos
    //
    // para qualquer parâmetro desconhecido ou falta de parâmetros
    // escrever mensagem de utilização "Como usar", dar exemplo e terminar

    //erro no parametros
    if(argc < 2)
    {
        printf("Como usar: ./soinstala ../testes/in/cenario1 ../testes/out/cenario1 -l cenario1.log -t 1000\n");
        exit(1);
    }
    else if(argc > 2){
        ficEntrada = argv[1];       //ficheiro de config
        ficSaida = argv[2];     //ficheiro de output
        ficLog = argv[4];       //ficheiro de logs
        intervalo = atoi(argv[6]);  //intervalo
    }else{
        ficEntrada = argv[1];
    }
    /*intervalo = so_main_args(argc, argv, &ficEntrada, &ficSaida, &ficLog);*/
    //==============================================

    printf("\n---------------------------");
    printf("\n--- Oficina SOinstala ---");
    printf("\n---------------------------\n");

    // Ler dados de entrada
    ficheiro_iniciar(ficEntrada,ficSaida,ficLog);

    // criar zonas de memória e semáforos
    memoria_criar_buffers();
    prodcons_criar_buffers();
    controlo_criar();

    // Criar estruturas para indicadores e guardar stock inicial
    memoria_criar_indicadores();

    printf("\n*** Abrir SOinstala\n");
    controlo_abrir_soinstala();

    // Registar início de operação e armar alarme
    tempo_iniciar(intervalo);

    //
    // Iniciar sistema
    //

    // Criar rececionistas
    main_rececionista(Config.RECECIONISTAS);
    // Criar instalador
    main_instalador(Config.INSTALADORES);
    // Criar clientes
    main_cliente(Config.CLIENTES);


    //==============================================
    // ESPERAR PELA TERMINAÇÃO DOS CLIENTES E ATUALIZAR INDICADORES
    //
    // esperar e incrementar o indicador de clientes
    // Ind.servicos_obtidos_pelos_clientes[n], n=0,1,...
    // com o estado devolvido pela terminação do processo
    int counter;
    for(counter = 0; counter < Config.CLIENTES; counter++)
    {
        int status;
        waitpid(Ind.pid_clientes[counter], &status, 0);
        if (WEXITSTATUS(status) != Config.SERVICOS) {
            Ind.servicos_obtidos_pelos_clientes[WEXITSTATUS(status)]++;
        }

    }
    // so_main_wait_clientes();
    //==============================================

    printf("*** Fechar SOinstala\n\n");
    controlo_fechar_soinstala();

    //==============================================
    // ESPERAR PELA TERMINAÇÃO DOS RECECIONISTAS E ATUALIZAR INDICADORES
    //
    // esperar e incrementar o indicador de rececionistas
    // Ind.clientes_atendidos_pelos_rececionistas[n], n=0,1,...
    // com o estado devolvido pela terminação do processo
    for(counter = 0; counter < Config.RECECIONISTAS; counter++)
    {
        int status;
        waitpid(Ind.pid_rececionistas[counter], &status, 0);
        Ind.clientes_atendidos_pelos_rececionistas[counter] += WEXITSTATUS(status);
    }
    // so_main_wait_rececionistas();
    //==============================================

    //==============================================
    // ESPERAR PELA TERMINAÇÃO DOS CLIENTES E ATUALIZAR INDICADORES
    //
    // esperar e incrementar o indicador de instaladores
    // Ind.clientes_atendidos_pelos_instaladores[n], n=0,1,...
    // com o estado devolvido pela terminação do processo
    for(counter = 0; counter < Config.INSTALADORES; counter++)
    {
        int status;
        waitpid(Ind.pid_instaladores[counter], &status, 0);
        Ind.clientes_atendidos_pelos_instaladores[counter] += WEXITSTATUS(status);
    }
    // so_main_wait_instaladores();
    //==============================================

    printf("*** Indicadores\n");
    so_escreve_indicadores();

    // destruir zonas de memória e semáforos
    ficheiro_destruir();
    controlo_destruir();
    prodcons_destruir();
    memoria_destruir();

    return 0;
}