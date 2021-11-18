README IN BRAZILIAN PORTUGUSE [PT-BR]

README-PseudoAssembler  
Autor: 		Gabriel Tambara Rabelo  
Matricula: 	18/0017021  
Universidade de Brasilia, Departamento de Ciência da computação.  

As instruções para a língua hipotética são representadas pela figura a seguir:

| OPERATION | OPCODE | SIZE |     DESCRIPTION     |
|:---------:|:------:|:----:|:-------------------:|
|    ADD    |    1   |   2  | ACC <- ACC + mem(OP) |
|    SUB    |    2   |   2  | ACC <- ACC - mem(OP) |
|    MUL    |    3   |   2  | ACC <- ACC * mem(OP) |
|    DIV    |    4   |   2  | ACC <- ACC / mem(OP) |
|    JMP    |    5   |   2  |       PC < OP       |
|    JMPN   |    6   |   2  |   JMP if (ACC < 0)   |
|    JMPP   |    7   |   2  |   JMP if (ACC > 0)   |
|    JMPZ   |    8   |   2  |   JMP if (ACC = 0)   |
|    COPY   |    9   |   2  | mem(OP2) <- mem(OP1) |
|    LOAD   |   10   |   2  |    ACC <- mem(OP)    |
|   STORE   |   11   |   2  |    mem(OP) <- ACC    |
|   INPUT   |   12   |   2  |   mem(OP) <- input   |
|   OUTPUT  |   13   |   2  |   output <- mem(OP)  |
|    STOP   |   14   |   1  |    stop execution   |

O montador é capaz de detectar e notificar erros e possui certar propriedades:

* Não é sensível ao caso
* Desconsidera tabulação, quebra de linha e espaços desnecessários
* Pode trabalhar com vetores, utilizando o seguinte formato: LABEL+'NUMERO', sem espaço em torno do '+'
* Aceita comentários em linhas iniciadas com ";"
* Utiliza um algoritmo de passagem única
* Pode criar um rótulo, ler uma quebra de linha e continuar com as intruções em seguida
* Pode detectar os erros:
  * declarações de rótulos ausentes
  * declarações de rótulos repetidos
  * instruções inválidas
  * diretivas inválidas
  * instruções com a quantidade de operando errada
  * tokens inválidos
  * dois rótulos na mesma linha

Para compilar o montador, assim como pedido nas instruções do trabalho da universidade, executa-se a seguinte linha de comando no terminal:

g++ .\montador.cpp 

e para executar o arquivo objeto gerado(realizar a montagem):

.\a.exe [nome do arquivo .asm sem os colchetes] -r x

sendo que 'x' pode ser substituído por 0 (mapa de bits) ou 1 (lista de endereços) como formas de alocacao

---------------

Para o simulador, que recebe a saída do montador, o processo é análogo:

g++ .\simulador.cpp

e para executar:

.\a.exe [nome do arquivo .obj sem os colchetes]

---------------

Para o carregador, que também recebe como entrada a saída do montador, o processo é:

g++ .\simulador.cpp

e para executar:

.\a.exe [nome do arquivo .obj sem os colchetes] 3 10 11 12 1000 2000 3000

Sendo o 3 o número de chunks a serem utilizados e em seguida os 3 tamanhos de cada chunk e em seguida os 3 respectivos inícios de espaço
