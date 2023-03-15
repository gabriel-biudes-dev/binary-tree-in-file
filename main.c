#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

//Escreve um cabeçalho no arquivo
//Recebe: O nome do arquivo e o cabeçalho
//Retorna: Nada
void writeHeader(char *filename, Header *h){
    FILE *f = fopen(filename, "rb+");
    fseek(f, 0, SEEK_SET);
    fwrite(h, sizeof(Header), 1, f);
    fclose(f);
}

//Lê o cabeçalho do arquivo
//Recebe: O nome do arquivo
//Retorna: O cabeçalho
Header* readHeader(char *filename){
    Header *h = (Header*) malloc(sizeof(Header));
    FILE *f = fopen(filename, "rb");
    fseek(f, 0, SEEK_SET); // posiciona no in ́ıcio do arquivo
    fread(h, sizeof(Header), 1, f);
    fclose(f);
    return h;
}

//Inicia o cabeçalho no arquivo
//Recebe: O nome do arquivo
//Retorna: Nada
void startHeader(char *filename){
    Header *h = (Header*) malloc(sizeof(Header));
    h->rootPos = -1;
    h->top = 0;
    h->freePos = -1;
    writeHeader(filename, h);
    free(h);
}

//Lê uma posição no arquivo
//Recebe: O nome do arquivo e a posição
//Retorna: O nó na posição
struct no* readPosition(char *filename, int pos){
    if(pos == -1) return NULL;
    struct no *aux = (struct no*) malloc(sizeof(struct no));
    FILE *f = fopen(filename, "rb");
    fseek(f, sizeof(Header) + (pos * sizeof(struct no)), SEEK_SET);
    fread(aux, sizeof(struct no), 1, f);
    fclose(f);
    if(aux->codigo <= 0) return NULL;
    return aux;
}

//Escreve um nó em certa posição
//Recebe: O nome do arquivo, o nó e a posição
//Retorna: Nada
void writePosition(char *filename, struct no* node, int pos){
    FILE *f = fopen(filename, "rb+");
    fseek(f, sizeof(Header) + (pos * sizeof(struct no)), SEEK_SET);
    fwrite(node, sizeof(struct no), 1, f);
    fclose(f);
}

//Calcula a primeira posição livre no arquivo
//Recebe: O nome do arquivo
//Retorna: Nada
int getFreePos(char *filename){
    int filesize, currentsize, currentpos, freepos;
    struct no *aux = (struct no*) malloc(sizeof(struct no));
    currentsize = sizeof(Header);
    currentpos = 0;
    freepos = -1;
    FILE *f = fopen(filename, "rb");
    fseek(f, 0, SEEK_END);
    filesize = ftell(f);
    while(currentsize < filesize){
        fseek(f, sizeof(Header) + (currentpos * sizeof(struct no)), SEEK_SET);
        fread(aux, sizeof(struct no), 1, f);
        if(aux->codigo == -2){
            freepos = currentpos;
            break;
        }
        currentpos++;
        currentsize = currentsize + sizeof(struct no);
    }
    fclose(f);
    free(aux);
    return freepos;
}

//Separa string em tokens
//Recebe: String
//Retorna: Token da string
char *strsep(char **stringp, const char *delim){
    char *rv = *stringp;
    if(rv){
        *stringp += strcspn(*stringp, delim);
        if (**stringp) *(*stringp)++ = '\0';
        else *stringp = 0; }
    return rv;
}

//Calcula o tamanho do arquivo
//Recebe: O nome do arquivo
//Retorna: O tamanho do arquivo
int getSize(char *filename){
    int filesize;
    FILE *f = fopen(filename, "rb");
    fseek(f, 0, SEEK_END);
    filesize = ftell(f);
    fclose(f);
    return filesize;
}

