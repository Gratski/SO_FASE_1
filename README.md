# SO_FASE_1

## TODOS:

### Processos (main.c):
- Adicionar o código para tratar os parâmetros de entrada tal como especificados no código fonte. 
- Adicionar o código que permita criar todos os processos filho (clientes, rececionistas e instaladores). 
- Adicionar o código que permita ao processo principal aguardar a terminação dos filhos e registar o estado que cada um devolveu, através da chamada de sistema exit, na estrutura de dados indicadores.


### Alocação de Memória (memoria.c):

- Criação e terminação de um **buffer circular** (BServico), como zona de memória partilhada entre processos independentes, para os clientes colocarem as encomendas para os rececionistas. O tamanho do buffer deverá estar de acordo com o valor contido em Config.BUFFER_SERVICO (estrutura configuracao). 
- Criação e terminação de um **buffer de acesso aleatório** (BInstalacao), como zona de memória partilhada entre processos independentes, para os rececionistas colocarem os pedidos para os instaladores. O tamanho do buffer deverá estar de acordo com o valor contido em Config.BUFFER_INSTALACAO (estrutura configuracao). 
- Criação e terminação de um **buffer de acesso aleatório** (BConclusao), como zona de memória partilhada entre processos independentes, para os instaladores colocarem os relatórios de conclusão para os clientes. O tamanho do buffer deverá estar de acordo com o valor contido em Config.BUFFER_CONCLUSAO (estrutura configuracao). 
- Adicionar o código que permite aos clientes, rececionistas e instaladores escreverem e lerem a informação dos buffers partilhados. Funções a alterar: memoria_pedido_s_escreve, memoria_pedido_s_le, memoria_pedido_i_escreve, memoria_pedido_i_le, memoria_relatorio_c_escreve e memoria_relatorio_c_le. 
- Adicionar o código que permita actualizar os campos da estrutura servico nas etapas adequadas, ex: na função memoria_relatorio_c_escreve o rececionista deve colocar o seu id no campo rececionista. 
- Na função memoria_criar_indicadores, reservar e libertar memória dinamicamente para cada um dos campos da estrutura indicadores com os tamanhos necessários (consultar estrutura configuracao). Iniciar o vetor stock_inicial com os valores contidos em Config.stock e apagar o conteúdo dos vetores clientes_atendidos_pelos_rececionistas, clientes_atendidos_pelos_instaladores, servicos_obtidos_pelos_clientes e servicos_realizados_pelos_instaladores. 