//Importando bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Definindo algumas constantes
#define WORD u_int16_t
#define DWORD u_int32_t
#define INPUTFILE "input.wav"

//Cabeçalho RIFF
struct RiffHeader {
	WORD riffID;
	DWORD riffSize;
	WORD waveID;
}; //Fim de RiffHeader

//Dados referentes ao arquivo, é parte do bloco FMT
struct WaveFormat {
	WORD wFormatTag;
	WORD wChannels;
	DWORD dwSamplesPerSec;
	DWORD dwAvgBytesPerSec;
	WORD wBlockAlign;
	WORD wBitsPerSample;
}; //Fim de WaveFormat

//Bloco FMT
struct FmtBlock {
	DWORD fmtID;
	DWORD fmtSize;
	struct WaveFormat wavFormat;
}; //Fim de FmtBlock

//Bloco de dados
struct DataBlock {
	DWORD dataID;
	DWORD dataSize;
}; //Fim de DataBlock

//Declaração da função de busca
int searchDataBlock ();

//Início do programa
void main (char* argv[], int argc) {
	//Definindo uma variável para o cabeçalho riff e apontando um ponteiro para ela
	struct RiffHeader riff, *pRiff;
	pRiff = &riff;
	//Definindo uma variável para o bloco fmt e apontando um ponteiro para ela
	struct FmtBlock fmt, *pFmt;
	pFmt = &fmt;
	//Definindo uma variável para o bloco de dados e apontando um ponteiro para ela
	struct DataBlock data, *pData;
	pData = &data;
	//Abertura do arquivo "input.wav"
	FILE* fp = fopen (INPUTFILE, "r");
	if (!fp) {
		printf ("Erro ao abrir arquivo!\n");
		exit (1);
	}
	//Leitura do bloco riff usando o ponteiro pRiff
	fread (pRiff, sizeof (struct RiffHeader), 1, fp);
	//Leitura do bloco fmt usando o ponteiro pFmt
	fread (pFmt, sizeof (struct FmtBlock), 1, fp);
	//Busca pelo bloco de dados
	int dataPosition = searchDataBlock ();
	if (dataPosition == -1) {
		printf ("Arquivo inválido!\n");
		exit (1);
	}
	//Leitura do bloco de dados
	fseek (fp, dataPosition, SEEK_SET);
	fread (pData, sizeof (struct DataBlock), 1, fp);
	//Mostra as informações coletadas
	printf ("RiffID: \t\t%.4s\n", (char*) &pRiff->riffID);
	printf ("RiffSize: \t\t%d bytes\n", pRiff->riffSize);
	printf ("WaveID: \t\t%.4s\n", (char*) &pRiff->waveID);
	printf ("FmtID: \t\t\t%.4s\n", (char*) &pFmt->fmtID);
	printf ("FmtSize: \t\t%d bytes\n", pFmt->fmtSize);
	printf ("FormatTag: \t\t0x%x\n", pFmt->wavFormat.wFormatTag);
	printf ("Channels: \t\t%d\n", pFmt->wavFormat.wChannels);
	printf ("SamplesPerSec: \t\t%d\n", pFmt->wavFormat.dwSamplesPerSec);
	printf ("BytesPerSec: \t\t%d bytes/s\n", pFmt->wavFormat.dwAvgBytesPerSec);
	printf ("BlockAlign: \t\t%d\n", pFmt->wavFormat.wBlockAlign);
	printf ("BitsPerSample: \t\t%d bits/sample\n", pFmt->wavFormat.wBitsPerSample);
	printf ("DataID: \t\t%.4s\n", (char*) &pData->dataID);
	printf ("DataSize: \t\t%d bytes\n", pData->dataSize);
	//Fecha o arquivo e encerra o programa
	fclose (fp);
	return;
} //Fim de main

//Função que busca pelo bloco de dados
int searchDataBlock () {
	FILE *fp = fopen (INPUTFILE, "r");
	if (!fp) {
		exit (1);
	}
	//Variável para o identificador do bloco e um ponteiro apontando para ela
	char data[4], *p;
	p = data;
	//Variável que vai armazenar a posição do bloco
	int position = 0;
	//Busca pelo identificador "data", que é usado para identificar o início de um bloco de dados
	//Quando encontrado, retorna a posição do bloco
	while (fread (p, 4, 1, fp)) {
		if (strstr (p, "data")) {
			position = ftell (fp);
			fclose(fp);
			return (position - 4);
		}
		position++;
		fseek (fp, position, SEEK_SET);
	}
	//Retorna -1 caso não encontre nenhum bloco de dados
	return (-1);
} //Fim de searchDataBlock
