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
float POS_INICIAL_Y = 1;
float SCROLL_IMAGEMFUNDO = 0.0f;


bool COLISÃO_CIMA = false;
bool COLISÃO_BAIXO = false;
bool GAME_OVER = false;
bool GAME_MODE = false;

int SCORE = 0;
int X_TO_NEXTPIPE = 0;
int GAP_PIPE = 0;


struct pipes {
	float pipe_x;
	int altura_pipeteto;
	int altura_pipechão;
	bool score;
};

//------------------------------------------------

typedef struct {

	double input [3];
	double weights [3];

	double bias;

} NEURÓNIO;



double sigmoid_function (double x) {
	
	//return  x / (1 + fabs(x));

	return 1.0 / (1.0 + exp(-x));
}


void INICIO_NEURÓNIO (NEURÓNIO* neurónio) {

	for (int i = 0; i < 3; i++) {

		neurónio -> weights[i] = ((double)rand() / RAND_MAX) * 2.0 - 1.0;;

	}

	neurónio -> bias = ((double)rand() / RAND_MAX) * 2.0 - 1.0;;
}


void EXECUÇÃO_NEURÓNIO (NEURÓNIO* neurónio, float POS_INICIAL_Y, int X_TO_NEXTPIPE, int GAP_PIPE) {
	
	double sum = (POS_INICIAL_Y * neurónio -> weights[0]) + (X_TO_NEXTPIPE * neurónio -> weights [1]) + (GAP_PIPE * neurónio -> weights[2]) + neurónio -> bias;

	double output = sigmoid_function (sum);	
			
		if (output > 0.5) {
			if (MOV_Y > 0) {
                        	MOV_Y = -8.8; //***
               		}
		}

}



//---------------------------------------------
void main () {

	srand(time(NULL));

	NEURÓNIO neu;
	INICIO_NEURÓNIO(&neu);


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



	int HITBOX_BONECO_X = BONECO.width * 0.5 * 0.75; //LARGURA * ESCALA * METAD
	int HITBOX_BONECO_Y = BONECO.height * 0.5 * 0.71;

	

	while (!WindowShouldClose()) {
	
		int NEXTPIPE = -1;

		
		if (GAME_OVER == false && GAME_MODE == false) {

			MOV_Y += GRAVITY;
			POS_INICIAL_Y += MOV_Y;

					
				
			POS_INICIAL_X += MOV_X;


/*
			SCROLL_IMAGEMFUNDO -= 1.0f;
			if (SCROLL_IMAGEMFUNDO <= IMAGEMFUNDO.width) {
				SCROLL_IMAGEMFUNDO = 0;
			}
*/			
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
			
			if (NEXTPIPE != -1) {

                                X_TO_NEXTPIPE = (colunas[NEXTPIPE].pipe_x + 70) - POS_INICIAL_X;

                                GAP_PIPE = HEIGHT - (colunas[NEXTPIPE].altura_pipeteto + colunas[NEXTPIPE].altura_pipeteto);

                                }

			EXECUÇÃO_NEURÓNIO (&neu, POS_INICIAL_Y / HEIGHT, X_TO_NEXTPIPE / WIDTH , (GAP_PIPE - 150) / 150 );

					
				}
			}

		
			
		
		


		if (POS_INICIAL_Y > (HEIGHT - 50) || POS_INICIAL_Y <= 0) {

			GAME_OVER = true;
		}




		BeginDrawing();
		ClearBackground(SKYBLUE);


	//	DrawTextureV (IMAGEMFUNDO, (Vector2) {0, 0}, WHITE);
	//	DrawTextureV (IMAGEMFUNDO, (Vector2) {0 - SCROLL_IMAGEMFUNDO, 0 }, WHITE);


		DrawTextureEx(BONECO, (Vector2) {POS_INICIAL_X, POS_INICIAL_Y}, 0, 0.5,  WHITE);
	
		DrawText(TextFormat("POS_INICIAL_Y NORMALIZADA = %f", POS_INICIAL_Y / HEIGHT), 10, 160, 15, BLACK);
		DrawText(TextFormat("X_TO_NEXTPIPE NORMALIZADO = %f", X_TO_NEXTPIPE / 900.0), 10, 200, 15, BLACK);
		DrawText(TextFormat("GAP_PIPE NORMALIZADO = %f", (GAP_PIPE - 150) / 150.0), 10, 240, 15, BLACK);


		for (int i = 0; i < 50; i++) {
			
			DrawText(TextFormat("X_TO_NEXTPIPE = %d", X_TO_NEXTPIPE), 10, 40, 15, BLACK);
                        DrawText(TextFormat("PIPE_GAP = %d", GAP_PIPE), 10, 80, 15, BLACK);
                        DrawText(TextFormat("NEXTPIPE = %d", NEXTPIPE), 10, 120, 15, BLACK);

	
			DrawRectangle (colunas[i].pipe_x, 0, 70, colunas[i].altura_pipeteto, GREEN);
			DrawRectangle (colunas[i].pipe_x + 2, (HEIGHT - colunas[i].altura_pipechão), 70, colunas[i].altura_pipechão, GREEN);
		}

/*
		if (GAME_MODE == false) {
			DrawText("CLICK IN SPACE BAR", WIDTH / 4, HEIGHT / 2, 30, RED);
	      		}
		if (IsKeyPressed(KEY_SPACE)) {
                	GAME_MODE = true;
       			}
		
*/		
		if (GAME_MODE == false) {

		
			DrawText(TextFormat("SCORE: %i", SCORE), 10, 10, 20, BLACK);
			DrawText(TextFormat("FPS: %i", GetFPS()), WIDTH - 90, 10, 20, BLACK);
		}	

		if (GAME_OVER == true) {
			
			
			DrawText(TextFormat("GAME OVER"), WIDTH / 8.5, HEIGHT / 4, 80, RED);
			DrawText(TextFormat("SCORE: %i", SCORE), WIDTH / 2.8, HEIGHT / 1.9, 40, RED);
			
		}

		EndDrawing();
	}

//		UnloadTexture(IMAGEMFUNDO);
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





