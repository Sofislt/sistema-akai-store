#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cJSON.h"

#define ARQUIVO_FUNCIONARIOS "funcionarios.json"
#define ARQUIVO_PRODUTOS     "produtos.json"

typedef struct {
    char id[15];
    char nome[60];
    char cpf[20];
    char cargo[40];
    double salario;
    int ativo;
} Funcionario;

// Função auxiliar para ler o arquivo JSON do disco
char* ler_arquivo(const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (!arquivo) return NULL;

    fseek(arquivo, 0, SEEK_END);
    long tamanho = ftell(arquivo);
    rewind(arquivo);

    char *conteudo = (char *)malloc(tamanho + 1);
    if (conteudo) {
        fread(conteudo, 1, tamanho, arquivo);
        conteudo[tamanho] = '\0';
    }
    fclose(arquivo);
    return conteudo;
}

// Lê um arquivo JSON e garante que ele retorne um Array válido
cJSON* carregar_json_array(const char *nome_arquivo) {
    char *conteudo = ler_arquivo(nome_arquivo);
    cJSON *json = NULL;

    if (conteudo) {
        json = cJSON_Parse(conteudo);
        free(conteudo);
    }

    if (!json || !cJSON_IsArray(json)) {
        if (json) cJSON_Delete(json);
        json = cJSON_CreateArray();
    }
    return json;
}

// Grava qualquer estrutura cJSON de volta no disco
int salvar_json(const char *nome_arquivo, cJSON *json) {
    char *texto_json = cJSON_Print(json);
    if (!texto_json) return 0;

    FILE *arquivo = fopen(nome_arquivo, "w");
    if (!arquivo) {
        free(texto_json);
        return 0;
    }

    fputs(texto_json, arquivo);
    fclose(arquivo);
    free(texto_json);
    return 1;
}

// Descarta caracteres pendentes no buffer do teclado até o '\n'
void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Pausa a tela antes de chamar system("clear") novamente
void pausar() {
    printf("\nPressione ENTER para continuar...");
    getchar();
}

// Lê strings com espaços e garante que o buffer fique limpo
void ler_texto(char *destino, int tamanho) {
    if (fgets(destino, tamanho, stdin) != NULL) {
        size_t len = strlen(destino);
        if (len > 0 && destino[len - 1] == '\n') {
            destino[len - 1] = '\0';
        } else {
            limpar_buffer();
        }
    }
}

// Alinha colunas no terminal ignorando bytes extras de acentos UTF-8
void imprimir_coluna(const char *texto, int largura) {
    int letras = 0;
    for (int i = 0; texto[i] != '\0'; i++) {
        if (((unsigned char)texto[i] & 0xC0) != 0x80) {
            letras++;
        }
    }

    printf("%s", texto);
    for (int i = letras; i < largura; i++) {
        putchar(' ');
    }
}

void consultarFornecedores() {
    system("clear");
    char fornecedores[5][2][50] = {
        {"Malharia Streetwear Brasil",    "(11) 99123-4567"},
        {"Importadora Geek & Anime SP",   "(11) 98234-5678"},
        {"Estamparia Silk & DTF Ltda",    "(21) 97345-6789"},
        {"Acessorios Urbanos Atacado",    "(41) 96456-7890"},
        {"Embalagens E-commerce Express", "(19) 95567-8901"}
    };

    printf("=== FORNECEDORES DA AKAI STORE ===\n\n");

    for (int i = 0; i < 5; i++) {
        printf("Fornecedor: %-30s | Contato: %s\n", fornecedores[i][0], fornecedores[i][1]);
    }

    pausar();
}

void cadastrarFuncionario() {
    system("clear");
    char id[15], nome[60], cpf[20], senha[20], cargo[40];
    double salario;
    int ativo_opcao;

    printf("=== CADASTRAR NOVO FUNCIONÁRIO ===\n\n");
    printf("ID: ");
    ler_texto(id, sizeof(id));

    printf("Nome completo: ");
    ler_texto(nome, sizeof(nome));

    printf("CPF: ");
    ler_texto(cpf, sizeof(cpf));

    printf("Senha: ");
    ler_texto(senha, sizeof(senha));

    printf("Cargo: ");
    ler_texto(cargo, sizeof(cargo));

    printf("Salário base: ");
    scanf("%lf", &salario);
    limpar_buffer();

    printf("Ativo? (1 - Sim / 0 - Não): ");
    scanf("%d", &ativo_opcao);
    limpar_buffer();

    cJSON *json = carregar_json_array(ARQUIVO_FUNCIONARIOS);

    cJSON *novo_func = cJSON_CreateObject();
    cJSON_AddStringToObject(novo_func, "id", id);
    cJSON_AddStringToObject(novo_func, "nome_completo", nome);
    cJSON_AddStringToObject(novo_func, "cpf", cpf);
    cJSON_AddStringToObject(novo_func, "senha", senha);
    cJSON_AddStringToObject(novo_func, "cargo", cargo);
    cJSON_AddNumberToObject(novo_func, "salario_base", salario);
    cJSON_AddBoolToObject(novo_func, "ativo", ativo_opcao == 1);
    cJSON_AddItemToArray(json, novo_func);

    if (salvar_json(ARQUIVO_FUNCIONARIOS, json)) {
        printf("\nFuncionário cadastrado com sucesso!\n");
    } else {
        printf("\nErro ao salvar em '%s'!\n", ARQUIVO_FUNCIONARIOS);
    }

    cJSON_Delete(json);
    pausar();
}

