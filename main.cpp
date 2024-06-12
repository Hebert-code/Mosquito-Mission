#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <GL/glut.h>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define MAX_MOSQUITOS 1000

/*---------------------REFERENTES AO TIMER--------------------*/

int tempoDecorrido = 0;

/*---------------------REFERENTES AO TIMER--------------------*/

// Variáveis globais
int modo_jogo = 0;
int altura_tela = 700;
int largura_tela = 900;
int vidas = 4;
int score = 0;
float posicaoTelaX = 0.0f;
bool gameover = false;
int mouseLeftPressed = 0;
GLuint gameOverTexture;
GLuint telaQuiztextuta;
GLfloat cursorX, cursorY;

/*------------------REFERENTES AO SANGUE------------------------*/

GLuint sangueTextures[15];
int frameSangue = 0;
int delayAnimSangue = 3;
int contAnimSangue = 0;
bool colidiu = false;

/*------------------REFERENTES AO SANGUE------------------------*/

typedef struct jogador {
	GLfloat x, y;
	float velocidade;
	bool equipado;
} Jogador;

Jogador jogador = {-0.75f, -0.15f, 0.02f};

const float LIMITE_ESQUERDA = -1.0f;
const float LIMITE_DIREITA = 1.0f;

// Constantes para o tamanho do jogador
const float JOGADOR_SIZE_X = 0.2f;
const float JOGADOR_SIZE_Y = 0.2f;


/*--------------------REFERENTES AO JOGADOR---------------------*/

int direcaoJogador = 0; // 0 = direita, 1 = esquerda;
int frameJogador = 0; //índice da textura atual
int delayAnim = 7; //controle de tempo para a animação;
int contAnim = 0;
bool jogadorMove = false;
GLuint jogadorDireitaTextures[6]; //direita
GLuint jogadorEsquerdaTextures[6]; //esquerda

/*--------------------REFERENTES AO JOGADOR---------------------*/


//texturas da tela inicial
GLuint telaInicioTexture;
GLuint telaEscolhaID[10];
GLuint botaoAventura, botaoQuiz;

//texturas do modo aventura
GLuint aventuraTextures[14];
GLuint coracaoTexture;

/*------------------REFERENTES AO ITEM----------------------*/

typedef struct item {
	GLfloat x, y;
	int dano;
	bool ativo;
} Item;

/*------------------REFERENTES AO ITEM----------------------*/


bool showMenu = true;

typedef struct mosquito {
	
	GLfloat x, y;
	float velocidade;
	int vida;
	GLfloat dirX, dirY;
	
} Mosquito;

/*-------------------REFERENTES AO MOSQUITO---------------------*/
std::vector<Mosquito> mosquitos;
GLuint mosquitoTextures[4];
int frameMosquito = 0;
int contAnimMosquito = 0;
int delayAnimMosquito = 7;
int numMosquitos = 0;
/*-------------------REFERENTES AO MOSQUITO---------------------*/

/*------------------------------------------CARREGANDO IMAGENS----------------------------------------*/

GLuint carregarTextura(const char* filename) {
	
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (data) {
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, (nrChannels == 4 ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        return texture;
    } else {
        std::cerr << "Failed to load texture: " << filename << std::endl;
        return 0;
    }
}


void reiniciarJogo() {
    gameover = false;
    score = 0;
    vidas = 4;
    mosquitos.clear();

    // Reinicia a posição dos mosquitos
    for (int i = 0; i < mosquitos.size(); ++i) {
        Mosquito m;
        m.x = -1.0f + static_cast<float>(rand()) / (RAND_MAX / 2.0f);
        m.y = -1.0f + static_cast<float>(rand()) / (RAND_MAX / 2.0f);
        mosquitos.push_back(m);
    }

    glutPostRedisplay();
}
/*------------------------------------------CARREGANDO IMAGENS----------------------------------------*/

void verificaBordasTela() {
    if (jogador.x - JOGADOR_SIZE_X / 2 < LIMITE_ESQUERDA) {
        jogador.x = LIMITE_ESQUERDA + JOGADOR_SIZE_X / 2;
    }
    if (jogador.x + JOGADOR_SIZE_X / 2 > LIMITE_DIREITA) {
        jogador.x = LIMITE_DIREITA - JOGADOR_SIZE_X / 2;
    }
}


void fimJogo() {
	gameover = true;	
}


void desenharTexto(const std::string& texto) {
    glColor3f(1.0f, 1.0f, 1.0f); // Cor branca para o texto
    
    // Ajusta a posição do texto (mais para baixo)
    glRasterPos2f(-0.09f, -0.3f); // Posição na tela

    // Define o tamanho da fonte
    int fontSize = 24;
    glPointSize(fontSize);

    // Desenha o texto
    for (size_t i = 0; i < texto.length(); ++i) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, texto[i]);
    }

    glutSwapBuffers(); // Atualiza a tela
}


