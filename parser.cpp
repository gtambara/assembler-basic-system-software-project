#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <locale>

using namespace std;

// item da tabela de símbolos com construtor para inicialização
class item_simbolo{
	public:
		string token;
		int valor;
		bool definido;
		vector<vector<int>> pendencia;
	
		item_simbolo(){
			definido = false;
			valor = -1;
			vector<int> aux = {-1,0};
			pendencia.push_back(aux);
		}
};

class item_linha{
	public:
		string rotulo, operacao, arg1, arg1vetor, arg2, arg2vetor;
		int indice_linha; // posicao na memoria do primeiro item da linha;
		bool flag_rot;
	
		item_linha(){
			rotulo = " ";
			operacao = " ";
			arg1 = " ";
			arg2 = " ";
			arg1vetor = " ";
			arg2vetor = " ";
			indice_linha = 0;
			flag_rot = false; // se for true, nao apaga o rotulo na iteracao seguinte
		}
};

struct item_instrucao{
	string operador;
	int opcode;
	int argumentos;
};

string uppercase(string entrada){
	locale loc;
	for(int i=0 ; i < entrada.size() ;i++){
	  entrada[i] = toupper(entrada[i], loc);
	}
	return entrada;
}

void parser(string *linha, int *contador_palavra, item_linha *itens){
	int aux;
	vector<string> tokens;
	
	if((*itens).flag_rot == false){
		(*itens).rotulo = " ";
	}
	(*itens).operacao = " ";
	(*itens).arg1 = " ";
	(*itens).arg2 = " ";
	(*itens).arg1vetor = " ";
	(*itens).arg2vetor = " ";
	
	
	//trata para separar os tokens
	replace((*linha).begin(), (*linha).end(), '\t', ' ');
	
	bool flag = false; // o próximo item a ser analisado é um espaço vazio?
	stringstream checa(*linha);
	
	while(getline(checa, *linha, ' ')){
		if(flag == true){
			flag = false;
			continue;
		}
		
		if(checa.peek() == ' '){
			flag = true;
		}

		tokens.push_back(*linha);
	}

	flag = false; // na linha já foi coletado um operando?
	bool linha_contada = false;
	
	// deixando não sensivel ao caso
	for(int i = 0; i < tokens.size(); i++){
		tokens[i] = uppercase(tokens[i]);
	}
	
	for(int i = 0; i < tokens.size(); i++){
		//checa se há rótulo
		aux = tokens[i].find(':');
		
		if(aux != string::npos){
			tokens[i].pop_back();
			(*itens).rotulo = tokens[i];

			if(linha_contada == false){
				(*itens).indice_linha = *contador_palavra;
				linha_contada = true;
			}
			
			continue;
		}else{
			if(flag == false){
				(*itens).operacao = tokens[i];
				flag = true;
				
				if(linha_contada == false){
					(*itens).indice_linha = *contador_palavra;
					linha_contada = true;
				}
				if((*itens).operacao != "SECTION" and (*itens).operacao != "CONST"){
					*contador_palavra = *contador_palavra + 1;
				}
				continue;
			}
		}
		
		//checa se há situacao de multiplos argumentos(funcao copy)
		aux = tokens[i].find(',');
		int aux_plus = 0;
		
		if(aux != string::npos){
			(*itens).arg1 = tokens[i].substr(0,aux);
			(*itens).arg2 = tokens[i].substr(aux+1,tokens[i].length());
			
			aux_plus = (*itens).arg1.find('+');
			if(aux_plus != string::npos){
				(*itens).arg1vetor = (*itens).arg1.substr(aux_plus+1,aux);
				(*itens).arg1 = (*itens).arg1.substr(0,aux_plus);
			}
			aux_plus = (*itens).arg2.find('+');
			if(aux_plus != string::npos){
				(*itens).arg2vetor = (*itens).arg2.substr(aux_plus+1,tokens[i].length());
				(*itens).arg2 = (*itens).arg2.substr(0,aux_plus);
			}
			
			*contador_palavra = *contador_palavra + 2;
			continue;
		}
		
		aux_plus = tokens[i].find('+');
		if(aux_plus != string::npos){
			(*itens).arg1vetor = tokens[i].substr(aux_plus+1, tokens[i].size());
			(*itens).arg1 = tokens[i].substr(0,aux_plus);
		}else{
			(*itens).arg1 = tokens[i];
		}
			
		if((*itens).operacao != "SECTION"){
			*contador_palavra = *contador_palavra + 1;
		}
	}
	
	// caso onde na linha existe apenas o rotulo
	if((*itens).flag_rot == true){
		(*itens).flag_rot = false;
	}else{
		if((*itens).flag_rot == true and (*itens).arg1 == " " and (*itens).arg2 == " " and (*itens).operacao == " "){
			(*itens).flag_rot = true;
		}
	}

	cout << "ROTULO: " << (*itens).rotulo << endl;
	cout << "OPERACAO: " << (*itens).operacao << endl;
	cout << "ARG1: " << (*itens).arg1 << endl;
	cout << "ARG1VETOR: " << (*itens).arg1vetor << endl;
	cout << "ARG2: " << (*itens).arg2 << endl;	
	cout << "ARG2VETOR: " << (*itens).arg2vetor << endl;
	cout << "INDICE LINHA: " << (*itens).indice_linha << endl;

};