void criarProduto() {
    system("clear");
    char id[15], nome[80], descricao[250], categoria[40], tamanho[20], fornecedor[60];
    double preco_fornecedor, preco_loja;
    int estoque, ativo_opcao;

    printf("=== CADASTRAR NOVO PRODUTO ===\n\n");
    printf("ID (ex: PROD011): ");
    ler_texto(id, sizeof(id));

    printf("Nome do produto: ");
    ler_texto(nome, sizeof(nome));

    printf("Descrição: ");
    ler_texto(descricao, sizeof(descricao));

    printf("Categoria: ");
    ler_texto(categoria, sizeof(categoria));

    printf("Tamanho (ex: P, M, G, Único): ");
    ler_texto(tamanho, sizeof(tamanho));

    printf("Fornecedor: ");
    ler_texto(fornecedor, sizeof(fornecedor));

    printf("Preço do fornecedor: ");
    scanf("%lf", &preco_fornecedor);
    limpar_buffer();

    printf("Preço da loja: ");
    scanf("%lf", &preco_loja);
    limpar_buffer();

    printf("Quantidade em estoque: ");
    scanf("%d", &estoque);
    limpar_buffer();

    printf("Ativo? (1 - Sim / 0 - Não): ");
    scanf("%d", &ativo_opcao);
    limpar_buffer();

    cJSON *json = carregar_json_array(ARQUIVO_PRODUTOS);

    cJSON *novo_prod = cJSON_CreateObject();
    cJSON_AddStringToObject(novo_prod, "id", id);
    cJSON_AddStringToObject(novo_prod, "nome", nome);
    cJSON_AddStringToObject(novo_prod, "descricao", descricao);
    cJSON_AddStringToObject(novo_prod, "categoria", categoria);
    cJSON_AddStringToObject(novo_prod, "tamanho", tamanho);
    cJSON_AddStringToObject(novo_prod, "fornecedor", fornecedor);
    cJSON_AddNumberToObject(novo_prod, "preco_fornecedor", preco_fornecedor);
    cJSON_AddNumberToObject(novo_prod, "preco_loja", preco_loja);
    cJSON_AddNumberToObject(novo_prod, "quantidade_estoque", estoque);
    cJSON_AddBoolToObject(novo_prod, "ativo", ativo_opcao == 1);

    cJSON_AddItemToArray(json, novo_prod);

    if (salvar_json(ARQUIVO_PRODUTOS, json)) {
        printf("\nProduto cadastrado com sucesso!\n");
    } else {
        printf("\nErro ao salvar em '%s'!\n", ARQUIVO_PRODUTOS);
    }

    cJSON_Delete(json);
    pausar();
}

