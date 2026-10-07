/*

Para compilar de dentro do VS Code:

Windows/Linux: CTRL+SHIFT+B (Terminal -> Run Build Task)
macOS: COMMAND+SHIFT+B

Para executar de dentro do VS Code:

Executar normalmente: CTRL+F5 (Run -> Run Without Debugging)
Debugar: F5 (Run -> Start Debugging)

Pelo terminal:

Windows: mingw32-make
Linux/macOS: make

Para executar:

./bagunceitor [arquivo com a imagem de entrada]

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // Para usar strings
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include <stb_image.h>
#include <stb_image_write.h>

// Um pixel Pixel (24 bits)
typedef struct {
  unsigned char r, g, b;
} Pixel;

// Uma imagem Pixel
typedef struct {
  int width, height;  // largura, altura
  int channels;       // qtd de canais (geralmente 3, RGB)
  Pixel* pixels;
} Img;

// As 2 imagens
Img in, out;

// Protótipos
void load(char* name, Img* pic);
void confusao(int altura, int largura, Pixel pin[altura][largura],
             Pixel pout[altura][largura]);
void confusao_inversa(int altura, int largura, Pixel pout[altura][largura],
                      Pixel pin[altura][largura]);
void difusao(int altura, int largura, Pixel pin[altura][largura],
             Pixel pout[altura][largura]);
void difusao_inversa(int altura, int largura, Pixel pout[altura][largura],
                     Pixel pin[altura][largura]);

/* Confusao: soma esta chave em cada byte. A volta e subtrair a mesma chave. */

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("bagunceitor [origem]\n");
    exit(1);
  }

  // Carrega a imagem original
  load(argv[1], &in);

  // Exibe as dimensões na tela, para conferência
  printf("Origem   : %s %d x %d\n", argv[1], in.width, in.height);

  printf("Processando...\n");

  // Cria imagem de saída e "zera" ela
  int tam = in.width * in.height;
  out = in;
  out.pixels = malloc(tam * sizeof(Pixel));
  if (!out.pixels) {
    printf("Sem memoria\n");
    exit(1);
  }
  memset(out.pixels, 0, tam * sizeof(Pixel));

  // pin = imagem original, pout = imagem de saida, as duas como matriz
  Pixel(*pin)[in.width] = (Pixel(*)[in.height])in.pixels;
  Pixel(*pout)[in.width] = (Pixel(*)[in.height])out.pixels;

  confusao(in.height, in.width, pin, pout);

  difusao(in.height, in.width, pout, pin);

  stbi_write_png("criptografada.png",
                 in.width, in.height, 3, pin, 0);

  difusao_inversa(in.height, in.width, pin, pout);

  stbi_write_png("semdifusao.png",
                 in.width, in.height, 3, pout, 0);

  confusao_inversa(in.height, in.width, pout, pin);

  stbi_write_png("recuperada.png",
                 in.width, in.height, 3, pin, 0);

  free(in.pixels);
  free(out.pixels);
}

void confundir_pixel(Pixel* pin, Pixel* pout) {
  pout->r = pin->g ;
  pout->g = pin->b ;
  pout->b = pin->r ;
}

void desconfundir_pixel(Pixel* pin, Pixel* pout) {
  pout->r = pin->b;
  pout->g = pin->r;
  pout->b = pin->g;
}

void confusao(int altura, int largura, Pixel pin[altura][largura],
              Pixel pout[altura][largura]) {
  for (int i = 0; i < altura; i++) {
    for (int j = 0; j < largura; j++) {
      confundir_pixel(&pin[i][j], &pout[i][j]);
    }
  }
}

void confusao_inversa(int altura, int largura, Pixel pout[altura][largura],
                      Pixel pin[altura][largura]) {
  for (int i = 0; i < altura; i++) {
    for (int j = 0; j < largura; j++) {
      desconfundir_pixel(&pout[i][j], &pin[i][j]);
    }
  }
}


void difusao(int altura, int largura, Pixel pin[altura][largura],
             Pixel pout[altura][largura]) {
  for (int i = 0; i < altura; i++) {
    for (int j = 0; j < largura; j++) {
      pout[i][(j + i) % largura] = pin[i][j];
    }
  }
}

void difusao_inversa(int altura, int largura, Pixel pout[altura][largura],
                     Pixel pin[altura][largura]) {
  for (int i = 0; i < altura; i++) {
    for (int j = 0; j < largura; j++) {
      pin[i][j] = pout[i][(j + i) % largura];
    }
  }
}

void load(char* name, Img* pic) {
  pic->pixels =
      (Pixel*)stbi_load(name, &pic->width, &pic->height, &pic->channels, 0);
  if (!pic->pixels) {
    printf("Erro de leitura: %s\n", stbi_failure_reason());
    exit(1);
  }
  printf("Load: %d x %d x %d\n", pic->width, pic->height, pic->channels);
  // Exibe um bloco de 8 x 8 pixels em hexadecimal (teste)
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      printf("[%02X %02X %02X] ", pic->pixels[i].r, pic->pixels[i].g,
             pic->pixels[i].b);
    }
    printf("\n");
  }
}
