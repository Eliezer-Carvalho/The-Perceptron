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
bool GAME_MODE = false;
bool REDE_MODE = true;

int SCORE = 0;
int X_TO_NEXTPIPE = 0;
int GAP_PIPE = 0;
static int AÇÃO = 0;
int EPOCH = 0;
int NEW_RECORD = 0;

double TARGET = 0.55;
double TAXA_APRENDIZAGEM = 0.0001;

struct pipes {
	float pipe_x;
	int altura_pipeteto;
	int altura_pipechão;
	bool score;
};

//------------------------------------------------

typedef struct {

	double weights[3];

	double bias;

	double soma;
	double sigmoid;
	double derivada_sigmoid;

	double delta;

} NEURÓNIO;


typedef struct {

	double final_weights [5];
	double final_bias;
	double final_soma;
	
	double sigmoid_output;
	double derivada_sigmoid_output;
		
	double delta;

} OUTPUT;


void GERAÇÃO_PESOS_ALEATÓRIOS (NEURÓNIO* neurónio) {


	for (int i = 0; i < 3; i++) {

		neurónio -> weights [i] = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
	}

	neurónio -> bias = ((double)rand() / RAND_MAX) * 2.0 - 1.0;

}




double FUNÇÃO_SIGMOID (double x) {
	
	//return  x / (1 + fabs(x));
	double sigfunc = 1.0 / (1.0 + exp(-x));
	
	return (sigfunc);		
}

double DERIVADA_FUNÇÃO_SIGMOID (double y) {

	double derivada = y * (1 - y);

	return (derivada);
}