//Calcula o pai de um nó
//Recebe: O nome do arquivo, o código do nó e a posição da raíz
//Retorna: A posição do pai
int getParent(char *filename, int x, int startpos){
    int cod, right, left;
    struct no* aux = (struct no*) malloc(sizeof(struct no));
    aux = readPosition(filename, startpos);
    cod = aux->codigo;
    right = aux->right;
    left = aux->left;
    free(aux);
    aux = NULL;
    if(x < cod && left != -1) return getParent(filename, x, left);
    if(x >= cod && right != -1) return getParent(filename, x, right);
    return startpos;
}

//Procura pela posição de um código
//Recebe: O nome do arquivo, o código e a posição da raíz
//Retorna: A posição do nó
int searchCode(char *filename, int x, int startpos){
    if(startpos == -1) return -1;
    int cod, right, left;
    struct no* aux = (struct no*) malloc(sizeof(struct no));
    aux = readPosition(filename, startpos);
    cod = aux->codigo;
    right = aux->right;
    left = aux->left;
    free(aux);
    aux = NULL;
    if(startpos == -1) return -1;
    if(x < cod && left != -1) return searchCode(filename, x, left);
    if(x > cod && right != -1) return searchCode(filename, x, right);
    if(x == cod) return startpos;
    return -1;
}

//Insere um nó na árvore
//Recebe: O nome do arquivo e o nó a ser escrito
//Retorna: Nada
void insertNode(char *filename, struct no* node){
    int freepos, parentpos;
    Header *h = readHeader(filename);
    if(searchCode(filename, node->codigo, h->rootPos) != -1) return;
    struct no* parent;
    parentpos = -1;
    node->left = -1;
    node->right = -1;
    freepos = getFreePos(filename);
    if(h->rootPos == -1) h->rootPos = 0;
    else parentpos = getParent(filename, node->codigo, h->rootPos);
    if(freepos == -1){
        if(parentpos != -1){
            parent = readPosition(filename, parentpos);
            if(parent->left == -1 && node->codigo < parent->codigo) parent->left = h->top;
            if(parent->right == -1 && node->codigo >= parent->codigo) parent->right = h->top;
            writePosition(filename, parent, parentpos);
        }
        writePosition(filename, node, h->top);
        h->top = h->top + 1;
        writeHeader(filename, h);
    }else{
        writeHeader(filename, h);
        if(parentpos != -1){
            parent = readPosition(filename, parentpos);
            if(parent->left == -1 && node->codigo < parent->codigo) parent->left = freepos;
            if(parent->right == -1 && node->codigo >= parent->codigo) parent->right = freepos;
            writePosition(filename, parent, parentpos);
        }
        writePosition(filename, node, freepos);
    }
    free(h);
}

//Mostra a árvore in ordem
//Recebe: O nome do arquivo e a posição da raíz
//Retorna: Nada
void printOrder(char *filename, int pos){
    struct no *aux = readPosition(filename, pos);
    if(aux->left != -1) printOrder(filename, aux->left);
    printf("%d - %s\n", aux->codigo, aux->nome);
    if(aux->right != -1) printOrder(filename, aux->right);
    free(aux);
}

//Mostra a árvore
//Recebe: O nome do arquivo
//Retorna: Nada
void printtree(char *filename){
    int rootpos;
    Header *h = readHeader(filename);
    rootpos = h->rootPos;
    free(h);
    if(rootpos == -1) return;
    printOrder(filename, rootpos);
}

//Mostra o menu da aplicação
//Recebe: Nada
//Retorna: A opção escolhida
int showMenu(){
    int opt;
    printf("\n\t[MENU DO PROGRAMA]\n");
    printf("1)Cadastrar produto\n");
    printf("2)Imprimir informacoes de um produto\n");
    printf("3)Imprimir lista de todos os produtos\n");
    printf("4)Desenhar arvore binaria\n");
    printf("5)Carregar arquivo de lote\n");
    printf("6)Atualizar preco\n");
    printf("7)Atualizar estoque\n");
    printf("8)Remover produto\n");
    printf("9)Mostrar lista de posicoes livres\n");
    printf("\tEscolha a opcao: ");
    scanf("%d%*c", &opt);
    return opt;
}

