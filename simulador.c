#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

FILE *zeroP1;
FILE *zeroP1Perda;
int qtdZero;

double lowerBound(double sucessos, double colisoes, double vazios)
{
	return 2*colisoes;
}

double EomLee(double sucessos, double colisoes, double vazios)
{
	double gama=2;
	double beta = 1 << 30; //Infinito
	double L = colisoes+sucessos+vazios;
	double eThreshold = 0.001;

	while(1)
	{
		double novo_beta = L/((gama*colisoes)+sucessos);
		double novo_gama = (1-exp(-1/novo_beta)) / (novo_beta * (1-(1+1/novo_beta)*exp(-1/novo_beta)));

		if(fabs(novo_gama - gama) < eThreshold)
		{
			gama = novo_gama;
			break;
		}
		else
		{
			gama = novo_gama;
			beta = novo_beta;
		}
	}

	return ceil(gama*colisoes);
}

double P1 (double sucessos,double colisoes,double vazios)
{
	double l = colisoes+sucessos+vazios;	
	double EN = colisoes+sucessos;
    double n;

	double alfa = (l-1)/l;
	double aux = (1-((1-alfa)*EN));

    if(l == EN)
    {
        n = lowerBound(sucessos,colisoes,vazios);
        qtdZero++;
    }
    else
        n = log10(aux)/log10(alfa);

    return ceil(n);
}

double P2(double sucessos,double colisoes,double vazios)
{
	double l = colisoes+sucessos+vazios;
	double N = log10(vazios/l)/log10(1-(1/l));

	if(vazios!=0)
		return ceil(N);
	else
		return ceil(2*colisoes);
}

double P3(double sucessos,double colisoes, double vazios)
{
	double l = colisoes+sucessos+vazios;
	double N;
	if(colisoes == l)
		N = ((12.047*(l-1))+2);
	else
		N = EomLee(sucessos,colisoes,vazios);

	if(ceil(fabs(N - sucessos)) ==0 )
        return N;
    else
        return ceil(fabs(N - sucessos));
}

void simulador(FILE *saida, double estimador(double,double,double),int qtdEtiquetas)
{
	int contador,contador2,contador3;
	int qtdEtiSimulacao;
	
	//Executa mil simulações
	for(contador=0; contador < 1000; contador++)
	{
		double colisoesTotal=0,vaziosTotal=0,slotsTotal=0;

		double colisoesQuadro=0,vaziosQuadro=0,sucessosQuadro=0;
		double tamanhoQuadro=0;

		//Enquanto tiver etiquetas
		for(qtdEtiSimulacao = qtdEtiquetas; qtdEtiSimulacao > 0; )
		{
			//No começo o tamanho do quadro é 64
			if(sucessosQuadro==0 && colisoesQuadro==0 && vaziosQuadro==0)
			{
				tamanhoQuadro = 64;
			}
			else
			{
				tamanhoQuadro = estimador(sucessosQuadro,colisoesQuadro,vaziosQuadro);
				sucessosQuadro=0;
				colisoesQuadro=0;
				vaziosQuadro=0;
			}

			slotsTotal += tamanhoQuadro;
			double *quadro = (double*)calloc(tamanhoQuadro,sizeof(double));

			for(contador3=0; contador3 < qtdEtiSimulacao; contador3++) //contador3 aqui serve como a etiqueta da vez
			{
				//É sorteado o slot para aquela tag
				quadro[rand()%(int)tamanhoQuadro]++;
			}

			for(contador3=0; contador3 < tamanhoQuadro; contador3++)
			{
				if(quadro[contador3]==1)
				{
					//Se so tiver uma tag naquele slot: sucesso
					sucessosQuadro++;
					qtdEtiSimulacao--;
				}
				else if(quadro[contador3]==0)
				{
					//Se não tiver tag naquele slot: vazio
					vaziosQuadro++;
					vaziosTotal++;
				}
				else
				{
					//Se tiver mais de uma tag no slot: colisão
					colisoesQuadro++;
					colisoesTotal++;
				}
			}
			free(quadro);
		}
		fprintf(saida, "%d %d %d %d\n",qtdEtiquetas,(int)slotsTotal,(int)vaziosTotal,(int)colisoesTotal);
		fprintf(zeroP1, "%d %d\n",qtdEtiquetas,(int)qtdZero);
		qtdZero=0;
	}
}

