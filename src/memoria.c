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

//******************************************
// CRIAR ZONAS DE MEMORIA
//
void * memoria_criar(char * name, int size)
{
    //==============================================
    // FUNÇÃO GENÉRICA DE CRIAÇÃO DE MEMÓRIA PARTILHADA
    //
    return so_memoria_criar(name, size);
    //==============================================
}
void memoria_criar_stock()
{
    //==============================================
    // CRIAR ZONA DE MEMÓRIA PARA O STOCK DE CADO SERVICO
    //
    // utilizar a função genérica memoria_criar(char *,int)
    so_memoria_criar_stock();
    //==============================================
}
void memoria_criar_buffers()
{
    //==============================================
    // CRIAR ZONAS DE MEMÓRIA PARA OS BUFFERS: PEDIDOS DE SERVIÇO, PEDIDOS DE INSTALAÇÃO e RELATÓRIOS DE CONCLUSÃO
    //
    // utilizar a função genérica memoria_criar(char *,int)
    so_memoria_criar_buffers();
    //==============================================
}
void memoria_criar_escalonador()
{
    //==============================================
    // CRIAR ZONA DE MEMÓRIA PARA O MAPA DE ESCALONAMENTO
    //
    // utilizar a função genérica memoria_criar(char *,int)
    so_memoria_criar_escalonador();
    //==============================================
}

void memoria_terminar(char * name, void * ptr, int size)
{
    //==============================================
    // FUNÇÃO GENÉRICA DE DESTRUIÇÃO DE MEMÓRIA PARTILHADA
    //
    so_memoria_terminar(name, ptr, size);
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
    so_memoria_destruir();
    //==============================================
}



//******************************************
// MEMORIA_PEDIDO_S_ESCREVE
//
void memoria_pedido_s_escreve (int id, struct servico *pServico)
{
    prodcons_pedido_s_produzir_inicio();

    //==============================================
    // ESCREVER PEDIDO DE SERVIÇO NO BUFFER PEDIDO DE SERVIÇOS
    //
    so_memoria_pedido_s_escreve (id, pServico);
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
void memoria_pedido_i_escreve (int id, struct servico *pServico)
{
    int pos,instalador;
    
    prodcons_pedido_i_produzir_inicio();
    
    // decidir a que instalador se destina
    instalador = escalonador_obter_instalador(id, pServico->id);

    //==============================================
    // ESCREVER PEDIDO NO BUFFER DE PEDIDOS DE INSTALACAO
    //
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
void memoria_relatorio_c_escreve (int id, struct servico *pServico)
{
    int pos;

    prodcons_relatorio_c_produzir_inicio();

    //==============================================
    // ESCREVER RELATORIO DE CONCLUSAO NO BUFFER DE RELATORIOS DE CONCLUSAO
    //
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
    so_memoria_criar_indicadores();
    // iniciar indicadores relevantes:
    // - Ind.stock_inicial
    // - Ind.clientes_atendidos_pelos_rececionistas
    // - Ind.clientes_atendidos_pelos_instaladores
    // - Ind.servicos_obtidos_pelos_clientes
    so_memoria_iniciar_indicadores();
    //==============================================
}

