#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define WIDTH 680
#define HEIGHT 680
#define FPS 60
#define GRAVITY 0.5
#define POPULAÇÃO  200

/*
float MOV_Y = 0.5f;
float MOV_X = 0.5f;
float SCROLL_IMAGEMFUNDO = 0.0f;
float POS_INICIAL_X = 150.0f;
float POS_INICIAL_Y = HEIGHT / 2;
*/

bool COLISÃO_CIMA = false;
bool COLISÃO_BAIXO = false;
bool NEXT_POP = false;

//int X_TO_NEXTPIPE = 0;
int GAP_PIPE = 180;
int NÚMERO_ELITES = 20;
int NÚMERO_DE_GENES = 26;
int GERAÇÃO = 0;

double bias_output = 0;
double output = 0;
double TAXA_MUTAÇÃO = 0.30;

int MORTES = 0;


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
    bool PODE_SALTAR;    

    float POS_INICIAL_Y;
    float POS_INICIAL_X;
    float MOV_Y;

    int X_TO_NEXTPIPE;



} INDIVÍDUO;


int COMPARAR_FITNESS (const void *a, const void *b) {
    
	INDIVÍDUO *ind1 = (INDIVÍDUO *)a;
   	INDIVÍDUO *ind2 = (INDIVÍDUO *)b;

    
    	if (ind1 -> fitness < ind2 -> fitness) return 1;
    	if (ind1 -> fitness > ind2 -> fitness) return -1;
    	return 0;
}


/*
void GERAÇÃO_PESOS_ALEATÓRIOS (double weights [], int NÚMERO_WEIGHTS) {


	for (int i = 0; i < NÚMERO_WEIGHTS; i++) {

		weights [i] = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
	}

}
*/

//Melhorar, existe uma versão mais rápida
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
        
        	bias_output = genes[25];
	}
		soma_hidden_layer += bias_output;
   	 

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

	
	
void RESET_JOGO (struct pipes colunas [50], INDIVÍDUO indivíduos [200]) {
    
    for (int i = 0; i < POPULAÇÃO; i++) {
	 
        indivíduos[i].POS_INICIAL_X = 150.0f;
	    indivíduos[i].POS_INICIAL_Y = ((double)rand() / RAND_MAX) * HEIGHT;
	    indivíduos[i].MOV_Y = 0.5;
	}


	float pipe_x = 640; //POSIçÃO INICIAL DO PRIMEIRO DUO DE PIPES
        for (int i = 0; i < 50; i++) {

		
                int pipe_cima_max = HEIGHT - GAP_PIPE - 50;

                int height1 = rand() % (pipe_cima_max - 50) + 50;

                int height2 = HEIGHT - height1 - GAP_PIPE; 


                int altura_pipeteto = height1;
                int altura_pipechão = height2;


                colunas[i].pipe_x = pipe_x;
                colunas[i].altura_pipeteto = altura_pipeteto;
                colunas[i].altura_pipechão = altura_pipechão;


                pipe_x += 300; //CRIA DUOS DE PIPES DE 200 EM 200
        }

}


void INICIAR_POPULAÇÃO (INDIVÍDUO indivíduos[200], REDE_NEURAL neurónio[5], int individuo) {
	
	int genes_indice = 0;

	for (int i = 0; i < 5; i++) {
   		for (int j = 0; j < 3; j++) {

			double val_random = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
                        neurónio[i].weights[j] = val_random;
                        indivíduos[individuo].genes[genes_indice++] = val_random;
                }

                        double val_random_2 = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
                        neurónio[i].bias = val_random_2;
                        indivíduos[individuo].genes[genes_indice++] = val_random_2;

                        double val_random_3 = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
                        neurónio[i].weights_neurónios = val_random_3;
                        indivíduos[individuo].genes[genes_indice++] = val_random_3;
	}
			double val_random_4 = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
			neurónio[0].bias_output = val_random_4;
			indivíduos[individuo].genes[genes_indice++] = val_random_4;
		
	
	indivíduos[individuo].fitness = 0;
	indivíduos[individuo].VIVO = true;
    indivíduos[individuo].PODE_SALTAR = true;
    indivíduos[individuo].POS_INICIAL_Y = ((double)rand() / RAND_MAX) * HEIGHT;
    indivíduos[individuo].POS_INICIAL_X = 150.0f;
    indivíduos[individuo].MOV_Y = 0.5;

}   