//Executa operação insert do arquivo de lotes
//Recebe: String do insert
//Retorna: Nada
void insertoperation(char *filename, char *line){
    int value1, value2;
    float finalprice;
    char *straux = (char*) malloc(sizeof(line) + 1);
    struct no *aux = (struct no*) malloc(sizeof(struct no));
    line = strtok(line, ";");
    line = strtok(NULL, ";");
    aux->codigo = atoi(line);
    line = strtok(NULL, ";");
    strcpy(aux->nome, line);
    line = strtok(NULL, ";");
    strcpy(aux->marca, line);
    line = strtok(NULL, ";");
    strcpy(aux->categoria, line);
    line = strtok(NULL, ";");
    aux->estoque = atoi(line);
    line = strtok(NULL, ";");
    strcpy(straux, line);
    straux = strtok(straux, ",");
    value1 = atoi(straux);
    straux = strtok(NULL, ",");
    value2 = atoi(straux);
    straux = strtok(NULL, ",");
    finalprice = (float)value2 / 100;
    finalprice = finalprice + value1;
    aux->preco = finalprice;
    line = strtok(NULL, ";");
    insertNode(filename, aux);
    //free(straux);
    free(aux);
}

//Busca por um código a partir de certa posição
//Recebe: O nome do arquivo, o código e a posição
//Retorna: A posição do código
int searchposrec(char *filename, int x, int position){
    int size, currentsize, pos;
    pos = position;
    FILE *f = fopen(filename, "rb");
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    if(size < (int)sizeof(Header)){
        fclose(f);
        return -1;
    }
    fseek(f, sizeof(Header) + (pos * sizeof(struct no)), SEEK_SET);
    currentsize = ftell(f);
    struct no* aux = (struct no*) malloc(sizeof(struct no));
    while(currentsize < size){
        fread(aux, sizeof(struct no), 1, f);
        if(x == aux->codigo && x != 0) return pos;
        if(x >= aux->codigo) pos = aux->right;
        else pos = aux->left;
        fseek(f, sizeof(Header) + (pos * sizeof(struct no)), SEEK_SET);
        currentsize = ftell(f);
    }
    fclose(f);
    return -1;
}

//Operação de alterar do arquivo de lotes
//Recebe: O nome do arquivo e a string a ser processada
//Retorna: Nada
void alteroperation(char *filename, char *str){
    Header *h = readHeader(filename);
    int code, stock, p1, p2, pos, startpos, rpos;
    float price;
    struct no *r;
    char *aux = (char*) malloc(strlen(str) + 1);
    char *aux2 = (char*) malloc(strlen(str) + 1);
    char *temp;
    strcpy(aux, str);
    temp = strsep(&aux, ";");
    temp = strsep(&aux, ";");
    code = atoi(temp);
    pos = searchposrec(filename, code, 0);
    startpos = h->rootPos;
    free(h);
    if(pos == -1) return;
    temp = strsep(&aux, ";");
    if(strcmp(temp, "") == 0) stock = -1;
    else stock = atoi(temp);
    temp = strsep(&aux, ";");
    if(*temp == 13 || strlen(temp) == 0 || temp == NULL || *temp == '\n') price = -1;
    else{
        strcpy(aux2, temp);
        aux2 = strtok(aux2, ",");
        p1 = atoi(aux2);
        aux2 = strtok(NULL, ",");
        p2 = atoi(aux2);
        aux2 = strtok(NULL, ",");
        price = (float)p2 / 100;
        price = price + p1;
    }
    temp = strsep(&aux, ";");
    rpos = searchCode(filename, code, startpos);
    r = readPosition(filename, rpos);
    if(r == NULL) return;
    else{
        if(stock > -1) r->estoque = stock;
        if(price > -1) r->preco = price;
        writePosition(filename, r, rpos);
    }
    free(r);
    free(aux);
    free(temp);
    free(aux2);
    aux2 = NULL;
}

