# Objetivo:

Este trabalho tem como objetivo avaliar a aprendizagem das técnicas de otimização de código, incluindo as técnicas/ferramentas de análise de código.

# Passos de Desenvolvimento:

1. Desenvolver, em C, uma versão sequencial otimizada do algoritmo k-means.
2. Definir função para criar conjunto de amostras para teste.
3. Algoritmo original pode ser alterado para minimizar o tempo total (mas filosofia do algoritmos original deve ser mantido)
4. Resultados: Atençãoooo
5. Entrega código e Makefile (segundo anexo 1)
6. Relatório pdf e máximo duas páginas (excluindo anexos). Com o template IEEE.

# Critérios de avaliação:

(i) Desempenho do código (50%)
(ii) Otimizações realizadas e justificação (30%)
(iii) Legibilidade do código e relatório (20%)

v2 remoção da op sqrt no calculo da distancia
v3 remover determinação de distancia inicial e menos um acesso à memória - menos um miss
v4 remover o pow

ver dependencias assembly
analisar complexidade
localidade espacial e temporal se existe ou se é possivel ter
vetorização

loop-unroll comparação das distancias -> tira jumps

perguntas : se é necessária a analise com diferentes otimizações: O2, O3, etc

//-----------------------------------------------------------
Podemos ver que com as otimizações que fizemos até aqui obtemos:
60% => update_cluster_points
30% => determine_new_centroid
1,5% => init

//A testar:

Dependências de atributos:
Spatial/Temporal locality:
Loop Unrool (not very good => show why)
Vetorization (não melhora em nenhum aspeto => por causa do malloc)

V1: og
V2: sem mat
V3: loop unroll
V4: loop unroll + vetorização
V5: sem malloc
V6: sem malloc + loop unroll
V7: sem malloc + loop unroll + vetorização
V8: sem malloc + vetorização
