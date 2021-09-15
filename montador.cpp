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
#include <cmath>

using namespace std;

// item da tabela de símbolos com construtor para inicialização
class item_simbolo{
	public:
		string token;
		int valor;						// valor do simbolo
		bool definido;                  // se o valor foi definido (*ordem_alt muda funcionamento)
		int insercoes;					// quantas vezes o simbolo aparece no codigo como operando
		vector<vector<int>> pendencia;  // lista de pendencias
		int linha_declarada;            // linha/espaco na memoria para onde aponta
	
		item_simbolo(){
			definido = false;
			insercoes = 0;
			valor = -1;
			vector<int> aux = {-1,0};
			pendencia.push_back(aux);
			linha_declarada = -1;
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
	for(int i = 0 ; i < entrada.size() ;i++){
	  entrada[i] = toupper(entrada[i], loc);
	}
	return entrada;
}

bool is_number(string str) {	

	if(str.front() == '-'){
		str.front() = '0';
	}
	
	for(char const &c : str){
        if (isdigit(c) == 0){
			return false;
		}
    }
    return true;
}

bool is_integer(float num){
  return floor(num) == num;
}

void anal_Lexico(string str, int *contador_linha){
	int n = str.length();

	if(str.size() > 50){
		cout << "Erro lexico na linha " << to_string(*contador_linha) <<". Token " << str << " de tamanho invalido." <<  endl;
	}
	
    if (!(str[0] == '_' or (str[0] >= 'a' and str[0] <= 'z') or (str[0] >= 'A' and str[0] <= 'Z'))){
		cout << "Erro lexico na linha " << to_string(*contador_linha) <<". Token " << str << "  invalido." <<  endl;
		return;
	}
 
    for (int i = 1; i < str.length(); i++) {
        if (!(str[i] == '_' or (str[i] >= 'a' and str[i] <= 'z') or (str[i] >= 'A' and str[i] <= 'Z') or (str[i] >= '0' and str[i] <= '9'))){
			cout << "Erro lexico na linha " << to_string(*contador_linha) <<". Token " << str << "  invalido." <<  endl;
			return;
		}
    }
}

void parser(string *linha, int *contador_palavra, item_linha *itens, int *contador_palavra_aux, bool *ordem_alt, bool *secText_lido, int *contador_linha, bool *ha_rotulo, item_instrucao tabela_de_simbolos[]){
	int aux;
	vector<string> tokens;
	string token;
	*ha_rotulo = false;
	bool tem_copy = false;
	int num_argumentos = 0;

	if((*itens).flag_rot == false){
		(*itens).rotulo = " ";
		*ha_rotulo = false;
	}
	(*itens).operacao = " ";
	(*itens).arg1 = " ";
	(*itens).arg2 = " ";
	(*itens).arg1vetor = " ";
	(*itens).arg2vetor = " ";

// Lida com o caso de não haver espaço entre rótulo e operador
	if((*linha).find(":") != string::npos){
		(*linha).insert((*linha).find(":") + 1 , " ", 0, string::npos);
	}

	int virgula = (*linha).find(",");
	
	if((*linha)[virgula + 1] == ' '){
		while((*linha)[virgula + 1] == ' '){
			(*linha).erase(virgula + 1,1);
		}
		printf("Erro sintatico na linha %d. ", *contador_linha);
		printf("Espacos inadequados entre operandos de COPY apos virgula. Ref.: 'COPY A,B' \n");
	}
	virgula = (*linha).find(",");
	if(virgula > 0){
		if((*linha)[virgula - 1] == ' '){
			while((*linha)[virgula - 1] == ' '){
				(*linha).erase(virgula - 1,1);
				virgula = (*linha).find(",");
			}
			printf("Erro sintatico na linha %d. ", *contador_linha);
			printf("Espacos inadequados entre operandos de COPY antes da virgula. Ref.: 'COPY A,B' \n");					
		}	
	}
	
	//trata para separar os tokens
	replace((*linha).begin(), (*linha).end(), '\t', ' ');
	
	bool flag = false; // o próximo item a ser analisado é um espaço vazio?
	stringstream checa(*linha);
	
	while(checa >> token){
		tokens.push_back(token);
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
			if(*ha_rotulo == true){
				printf("Erro sintatico na linha %d. ", *contador_linha);
				printf("Outro rotulo foi previamente declarado na mesma linha.\n");
			}
			*ha_rotulo = true;

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

				continue;
			}
		}
		
