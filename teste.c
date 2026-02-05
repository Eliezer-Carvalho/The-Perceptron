#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define WIDTH 900
#define HEIGHT 600
#define FPS 60
#define GRAVITY 0.5
#define POPULACAO 200
#define NUM_PIPES 50
#define ELITE 20
#define MUT_RATE 0.1

// ===================== ESTRUTURAS =====================

typedef struct {
    double weights[3];
    double bias;
} NEURONIO;

typedef struct {
    double weights[5];
    double bias;
} OUTPUT;

typedef struct {
    NEURONIO hidden[5];
    OUTPUT output;
    double fitness;
} INDIVIDUO;

typedef struct {
    float pipe_x;
    int altura_pipeteto;
    int altura_pipechao;
    bool score;
} PIPE;

// ===================== VARIÁVEIS =====================

INDIVIDUO populacao[POPULACAO];
INDIVIDUO nova_pop[POPULACAO];

PIPE colunas[NUM_PIPES];

float POS_X = 150;
float POS_Y;
float VEL_Y;

int X_TO_NEXTPIPE;
int GAP_PIPE;
int SCORE;
int individuo_atual = 0;
int geracao = 1;

// ===================== NN =====================

double sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));
}

double forward_neuron(NEURONIO *n, double a, double b, double c) {
    return sigmoid(
        a * n->weights[0] +
        b * n->weights[1] +
        c * n->weights[2] +
        n->bias
    );
}

double forward_output(OUTPUT *o, double h[5]) {
    double s = o->bias;
    for (int i = 0; i < 5; i++)
        s += h[i] * o->weights[i];
    return sigmoid(s);
}

// ===================== GA =====================

double randf() {
    return ((double)rand() / RAND_MAX) * 2.0 - 1.0;
}

void random_individuo(INDIVIDUO *i) {
    for (int h = 0; h < 5; h++) {
        for (int w = 0; w < 3; w++)
            i->hidden[h].weights[w] = randf();
        i->hidden[h].bias = randf();
        i->output.weights[h] = randf();
    }
    i->output.bias = randf();
    i->fitness = 0;
}

void init_pop() {
    for (int i = 0; i < POPULACAO; i++)
        random_individuo(&populacao[i]);
}

void sort_pop() {
    for (int i = 0; i < POPULACAO - 1; i++)
        for (int j = i + 1; j < POPULACAO; j++)
            if (populacao[j].fitness > populacao[i].fitness) {
                INDIVIDUO t = populacao[i];
                populacao[i] = populacao[j];
                populacao[j] = t;
            }
}

INDIVIDUO crossover(INDIVIDUO a, INDIVIDUO b) {
    INDIVIDUO f = a;
    for (int i = 0; i < 5; i++) {
        if (rand() % 2) f.hidden[i] = b.hidden[i];
        if (rand() % 2) f.output.weights[i] = b.output.weights[i];
    }
    if (rand() % 2) f.output.bias = b.output.bias;
    f.fitness = 0;
    return f;
}

void mutate(INDIVIDUO *i) {
    for (int h = 0; h < 5; h++) {
        for (int w = 0; w < 3; w++)
            if ((double)rand()/RAND_MAX < MUT_RATE)
                i->hidden[h].weights[w] += randf() * 0.3;
        if ((double)rand()/RAND_MAX < MUT_RATE)
            i->hidden[h].bias += randf() * 0.3;
    }
    if ((double)rand()/RAND_MAX < MUT_RATE)
        i->output.bias += randf() * 0.3;
}

void nova_geracao() {
    sort_pop();

    for (int i = 0; i < ELITE; i++)
        nova_pop[i] = populacao[i];

    for (int i = ELITE; i < POPULACAO; i++) {
        INDIVIDUO a = populacao[rand() % ELITE];
        INDIVIDUO b = populacao[rand() % ELITE];
        nova_pop[i] = crossover(a, b);
        mutate(&nova_pop[i]);
    }

    for (int i = 0; i < POPULACAO; i++)
        populacao[i] = nova_pop[i];

    geracao++;
    individuo_atual = 0;
}

