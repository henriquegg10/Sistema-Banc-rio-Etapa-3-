#include <iostream>
#include <iomanip>
#include <string>
#include <limits>
#include <cmath>
#include <fstream>
#include <sstream>

using namespace std;

// Limites definidos no enunciado.
const int MAX_CONTAS = 100;
const int MAX_MOVIMENTACOES = 500;

// Nomes dos arquivos usados para a persistencia dos dados (Etapa 3).
const string ARQUIVO_CONTAS = "contas.txt";
const string ARQUIVO_MOVIMENTACOES = "movimentacoes.txt";

// Arrays paralelos para armazenar as contas.
int numeroConta[MAX_CONTAS];
string nomeCliente[MAX_CONTAS];
string cpf[MAX_CONTAS];
int tipoConta[MAX_CONTAS]; // 1 = Corrente | 2 = Poupanca
double saldo[MAX_CONTAS];
bool contaAtiva[MAX_CONTAS];
int quantidadeContas = 0;

// Arrays paralelos para armazenar o historico de movimentacoes.
int contaMovimentacao[MAX_MOVIMENTACOES];
char tipoMovimentacao[MAX_MOVIMENTACOES]; // D = Deposito/entrada | S = Saque/saida
double valorMovimentacao[MAX_MOVIMENTACOES];
int quantidadeMovimentacoes = 0;

// Numero inicial usado para gerar os numeros das contas automaticamente.
// Ao carregar o arquivo, ele e ajustado para continuar depois da maior conta salva.
int proximoNumeroConta = 1001;

// ======================================================================
// Funcoes auxiliares de leitura do teclado
// ======================================================================