void alterarProduto() {
    system("clear");
    char id_busca[15];

    printf("=== ALTERAR PRODUTO ===\n\n");
    printf("Digite o ID do produto (ex: PROD001): ");
    ler_texto(id_busca, sizeof(id_busca));

    cJSON *json = carregar_json_array(ARQUIVO_PRODUTOS);
    cJSON *item = NULL;
    cJSON *encontrado = NULL;

    cJSON_ArrayForEach(item, json) {
        cJSON *id = cJSON_GetObjectItemCaseSensitive(item, "id");
        if (cJSON_IsString(id) && strcmp(id->valuestring, id_busca) == 0) {
            encontrado = item;
            break;
        }
    }

    if (!encontrado) {
        printf("\nProduto com ID '%s' não encontrado.\n", id_busca);
        cJSON_Delete(json);
        pausar();
        return;
    }

    int campo;
    cJSON *nome_atual = cJSON_GetObjectItemCaseSensitive(encontrado, "nome");
    printf("\nProduto selecionado: %s\n", nome_atual ? nome_atual->valuestring : id_busca);
    printf("Qual campo deseja alterar?\n");
    printf("[1] Nome\n[2] Descrição\n[3] Categoria\n[4] Tamanho\n[5] Fornecedor\n");
    printf("[6] Preço do fornecedor\n[7] Preço da loja\n[8] Quantidade em estoque\n[9] Status (Ativo/Inativo)\n");
    printf("Opção: ");

    if (scanf("%d", &campo) != 1) {
        printf("\nEntrada inválida!\n");
        limpar_buffer();
        cJSON_Delete(json);
        pausar();
        return;
    }
    limpar_buffer();

    char novo_texto[250];
    double novo_valor;
    int novo_inteiro;

    switch (campo) {
        case 1:
            printf("Novo nome: ");
            ler_texto(novo_texto, sizeof(novo_texto));
            cJSON_ReplaceItemInObjectCaseSensitive(encontrado, "nome", cJSON_CreateString(novo_texto));
            break;
        case 2:
            printf("Nova descrição: ");
            ler_texto(novo_texto, sizeof(novo_texto));
            cJSON_ReplaceItemInObjectCaseSensitive(encontrado, "descricao", cJSON_CreateString(novo_texto));
            break;
        case 3:
            printf("Nova categoria: ");
            ler_texto(novo_texto, sizeof(novo_texto));
            cJSON_ReplaceItemInObjectCaseSensitive(encontrado, "categoria", cJSON_CreateString(novo_texto));
            break;
        case 4:
            printf("Novo tamanho: ");
            ler_texto(novo_texto, sizeof(novo_texto));
            cJSON_ReplaceItemInObjectCaseSensitive(encontrado, "tamanho", cJSON_CreateString(novo_texto));
            break;
        case 5:
            printf("Novo fornecedor: ");
            ler_texto(novo_texto, sizeof(novo_texto));
            cJSON_ReplaceItemInObjectCaseSensitive(encontrado, "fornecedor", cJSON_CreateString(novo_texto));
            break;
        case 6:
            printf("Novo preço do fornecedor: ");
            scanf("%lf", &novo_valor);
            limpar_buffer();
            cJSON_ReplaceItemInObjectCaseSensitive(encontrado, "preco_fornecedor", cJSON_CreateNumber(novo_valor));
            break;
        case 7:
            printf("Novo preço da loja: ");
            scanf("%lf", &novo_valor);
            limpar_buffer();
            cJSON_ReplaceItemInObjectCaseSensitive(encontrado, "preco_loja", cJSON_CreateNumber(novo_valor));
            break;
        case 8:
            printf("Nova quantidade em estoque: ");
            scanf("%d", &novo_inteiro);
            limpar_buffer();
            cJSON_ReplaceItemInObjectCaseSensitive(encontrado, "quantidade_estoque", cJSON_CreateNumber(novo_inteiro));
            break;
        case 9:
            printf("Ativo? (1 - Sim / 0 - Não): ");
            scanf("%d", &novo_inteiro);
            limpar_buffer();
            cJSON_ReplaceItemInObjectCaseSensitive(encontrado, "ativo", cJSON_CreateBool(novo_inteiro == 1));
            break;
        default:
            printf("\nOpção de campo inválida!\n");
            cJSON_Delete(json);
            pausar();
            return;
    }

    if (salvar_json(ARQUIVO_PRODUTOS, json)) {
        printf("\nProduto atualizado com sucesso!\n");
    } else {
        printf("\nErro ao salvar alterações!\n");
    }

    cJSON_Delete(json);
    pausar();
}

void excluirProduto() {
    system("clear");
    char id_busca[15];
    char confirmacao;

    printf("=== EXCLUIR PRODUTO ===\n\n");
    printf("Digite o ID do produto que deseja excluir (ex: PROD001): ");
    ler_texto(id_busca, sizeof(id_busca));

    cJSON *json = carregar_json_array(ARQUIVO_PRODUTOS);
    int total = cJSON_GetArraySize(json);
    int indice_encontrado = -1;

    for (int i = 0; i < total; i++) {
        cJSON *item = cJSON_GetArrayItem(json, i);
        cJSON *id = cJSON_GetObjectItemCaseSensitive(item, "id");

        if (cJSON_IsString(id) && strcmp(id->valuestring, id_busca) == 0) {
            indice_encontrado = i;
            cJSON *nome = cJSON_GetObjectItemCaseSensitive(item, "nome");
            printf("\nProduto encontrado: %s - %s\n", id->valuestring, nome ? nome->valuestring : "");
            break;
        }
    }

    if (indice_encontrado == -1) {
        printf("\nProduto com ID '%s' não encontrado.\n", id_busca);
        cJSON_Delete(json);
        pausar();
        return;
    }

    printf("Tem certeza que deseja excluir este produto? (s/n): ");
    scanf(" %c", &confirmacao);
    limpar_buffer();

    if (confirmacao == 's' || confirmacao == 'S') {
        cJSON_DeleteItemFromArray(json, indice_encontrado);
        if (salvar_json(ARQUIVO_PRODUTOS, json)) {
            printf("\nProduto excluído com sucesso!\n");
        } else {
            printf("\nErro ao salvar alterações no arquivo!\n");
        }
    } else {
        printf("\nExclusão cancelada.\n");
    }

    cJSON_Delete(json);
    pausar();
}

