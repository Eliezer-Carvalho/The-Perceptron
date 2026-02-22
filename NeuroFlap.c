#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 680
#define HEIGHT 680
#define FPS 60
#define GRAVITY 0.5
#define POPULAÇÃO 200
#define GAP_PIPE 150
#define NÚMERO_DE_GENES 31
#define NÚMERO_ELITES 20
#define TAXA_MUTAÇÃO 0.5


bool COLISÃO_CIMA = false;
bool COLISÃO_BAIXO = false;
bool NEXT_POP = false;

int GERAÇÃO = 0;
int MORTES = 0;

double bias_output = 0;
double output = 0;

float best = 0;



struct pipes {
	float pipe_x;
	int altura_pipeteto;
	int altura_pipechão;
};

//------------------------------------------------

typedef struct {

	double weights[4];
	double weights_neurónios;
	
	double bias;
	double bias_output;

	double soma;
	
	double sigmoid;

} REDE_NEURAL;


typedef struct {
	
	double genes[31]; //pesos e bias
	double fitness;
    double fitness_armazenado; //Capacidade do mesmo - Distância percorrida
	
	bool VIVO;
    bool PODE_SALTAR;    

    float POS_INICIAL_Y; //input1
    float POS_INICIAL_X;
    float MOV_Y; //input2
    float CENTRO_COORDENADA_PIPE; //input3
    
    int X_TO_NEXTPIPE; //input4



} INDIVÍDUO;


int COMPARAR_FITNESS (const void *a, const void *b) {
    
	INDIVÍDUO *ind1 = (INDIVÍDUO *)a;
   	INDIVÍDUO *ind2 = (INDIVÍDUO *)b;

    
    	if (ind1 -> fitness_armazenado < ind2 -> fitness_armazenado) return 1;
    	if (ind1 -> fitness_armazenado > ind2 -> fitness_armazenado) return -1;
    	return 0;
}



double FUNÇÃO_SIGMOID (double x) {
	
	//return  x / (1 + fabs(x));
	double sigfunc = 1.0 / (1.0 + exp(-x));
	
	return (sigfunc);		
}



double RELU (double x) {

    if (x < 0) {

        return 0.0;

    }

    return x;

}





double REDE(double in1, double in2, double in3, double in4, double genes[31])
{
    int g = 0;
    double soma_hidden_total = 0.0;

    for (int i = 0; i < 5; i++)
    {
        double soma = 0.0;

        soma += in1 * genes[g++];
        soma += in2 * genes[g++];
        soma += in3 * genes[g++];
        soma += in4 * genes[g++];
        soma += genes[g++];              

        double relu = RELU (soma);

        double peso_saida = genes[g++];

        soma_hidden_total += relu * peso_saida;
    }

    soma_hidden_total += genes[30]; // bias final


    double output = RELU (soma_hidden_total);

    return output;
}





	
	
void RESET_JOGO (struct pipes colunas [50]) {
    

    

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


                pipe_x += 400; //CRIA DUOS DE PIPES DE 200 EM 200
        }

}


