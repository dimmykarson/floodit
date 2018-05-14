#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "mapa.h"
#include <math.h>

void insere_plano(tplano *p, int cor) {
	//printf("\nAdicionando cor %d", cor);
//printf("\nInsere");
  p->cor[p->passos] = cor;
  p->passos++;
}

void remove_plano(tplano *p) {
  p->passos--;
}

void remove_n_planos(tplano *p, int qt) {
	//printf("\nRemovendo %d passos", qt);
  p->passos-=qt;
}

void copia_plano(tplano *po, tplano *pd) {
  int i;

  pd->solucao = po->solucao;
  pd->passos = po->passos;
  pd->qualidade = po->qualidade;
  pd->fronteira = po->fronteira;
  pd->sugestao = po->sugestao;
  pd->restam = po->restam;
  
  for(i = 0; i < po->passos; i++)
    pd->cor[i] = po->cor[i];

}
  
tplano * aloca_plano(tmapa *m) {
  tplano *p;

  p = (tplano *) malloc(sizeof(tplano));
  p->solucao = 0;
  p->passos = 0;
  p->qualidade = 0;
  p->fronteira = 0;
  p->restam = 0;
  p->cor = (int *) malloc(m->nlinhas * m->ncolunas * sizeof(int));
  return p;
}
  
void libera_plano(tplano *p) {
  free(p->cor);
  free(p);
}

void carrega_plano(tplano *p) {
  int i;

  scanf("%d", &(p->passos));
  for(i = 0; i < p->passos; i++)
    scanf("%d", &(p->cor[i]));
}

void mostra_plano(tplano *p) {
  int i;

  printf("\n%d\n", p->passos);
  for(i = 0; i < p->passos; i++)
    printf("%d ", p->cor[i]);
  printf("\n");
}

void simula_plano(tmapa *m, tplano *p) {
  //printf("\nSimula");
  int i;
  tmapa *ms;
  ms = aloca_mapa(m);
  copia_mapa(m, ms);
  for(i = 0; i < p->passos; i++) {
    pinta_mapa(ms, p->cor[i]);
  }
  tfronteira *f;  
  f = aloca_fronteira(ms);
  fronteira_mapa(ms, f);
  //printf("\n Contando cores"); 
  p->fronteira = f->tamanho;
  //Contando cores
	int cores[ms->ncores+1] = {};
	int index_maior=-1;
	int maior=0;
	int cor_0_0 = ms->mapa[0][0];	
	int soma_dif = 0;
	for(i = 0;i<ms->nlinhas;i++){
		for(int j = 0;j<ms->ncolunas;j++){
			cores[ms->mapa[i][j]]++;
			if(cores[ms->mapa[i][j]]>maior){
				index_maior=ms->mapa[i][j];
				maior = cores[ms->mapa[i][j]];
			}
			if(ms->mapa[i][j]!=cor_0_0){
				soma_dif++;
			}
		}	
	}
	cores[0] = soma_dif;
	//fim contar cores
  int fm[ms->ncores+1] = {};
  int mult_maior = 0;
  int cor_sugerir = -1;
  for(i = 0;i<f->tamanho;i++){
  	fm[f->pos[i].v]++;		
  	int mult = fm[f->pos[i].v]*cores[f->pos[i].v];
  	if(mult>mult_maior){
  		cor_sugerir = f->pos[i].v;
  		mult_maior = mult;	
	}
  }
  //printf("\n fim Contando cores");
  p->sugestao = cor_sugerir;
  p->restam = cores[0];
  
  //libera_fronteira(f);
  //libera_mapa(ms);
}



void simula_plano_aux(tmapa *m, tplano *p) {
  
  int i;
  tmapa *ms;
  ms = aloca_mapa(m);
  copia_mapa(m, ms);
  for(i = 0; i < p->passos; i++) {
    pinta_mapa(ms, p->cor[i]);
  }
  mostra_mapa(ms);
  printf("\n");
  
}