void desenharGameOver() {
    if (gameover && modo_jogo == 3) {
        glClear(GL_COLOR_BUFFER_BIT);

        // Desenha a textura de tela de início
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, gameOverTexture);

        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, -1.0f); // Inferior esquerdo
        glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f, -1.0f); // Inferior direito
        glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f,  1.0f); // Superior direito
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f,  1.0f); // Superior esquerdo
        glEnd();

        glDisable(GL_TEXTURE_2D);

        // Desenha a pontuação
        std::stringstream ss;
        ss << "Score: " << score;

        // Desenha o texto diretamente na tela
        desenharTexto(ss.str());
    }
}




/*------------------------------------------DESENHANDO A TELA INICIAL----------------------------------------*/

void desenha_tela_escolha() {

    // Limpar a tela com um fundo branco
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Branco
    glClear(GL_COLOR_BUFFER_BIT);

    // Desenhar a textura de fundo telaInicioTexture
    glBindTexture(GL_TEXTURE_2D, telaInicioTexture);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, 1.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    

    // Desenhar botaoAventura (tamanho reduzido e mais centralizado)
    glBindTexture(GL_TEXTURE_2D, botaoAventura);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-0.25f, -0.15f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(0.25f, -0.15f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(0.25f, 0.1f);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-0.25f, 0.1f);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // Desenhar botaoQuiz (tamanho reduzido e mais centralizado)
    glBindTexture(GL_TEXTURE_2D, botaoQuiz);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-0.25f, -0.45f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(0.25f, -0.45f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(0.25f, -0.2f);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-0.25f, -0.2f);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    glutSwapBuffers();
}

/*------------------------------------------DESENHANDO A TELA INICIAL----------------------------------------*/

/*--------------------------------------------------ATUALIZA TIMER---------------------------------------------*/

void atualizaTempo(int value) {
    if (modo_jogo == 1) {
        tempoDecorrido++; // Incrementa o tempo decorrido a cada segundo
        glutPostRedisplay(); // Marca a janela para redesenho
    }
    glutTimerFunc(1000, atualizaTempo, 0); // Configura a função para ser chamada novamente após 1 segundo
}

/*---------------------------------------------------ATUALIZA TIMER-------------------------------------------*/



/*------------------------------------------DESENHANDO OS CORAÇÕES----------------------------------------*/

