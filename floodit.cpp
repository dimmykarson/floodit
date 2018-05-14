#include <stdio.h>
#include <conio.h>
#include "mapa.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <float.h>

tmapa ler_mapa(char * filename, tmapa*);
void gerar_mapa_aux(tmapa *, bool **);
void pertuba(tplano * s, tmapa * m);
void qualidade(tplano* s, tmapa* m);
double calcular_delta(tplano * s, tplano * next, double temperatura);
void plano_aleatorio(int cores, tplano *s, int tam);
void contrair(tplano *s);
int cor_aleatoria(int cores);
int seleciona_cor(tplano * s, tmapa * m);


int main(int argc, char **argv){
	srand(time(NULL));   
	
	
	/*
	Comments
	Carregando o mapa do Flood It.
	*/
	tmapa mapa;
	if(argc==2){
		ler_mapa(argv[1], &mapa);
	}else{
		//ler_mapa("./teste_5_4.txt", &mapa);
		//ler_mapa("./teste_10_10.txt", &mapa);
		ler_mapa("./teste_30_30.txt", &mapa);
		//ler_mapa("./teste_100_100.txt", &mapa);
	}
	
	/*Parâmetros para a Têmpera Simulada*/
	double temperatura = 4000;
	double epsilon = 1;
	double delta = 0.0;
	double alpha = 0.999;
	
	tplano * s;
	s = aloca_plano(&mapa);
	plano_aleatorio(mapa.ncores, s, mapa.ncores);
	

	/*A função contrair retira da solução passos repetidos*/
	contrair(s);
	simula_plano(&mapa, s);
	qualidade(s, &mapa);
	
	tplano * melhor;
	melhor = aloca_plano(&mapa);
	copia_plano(s, melhor);
	
	tplano * melhor_zero;
	melhor_zero = aloca_plano(&mapa);

	clrscr();
	printf("\n*********************");
	printf("\nIniciando Simulated Annealing");
	printf("\nTemperatura 0: %.2f", temperatura);
	printf("\nSolucao inicial: ");
	mostra_plano(s);
	printf("\nTamanho maximo da solucao: %d", mapa.ncolunas*mapa.nlinhas);

	int interacoes = 0;
	while(temperatura > epsilon){
		
		interacoes++;
		tplano * r = aloca_plano(&mapa);
		if(interacoes%120==0){
			printf("\nInt %d", interacoes);
		}
		copia_plano(s, r);
		/*Tweak do SA*/
		pertuba(r, &mapa);
		qualidade(r, &mapa);
		double random = (double)(rand()%100)/100;
		/*Cálculo de Delta para que sejam aceitas soluções ruins em determinadas circunstâncias*/
    	delta = calcular_delta(s, r, temperatura);
    	if(r->qualidade > s->qualidade || random < delta){
    		copia_plano(r, s);
		}
		if(r->fronteira ==0 && melhor_zero->qualidade < r->qualidade){
			copia_plano(r, melhor_zero);
		}
		/*Decréscimo da temperatura*/
		temperatura = temperatura*alpha;
    	if(s->qualidade > melhor->qualidade){
    		/*Armazenando a solução de melhor qualidade*/
    		copia_plano(s, melhor);
		}
		//libera_plano(r);
	}
	simula_plano(&mapa, melhor);
	if(melhor->fronteira>0){
		copia_plano(melhor_zero, melhor);
	}
	
	printf("\nInteracoes: %d\nSolucao Final:\n", interacoes);
	mostra_plano(melhor);
	//printf("\nFronteira: %d", melhor->fronteira);
	//printf("\nResta resolver: %d", melhor->solucao);
	//simula_plano_aux(&mapa, melhor);
	
}

void qualidade(tplano* p, tmapa* m){
	if(p->restam==0)
		p->qualidade = 1.0/(double)p->passos;
	else
 		p->qualidade = 1.0/((double)p->restam*(double)p->passos);
}

/*
Funcao Tweak

caso a simulação da solução anterior tenha sugerido algum movimento, este é adicionado a solução
caso contrário, uma 'permutação' entre passos é realizada

para adicionar uma sugestão, o algoritmo realiza o produto da fronteira do mapa com a quantidade de 
elementos restantes no mapa, isto é, se a cor 2 parece na duas vezes na fronteira e aparece mais 10 vezes 
no mapa, então seu peso na escolha é 20, logo se nenhuma cor superar o valor de 20, então a cor 2 será sugerida 
para o próximo movimento.

*/

void pertuba(tplano * s, tmapa * m){
	simula_plano(m, s);
	if(s->sugestao==-1){
		int pos1 = rand()%s->passos;	
		int pos2 = rand()%s->passos;		
		int cor_aux = s->cor[pos1];
		s->cor[pos1] = s->cor[pos2];
		s->cor[pos2] = cor_aux;	
		contrair(s);
	} else {
		insere_plano(s, s->sugestao);	
	}
	simula_plano(m, s);
}


double calcular_delta( tplano * s, tplano * next, double temperatura){
	double d = next->qualidade - s->qualidade;
	double x = pow(M_E, (d / temperatura));
	return x;
}


void gerar_mapa_aux(tmapa * mapa, bool ** mapa_auxiliar) {
	int i, j;
	mapa_auxiliar = (bool**) malloc(mapa->nlinhas * sizeof(bool*));
	for(i = 0; i < mapa->nlinhas; i++) {
		mapa_auxiliar[i] = (bool*) malloc(mapa->ncolunas * sizeof(bool));
    	for(j = 0; j < mapa->ncolunas; j++)
      		mapa_auxiliar[i][j] = false;
  	}
  	mapa_auxiliar[0][0] = true;
  	//mapa->mapa_auxiliar = mapa_auxiliar;
}


void plano_aleatorio(int cores, tplano *s, int tam){
	s->qualidade = 0;
	for(int i = 0;i<tam;i++){
		int cor = (rand()%cores)+1;
		insere_plano(s, cor);
	}
}



void contrair(tplano *p){
	int i;
	int t_original = p->passos;
	for(i = 0; i < p->passos; i++){
		if(p->cor[i]==p->cor[i+1]){
			p->cor[i]=-1;
			p->passos--;
		}
	}
	int * cor = (int*) malloc(p->passos * sizeof(int));
	int index = 0;
	for(i = 0;i<t_original;i++){
		if(p->cor[i]!=-1){
			cor[index] = p->cor[i];
			index++;
		}
	}
	free(p->cor);
	p->cor = cor;
}



tmapa  ler_mapa(char * filename, tmapa* mapa){
	printf("\nLendo arquivo");
	FILE *arq;
	arq = fopen(filename, "rt");
	int i = 1;
	int n;
	int m;
	int k;
    fscanf(arq, "%d", &n);
	fscanf(arq, "%d", &m);
	fscanf(arq, "%d", &k);
	
	mapa->nlinhas = n;
	mapa->ncolunas = m;
	mapa->ncores = k;
    mapa->mapa = (int**) malloc(mapa->nlinhas * sizeof(int*));
	for(int i = 0;i<n;i++){
		mapa->mapa[i] = (int*) malloc(mapa->ncolunas * sizeof(int));
		for(int j = 0;j<m;j++){
			int c;
			fscanf(arq, "%d", &c);
			mapa->mapa[i][j] = 	c;
		}
	}
	mostra_mapa(mapa);
	fclose(arq);
	printf("\nFinalizou leitura do arquivo\n");
}


int cor_aleatoria(int cores){
	return (rand()%cores)+1;
}

int seleciona_cor(tplano * s, tmapa * m){
	return cor_aleatoria(m->ncores);
}



