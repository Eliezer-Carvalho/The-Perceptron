#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 900
#define HEIGHT 600
#define FPS 60
#define GRAVITY 0.5
#define POPULAÇÃO = 200

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



double bias_output = 0;
double soma_hidden_layer = 0;
double FITNESS_SCORE = 0;

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
	
	float MOV_Y;
	float POS_y = HEIGHT / 2;
	float POS_x = 150;

	bool VIVO = true;

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


double REDE (double INPUT1_NORMALIZADO, double INPUT2_NORMALIZADO, double INPUT3_NORMALIZADO, REDE_NEURAL neurónio[5], INDIVÍDUO indivíduo[INDIVÍDUO_ATUAL]) {

	int genes_indice = 0;

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 3; j++) {
			
			double val_random = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
			neurónio[i].weights[j] = val_random;
			indivíduo[INDIVÍDUO_ATUAL].genes[genes_indice++] = val_random;
		}

			double val_random_2 = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
			neurónio[i].bias = val_random_2;
			indivíduo[INDIVÍDUO_ATUAL].genes[genes_indice++] = val_random_2;

			double val_random_3 = ((double)rand() / RAND_MAX) * 2.0 - 1.0;	
			neurónio[i].weights_neurónios = val_random_3;
			indivíduo[INDIVÍDUO_ATUAL].genes[genes_indice++] = val_random_3;
		
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

	
	
void RESET_JOGO () {


	SCORE = 0;
	POS_INICIAL_X = 150;
	POS_INICIAL_Y = HEIGHT / 2;





	for (int i = 0; i < 50; i++) {

		colunas[i].pipe_x = 0;
	}




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
	INDIVÍDUO indivíduo_atual [200];

	while (!WindowShouldClose()) {
	
		int NEXTPIPE = -1;

                              

		for (int i = 0; i < POPULAÇÃO; i++) {
		
			
		if (indivíduo_atual[i].VIVO == true) {
					
			REDE (indivíduo_atual[i].POS_INICIAL_Y / HEIGHT, X_TO_NEXTPIPE_UP / WIDTH, X_TO_NEXTPIPE_DOWN / WIDTH, neurónio, indivíduo_atual[i]);
					
			FITNESS_SCORE +=;


			indivíduo_atual[i].MOV_Y += GRAVITY;
		        indivíduo_atual[i].POS_y += indivíduo_atual[i].MOV_Y;

        		  //      POS_INICIAL_X += MOV_X;

			Rectangle BONECOHITBOX = {
                                indivíduo_atual[i].POS_x + (BONECO.width * 0.6 - HITBOX_BONECO_X) / 2,
                                indivíduo_atual[i].POS_y + (BONECO.height * 0.55 - HITBOX_BONECO_Y) / 2,
                                HITBOX_BONECO_X,
                                HITBOX_BONECO_Y
                        };


                        for (int j = 0; j < 50; j++) {


                                Rectangle PIPECIMA = {colunas[j].pipe_x, 0, 70, colunas[j].altura_pipeteto};
                                Rectangle PIPEBAIXO = {colunas[j].pipe_x + 2, (HEIGHT - colunas[j].altura_pipechão), 70, colunas[j].altura_pipechão};


                                COLISÃO_CIMA = CheckCollisionRecs (BONECOHITBOX, PIPECIMA);
                                COLISÃO_BAIXO = CheckCollisionRecs (BONECOHITBOX, PIPEBAIXO);


                                if (colunas[j].pipe_x + 70 >= POS_INICIAL_X && NEXTPIPE == -1) { //Enquanto o Boneco tiver atrás do Pipe, o NEXTPIPE vai ser sempre o mesmo índice, só reseta quando for maior que a POS_INICIAL_X
                                        NEXTPIPE = i;
                                }


                                if (COLISÃO_CIMA == true || COLISÃO_BAIXO == true)
                               	{

					indivíduo_atual[i].VIVO = false;
					indivíduo_atual[i].fitness = FITNESS_SCORE;
					break;


                              	}


                                if (colunas[j].score == false && POS_INICIAL_X > colunas[i].pipe_x + 70) {
                                        SCORE += 10;
                                        colunas[j].score = true;
                               	}


                                if (GAME) {
                                        colunas[j].pipe_x -= 2.5;
					indivíduo_atual[i].fitness = FITNESS_SCORE;

                               	}

                       	}

                if (NEXTPIPE != -1) {

                        X_TO_NEXTPIPE_UP  = (colunas[NEXTPIPE].pipe_x + 70) - POS_INICIAL_X;

                        X_TO_NEXTPIPE_DOWN = (colunas[NEXTPIPE].pipe_x + 70) - POS_INICIAL_X;

                        GAP_PIPE = HEIGHT - (colunas[NEXTPIPE].altura_pipeteto + colunas[NEXTPIPE].altura_pipechão);

		}

			


                if (POS_INICIAL_Y > (HEIGHT - 50) || POS_INICIAL_Y <= 0) {

			indivíduo_atual[i].VIVO = false;
			indivíduo_atual[i].fitness = FITNESS_SCORE;
		        break;	
                }

					
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