void desenhaVidas() {
	
	float heartSize = 0.1f; // Tamanho do coração em coordenadas do mundo
    float spacing = 0.001f; // Espaço entre os corações

    glBindTexture(GL_TEXTURE_2D, coracaoTexture);
    glEnable(GL_TEXTURE_2D);

    for (int i = 0; i < vidas; ++i) {
        float x = -1.0f + (heartSize + spacing) * i + spacing + 0.075;
        float y = 0.9f;

        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(x + heartSize, y, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(x + heartSize, y - heartSize, 0.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y - heartSize, 0.0f);
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);
}
/*------------------------------------------DESENHANDO OS CORAÇÕES----------------------------------------*/

/* --------------------------------------DESENHA JOGADOR--------------------------------------------- */

void desenhaJogador() {
	
	verificaBordasTela();
	
    GLuint* jogadorTextures = direcaoJogador == 0 ? jogadorDireitaTextures : jogadorEsquerdaTextures;

    // Desenhar o jogador com base na direção e no frame atual
    glBindTexture(GL_TEXTURE_2D, jogadorTextures[frameJogador]);
    glEnable(GL_TEXTURE_2D);

    // Definir o tamanho do jogador e sua posição
    float jogadorSizeX = 0.2f; // Tamanho do jogador em coordenadas do mundo no eixo X
    float jogadorSizeY = 0.2f; // Tamanho do jogador em coordenadas do mundo no eixo Y
    
    float yOffset = -0.45f; //controlo a coordenada y do jogador!

    // Desenhar o jogador
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(jogador.x - jogadorSizeX / 2, jogador.y + jogadorSizeY / 2 + yOffset, 0.0f); // Canto superior esquerdo
    glTexCoord2f(1.0f, 0.0f); glVertex3f(jogador.x + jogadorSizeX / 2, jogador.y + jogadorSizeY / 2 + yOffset, 0.0f); // Canto superior direito
    glTexCoord2f(1.0f, 1.0f); glVertex3f(jogador.x + jogadorSizeX / 2, jogador.y - jogadorSizeY / 2 + yOffset, 0.0f); // Canto inferior direito
    glTexCoord2f(0.0f, 1.0f); glVertex3f(jogador.x - jogadorSizeX / 2, jogador.y - jogadorSizeY / 2 + yOffset, 0.0f); // Canto inferior esquerdo
    glEnd();

    glDisable(GL_TEXTURE_2D);
}
/* --------------------------------------DESENHA JOGADOR--------------------------------------------- */

/*---------------------------------ATUALIZA OS MOVIMENTOS DOS MOSQUITOS------------------------------*/

void atualizarMosquitos(int value) {
    // Gere coordenadas aleatórias dentro dos limites da tela
    float posX = -1.0f + static_cast<float>(rand()) / (RAND_MAX / 0.5f) - 1.0f;
    float posY = -1.0f + static_cast<float>(rand()) / (RAND_MAX / 0.5f) - 1.0f;

    // Adicione um novo mosquito ao vetor com a posição aleatória
    Mosquito novoMosquito;
    novoMosquito.x = posX;
    novoMosquito.y = posY;
    novoMosquito.velocidade = 0.007f;
    novoMosquito.vida = 100;
    novoMosquito.dirX = 0.0f;
    novoMosquito.dirY = 0.0f;
    mosquitos.push_back(novoMosquito);

    // Redefina o temporizador para chamar essa função novamente após 1 segundo
    glutTimerFunc(1000, atualizarMosquitos, 0);
}


void atualizaMovimentoMosquitos() {
	
    for (size_t i = 0; i < mosquitos.size(); ++i) {
        // Atualize a posição de cada mosquito
        GLfloat dx = jogador.x - mosquitos[i].x;
        GLfloat dy = jogador.y - mosquitos[i].y;
        GLfloat distance = sqrt(dx * dx + dy * dy);

        mosquitos[i].dirX = dx / distance;
        mosquitos[i].dirY = dy / distance;

        mosquitos[i].x += mosquitos[i].dirX * mosquitos[i].velocidade;
        mosquitos[i].y += mosquitos[i].dirY * mosquitos[i].velocidade;

        // Verificar bordas da tela
        if (mosquitos[i].x > 1.0f) mosquitos[i].x = 1.0f;
        if (mosquitos[i].x < -1.0f) mosquitos[i].x = -1.0f;
        if (mosquitos[i].y > 1.0f) mosquitos[i].y = 1.0f;
        if (mosquitos[i].y < -1.0f) mosquitos[i].y = -1.0f;
    }
}

/*---------------------------------ATUALIZA OS MOVIMENTOS DOS MOSQUITOS------------------------------*/


/*---------------------------------------DESENHA MOSQUITO---------------------------------------------*/

void desenhaMosquito() {
	
    for (size_t i = 0; i < mosquitos.size(); ++i) {
        glBindTexture(GL_TEXTURE_2D, mosquitoTextures[frameMosquito]);
        glEnable(GL_TEXTURE_2D);

        // Obter as coordenadas do mosquito atual do vetor
        float mosquitoX = mosquitos[i].x;
        float mosquitoY = mosquitos[i].y;

        // Definir as coordenadas dos vértices para desenhar o mosquito
        float mosquitoSize = 0.15f; // Tamanho do mosquito em coordenadas do mundo
        float offsetY = -0.2f;

        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(mosquitoX, mosquitoY + offsetY); // Canto superior esquerdo (coordenada de textura invertida)
        glTexCoord2f(1.0f, 0.0f); glVertex2f(mosquitoX + mosquitoSize, mosquitoY + offsetY); // Canto superior direito (coordenada de textura invertida)
        glTexCoord2f(1.0f, 1.0f); glVertex2f(mosquitoX + mosquitoSize, mosquitoY - mosquitoSize + offsetY); // Canto inferior direito (coordenada de textura invertida)
        glTexCoord2f(0.0f, 1.0f); glVertex2f(mosquitoX, mosquitoY - mosquitoSize + offsetY); // Canto inferior esquerdo (coordenada de textura invertida)
        glEnd();

        glDisable(GL_TEXTURE_2D);
    }
}


/*---------------------------------------DESENHA MOSQUITO---------------------------------------------*/

/*---------------------------------DESENHA OS FRAMES DO SANGUE------------------------------*/

void desenhaSangue() {
    // Definir o tamanho do sangue
    float sangueSizeX = 0.3f; // Tamanho do sangue em coordenadas do mundo no eixo X
    float sangueSizeY = 0.3f; // Tamanho do sangue em coordenadas do mundo no eixo Y

    // Desenhar o sangue na posição do jogador com um offset em Y
    glBindTexture(GL_TEXTURE_2D, sangueTextures[frameSangue]);
    glEnable(GL_TEXTURE_2D);
    
    float ySangueOffset = -0.47f;
    float xSangueOffset = -0.05;

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(jogador.x - sangueSizeX / 2 + xSangueOffset, jogador.y + sangueSizeY / 2 + ySangueOffset, 0.0f); // Canto superior esquerdo
    glTexCoord2f(1.0f, 0.0f); glVertex3f(jogador.x + sangueSizeX / 2 + xSangueOffset, jogador.y + sangueSizeY / 2 + ySangueOffset, 0.0f); // Canto superior direito
    glTexCoord2f(1.0f, 1.0f); glVertex3f(jogador.x + sangueSizeX / 2 + xSangueOffset, jogador.y - sangueSizeY / 2 + ySangueOffset, 0.0f); // Canto inferior direito
    glTexCoord2f(0.0f, 1.0f); glVertex3f(jogador.x - sangueSizeX / 2 + xSangueOffset, jogador.y - sangueSizeY / 2 + ySangueOffset, 0.0f); // Canto inferior esquerdo
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void atualizaSangue() {
    contAnimSangue++;
    if (contAnimSangue >= delayAnimSangue) {
        contAnimSangue = 0;
        frameSangue = (frameSangue + 1) % 15; // Avança para o próximo frame de animação
    }
}



/*---------------------------------DESENHA OS FRAMES DO SANGUE------------------------------*/

struct Poder {
    GLfloat x;
    GLfloat y;
    GLfloat dirX;
    GLfloat dirY;
    GLfloat velocidade;
    int ativo;
};



std::vector<Poder> poderes;

void desenhaPoder() {
	
	if(modo_jogo == 1) {
	
		glColor3f(0.0f, 1.0f, 0.0f); // Cor verde
	    float poderSize = 0.015f; // Tamanho do poder em coordenadas do mundo
	
	    for (std::vector<Poder>::iterator it = poderes.begin(); it != poderes.end(); ++it) {
	        Poder& poder = *it;
	        if (poder.ativo) {
	            // Atualizar a posição do poder
	            poder.x += poder.dirX * poder.velocidade;
	            poder.y += poder.dirY * poder.velocidade;
	            
	            float poderYOffset = -0.65f;
	
	            // Desenhar o poder (quadrado verde)
	            glBegin(GL_QUADS);
	            glVertex3f(poder.x - poderSize / 2, poder.y + poderSize + poderYOffset/ 2, 0.0f); // Canto superior esquerdo
	            glVertex3f(poder.x + poderSize / 2, poder.y + poderSize + poderYOffset/ 2, 0.0f); // Canto superior direito
	            glVertex3f(poder.x + poderSize / 2, poder.y - poderSize + poderYOffset/ 2, 0.0f); // Canto inferior direito
	            glVertex3f(poder.x - poderSize / 2, poder.y - poderSize + poderYOffset/ 2, 0.0f); // Canto inferior esquerdo
	            glEnd();
	        }
	    }
	}
}

/* --------------------------------------DESENHA MODO AVENTURA--------------------------------------------- */

void desenhaAventura() {

    // Limpar a tela com um fundo branco
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glColor3f(1.0f, 1.0f, 1.0f); // Cor branca
	
    // Configurar a textura do céu
    glBindTexture(GL_TEXTURE_2D, aventuraTextures[1]); // Usar a textura do céu
    glEnable(GL_TEXTURE_2D);

    // Definir o tamanho do tile e a área do céu
    float skySizeX = 1.0f; // Tamanho da área do céu em coordenadas do mundo no eixo X
    float skySizeY = 1.0f;  // Tamanho da área do céu em coordenadas do mundo no eixo Y (ajustado para preencher até o chão)

    // Desenhar o céu com a textura (coordenadas ajustadas)
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-skySizeX, skySizeY, 0.0f); // Canto superior esquerdo
    glTexCoord2f(1.0f, 0.0f); glVertex3f(skySizeX, skySizeY, 0.0f);  // Canto superior direito
    glTexCoord2f(1.0f, 1.0f); glVertex3f(skySizeX, -0.75f, 0.0f);    // Canto inferior direito (ajustado para preencher até o chão)
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-skySizeX, -0.75f, 0.0f);   // Canto inferior esquerdo (ajustado para preencher até o chão)
    glEnd();

    glDisable(GL_TEXTURE_2D);
    
	// Configurar a textura do chão

	glBindTexture(GL_TEXTURE_2D, aventuraTextures[0]); // Usar a textura do chão
	glEnable(GL_TEXTURE_2D);
	
	// Definir o tamanho do tile e a área do chão
	float tileSizeX = 0.1f; // Tamanho de cada tile em coordenadas de textura no eixo X
	float tileSizeY = 0.1f; // Tamanho de cada tile em coordenadas de textura no eixo Y
	float groundSizeX = 2.0f; // Tamanho da área do chão em coordenadas do mundo no eixo X
	float groundSizeY = 0.3f; // Tamanho da área do chão em coordenadas do mundo no eixo Y
	
	// Ajustar a posição y para mover o chão mais para baixo
	float offsetY = -1.0f;
	
	// Desenhar o chão com a textura repetida
	
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-groundSizeX, offsetY, 0.0f); // Canto inferior esquerdo
	glTexCoord2f(groundSizeX / tileSizeX, 1.0f); glVertex3f(groundSizeX, offsetY, 0.0f); // Canto inferior direito
	glTexCoord2f(groundSizeX / tileSizeX, 0.0f); glVertex3f(groundSizeX, groundSizeY + offsetY, 0.0f); // Canto superior direito
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-groundSizeX, groundSizeY + offsetY, 0.0f); // Canto superior esquerdo
	glEnd();
	
	glDisable(GL_TEXTURE_2D);


    desenhaJogador();
    desenhaMosquito();
	desenhaPoder();
 
    if(colidiu) {
    	desenhaSangue();
    }
    
		
		// Desenha o Timer
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, largura_tela, 0, altura_tela); // Defina a projeção para as dimensões da tela
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	
	// Posição do texto do Timer na tela
	int posXTimer = 750;
	int posYTimer = altura_tela - 60;
	
	glColor3f(1.0f, 0.0f, 0.0f); // Cor do texto (vermelho)
	
	// Renderizar o texto do Timer
	std::stringstream ssTimer;
	ssTimer << "Timer: " << tempoDecorrido << "s";
	std::string tempoStr = ssTimer.str();
	
	glRasterPos2i(posXTimer, posYTimer); // Posição do texto
	for (int i = 0; i < tempoStr.length(); ++i) {
	    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, tempoStr[i]);
	}
	
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	
	// Desenha o Score
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, largura_tela, 0, altura_tela); // Defina a projeção para as dimensões da tela
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	
	// Posição do texto do Score na tela
	int posXScore = 750;
	int posYScore = altura_tela - 40;
	
	glColor3f(1.0f, 0.0f, 0.0f); // Cor do texto (vermelho)
	
	// Renderizar o texto do Score
	std::stringstream ssScore;
	ssScore << "Score: " << score;
	std::string scoreStr = ssScore.str();
	
	glRasterPos2i(posXScore, posYScore); // Posição do texto
	for (int i = 0; i < scoreStr.length(); ++i) {
	    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, scoreStr[i]);
	}
	
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	desenhaVidas();

    glutSwapBuffers();
}

