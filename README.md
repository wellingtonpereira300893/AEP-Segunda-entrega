# PSE em Ação | AEP 2ª entrega | Grupo Gaia

Sistema acadêmico em **linguagem C (C99)** para planejar, registrar e acompanhar ações coletivas do **Programa Saúde na Escola (PSE)**. Executa em terminal, armazena até **100 ações em memória** e não exige bibliotecas externas.

**Privacidade:** utilize somente dados fictícios e coletivos. O sistema não foi feito para registrar nomes de estudantes, condições clínicas, prontuários ou informações individuais. As entradas de texto são livres: o uso de dados fictícios depende do operador. As informações são perdidas ao encerrar o programa.

## Compilar e executar

Na pasta onde está `pse_em_acao.c`, com GCC instalado:

**Windows (MinGW/MSYS2):**
```bash
gcc -std=c99 -Wall -Wextra -pedantic -o pse_em_acao.exe pse_em_acao.c
.\pse_em_acao.exe
```
No Prompt de Comando do Windows, execute também com `pse_em_acao.exe`.

**Linux/macOS:**
```bash
gcc -std=c99 -Wall -Wextra -pedantic -o pse_em_acao pse_em_acao.c
./pse_em_acao
```

## Menu

| Opção | Operação |
|---|---|
| 1 | Cadastrar ação |
| 2 | Listar todas as ações |
| 3 | Pesquisar por código, escola, tema, professor/responsável ou turma |
| 4 | Atualizar situação e quantidade efetiva |
| 5 | Gerar resumo: digitar uma **data (DD/MM/AAAA)** para o dia ou **G** para o geral |
| 6 | Sair, com confirmação |

## Cadastro

Cada ação possui: **código** inteiro positivo e único; **escola**, **turma**, **período** (Manhã/Tarde/Noite), **tema**, **data prevista**, **público-alvo**, **professor/responsável**, **quantidade prevista**, **quantidade efetiva** (nas realizadas) e **situação** (Planejada, Realizada ou Cancelada).

O período e a situação são selecionados por números para evitar digitação inconsistente. Um cadastro inicialmente marcado como **Realizada** solicita a quantidade efetiva. A quantidade prevista deve ser **ao menos 1** e a efetiva pode ser **0 ou mais**. Quando uma ação é alterada para Planejada ou Cancelada, a quantidade efetiva é zerada para não misturar dados de uma situação anterior.

## Pesquisa e resumo

- A pesquisa por **código** é exata. As buscas por texto aceitam **parte do conteúdo** e ignoram diferenças de letras maiúsculas e minúsculas **ASCII**. A busca não faz normalização completa de acentos Unicode.
- O **resumo por dia** usa a **data prevista** para selecionar ações. Exibe planejadas, realizadas, canceladas, total previsto para todas as ações desse dia e total efetivo das realizadas.
- O **resumo geral** considera as ações cadastradas de todas as datas.
- O **percentual de participação** é calculado assim: `100 × participantes efetivos das ações realizadas / participantes previstos das ações realizadas`. Não inclui planejadas e canceladas no denominador. Sem realizadas, o percentual é exibido como não calculável.

## Exemplo de cadastro

```text
1 - Cadastrar acao
Codigo da acao: 101
Escola: Escola Municipal Aurora
Turma: 5A
Periodo: 1 - Manha | 2 - Tarde | 3 - Noite
Escolha o periodo: 1
Tema: Saude bucal
Data prevista (DD/MM/AAAA): 15/11/2026
Publico-alvo: 5o ano
Professor/Responsavel: Prof. Ana
Participantes previstos (minimo 1): 30
Situacao inicial: 1 - Planejada | 2 - Realizada | 3 - Cancelada
Escolha a situacao: 1
Acao cadastrada com sucesso! Codigo 101. Situacao: Planejada.
```

## Validações

- Impede código repetido, código nulo/negativo, opções e situações inexistentes.
- Não aceita campos obrigatórios vazios, mesmo que só contenham espaços.
- Verifica que a data exista no calendário (incluindo ano bissexto) e esteja entre 2000 e 2100.
- Rejeita quantidades fora dos limites e entrada que não seja inteiro.
- Informa quando um código não existe ou a pesquisa não encontra resultados.
- Impede ultrapassar `MAX_ACOES` (100 por padrão).
- Leitura com `fgets` e conversão com `strtol`, evitando problemas comuns de `scanf`.

## Testes

Na pasta do projeto:
```bash
python3 testes/testar_sistema.py
```
Esse script exige `gcc` e Python 3, compila o programa, executa exemplos reais e compara as saídas esperadas. Há **16 verificações de comportamento e uma compilação rigorosa aprovadas**, comportamento das operações, validações, limite com `MAX_ACOES=2` e capacidade padrão com 100 ações. Consulte `testes/resultado_testes.txt` e as transcrições.

As imagens em `imagens/` representam saídas de **execução real** do programa em pseudo-terminal, geradas pelo script `testes/gerar_capturas.py` (necessita `pexpect` e `Pillow` apenas para recriar as imagens, não para executar o aplicativo).

Para testar o limite de 2 ações manualmente:
```bash
gcc -DMAX_ACOES=2 -std=c99 -Wall -Wextra -o pse_limite pse_em_acao.c
```

## Organização da entrega

- `pse_em_acao.c` - código-fonte principal.
- `README.md` - este guia.
- `documentacao/Relatorio_Final_Grupo_Gaia_AEP2.docx` - documento editável para entregar.
- `documentacao/Relatorio_Final_Grupo_Gaia_AEP2.pdf` - versão para leitura/avaliação.
- `documentacao/roteiro_video.md` - divisão sugerida para vídeo de até 7 minutos.
- `diagramas/` - diagrama de casos de uso e fluxogramas atualizados, em PNG e DOT.
- `imagens/` - capturas de tela de execução.
- `testes/` - scripts de teste, transcrições reais e resultado.

## Relação com a 1ª etapa

O programa parte do código em C e dos fluxogramas enviados pelo Grupo Gaia. Foram completados: turma, período, seleção de situação inicial, pesquisas por professor/turma, resumo por data e opção 6 para sair. O **resumo geral já existente foi preservado** como alternativa `G` da opção 5. As diferenças e justificativas estão no relatório.

## Entrega acadêmica: ações externas necessárias

1. Subir os arquivos no **GitHub do grupo**, mantendo o `README.md` na raiz.
2. Gravar o **vídeo com os integrantes**, incluindo problema, demonstração, código, fluxogramas e resultados, com no máximo 7 minutos.
3. Incluir no documento final o **link real do GitHub** e o **link real do vídeo**, dando acesso à pessoa que fará a avaliação.
4. Conferir nomes dos integrantes, turma, disciplina e regras de formatação da instituição.

Não há link de GitHub ou vídeo neste pacote porque esses envios exigem acesso à conta e gravação do grupo.

### Histórico do projeto

O PDF original da primeira entrega foi preservado em `referencias/Primeira_Entrega_Original_Grupo_Gaia.pdf` **apenas para consulta**. O código final e a documentação revisada refletem as decisões da segunda etapa e prevalecem em caso de diferença com o pseudocódigo antigo.
