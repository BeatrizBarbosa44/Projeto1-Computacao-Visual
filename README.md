# Projeto 1 - Computação Visual

Projeto desenvolvido para a disciplina de **Computação Visual** do curso de Ciência da Computação da Universidade Presbiteriana Mackenzie.

O projeto consiste em uma aplicação de processamento de imagens desenvolvida em linguagem C utilizando as bibliotecas SDL3, SDL_image e SDL_ttf.

---

## Integrantes

- **Beatriz Aparecida de Mello Barbosa** - RA: **10354067**
- **Henrique Jeam Ferreira Lima** - RA: **10277156**

---

## Repositório

Repositório público do projeto:

https://github.com/BeatrizBarbosa44/Projeto1-Computacao-Visual

---

## Descrição

O programa recebe o caminho de uma imagem através da linha de comando e realiza operações de processamento e análise.

Após o carregamento, o programa identifica se a imagem já está em escala de cinza ou se é colorida.

Caso seja colorida, ela é convertida para escala de cinza utilizando:

```text
Y = 0.2125R + 0.7154G + 0.0721B
```

A imagem em escala de cinza é utilizada como base para as demais operações do programa.

A aplicação utiliza duas janelas:

- uma janela principal para exibição da imagem;
- uma janela secundária para exibição do histograma, informações estatísticas e botões de controle.

---

## Funcionalidades

O projeto possui as seguintes funcionalidades:

- carregamento de imagens por linha de comando;
- tratamento de arquivos inexistentes;
- tratamento de arquivos em formato inválido;
- identificação de imagens coloridas ou em escala de cinza;
- conversão de imagens coloridas para escala de cinza;
- cálculo do histograma;
- exibição gráfica do histograma;
- cálculo da média de intensidade;
- classificação da luminosidade;
- cálculo do desvio padrão;
- classificação do contraste;
- equalização do histograma;
- restauração da imagem original em escala de cinza;
- atualização do histograma após equalização;
- atualização das informações estatísticas após equalização;
- alternância entre resolução original e 1024x768;
- redimensionamento da janela principal;
- estados visuais dos botões;
- salvamento da imagem através da tecla `S`.

---
## Estrutura do programa

O programa é dividido nas seguintes etapas:

1. Carregamento da imagem
2. Conversão para RGBA32
3. Verificação/conversão para escala de cinza
4. Cálculo do histograma
5. Cálculo da média e desvio padrão
6. Classificação de luminosidade e contraste
7. Equalização do histograma
8. Atualização das janelas

## Interface

### Janela principal

A janela principal inicia com resolução:

```text
1024x768
```

O botão de resolução permite alternar entre:

- resolução 1024x768;
- resolução original da imagem.

Quando o modo de resolução original é utilizado, a janela principal assume as dimensões da imagem.

Caso o tamanho da janela ultrapasse a área disponível da tela, a janela é posicionada a partir da coordenada `(0,0)`.

---

### Janela secundária

A janela secundária possui tamanho fixo e apresenta:

- histograma;
- média de intensidade;
- classificação de luminosidade;
- desvio padrão;
- classificação de contraste;
- botão de equalização;
- botão de resolução.

---

## Histograma

O histograma possui 256 posições, correspondentes aos níveis de intensidade entre 0 e 255.

A partir dos valores do histograma são calculadas as informações estatísticas apresentadas na interface.

---

## Luminosidade

A luminosidade é classificada utilizando a média de intensidade da imagem.

Os intervalos utilizados pelo grupo foram:

```text
0 a 84     -> escura
85 a 170   -> média
171 a 255  -> clara
```

---


## Contraste

O contraste é classificado através do desvio padrão.

Os intervalos utilizados pelo grupo foram:

```text
até 42,5       -> baixo
até 85,0       -> médio
acima de 85,0  -> alto
```

---

## Equalização

O botão **Equalizar** aplica a equalização de histograma à imagem.

Durante o processo:

1. o histograma da imagem é calculado;
2. é calculada a distribuição acumulada (CDF);
3. os valores de intensidade são redistribuídos;
4. a imagem é atualizada;
5. o histograma é recalculado;
6. a média e o desvio padrão são recalculados;
7. as classificações de luminosidade e contraste são atualizadas.

Após a equalização, o botão passa a apresentar:

```text
Mostrar original
```

Ao pressioná-lo novamente, a imagem original em escala de cinza é restaurada.

---

## Resolução

O segundo botão permite alternar entre:

```text
Resolucao: 1024x768
```

e:

```text
Resolucao: original
```

A imagem e o tamanho da janela principal são atualizados de acordo com a opção selecionada.

---

## Estados dos botões

Os botões possuem três estados visuais:

- normal;
- mouse sobre o botão;
- botão pressionado.

Foram utilizadas diferentes intensidades de cinza para representar cada estado.

Os estados dos dois botões são controlados de forma independente.

---

## Salvamento

Ao pressionar:

```text
S
```

o programa salva a imagem no arquivo:

```text
output_image.png
```

Caso o arquivo já exista, ele é sobrescrito.

O resultado da operação também é informado no terminal.

---

## Fonte utilizada

Foi utilizada a fonte:

```text
DejaVu Sans
```