//Mostra informações sobre um nó
//Recebe: O nó
//Retorna: Nada
void shownode(struct no* n){
    printf("\nCodigo do produto: %d\n", n->codigo);
    printf("Nome do produto: %s\n", n->nome);
    printf("Marca do produto: %s\n", n->marca);
    printf("Categoria do produto: %s\n", n->categoria);
    printf("Estoque do produto: %d\n", n->estoque);
    printf("Preco do produto: %.2f\n", n->preco);
}

//Mostra um nó pelo código
//Recebe: O nome do arquivo
//Retorna: Nada
void looktree(char *filename){
    int cod, rpos;
    struct no *r;
    printf("Insira o codigo: ");
    scanf("%d%*c", &cod);
    rpos = searchCode(filename, cod, 0);
    r = readPosition(filename, rpos);
    if(r == NULL) printf("Codigo nao encontrado\n");
    else{
        shownode(r);
        free(r);
    }
}

//Atualiza o preço de um nó no arquivo
//Recebe: Nada
//Retorna: Nada
void updateprice(char *filename){
    int cod, rpos, startpos;
    float newprice;
    struct no *r;
    Header *h = readHeader(filename);
    startpos = h->rootPos;
    free(h);
    printf("Insira o codigo: ");
    scanf("%d%*c", &cod);
    rpos = searchCode(filename, cod, startpos);
    r = readPosition(filename, rpos);
    if(r == NULL) printf("Codigo nao encontrado\n");
    else{
        printf("\nInsira o novo preco: ");
        scanf("%f%*c", &newprice);
        r->preco = newprice;
        writePosition(filename, r, rpos);
        free(r);
    }
}

//Atualiza o estoque de um nó no arquivo
//Recebe: Nada
//Retorna: Nada
void updatestock(char *filename){
    int cod, startpos, newstock, rpos;
    struct no *r;
    Header *h = readHeader(filename);
    startpos = h->rootPos;
    free(h);
    printf("Insira o codigo: ");
    scanf("%d%*c", &cod);
    rpos = searchCode(filename, cod, startpos);
    r = readPosition(filename, rpos);
    if(r == NULL) printf("Codigo nao encontrado\n");
    else{
        printf("\nInsira o novo estoque: ");
        scanf("%d%*c", &newstock);
        r->estoque = newstock;
        writePosition(filename, r, rpos);
        free(r);
    }
}

//Mostra a lista de posições livres
//Recebe: Nada
//Retorna: Nada
void showfree(char *filename){
    int filesize, currentsize, currentpos;
    struct no *aux = (struct no*) malloc(sizeof(struct no));
    currentsize = sizeof(Header);
    currentpos = 0;
    FILE *f = fopen(filename, "rb");
    fseek(f, 0, SEEK_END);
    filesize = ftell(f);
    while(currentsize < filesize){
        fseek(f, sizeof(Header) + (currentpos * sizeof(struct no)), SEEK_SET);
        fread(aux, sizeof(struct no), 1, f);
        if(aux->codigo == -2) printf(" %d ", currentpos);
        currentpos++;
        currentsize = currentsize + sizeof(struct no);
    }
}

//Calcula a posição do menor valor a partir de certa posição
//Recebe: A posição de início
//Retorna: a posição do menor valor
int calcmin(int position, char *filename){
    int pos;
    struct no* aux;
    aux = readPosition(filename, position);
    pos = position;
    while(aux->left != -1){
        pos = aux->left;
        aux = readPosition(filename, aux->left);
    }
    free(aux);
    return pos;
}

//Calcula a posição do maior valor a partir de certa posição
//Recebe: A posição de início
//Retorna: a posição do maior valor
int calcmax(int position, char *filename){
    int pos;
    struct no* aux;
    aux = readPosition(filename, position);
    pos = position;
    while(aux->right != -1){
        pos = aux->right;
        aux = readPosition(filename, aux->right);
    }
    free(aux);
    return pos;
}