double FORWARD_PROP (NEURÓNIO* neurónio, double INPUT1, double INPUT2, double INPUT3) {

	neurónio -> soma = INPUT1 * neurónio -> weights[0] +
	       		   INPUT2 * neurónio -> weights[1] + 
		    	   INPUT3 * neurónio -> weights[2] +
	       		   neurónio -> bias;

	neurónio -> sigmoid = FUNÇÃO_SIGMOID(neurónio -> soma);

	neurónio -> derivada_sigmoid = DERIVADA_FUNÇÃO_SIGMOID (neurónio -> sigmoid);

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


double BACK_PROP_OUTPUT (OUTPUT *x, double TARGET) {

	x -> sigmoid_output = FUNÇÃO_SIGMOID (x -> final_soma);
	
	x -> derivada_sigmoid_output = DERIVADA_FUNÇÃO_SIGMOID (x -> sigmoid_output);

	x -> delta = (x -> sigmoid_output - TARGET) * x -> derivada_sigmoid_output;


	return x -> delta;
}


double DELTA_NEURÓNIO_HIDDEN_LAYER (NEURÓNIO* neurónio, double peso_para_output, double delta) {

	neurónio -> delta = neurónio -> derivada_sigmoid * peso_para_output * delta;
	

	return neurónio -> delta;
}

//---------------------------------------------
void main () {

	srand(time(NULL));
	
	NEURÓNIO neurónio_hidden_layer [5];

	for (int i = 0; i < 5; i++) {
		
		GERAÇÃO_PESOS_ALEATÓRIOS (&neurónio_hidden_layer[i]);

	}




	OUTPUT output;
	for (int i = 0; i < 5; i++) {
		
		output.final_weights [i] = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
	}

	output.final_bias = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
	

/*

	for (int i = 0; i < 5; i++) {

		double teste = DELTA_NEURÓNIO_HIDDEN_LAYER (&neurónio_hidden_layer [i], output.final_weights[i], output.delta);

		printf ("%lf\n", teste);
	}
	
*/	
	InitWindow(WIDTH, HEIGHT, "Flappy Bird");
	SetTargetFPS(FPS);

	//Texture2D IMAGEMFUNDO = LoadTexture("Background/Tropical mountains - Layer 1.png");
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

	
	void reset_game () {

        POS_INICIAL_Y = HEIGHT / 2;
        POS_INICIAL_X = 100;
        MOV_Y = 0;

	EPOCH += 1;
        AÇÃO = 0;
        GAME_OVER = false;

	if (SCORE > NEW_RECORD) {
		SCORE = NEW_RECORD;
	}

	SCORE = 0;
	TARGET = 0;

	float pipe_x = 640;

	for (int i = 0; i < 50; i++) {
        	colunas[i].pipe_x = pipe_x;
        	colunas[i].score = false;
        	colunas[i].altura_pipeteto = (int)((rand() / (double)RAND_MAX) * (300 - 170) + 170);
        	colunas[i].altura_pipechão = (int)((rand() / (double)RAND_MAX) * (200 - 150) + 150);
   
		pipe_x += 250;       
	}

}






	int HITBOX_BONECO_X = BONECO.width * 0.5 * 0.75; //LARGURA * ESCALA * METAD
	int HITBOX_BONECO_Y = BONECO.height * 0.5 * 0.71;

	

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
					reset_game();
				}
					
				
				if (colunas[i].score == false && POS_INICIAL_X > colunas[i].pipe_x + 70) {
					SCORE += 10;
					colunas[i].score = true;
					TARGET = 1;
				}


				if (GAME_OVER == false) {
					colunas[i].pipe_x -= 2.5;
					}
		
				}
		
				
			if (NEXTPIPE != -1) {

                                X_TO_NEXTPIPE = (colunas[NEXTPIPE].pipe_x + 70) - POS_INICIAL_X;

                                GAP_PIPE = HEIGHT - (colunas[NEXTPIPE].altura_pipeteto + colunas[NEXTPIPE].altura_pipeteto);

                                }
		

			
		
		
			
					
                        




			double ex1 = FORWARD_PROP (&neurónio_hidden_layer[0], (double)POS_INICIAL_Y / (double)HEIGHT, (double) X_TO_NEXTPIPE / (double)WIDTH, ((double)GAP_PIPE - 150) / 150 );
			double ex2 = FORWARD_PROP (&neurónio_hidden_layer[1], (double)POS_INICIAL_Y / (double)HEIGHT, (double) X_TO_NEXTPIPE / (double)WIDTH, ((double)GAP_PIPE - 150) / 150 );
			double ex3 = FORWARD_PROP (&neurónio_hidden_layer[2], (double)POS_INICIAL_Y / (double)HEIGHT, (double) X_TO_NEXTPIPE / (double)WIDTH, ((double)GAP_PIPE - 150) / 150 );
			double ex4 = FORWARD_PROP (&neurónio_hidden_layer[3], (double)POS_INICIAL_Y / (double)HEIGHT, (double) X_TO_NEXTPIPE / (double)WIDTH, ((double)GAP_PIPE - 150) / 150 );
			double ex5 = FORWARD_PROP (&neurónio_hidden_layer[4], (double)POS_INICIAL_Y / (double)HEIGHT, (double) X_TO_NEXTPIPE / (double)WIDTH, ((double)GAP_PIPE - 150) / 150 );
			

			double out = FORWARD_PROP_OUTPUT (&output, ex1, ex2, ex3, ex4, ex5);
			
			
			int mov = (out > 0.5);

			if (mov == 1 && AÇÃO == 0) {
				MOV_Y = -8.8;
			}

			AÇÃO = mov;

			double fim = BACK_PROP_OUTPUT(&output, TARGET);
			
			for (int i = 0; i < 5; i++) {

		                double teste = DELTA_NEURÓNIO_HIDDEN_LAYER (&neurónio_hidden_layer [i], output.final_weights[i], output.delta);
	
        		 
       				 }

		
			double inputs[3] = {POS_INICIAL_Y / HEIGHT, X_TO_NEXTPIPE / WIDTH, (GAP_PIPE - 150)/150};

			for (int i = 0; i < 5; i++) {
    				for (int j = 0; j < 3; j++) {
        				neurónio_hidden_layer[i].weights[j] -= TAXA_APRENDIZAGEM * neurónio_hidden_layer[i].delta * inputs[j];
    					}
    				neurónio_hidden_layer[i].bias -= TAXA_APRENDIZAGEM * neurónio_hidden_layer[i].delta;
				}


			for (int i = 0; i < 5; i++) {
			    output.final_weights[i] -= TAXA_APRENDIZAGEM  * output.delta * neurónio_hidden_layer[i].sigmoid;
				}
			output.final_bias -= TAXA_APRENDIZAGEM * output.delta;


			}
		
			
		
			if (POS_INICIAL_Y > (HEIGHT - 50) || POS_INICIAL_Y <= 0) {

                                reset_game();
                                }

		
			
		
		



		BeginDrawing();
		ClearBackground(SKYBLUE);


		DrawTextureEx(BONECO, (Vector2) {POS_INICIAL_X, POS_INICIAL_Y}, 0, 0.5,  WHITE);
	
		

		for (int i = 0; i < 50; i++) {
				
			DrawRectangle (colunas[i].pipe_x, 0, 70, colunas[i].altura_pipeteto, GREEN);
			DrawRectangle (colunas[i].pipe_x + 2, (HEIGHT - colunas[i].altura_pipechão), 70, colunas[i].altura_pipechão, GREEN);
		}

		if (GAME_OVER == false) {

			DrawText(TextFormat("EPOCH = %i", EPOCH), 10, 30, 20, BLACK);
			DrawText(TextFormat("SCORE: %i", SCORE), 10, 10, 20, BLACK);
			DrawText(TextFormat("FPS: %i", GetFPS()), WIDTH - 90, 10, 20, BLACK);
			DrawText(TextFormat("RECORD: %i", NEW_RECORD), 10, 50, 20, BLACK);
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