void simuladorComPerda(FILE *saida, double estimador(double,double,double), int qtdEtiquetas)
{
	int contador,contador2,contador3;
	int qtdEtiSimulacao;
	
	//Executa mil simulações
	for(contador=0; contador < 1000; contador++)
	{
		double colisoesTotal=0,vaziosTotal=0,slotsTotal=0,perdaTotal=0;

		double colisoesQuadro=0,vaziosQuadro=0,sucessosQuadro=0;
		double tamanhoQuadro=0;

		//Enquanto tiver etiquetas
		for(qtdEtiSimulacao = qtdEtiquetas; qtdEtiSimulacao > 0; )
		{
			//No começo o tamanho do quadro é 64
			if(sucessosQuadro==0 && colisoesQuadro==0 && vaziosQuadro==0)
			{
				tamanhoQuadro = 64;
			}
			else
			{
				tamanhoQuadro = estimador(sucessosQuadro,colisoesQuadro,vaziosQuadro);
				sucessosQuadro=0;
				colisoesQuadro=0;
				vaziosQuadro=0;
			}

			slotsTotal += tamanhoQuadro;
			double *quadro = (double*)calloc(tamanhoQuadro,sizeof(double));

			for(contador3=0; contador3 < qtdEtiSimulacao; contador3++)//contador3 aqui serve como a etiqueta da vez
			{
				//É sorteado o slot para aquela tag
				quadro[rand()%(int)tamanhoQuadro]++;
			}

			for(contador3=0; contador3 < tamanhoQuadro; contador3++)
			{
				if(quadro[contador3]==1)
				{
					//Se so tiver uma tag naquele slot: provavel sucesso
					int chancePerda = rand()%100;
					if(chancePerda >= 0 && chancePerda < 10) // se o número sorteado estiver entre 0 e 9, ocorre uma perda
					{
						perdaTotal++;
					}
					else
					{
						sucessosQuadro++;
					}
					qtdEtiSimulacao--;
				}
				else if(quadro[contador3]==0)
				{
					//Se não tiver tag naquele slot: vazio
					vaziosQuadro++;
					vaziosTotal++;
				}
				else
				{
					//Se tiver mais de uma tag no slot: provavel colisão
					int envio=0;
					while(quadro[contador3]>=0) //Roda enquanto tiver etiquetas naquele slot
					{
						int chancePerda = rand()%100;
						if(chancePerda >= 0 && chancePerda < 10)// se o número sorteado estiver entre 0 e 9, ocorre uma perda
						{
							perdaTotal++;
						}
						else
						{
							envio++; //Se não, essa tag conseguiu enviar
						}

						quadro[contador3]--; // Passa para a próxima tag do slot
					}
					if(envio==1) //Se só uma tag enviou: sucesso
					{
						sucessosQuadro++;
						qtdEtiSimulacao--;
					}
					else
					{
						//Se mais de uma tag enviou: colisao
						colisoesQuadro++;
						colisoesTotal++;
					}

					//Como a simulação roda para todas as tags, então tem que ter pelo menos uma colisão para os estimadores funcionarem.
					if(colisoesQuadro==0)
						colisoesQuadro++;
				}
			}
			free(quadro);
		}
		fprintf(saida, "%d %d %d %d %d\n",qtdEtiquetas,(int)slotsTotal,(int)vaziosTotal,(int)colisoesTotal,(int)perdaTotal);
		fprintf(zeroP1Perda, "%d %d\n",qtdEtiquetas,(int)qtdZero);
		qtdZero=0;
	}
}

int main(int argc, char const *argv[])
{
	srand(time(NULL));

	zeroP1 = fopen("zeroP1.txt","w");
	zeroP1Perda = fopen("zeroP1Perda.txt","w");

	FILE *logLowerbound = fopen("LowerBound.txt","w");
	FILE *logEomLee = fopen("EomLee.txt","w");
	FILE *logP3 = fopen("P3.txt","w");
	FILE *logP2 = fopen("P2.txt","w");
	FILE *logP1 = fopen("P1.txt","w");

	FILE *logLowerboundPerda = fopen("LowerBoundPerda.txt","w");
	FILE *logEomLeePerda = fopen("EomLeePerda.txt","w");
	FILE *logP3Perda = fopen("P3Perda.txt","w");
	FILE *logP2Perda = fopen("P2Perda.txt","w");
	FILE *logP1Perda = fopen("P1Perda.txt","w");

	int contador;

	//Os simuladores são executados pra cada quantidade de etiquetas
	for(contador=100; contador<=1000; contador+=100)
	{
		simulador(logLowerbound,lowerBound,contador);
		simulador(logEomLee,EomLee,contador);
		simulador(logP3,P3,contador);
		simulador(logP2,P2,contador);
		simulador(logP1,P1,contador);


		qtdZero=0;
		
		simuladorComPerda(logLowerboundPerda,lowerBound,contador);
		simuladorComPerda(logEomLeePerda,EomLee,contador);
		simuladorComPerda(logP3Perda,P3,contador);
		simuladorComPerda(logP2Perda,P2,contador);
		simuladorComPerda(logP1Perda,P1,contador);
	}
	return 0;
}