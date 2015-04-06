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
#include "so.h"
#include "memoria.h"
#include "prodcons.h"
#include "controlo.h"
#include "escalonador.h"
#include "ficheiro.h"

struct relatorio_c BConclusao;  // buffer instalador-cliente
struct pedido_i    BInstalacao; // buffer rececionista-instalador
struct pedido_s    BServico;    // buffer cliente-rececionista

extern struct configuracao Config;
extern struct escalonamento Escalonamento;
extern struct indicadores Ind; 

//******************************************
// CRIAR ZONAS DE MEMORIA
//
// parâmetros de entrada:
// name - nome para a zona de memória partilhada
// size - dimensão da memória partilhada
//
// retorno:
// endereço para a memória partilhada
//
// obs: os grupos que queiram ter nomes para a zona de memória partilhada que
// não colidam com os nomes utilizados por outros grupos devem usar os ficheiros
// disponibilizados no site da disciplina 

void * memoria_criar(char * name, int size)
{
    //==============================================
    // FUNÇÃO GENÉRICA DE CRIAÇÃO DE MEMÓRIA PARTILHADA
    //

    // int ret;
    // int *ptr;
    // int fd = shm_open(name, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
    // if(fd == -1){
    //     perror("Não conseguiu criar memoria_partilhada");
    //     exit(1);
    // }

    // ret = ftruncate(fd, size);
    // if (ret == -1){
    //     perror("Não conseguiu inicializar memoria_partilhada");
    //     exit(2);
    // }

    // ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    // if (ptr == MAP_FAILED){
    //     perror("Não devolveu o endereço de memória partilhada");
    //     exit(3);
    // } 

    printf("------------CRIA---------- %s - %d\n", name, size);

    // return ptr;

    return so_memoria_criar(name, size);
    //==============================================
}

// esta função altera: 
// -- Config.stock
// e usa informação da variável Config 
void memoria_criar_stock()
{
    //==============================================
    // CRIAR ZONA DE MEMÓRIA PARA O STOCK DE CADA SERVICO
    //
    // utilizar a função genérica memoria_criar(char *,int)


    // char *line = Config.lista_servicos;
    // printf("nr de serviços: %d\n", Config.SERVICOS);
    // printf("Linha de serviços: %s\n", Config.lista_servicos);
    // printf("Linha de clientes: %s\n", Config.lista_clientes);
    // printf("Linha de recepcionistas: %s\n", Config.lista_rececionistas);
    // printf("Linha de instaladores: %s\n", Config.lista_instaladores);
    // char *cur;
    // int index;
    // //vector
    // int *arr = (int *) malloc (sizeof(int) * Config.SERVICOS);
    // cur = strtok(line, " ");
    
    // while(cur != NULL){
    //     printf("%d\n", (atoi(cur)));
    //     cur = strtok(NULL, " ");
    // }
    // free(arr);
    
    Config.stock = memoria_criar("shm_stock", (sizeof(int) * Config.SERVICOS));

    // so_memoria_criar_stock();
    //==============================================
}

// esta função altera:
// -- BConclusao
// -- BInstalacao
// -- BServico
// e usa informação da variável Config
//
// obs:
// para cada uma das 3 variáveis têm de reservar espaço para ambos os campos
// (buffer e ptr)
// ver no enunciado do trabalho (pags 6 e 7) a forma como é efectuado o acesso a
// estes buffers e o seu conteúdo - estes buffers não são simplesmente buffers de
// inteiros 
void memoria_criar_buffers()
{
    //==============================================
    // CRIAR ZONAS DE MEMÓRIA PARA OS BUFFERS: PEDIDOS DE SERVIÇO, PEDIDOS DE INSTALAÇÃO e RELATÓRIOS DE CONCLUSÃO
    //
    // utilizar a função genérica memoria_criar(char *,int)

    int size_servico = ((sizeof(int) * 5) + (sizeof(long) + sizeof(time_t)) * 3);

    BConclusao.buffer = memoria_criar("shm_relatorio_c_buffer", (size_servico * Config.BUFFER_CONCLUSAO));
    BConclusao.ptr = memoria_criar("shm_relatorio_c_ptr", (sizeof(int) * Config.BUFFER_CONCLUSAO));

    BInstalacao.buffer = memoria_criar("shm_pedido_i_buffer", (size_servico * Config.BUFFER_CONCLUSAO));
    BInstalacao.ptr = memoria_criar("shm_pedido_i_ptr", (sizeof(int) * Config.BUFFER_INSTALACAO));

    BServico.buffer = memoria_criar("shm_pedido_s_buffer", (size_servico * Config.BUFFER_CONCLUSAO));
    BServico.ptr = memoria_criar("shm_pedido_s_ptr", (sizeof(int) * 2));

    // so_memoria_criar_buffers();
    //==============================================
}