/* --------------------------------------DESENHA MODO AVENTURA--------------------------------------------- */


/*-----------------------------------ATUALIZAR FRAMES DO JOGADOR---------------------------------------------*/

void atualizaFrameJogador() {
	
	if(jogadorMove) {
		contAnim++;
		if (contAnim >= delayAnim) {
			contAnim = 0;
			frameJogador = (frameJogador + 1) % 6; //Avança para o próximo frame de animação
		}
	}
}

/* --------------------------------------ATUALIZAR FRAMES DO JOGADOR---------------------------------------------*/


/* --------------------------------------ATUALIZAR FRAMES DO MOSQUITO---------------------------------------------*/

void atualizaFrameMosquito() {
	contAnimMosquito++;
	if(contAnimMosquito >= delayAnimMosquito) {
		frameMosquito = (frameMosquito + 1 ) % 4;
		contAnimMosquito = 0;
	}	
}

/* --------------------------------------ATUALIZAR FRAMES DO MOSQUITO---------------------------------------------*/

/*-----------------------------------------------CHECA COLISÕES-----------------------------------------------*/

void checkCollision() {
    for (size_t i = 0; i < mosquitos.size(); ++i) {
        float distancia = sqrt(pow(jogador.x - mosquitos[i].x, 2) + pow(jogador.y - mosquitos[i].y, 2));
        if (distancia < 0.01f) {
            colidiu = true;
            vidas--;
            if (vidas <= 0) {
                colidiu = false;
            } else {
                mosquitos[i].x = -1.0f + static_cast<float>(rand()) / (RAND_MAX / 2.0f);
                mosquitos[i].y = -1.0f + static_cast<float>(rand()) / (RAND_MAX / 2.0f);
            } 
            
            
            if (vidas == 0) {
            	gameover = true;
            	modo_jogo = 3;
			}
        }
    }
}


