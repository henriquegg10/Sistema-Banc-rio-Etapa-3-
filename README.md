# 🏦 Sistema de Registro e Gestão de Contas Bancárias

Trabalho da disciplina **INF101 – Programação de Computadores I**
Centro Universitário de Viçosa – **UNIVIÇOSA**
Professores: Anderson R. Lamas e Vanderlea Queiroz
Aluno: **Henrique Gomes Gonçalves**

---

## 📌 Sobre o projeto

Sistema bancário em **C++** executado no terminal, que permite cadastrar contas, fazer depósitos, saques e transferências, e consultar o extrato de cada conta.

### Etapa 3: Persistência em arquivos
Até a Etapa 2, os dados ficavam só na memória (arrays) e se perdiam quando o programa era fechado.
Nesta etapa o sistema passou a **salvar tudo em arquivos de texto** e a **carregar os dados automaticamente** quando é aberto de novo. Assim, as contas, os saldos e o histórico de movimentações continuam lá depois que o programa é encerrado.

---

## ⚙️ Funcionalidades

1. Cadastrar contas bancárias (número gerado automaticamente a partir de 1001)
2. Listar as contas cadastradas
3. Consultar uma conta pelo número
4. Realizar depósitos
5. Realizar saques (com verificação de saldo)
6. Realizar transferências entre duas contas
7. Exibir o extrato de uma conta (entradas, saídas e saldo atual)

---

## 💾 Arquivos do sistema

Os arquivos são criados automaticamente na primeira operação e ficam na mesma pasta de onde o programa é executado.

### `contas.txt`
Formato: `numero;nome;cpf;tipo;saldo;ativa`

```
1001;Ana Silva;12345678900;1;700.00;1
1002;Bruno Souza;98765432100;2;800.00;1
```

| Campo | Descrição |
|-------|-----------|
| tipo  | 1 = Corrente, 2 = Poupança |
| ativa | 1 = Ativa, 0 = Inativa |

### `movimentacoes.txt`
Formato: `numeroConta;tipo;valor`

```
1001;S;300.00
1002;D;300.00
```

| Código | Movimentação |
|--------|--------------|
| D | Depósito / entrada |
| S | Saque / saída |

> Nas transferências são gravados dois registros: uma **saída (S)** na conta de origem e uma **entrada (D)** na conta de destino.

---

## 🧩 Funções de persistência

| Função | O que faz |
|--------|-----------|
| `carregarContas()` | Lê `contas.txt` ao iniciar e preenche os arrays de contas |
| `salvarContas()` | Grava todas as contas dos arrays em `contas.txt` |
| `carregarMovimentacoes()` | Lê `movimentacoes.txt` ao iniciar e preenche os arrays do histórico |
| `salvarMovimentacoes()` | Grava todo o histórico em `movimentacoes.txt` |

Recursos de C++ utilizados: `fstream` (`ifstream`, `ofstream`), `is_open()`, `getline()` e `close()`.

- Se os arquivos ainda não existirem (primeira execução), o sistema começa sem contas.
- Depois de cada cadastro, depósito, saque ou transferência, os arquivos são atualizados na hora.
- O número das novas contas continua a partir da maior conta já salva, sem repetir números.
- Linhas com formato inválido nos arquivos são ignoradas com um aviso, sem travar o programa.
- O caractere `;` não é aceito no nome nem no CPF, porque é o separador dos campos no arquivo.

---

## ▶️ Como compilar e executar

Com o **g++** instalado:

```bash
g++ -std=c++11 -o banco sistema_bancario.cpp
./banco
```

No Windows:

```bash
g++ -std=c++11 -o banco.exe sistema_bancario.cpp
banco.exe
```

---

## ✅ Testes realizados

| Teste | Passos | Resultado |
|-------|--------|-----------|
| 1 – Cadastro e recuperação | Cadastrar 3 contas, fechar, abrir e listar | As 3 contas aparecem com todos os dados |
| 2 – Persistência dos saldos | Depositar em uma conta, sacar em outra, fechar e abrir | Os saldos continuam iguais |
| 3 – Persistência das transferências | Transferir R$ 300,00 da conta 1001 para a 1002, fechar e abrir | Saldos de R$ 700,00 e R$ 800,00; o extrato mostra a saída e a entrada |
| 4 – Histórico de movimentações | Fazer várias operações, fechar, abrir e ver o extrato | Todas as movimentações aparecem, inclusive as de execuções anteriores |

---

## 📁 Estrutura do repositório

```
├── sistema_bancario.cpp   # Código-fonte do sistema
└── README.md              # Este arquivo
```
