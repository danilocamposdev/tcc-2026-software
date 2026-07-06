# Barbosa Minerals - Software de Otimização da Produção (GUI)

Este repositório contém o código-fonte da aplicação desktop desenvolvida para a **Barbosa Minerals** como parte de um Trabalho de Conclusão de Curso (TCC) em Engenharia de Produção na **Universidade Federal de Minas Gerais (UFMG)**. 

O software fornece uma interface gráfica (GUI) amigável que abstrai a complexidade matemática do modelo de sequenciamento e alocação de produção da fábrica, permitindo que a gerência tome decisões otimizadas sem a necessidade de interagir diretamente com linhas de comando ou scripts matemáticos.

A aplicação consome internamente o motor de otimização desenvolvido em C++ com o solver **CP-SAT** do **Google OR-Tools**, disponível no repositório adjacente: [`tcc-2026-lib`](https://github.com/danilocamposdev/tcc-2026-lib).

---

## 📌 Contexto do Problema

A Barbosa Minerals (Juatuba - MG) atua na fabricação sob encomenda (*Make-to-Order*) de componentes críticos em borracha e poliuretano para os setores de mineração e indústria. 

O planejamento da produção era realizado manualmente em um quadro físico, dependendo exclusivamente do conhecimento tácito do gestor. Esta aplicação visa sanar o principal gargalo produtivo da linha de peças de borracha, otimizando o sequenciamento sob duas camadas críticas de capacidade:
1. **Disponibilidade das Matrizes (Moldes):** Recurso físico restritivo central compartilhado de uso simultâneo limitado.
2. **Capacidade e Nível dos Operadores:** Limitação de jornada diária (8 horas/slots de produção) e restrição de compatibilidade técnica entre a habilidade do operador e a complexidade exigida pela matriz da peça.

O objetivo do modelo integrado é **minimizar o atraso ponderado de ordens de produção**.

---

## 🛠️ Tecnologias Utilizadas

O ecossistema do software foi projetado utilizando práticas modernas de engenharia de software para garantir alto desempenho computacional e manutenibilidade:

* **Linguagem Principal:** C++ (padrão C++20)
* **Framework de Interface Gráfica:** Qt (Widgets)
* **Banco de Dados:** SQLite (padrão local e embarcado para persistência de dados)
* **Biblioteca de Otimização Interna:** [tcc-2026-lib](https://github.com/danilocamposdev/tcc-2026-lib) (utilizando Google OR-Tools CP-SAT Solver)

---

## 🏗️ Arquitetura do Sistema

O projeto adota os princípios de orientação a objetos, **SOLID** e conceitos inspirados na *Clean Architecture*.

---

## 🚀 Funcionalidades Principais

* **Cadastro e Gestão de Cadastros:**
  * Operadores (com definição de nível de habilidade 1 ou 2).
  * Matrizes (especificando dimensões, tempos de produção $h_m$ e complexidade exigida $q_m$).
  * Clientes e Pedidos (atribuição de pesos de prioridade e prazos de entrega $d_j$).
* **Motor de Resolução Operacional:**
  * Botão de disparo para rodar o modelo de otimização matemático integrado da `tcc-2026-lib`.
* **Visualizador de Cronograma:**
  * Exibição visual da agenda de produção organizada por dia útil da semana e por operador, respeitando os finais de semana e a jornada máxima permitida.

---

**Autores:** Danilo Teixeira Gonçalves Campos, Victória Dias de Almeida
 
**Orientador:** Prof. Dr. Maurício Cardoso
 
**Curso:** Engenharia de Produção - UFMG - 2026