void checkPowerCollision() {
      for (size_t i = 0; i < mosquitos.size(); ++i) {
        for (size_t j = 0; j < poderes.size(); ++j) {
            float distance = sqrt(pow(poderes[j].x - mosquitos[i].x, 2) + pow(poderes[j].y - mosquitos[i].y, 2));
            if (distance < 0.05f) {
                mosquitos.erase(mosquitos.begin() + i);
                poderes[j].ativo = 0;
                score+=5;
                return;
            }
        }
    }
}







/*-----------------------------------------------CHECA COLISÕES-----------------------------------------------*/

/*---------------------------------------FUNÇÃO IDLE------------------------------------------------*/


void idle() {
	
	desenhaPoder();
	atualizaFrameMosquito();
	atualizaFrameJogador();
	atualizaSangue();
	checkCollision();
	checkPowerCollision();
	atualizaMovimentoMosquitos();
	
	glutPostRedisplay();
}


/*---------------------------------------FUNÇÃO IDLE------------------------------------------------*/


/* --------------------------------------DESENHA MODO QUIZ---------------------------------------------*/	

void desenhaQuiz() {
	
    // Limpar a tela com um fundo branco
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Desenhar um triângulo no centro da tela
    glColor3f(0.0f, 0.0f, 1.0f); // Cor azul
    glBegin(GL_TRIANGLES);
    glVertex2f(0.0f, 0.3f);
    glVertex2f(-0.3f, -0.3f);
    glVertex2f(0.3f, -0.3f);
    glEnd();

    showMenu = false;

    glutSwapBuffers();
}