bool is_number(const string &s) {
	return !s.empty() && all_of(s.begin(), s.end(), ::isdigit);
}

// Adaptar lista de pendencia para poder usar vetores: a lsita de pendencia deve ser um vetor de uma dupla(primeiro valor o endereço normal e o segundo o adicional que seria o indice do vetor)

int handle_tabelas(vector<item_simbolo> *p_tabela_simbolos, item_linha itens, item_instrucao tabela_inst[14], int *p_contador_palavra, vector<int> *arq_saida){

	// inicializações principais
	bool simbolo_existente = false;
	item_simbolo item;
	int tem_rotulo = 0;
	int tamanho_a_aumentar = 0;
	int indice = 0;
	
	// define o principal aumento de tamanho do arquivo saída
	if(itens.operacao == "SECTION"){
		return 1;
	}
	if(itens.operacao != " " and itens.operacao != "CONST"){
		tamanho_a_aumentar++;
	}
	if(itens.arg1 != " " and itens.operacao != "SPACE"){
		tamanho_a_aumentar++;
	}
	if(itens.arg2 != " " and itens.operacao != "CONST" and itens.operacao != "SPACE"){
		tamanho_a_aumentar++;
	}
	while(tamanho_a_aumentar > 0){
		(*arq_saida).push_back(0);
		tamanho_a_aumentar--;
	}
	
	//trata o rótulo da linha
	if(itens.rotulo != " "){
		printf("testeradical\n");
		for(int i = 0; i < (*p_tabela_simbolos).size(); i++){
			if((*p_tabela_simbolos)[i].token == itens.rotulo){
				simbolo_existente = true;
				indice = i;
			}
		}
		if(simbolo_existente == false){
			printf("É AQUI QUE DEU RUIM?\n");
			item.token = itens.rotulo;
			item.valor = itens.indice_linha;
			item.definido = true;
			(*p_tabela_simbolos).push_back(item);
			
		}else{
			(*p_tabela_simbolos)[indice].valor = itens.indice_linha;
			(*p_tabela_simbolos)[indice].definido = true;
/*CHECAR*/	cout << "MASTE;R CHECK:\n" << (*p_tabela_simbolos).back().token << endl;

			//Loop de preenchimento de pendências
			while((*p_tabela_simbolos)[indice].pendencia.size() > 1){
//				cout << "item da pilha de tras / tamanho: " << (*p_tabela_simbolos)[indice].pendencia.back()[0] << (*p_tabela_simbolos)[indice].pendencia.back()[1] << "/ " << (*p_tabela_simbolos)[indice].pendencia.size() << endl;
				(*arq_saida).at((*p_tabela_simbolos)[indice].pendencia.back()[0]) = itens.indice_linha + (*p_tabela_simbolos)[indice].pendencia.back()[1];
				printf("AOBA\n");
				(*p_tabela_simbolos)[indice].pendencia.pop_back();
				
			}
		}
		simbolo_existente = false;
		tem_rotulo++;
	}
	
	for (int i=0; i < (*arq_saida).size(); i++)
		printf("%d ",(*arq_saida).at(i));
	cout << '\n';	
	
	//trata o operador/diretiva da linha
	bool tem_opcode = false;
	if(itens.operacao != " "){
		for(int i = 0; i < 14; i++){
			if(tabela_inst[i].operador == itens.operacao){
				//(*arq_saida).insert((*arq_saida).begin() + itens.indice_linha, tabela_inst[i].opcode);
				(*arq_saida).at(itens.indice_linha) = tabela_inst[i].opcode;
				cout << "\nindice: " << (*arq_saida)[itens.indice_linha] << "\n\n" << endl;
				tem_opcode = true;
				break;
			}
		}
		
		if(tem_opcode == false){
			// é diretiva
			if(itens.operacao == "SPACE"){
				if( is_number(itens.arg1) ){
					int tamanho_vetor = stoi(itens.arg1);
					while(tamanho_vetor > 1){
						(*arq_saida).push_back(0);
						tamanho_vetor--;
					}
				}else if( itens.arg1 == " " ){
					(*arq_saida).at(itens.indice_linha) = 0;
				}
				
				cout << "vetor saida de tamanho: " << (*arq_saida).size() << "\n" << endl;
				for (int i=0; i < (*arq_saida).size(); i++)
					printf("%d ",(*arq_saida).at(i));
				cout << '\n';		
				
				return 0;
			}else if(itens.operacao == "CONST"){
				(*arq_saida).at(itens.indice_linha) = stoi(itens.arg1);
				
				cout << "vetor saida de tamanho: " << (*arq_saida).size() << "\n" << endl;
				for (int i=0; i < (*arq_saida).size(); i++)
					printf("%d ",(*arq_saida).at(i));
				cout << '\n';			
				
				return 0;
			}else{
				// Operação/diretiva invalida
			}
		}
	}
	
	cout << "vetor saida de tamanho: " << (*arq_saida).size() << "\n" << endl;
	for (int i=0; i < (*arq_saida).size(); i++)
		printf("%d ",(*arq_saida).at(i));
	cout << '\n';	
	
	// resetando item base
	item.token = " ";
	item.valor = 0;
	item.definido = false;
	int indice_vetor = 0;
	
	//trata o primeiro argumento da linha
	if(itens.arg1 != " " and tem_opcode == true){
		indice_vetor = 0;
		if( is_number(itens.arg1vetor) == true){
			indice_vetor = stoi(itens.arg1vetor);
			printf("\n arg1vetor eh numero!\n  ");
		}else{
			indice_vetor = 0;
		}
		for(int i = 0 ;i < (*p_tabela_simbolos).size() ; i++){
			if((*p_tabela_simbolos)[i].token == itens.arg1){
				simbolo_existente = true;
				if( (*p_tabela_simbolos)[i].definido == true ){
					(*arq_saida).at(itens.indice_linha + 1) = (*p_tabela_simbolos)[i].valor + indice_vetor;
				}else{
					vector<int> aux2 = {1 + itens.indice_linha, indice_vetor};					
					
					((*p_tabela_simbolos)[i].pendencia).insert(((*p_tabela_simbolos)[i].pendencia).end(), aux2);
					(*arq_saida).at(itens.indice_linha + 1) = 0;
					
					cout << "\nteste pendencia arg1:"  << to_string(((*p_tabela_simbolos)[i].pendencia).back()[0])  <<" com indices de vetor: " << to_string(((*p_tabela_simbolos)[i].pendencia).back()[1])  << endl;
				}
			}
		}
		if(simbolo_existente == false){
			item.token = itens.arg1;
			(*p_tabela_simbolos).insert((*p_tabela_simbolos).end(), item);
			int indice_final = (*p_tabela_simbolos).size() - 1;
			
			vector<int> aux3 = {1 + itens.indice_linha, indice_vetor};
			
			((*p_tabela_simbolos)[indice_final].pendencia).insert(((*p_tabela_simbolos)[indice_final].pendencia).end(), aux3);
			(*arq_saida).at(itens.indice_linha + 1) = 0;
		}
	}
	item.token = " ";
	item.valor = 0;
	item.definido = false;
	indice_vetor = 0;
	item.pendencia.clear();

	//trata o segundo argumento da linha
	if(itens.arg2 != " " and tem_opcode == true){
		if( is_number(itens.arg2vetor) == true){
			indice_vetor = stoi(itens.arg2vetor);
		}else{
			indice_vetor = 0;
		}
		for(int i = 0 ;i < (*p_tabela_simbolos).size() ; i++){
			if((*p_tabela_simbolos)[i].token == itens.arg2){
				simbolo_existente = true;			
				if( (*p_tabela_simbolos)[i].definido == true ){
					(*arq_saida).at(itens.indice_linha + 2) = (*p_tabela_simbolos)[i].valor + indice_vetor;
				}else{
					vector<int> aux4 = {2 + itens.indice_linha, indice_vetor};					
					
					((*p_tabela_simbolos)[i].pendencia).insert(((*p_tabela_simbolos)[i].pendencia).end(), aux4);
					(*arq_saida).at(itens.indice_linha + 2) = 0;
				}
			}
		}
		if(simbolo_existente == false){	
			item.token = itens.arg2;
			(*p_tabela_simbolos).insert((*p_tabela_simbolos).end(), item);
			int indice_final = (*p_tabela_simbolos).size() - 1;
			
			vector<int> aux5 = {2 + itens.indice_linha, indice_vetor};
			
			((*p_tabela_simbolos)[indice_final].pendencia).insert(((*p_tabela_simbolos)[indice_final].pendencia).end(), aux5);
			(*arq_saida).at(itens.indice_linha + 2) = 0;
		}
		//alternativa?  (*arq_saida).insert((*arq_saida).end(), 0);
	}


	for(int j = 0; j < (*p_tabela_simbolos).size() ;j++){
		for(int i = 0; i <  (*p_tabela_simbolos)[j].pendencia.size()  ; i++){
			cout << "debug pilha: " << (*p_tabela_simbolos)[j].token << ":" << to_string((*p_tabela_simbolos)[j].pendencia[i][0]) << "," << to_string((*p_tabela_simbolos)[j].pendencia[i][1]) << endl;
		}
	}
	
	for (int i=0; i < (*arq_saida).size(); i++)
		printf("%d ",(*arq_saida).at(i));
	cout << '\n';

	return 0;
}

