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
#include "controlo.h"
#include "prodcons.h"

struct prodcons ProdCons;

//******************************************
// SEMAFORO_CRIAR
//
sem_t * semaforo_criar(char * name, int value)
{
    //==============================================
    // FUNÇÃO GENÉRICA DE CRIAÇÃO DE UM SEMÁFORO
    //
    return so_semaforo_criar(name, value);
    //==============================================
}
void prodcons_criar_stock()
{
    //==============================================
    // CRIAR MUTEX PARA CONTROLAR O ACESSO AO STOCK
    //
    // utilizar a função genérica semaforo_criar
    so_prodcons_criar_stock();
    //==============================================
}
void prodcons_criar_buffers()
{
    //==============================================
    // CRIAR SEMAFOROS PARA CONTROLAR O ACESSO AOS 3 BUFFERS
    //
    // utilizar a função genérica semaforo_criar
    so_prodcons_criar_buffers();
    //==============================================
}
void semaforo_terminar(char * name, void * ptr)
{
    //==============================================
    // FUNÇÃO GENÉRICA DE DESTRUIÇÃO DE UM SEMÁFORO
    //
    so_semaforo_terminar(name, ptr);
    //==============================================
}

void prodcons_destruir()
{
    //==============================================
    // DESTRUIR SEMÁFORO E RESPETIVO NOME
    //
    // utilizar a função genérica semaforo_terminar
    so_prodcons_destruir();
    //==============================================
}


//******************************************
void prodcons_pedido_s_produzir_inicio()
{
    //==============================================
    // CONTROLAR ACESSO DE PRODUTOR AO BUFFER PEDIDO DE SERVIÇO
    //
    so_prodcons_pedido_s_produzir_inicio();
    //==============================================
}
//******************************************
void prodcons_pedido_s_produzir_fim()
{
    //==============================================
    // CONTROLAR ACESSO DE PRODUTOR AO BUFFER PEDIDO DE SERVIÇO
    //
    so_prodcons_pedido_s_produzir_fim();
    //==============================================
}
//******************************************
void prodcons_pedido_s_consumir_inicio()
{
    //==============================================
    // CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER PEDIDO DE SERVIÇO
    //
    so_prodcons_pedido_s_consumir_inicio();
    //==============================================
}
//******************************************
void prodcons_pedido_s_consumir_fim()
{
    //==============================================
    // CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER PEDIDO DE SERVIÇO
    //
    so_prodcons_pedido_s_consumir_fim();
    //==============================================
}
//******************************************
void prodcons_pedido_i_produzir_inicio()
{
    //==============================================
    // CONTROLAR ACESSO DE PRODUTOR AO BUFFER PEDIDO DE INSTALACAO
    //
    so_prodcons_pedido_i_produzir_inicio();
    //==============================================
}
//******************************************
void prodcons_pedido_i_produzir_fim()
{
    //==============================================
    // CONTROLAR ACESSO DE PRODUTOR AO BUFFER PEDIDO DE INSTALACAO
    //
    so_prodcons_pedido_i_produzir_fim();
    //==============================================
}
//******************************************
void prodcons_pedido_i_consumir_inicio()
{
    //==============================================
    // CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER PEDIDO DE INSTALACAO
    //
    so_prodcons_pedido_i_consumir_inicio();
    //==============================================
}
//******************************************
void prodcons_pedido_i_consumir_fim()
{
    //==============================================
    // CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER PEDIDO DE INSTALACAO
    //
    so_prodcons_pedido_i_consumir_fim();
    //==============================================
}
//******************************************
void prodcons_relatorio_c_produzir_inicio()
{
    //==============================================
    // CONTROLAR ACESSO DE PRODUTOR AO BUFFER RELATORIO DE CONCLUSAO
    //
    so_prodcons_relatorio_c_produzir_inicio();
    //==============================================
}
//******************************************
void prodcons_relatorio_c_produzir_fim()
{
    //==============================================
    // CONTROLAR ACESSO DE PRODUTOR AO BUFFER RELATORIO DE CONCLUSAO
    //
    so_prodcons_relatorio_c_produzir_fim();
    //==============================================
}
//******************************************
void prodcons_relatorio_c_consumir_inicio()
{
    //==============================================
    // CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER RELATORIO DE CONCLUSAO
    //
    so_prodcons_relatorio_c_consumir_inicio();
    //==============================================
}
//******************************************
void prodcons_relatorio_c_consumir_fim()
{
    //==============================================
    // CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER RELATORIO DE CONCLUSAO
    //
    so_prodcons_relatorio_c_consumir_fim();
    //==============================================
}
//******************************************
void prodcons_buffers_inicio()
{
    //==============================================
    // CONTROLAR ACESSO AOS 3 BUFFERS
    //
    so_prodcons_buffers_inicio();
    //==============================================
}
//******************************************
void prodcons_buffers_fim()
{
    //==============================================
    // CONTROLAR ACESSO AOS 3 BUFFERS
    //
    so_prodcons_buffers_fim();
    //==============================================
}
//******************************************
int prodcons_atualizar_stock(int servico)
{
    //==============================================
    // OBTER MUTEX DO STOCK E ATUALIZAR STOCK
    //
    // se stock de servico>0 então reduzi-lo de uma unidade
    return so_prodcons_atualizar_stock(servico);
    //==============================================
}