/* --------------------------------------DESENHA MODO QUIZ---------------------------------------------*/

/* --------------------------------------DESENHA TELA DE ACORDO COM A ESCOLHA---------------------------------------------*/

void desenha() {
	
    if (modo_jogo == 0) {
        desenha_tela_escolha();
    } else if (modo_jogo == 1) {
        desenhaAventura();
    } else if (modo_jogo == 2) {
        desenhaQuiz();
    } else if (modo_jogo == 3) {
    	desenharGameOver();
	}

}


/* --------------------------------------DESENHA TELA DE ACORDO COM A ESCOLHA---------------------------------------------*/


/* --------------------------------------FUNÇÕES DE CALLBACK---------------------------------------------*/
				
				
void teclado(unsigned char key, int x, int y) {
	
 if (showMenu) {
        if (key == 13) { // Tecla Enter
            showMenu = false; // Esconder o menu
            modo_jogo = 1; // Iniciar o modo aventura
        }
    } else {
        switch (key) {
            case 'd': // Move para a direita
            case 'D':
                jogador.x += jogador.velocidade;
                direcaoJogador = 0;
                jogadorMove = true; // Jogador está se movendo
                break;
            case 'a': // Move para a esquerda
            case 'A':
                jogador.x -= jogador.velocidade;
                direcaoJogador = 1;
                jogadorMove = true; // Jogador está se movendo
                break;
            case 27: // Tecla ESC
                showMenu = true;
                modo_jogo = 0;
                
          	case 'r': // Tecla 'R' para reiniciar o jogo
            	if (gameover) {
                	reiniciarJogo();
            	}
            break;
        }
    }
}

