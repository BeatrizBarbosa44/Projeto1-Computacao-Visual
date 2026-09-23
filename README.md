# Projeto 1 - Computação Visual

Projeto desenvolvido para a disciplina de Computação Visual.

## Integrantes

- Beatriz Aparecida de Mello Barbosa - RA: 10354067
- Henrique Jeam Ferreira Lima - RA: 10277156

## Descrição

O programa realiza o carregamento e processamento de imagens utilizando C, SDL3, SDL_image e SDL_ttf.

A imagem é recebida pela linha de comando. Caso seja colorida, ela é convertida para escala de cinza utilizando a fórmula:

Y = 0.2125R + 0.7154G + 0.0721B

Após o carregamento, o programa apresenta a imagem em uma janela principal e, em uma janela secundária, exibe seu histograma e informações estatísticas.

## Funcionalidades

- Carregamento de imagens por linha de comando.
- Detecção automática de imagens coloridas ou em escala de cinza.
- Conversão para escala de cinza.
- Exibição do histograma.
- Cálculo da média de intensidade.
- Classificação de luminosidade.
- Cálculo do desvio padrão.
- Classificação de contraste.
- Equalização de histograma.
- Alternância entre imagem original e imagem equalizada.
- Alternância entre resolução 1024x768 e resolução original da imagem.
- Estados visuais dos botões: normal, hover e pressionado.
- Salvamento da imagem atual em `output_image.png` através da tecla `S`.

## Fonte utilizada

Foi utilizada a fonte **DejaVu Sans**, localizada em:

`assets/fonts/DejaVuSans.ttf`

## Dependências

- GCC
- SDL3
- SDL3_image
- SDL3_ttf

## Compilação

No Windows, utilizando MSYS2 UCRT64:

```powershell
mingw32-make -B