//Procura pelo pai de um nó em certa posição
//Recebe: O nome do arquivo e a posição do nó
//Retorna: A posição
int getParent2(char *filename, int y){
    int filesize, currentsize, currentpos, thepos;
    struct no *aux = (struct no*) malloc(sizeof(struct no));
    currentsize = sizeof(Header);
    currentpos = 0;
    thepos = -1;
    FILE *f = fopen(filename, "rb");
    fseek(f, 0, SEEK_END);
    filesize = ftell(f);
    while(currentsize < filesize){
        fseek(f, sizeof(Header) + (currentpos * sizeof(struct no)), SEEK_SET);
        fread(aux, sizeof(struct no), 1, f);
        if(aux->left == y || aux->right == y){
            thepos = currentpos;
            break;
        }
        currentpos++;
        currentsize = currentsize + sizeof(struct no);
    }
    fclose(f);
    free(aux);
    return thepos;
}

//Faz a remoção de um nó do arquivo
//Recebe: O código a ser removido, a posição da raíz e o nome do arquivo
//Retorna: Nada
void removedata(int cod, int position, char *filename){
    Header *h = readHeader(filename);
    int mypos = h->rootPos;
    free(h);
    int pos;
    pos = searchposrec(filename, cod, position);
    if(pos == -1) return;
    struct no* current = readPosition(filename, pos);
    struct no* left = readPosition(filename, current->left);
    struct no* right = readPosition(filename, current->right);
    if(left == NULL && right == NULL){
        if(pos == mypos){
            Header *h2 = readHeader(filename);
            h2->rootPos = -1;
            writeHeader(filename, h2);
            free(h2);
        }
        int pzp = getParent2(filename, pos);
        struct no *parent = readPosition(filename, pzp);
        if(parent != NULL){
            struct no *nleft = readPosition(filename, parent->left);
            struct no *nright = readPosition(filename, parent->right);
            if(nright != NULL && nright->codigo == current->codigo) parent->right = -1;
            else if(nleft != NULL) parent->left = -1;
            if(nleft != NULL || nright != NULL) writePosition(filename, parent, pzp);
            free(nleft);
            free(nright);
        }
        current->codigo = -2;
        writePosition(filename, current, pos);
        free(current);
        free(left);
        free(right);
        free(parent);
        
    }else if(left == NULL){
        int cod2, dirminpos = calcmin(current->right, filename);
        struct no* new = readPosition(filename, dirminpos);
        cod2 = new->codigo;
        new->left = current->left;
        new->right = current->right;
        writePosition(filename, new, pos);
        free(new);
        removedata(cod2, current->right, filename);
    }else{
        int cod2, esqmaxpos = calcmax(current->left, filename);
        struct no* new = readPosition(filename, esqmaxpos);
        cod2 = new->codigo;
        new->left = current->left;
        new->right = current->right;
        writePosition(filename, new, pos);
        free(new);
        removedata(cod2, current->left, filename);
    }
}

//Solicita o código do nó a ser excluido
//Recebe: O nome do arquivo
//Retorna: Nada
void removetree(char *filename){
    int cod;
    printf("\nInsira o codigo: ");
    scanf("%d%*c", &cod);
    removedata(cod, 0, filename);
}

//Operação remover do arquivo de lotes
//Recebe: O nome do arquivo e a string a ser processada
//Retorna: Nada
void removeoperation(char *filename, char *str){
    int cod;
    char *aux = (char*) malloc(strlen(str) + 1);
    strcpy(aux, str);
    aux = strtok(aux, ";");
    aux = strtok(NULL, ";");
    cod = atoi(aux);
    aux = strtok(NULL, ";");
    free(aux);
    removedata(cod, 0, filename);
}

//Solicita informações do nó a ser inserido
//Recebe: O nome do arquivo
//Retorna: Nada
void inserttree(char* filename){
    struct no *aux = (struct no*) malloc(sizeof(struct no));
    printf("Insira o codigo: ");
    scanf("%d%*c", &aux->codigo);
    printf("\nInsira o nome: ");
    scanf("%[^\n]%*c", aux->nome);
    printf("\nInsira a marca: ");
    scanf("%[^\n]%*c", aux->marca);
    printf("\nInsira a categoria: ");
    scanf("%[^\n]%*c", aux->categoria);
    printf("\nInsira o estoque: ");
    scanf("%d%*c", &aux->estoque);
    printf("\nInsira o preco: ");
    scanf("%f%*c", &aux->preco);
    insertNode(filename, aux);
    printf("\n");
    free(aux);
}

