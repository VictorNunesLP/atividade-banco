#include <stdio.h>
#include "banco.h"

int main() {
    int op;
    do {
        printf("\n");
        alertas();
        printf("\n===== MENU =====\n");
        printf("1 - Abrir o banco\n");
        printf("2 - Consultar historico\n");
        printf("3 - Efetuar sorteio\n");
        printf("4 - Sair\n");
        printf("Opcao: ");
        scanf("%d", &op);
        if ((op == 2 || op == 3) && !bancoAberto()) {
            printf("Abra o banco primeiro.\n");
            continue;
        }
        switch (op) {
            case 1:
                abrirBanco();
                break;
            case 2:
                consultarHistorico();
                break;
            case 3:
                efetuarSorteio();
                break;
            case 4:
                printf("Encerrando.\n");
                break;
            default:
                printf("Opcao invalida.\n");
        }
    } while (op != 4);
    return 0;
}