		//checa se há situacao de multiplos argumentos(funcao copy)
		aux = tokens[i].find(',');
		int aux_plus = 0;
		if(aux != string::npos){
			tokens[i][aux] = 'a';
			if(count(tokens[i].begin(), tokens[i].end(), ',')){
				tokens[i][aux] = ',';
				printf("Erro sintatico na linha %d. ", *contador_linha);
				printf("Numero invalido de argumentos para operador.\n");
				while(count(tokens[i].begin(), tokens[i].end(), ',') > 1){
					tokens[i] = tokens[i].substr(0,tokens[i].rfind(","));
				}
			}else{
				tokens[i][aux] = ',';
			}
			
			tem_copy = true;
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
			continue;
		}
		
		aux_plus = tokens[i].find('+');
		if(aux_plus != string::npos){
			(*itens).arg1vetor = tokens[i].substr(aux_plus+1, tokens[i].size());
			(*itens).arg1 = tokens[i].substr(0,aux_plus);
		}else{
			(*itens).arg1 = tokens[i];
		}
	}
	
	// caso onde na linha existe apenas o rotulo
	if((*itens).flag_rot == true){
		(*itens).flag_rot = false;
		*ha_rotulo = false;
	}else{
		if((*itens).flag_rot == true and (*itens).arg1 == " " and (*itens).arg2 == " " and (*itens).operacao == " "){
			if(*ha_rotulo == true){
				printf("Erro sintatico na linha %d. ", *contador_linha);
				printf("Redeclaracao de rotulo na mesma linha.\n");
			}
			(*itens).flag_rot = true;
			*ha_rotulo = true;
		}
	}

// analise de erros:
	bool achou_operador = false;
	int aux_soma = 0;
	if(tem_copy == true){
		aux_soma = 1;
	}
	
	if(*ha_rotulo == true){
		for(int k=0; k < 14  ;k++){
			if( (*itens).operacao == tabela_de_simbolos[k].operador ){
				achou_operador = true; 
				if(tokens.size() + aux_soma != tabela_de_simbolos[k].argumentos + 1){
					printf("Erro sintatico na linha %d. ", *contador_linha);
					printf("Numero invalido de argumentos para operador.\n");
				}
			}
		}if(achou_operador == false and tokens.size() < 2 and tokens.size() > 4){
			printf("Erro sintatico na linha %d. ", *contador_linha);
			printf("Numero invalido de argumentos para qualquer possivel operador(nao identificado).\n");
		}
	}else{
		for(int k=0; k < 14  ;k++){
			if( (*itens).operacao == tabela_de_simbolos[k].operador ){
				achou_operador = true;
				if(tokens.size() + aux_soma != tabela_de_simbolos[k].argumentos){
					printf("Erro sintatico na linha %d. ", *contador_linha);
					printf("Numero invalido de argumentos para operador.\n");
				}
			}
		}if(achou_operador == false and tokens.size() < 2 and tokens.size() > 4){
			printf("Erro sintatico na linha %d. ", *contador_linha);
			printf("Numero invalido de argumentos para qualquer possivel operador(nao identificado).\n");
		}
	}
	if((*itens).rotulo != " "){
		anal_Lexico((*itens).rotulo, contador_linha);
	}
	if((*itens).operacao != " "){
		anal_Lexico((*itens).operacao, contador_linha);
	}
	if((*itens).arg1 != " "){
		if(((*itens).operacao == "CONST" or (*itens).operacao == "SPACE") and ! is_number((*itens).arg1)){
			printf("Erro semantico na linha %d. ", *contador_linha);
			printf("Argumento de diretiva nao eh um numero.\n");
		}else if(! is_number((*itens).arg1) ){
			anal_Lexico((*itens).arg1, contador_linha);
		}
	}
	if((*itens).arg2 != " "){
		if(((*itens).operacao == "CONST" or (*itens).operacao == "SPACE")){
			printf("Erro sintatico na linha %d. ", *contador_linha);
			printf("Numero de argumentos de diretiva invalidos.\n");
		}else if(! is_number((*itens).arg2) ){
			anal_Lexico((*itens).arg2, contador_linha);
		}
	}
	if((*itens).operacao == "SPACE" and is_number((*itens).arg1)){
		if(stoi((*itens).arg1) < 0 ){
			printf("Erro semantico na linha %d. ", *contador_linha);
			printf("Argumento de diretiva SPACE deve ser um valor positivo ou nulo.\n");
		}
	}
	if(!is_number((*itens).arg1vetor) and (*itens).arg1vetor != " "){
		printf("Erro semantico na linha %d. ", *contador_linha);
		printf("Indice de acesso a vetor nao eh um numero.\n");
		(*itens).arg1vetor = "0";
	}
	if(!is_number((*itens).arg2vetor) and (*itens).arg2vetor != " "){
		printf("Erro semantico na linha %d. ", *contador_linha);
		printf("Indice de acesso a vetor nao eh um numero.\n");
		(*itens).arg2vetor = "0";
	}

