struct no{
    int codigo;
    char nome[50];
    char marca[30];
    char categoria[50];
    int estoque;
    float preco;
    int left;
    int right;
};

typedef struct{
    int rootPos;
    int top;
    int freePos;
}Header;

//Escreve um cabeçalho no arquivo
//Recebe: O nome do arquivo e o cabeçalho
//Retorna: Nada
void writeHeader(char *filename, Header *h);

//Lê o cabeçalho do arquivo
//Recebe: O nome do arquivo
//Retorna: O cabeçalho
Header* readHeader(char *filename);

//Inicia o cabeçalho no arquivo
//Recebe: O nome do arquivo
//Retorna: Nada
void startHeader(char *filename);

//Lê uma posição no arquivo
//Recebe: O nome do arquivo e a posição
//Retorna: O nó na posição
struct no* readPosition(char *filename, int pos);

//Escreve um nó em certa posição
//Recebe: O nome do arquivo, o nó e a posição
//Retorna: Nada
void writePosition(char *filename, struct no* node, int pos);

//Calcula a primeira posição livre no arquivo
//Recebe: O nome do arquivo
//Retorna: Nada
int getFreePos(char *filename);

//Separa string em tokens
//Recebe: String
//Retorna: Token da string
char *strsep(char **stringp, const char *delim);

//Calcula o tamanho do arquivo
//Recebe: O nome do arquivo
//Retorna: O tamanho do arquivo
int getSize(char *filename);

//Calcula o pai de um nó
//Recebe: O nome do arquivo, o código do nó e a posição da raíz
//Retorna: A posição do pai
int getParent(char *filename, int x, int startpos);

//Procura pela posição de um código
//Recebe: O nome do arquivo, o código e a posição da raíz
//Retorna: A posição do nó
int searchCode(char *filename, int x, int startpos);

//Insere um nó na árvore
//Recebe: O nome do arquivo e o nó a ser escrito
//Retorna: Nada
void insertNode(char *filename, struct no* node);

//Mostra a árvore in ordem
//Recebe: O nome do arquivo e a posição da raíz
//Retorna: Nada
void printOrder(char *filename, int pos);

//Mostra a árvore
//Recebe: O nome do arquivo
//Retorna: Nada
void printtree(char *filename);

//Mostra o menu da aplicação
//Recebe: Nada
//Retorna: A opção escolhida
int showMenu();

//Executa operação insert do arquivo de lotes
//Recebe: String do insert
//Retorna: Nada
void insertoperation(char *filename, char *line);

//Busca por um código a partir de certa posição
//Recebe: O nome do arquivo, o código e a posição
//Retorna: A posição do código
int searchposrec(char *filename, int x, int position);

//Operação de alterar do arquivo de lotes
//Recebe: O nome do arquivo e a string a ser processada
//Retorna: Nada
void alteroperation(char *filename, char *str);

//Mostra informações sobre um nó
//Recebe: O nó
//Retorna: Nada
void shownode(struct no* n);

//Mostra um nó pelo código
//Recebe: O nome do arquivo
//Retorna: Nada
void looktree(char *filename);

//Atualiza o preço de um nó no arquivo
//Recebe: Nada
//Retorna: Nada
void updateprice(char *filename);

//Atualiza o estoque de um nó no arquivo
//Recebe: Nada
//Retorna: Nada
void updatestock(char *filename);

//Mostra a lista de posições livres
//Recebe: Nada
//Retorna: Nada
void showfree(char *filename);

//Calcula a posição do menor valor a partir de certa posição
//Recebe: A posição de início
//Retorna: a posição do menor valor
int calcmin(int position, char *filename);

//Calcula a posição do maior valor a partir de certa posição
//Recebe: A posição de início
//Retorna: a posição do maior valor
int calcmax(int position, char *filename);

//Procura pelo pai de um nó em certa posição
//Recebe: O nome do arquivo e a posição do nó
//Retorna: A posição
int getParent2(char *filename, int y);

//Faz a remoção de um nó do arquivo
//Recebe: O código a ser removido, a posição da raíz e o nome do arquivo
//Retorna: Nada
void removedata(int cod, int position, char *filename);

//Solicita o código do nó a ser excluido
//Recebe: O nome do arquivo
//Retorna: Nada
void removetree(char *filename);

//Operação remover do arquivo de lotes
//Recebe: O nome do arquivo e a string a ser processada
//Retorna: Nada
void removeoperation(char *filename, char *str);

//Solicita informações do nó a ser inserido
//Recebe: O nome do arquivo
//Retorna: Nada
void inserttree(char* filename);

//Processa o arquivo de lotes
//Recebe: O nome do arquivo e a linha a ser processada
//Retorna: Nada
void avaliate(char* filename, char* line);

//Carrega o arquivo de lotes
//Recebe: O nome do arquivo
//Retorna: Nada
void loadfile(char *filename);

//Calcula x elevado a n
//Recebe: x e n
//Retorna: x elevado a n
int power(int x, int n);

//Mostra todos os nós de um nível
//Recebe: O nome do arquivo, a posição da raíz, o nó a ser procurado, 0 e a metade da altura elevada ao quadrado
//Retorna: Nada
void printleveln(char *filename, int pos, int l, int currentlevel, int space);

//Calcula a altura da árvore
//Recebe: O nome do arquivo e a posição da raíz
//Retorna: A altura da árvore
int getheight(char *filename, int rootpos);

//Função que chama a função recursiva de mostrar por nível
//Recebe: O nome do arquivo, o nível e a altura da árvore
//Retorna: Nada
void printlevel(char *filename, int x, int height);

//Desenha a árvore
//Recebe: O nome do arquivo
//Retorna: Nada
void drawtree(char *filename);

//Função principal
//Recebe: Nada
//Retorna: 0
int main();
