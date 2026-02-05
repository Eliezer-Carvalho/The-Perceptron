#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 900
#define HEIGHT 600
#define FPS 60
#define GRAVITY 0.5


float MOV_X = 0;
float MOV_Y = 0.5;
float POS_INICIAL_X = 150;
float POS_INICIAL_Y = HEIGHT / 2;
float SCROLL_IMAGEMFUNDO = 0.0f;


bool COLISÃO_CIMA = false;
bool COLISÃO_BAIXO = false;
bool GAME_OVER = false;
bool AG_MODE = true;

int SCORE = 0;
int X_TO_NEXTPIPE_UP = 0;
int X_TO_NEXTPIPE_DOWN = 0;
int GAP_PIPE = 0;


double bias_output = 0;
double soma_hidden_layer = 0;

struct pipes {
	float pipe_x;
	int altura_pipeteto;
	int altura_pipechão;
	bool score;
};

//------------------------------------------------

typedef struct {

	double weights[3];
	double weights_neurónios;
	
	double bias;

	double soma;
	
	double sigmoid;

} REDE_NEURAL;


typedef struct {
	
	double genes[26]; //pesos e bias
	double fitness; //Capacidade do mesmo - Distância percorrida


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


double REDE (double INPUT1, double INPUT2, double INPUT3, REDE_NEURAL neurónio[5]) {

	
	for (int i = 0; i < 3; i++) {

		neurónio[0].weights[i] = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
		neurónio[1].weights[i] = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
		neurónio[2].weights[i] = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
		neurónio[3].weights[i] = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
		neurónio[4].weights[i] = ((double)rand() / RAND_MAX) * 2.0 - 1.0;	
		
		for (int j = 0; j < 5; j++) {

			neurónio[j].bias = ((double)rand() / RAND_MAX) * 2.0 - 1.0;

			neurónio[j].weights_neurónios = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
		}
	}
	
	
	//Camada Oculta com 5 Neurónios na Hidden Layer || Cada Neurónio tem inicialmente 3 pesos porque temos 3 inputs.
	for (int i = 0; i < 5; i++) {
		neurónio[i].soma = INPUT1 * neurónio[i].weights[0] +
			   INPUT2 * neurónio[i].weights[1] +
			   INPUT3 * neurónio[i].weights[2] +
			   neurónio[i].bias;
	//Cálculo do Sigmoid que é o output que vai para o Neurónio Final
		neurónio[i].sigmoid = FUNÇÃO_SIGMOID (neurónio[i].soma);
	}


	bias_output = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
	//Por cada resultado Sigmoid multiplicamos por um novo peso e somamos um bias novo também
	soma_hidden_layer = neurónio [0].sigmoid * neurónio[0].weights_neurónios +
		     neurónio [1].sigmoid * neurónio[1].weights_neurónios +
		     neurónio [2].sigmoid * neurónio[2].weights_neurónios +
		     neurónio [3].sigmoid * neurónio[3].weights_neurónios +
		     neurónio [4].sigmoid * neurónio[4].weights_neurónios +
		     bias_output;

	//O resultado da Função Sigmoid da soma dos Sigmoids dos 5 Neurónio Escondidos é a decisão final. Se > 0.5 salta.
	double sigmoid_output = FUNÇÃO_SIGMOID (soma_hidden_layer);
	
	return sigmoid_output;

}

	
	
	
/*

double FORWARD_PROP (NEURÓNIO* neurónio, double INPUT1, double INPUT2, double INPUT3) {

	neurónio -> soma = INPUT1 * neurónio -> weights[0] +
	       		   INPUT2 * neurónio -> weights[1] + 
		    	   INPUT3 * neurónio -> weights[2] +
	       		   neurónio -> bias;

	neurónio -> sigmoid = FUNÇÃO_SIGMOID(neurónio -> soma);


	return neurónio -> sigmoid;

}


double FORWARD_PROP_OUTPUT (OUTPUT *x, double a, double b, double c, double d, double e) {

	x -> final_soma = a * x -> final_weights[0] + 
	       		    b * x -> final_weights[1] +
		    	    c * x -> final_weights[2] +
			    d * x -> final_weights[3] +
			    e * x -> final_weights[4] +
			    x -> final_bias;

	x -> sigmoid_output = FUNÇÃO_SIGMOID (x -> final_soma);

	return x -> sigmoid_output;
}
*/
//---------------------------------------------
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
		bool score = false;

		colunas[i].pipe_x = pipe_x;
		colunas[i].altura_pipeteto = altura_pipeteto;
		colunas[i].altura_pipechão = altura_pipechão;
		colunas[i].score = false;

		pipe_x += 250; //CRIA DUOS DE PIPES DE 200 EM 200
	}

	


	int HITBOX_BONECO_X = BONECO.width * 0.5 * 0.75; //LARGURA * ESCALA * METAD
	int HITBOX_BONECO_Y = BONECO.height * 0.5 * 0.71;

	
	REDE_NEURAL neurónio [5];


	while (!WindowShouldClose()) {
	
		int NEXTPIPE = -1;

		
		if (GAME_OVER == false) {



			
			MOV_Y += GRAVITY;
			POS_INICIAL_Y += MOV_Y;

					
				
			POS_INICIAL_X += MOV_X;


			Rectangle BONECOHITBOX = {
                                POS_INICIAL_X + (BONECO.width * 0.6 - HITBOX_BONECO_X) / 2,
                                POS_INICIAL_Y + (BONECO.height * 0.55 - HITBOX_BONECO_Y) / 2,
                                HITBOX_BONECO_X,
                                HITBOX_BONECO_Y
                        };
       		

			for (int i = 0; i < 50; i++) {

	
				Rectangle PIPECIMA = {colunas[i].pipe_x, 0, 70, colunas[i].altura_pipeteto};
				Rectangle PIPEBAIXO = {colunas[i].pipe_x + 2, (HEIGHT - colunas[i].altura_pipechão), 70, colunas[i].altura_pipechão};
				
				
				COLISÃO_CIMA = CheckCollisionRecs (BONECOHITBOX, PIPECIMA);
				COLISÃO_BAIXO = CheckCollisionRecs (BONECOHITBOX, PIPEBAIXO);


				if (colunas[i].pipe_x + 70 >= POS_INICIAL_X && NEXTPIPE == -1) { //Enquanto o Boneco tiver atrás do Pipe, o NEXTPIPE vai ser sempre o mesmo índice, só reseta quando for maior que a POS_INICIAL_X
                                        NEXTPIPE = i;
                                }
				

				if (COLISÃO_CIMA == true || COLISÃO_BAIXO == true) 
				{
				
					GAME_OVER = true;

				}
					
				
				if (colunas[i].score == false && POS_INICIAL_X > colunas[i].pipe_x + 70) {
					SCORE += 10;
					colunas[i].score = true;	
				}


				if (GAME_OVER == false) {
					colunas[i].pipe_x -= 2.5;
				}
		
			}
				
		if (NEXTPIPE != -1) {

                	X_TO_NEXTPIPE_UP  = (colunas[NEXTPIPE].pipe_x + 70) - POS_INICIAL_X;

			X_TO_NEXTPIPE_DOWN = (colunas[NEXTPIPE].pipe_x + 70) - POS_INICIAL_X;

                        GAP_PIPE = HEIGHT - (colunas[NEXTPIPE].altura_pipeteto + colunas[NEXTPIPE].altura_pipechão);

			}

		}
			
		
			if (POS_INICIAL_Y > (HEIGHT - 50) || POS_INICIAL_Y <= 0) {
					
					GAME_OVER = true;
                               }

		
			
		
			if (GAME_OVER == false && AG_MODE == true) {


				REDE (POS_INICIAL_Y / HEIGHT, X_TO_NEXTPIPE_UP / WIDTH, X_TO_NEXTPIPE_DOWN / WIDTH, neurónio);
	
/*				for (int i = 0; i < 5; i++) {
					
					printf("%lf || %lf || %lf\n", neurónio[i].weights[0], neurónio[i].weights[1], neurónio[i].weights[2]);
					printf("%lf\n", neurónio[i].bias);
					printf("%lf\n", neurónio[i].soma);
					printf("%lf\n", neurónio[i].sigmoid);

					printf("%lf\n\n", neurónio[i].weights_neurónios); 
				}

				printf("%lf\n", bias_output);
				printf("%lf\n", soma_hidden_layer);
				printf("%lf\n", sigmoid_output);

				AG_MODE = false;
*/
			}		



		BeginDrawing();
		ClearBackground(SKYBLUE);


		DrawTextureEx(BONECO, (Vector2) {POS_INICIAL_X, POS_INICIAL_Y}, 0, 0.5,  WHITE);
	
		

		for (int i = 0; i < 50; i++) {
				
			DrawRectangle (colunas[i].pipe_x, 0, 70, colunas[i].altura_pipeteto, GREEN);
			DrawRectangle (colunas[i].pipe_x + 2, (HEIGHT - colunas[i].altura_pipechão), 70, colunas[i].altura_pipechão, GREEN);
		}

		if (GAME_OVER == false) {

			DrawText(TextFormat("DISTÂNCIA_PIPE: %lf", (double) X_TO_NEXTPIPE_DOWN / (double) WIDTH), 10, 30, 20, BLACK);
			DrawText(TextFormat("FPS: %i", GetFPS()), WIDTH - 90, 10, 20, BLACK);
			DrawText(TextFormat("GAP_PIPE = %lf", ((double) GAP_PIPE - 150) / 150), 10, 50, 20, BLACK);
//			DrawText(TextFormat("OUTPUT: %lf", out), 10, 70, 20, BLACK);
		}

		EndDrawing();
	}

		UnloadTexture(BONECO);
		CloseWindow();

}



//Uma Rede Neural, recebe os valores de entrada e depois processa esses valores nas camadas ocultas, nas camadas ocultas estão vários Neurónios que fazem todos o mesmo, multiplicam as entradas com os pesos gerados, adicionam o bias e aplicam a Função de Ativação.
//
//Após isso passam esse valor para a próxima camada, que faz exatamente o mesmo. Depois passa para a última camada que guess what ? vai fazer exatamente o mesmo mas como é apenas um neurónio dá apenas um valor e é esse valor que é processado para tomar uma decisão.
//
//Depois calcula-se a taxa de erro.
//
//Depois aplica-se um algoritmo. 




//ERRO NA HIDDEN LAYER, LOOP DE RANDOM NUMBERS