void FILHOS (INDIVÍDUO *PAI1, INDIVÍDUO *PAI2, INDIVÍDUO *FILHO, int NÚMERO_GENES, int TAXA_DE_MUTAÇÃO) {


	for (int i = 0; i < NÚMERO_GENES; i++) {

		FILHO -> genes [i] = (rand() % 2) ? PAI1 -> genes [i] : PAI2 -> genes[i];
	}


	for (int i = 0; i < NÚMERO_GENES; i++) {

		if (((double)rand() / RAND_MAX) < TAXA_DE_MUTAÇÃO) {

			FILHO -> genes [i] += ((double)rand() / RAND_MAX) * 2.0 - 1.0;	

		}
	}
}



void PRÓXIMA_GERAÇÃO (INDIVÍDUO indivíduos [], int NÚMERO_ELITES) {
	
	qsort (indivíduos, POPULAÇÃO, sizeof(INDIVÍDUO), COMPARAR_FITNESS); //ORGANIZA O ARRAY

	INDIVÍDUO próxima_população [POPULAÇÃO];

	for (int i = 0; i < NÚMERO_ELITES; i++) {

		próxima_população [i] = indivíduos[i];
	}

	
	for (int i = (NÚMERO_ELITES - 1); i < POPULAÇÃO; i++) {

		int PAI1 = rand() % (NÚMERO_ELITES - 1);
		int PAI2 = rand() % (NÚMERO_ELITES - 1);

		FILHOS (&indivíduos[PAI1], &indivíduos[PAI2], &próxima_população[i], NÚMERO_DE_GENES, TAXA_MUTAÇÃO);
	
	}

	for (int i = 0; i < POPULAÇÃO; i++) {

		indivíduos[i] = próxima_população[i];
		indivíduos[i].VIVO = true;
        indivíduos[i].fitness = 0;
        indivíduos[i].PODE_SALTAR = true;
        indivíduos[i].POS_INICIAL_Y = ((double)rand() / RAND_MAX) * HEIGHT;
        indivíduos[i].POS_INICIAL_X = 150.0f;
        indivíduos[i].MOV_Y = 0.5;
	}


}


void ALGORITMO_GENÉTICO (INDIVÍDUO indivíduos[200], int HITBOX_BONECO_X, int HITBOX_BONECO_Y, struct pipes colunas[50], Texture2D BONECO) {
        
    

    for (int i = 0; i < POPULAÇÃO; i++) {

        if (!indivíduos[i].VIVO) {
           
            continue;
        
        }
   
        int NEXTPIPE = -1;

        indivíduos[i].fitness ++;

        indivíduos[i].MOV_Y += GRAVITY;
        indivíduos[i].POS_INICIAL_Y += indivíduos[i].MOV_Y;


        

    



        Rectangle BONECOHITBOX = {
                                indivíduos[i].POS_INICIAL_X + (BONECO.width * 0.6 - HITBOX_BONECO_X) / 2,
                                indivíduos[i].POS_INICIAL_Y + (BONECO.height * 0.55 - HITBOX_BONECO_Y) / 2,
                                HITBOX_BONECO_X,
                                HITBOX_BONECO_Y
                        };

        
    
        for (int j = 0; j < 50; j++) {


            Rectangle PIPECIMA = {colunas[j].pipe_x, 0, 90, colunas[j].altura_pipeteto};
            Rectangle PIPEBAIXO = {colunas[j].pipe_x + 2, (HEIGHT - colunas[j].altura_pipechão), 90, colunas[j].altura_pipechão};



            COLISÃO_CIMA = CheckCollisionRecs (BONECOHITBOX, PIPECIMA);
            COLISÃO_BAIXO = CheckCollisionRecs (BONECOHITBOX, PIPEBAIXO);


            if (colunas[j].pipe_x + 90 >= indivíduos[i].POS_INICIAL_X && NEXTPIPE == -1) { //Enquanto o Boneco tiver atrás do Pipe, o NEXTPIPE vai ser sempre o mesmo índice, só reseta quando for maior que a POS_INICIAL_X
                NEXTPIPE = j;
            }



            if (COLISÃO_CIMA == true || COLISÃO_BAIXO == true) {

			    indivíduos[i].VIVO = false;
                indivíduos[i].MOV_Y = 0;
                //indivíduos[i].POS_INICIAL_X = 0;
                //indivíduos[i].POS_INICIAL_Y = 0;
                indivíduos[i].fitness = indivíduos[i].fitness;
                MORTES +=1;

            }
        } 


        if (NEXTPIPE != -1) {

            indivíduos[i].X_TO_NEXTPIPE = (colunas[NEXTPIPE].pipe_x + 90) - indivíduos[i].POS_INICIAL_X;

      	}


        if (indivíduos[i].POS_INICIAL_Y > (HEIGHT - 50) || indivíduos[i].POS_INICIAL_Y <= 0) {

            indivíduos[i].VIVO = false;
            indivíduos[i].MOV_Y = 0;
            //indivíduos[i].POS_INICIAL_X = 0;
            //indivíduos[i].POS_INICIAL_Y = 0;
            MORTES += 1;
            indivíduos[i].fitness = indivíduos[i].fitness;

		
                }


        output = REDE ((double) indivíduos[i].POS_INICIAL_Y / (double) HEIGHT, (double) indivíduos[i].X_TO_NEXTPIPE / (double) WIDTH, (double) GAP_PIPE / (double) HEIGHT, indivíduos[i].genes);

     
        bool deseja_saltar = (output > 0.5f);


        if (deseja_saltar && indivíduos[i].MOV_Y >= -0.1f) { 

            indivíduos[i].MOV_Y = -8.8f;
            }

        }

}


