//Universidade de Brasilia - Departamento de Ciencia da Computacao - 1/2021 (Setembro de 2021)
//Autor: 		Gabriel Tambara Rabelo
//Matricula: 	18/0017021

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <locale>

using namespace std;

void executa_codigo(vector<int> *dados, int *PC, int *ACC, bool *flag_fim, int *linha){

	int operador = 0, arg1 = 0, arg2 = 0;
	string input = "";

	operador = (*dados)[*PC];
	if(operador == 1){	     // ADD
		(*PC)++;
		arg1 = (*dados)[*PC];
		*ACC = *ACC + (*dados)[arg1];
		(*linha)++;
		(*PC)++;

	}else if(operador == 2){ // SUB
		(*PC)++;
		arg1 = (*dados)[*PC];
		*ACC = *ACC - (*dados)[arg1];
		(*linha)++;
		(*PC)++;

	}else if(operador == 3){ // MUL
		(*PC)++;
		arg1 = (*dados)[*PC];
		*ACC = *ACC * (*dados)[arg1];
		(*linha)++;
		(*PC)++;

	}else if(operador == 4){ // DIV
		(*PC)++;
		arg1 = (*dados)[*PC];
		*ACC = *ACC / (*dados)[arg1];
		(*linha)++;
		(*PC)++;

	}else if(operador == 5){ // JMP
		(*PC)++;
		arg1 = (*dados)[*PC];
		*PC = arg1;
		(*linha)++;

	}else if(operador == 6){ // JMPN
		(*PC)++;
		arg1 = (*dados)[*PC];
		if(*ACC < 0){
			*PC = arg1;
		}
		(*linha)++;
	
	}else if(operador == 7){ // JMPP
		(*PC)++;
		arg1 = (*dados)[*PC];
		if(*ACC > 0){
			*PC = arg1;
		}
		(*linha)++;
	
	}else if(operador == 8){ // JMPZ
		(*PC)++;
		arg1 = (*dados)[*PC];
		if(*ACC == 0){
			*PC = arg1;
		}
		(*linha)++;

	}else if(operador == 9){ // COPY
		(*PC)++;
		arg1 = (*dados)[*PC];
		(*PC)++;
		arg2 = (*dados)[*PC];
		(*dados)[arg2] = (*dados)[arg1];
		(*linha)++;
		(*PC)++;

	}else if(operador == 10){ // LOAD
		(*PC)++;
		arg1 = (*dados)[*PC];
		(*ACC) = (*dados)[arg1];
		(*linha)++;
		(*PC)++;

	}else if(operador == 11){ // STORE
		(*PC)++;
		arg1 = (*dados)[*PC];
		(*dados)[arg1] = (*ACC);
		(*linha)++;
		(*PC)++;

	}else if(operador == 12){ // INPUT
		(*PC)++;
		arg1 = (*dados)[*PC];
		cout << "Digite o input: " << endl;
		
		cin.clear();
		fflush(stdin);
		
		cin >> input;
		
		(*dados)[arg1] = stoi(input);
		(*linha)++;
		(*PC)++;

	}else if(operador == 13){ // OUTPUT
		(*PC)++;
		arg1 = (*dados)[*PC];
		cout << "Output: " << endl;
		cout << (*dados)[arg1] << endl;
		cin.get();
		cin.get();
		(*linha)++;
		(*PC)++;

	}else if(operador == 14){ // STOP
		(*PC)++;
		*flag_fim = true;
		(*linha)++;

	}else{
		printf("Codigo com operador invalido\n");
		(*PC)++;
	}
}

int main(int argc, char* argv[]){
	int PC = 0, ACC = 0, linha = 0;
	bool flag_fim = false;
	ifstream arq_entrada(argv[1]);
	string token;
	vector<int> dados;

	while(arq_entrada >> token){
		dados.push_back(stoi(token));
//		cout << dados.back() << " "; // debug = mostra o arquivo de entrada
	}
	cout << endl;

	while( PC < dados.size() - 1 and flag_fim == false ){
		executa_codigo(&dados, &PC, &ACC, &flag_fim, &linha);
		cout << "Linha/n de operacoes executadas: "  << linha << endl;
		cout << "Acc: " << ACC << endl;
		cout << "PC: " << PC << endl << endl;
		
	/*	debug mostra operacoes pausadas
			cin.clear();
			fflush(stdin);
			cin.get(); 
	*/
	}
	arq_entrada.close();

    return 0;
}