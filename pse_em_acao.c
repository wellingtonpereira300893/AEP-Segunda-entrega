/*
 * PSE EM ACAO - Grupo Gaia - AEP 2a entrega
 * Sistema academico para planejar e acompanhar acoes coletivas do
 * Programa Saude na Escola. C99, terminal, armazenamento em memoria.
 * Somente dados ficticios e coletivos. Nao inserir dados de estudantes.
 *
 * Compilar: gcc -std=c99 -Wall -Wextra -pedantic -o pse_em_acao pse_em_acao.c
 * Executar: ./pse_em_acao (Windows: pse_em_acao.exe)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

#ifndef MAX_ACOES
#define MAX_ACOES 100
#endif
#define TAM_ESCOLA 60
#define TAM_TURMA 40
#define TAM_PERIODO 12
#define TAM_TEMA 60
#define TAM_DATA 11
#define TAM_PUBLICO 50
#define TAM_RESPONSAVEL 60
#define TAM_BUFFER 256

#define PLANEJADA 1
#define REALIZADA 2
#define CANCELADA 3

typedef struct {
    int codigo;
    char escola[TAM_ESCOLA];
    char turma[TAM_TURMA];
    char periodo[TAM_PERIODO];
    char tema[TAM_TEMA];
    char dataPrevista[TAM_DATA];
    char publicoAlvo[TAM_PUBLICO];
    char responsavel[TAM_RESPONSAVEL];
    int qtdPrevista;
    int qtdEfetiva;
    int situacao;
} Acao;

static Acao acoes[MAX_ACOES];
static int totalAcoes = 0;

/* ---------- Entrada e validacao ---------- */
static void lerLinha(char *destino, size_t capacidade)
{
    size_t len;
    int c;
    if (fgets(destino, (int)capacidade, stdin) == NULL) {
        puts("\nEntrada encerrada. Finalizando o programa.");
        exit(EXIT_SUCCESS);
    }
    len = strlen(destino);
    if (len > 0 && destino[len - 1] == '\n') {
        destino[len - 1] = '\0';
    } else if (len == capacidade - 1) {
        while ((c = getchar()) != '\n' && c != EOF) { /* descartar excesso */ }
    }
}

static void aparar(char *texto)
{
    size_t inicio = 0;
    size_t fim = strlen(texto);
    while (texto[inicio] && isspace((unsigned char)texto[inicio])) inicio++;
    while (fim > inicio && isspace((unsigned char)texto[fim - 1])) fim--;
    memmove(texto, texto + inicio, fim - inicio);
    texto[fim - inicio] = '\0';
}

static int lerInteiro(const char *pergunta, int *resultado)
{
    char linha[TAM_BUFFER];
    char *fim;
    long numero;
    printf("%s", pergunta);
    lerLinha(linha, sizeof linha);
    aparar(linha);
    if (linha[0] == '\0') return 0;
    errno = 0;
    numero = strtol(linha, &fim, 10);
    if (*fim != '\0' || errno == ERANGE || numero < INT_MIN || numero > INT_MAX)
        return 0;
    *resultado = (int)numero;
    return 1;
}

static int lerInteiroMinimo(const char *pergunta, int minimo)
{
    int numero;
    while (1) {
        if (!lerInteiro(pergunta, &numero)) {
            puts("Erro: informe um numero inteiro valido.");
        } else if (numero < minimo) {
            printf("Erro: informe um numero maior ou igual a %d.\n", minimo);
        } else {
            return numero;
        }
    }
}

static int lerOpcao(const char *pergunta, int menor, int maior)
{
    int numero;
    while (1) {
        if (lerInteiro(pergunta, &numero) && numero >= menor && numero <= maior)
            return numero;
        printf("Erro: escolha uma opcao de %d a %d.\n", menor, maior);
    }
}

static void lerTextoObrigatorio(const char *pergunta, char *destino, size_t capacidade)
{
    char linha[TAM_BUFFER];
    while (1) {
        printf("%s", pergunta);
        lerLinha(linha, sizeof linha);
        aparar(linha);
        if (!linha[0]) {
            puts("Erro: este campo obrigatorio nao pode ficar vazio.");
        } else if (strlen(linha) >= capacidade) {
            printf("Erro: texto muito longo (maximo %zu caracteres).\n", capacidade - 1);
        } else {
            strcpy(destino, linha);
            return;
        }
    }
}

