#include <stdio.h>
#include <string.h>
#include <time.h>
#include "banco.h"

#define MAX 50

typedef struct {
    int ativa;
    double saldo;
    char historico[60];
} Conta;

typedef struct {
    char nome[50];
    int base;
    int titulo;
    Conta contas[3];
} Cliente;

Cliente clientes[MAX];
int total = 0;
int aberto = 0;
int vencedor = -1;
double premio = 0;
int contaPremio = 0;

const char *tipos[3] = {"conta corrente", "conta salario", "conta poupanca"};

double saldoTotal(Cliente *c) {
    double soma = 0;
    int t;
    for (t = 0; t < 3; t++) {
        if (c->contas[t].ativa) {
            soma += c->contas[t].saldo;
        }
    }
    return soma;
}

int aptoEmprestimo(Cliente *c) {
    return c->contas[0].ativa && c->contas[2].ativa && c->contas[2].saldo > 0;
}

void alertas() {
    int i;
    if (!aberto) {
        return;
    }
    for (i = 0; i < total; i++) {
        if (clientes[i].contas[0].ativa && clientes[i].contas[0].saldo < 0) {
            printf("ALERTA: %s esta com saldo negativo na conta corrente (%d-01).\n",
                   clientes[i].nome, clientes[i].base);
        }
    }
}

void abrirBanco() {
    int n, i, t, op, qtd;
    if (aberto) {
        printf("O banco ja esta aberto.\n");
        return;
    }
    printf("Quantidade de clientes: ");
    scanf("%d", &n);
    if (n < 1 || n > MAX) {
        printf("Quantidade invalida.\n");
        return;
    }
    for (i = 0; i < n; i++) {
        Cliente *c = &clientes[i];
        memset(c, 0, sizeof(Cliente));
        printf("\nNome do cliente %d: ", i + 1);
        scanf(" %49[^\n]", c->nome);
        c->base = 1000 + i;
        do {
            qtd = 0;
            for (t = 0; t < 3; t++) {
                c->contas[t].ativa = 0;
                if (t == 1 && c->contas[0].ativa) {
                    printf("Conta salario nao permitida junto com conta corrente.\n");
                    continue;
                }
                printf("Abrir %s? (1-sim 0-nao): ", tipos[t]);
                scanf("%d", &op);
                if (op == 1) {
                    c->contas[t].ativa = 1;
                    qtd++;
                }
            }
            if (qtd == 0) {
                printf("O cliente precisa ter pelo menos uma conta.\n");
            }
        } while (qtd == 0);
        for (t = 0; t < 3; t++) {
            if (c->contas[t].ativa) {
                c->contas[t].saldo = 0;
                strcpy(c->contas[t].historico, "Conta aberta");
                printf("Conta %d-%02d criada com saldo R$ 0.00\n", c->base, t + 1);
                printf("Informe o saldo da %s: ", tipos[t]);
                scanf("%lf", &c->contas[t].saldo);
                if (t == 1 && c->contas[t].saldo >= 1000 && c->contas[t].saldo <= 1500) {
                    printf("Comprar titulo de capitalizacao? (1-sim 0-nao): ");
                    scanf("%d", &op);
                    c->titulo = (op == 1);
                }
            }
        }
    }
    total = n;
    aberto = 1;
    printf("\nBanco aberto com %d clientes.\n", total);
}

void consultarHistorico() {
    int i, t;
    double soma = 0;
    for (i = 0; i < total; i++) {
        soma += saldoTotal(&clientes[i]);
    }
    printf("\n== Saldo Total do banco ==\n");
    printf("%d | R$ %.2f\n", total, soma);

    printf("== Clientes Aptos a Emprestimos ==\n");
    for (i = 0; i < total; i++) {
        if (aptoEmprestimo(&clientes[i])) {
            printf("%s|%d-03|%.2f\n", clientes[i].nome, clientes[i].base,
                   0.25 * 10 * saldoTotal(&clientes[i]));
        }
    }

    printf("== Clientes com Saldo Negativo na Conta Salario ==\n");
    for (i = 0; i < total; i++) {
        if (clientes[i].contas[1].ativa && clientes[i].contas[1].saldo < 0) {
            printf("%s|%d-02|%.2f\n", clientes[i].nome, clientes[i].base,
                   clientes[i].contas[1].saldo);
        }
    }

    printf("== Clientes Aptos a Compra de Titulos ==\n");
    for (i = 0; i < total; i++) {
        t = 1;
        if (clientes[i].contas[t].ativa && !clientes[i].titulo &&
            clientes[i].contas[t].saldo >= 1000 && clientes[i].contas[t].saldo <= 1500) {
            printf("%s|%d-02|%.2f\n", clientes[i].nome, clientes[i].base,
                   clientes[i].contas[t].saldo);
        }
    }

    printf("== Clientes Contemplado no Sorteio ==\n");
    if (vencedor >= 0) {
        printf("%s|%d-%02d|%.2f\n", clientes[vencedor].nome, clientes[vencedor].base,
               contaPremio + 1, premio);
    }
}

void efetuarSorteio() {
    int idx[MAX], i, j, aux, t, melhor, a, b, prox, n, pos;
    if (vencedor >= 0) {
        printf("O sorteio ja foi realizado.\n");
        return;
    }
    if (total < 5) {
        printf("E preciso ter pelo menos 5 clientes para o sorteio.\n");
        return;
    }
    for (i = 0; i < total; i++) {
        idx[i] = i;
    }
    for (i = 0; i < total - 1; i++) {
        for (j = 0; j < total - 1 - i; j++) {
            if (saldoTotal(&clientes[idx[j]]) < saldoTotal(&clientes[idx[j + 1]])) {
                aux = idx[j];
                idx[j] = idx[j + 1];
                idx[j + 1] = aux;
            }
        }
    }
    for (i = 0; i < 5; i++) {
        if (!clientes[idx[i]].titulo) {
            printf("Sorteio nao realizado: %s nao adquiriu titulo de capitalizacao.\n",
                   clientes[idx[i]].nome);
            return;
        }
    }
    a = 1;
    b = 1;
    n = time(NULL) % 10 + 5;
    for (i = 0; i < n; i++) {
        prox = a + b;
        a = b;
        b = prox;
    }
    pos = b % 5;
    vencedor = idx[pos];
    melhor = -1;
    for (t = 0; t < 3; t++) {
        if (clientes[vencedor].contas[t].ativa &&
            (melhor == -1 || clientes[vencedor].contas[t].saldo > clientes[vencedor].contas[melhor].saldo)) {
            melhor = t;
        }
    }
    contaPremio = melhor;
    premio = 10 * clientes[vencedor].contas[melhor].saldo;
    clientes[vencedor].contas[melhor].saldo += premio;
    strcpy(clientes[vencedor].contas[melhor].historico, "Premio do sorteio");
    printf("Cliente contemplado: %s | Premio: R$ %.2f\n", clientes[vencedor].nome, premio);
}

int bancoAberto() {
    return aberto;
}