tfronteira * aloca_fronteira(tmapa *m) {
  tfronteira *f;

  f = (tfronteira *) malloc(sizeof(tfronteira));
  f->tamanho = 0;
  f->pos = (tpos *) malloc(m->nlinhas * m->ncolunas * sizeof(tpos));
  return f;
}
  
void libera_fronteira(tfronteira *f) {
  free(f->pos);
  free(f);
}

void mostra_fronteira(tfronteira *f) {
  int i;

  for(i = 0; i < f->tamanho; i++)
    printf("(%d,%d):%d ", f->pos[i].l, f->pos[i].c, f->pos[i].v);
  printf("\n%d\n", f->tamanho);
}

int compara_pos(int l1, int c1, int l2, int c2) {
  if(l1 < l2)
    return -1;
  if(l1 > l2)
    return 1;
  if(c1 < c2)
    return -1;
  if(c1 > c2)
    return 1;
  return 0;
}

void insere_fronteira(tfronteira *f, int l, int c, int v) {
  int i;
  int j;
  int cmp;

  for(i = 0; i < f->tamanho; i++) {
    cmp = compara_pos(f->pos[i].l, f->pos[i].c, l, c);
    if(!cmp)
      return;
    if(cmp > 0)
      break;
  }
  for(j = f->tamanho; j > i; j--) {
    f->pos[j].l = f->pos[j-1].l;
    f->pos[j].c = f->pos[j-1].c;
    f->pos[j].v = f->pos[j-1].v;
  }
  f->pos[i].l = l;
  f->pos[i].c = c;
  f->pos[i].v = v;
  f->tamanho++;
}

void copia_mapa(tmapa *mo, tmapa *md)  {
  int i, j;
  
  for(i = 0; i < md->nlinhas; i++)
    for(j = 0; j < md->ncolunas; j++)
      md->mapa[i][j] = mo->mapa[i][j];
}

tmapa * aloca_mapa(tmapa *mo) {
  int i;
  tmapa *md;
  md = (tmapa *) malloc(sizeof(tmapa));
  md->nlinhas = mo->nlinhas;
  md->ncolunas = mo->ncolunas;
  md->ncores = mo->ncores;
  md->mapa = (int**) malloc(md->nlinhas * sizeof(int*));
  for(i = 0; i < md->nlinhas; i++)
    md->mapa[i] = (int*) malloc(md->ncolunas * sizeof(int));
  return md;
}

void libera_mapa(tmapa *m)  {
  int i;
  
  for(i = 0; i < m->nlinhas; i++)
    free(m->mapa[i]);
  free(m->mapa);
  free(m);
}

void gera_mapa(tmapa *m, int semente) {
  int i, j;

  if(semente)
    srand(semente);  
  else
    srand(time(NULL));  
  m->mapa = (int**) malloc(m->nlinhas * sizeof(int*));
  for(i = 0; i < m->nlinhas; i++) {
    m->mapa[i] = (int*) malloc(m->ncolunas * sizeof(int));
    for(j = 0; j < m->ncolunas; j++)
      m->mapa[i][j] = 1 + rand() % m->ncores;
  }
}

void carrega_mapa(tmapa *m) {
  int i, j;

  scanf("%d", &(m->nlinhas));
  scanf("%d", &(m->ncolunas));
  scanf("%d", &(m->ncores));
  m->mapa = (int**) malloc(m->nlinhas * sizeof(int*));
  for(i = 0; i < m->nlinhas; i++) {
    m->mapa[i] = (int*) malloc(m->ncolunas * sizeof(int));
    for(j = 0; j < m->ncolunas; j++)
      scanf("%d", &(m->mapa[i][j]));
  }
}

void mostra_mapa(tmapa *m) {
  int i, j;
	
  printf("\n%d %d %d\n", m->nlinhas, m->ncolunas, m->ncores);
  for(i = 0; i < m->nlinhas; i++) {
    for(j = 0; j < m->ncolunas; j++)
      if(m->ncores > 10)
	printf("%02d ", m->mapa[i][j]);
      else
	printf("%d ", m->mapa[i][j]);
    printf("\n");
  }
}