// Função de controle de teclas soltas
void teclasSoltas(unsigned char key, int x, int y) {
    if (key == 'a' || key == 'A' || key == 'd' || key == 'D') {
        jogadorMove = false; // Jogador parou de se mover
        frameJogador = 0; // Resetar a animação para o frame inicial
    }
}


void mouse(int button, int state, int x, int y) {
	
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (showMenu) {
            // Verificar clique nos botões apenas quando estiver no menu
            float posX = (float)x / (largura_tela / 2) - 1.0f;
            float posY = 1.0f - (float)y / (altura_tela / 2);
            
            // Verificar clique no botaoAventura
            if (posX >= -0.25f && posX <= 0.25f && posY >= -0.15f && posY <= 0.1f) {
                modo_jogo = 1;
                showMenu = false;
                glutPostRedisplay();
                return;
            }
            // Verificar clique no botaoQuiz
            else if (posX >= -0.25f && posX <= 0.25f && posY >= -0.45f && posY <= -0.2f) {
                modo_jogo = 2;
                showMenu = false;
                glutPostRedisplay();
                return;
            }
        } else {
            // Código adicional para outros estados do jogo
        }     
    }
    
    
   if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        Poder poder;
        poder.ativo = 1;
        poder.x = jogador.x;
        poder.y = jogador.y;

        // Convertendo as coordenadas da tela para coordenadas do mundo
        float windowWidth = glutGet(GLUT_WINDOW_WIDTH);
        float windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

        float worldX = (float)x / largura_tela * 2.0f - 1.0f;
        float worldY = 1.0f - (float)y / altura_tela * 2.0f;

        // Calculando a direção do poder
        float dirX = worldX - poder.x;
        float dirY = worldY - poder.y;
        float magnitude = sqrt(dirX * dirX + dirY * dirY);
        poder.dirX = dirX / magnitude;
        poder.dirY = dirY / magnitude;

        poder.velocidade = 0.02f; // Ajuste a velocidade conforme necessário
        poderes.push_back(poder);
    }
}

        






	/* --------------------------------------FUNÇÕES DE CALLBACK---------------------------------------------*/



/*--------------------------------------------FUNÇÃO PARA INICIALIZAR AS TEXTURAS----------------------------------------------*/

