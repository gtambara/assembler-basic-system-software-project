//Universidade de Brasilia - Departamento de Ciencia da Computacao - 1/2021 (Novembro de 2021)
//Autor: 		Gabriel Tambara Rabelo
//Matricula: 	18/0017021
//Compilador: GCC/G++ 6.3.0 MinGW
//OS: Windows 10

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <locale>
#include <cmath>

using namespace std;

bool is_number(string str){	

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

class chunk{
	public:
		int tamanho;
		int addr_init;
		int espaco_livre;
	
	chunk(){
		tamanho = 0;
		addr_init = 0;
		espaco_livre = 0;
	}
};

class programa{
	public:
		string nome_arq;
		int tamanho;
		bool mapa_de_bits;
		vector<int> alocacao;
		vector<int> codigo;
		vector<vector<int>> relacao_alocacao;
			// vetor de vetores do tipo (indice do chunk, inicio de onde será salvo no chunk, quantos simbolos serao escritos)
		int simbolos_consumidos; // quantos simbolos do codigo ja estao marcados para serem salvos em algum chunk
		vector<int> offsets;
		vector<int> codigo_fim; // codigo realocado e corrigido por chunks
		
		
	programa(){
		nome_arq = "";
		tamanho = -1;
		mapa_de_bits = false;
	}
};

void divide_chunks(programa *programas, vector<chunk> *chunks, int qtd_programas, int qtd_de_chunks){
	bool flag_pronto = false;
	for(int i = 0; i < qtd_programas; i++){
		for(int j = 0; j < qtd_de_chunks; j++){
			// vê se cabe por completo em algum chunk
			if((*chunks)[j].espaco_livre == 0){
				continue;
			}
			int diferenca = (*chunks)[j].espaco_livre - (programas)[i].tamanho;
			
			if(diferenca >= 0){
				// ha espaco para alocar completamente nesse chunk
				vector<int> vetor{j, (*chunks)[j].tamanho - (*chunks)[j].espaco_livre + (*chunks)[j].addr_init, (programas)[i].tamanho};
				(programas)[i].relacao_alocacao.push_back(vetor);
				(*chunks)[j].espaco_livre = diferenca;
				(programas)[i].simbolos_consumidos = (programas)[i].tamanho;
				
				flag_pronto = true;
				break;
			}
		}
		if(flag_pronto){
			flag_pronto = false;
			continue;
		}
		for(int j = 0; j < qtd_de_chunks; j++){
			// vê se cabe parcialmente em algum chunk
			if((*chunks)[j].espaco_livre == 0){
				continue;
			}
			
			int tamanho_a_escrever = (programas)[i].tamanho - (programas)[i].simbolos_consumidos; 
			if((*chunks)[j].espaco_livre > tamanho_a_escrever){
				// tem mais espaco do que precisa pra terminar de escrever todo o codigo
				(programas)[i].simbolos_consumidos = (programas)[i].tamanho;
				(*chunks)[j].espaco_livre -= tamanho_a_escrever;
				vector<int> vetor{j, (*chunks)[j].tamanho - (*chunks)[j].espaco_livre - tamanho_a_escrever + (*chunks)[j].addr_init, tamanho_a_escrever};
				(programas)[i].relacao_alocacao.push_back(vetor);
				
				break;
				
			}else if((*chunks)[j].espaco_livre == tamanho_a_escrever){
				// tem exatamente o que precisa pra terminar de escrever o codigo
				(programas)[i].simbolos_consumidos = (programas)[i].tamanho;
				(*chunks)[j].espaco_livre = 0;
				vector<int> vetor{j, (*chunks)[j].tamanho - (*chunks)[j].espaco_livre - tamanho_a_escrever + (*chunks)[j].addr_init, tamanho_a_escrever};
				(programas)[i].relacao_alocacao.push_back(vetor);
				
				break;
				
			}else{
				// ainda falta espaco pra escrever o codigo
				vector<int> vetor{j, (*chunks)[j].tamanho - (*chunks)[j].espaco_livre + (*chunks)[j].addr_init, (*chunks)[j].espaco_livre};
				(programas)[i].relacao_alocacao.push_back(vetor);	
				(programas)[i].simbolos_consumidos += (*chunks)[j].espaco_livre;	
				(*chunks)[j].espaco_livre = 0;			
				
			}
		}
	}
	
	// retorna no terminal as informacoes de alocacao dos programas
	cout << endl;
	for(int i=0;i< qtd_programas;i++){
		if((programas)[i].simbolos_consumidos - (programas)[i].tamanho != 0){
			cout << (programas)[i].nome_arq << " NAO FOI POSSIVEL ALOCAR\n";
			continue;
		}
		cout << (programas)[i].nome_arq << " utilizando " << (programas)[i].relacao_alocacao.size() << " CHUNKS. Enderecos iniciais: ";
		for(int j=0; j < (programas)[i].relacao_alocacao.size(); j++){
			cout << (programas)[i].relacao_alocacao[j][1] << " ";
		}
		cout << "\n";
	}	
	
	
}

void parser(programa *programas, int qtd_programas){
	string linha;
	int contador;

	for(int i=0;i<qtd_programas;i++){
		
		ifstream arquivo(programas[i].nome_arq + ".obj");
		string linha;
		contador = 0;
		
		while(getline(arquivo, linha)){
			contador++;
			linha.erase(0,3); // Remove o inicio da linha: "H: "
			if(contador == 2){
				programas[i].tamanho = stoi(linha);
			}
			
			if(contador == 3){
				if(linha[0] == '0'){
					programas[i].mapa_de_bits = true;
					for (size_t k = 0; k < linha.size(); ++k){
						programas[i].alocacao.push_back(linha[k] - '0');
					}
				}else{
					int numero;
					stringstream iss(linha);
					while ( iss >> numero ){
						programas[i].alocacao.push_back(numero);
					}
				}
			}
			
			if(contador == 4){
				int numero;
				stringstream iss(linha);
				while ( iss >> numero ){
					programas[i].codigo.push_back(numero);
				}
			}
		}	
	}	
}

void alocacao_chunks(programa *programas, vector<chunk> *chunks, int qtd_programas, int qtd_de_chunks){
	int offset;
	int coef_corretor; // auxilia para achar offset de chunks pelo vetor relacao_alocacao
	int tamanho_acumulado; // tamanho acumulado dos chunks anteriores usados para alocacao do codigo
	
	for(int i=0; i < qtd_programas ; i++){
		offset = 0;
		coef_corretor = 0;
		tamanho_acumulado = 0;
		
		for(int j=0;j < programas[i].codigo.size();j++){
			if(j >= programas[i].relacao_alocacao[coef_corretor][2] + tamanho_acumulado){ // será que ** por tamanho_acumulado funciona?
				tamanho_acumulado += programas[i].relacao_alocacao[coef_corretor][2]; // ? sera que pode somar por esse valor mesmo?
				coef_corretor++;
			}
			offset = (*chunks)[ programas[i].relacao_alocacao[coef_corretor][0] ].addr_init - tamanho_acumulado;
			programas[i].offsets.push_back(offset);
		}
		
		cout << "offsets: ";
		for(int j=0;j < programas[i].offsets.size();j++){
			cout << programas[i].offsets[j]  << " ";
		}
		cout << "\n\n";
		
		// SE FOR MAPA DE BITS
		if(programas[i].mapa_de_bits){
			for(int j=0;j < programas[i].codigo.size();j++){
				if(programas[i].alocacao[j] == 1){
					programas[i].codigo_fim.push_back( programas[i].codigo[j] + programas[i].offsets[ programas[i].codigo[j] ] );
				}else{
					programas[i].codigo_fim.push_back(programas[i].codigo[j]);
				}
			}
		}else{
		// SE FOR LISTA DE ENDERECOS
			
			for(int j=0;j < programas[i].codigo.size();j++){
				programas[i].codigo_fim.push_back(programas[i].codigo[j]);
			}
			for(int j=0;j < programas[i].alocacao.size();j++){
				programas[i].codigo_fim.at(programas[i].alocacao[j]) += programas[i].offsets[ programas[i].codigo_fim.at(programas[i].alocacao[j]) ];
			}
		}
	}
}

void cria_arquivos(programa *programas, vector<chunk> *chunks, int qtd_programas, int qtd_de_chunks){
	for(int i = 0; i < qtd_programas ;i++){
		if(programas[i].simbolos_consumidos != programas[i].tamanho){
			continue;
		}
		
		string nome;
		nome = programas[i].nome_arq + ".saida";
		ofstream arq_saida(nome);
		bool flag_secao_data = false;
		
		for(int j = 0; j < programas[i].codigo_fim.size(); j++){
			if(programas[i].codigo_fim[j] == 14){
				// FIM DO SECTION TEXT
				flag_secao_data = true;
			}
			arq_saida << programas[i].offsets[j] + j << " " << programas[i].codigo_fim[j];
			
			if(flag_secao_data == false){
				if(programas[i].codigo_fim[j] == 9){
					// CASO FUNCAO COPY
					
					arq_saida << " " << programas[i].codigo_fim[j+1] << " " << programas[i].codigo_fim[j+2];
					j = j+2;
				}else{
					arq_saida << " " << programas[i].codigo_fim[j+1];
					j++;
				}
			}
			arq_saida << "\n";
		}
		arq_saida.close();
	}
}

int main(int argc, char* argv[]){
	int qtd_programas = 0;
	int qtd_de_chunks = 0;
	programa programas[3];
	vector<chunk> chunks;
	chunk base;
	
	// inicialização da classe que guarda dados dos 3 programas
	for(int i=0;i<3;i++){
		(programas[i]).nome_arq = "";
		(programas[i]).tamanho = -1;
		(programas[i]).mapa_de_bits = false;
		(programas[i]).alocacao.clear();
		(programas[i]).codigo.clear();
		(programas[i]).offsets.clear();
		(programas[i]).codigo_fim.clear();
		(programas[i]).relacao_alocacao.clear();
		(programas[i]).simbolos_consumidos = 0;
	}
	
	// Coleta de dados
	for(qtd_programas; qtd_programas < argc ;qtd_programas++){
		if(is_number(argv[qtd_programas+1])){
			break;
		}
	}
	
	// cout << "Nomes arquivos: \n";
	for(int i=0; i < qtd_programas;i++){
		programas[i].nome_arq = argv[i+1];
		programas[i].nome_arq = programas[i].nome_arq.substr(0,programas[i].nome_arq.size() - 4);
		// cout << programas[i].nome_arq;
	}
	
	qtd_de_chunks = stoi(argv[qtd_programas + 1]);
	
	for(int j=0; j < qtd_de_chunks ;j++){
		base.tamanho = stoi(argv[qtd_programas + 2 + j]);
		base.espaco_livre = base.tamanho;
		base.addr_init = stoi(argv[qtd_programas + 2 + qtd_de_chunks + j]);
		chunks.push_back(base);
	}
	
	parser(programas, qtd_programas);
	divide_chunks(programas, &chunks, qtd_programas, qtd_de_chunks);
	alocacao_chunks(programas, &chunks, qtd_programas, qtd_de_chunks);
	cria_arquivos(programas, &chunks, qtd_programas, qtd_de_chunks);

    return 0;
}