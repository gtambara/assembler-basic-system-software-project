README-PseudoAssembler
Autor: 		Gabriel Tambara Rabelo
Matricula: 	18/0017021
Universidade de Brasilia, Departamento de Ciência da computação. 

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