/* Valida formato DD/MM/AAAA, digitos, ano e dias reais, incluindo bissextos. */
static int dataValida(const char *data)
{
    static const int dias[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    int d, m, a, limite, i, bissexto;
    if (strlen(data) != 10 || data[2] != '/' || data[5] != '/') return 0;
    for (i = 0; i < 10; i++) {
        if (i != 2 && i != 5 && !isdigit((unsigned char)data[i])) return 0;
    }
    d = (data[0]-'0')*10 + data[1]-'0';
    m = (data[3]-'0')*10 + data[4]-'0';
    a = (data[6]-'0')*1000 + (data[7]-'0')*100 + (data[8]-'0')*10 + data[9]-'0';
    if (a < 2000 || a > 2100 || m < 1 || m > 12 || d < 1) return 0;
    bissexto = (a % 4 == 0 && a % 100 != 0) || a % 400 == 0;
    limite = dias[m - 1] + (m == 2 && bissexto ? 1 : 0);
    return d <= limite;
}

static void lerData(const char *pergunta, char *destino)
{
    while (1) {
        lerTextoObrigatorio(pergunta, destino, TAM_DATA);
        if (dataValida(destino)) return;
        puts("Erro: data invalida. Use DD/MM/AAAA com uma data real (ano 2000 a 2100).");
    }
}

/* ---------- Funcoes de apoio ---------- */
static int buscarIndicePorCodigo(int codigo)
{
    int i;
    for (i = 0; i < totalAcoes; i++)
        if (acoes[i].codigo == codigo) return i;
    return -1;
}

static const char *nomeSituacao(int situacao)
{
    switch (situacao) {
        case PLANEJADA: return "Planejada";
        case REALIZADA: return "Realizada";
        case CANCELADA: return "Cancelada";
        default: return "Desconhecida";
    }
}

/* Compara trechos em maiusculas/minusculas ASCII, sem alterar os originais. */
static int contemTexto(const char *texto, const char *termo)
{
    size_t i, j, tamanho = strlen(termo);
    if (tamanho == 0) return 1;
    for (i = 0; texto[i]; i++) {
        for (j = 0; j < tamanho && texto[i + j]; j++) {
            if (tolower((unsigned char)texto[i + j]) !=
                tolower((unsigned char)termo[j])) break;
        }
        if (j == tamanho) return 1;
    }
    return 0;
}

static void exibirAcao(const Acao *acao)
{
    puts("---------------------------------------------");
    printf("Codigo: %d\nEscola: %s\nTurma: %s\nPeriodo: %s\n",
           acao->codigo, acao->escola, acao->turma, acao->periodo);
    printf("Tema: %s\nData prevista: %s\nPublico-alvo: %s\n",
           acao->tema, acao->dataPrevista, acao->publicoAlvo);
    printf("Professor/Responsavel: %s\nQuantidade prevista: %d\n",
           acao->responsavel, acao->qtdPrevista);
    if (acao->situacao == REALIZADA)
        printf("Quantidade efetiva: %d\n", acao->qtdEfetiva);
    else
        puts("Quantidade efetiva: ainda nao aplicavel");
    printf("Situacao: %s\n", nomeSituacao(acao->situacao));
}

static void exibirMenu(void)
{
    puts("\n=============================================");
    puts("               PSE EM ACAO");
    puts("=============================================");
    puts("1 - Cadastrar acao");
    puts("2 - Listar acoes");
    puts("3 - Pesquisar acoes");
    puts("4 - Atualizar situacao");
    puts("5 - Gerar resumo (por dia ou geral)");
    puts("6 - Sair");
    puts("=============================================");
}

/* ---------- Funcionalidades ---------- */
static void cadastrarAcao(void)
{
    Acao nova;
    int escolha;
    puts("\n--- CADASTRAR ACAO ---");
    if (totalAcoes >= MAX_ACOES) {
        printf("Erro: limite maximo de %d acoes atingido.\n", MAX_ACOES);
        return;
    }
    puts("Use somente informacoes ficticias e coletivas.");
    nova.codigo = lerInteiroMinimo("Codigo da acao: ", 1);
    if (buscarIndicePorCodigo(nova.codigo) != -1) {
        puts("Erro: codigo ja cadastrado. Informe outro codigo em novo cadastro.");
        return;
    }
    lerTextoObrigatorio("Escola: ", nova.escola, sizeof nova.escola);
    lerTextoObrigatorio("Turma: ", nova.turma, sizeof nova.turma);
    puts("Periodo: 1 - Manha | 2 - Tarde | 3 - Noite");
    escolha = lerOpcao("Escolha o periodo: ", 1, 3);
    strcpy(nova.periodo, escolha == 1 ? "Manha" : escolha == 2 ? "Tarde" : "Noite");
    lerTextoObrigatorio("Tema: ", nova.tema, sizeof nova.tema);
    lerData("Data prevista (DD/MM/AAAA): ", nova.dataPrevista);
    lerTextoObrigatorio("Publico-alvo: ", nova.publicoAlvo, sizeof nova.publicoAlvo);
    lerTextoObrigatorio("Professor/Responsavel: ", nova.responsavel,
                       sizeof nova.responsavel);
    nova.qtdPrevista = lerInteiroMinimo("Participantes previstos (minimo 1): ", 1);
    puts("Situacao inicial: 1 - Planejada | 2 - Realizada | 3 - Cancelada");
    nova.situacao = lerOpcao("Escolha a situacao: ", 1, 3);
    nova.qtdEfetiva = 0;
    if (nova.situacao == REALIZADA)
        nova.qtdEfetiva = lerInteiroMinimo("Participantes efetivos (minimo 0): ", 0);
    acoes[totalAcoes++] = nova;
    printf("Acao cadastrada com sucesso! Codigo %d. Situacao: %s.\n",
           nova.codigo, nomeSituacao(nova.situacao));
}

static void listarAcoes(void)
{
    int i;
    puts("\n--- ACOES CADASTRADAS ---");
    if (totalAcoes == 0) {
        puts("Nenhuma acao cadastrada.");
        return;
    }
    for (i = 0; i < totalAcoes; i++) exibirAcao(&acoes[i]);
    printf("---------------------------------------------\nTotal: %d acao(oes).\n", totalAcoes);
}

static void pesquisarAcoes(void)
{
    int filtro, codigo, i, encontrados = 0;
    char termo[TAM_BUFFER];
    const char *campo;
    puts("\n--- PESQUISAR ACOES ---");
    if (totalAcoes == 0) {
        puts("Nenhuma acao cadastrada para pesquisar.");
        return;
    }
    puts("1 - Codigo | 2 - Escola | 3 - Tema | 4 - Professor/Responsavel | 5 - Turma");
    filtro = lerOpcao("Escolha o filtro: ", 1, 5);
    if (filtro == 1) {
        codigo = lerInteiroMinimo("Codigo procurado: ", 1);
        i = buscarIndicePorCodigo(codigo);
        if (i == -1) puts("Nenhuma acao encontrada com esse codigo.");
        else exibirAcao(&acoes[i]);
        return;
    }
    lerTextoObrigatorio("Digite o termo da pesquisa: ", termo, sizeof termo);
    for (i = 0; i < totalAcoes; i++) {
        switch (filtro) {
            case 2: campo = acoes[i].escola; break;
            case 3: campo = acoes[i].tema; break;
            case 4: campo = acoes[i].responsavel; break;
            default: campo = acoes[i].turma; break;
        }
        if (contemTexto(campo, termo)) {
            exibirAcao(&acoes[i]);
            encontrados++;
        }
    }
    if (!encontrados) puts("Nenhuma acao encontrada para o termo pesquisado.");
    else printf("Total encontrado: %d acao(oes).\n", encontrados);
}

static void atualizarSituacao(void)
{
    int codigo, indice, situacao, efetiva = 0;
    puts("\n--- ATUALIZAR SITUACAO ---");
    if (totalAcoes == 0) {
        puts("Nenhuma acao cadastrada para atualizar.");
        return;
    }
    codigo = lerInteiroMinimo("Codigo da acao: ", 1);
    indice = buscarIndicePorCodigo(codigo);
    if (indice == -1) {
        puts("Erro: registro inexistente. Acao nao encontrada.");
        return;
    }
    exibirAcao(&acoes[indice]);
    puts("Nova situacao: 1 - Planejada | 2 - Realizada | 3 - Cancelada");
    situacao = lerOpcao("Escolha a situacao: ", 1, 3);
    if (situacao == REALIZADA)
        efetiva = lerInteiroMinimo("Participantes efetivos (minimo 0): ", 0);
    /* So altera o registro apos validar as entradas. */
    acoes[indice].situacao = situacao;
    acoes[indice].qtdEfetiva = efetiva;
    printf("Situacao atualizada com sucesso: %s.\n", nomeSituacao(situacao));
}

static void imprimirResumo(const char *titulo, const char *dataFiltro)
{
    int i, encontradas = 0, planejadas = 0, realizadas = 0, canceladas = 0;
    long long previstasTotal = 0, previstasRealizadas = 0, efetivas = 0;
    double percentual;
    for (i = 0; i < totalAcoes; i++) {
        const Acao *acao = &acoes[i];
        if (dataFiltro && strcmp(dataFiltro, acao->dataPrevista) != 0) continue;
        encontradas++;
        previstasTotal += acao->qtdPrevista;
        switch (acao->situacao) {
            case PLANEJADA: planejadas++; break;
            case REALIZADA:
                realizadas++;
                previstasRealizadas += acao->qtdPrevista;
                efetivas += acao->qtdEfetiva;
                break;
            case CANCELADA: canceladas++; break;
        }
    }
    printf("\n--- %s ---\n", titulo);
    if (dataFiltro) printf("Data consultada: %s\n", dataFiltro);
    if (encontradas == 0) {
        puts("Nenhuma acao encontrada para esta consulta.");
        return;
    }
    printf("Acoes cadastradas: %d\n", encontradas);
    printf("Planejadas: %d | Realizadas: %d | Canceladas: %d\n",
           planejadas, realizadas, canceladas);
    printf("Participantes previstos (todas as acoes): %lld\n", previstasTotal);
    printf("Participantes efetivos (acoes realizadas): %lld\n", efetivas);
    if (previstasRealizadas > 0) {
        percentual = (double)efetivas * 100.0 / (double)previstasRealizadas;
        printf("Participacao nas acoes realizadas: %.1f%%\n", percentual);
    } else {
        puts("Participacao nas acoes realizadas: nao calculavel (sem acoes realizadas).");
    }
}

static void gerarResumo(void)
{
    char consulta[TAM_BUFFER];
    puts("\n--- GERAR RESUMO ---");
    puts("Informe uma data DD/MM/AAAA para resumo do dia ou G para resumo geral.");
    printf("Data ou G: ");
    lerLinha(consulta, sizeof consulta);
    aparar(consulta);
    if ((consulta[0] == 'G' || consulta[0] == 'g') && consulta[1] == '\0') {
        imprimirResumo("RESUMO GERAL", NULL);
    } else if (!dataValida(consulta)) {
        puts("Erro: data invalida. Use DD/MM/AAAA ou digite G.");
    } else {
        imprimirResumo("RESUMO POR DIA", consulta);
    }
}

int main(void)
{
    int opcao, confirmar;
    while (1) {
        exibirMenu();
        if (!lerInteiro("Digite uma opcao: ", &opcao)) {
            puts("Erro: opcao invalida. Digite um numero de 1 a 6.");
            continue;
        }
        switch (opcao) {
            case 1: cadastrarAcao(); break;
            case 2: listarAcoes(); break;
            case 3: pesquisarAcoes(); break;
            case 4: atualizarSituacao(); break;
            case 5: gerarResumo(); break;
            case 6:
                puts("Deseja realmente sair? 1 - Sim | 2 - Nao");
                confirmar = lerOpcao("Escolha: ", 1, 2);
                if (confirmar == 1) {
                    puts("Sistema encerrado. Ate logo!");
                    return 0;
                }
                puts("Retornando ao menu principal...");
                break;
            default:
                puts("Erro: opcao invalida. Digite um numero de 1 a 6.");
        }
    }
}