//Processa o arquivo de lotes
//Recebe: O nome do arquivo e a linha a ser processada
//Retorna: Nada
void avaliate(char* filename, char* line){
    if(*line == 'I' || *line == 'i') insertoperation(filename, line);
    if(*line == 'A' || *line == 'a') alteroperation(filename, line);
    if(*line == 'R' || *line == 'r') removeoperation(filename, line);
}

//Carrega o arquivo de lotes
//Recebe: O nome do arquivo
//Retorna: Nada
void loadfile(char *filename){
    char name[30];
    char line[300];
    printf("Insira o nome do arquivo: ");
    scanf("%[^\n]%*c", name);
    FILE *f = fopen(name, "r");
    while(fgets(line, 299, f)){
        avaliate(filename, line);
    }
}

//Calcula x elevado a n
//Recebe: x e n
//Retorna: x elevado a n
int power(int x, int n){
    if (n == 0) return 1;
    if (x == 0) return 0;
    return x * power(x, n - 1);
}

//Mostra todos os nós de um nível
//Recebe: O nome do arquivo, a posição da raíz, o nó a ser procurado, 0 e a metade da altura elevada ao quadrado
//Retorna: Nada
void printleveln(char *filename, int pos, int l, int currentlevel, int space){
    int i;
    struct no *aux = readPosition(filename, pos);
    if(aux == NULL){
        printf("   ");
        return;
    }
    printleveln(filename, aux->left, l, currentlevel + 1, space / 2);
    if(currentlevel == l){
        for(i = 0; i < space - 1; i++) printf(" ");
        printf("%03d", aux->codigo);
        for(i = 0; i < space; i++) printf(" ");
    }
    printleveln(filename, aux->right, l, currentlevel + 1, space / 2);
    free(aux);
}

//Calcula a altura da árvore
//Recebe: O nome do arquivo e a posição da raíz
//Retorna: A altura da árvore
int getheight(char *filename, int rootpos){
    struct no *aux = readPosition(filename, rootpos);
    if(aux == NULL){
        free(aux);
        return 0;
    }
    if(getheight(filename, aux->left) > getheight(filename, aux->right)) return 1 + getheight(filename, aux->left);
    else return 1 + getheight(filename, aux->right);
}

//Função que chama a função recursiva de mostrar por nível
//Recebe: O nome do arquivo, o nível e a altura da árvore
//Retorna: Nada
void printlevel(char *filename, int x, int height){
    int rootpos;
    Header *h = readHeader(filename);
    rootpos = h->rootPos;
    free(h);
    if(rootpos == -1) return;
    printleveln(filename, rootpos, x, 0, power(2, height) / 2);
}

//Desenha a árvore
//Recebe: O nome do arquivo
//Retorna: Nada
void drawtree(char *filename){
    int startpos, value, i;
    Header *h = readHeader(filename);
    startpos = h->rootPos;
    free(h);
    value = getheight(filename, startpos);
    for(i = 0; i < value; i++){
        printlevel(filename, i, value);
        printf("\n");
    }
}

//Função principal
//Recebe: Nada
//Retorna: 0
int main(){
    int opt;
    char *filename = "arq.bin";
    if(getSize(filename) < (int)sizeof(Header)) startHeader(filename);
    opt = 1;
    while(opt){
        opt = showMenu();
        if(opt == 1) inserttree(filename);
        if(opt == 2) looktree(filename);
        if(opt == 3) printtree(filename);
        if(opt == 4) drawtree(filename);
        if(opt == 5) loadfile(filename);
        if(opt == 6) updateprice(filename);
        if(opt == 7) updatestock(filename);
        if(opt == 8) removetree(filename);
        if(opt == 9) showfree(filename);
    }
    return 0;
}
