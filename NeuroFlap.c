#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 900
#define HEIGHT 600
#define FPS 60
#define GRAVITY 0.5
#define POPULAÇÃO  200

float MOV_Y = 0.5;
float MOV_X = 0;
float SCROLL_IMAGEMFUNDO = 0.0f;
float POS_INICIAL_X = 150;
float POS_INICIAL_Y = HEIGHT / 2;

bool COLISÃO_CIMA = false;
bool COLISÃO_BAIXO = false;
bool GAME = true;
bool AG_MODE = true;

int SCORE = 0;
int X_TO_NEXTPIPE_UP = 0;
int X_TO_NEXTPIPE_DOWN = 0;
int GAP_PIPE = 0;
int INDIVÍDUO_ATUAL = 0;


double bias_output = 0;
double soma_hidden_layer = 0;
double FITNESS_SCORE = 0;
double output = 0;


struct pipes {
	float pipe_x;
	int altura_pipeteto;
	int altura_pipechão;
};

//------------------------------------------------

typedef struct {

	double weights[3];
	double weights_neurónios;
	
	double bias;
	double bias_output;

	double soma;
	
	double sigmoid;

} REDE_NEURAL;


typedef struct {
	
	double genes[26]; //pesos e bias
	double fitness; //Capacidade do mesmo - Distância percorrida
	

	bool VIVO;

} INDIVÍDUO;


void GERAÇÃO_PESOS_ALEATÓRIOS (double weights [], int NÚMERO_WEIGHTS) {


	for (int i = 0; i < NÚMERO_WEIGHTS; i++) {

		weights [i] = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
	}

}


double FUNÇÃO_SIGMOID (double x) {
	
	//return  x / (1 + fabs(x));
	double sigfunc = 1.0 / (1.0 + exp(-x));
	
	return (sigfunc);		
}


double REDE (double INPUT1_NORMALIZADO, double INPUT2_NORMALIZADO, double INPUT3_NORMALIZADO, double genes[26]) {

	double soma_hidden_layer = 0.0;
	int g = 0; 

	for (int i = 0; i < 5; i++) {
        
        	double soma_neuronio = INPUT1_NORMALIZADO * genes[g++] +
                               INPUT2_NORMALIZADO * genes[g++] +
                               INPUT3_NORMALIZADO * genes[g++];
        
        	soma_neuronio += genes[g++];
        
        	double sigmoid = FUNÇÃO_SIGMOID(soma_neuronio);
        
        	double peso_saida = genes[g++];
        
        	soma_hidden_layer += sigmoid * peso_saida;
        
        	double bias_saida = genes[25];
        	soma_hidden_layer += bias_saida;
   	 }

    	double output = FUNÇÃO_SIGMOID(soma_hidden_layer);
    	return output;






/*
	double soma_hidden_layer = 0.0;
	
	//Camada Oculta com 5 Neurónios na Hidden Layer || Cada Neurónio tem inicialmente 3 pesos porque temos 3 inputs.
	for (int i = 0; i < 5; i++) {
		neurónio[i].soma = INPUT1_NORMALIZADO * neurónio[i].weights[0] +
			   INPUT2_NORMALIZADO * neurónio[i].weights[1] +
			   INPUT3_NORMALIZADO * neurónio[i].weights[2] +
			   neurónio[i].bias;
	//Cálculo do Sigmoid que é o output que vai para o Neurónio Final
		neurónio[i].sigmoid = FUNÇÃO_SIGMOID (neurónio[i].soma);
	
		soma_hidden_layer = neurónio [i].sigmoid * neurónio[i].weights_neurónios;

	}

	soma_hidden_layer += neurónio[0].bias_output;
	//O resultado da Função Sigmoid da soma dos Sigmoids dos 5 Neurónio Escondidos é a decisão final. Se > 0.5 salta.
	double sigmoid_output = FUNÇÃO_SIGMOID (soma_hidden_layer);
	
	return sigmoid_output;
*/
}

	
	