// ===================== RESET JOGO =====================

void reset_jogo() {
    POS_Y = HEIGHT / 2;
    VEL_Y = 0;
    SCORE = 0;

    float pipe_x = 640;
    for (int i = 0; i < NUM_PIPES; i++) {
        colunas[i].pipe_x = pipe_x;
        colunas[i].altura_pipeteto = rand() % 130 + 170;
        colunas[i].altura_pipechao = rand() % 50 + 150;
        colunas[i].score = false;
        pipe_x += 250;
    }
}

// ===================== MAIN =====================

int main() {

    srand(time(NULL));
    init_pop();

    InitWindow(WIDTH, HEIGHT, "Flappy Bird - Genetic Algorithm");
    SetTargetFPS(FPS);

    Texture2D BONECO = LoadTexture("Background/flappybird2.png");

    reset_jogo();

    while (!WindowShouldClose()) {

        INDIVIDUO *ind = &populacao[individuo_atual];

        VEL_Y += GRAVITY;
        POS_Y += VEL_Y;

        int next = -1;
        for (int i = 0; i < NUM_PIPES; i++) {
            colunas[i].pipe_x -= 2.5;
            if (colunas[i].pipe_x + 70 > POS_X && next == -1)
                next = i;
        }

        if (next != -1) {
            X_TO_NEXTPIPE = (colunas[next].pipe_x + 70) - POS_X;
            GAP_PIPE = HEIGHT - (colunas[next].altura_pipeteto + colunas[next].altura_pipechao);
        }

        double in1 = POS_Y / HEIGHT;
        double in2 = (double)X_TO_NEXTPIPE / WIDTH;
        double in3 = ((double)GAP_PIPE - 150) / 150;

        double h[5];
        for (int i = 0; i < 5; i++)
            h[i] = forward_neuron(&ind->hidden[i], in1, in2, in3);

        double out = forward_output(&ind->output, h);
	static int tete = 0;
	int move = out > 0.5;

	if (move == 1 && tete == 0) VEL_Y = -8.8; tete = move;

        ind->fitness++;

        bool morreu = (POS_Y <= 0 || POS_Y >= HEIGHT - 40);
        for (int i = 0; i < NUM_PIPES; i++) {
            Rectangle cima = { colunas[i].pipe_x, 0, 70, colunas[i].altura_pipeteto };
            Rectangle baixo = { colunas[i].pipe_x, HEIGHT - colunas[i].altura_pipechao, 70, colunas[i].altura_pipechao };
            Rectangle bird = { POS_X, POS_Y, 30, 30 };
            if (CheckCollisionRecs(bird, cima) || CheckCollisionRecs(bird, baixo))
                morreu = true;
        }

        if (morreu) {
            individuo_atual++;
            if (individuo_atual >= POPULACAO)
                nova_geracao();
            reset_jogo();
        }

        BeginDrawing();
        ClearBackground(SKYBLUE);

        DrawTextureEx(BONECO, (Vector2){POS_X, POS_Y}, 0, 0.5, WHITE);

        for (int i = 0; i < NUM_PIPES; i++) {
            DrawRectangle(colunas[i].pipe_x, 0, 70, colunas[i].altura_pipeteto, GREEN);
            DrawRectangle(colunas[i].pipe_x, HEIGHT - colunas[i].altura_pipechao, 70, colunas[i].altura_pipechao, GREEN);
        }

        DrawText(TextFormat("GERACAO: %d", geracao), 10, 10, 20, BLACK);
        DrawText(TextFormat("INDIVIDUO: %d/%d", individuo_atual+1, POPULACAO), 10, 30, 20, BLACK);
        DrawText(TextFormat("FITNESS: %.0lf", ind->fitness), 10, 50, 20, BLACK);

        EndDrawing();
    }

    UnloadTexture(BONECO);
    CloseWindow();
    return 0;
}