int main(int argc, char* argv[]){
	
	//DEFININDO TABELA DE INSTRUCOES	
	item_instrucao tabela[14];

	tabela[0].operador = "ADD";
	tabela[0].opcode = 1;
	tabela[0].argumentos = 2;
	
	tabela[1].operador = "SUB";
	tabela[1].opcode = 2;
	tabela[1].argumentos = 2;
	
	tabela[2].operador = "MUL";
	tabela[2].opcode = 3;
	tabela[2].argumentos = 2;
	
	tabela[3].operador = "DIV";
	tabela[3].opcode = 4;
	tabela[3].argumentos = 2;

	tabela[4].operador = "JMP";
	tabela[4].opcode = 5;
	tabela[4].argumentos = 2;
	
	tabela[5].operador = "JMPN";
	tabela[5].opcode = 6;
	tabela[5].argumentos = 2;
	
	tabela[6].operador = "JMPP";
	tabela[6].opcode = 7;
	tabela[6].argumentos = 2;
	
	tabela[7].operador = "JMPZ";
	tabela[7].opcode = 8;
	tabela[7].argumentos = 2;
	
	tabela[8].operador = "COPY";
	tabela[8].opcode = 9;
	tabela[8].argumentos = 3;

	tabela[9].operador = "LOAD";
	tabela[9].opcode = 10;
	tabela[9].argumentos = 2;
	
	tabela[10].operador = "STORE";
	tabela[10].opcode = 11;
	tabela[10].argumentos = 2;
	
	tabela[11].operador = "INPUT";
	tabela[11].opcode = 12;
	tabela[11].argumentos = 2;
	
	tabela[12].operador = "OUTPUT";
	tabela[12].opcode = 13;
	tabela[12].argumentos = 2;
	
	tabela[13].operador = "STOP";
	tabela[13].opcode = 14;
	tabela[13].argumentos = 1;

	// Definições e instâncias gerais
	ifstream arq_entrada(argv[1]);
	string linha;
	vector<item_simbolo> tabela_simbolos;
	vector<int> arq_saida;
	item_linha itens;
	int contador_palavra = 0;
	int pega;
	
	while(getline(arq_entrada, linha)){
		// ignora os comentários das linhas
		if(int inicio_comentario = linha.find(';') != string::npos){
			linha.erase(inicio_comentario);
		}
		cout << "linha:" << linha << endl;
		parser(&linha, &contador_palavra, &itens);
		pega = handle_tabelas(&tabela_simbolos, itens, tabela, &contador_palavra, &arq_saida);
		
		/*
		cout << "tabela_de_simbolos:\n" << endl;
		for(int i;i<tabela_simbolos.size();i++){
			cout << "token: " << tabela_simbolos[i].token << " valor: " << tabela_simbolos[i].valor << endl;
		}
		*/
	}
	
	cout << "tabela de simbolos\n" << endl;
	for(int i=0 ;i < (tabela_simbolos).size()  ;i++){
		cout << "token: " << (tabela_simbolos)[i].token  << endl;
		cout << "definido: " << (tabela_simbolos)[i].definido  << endl;
		cout << "valor: " << to_string((tabela_simbolos)[i].valor)  << endl;
		cout << "pendencia: " << (tabela_simbolos)[i].pendencia.size()  << endl;
		cout << endl;
	}

	cout << "vetor saida de tamanho: " << arq_saida.size() << "\n" << endl;
	for (int i=0; i<arq_saida.size(); i++)
		cout << ' ' << arq_saida.at(i);
	cout << '\n';

    return 0;
}