// debug
/*
	cout << "ROTULO: " << (*itens).rotulo << endl;
	cout << "OPERACAO: " << (*itens).operacao << endl;
	cout << "ARG1: " << (*itens).arg1 << endl;
	cout << "ARG1VETOR: " << (*itens).arg1vetor << endl;
	cout << "ARG2: " << (*itens).arg2 << endl;	
	cout << "ARG2VETOR: " << (*itens).arg2vetor << endl;
	cout << "INDICE LINHA: " << (*itens).indice_linha << endl;
*/
};

int handle_tabelas(vector<item_simbolo> *p_tabela_simbolos, item_linha itens, item_instrucao tabela_inst[14], int *contador_palavra, vector<int> *arq_saida, bool *ordem_alt, bool *secText_lido, vector<int> *arq_saida_aux, int *contador_palavra_aux, bool *flag_alt_order, int *contador_linha, bool *secData_lido){

	// inicializações principais
	bool simbolo_existente = false;
	item_simbolo item;
	int tem_rotulo = 0;
	int tamanho_a_aumentar = 0;
	int indice = 0;
	
	if(itens.operacao == " " and itens.arg1 == " " and itens.arg2 == " " and itens.rotulo == " "){
		return 1;
	}
	
	if(itens.operacao == "SECTION"){
		if(itens.arg1 == "TEXT"){
			*secText_lido = true;
		}else if(itens.arg1 == "DATA" and *secText_lido == false){
			*ordem_alt = true;
		}if(itens.arg1 == "TEXT"){
			*secData_lido = true;
		}
		if(*flag_alt_order == false and *secText_lido == true and *ordem_alt == true){
			*contador_palavra = 0;
			*flag_alt_order = true;
		}
		return 1;
	}
	
	// define o principal aumento de tamanho do arquivo saída
	if(itens.operacao != " " and itens.operacao != "CONST" and itens.operacao != "SPACE"){
		tamanho_a_aumentar++;
	}
	if(itens.arg1 != " " and itens.operacao != "CONST" and itens.operacao != "SPACE"){
		tamanho_a_aumentar++;
	}
	if(itens.arg2 != " " and itens.operacao != "CONST" and itens.operacao != "SPACE"){
		tamanho_a_aumentar++;
	}
	while(tamanho_a_aumentar > 0){
		if(*ordem_alt == true and *secText_lido == false){
			(*arq_saida_aux).push_back(0);
			tamanho_a_aumentar--;
			
			*contador_palavra = *contador_palavra + 1;
			*contador_palavra_aux = *contador_palavra_aux + 1;
		}else{
			(*arq_saida).push_back(0);
			tamanho_a_aumentar--;
			
			*contador_palavra = *contador_palavra + 1;
		}
	}
	
	bool flag_acha_simbolo = false;
	if(itens.rotulo != " " and *ordem_alt == false and *secData_lido == true){
		for(int i = 0 ; i < (*p_tabela_simbolos).size()  ; i++){
			if((*p_tabela_simbolos)[i].token == itens.rotulo){
				flag_acha_simbolo = true;
			}
		}
		if(flag_acha_simbolo == false){
			cout << "Erro semantico na linha " << *contador_linha << ". Simbolo ausente declarado." << endl;
		}
	}
	
	//trata o rótulo da linha
	if(itens.rotulo != " "){
		for(int i = 0; i < (*p_tabela_simbolos).size(); i++){
			if((*p_tabela_simbolos)[i].token == itens.rotulo){
				if((*p_tabela_simbolos)[i].definido == true){
					cout << "Erro semantico na linha " << *contador_linha << ". Simbolo "<< itens.rotulo << " previamente declarado." << endl;
				}else if(*ordem_alt == true and (*p_tabela_simbolos)[i].insercoes == 0){
					cout << "Erro semantico na linha " << *contador_linha << ". Simbolo "<< itens.rotulo << " previamente declarado." << endl;
				}
				
				simbolo_existente = true;
				indice = i;
			}
		}
		if(simbolo_existente == false){
			item.token = itens.rotulo;
			item.valor = itens.indice_linha;
			if(*ordem_alt == false or (*ordem_alt == true and *secText_lido == true)){
				item.definido = true;
			}
			
			item.linha_declarada = *contador_linha;
			(*p_tabela_simbolos).push_back(item);
			item.linha_declarada = -1;
			
		}else{
			(*p_tabela_simbolos)[indice].valor = itens.indice_linha;
			if(*ordem_alt == false or (*ordem_alt == true and *secText_lido == true)){
				(*p_tabela_simbolos)[indice].definido = true;
			}

			(*p_tabela_simbolos)[indice].linha_declarada = *contador_linha;
			
			//Loop de preenchimento de pendências
			if(*ordem_alt == false or (*ordem_alt == true and *secText_lido == true)){
				while((*p_tabela_simbolos)[indice].pendencia.size() > 1){
/*DEBUG*/	//		cout << "item da pilha de tras/ posicao a somar(vetor) / tamanho: " << (*p_tabela_simbolos)[indice].pendencia.back()[0] << "/ " << (*p_tabela_simbolos)[indice].pendencia.back()[1] << "/ " << (*p_tabela_simbolos)[indice].pendencia.size() << endl;
					(*arq_saida).at((*p_tabela_simbolos)[indice].pendencia.back()[0]) = itens.indice_linha + (*p_tabela_simbolos)[indice].pendencia.back()[1];
					(*p_tabela_simbolos)[indice].pendencia.pop_back();
				}
			}
		}
		simbolo_existente = false;
		tem_rotulo++;
	}
	
	//trata o operador/diretiva da linha
	bool tem_opcode = false;
	if(itens.operacao != " "){
		for(int i = 0; i < 14; i++){
			if(tabela_inst[i].operador == itens.operacao){
				//(*arq_saida).insert((*arq_saida).begin() + itens.indice_linha, tabela_inst[i].opcode);
				if(*ordem_alt == false or (*ordem_alt == true and *secText_lido == true) ){
					(*arq_saida).at(itens.indice_linha) = tabela_inst[i].opcode;
//					cout << "\nindice: " << (*arq_saida)[itens.indice_linha] << "\n\n" << endl;					
				}else{
					(*arq_saida_aux).push_back(tabela_inst[i].opcode);
					*contador_palavra = *contador_palavra + 1;
					if(*ordem_alt == true and *secText_lido == true){
						*contador_palavra_aux = *contador_palavra_aux + 1;
					}
//					cout << "\nindice: " << (*arq_saida_aux).back() << "\n\n" << endl;									
				}
				tem_opcode = true;
				break;
			}
		}
		if(tem_opcode == false){
			// é diretiva
			if(*ordem_alt == false or (*ordem_alt == true and *secText_lido == true)){
				if(itens.operacao == "SPACE"){
					if( is_number(itens.arg1) ){
						int tamanho_vetor = 0;
						if(is_number(itens.arg1)){
							tamanho_vetor = stoi(itens.arg1);
						}				
						while(tamanho_vetor >= 1){
							(*arq_saida).push_back(0);
							
							*contador_palavra = *contador_palavra + 1;
							if(*ordem_alt == true and *secText_lido == true){
								*contador_palavra_aux = *contador_palavra_aux + 1;
							}
							tamanho_vetor--;
						}
					}else if( itens.arg1 == " " ){
						(*arq_saida).push_back(0);
						*contador_palavra = *contador_palavra + 1;
						if(*ordem_alt == true and *secText_lido == true){
							*contador_palavra_aux = *contador_palavra_aux + 1;
						}
					}
//debug
/*				
					cout << "vetor saida de tamanho: " << (*arq_saida).size() << "\n" << endl;
					// DEBUG	
					printf("Arquivo normal\n");
					for (int i=0; i < (*arq_saida).size(); i++)
						printf("%d ",(*arq_saida).at(i));
					cout << '\n';	
					printf("Arquivo auxiliar\n");
					for (int i=0; i < (*arq_saida_aux).size(); i++)
						printf("%d ",(*arq_saida_aux).at(i));
					cout << '\n';		
*/			
					return 0;
				}else if(itens.operacao == "CONST"){
					if(is_number(itens.arg1)){
						(*arq_saida).push_back(stoi(itens.arg1));	
					}else{
						(*arq_saida).push_back(0);
					}

					*contador_palavra = *contador_palavra + 1;
					if(*ordem_alt == true and *secText_lido == true){
						*contador_palavra_aux = *contador_palavra_aux + 1;
					}
//debug
/*
					printf("Arquivo normal\n");
					for (int i=0; i < (*arq_saida).size(); i++)
						printf("%d ",(*arq_saida).at(i));
					cout << '\n';	
					printf("Arquivo auxiliar\n");
					for (int i=0; i < (*arq_saida_aux).size(); i++)
						printf("%d ",(*arq_saida_aux).at(i));
					cout << '\n';			
*/
					return 0;
				}else{
					printf("Erro sintatico na linha %d. ", *contador_linha);
					printf("Operador/Diretiva invalido/a. \n");
				}
			}else{
				if(itens.operacao == "SPACE"){
					if( is_number(itens.arg1) ){
						int tamanho_vetor = 0;
						if(is_number(itens.arg1)){
							tamanho_vetor = stoi(itens.arg1);
						}
						while(tamanho_vetor >= 1){
							(*arq_saida_aux).push_back(0);
							
							*contador_palavra = *contador_palavra + 1;
							if(*ordem_alt == true and *secText_lido == true){
								*contador_palavra_aux = *contador_palavra_aux + 1;
							}
							
							tamanho_vetor--;
						}
					}else if( itens.arg1 == " " ){
						(*arq_saida_aux).push_back(0);
						
						*contador_palavra = *contador_palavra + 1;
						if(*ordem_alt == true and *secText_lido == true){
							*contador_palavra_aux = *contador_palavra_aux + 1;
						}
						
					}
// debug					
/*
					printf("Arquivo normal\n");
					for (int i=0; i < (*arq_saida).size(); i++)
						printf("%d ",(*arq_saida).at(i));
					cout << '\n';	
					printf("Arquivo auxiliar\n");
					for (int i=0; i < (*arq_saida_aux).size(); i++)
						printf("%d ",(*arq_saida_aux).at(i));
					cout << '\n';			
*/				
					return 0;
				}else if(itens.operacao == "CONST"){
					if(is_number(itens.arg1)){
						(*arq_saida_aux).push_back(stoi(itens.arg1));
					}else{
						(*arq_saida_aux).push_back(0);
					}
					
					*contador_palavra = *contador_palavra + 1;
					if(*ordem_alt == true and *secText_lido == true){
						*contador_palavra_aux = *contador_palavra_aux + 1;
					}
					
// debug					
/*
					printf("Arquivo normal\n");
					for (int i=0; i < (*arq_saida).size(); i++)
						printf("%d ",(*arq_saida).at(i));
					cout << '\n';	
					printf("Arquivo auxiliar\n");
					for (int i=0; i < (*arq_saida_aux).size(); i++)
						printf("%d ",(*arq_saida_aux).at(i));
					cout << '\n';				
*/
					return 0;
				}else{
					printf("Erro sintatico na linha %d. ", *contador_linha);
					printf("Operador/Diretiva invalido/a. \n");
				}			
			}
		}
	}	

	// resetando item base
	item.token = " ";
	item.valor = 0;
	item.definido = false;
	int indice_vetor = 0;
	item.pendencia.clear();
	item.pendencia.push_back({-1,0});	
	
	//trata o primeiro argumento da linha
	if(itens.arg1 != " " and tem_opcode == true){
		indice_vetor = 0;
		if( is_number(itens.arg1vetor) == true){
			indice_vetor = stoi(itens.arg1vetor);
		}else{
			indice_vetor = 0;
		}
		for(int i = 0 ;i < (*p_tabela_simbolos).size() ; i++){
			if((*p_tabela_simbolos)[i].token == itens.arg1){
				simbolo_existente = true;
				if( (*p_tabela_simbolos)[i].definido == true ){
					(*arq_saida).at(itens.indice_linha + 1) = (*p_tabela_simbolos)[i].valor + indice_vetor;
					((*p_tabela_simbolos)[i].insercoes)++;
				}else{
					vector<int> aux2 = {1 + itens.indice_linha, indice_vetor};					

					((*p_tabela_simbolos)[i].insercoes)++;
					((*p_tabela_simbolos)[i].pendencia).push_back(aux2);
					(*arq_saida).at(itens.indice_linha + 1) = 0;
					
//					cout << "\nteste pendencia arg1:"  << to_string(((*p_tabela_simbolos)[i].pendencia).back()[0])  <<" com indices de vetor: " << to_string(((*p_tabela_simbolos)[i].pendencia).back()[1]) << "na linha: "  << endl;
				}
			}
		}
		if(simbolo_existente == false){
			item.token = itens.arg1;
			(*p_tabela_simbolos).push_back(item);
			int indice_final = (*p_tabela_simbolos).size() - 1;
			
			vector<int> aux3 = {1 + itens.indice_linha, indice_vetor};
			
			((*p_tabela_simbolos)[indice_final].insercoes)++;
			((*p_tabela_simbolos)[indice_final].pendencia).push_back(aux3);
			(*arq_saida).at(itens.indice_linha + 1) = 0;
		}
	}
	simbolo_existente = false;
	item.token = " ";
	item.valor = 0;
	item.definido = false;
	indice_vetor = 0;
	item.pendencia.clear();
	item.pendencia.push_back({-1,0});

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
					((*p_tabela_simbolos)[i].insercoes)++;
				}else{
					vector<int> aux4 = {2 + itens.indice_linha, indice_vetor};					
					
					((*p_tabela_simbolos)[i].insercoes)++;
					((*p_tabela_simbolos)[i].pendencia).push_back(aux4);
					(*arq_saida).at(itens.indice_linha + 2) = 0;
				}
			}
		}
		if(simbolo_existente == false){	
			item.token = itens.arg2;
			(*p_tabela_simbolos).push_back(item);
			int indice_final = (*p_tabela_simbolos).size() - 1;
			
			vector<int> aux5 = {2 + itens.indice_linha, indice_vetor};
			
			((*p_tabela_simbolos)[indice_final].insercoes)++;
			((*p_tabela_simbolos)[indice_final].pendencia).push_back(aux5);
			(*arq_saida).at(itens.indice_linha + 2) = 0;
		}
		//alternativa?  (*arq_saida).insert((*arq_saida).end(), 0);
	}