void RESET_JOGO (struct pipes colunas []) {




	POS_INICIAL_X = 150;
	POS_INICIAL_Y = HEIGHT / 2;
	FITNESS_SCORE = 0;
	
	float pipe_x = 640; //POSIçÃO INICIAL DO PRIMEIRO DUO DE PIPES
        for (int i = 0; i < 50; i++) {

                int height1 = (int)((rand() / (double)RAND_MAX) * (300 - 170) + 170);
                int height2 = (int)((rand() / (double)RAND_MAX) * (200 - 150) + 150);


                int altura_pipeteto = height1;
                int altura_pipechão = height2;


                colunas[i].pipe_x = pipe_x;
                colunas[i].altura_pipeteto = altura_pipeteto;
                colunas[i].altura_pipechão = altura_pipechão;


                pipe_x += 250; //CRIA DUOS DE PIPES DE 200 EM 200
        }

}


void INICIAR_POPULAÇÃO (INDIVÍDUO indivíduos[200], REDE_NEURAL neurónio[5], int INDIVÍDUO_ATUAL) {
	
	int genes_indice = 0;

	for (int i = 0; i < 5; i++) {
   		for (int j = 0; j < 3; j++) {

			double val_random = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
                        neurónio[i].weights[j] = val_random;
                        indivíduos[INDIVÍDUO_ATUAL].genes[genes_indice++] = val_random;
                }

                        double val_random_2 = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
                        neurónio[i].bias = val_random_2;
                        indivíduos[INDIVÍDUO_ATUAL].genes[genes_indice++] = val_random_2;

                        double val_random_3 = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
                        neurónio[i].weights_neurónios = val_random_3;
                        indivíduos[INDIVÍDUO_ATUAL].genes[genes_indice++] = val_random_3;
	}
			double val_random_4 = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
			neurónio[0].bias_output = val_random_4;
			indivíduos[INDIVÍDUO_ATUAL].genes[genes_indice++] = val_random_4;
		
	
	indivíduos[INDIVÍDUO_ATUAL].fitness = 0;
	indivíduos[INDIVÍDUO_ATUAL].VIVO = true;
}




