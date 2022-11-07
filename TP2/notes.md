# Trabalho Prático 2:

* Usar o código do TP1
* Usar as primitivas OpenMP (para criar várias threads)
* Pontos:
1. Identificar os blocos de código com maior carga computacional;
2. Apresentar e analisar diferentes alternativas para exploração de paralelismo para cada bloco identificado em 1;
3. Selecionar a alternativa mais viável justificando com uma análise de escalabilidade;
4. Implementação da versão paralela mais adequada ao ambiente de execução (nó do Search da fila cpar)
5. Analisar o desempenho da proposta implementada


## Resultados:
Tempos de execução no Search (N = 10 000 000)
Clusters = 4 e 32
Número de iterações fixo = 20 iterações

## Grupos, estrutura do código/relatório e datas:
Importante: Código e Makefile == Anexo I

Relatório: máximo 2 páginas, template IEEE

Entrega: Pasta zipada com código, Makefile, e Relatório

## Critérios de Avaliação:
(i)  (40%) Proposta de paralelização
(ii) (20%) Desempenho do código
(iii)(40%) Relatório: com análise de escalabilidade, medição do perfil de execução, balanceamento de carga

## Anexo I:

### Input:
* Deve ser passado como argumento da função.
* três argumentos: 1) Número de Pontos. 2) Número de Clusters. 3) Número de Fios de Execução
* ./k_means 10000000 4 2

### Validação do resultado e output do programa:
* Resultados da primeira fase;
* Mas critério de paragem: 20 iterações

### Makefile:
* Executável k_means na diretoria bin
* make runseq => execução sequencial
* make runpar => execução paralela
* CFLAGS que quisermos
* CP_CLUSTERS será variável de ambiente.
* THREADS é o número de threads ideiais.