int INICIAR_POPULAÇÃO (INDIVÍDUO indivíduos[200], REDE_NEURAL neurónio[5], int individuo) {
	
	int genes_indice = 0;

	for (int i = 0; i < 5; i++) {
   		for (int j = 0; j < 4; j++) {

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
   	indivíduos[individuo].fitness_armazenado = 0;

   	indivíduos[individuo].VIVO = true;
    indivíduos[individuo].PODE_SALTAR = true;
    indivíduos[individuo].POS_INICIAL_Y = ((double)rand() / RAND_MAX) * HEIGHT;
    indivíduos[individuo].POS_INICIAL_X = 150.0f;
    indivíduos[individuo].MOV_Y = 0.5;
    indivíduos[individuo].CENTRO_COORDENADA_PIPE = 0;
    indivíduos[individuo].X_TO_NEXTPIPE = 0;

   // return printf("%lf || %lf || %lf || %lf \n \n", val_random, val_random_2, val_random_3, val_random_4);

}   


void FILHOS(INDIVÍDUO *pai1, INDIVÍDUO *pai2, INDIVÍDUO *filho)
{
    for (int i = 0; i < 31; i++)
    {
        filho->genes[i] = (rand() % 2) ? pai1->genes[i] : pai2->genes[i];

        if ((double)rand() / RAND_MAX < TAXA_MUTAÇÃO)
        {
            filho->genes[i] += ((double)rand() / RAND_MAX) * 0.4 - 0.2;
        }
    }
}



void PRÓXIMA_GERAÇÃO(INDIVÍDUO indivíduos[])
{
    qsort(indivíduos, POPULAÇÃO, sizeof(INDIVÍDUO), COMPARAR_FITNESS);

    INDIVÍDUO nova_pop[POPULAÇÃO];

   
    for (int i = 0; i < NÚMERO_ELITES; i++)
        nova_pop[i] = indivíduos[i];


    for (int i = NÚMERO_ELITES; i < POPULAÇÃO; i++)
    {
        int p1 = rand() % NÚMERO_ELITES;
        int p2 = rand() % NÚMERO_ELITES;

        FILHOS(&nova_pop[p1], &nova_pop[p2], &nova_pop[i]);
    }

  
    for (int i = 0; i < POPULAÇÃO; i++)
    {
        indivíduos[i] = nova_pop[i];

        indivíduos[i].fitness = 0;
        indivíduos[i].VIVO = true;
        indivíduos[i].POS_INICIAL_Y = rand() % HEIGHT;
        indivíduos[i].POS_INICIAL_X = 150;

        indivíduos[i].fitness_armazenado = 0;
        indivíduos[i].CENTRO_COORDENADA_PIPE = 0;
        indivíduos[i].X_TO_NEXTPIPE = 0;
        indivíduos[i].MOV_Y = 0.5f;
    }
}


double ALGORITMO_GENÉTICO (INDIVÍDUO indivíduos[200], int HITBOX_BONECO_X, int HITBOX_BONECO_Y, struct pipes colunas[50], Texture2D BONECO) {
        
    

    for (int i = 0; i < POPULAÇÃO; i++) {

        if (indivíduos[i].VIVO == false) {
           
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
                //indivíduos[i].MOV_Y = 0;
                indivíduos[i].POS_INICIAL_X = -100.0f;
                //indivíduos[i].POS_INICIAL_Y += HEIGHT;
                indivíduos[i].fitness_armazenado = indivíduos[i].fitness;
                MORTES +=1;
                

            }



            indivíduos[i].CENTRO_COORDENADA_PIPE = colunas[j].altura_pipeteto + (GAP_PIPE / 2.0);
        } 


       if (NEXTPIPE != -1)
{
    indivíduos[i].X_TO_NEXTPIPE =
        (colunas[NEXTPIPE].pipe_x + 90) - indivíduos[i].POS_INICIAL_X;

    indivíduos[i].CENTRO_COORDENADA_PIPE =
        colunas[NEXTPIPE].altura_pipeteto + (GAP_PIPE / 2.0);
}


        if (indivíduos[i].POS_INICIAL_Y > (HEIGHT - 50) || indivíduos[i].POS_INICIAL_Y <= 0) {

            indivíduos[i].VIVO = false;
            //indivíduos[i].MOV_Y = 0;
            indivíduos[i].POS_INICIAL_X = -100.0f;
            //indivíduos[i].POS_INICIAL_Y += HEIGHT;
            MORTES += 1;
            indivíduos[i].fitness_armazenado = indivíduos[i].fitness;
            

		
                }


        double in1 = (double) indivíduos[i].POS_INICIAL_Y / (double) HEIGHT;
        double in2 = (double) indivíduos[i].X_TO_NEXTPIPE / (double) WIDTH;
        double in3 = (double) indivíduos[i].MOV_Y / (double) 10.0;  
        double in4 = ((double) indivíduos[i].CENTRO_COORDENADA_PIPE - (double) indivíduos[i].POS_INICIAL_Y) / (double) HEIGHT;

        output = REDE (in1, in2, in3, in4, indivíduos[i].genes);

        


          if (output > 0.0) {
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
		

		pipe_x += 400; //CRIA DUOS DE PIPES DE 200 EM 200
	}

	


	int HITBOX_BONECO_X = BONECO.width * 0.5 * 0.75; //LARGURA * ESCALA * METAD
	int HITBOX_BONECO_Y = BONECO.height * 0.5 * 0.71;

	
	REDE_NEURAL neurónio [5];
	INDIVÍDUO indivíduos [200];

	for (int i = 0; i < POPULAÇÃO; i++) {

		INICIAR_POPULAÇÃO(indivíduos, neurónio, i);
	
	}

    /*for (int i = 0; i < POPULAÇÃO; i++) {
            printf("REDE %i -------- \n \n", i);
            for (int j = 0; j < 31; j++) {
                printf("Pesos e Bias = %lf \n", indivíduos[i].genes[j]);
            }
           
        }

	*/

	while (!WindowShouldClose()) {


		ALGORITMO_GENÉTICO (indivíduos, HITBOX_BONECO_X, HITBOX_BONECO_Y, colunas, BONECO);

        for (int i = 0; i < 50; i++) {
           
             colunas[i].pipe_x -= 3;
		}		



       
      
    	

		if (MORTES >= POPULAÇÃO) {

			NEXT_POP = true;
            
            
            float last = best;


        for (int i = 0; i < POPULAÇÃO; i++) {
   
            if (indivíduos[i].fitness_armazenado > 150) {
       
                if (indivíduos[i].fitness_armazenado > best) {
                    best = indivíduos[i].fitness_armazenado;
                }
            }
        }


            if (best > last) {
    
                continue;
            }


            if (best < last) {
                best = last;            
                
            }
		}

		if (NEXT_POP == true) {

           
			PRÓXIMA_GERAÇÃO (indivíduos);          
            RESET_JOGO (colunas);
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
        DrawText(TextFormat("MELHOR FITNESS = %lf", best), 10, 160, 20, RED);
     


		EndDrawing();
	}

		UnloadTexture(BONECO);
		CloseWindow();

      




       	
}   

//MORREM SEM BATER ??
//TESTAR VALORES COM POPULAÇÃO PEQUENA