void modificarCatalogo() {
    int opcao = 0;

    do {
        system("clear");
        printf("=== MODIFICAR CATÁLOGO ===\n");
        printf("\n[1] Criar produto; \n[2] Alterar produto; \n[3] Excluir produto; \n[4] Voltar. \nOpção: ");

        if (scanf("%d", &opcao) != 1) {
            printf("\nEntrada inválida! Digite apenas números.\n");
            limpar_buffer();
            opcao = 0;
            pausar();
            continue;
        }
        limpar_buffer();

        switch (opcao) {
            case 1:
                criarProduto();
                break;
            case 2:
                alterarProduto();
                break;
            case 3:
                excluirProduto();
                break;
            case 4:
                break;
            default:
                printf("\nOpção inválida!\n");
                pausar();
        }
    } while (opcao != 4);
}

void consultarCatalogo() {
    system("clear");
    char *conteudo_json = ler_arquivo(ARQUIVO_PRODUTOS);
    if (!conteudo_json) {
        printf("Erro: Arquivo '%s' não encontrado.\n", ARQUIVO_PRODUTOS);
        pausar();
        return;
    }

    cJSON *json = cJSON_Parse(conteudo_json);
    free(conteudo_json);

    if (!json || !cJSON_IsArray(json)) {
        printf("Erro ao interpretar o catálogo ou formato inválido.\n");
        if (json) cJSON_Delete(json);
        pausar();
        return;
    }

    printf("============================== CATÁLOGO - AKAI STORE ==============================\n");
    imprimir_coluna("ID", 10);        printf(" | ");
    imprimir_coluna("PRODUTO", 35);   printf(" | ");
    imprimir_coluna("CATEGORIA", 16); printf(" | ");
    printf("PREÇO LOJA\n");
    printf("-----------------------------------------------------------------------------------\n");

    cJSON *item = NULL;
    cJSON_ArrayForEach(item, json) {
        cJSON *id        = cJSON_GetObjectItemCaseSensitive(item, "id");
        cJSON *nome      = cJSON_GetObjectItemCaseSensitive(item, "nome");
        cJSON *categoria = cJSON_GetObjectItemCaseSensitive(item, "categoria");
        cJSON *preco     = cJSON_GetObjectItemCaseSensitive(item, "preco_loja");

        if (cJSON_IsString(id) && cJSON_IsString(nome) &&
            cJSON_IsString(categoria) && cJSON_IsNumber(preco)) {
            
            imprimir_coluna(id->valuestring, 10);        printf(" | ");
            imprimir_coluna(nome->valuestring, 35);      printf(" | ");
            imprimir_coluna(categoria->valuestring, 16); printf(" | ");
            printf("R$ %7.2f\n", preco->valuedouble);
        }
    }

    printf("===================================================================================\n");

    cJSON_Delete(json);
    pausar();
}

void paginaGerente(Funcionario f) {
    int opcao = 0;

    do {
        system("clear");
        printf("=== PÁGINA DO GERENTE ===");
        printf("\nGerente logado(a): %s | ID: %s\n", f.nome, f.id);
        printf("\n[1] Gerar relatório; \n[2] Consultar fornecedores; \n[3] Cadastrar funcionário; \n[4] Modificar catálogo; \n[5] Consultar catálogo; \n[6] Sair; \nOpção: ");
        
        if (scanf("%d", &opcao) != 1) {
            printf("\nEntrada inválida! Digite apenas números.\n");
            limpar_buffer();
            opcao = 0;
            pausar();
            continue;
        }
        limpar_buffer();

        switch (opcao) {
            case 1:
                system("clear");
                printf("=== RELATÓRIO ===\n\n");
                printf("Gerar relatório (ainda a ser implementado).\n");
                pausar();
                break;
            case 2:
                consultarFornecedores();
                break;
            case 3:
                cadastrarFuncionario();
                break;
            case 4:
                modificarCatalogo();
                break;
            case 5:
                consultarCatalogo();
                break;
            case 6:
                system("clear");
                printf("Sessão encerrada. Até logo!\n");
                break;
            default:
                printf("\nOpção inválida!\n");
                pausar();
        }
    } while (opcao != 6);
}