// esta função altera:
// -- Escalonamento.ptr
// e usa informação da variável Config 
void memoria_criar_escalonador()
{
    //==============================================
    // CRIAR ZONA DE MEMÓRIA PARA O MAPA DE ESCALONAMENTO
    //
    // utilizar a função genérica memoria_criar(char *,int)

    Escalonamento.ptr = memoria_criar("shm_escalonador", (sizeof(int) * Config.SERVICOS * Config.INSTALADORES));

    // so_memoria_criar_escalonador();
    //==============================================
}

// parâmetros de entrada:
// name - nome para a zona de memória partilhada
// size - dimensão da memória partilhada
// ptr - apontador para a zona de memória partilhada 
void memoria_terminar(char * name, void * ptr, int size)
{
    //==============================================
    // FUNÇÃO GENÉRICA DE DESTRUIÇÃO DE MEMÓRIA PARTILHADA
    //

    int ret;
    ret = munmap(ptr, size);
    if(ret == -1){
        perror("Não conseguiu terminar a projecção");
        exit(7);
    }

    ret = shm_unlink(name);
    if(ret == -1){
        perror("Não conseguiu eliminar a memória partilhada");
        exit(8);
    } 

    printf("------------TERMINA---------- %s - %d\n", name, size);

    // so_memoria_terminar(name, ptr, size);
    //==============================================
}

//******************************************
// MEMORIA_DESTRUIR
//
void memoria_destruir()
{
    //==============================================
    // DESTRUIR MAPEAMENTO E NOME DE PÁGINAS DE MEMÓRIA
    //
    // utilizar a função genérica memoria_terminar(char *,void *,int)

    // memoria_terminar("shm_relatorio_c_ptr", BConclusao.ptr, 40);
    // memoria_terminar("shm_relatorio_c_buffer", BConclusao.buffer, 440);
    // memoria_terminar("shm_pedido_i_ptr", BInstalacao.ptr, 40);
    // memoria_terminar("shm_pedido_i_buffer", BInstalacao.buffer, 440);
    // memoria_terminar("shm_pedido_s_ptr", BServico.ptr, 40);
    // memoria_terminar("shm_pedido_s_buffer", BServico.buffer, 440);
    // memoria_terminar("shm_escalonador", Escalonamento.ptr, 24);
    // memoria_terminar("shm_stock", Config.stock, 20);
    // memoria_terminar("/shm_cond_soinstala", Config.stock, 160);
    // memoria_terminar("/shm_servicos_instaladores", Config.stock, 24);

    so_memoria_destruir();
    //==============================================
}



//******************************************
// MEMORIA_PEDIDO_S_ESCREVE
//
// parâmetros de entrada:
// id - identificador do cliente
// pServico - com os identificadores do serviço e do cliente
//
// altera:
// BServiço
//
// obs:
// id igual a pServico.id 
void memoria_pedido_s_escreve (int id, struct servico *pServico)
{
    prodcons_pedido_s_produzir_inicio();

    //==============================================
    // ESCREVER PEDIDO DE SERVIÇO NO BUFFER PEDIDO DE SERVIÇOS
    //

    BServico.buffer = &(*pServico);
    BServico.ptr.in = BServico.ptr.in + 1;
    // BServico.buffer.cliente = *pServico.cliente;

    // so_memoria_pedido_s_escreve (id, pServico);
    //==============================================
    
    prodcons_pedido_s_produzir_fim();
    
    // informar rececionista de pedido de servico
    controlo_cliente_submete_pedido(id);

    // registar hora do pedido de servico
    tempo_registar(&BServico.buffer[BServico.ptr->in].hora_servico);
    
    // log
    ficheiro_escrever_log_ficheiro(1,id);
}
//******************************************
// MEMORIA_PEDIDO_S_LE
//
// parâmetros de entrada:
// id - identificador do recepcionista
//
// parâmetros de saída:
// pServiço (campos cliente, id, hora_servico.tv_sec, hora_servico.tv_nsec)
//
// altera:
// BServiço 
int memoria_pedido_s_le (int id, struct servico *pServico)
{
    // testar se existem clientes e se o SOinstala esta aberto
    if(controlo_rececionista_aguarda_pedido(id) == 0)
        return 0;

    prodcons_pedido_s_consumir_inicio();
    
    //==============================================
    // LER PEDIDO DE SERVIÇO DO BUFFER PEDIDO DE SERVIÇOS
    //
    so_memoria_pedido_s_le (id, pServico);
    //==============================================

    // testar se existe stock do servico pedido pelo cliente
    if(prodcons_atualizar_stock(pServico->id) == 0) {
        pServico->disponivel = 0;
    prodcons_pedido_s_consumir_fim();
        return 2;
    } else
        pServico->disponivel = 1;
    
    prodcons_pedido_s_consumir_fim();

    // log
    ficheiro_escrever_log_ficheiro(2,id);
    
    return 1;
}