O arquivo da fonte está incluído no projeto:

```text
assets/fonts/DejaVuSans.ttf
```

A fonte é carregada utilizando SDL_ttf, evitando dependência de uma instalação específica no sistema operacional.

---

## Ambiente utilizado

### Sistema operacional

```text
Windows 11 Pro
Versão 25H2
Build 26200.9457
```

### Editor

```text
Visual Studio Code
```

### Compilador

```text
GCC 16.1.0
MSYS2 UCRT64
```

### Bibliotecas

```text
SDL3       3.4.10-1
SDL3_image 3.4.4-1
SDL3_ttf   3.2.2-3
```

---

## Dependências

Para a compilação são necessárias:

- GCC;
- SDL3;
- SDL3_image;
- SDL3_ttf.

No ambiente utilizado durante o desenvolvimento, as bibliotecas foram instaladas através do MSYS2 UCRT64.

---

## Compilação no Windows

Caso o diretório do MSYS2 não esteja disponível no `PATH` do terminal:

```powershell
$env:Path += ";C:\msys64\ucrt64\bin"
```

Depois, dentro da pasta do projeto:

```powershell
mingw32-make -B
```

Após a compilação será gerado:

```text
main.exe
```

---

## Execução

O programa deve receber o caminho da imagem como argumento.

Exemplo:

```powershell
.\main.exe kodim23.png
```

Outro exemplo:

```powershell
.\main.exe cinza.jpg
```

Também é possível utilizar o caminho completo de uma imagem:

```powershell
.\main.exe C:\caminho\para\imagem.png
```

Caso nenhum argumento seja informado, o programa apresenta no terminal uma mensagem indicando a forma correta de execução.

---

## Tratamento de erros

### Arquivo inexistente

Exemplo:

```powershell
.\main.exe imagem_que_nao_existe.png
```

O programa apresenta uma mensagem de erro e encerra corretamente a execução.

### Arquivo inválido

Exemplo:

```powershell
.\main.exe makefile
```

O programa identifica que o arquivo não possui formato de imagem suportado e apresenta a mensagem correspondente.

---

## Testes realizados

O projeto foi testado com:

- imagem colorida;
- imagem originalmente em escala de cinza;
- arquivo inexistente;
- arquivo inválido;
- equalização do histograma;
- restauração da imagem original;
- atualização do histograma;
- atualização da média;
- atualização do desvio padrão;
- mudança da classificação de luminosidade;
- mudança da classificação de contraste;
- resolução original;
- resolução 1024x768;
- estados normal, hover e pressionado dos botões;
- criação de `output_image.png`;
- sobrescrita de `output_image.png`.

Imagens utilizadas durante os testes:

```text
kodim23.png
cinza.jpg
```

---

## Estrutura do projeto

```text
Projeto1_Computacao_Visual/
|
|-- assets/
|   `-- fonts/
|       `-- DejaVuSans.ttf
|
|-- .gitignore
|-- README.md
|-- cinza.jpg
|-- kodim23.png
|-- main.c
`-- makefile
```

---

## Arquivos ignorados pelo Git

O arquivo `.gitignore` evita o envio de arquivos gerados durante a compilação ou execução.

```text
*.o
*.exe
*.dll
output_image.png
```

---

## Código-base

O projeto foi desenvolvido a partir de um código-base disponibilizado pelo professor **André Kishimoto** para a disciplina de Computação Visual.

O código foi adaptado para implementar os requisitos do Projeto 1.

Entre as principais alterações realizadas estão:

- entrada da imagem pela linha de comando;
- tratamento de erros;
- conversão para escala de cinza;
- criação da janela secundária;
- cálculo e exibição do histograma;
- cálculo de média;
- cálculo de desvio padrão;
- classificação de luminosidade;
- classificação de contraste;
- equalização do histograma;
- restauração da imagem original;
- textos utilizando SDL_ttf;
- botão de equalização;
- botão de resolução;
- estados visuais dos botões;
- redimensionamento da janela;
- salvamento da imagem;
- remoção de funcionalidades do exemplo original que não eram necessárias ao projeto.

---

## Contribuições dos integrantes

### Beatriz Aparecida de Mello Barbosa - RA 10354067

Participou da configuração do ambiente de desenvolvimento, implementação e integração das funcionalidades do projeto, realização dos testes das operações de processamento de imagens, identificação e correção de problemas encontrados durante o desenvolvimento, elaboração da documentação e configuração inicial do repositório GitHub.

### Henrique Jeam Ferreira Lima - RA 10277156

Participou da revisão final do projeto e da documentação, conferência dos requisitos do trabalho, revisão e atualização do README, verificação das informações de compilação e execução e organização final da documentação no repositório GitHub.

---

## Controle de versão

O projeto utiliza Git para controle de versão e está armazenado em um repositório público no GitHub.

Durante parte do desenvolvimento, as implementações e testes foram realizados localmente no Visual Studio Code. Posteriormente, o projeto passou a ser versionado e sincronizado com o repositório GitHub.

---

## Repositório público

O código-fonte e a documentação estão disponíveis em:

https://github.com/BeatrizBarbosa44/Projeto1-Computacao-Visual