void main () {

	srand(time(NULL));
	
	InitWindow(WIDTH, HEIGHT, "Flappy Bird");
	SetTargetFPS(FPS);

	Texture2D BONECO = LoadTexture("Background/flappybird2.png");


	struct pipes colunas [50];
	float pipe_x = 640; //POSIçÃO INICIAL DO PRIMEIRO DUO DE PIPES
	for (int i = 0; i < 50; i++) {
		
		int height1 = (int)((rand() / (double)RAND_MAX) * (300 - 170) + 170);
	        int height2 = (int)((rand() / (double)RAND_MAX) * (200 - 150) + 150);


		int altura_pipeteto = height1;
		int altura_pipechão = height2;
		

		colunas[i].pipe_x = pipe_x;
		colunas[i].altura_pipeteto = altura_pipeteto;
		colunas[i].altura_pipechão = altura_pipechão;
		

		pipe_x += 250; //CRIA DUOS DE PIPES DE 200 EM 200
	}

	


	int HITBOX_BONECO_X = BONECO.width * 0.5 * 0.75; //LARGURA * ESCALA * METAD
	int HITBOX_BONECO_Y = BONECO.height * 0.5 * 0.71;

	
	REDE_NEURAL neurónio [5];
	INDIVÍDUO indivíduos [200];

	for (int i = 0; i < POPULAÇÃO; i++) {

		INICIAR_POPULAÇÃO(indivíduos, neurónio, i);
	
	}
	
	while (!WindowShouldClose()) {
	
		int NEXTPIPE = -1;

                     
		if (indivíduos[INDIVÍDUO_ATUAL].VIVO == true) {
					
				
			
			output = REDE (POS_INICIAL_Y / HEIGHT, X_TO_NEXTPIPE_UP / WIDTH, X_TO_NEXTPIPE_DOWN / WIDTH, indivíduos[INDIVÍDUO_ATUAL].genes);
			

			if (output > 0.5f && MOV_Y > 2.0f) {

				MOV_Y = -8.8f;
			}


			FITNESS_SCORE ++;
	
			MOV_Y += GRAVITY;
		        POS_INICIAL_Y += MOV_Y;

        		POS_INICIAL_X += MOV_X;

			Rectangle BONECOHITBOX = {
                                POS_INICIAL_X + (BONECO.width * 0.6 - HITBOX_BONECO_X) / 2,
                                POS_INICIAL_Y + (BONECO.height * 0.55 - HITBOX_BONECO_Y) / 2,
                                HITBOX_BONECO_X,
                                HITBOX_BONECO_Y
                        };


                        for (int j = 0; j < 50; j++) {


                                Rectangle PIPECIMA = {colunas[j].pipe_x, 0, 70, colunas[j].altura_pipeteto};
                                Rectangle PIPEBAIXO = {colunas[j].pipe_x + 2, (HEIGHT - colunas[j].altura_pipechão), 70, colunas[j].altura_pipechão};


                                COLISÃO_CIMA = CheckCollisionRecs (BONECOHITBOX, PIPECIMA);
                                COLISÃO_BAIXO = CheckCollisionRecs (BONECOHITBOX, PIPEBAIXO);


                                if (colunas[j].pipe_x + 70 >= POS_INICIAL_X && NEXTPIPE == -1) { //Enquanto o Boneco tiver atrás do Pipe, o NEXTPIPE vai ser sempre o mesmo índice, só reseta quando for maior que a POS_INICIAL_X
                                        NEXTPIPE = j;
                                }


                                if (COLISÃO_CIMA == true || COLISÃO_BAIXO == true)
                               	{

					indivíduos[INDIVÍDUO_ATUAL].VIVO = false;
					indivíduos[INDIVÍDUO_ATUAL].fitness = FITNESS_SCORE;
					RESET_JOGO(colunas);
					indivíduos[INDIVÍDUO_ATUAL++];			
					continue;


                              	}



                                if (GAME) {
                                        colunas[j].pipe_x -= 2.5;
					

                               	}

                       	}

                if (NEXTPIPE != -1) {

                        X_TO_NEXTPIPE_UP  = (colunas[NEXTPIPE].pipe_x + 70) - POS_INICIAL_X;

                        X_TO_NEXTPIPE_DOWN = (colunas[NEXTPIPE].pipe_x + 70) - POS_INICIAL_X;

                        GAP_PIPE = HEIGHT - (colunas[NEXTPIPE].altura_pipeteto + colunas[NEXTPIPE].altura_pipechão);

		}
	

		if (POS_INICIAL_Y > (HEIGHT - 50) || POS_INICIAL_Y <= 0) {

                        indivíduos[INDIVÍDUO_ATUAL].VIVO = false;
                        indivíduos[INDIVÍDUO_ATUAL].fitness = FITNESS_SCORE;
			RESET_JOGO(colunas);
                        indivíduos[INDIVÍDUO_ATUAL++];
                        continue;
                }
		
	}


		BeginDrawing();
		ClearBackground(SKYBLUE);


		DrawTextureEx(BONECO, (Vector2) {POS_INICIAL_X, POS_INICIAL_Y}, 0, 0.5,  WHITE);
	
		

		for (int i = 0; i < 50; i++) {
				
			DrawRectangle (colunas[i].pipe_x, 0, 70, colunas[i].altura_pipeteto, GREEN);
			DrawRectangle (colunas[i].pipe_x + 2, (HEIGHT - colunas[i].altura_pipechão), 70, colunas[i].altura_pipechão, GREEN);
		}

		
		DrawText(TextFormat("INDIVÍDUO = %i", INDIVÍDUO_ATUAL), 10, 20, 20, BLACK);
		DrawText(TextFormat("FITNESS SCORE = %lf", FITNESS_SCORE), 10, 40, 20, BLACK);
			


		EndDrawing();
	}

		UnloadTexture(BONECO);
		CloseWindow();

		
		for (int i = 0; i < INDIVÍDUO_ATUAL; i++) {

			printf ("%lf\n", indivíduos[i].fitness);

		}
					
}



//Uma Rede Neural, recebe os valores de entrada e depois processa esses valores nas camadas ocultas, nas camadas ocultas estão vários Neurónios que fazem todos o mesmo, multiplicam as entradas com os pesos gerados, adicionam o bias e aplicam a Função de Ativação.
//
//Após isso passam esse valor para a próxima camada, que faz exatamente o mesmo. Depois passa para a última camada que guess what ? vai fazer exatamente o mesmo mas como é apenas um neurónio dá apenas um valor e é esse valor que é processado para tomar uma decisão.
//
//Depois calcula-se a taxa de erro.
//
//Depois aplica-se um algoritmo. 




//ERRO NA HIDDEN LAYER, LOOP DE RANDOM NUMBERS