void mostra_mapa_cor(tmapa *m) {
  int i, j;
  char* cor_ansi[] = { "\x1b[0m",
                       "\x1b[31m", "\x1b[32m", "\x1b[33m",
                       "\x1b[34m", "\x1b[35m", "\x1b[36m",
                       "\x1b[37m", "\x1b[30;1m", "\x1b[31;1m",
                       "\x1b[32;1m", "\x1b[33;1m", "\x1b[34;1m",
                       "\x1b[35;1m", "\x1b[36;1m", "\x1b[37;1m" };

  if(m->ncores > 15) {
    mostra_mapa(m);
    return;
  }
  printf("%d %d %d\n", m->nlinhas, m->ncolunas, m->ncores);
  for(i = 0; i < m->nlinhas; i++) {
    for(j = 0; j < m->ncolunas; j++)
      if(m->ncores >= 10)
        printf("%s%02d%s ", cor_ansi[m->mapa[i][j]], m->mapa[i][j], cor_ansi[0]);
      else
        printf("%s%d%s ", cor_ansi[m->mapa[i][j]], m->mapa[i][j], cor_ansi[0]);
    printf("\n");
  }
}

void pinta(tmapa *m, int l, int c, int fundo, int cor) {
  m->mapa[l][c] = cor;
  if(l < m->nlinhas - 1 && m->mapa[l+1][c] == fundo)
    pinta(m, l+1, c, fundo, cor);
  if(c < m->ncolunas - 1 && m->mapa[l][c+1] == fundo)
    pinta(m, l, c+1, fundo, cor);
  if(l > 0 && m->mapa[l-1][c] == fundo)
    pinta(m, l-1, c, fundo, cor);
  if(c > 0 && m->mapa[l][c-1] == fundo)
    pinta(m, l, c-1, fundo, cor);
}

void pinta_mapa(tmapa *m, int cor) {
  if(cor == m->mapa[0][0])
    return;
  pinta(m, 0, 0, m->mapa[0][0], cor);
}

void limpa_mapa(tmapa *m) {
  int i;
  int j;

  for(i = 0; i < m->nlinhas; i++)
    for(j = 0; j < m->ncolunas; j++)
      if(m->mapa[i][j] < 0)
	m->mapa[i][j] *= -1;
}

void fronteira(tmapa *m, int l, int c, int fundo, tfronteira *f) {
  if(m->mapa[l][c] == fundo) {
    m->mapa[l][c] *= -1;
    if(l < m->nlinhas - 1)
      fronteira(m, l+1, c, fundo, f);
    if(c < m->ncolunas - 1)
      fronteira(m, l, c+1, fundo, f);
    if(l > 0)
      fronteira(m, l-1, c, fundo, f);
    if(c > 0)
      fronteira(m, l, c-1, fundo, f);
  }
  else if(m->mapa[l][c] != -fundo) {
    insere_fronteira(f, l, c, m->mapa[l][c]);
  }
}

void fronteira_mapa(tmapa *m, tfronteira *f) {
  f->tamanho = 0;
  fronteira(m, 0, 0, m->mapa[0][0], f);
  limpa_mapa(m);
}


int * contar_maior_cores(tmapa * m){
	int i, j;
	int cores[m->ncores+1] = {};
	int index_maior=-1;
	int maior=0;
	int cor_0_0 = m->mapa[0][0];	
	int soma_dif = 0;
	for(i = 0;i<m->nlinhas;i++){
		for(j = 0;j<m->ncolunas;j++){
			cores[m->mapa[i][j]]++;
			if(cores[m->mapa[i][j]]>maior){
				index_maior=m->mapa[i][j];
				maior = cores[m->mapa[i][j]];
			}
			if(m->mapa[i][j]!=cor_0_0){
				soma_dif++;
			}
		}	
	}
	cores[0] = soma_dif;
	return cores;
}



void clrscr(){
    system("@cls||clear");
}
