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

	tmapa mapa;
	if(argc==2){
		ler_mapa(argv[2], &mapa);
	}else{
		//ler_mapa("./teste_5_4.txt", &mapa);
		//ler_mapa("./teste_10_10.txt", &mapa);
		ler_mapa("./teste_100_100.txt", &mapa);

	}
	
	double temperatura = mapa.ncolunas*mapa.nlinhas;
	double epsilon = 1;
	double delta = 0.0;
	double alpha = 0.9;
	
	tplano * s;
	s = aloca_plano(&mapa);
	plano_aleatorio(mapa.ncores, s, mapa.ncores);
	
	contrair(s);
	simula_plano(&mapa, s);
	qualidade(s, &mapa);
	
	tplano * melhor;
	melhor = aloca_plano(&mapa);
	copia_plano(s, melhor);
	clrscr();
	printf("\n*********************");
	printf("\nIniciando Simulated Annealing");
	printf("\nTemperatura 0: %.2f", temperatura);
	printf("\nSolucao inicial: ");
	mostra_plano(s);
	printf("\nTamanho maximo da solucao: %d", mapa.ncolunas*mapa.nlinhas);
	printf("\nMapa: ");
	mostra_mapa(&mapa);
	int interacoes = 0;
	while(temperatura > epsilon){
		
		interacoes++;
		tplano * r = aloca_plano(&mapa);
		if(interacoes%120==0){
			printf("\nInt %d", interacoes);
		}
		copia_plano(s, r);
		pertuba(r, &mapa);
		qualidade(r, &mapa);
		double random = (double)(rand()%100)/100;
    	delta = calcular_delta(s, r, temperatura);
    	
		if(r->qualidade > s->qualidade || random < delta){
    		copia_plano(r, s);
		}
		
		temperatura = temperatura*alpha;
    	if(s->qualidade > melhor->qualidade){
    		copia_plano(s, melhor);
		}
		
	}
	printf("\nInteracoes: %d", interacoes);
	mostra_plano(melhor);
	printf("%d", melhor->fronteira);
	
}

void qualidade(tplano* p, tmapa* m){
  if(p->fronteira==0){
  	p->qualidade = (double)m->ncores/(double)p->passos;
  }else{
  	p->qualidade = (double)m->ncores/pow(M_E, ((double)p->passos*(double)p->fronteira));
  }
}

void pertuba(tplano * s, tmapa * m){
	simula_plano(m, s);
	if(s->sugestao==-1){
		int pos1 = rand()%s->passos;	
		int pos2 = rand()%s->passos;		
		int cor_aux = s->cor[pos1];
		s->cor[pos1] = s->cor[pos2];
		s->cor[pos2] = cor_aux;	
		contrair(s);
		simula_plano(m, s);
	}else{
		insere_plano(s, s->sugestao);	
	}
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
}


int cor_aleatoria(int cores){
	return (rand()%cores)+1;
}

int seleciona_cor(tplano * s, tmapa * m){
	return cor_aleatoria(m->ncores);
}