void paginaCatalogo(Funcionario f) {
    int opcao = 0;

    do {
        system("clear");
        printf("=== PÁGINA DO CATÁLOGO ===");
        printf("\nOperador(a) logado(a): %s (%s)\n", f.nome, f.cargo);
        printf("\n[1] Modificar catálogo; \n[2] Consultar catálogo; \n[3] Sair. \nOpção: ");

        if (scanf("%d", &opcao) != 1) {
            printf("\nEntrada inválida! Digite apenas números.\n");
            limpar_buffer();
            opcao = 0;
            pausar();
            continue;
        }
        limpar_buffer();

        switch (opcao) {
            case 1:
                modificarCatalogo();
                break;
            case 2:
                consultarCatalogo();
                break;
            case 3:
                system("clear");
                printf("Sessão encerrada. Até logo!\n");
                break;
            default:
                printf("\nOpção inválida!\n");
                pausar();
        }
    } while (opcao != 3);
}

int main() {
    system("clear");
    char cpf_digitado[20];
    char senha_digitada[20];

    printf("=== LOGIN - AKAI STORE ===\n");
    printf("CPF: ");
    scanf("%19s", cpf_digitado);

    printf("Senha: ");
    scanf("%19s", senha_digitada);
    limpar_buffer();

    char *conteudo_json = ler_arquivo(ARQUIVO_FUNCIONARIOS);
    if (!conteudo_json) {
        printf("\nErro: Arquivo '%s' não encontrado.\n", ARQUIVO_FUNCIONARIOS);
        return 1;
    }

    cJSON *json = cJSON_Parse(conteudo_json);
    if (!json || !cJSON_IsArray(json)) {
        printf("\nErro ao interpretar o JSON ou formato inválido.\n");
        if (json) cJSON_Delete(json);
        free(conteudo_json);
        return 1;
    }

    cJSON *func = NULL;
    int login_sucesso = 0;
    Funcionario usuario_logado;

    cJSON_ArrayForEach(func, json) {
        cJSON *id      = cJSON_GetObjectItemCaseSensitive(func, "id");
        cJSON *cpf     = cJSON_GetObjectItemCaseSensitive(func, "cpf");
        cJSON *senha   = cJSON_GetObjectItemCaseSensitive(func, "senha");
        cJSON *nome    = cJSON_GetObjectItemCaseSensitive(func, "nome_completo");
        cJSON *cargo   = cJSON_GetObjectItemCaseSensitive(func, "cargo");
        cJSON *salario = cJSON_GetObjectItemCaseSensitive(func, "salario_base");
        cJSON *ativo   = cJSON_GetObjectItemCaseSensitive(func, "ativo");

        if (cJSON_IsString(cpf) && cJSON_IsString(senha)) {
            if (strcmp(cpf->valuestring, cpf_digitado) == 0 &&
                strcmp(senha->valuestring, senha_digitada) == 0) {
                
                if (cJSON_IsTrue(ativo)) {
                    strcpy(usuario_logado.id, id->valuestring);
                    strcpy(usuario_logado.nome, nome->valuestring);
                    strcpy(usuario_logado.cpf, cpf->valuestring);
                    strcpy(usuario_logado.cargo, cargo->valuestring);
                    usuario_logado.salario = salario->valuedouble;
                    usuario_logado.ativo = 1;

                    printf("\nLogin realizado com sucesso!\n");
                    printf("Bem-vindo(a), %s (%s)\n", usuario_logado.nome, usuario_logado.cargo);
                    
                    if (strcmp(usuario_logado.cargo, "Gerente de Loja") == 0) {
                        printf("Acesso total liberado (Nível Gerencial).\n");
                        fflush(stdout); 
                        sleep(2); 
                        paginaGerente(usuario_logado);
                    } else {
                        printf("Acesso padrão liberado (Nível Operacional).\n");
                        fflush(stdout); 
                        sleep(2); 
                        paginaCatalogo(usuario_logado);
                    }
                    login_sucesso = 1;
                } else {
                    printf("\nAcesso negado: funcionário inativo.\n");
                    login_sucesso = -1;
                }
                break;
            }
        }
    }

    if (login_sucesso == 0) {
        printf("\nCPF ou senha inválidos.\n");
    }

    cJSON_Delete(json);
    free(conteudo_json);

    return 0;
}