void main () {

	srand(time(NULL));
	
	InitWindow(WIDTH, HEIGHT, "NeuroFlap");
	SetTargetFPS(FPS);

	Texture2D BONECO = LoadTexture("Background/flappybird2.png");


	struct pipes colunas [50];
	float pipe_x = 640; //POSIçÃO INICIAL DO PRIMEIRO DUO DE PIPES
	for (int i = 0; i < 50; i++) {
	
		int pipe_cima_max = HEIGHT - GAP_PIPE - 50;

		int height1 = rand() % (pipe_cima_max - 50) + 50;

		int height2 = HEIGHT - height1 - GAP_PIPE; 


		int altura_pipeteto = height1;
		int altura_pipechão = height2;
		

		colunas[i].pipe_x = pipe_x;
		colunas[i].altura_pipeteto = altura_pipeteto;
		colunas[i].altura_pipechão = altura_pipechão;
		

		pipe_x += 300; //CRIA DUOS DE PIPES DE 200 EM 200
	}

	


	int HITBOX_BONECO_X = BONECO.width * 0.5 * 0.75; //LARGURA * ESCALA * METAD
	int HITBOX_BONECO_Y = BONECO.height * 0.5 * 0.71;

	
	REDE_NEURAL neurónio [POPULAÇÃO] [5];
	INDIVÍDUO indivíduos [200];

	for (int i = 0; i < POPULAÇÃO; i++) {

		INICIAR_POPULAÇÃO(indivíduos, neurónio [i], i);
	
	}


	

	while (!WindowShouldClose()) {


		ALGORITMO_GENÉTICO (indivíduos, HITBOX_BONECO_X, HITBOX_BONECO_Y, colunas, BONECO);

        for (int i = 0; i < 50; i++) {
           
             colunas[i].pipe_x -= 3;
		}		



       
      
	

		if (MORTES == POPULAÇÃO) {

			NEXT_POP = true;	
		}

		if (NEXT_POP == true) {

           
			PRÓXIMA_GERAÇÃO (indivíduos, NÚMERO_ELITES);
            RESET_JOGO (colunas, indivíduos);
			GERAÇÃO ++;
            MORTES = 0;
			NEXT_POP = false;
		}


		BeginDrawing();
		ClearBackground(SKYBLUE);

        for (int i = 0; i < POPULAÇÃO; i++) {
		    DrawTextureEx(BONECO, (Vector2) {indivíduos[i].POS_INICIAL_X, indivíduos[i].POS_INICIAL_Y}, 0, 0.5,  WHITE);
	}
		

		for (int i = 0; i < 50; i++) {
			
			DrawRectangle (colunas[i].pipe_x, 0, 90, colunas[i].altura_pipeteto, GREEN);
			DrawRectangle (colunas[i].pipe_x + 2, (HEIGHT - colunas[i].altura_pipechão), 90, colunas[i].altura_pipechão, GREEN);
		}

		


		DrawText(TextFormat("GERAÇÃO = %i", GERAÇÃO), 10, 80, 20, BLACK);		
		DrawText(TextFormat("MORTES = %i", MORTES), 10, 120, 20, BLACK);

		EndDrawing();
	}

		UnloadTexture(BONECO);
		CloseWindow();

        for (int i = 0; i < 26; i++) {
            printf("%lf || ", indivíduos[20].genes[i]);
            printf("%lf \n \n", indivíduos[80].genes[i]);
            
        }

        
        printf("\n \n \n%f ||", indivíduos[1].POS_INICIAL_Y);
        printf("%i", indivíduos[30].X_TO_NEXTPIPE);
		
    				
}