//******************************************
// MEMORIA_PEDIDO_I_ESCREVE
//
// parâmetros de entrada:
// id - identificador do recepcionista
// pServico - com os identificadores do serviço e cliente, hora_servico.tv_sec e
// hora_servico.tv_nsec
// 
void memoria_pedido_i_escreve (int id, struct servico *pServico)
{
    int pos,instalador;
    
    prodcons_pedido_i_produzir_inicio();
    
    // decidir a que instalador se destina
    instalador = escalonador_obter_instalador(id, pServico->id);

    //==============================================
    // ESCREVER PEDIDO NO BUFFER DE PEDIDOS DE INSTALACAO
    //
    // pos - índice do buffer onde o pedido foi escrito
    pos = so_memoria_pedido_i_escreve (id, pServico, instalador);
    //==============================================

    prodcons_pedido_i_produzir_fim();

    // informar instalador respetivo de pedido de instalacao
    controlo_rececionista_submete_pedido(instalador);

    // registar hora pedido (instalacao)
    tempo_registar(&BConclusao.buffer[pos].hora_instalacao);
    
    // log
    ficheiro_escrever_log_ficheiro(3,id);
}
//******************************************
// MEMORIA_PEDIDO_LE
//
// parâmetros de entrada:
// id - identificador do instalador
//
// parâmetros de saída:
// pServico - com os identificadores do serviço, cliente, recepcionista, e
// hora_servico.tv_sec, hora_servico.tv_nsec, hora_instalacao.tv_sec,
// hora_instalacao.tv_nsec
// 
int memoria_pedido_i_le (int id, struct servico *pServico)
{
    // testar se existem pedidos e se o SOinstala esta aberto
    if(controlo_instalador_aguarda_pedido(id) == 0)
        return 0;
    
    prodcons_pedido_i_consumir_inicio();
    
    //==============================================
    // LER PEDIDO DO BUFFER DE PEDIDOS DE INSTALACAO
    //
    so_memoria_pedido_i_le (id, pServico);
    //==============================================

    prodcons_pedido_i_consumir_fim();

    // log
    ficheiro_escrever_log_ficheiro(4,id);
    
    return 1;
}



//******************************************
// MEMORIA_RELATORIO_C_ESCREVE
//
// parâmetros de entrada:
// id - identificador do instalador
// pServico - com os identificadores do serviço, cliente, recepcionista, e
// hora_servico.tv_sec, hora_servico.tv_nsec, hora_instalacao.tv_sec,
// hora_instalacao.tv_nsec
//
void memoria_relatorio_c_escreve (int id, struct servico *pServico)
{
    int pos;

    prodcons_relatorio_c_produzir_inicio();

    //==============================================
    // ESCREVER RELATORIO DE CONCLUSAO NO BUFFER DE RELATORIOS DE CONCLUSAO
    //
    // pos - índice do buffer onde o pedido foi escrito 
    pos = so_memoria_relatorio_c_escreve (id, pServico);
    //==============================================

    prodcons_relatorio_c_produzir_fim();
    
    // informar cliente de que o relatorio de conclusao esta pronto
    controlo_instalador_submete_relatorio(pServico->cliente);

    // registar hora pronta (relatorio)
    tempo_registar(&BConclusao.buffer[pos].hora_conclusao);
    
    // log
    ficheiro_escrever_log_ficheiro(5,id);
}
//******************************************
// MEMORIA_RELATORIO_C_LE
//
// parâmetros de entrada:
// id - identificador do cliente
//
// parâmetros de saída:
// pServico -
// 
void memoria_relatorio_c_le (int id, struct servico *pServico)
{
    int n;
    
    // aguardar relatorio
    controlo_cliente_aguarda_relatorio(pServico->cliente);
    
    prodcons_relatorio_c_consumir_inicio();
    
    //==============================================
    // LER RELATORIO DO BUFFER DE RELATORIOS DE CONCLUSAO
    //
    so_memoria_relatorio_c_le (id, pServico);
    //==============================================

    prodcons_relatorio_c_consumir_fim();

    // log
    ficheiro_escrever_log_ficheiro(6,id);
}



//******************************************
// MEMORIA_CRIAR_INDICADORES
//
void memoria_criar_indicadores()
{
    //==============================================
    // CRIAR ZONAS DE MEMÓRIA PARA OS INDICADORES
    //
    // criação dinâmica de memória
    // para cada campo da estrutura indicadores
    // por exemplo o Ind.pid_clientes tem de ter espaço para todos os pids dos
    // clientes 
    so_memoria_criar_indicadores();
    // iniciar indicadores relevantes:
    // - Ind.stock_inicial
    // - Ind.clientes_atendidos_pelos_rececionistas
    // - Ind.clientes_atendidos_pelos_instaladores
    // - Ind.servicos_obtidos_pelos_clientes
    so_memoria_iniciar_indicadores();
    //==============================================
}