// debug
/*
	for(int j = 0; j < (*p_tabela_simbolos).size() ;j++){
		for(int i = 0; i <  (*p_tabela_simbolos)[j].pendencia.size()  ; i++){
			cout << "debug pilha: " << (*p_tabela_simbolos)[j].token << ":" << to_string((*p_tabela_simbolos)[j].pendencia[i][0]) << "," << to_string((*p_tabela_simbolos)[j].pendencia[i][1]) << ", definido?  "<< (*p_tabela_simbolos)[j].definido << " valor?  "<< to_string((*p_tabela_simbolos)[j].valor) << " insercoes? " << to_string((*p_tabela_simbolos)[j].insercoes)  << endl;
		}
	}
*/
	
// DEBUG
/*	
	printf("Arquivo normal\n");
	for (int i=0; i < (*arq_saida).size(); i++)
		printf("%d ",(*arq_saida).at(i));
	cout << '\n';	
	printf("Arquivo auxiliar\n");
	for (int i=0; i < (*arq_saida_aux).size(); i++)
		printf("%d ",(*arq_saida_aux).at(i));
	cout << '\n';
	cout << "\nWOW\n" << itens.indice_linha << "\nWOW\n" << endl;
*/
	return 0;

}

int handle_ordem_alt(vector<item_simbolo> *p_tabela_simbolos, int *p_contador_palavra, vector<int> *arq_saida, vector<int> *arq_saida_aux, int *p_contador_palavra_aux, vector<int> *arq_saida_final){

/* debug
	for(int j = 0; j < (*p_tabela_simbolos).size() ;j++){
		for(int i = 0; i <  (*p_tabela_simbolos)[j].pendencia.size()  ; i++){
			cout << "debug pilha: " << (*p_tabela_simbolos)[j].token << ":" << to_string((*p_tabela_simbolos)[j].pendencia[i][0]) << "," << to_string((*p_tabela_simbolos)[j].pendencia[i][1]) << ", definido?  "<< (*p_tabela_simbolos)[j].definido << " valor?  "<< to_string((*p_tabela_simbolos)[j].valor)  << endl;
		}
	}
*/
	for(int i = 0; i < (*p_tabela_simbolos).size() ; i++){
		if((*p_tabela_simbolos)[i].definido == false or ( (*p_tabela_simbolos)[i].definido == true and ((*p_tabela_simbolos)[i].insercoes) == 0 )){
			(*p_tabela_simbolos)[i].definido = true;
			(*p_tabela_simbolos)[i].valor += *p_contador_palavra;
			
			if(((*p_tabela_simbolos)[i].insercoes) == 0){
				cout << "Erro semantico na linha " << (*p_tabela_simbolos)[i].linha_declarada  << ". Simbolo ausente declarado."<< endl;
				continue;
			}
			
			while( (*p_tabela_simbolos)[i].pendencia.size() > 1 ){
//				cout << "item da pilha de tras / tamanho: " << (*p_tabela_simbolos)[i].token << " "  << (*p_tabela_simbolos)[i].pendencia.back()[0] << " " << (*p_tabela_simbolos)[i].pendencia.back()[1] << "/ " << (*p_tabela_simbolos)[i].pendencia.size() << endl;
				(*arq_saida).at((*p_tabela_simbolos)[i].pendencia.back()[0]) = (*p_tabela_simbolos)[i].valor + (*p_tabela_simbolos)[i].pendencia.back()[1];
				(*p_tabela_simbolos)[i].pendencia.pop_back();
			}
		}
	}
	
	*arq_saida_final = *arq_saida;
	(*arq_saida_final).insert((*arq_saida_final).end() , (*arq_saida_aux).begin() , (*arq_saida_aux).end());
	
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
	vector<int> arq_saida_aux;
	vector<int> arq_saida_final;
	item_linha itens;
	int contador_palavra = 0;
	int contador_palavra_aux = 0;
	int pega;
	int contador_linha = 0;
	bool secText_lido = false;
	bool secData_lido = false;
	bool ordem_alt = false;
	bool flag_alt_order = false;
	bool ha_rotulo = false;
	
	while(getline(arq_entrada, linha)){
		contador_linha++;

//		cout << "\nA linha coletada eh essa: " << linha  << endl;
		// ignora os comentários das linhas

		if(linha.find(";") != string::npos){
			linha = linha.substr(0, linha.find(";"));
		}
		
//		cout << "linha:" << linha << endl;
		parser(&linha, &contador_palavra, &itens, &contador_palavra_aux, &ordem_alt, &secText_lido, &contador_linha, &ha_rotulo, tabela);
		
		pega = handle_tabelas(&tabela_simbolos, itens, tabela, &contador_palavra, &arq_saida, &ordem_alt, &secText_lido, &arq_saida_aux, &contador_palavra_aux, &flag_alt_order, &contador_linha, &secData_lido);
//		cout << "ordem_alt: " << ordem_alt << endl; 
//		cout << "secText_lido: " <<  secText_lido << endl; 
		
		/*
		cout << "tabela_de_simbolos:\n" << endl;
		for(int i;i<tabela_simbolos.size();i++){
			cout << "token: " << tabela_simbolos[i].token << " valor: " << tabela_simbolos[i].valor << endl;
		}
		*/
	}
	
	if(ordem_alt == true){
		handle_ordem_alt(&tabela_simbolos, &contador_palavra, &arq_saida, &arq_saida_aux, &contador_palavra_aux, &arq_saida_final);
	}
	
	if(ordem_alt == false){
		arq_saida_final = arq_saida;
	}

	string nome_arq(argv[1]);
	ofstream outFile( nome_arq.substr(0, nome_arq.length() - 4 ) + ".obj" );
	
    for (int e : arq_saida_final){
        if (e < 10 and e >= 0){
            outFile << "0" << e <<  " ";
        }else{
            outFile << e << " ";
        }
    }
	
//	outFile.close();

// debug
/*	
	cout << "tabela de simbolos\n" << endl;
	for(int i=0 ;i < (tabela_simbolos).size()  ;i++){
		cout << "token: " << (tabela_simbolos)[i].token  << endl;
		cout << "definido: " << (tabela_simbolos)[i].definido  << endl;
		cout << "valor: " << to_string((tabela_simbolos)[i].valor)  << endl;
		cout << "pendencia: " << (tabela_simbolos)[i].pendencia.size()  << endl;
		cout << endl;
	}
*/
// debug
/*
	cout << "vetor saida de tamanho: " << arq_saida_final.size() << "\n" << endl;
	for (int i=0; i<arq_saida_final.size(); i++)
		cout << ' ' << arq_saida_final.at(i);
	cout << '\n';
*/
    return 0;
}