int lerInteiro(const string& mensagem) {
    int valor;
    while (true) {
        cout << mensagem;
        if (cin >> valor) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return valor;
        }
        cout << "Entrada invalida. Digite um numero inteiro.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

double lerDouble(const string& mensagem) {
    double valor;
    while (true) {
        cout << mensagem;
        if (cin >> valor && isfinite(valor)) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return valor;
        }
        cout << "Entrada invalida. Digite um valor numerico.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

string lerTexto(const string& mensagem) {
    string texto;
    while (true) {
        cout << mensagem;
        getline(cin, texto);
        if (texto.empty()) {
            cout << "O campo nao pode ficar vazio.\n";
        } else if (texto.find(';') != string::npos) {
            // O ';' e o separador dos campos no arquivo, entao nao pode aparecer no texto.
            cout << "O caractere ';' nao e permitido.\n";
        } else {
            return texto;
        }
    }
}

// Arredonda um valor para centavos, para que o valor na memoria
// seja o mesmo que fica gravado no arquivo (com 2 casas decimais).
double arredondarCentavos(double valor) {
    return round(valor * 100.0) / 100.0;
}

// ======================================================================
// Funcoes auxiliares para interpretar as linhas dos arquivos
// ======================================================================

// Remove o '\r' do final da linha (caso o arquivo tenha sido editado no Windows).
void removerRetornoCarro(string& linha) {
    if (!linha.empty() && linha[linha.size() - 1] == '\r') {
        linha.erase(linha.size() - 1);
    }
}

// Divide uma linha no formato "campo1;campo2;campo3" e guarda os campos no array.
// Retorna quantos campos foram encontrados na linha.
int separarCampos(const string& linha, string campos[], int maxCampos) {
    stringstream ss(linha);
    string campo;
    int quantidade = 0;
    while (getline(ss, campo, ';')) {
        if (quantidade < maxCampos) {
            campos[quantidade] = campo;
        }
        quantidade++;
    }
    return quantidade;
}

// Converte texto em inteiro. Retorna false se o texto nao for um inteiro valido.
bool converterInteiro(const string& texto, int& valor) {
    try {
        size_t posicao;
        valor = stoi(texto, &posicao);
        return posicao == texto.size();
    } catch (...) {
        return false;
    }
}

// Converte texto em double. Retorna false se o texto nao for um numero valido.
bool converterDouble(const string& texto, double& valor) {
    try {
        size_t posicao;
        valor = stod(texto, &posicao);
        return posicao == texto.size() && isfinite(valor);
    } catch (...) {
        return false;
    }
}

int localizarConta(int numero) {
    for (int i = 0; i < quantidadeContas; i++) {
        if (numeroConta[i] == numero) {
            return i;
        }
    }
    return -1;
}

// ======================================================================
// Funcoes de persistencia (leitura e gravacao dos arquivos) - Etapa 3
// ======================================================================

// Le o arquivo contas.txt e preenche os arrays de contas.
void carregarContas() {
    ifstream arquivo(ARQUIVO_CONTAS);

    // Se o arquivo nao existe (primeira execucao), o sistema inicia sem contas.
    if (!arquivo.is_open()) {
        return;
    }

    string linha;
    int numeroLinha = 0;

    while (getline(arquivo, linha)) {
        numeroLinha++;
        removerRetornoCarro(linha);
        if (linha.empty()) {
            continue;
        }
        if (quantidadeContas >= MAX_CONTAS) {
            cout << "Aviso: limite de " << MAX_CONTAS
                 << " contas atingido. As demais linhas de " << ARQUIVO_CONTAS
                 << " foram ignoradas.\n";
            break;
        }

        // Formato: numero;nome;cpf;tipo;saldo;ativa
        string campos[6];
        int numero, tipo, ativa;
        double valorSaldo;

        bool linhaValida = separarCampos(linha, campos, 6) == 6
            && converterInteiro(campos[0], numero)
            && !campos[1].empty()
            && !campos[2].empty()
            && converterInteiro(campos[3], tipo) && (tipo == 1 || tipo == 2)
            && converterDouble(campos[4], valorSaldo) && valorSaldo >= 0
            && converterInteiro(campos[5], ativa) && (ativa == 0 || ativa == 1)
            && localizarConta(numero) == -1; // evita numero de conta repetido

        if (!linhaValida) {
            cout << "Aviso: linha " << numeroLinha << " de " << ARQUIVO_CONTAS
                 << " ignorada (formato invalido).\n";
            continue;
        }

        int indice = quantidadeContas;
        numeroConta[indice] = numero;
        nomeCliente[indice] = campos[1];
        cpf[indice] = campos[2];
        tipoConta[indice] = tipo;
        saldo[indice] = arredondarCentavos(valorSaldo);
        contaAtiva[indice] = (ativa == 1);
        quantidadeContas++;

        // Garante que as proximas contas cadastradas nao repitam numeros ja usados.
        if (numero >= proximoNumeroConta) {
            proximoNumeroConta = numero + 1;
        }
    }

    arquivo.close();
}

// Grava todas as contas dos arrays no arquivo contas.txt.
// Sem ios::app, o conteudo anterior e substituido pela versao atualizada.
bool salvarContas() {
    ofstream arquivo(ARQUIVO_CONTAS);

    if (!arquivo.is_open()) {
        cout << "ERRO: nao foi possivel gravar o arquivo " << ARQUIVO_CONTAS << ".\n";
        return false;
    }

    arquivo << fixed << setprecision(2);
    for (int i = 0; i < quantidadeContas; i++) {
        arquivo << numeroConta[i] << ";"
                << nomeCliente[i] << ";"
                << cpf[i] << ";"
                << tipoConta[i] << ";"
                << saldo[i] << ";"
                << contaAtiva[i] << endl;
    }

    arquivo.close();
    return true;
}

// Le o arquivo movimentacoes.txt e preenche os arrays de movimentacoes.
void carregarMovimentacoes() {
    ifstream arquivo(ARQUIVO_MOVIMENTACOES);

    // Se o arquivo nao existe (primeira execucao), o sistema inicia sem movimentacoes.
    if (!arquivo.is_open()) {
        return;
    }

    string linha;
    int numeroLinha = 0;

    while (getline(arquivo, linha)) {
        numeroLinha++;
        removerRetornoCarro(linha);
        if (linha.empty()) {
            continue;
        }
        if (quantidadeMovimentacoes >= MAX_MOVIMENTACOES) {
            cout << "Aviso: limite de " << MAX_MOVIMENTACOES
                 << " movimentacoes atingido. As demais linhas de "
                 << ARQUIVO_MOVIMENTACOES << " foram ignoradas.\n";
            break;
        }

        // Formato: numeroConta;tipo;valor
        string campos[3];
        int numero;
        double valor;

        bool linhaValida = separarCampos(linha, campos, 3) == 3
            && converterInteiro(campos[0], numero)
            && (campos[1] == "D" || campos[1] == "S")
            && converterDouble(campos[2], valor) && valor > 0;

        if (!linhaValida) {
            cout << "Aviso: linha " << numeroLinha << " de " << ARQUIVO_MOVIMENTACOES
                 << " ignorada (formato invalido).\n";
            continue;
        }

        contaMovimentacao[quantidadeMovimentacoes] = numero;
        tipoMovimentacao[quantidadeMovimentacoes] = campos[1][0];
        valorMovimentacao[quantidadeMovimentacoes] = arredondarCentavos(valor);
        quantidadeMovimentacoes++;
    }

    arquivo.close();
}

// Grava todo o historico dos arrays no arquivo movimentacoes.txt.
// Como os arrays ja contem tambem as movimentacoes carregadas das execucoes
// anteriores, o historico completo e preservado.
bool salvarMovimentacoes() {
    ofstream arquivo(ARQUIVO_MOVIMENTACOES);

    if (!arquivo.is_open()) {
        cout << "ERRO: nao foi possivel gravar o arquivo " << ARQUIVO_MOVIMENTACOES << ".\n";
        return false;
    }

    arquivo << fixed << setprecision(2);
    for (int i = 0; i < quantidadeMovimentacoes; i++) {
        arquivo << contaMovimentacao[i] << ";"
                << tipoMovimentacao[i] << ";"
                << valorMovimentacao[i] << endl;
    }

    arquivo.close();
    return true;
}

// ======================================================================
// Funcoes do sistema bancario
// ======================================================================

void mostrarConta(int indice) {
    cout << fixed << setprecision(2);
    cout << "\nNumero da conta: " << numeroConta[indice] << '\n';
    cout << "Cliente: " << nomeCliente[indice] << '\n';
    cout << "CPF: " << cpf[indice] << '\n';
    cout << "Tipo: " << (tipoConta[indice] == 1 ? "Corrente" : "Poupanca") << '\n';
    cout << "Saldo: R$ " << saldo[indice] << '\n';
    cout << "Situacao: " << (contaAtiva[indice] ? "Ativa" : "Inativa") << "\n";
}

bool podeRegistrarMovimentacoes(int quantidadeNecessaria) {
    return quantidadeMovimentacoes + quantidadeNecessaria <= MAX_MOVIMENTACOES;
}

void registrarMovimentacao(int numero, char tipo, double valor) {
    // A capacidade e conferida antes da operacao que altera o saldo.
    contaMovimentacao[quantidadeMovimentacoes] = numero;
    tipoMovimentacao[quantidadeMovimentacoes] = tipo;
    valorMovimentacao[quantidadeMovimentacoes] = valor;
    quantidadeMovimentacoes++;
}

void cadastrarConta() {
    if (quantidadeContas >= MAX_CONTAS) {
        cout << "Limite de 100 contas atingido.\n";
        return;
    }

    cout << "\n=== CADASTRO DE CONTA ===\n";
    int indice = quantidadeContas;
    nomeCliente[indice] = lerTexto("Nome completo do cliente: ");
    cpf[indice] = lerTexto("CPF: ");

    do {
        tipoConta[indice] = lerInteiro("Tipo de conta (1 - Corrente | 2 - Poupanca): ");
        if (tipoConta[indice] != 1 && tipoConta[indice] != 2) {
            cout << "Opcao invalida. Escolha 1 ou 2.\n";
        }
    } while (tipoConta[indice] != 1 && tipoConta[indice] != 2);

    numeroConta[indice] = proximoNumeroConta++;
    saldo[indice] = 0.0;
    contaAtiva[indice] = true;
    quantidadeContas++;

    // Etapa 3: grava a nova conta no arquivo.
    salvarContas();

    cout << "\nConta cadastrada com sucesso!\n";
    cout << "Numero da conta: " << numeroConta[indice] << '\n';
    cout << "Saldo inicial: R$ 0.00\n";
}

void listarContas() {
    cout << "\n=== CONTAS CADASTRADAS ===\n";
    if (quantidadeContas == 0) {
        cout << "Nenhuma conta cadastrada.\n";
        return;
    }

    for (int i = 0; i < quantidadeContas; i++) {
        cout << "\n------------------------------\n";
        mostrarConta(i);
    }
}

void consultarConta() {
    cout << "\n=== CONSULTAR CONTA ===\n";
    int numero = lerInteiro("Informe o numero da conta: ");
    int indice = localizarConta(numero);

    if (indice == -1) {
        cout << "Conta nao encontrada.\n";
        return;
    }
    mostrarConta(indice);
}

void depositar() {
    cout << "\n=== DEPOSITO ===\n";
    int numero = lerInteiro("Informe o numero da conta: ");
    int indice = localizarConta(numero);

    if (indice == -1 || !contaAtiva[indice]) {
        cout << "Conta nao encontrada ou inativa.\n";
        return;
    }
    if (!podeRegistrarMovimentacoes(1)) {
        cout << "Limite de movimentacoes atingido. Deposito nao realizado.\n";
        return;
    }

    double valor = arredondarCentavos(lerDouble("Valor do deposito: R$ "));
    if (valor <= 0) {
        cout << "O valor do deposito deve ser maior que zero.\n";
        return;
    }

    saldo[indice] = arredondarCentavos(saldo[indice] + valor);
    registrarMovimentacao(numero, 'D', valor);

    // Etapa 3: grava o novo saldo e o historico atualizado.
    salvarContas();
    salvarMovimentacoes();

    cout << "Deposito realizado com sucesso. Novo saldo: R$ "
         << fixed << setprecision(2) << saldo[indice] << '\n';
}

void sacar() {
    cout << "\n=== SAQUE ===\n";
    int numero = lerInteiro("Informe o numero da conta: ");
    int indice = localizarConta(numero);

    if (indice == -1 || !contaAtiva[indice]) {
        cout << "Conta nao encontrada ou inativa.\n";
        return;
    }
    if (!podeRegistrarMovimentacoes(1)) {
        cout << "Limite de movimentacoes atingido. Saque nao realizado.\n";
        return;
    }

    double valor = arredondarCentavos(lerDouble("Valor do saque: R$ "));
    if (valor <= 0) {
        cout << "O valor do saque deve ser maior que zero.\n";
        return;
    }
    if (saldo[indice] < valor) {
        cout << "Saldo insuficiente. Saque nao realizado.\n";
        return;
    }

    saldo[indice] = arredondarCentavos(saldo[indice] - valor);
    registrarMovimentacao(numero, 'S', valor);

    // Etapa 3: grava o novo saldo e o historico atualizado.
    salvarContas();
    salvarMovimentacoes();

    cout << "Saque realizado com sucesso. Novo saldo: R$ "
         << fixed << setprecision(2) << saldo[indice] << '\n';
}

void transferir() {
    cout << "\n=== TRANSFERENCIA ===\n";
    int numeroOrigem = lerInteiro("Numero da conta de origem: ");
    int numeroDestino = lerInteiro("Numero da conta de destino: ");

    int indiceOrigem = localizarConta(numeroOrigem);
    int indiceDestino = localizarConta(numeroDestino);

    if (indiceOrigem == -1 || indiceDestino == -1) {
        cout << "Uma ou ambas as contas nao foram encontradas.\n";
        return;
    }
    if (!contaAtiva[indiceOrigem] || !contaAtiva[indiceDestino]) {
        cout << "As duas contas precisam estar ativas.\n";
        return;
    }
    if (numeroOrigem == numeroDestino) {
        cout << "A conta de origem e a de destino devem ser diferentes.\n";
        return;
    }
    if (!podeRegistrarMovimentacoes(2)) {
        cout << "Nao ha espaco para registrar os dois lancamentos da transferencia.\n";
        return;
    }

    double valor = arredondarCentavos(lerDouble("Valor da transferencia: R$ "));
    if (valor <= 0) {
        cout << "O valor da transferencia deve ser maior que zero.\n";
        return;
    }
    if (saldo[indiceOrigem] < valor) {
        cout << "Saldo insuficiente na conta de origem.\n";
        return;
    }

    // Atualiza os dois saldos e registra a saida e a entrada,
    // conforme o exemplo de transferencia do enunciado.
    saldo[indiceOrigem] = arredondarCentavos(saldo[indiceOrigem] - valor);
    saldo[indiceDestino] = arredondarCentavos(saldo[indiceDestino] + valor);
    registrarMovimentacao(numeroOrigem, 'S', valor);
    registrarMovimentacao(numeroDestino, 'D', valor);

    // Etapa 3: grava os dois saldos e os dois lancamentos nos arquivos.
    salvarContas();
    salvarMovimentacoes();

    cout << "Transferencia realizada com sucesso!\n";
    cout << "Novo saldo da conta " << numeroOrigem << ": R$ "
         << fixed << setprecision(2) << saldo[indiceOrigem] << '\n';
    cout << "Novo saldo da conta " << numeroDestino << ": R$ "
         << fixed << setprecision(2) << saldo[indiceDestino] << '\n';
}

void exibirExtrato() {
    cout << "\n=== EXTRATO ===\n";
    int numero = lerInteiro("Informe o numero da conta: ");
    int indice = localizarConta(numero);

    if (indice == -1) {
        cout << "Conta nao encontrada.\n";
        return;
    }

    cout << "\nExtrato da conta " << numero << " - " << nomeCliente[indice] << '\n';
    cout << "----------------------------------------\n";
    cout << fixed << setprecision(2);

    bool encontrou = false;
    double totalEntradas = 0.0;
    double totalSaidas = 0.0;

    for (int i = 0; i < quantidadeMovimentacoes; i++) {
        if (contaMovimentacao[i] == numero) {
            encontrou = true;
            if (tipoMovimentacao[i] == 'D') {
                cout << "Deposito/entrada: + R$ " << valorMovimentacao[i] << '\n';
                totalEntradas += valorMovimentacao[i];
            } else if (tipoMovimentacao[i] == 'S') {
                cout << "Saque/saida:      - R$ " << valorMovimentacao[i] << '\n';
                totalSaidas += valorMovimentacao[i];
            }
        }
    }

    if (!encontrou) {
        cout << "Nenhuma movimentacao registrada para esta conta.\n";
    }

    cout << "----------------------------------------\n";
    cout << "Total de entradas: R$ " << totalEntradas << '\n';
    cout << "Total de saidas:   R$ " << totalSaidas << '\n';
    cout << "Saldo atual:       R$ " << saldo[indice] << '\n';
    cout << "\nObs.: transferencias aparecem como saida na conta de origem e entrada na conta de destino.\n";
}

void exibirMenu() {
    cout << "\n========== SISTEMA BANCARIO ==========" << '\n';
    cout << "1 - Cadastrar conta\n";
    cout << "2 - Listar contas\n";
    cout << "3 - Consultar conta\n";
    cout << "4 - Realizar deposito\n";
    cout << "5 - Realizar saque\n";
    cout << "6 - Realizar transferencia\n";
    cout << "7 - Exibir extrato\n";
    cout << "0 - Sair\n";
    cout << "======================================\n";
}

int main() {
    // Etapa 3: recupera automaticamente os dados salvos nas execucoes anteriores.
    carregarContas();
    carregarMovimentacoes();

    cout << "Dados carregados: " << quantidadeContas << " conta(s) e "
         << quantidadeMovimentacoes << " movimentacao(oes).\n";

    int opcao;

    do {
        exibirMenu();
        opcao = lerInteiro("Escolha uma opcao: ");

        switch (opcao) {
            case 1:
                cadastrarConta();
                break;
            case 2:
                listarContas();
                break;
            case 3:
                consultarConta();
                break;
            case 4:
                depositar();
                break;
            case 5:
                sacar();
                break;
            case 6:
                transferir();
                break;
            case 7:
                exibirExtrato();
                break;
            case 0:
                cout << "\nEncerrando o sistema. Os dados ja estao salvos. Ate mais!\n";
                break;
            default:
                cout << "Opcao invalida. Escolha uma opcao do menu.\n";
        }
    } while (opcao != 0);

    return 0;
}