void inicializa() {
	
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Carregar texturas dos botões
    botaoAventura = carregarTextura("sprites/botaoAventura.png");
    botaoQuiz = carregarTextura("sprites/botaoQuiz.png");
    
    aventuraTextures[0] = carregarTextura("sprites/ground2.png");
    aventuraTextures[1] = carregarTextura("sprites/ceu1.png"); //Carregar a textura do céu
   
    
    jogadorDireitaTextures[0] = carregarTextura("sprites/char/anadireita.png");
    jogadorDireitaTextures[1] = carregarTextura("sprites/char/anadireita0.png");
    jogadorDireitaTextures[2] = carregarTextura("sprites/char/anadireita1.png");
    jogadorDireitaTextures[3] = carregarTextura("sprites/char/anadireita2.png");
    jogadorDireitaTextures[4] = carregarTextura("sprites/char/anadireita3.png");
    jogadorDireitaTextures[5] = carregarTextura("sprites/char/anadireita4.png");
    
    
    
    jogadorEsquerdaTextures[0] = carregarTextura("sprites/char/anaesquerda.png");
	jogadorEsquerdaTextures[1] = carregarTextura("sprites/char/anaesquerda0.png");
	jogadorEsquerdaTextures[2] = carregarTextura("sprites/char/anaesquerda1.png");
	jogadorEsquerdaTextures[3] = carregarTextura("sprites/char/anaesquerda2.png");
	jogadorEsquerdaTextures[4] = carregarTextura("sprites/char/anaesquerda3.png");
	jogadorEsquerdaTextures[5] = carregarTextura("sprites/char/anaesquerda4.png");
	
	
	
	mosquitoTextures[0] = carregarTextura("sprites/mosquito/mosquito2.png");
	mosquitoTextures[1] = carregarTextura("sprites/mosquito/mosquito3.png");
	mosquitoTextures[2] = carregarTextura("sprites/mosquito/mosquito4.png");
	mosquitoTextures[3] = carregarTextura("sprites/mosquito/mosquito5.png");

	sangueTextures[0] = carregarTextura("sprites/blood/sangue1.png");
	sangueTextures[1] = carregarTextura("sprites/blood/sangue2.png");
	sangueTextures[2] = carregarTextura("sprites/blood/sangue3.png");
	sangueTextures[3] = carregarTextura("sprites/blood/sangue4.png");
	sangueTextures[4] = carregarTextura("sprites/blood/sangue5.png");
	sangueTextures[5] = carregarTextura("sprites/blood/sangue6.png");
	sangueTextures[6] = carregarTextura("sprites/blood/sangue7.png");
	sangueTextures[7] = carregarTextura("sprites/blood/sangue8.png");
	sangueTextures[8] = carregarTextura("sprites/blood/sangue9.png");
	sangueTextures[9] = carregarTextura("sprites/blood/sangue10.png");
	sangueTextures[10] = carregarTextura("sprites/blood/sangue11.png");
	sangueTextures[11] = carregarTextura("sprites/blood/sangue12.png");
	sangueTextures[12] = carregarTextura("sprites/blood/sangue13.png");
	sangueTextures[13] = carregarTextura("sprites/blood/sangue14.png");
	sangueTextures[14] = carregarTextura("sprites/blood/sangue15.png");
	
	telaQuiztextuta = carregarTextura("sprites/telaQuiz.jpg");
	
	gameOverTexture = carregarTextura("sprites/gameOver.png");
	
	telaInicioTexture = carregarTextura("sprites/telaInicio.png");
 
    coracaoTexture = carregarTextura("sprites/heart.png");
    
    
    
}

/*--------------------------------------------FUNÇÃO PARA INICIALIZAR AS TEXTURAS----------------------------------------------*/


/*--------------------------------------------FUNÇÃO PARA REDIMENSIONAR A TELA ---------------------------------------------*/

void redimensiona(int w, int h) {
	altura_tela = h;
	largura_tela = w;
	glViewport(0, 0, largura_tela, altura_tela);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

/*--------------------------------------------FUNÇÃO PARA REDIMENSIONAR A TELA----------------------------------------------*/




int main(int argc, char** argv) {
		
	srand(time(0));
	
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(largura_tela, altura_tela);
    glutCreateWindow("Mosquito Mission");
    inicializa();
    atualizarMosquitos(0);
	glutTimerFunc(1000, atualizaTempo, 0);
    glutDisplayFunc(desenha);
    glutIdleFunc(idle);
    glutReshapeFunc(redimensiona);
    glutMouseFunc(mouse);
    glutKeyboardFunc(teclado);
    glutKeyboardUpFunc(teclasSoltas); // Adiciona o callback para teclas soltas
    glutMainLoop();
    return 0;
}

