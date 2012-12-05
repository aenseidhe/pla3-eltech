#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main (int argc, char** argv) {
	// Значения ключей по умолчанию
	const char* alpha = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz0123456789";
	bool decode = false;
	bool help = false;
	bool err = false;
	
	// Для накопления позиционных параметров
	const int maxparc = 3;
	int parc = 0;
	char* parv[maxparc];

	// Параметры алгоритма
	int key;
	int alphalen;
	bool overwrite;

	// Дескрипторы файлов
	FILE *in = 0;
	FILE *out = 0;
	
	int ch;
	int ind;
	const char* pos;
	long rd;
	long wr;
	
	// Цикл разбора параметров
	int cur=1; 	
	int next;
	int i;
	while (cur < argc) {
		next = cur + 1;
		// Проверка на ключ
		if (argv[cur][0] == '-') {
			// Проверка на длинный ключ
			if (argv[cur][1] == '-') {
				int keylen = strlen ("--alphabet=");
				if (strncmp ("--alphabet=", argv[cur], keylen) == 0) {
					alpha = argv[cur] + keylen;
				}
				else if (strcmp ("--alphabet", argv[cur]) == 0) {
					if ((next < argc) && (argv[next][0] != '-')) {	// Есть аргумент ?
						alpha = argv[next];
						next ++;
					}
					else {
						printf ("ERROR: No argument for key %s specified\n", argv[cur]);
						err = true;
					}	
				}
				else if (strcmp ("--type", argv[cur]) == 0) {
					if ((next < argc) && (argv[next][0] != '-')) {	// Есть аргумент ?
						if (strcmp ("decode", argv[next]) == 0) {
							decode = true;
						}
						else if (strcmp ("encode", argv[next]) == 0) {
							decode = false;
						}
						else {
							printf ("ERROR: Invalid argument for key %s: %s\n", argv[cur], argv[next]);
							err = true;
						}
						next ++;
					}
					else {
						printf ("ERROR: No argument for key %s specified\n", argv[cur]);
						err = true;
					}
				}
				else if (strcmp ("--type=decode", argv[cur]) == 0) {
					decode = true;
				}
				else if (strcmp ("--type=encode", argv[cur]) == 0) {
					decode = false;
				}
				else if (strcmp ("--help", argv[cur]) == 0) {
					help = true;
				}
				else {
					printf ("ERROR: Invalid long key: %s\n", argv[cur]);
					err = true;
				}
			}
			// Разбор строки коротких ключей	
			else {
				int curlen = strlen (argv[cur]);
				// Цикл по коротким ключам
				for (i=1; i<curlen; i++)
				{
					switch (argv[cur][i]) {
					// Ключ -a
					case 'a':
						if ((next < argc) && (argv[next][0] != '-')) {	// Есть аргумент ?
							alpha = argv[next];
							next ++;
						}
						else {
							printf ("ERROR: No argument for key -%c specified\n", argv[cur][i]);
							err = true;
						}
						break;
					// Ключ -t
					case 't':
						if ((next < argc) && (argv[next][0] != '-')) {	// Есть аргумент ?
							if (strcmp ("decode", argv[next]) == 0) {
								decode = true;
							}
							else if (strcmp ("encode", argv[next]) == 0) {
								decode = false;
							}
							else {
								printf ("ERROR: Invalid argument for key -%c: %s\n", argv[cur][i], argv[next]);
								err = true;
							}
							next ++;
						}
						else {
							printf ("ERROR: No argument for key -%c specified\n", argv[cur][i]);
							err = true;
						}
						break;
					// Ключ -h
					case 'h':
						help = true;
						break;
					default:
						printf ("ERROR: Invalid key: -%c\n", argv[cur][i]);
						err = true;
					}
				}
			}
		}
		// Позиционный параметр
		else if (parc < maxparc) {
			parv[parc] = argv[cur];
			parc++;
		}
		// Слишком много позиционных параметров
		else {
			printf ("ERROR: Extra parameter specified: %s\n", argv[cur]);
			err = true;
		}
		cur = next;
	}
	
	// Проверяем, достаточно ли позиционных параметров
	if ((! help) && (parc < 2)) {
		printf ("ERROR: Too few parameters specified\n");
		err = true;
	}
	
	// Если были ошибки или требуется помощь - вывод справки и конец
	if (help || err) {
		printf ("\nUsage: crypt [options] <key> <source> [<dest>]\n");
		printf ("\noptions:\n");
		printf ("\t-a, --alphabet=<alphabet>  alphabet for chipher (default: AaBbCc..Zz0..9)\n"); 
		printf ("\t-t, --type=<type>          'encode' or 'decode', encode if not specified\n");
		printf ("\t-h, --help                 display this help\n");

		printf ("key:\n");
		printf ("\tencode/decode key (positive number)\n");

		printf ("source:\n");
		printf ("\tsource file for encode/decode\n");

		printf ("dest:\n");
		printf ("\tdestination file for encode/decode. Overwritting source file if not specified\n");
		return err ? -1 : 0;
	}
	
	// Длина алфавита
	alphalen = strlen(alpha);
	
	// Анализ допустимости ключа
	for (i=0; i<strlen(parv[0]); i++) {
		if (! isdigit(parv[0][i])) {
			printf ("ERROR: Invalid chipher key: %s, must be positive number\n", parv[0]);
			return -1;
		}
	}
	
	// Подготовка ключа
	key = atoi(parv[0]);
	if (decode) {	// Для декодирования меняем знак ключа на противоположный
		key = alphalen - key;
		if (key < 0) {
			key = alphalen + key % alphalen;
		}
	}
	key %= alphalen;
	
	// Если параметров всего два, входной файл перезаписывается
	overwrite = (parc == 2); 
	
	// Подготовка файлов
	if (overwrite) {
		in = fopen (parv[1], "r+t");
		if (!in) {
			printf ("ERROR: Input/output file %s can't be open", parv[1]);
			return -1;
		}
	}
	else {
		in = fopen (parv[1], "rt");
		if (!in) {
			printf ("ERROR: Input file %s can't be open", parv[1]);
			return -1;
		}
		out = fopen (parv[2], "wt");
		if (!out) {
			printf ("ERROR: Output file %s can't be open", parv[2]);
			fclose (in);
			return -1;
		}
	}
	
	// Кодирование/декодирование
	while (true) {
		wr = ftell (in);
		ch = fgetc(in);
		if (ch == EOF) break;
		
		pos = strchr(alpha, ch);	// Ищем очередной символ в алфавите
		if (pos) {
			// Преобразование символа
			ind = pos - alpha;	
			ind = (ind + key) % alphalen;
			ch = alpha[ind];
			// Запись символа в файл
			if (overwrite) {
				rd = ftell (in);
				fseek (in, wr, SEEK_SET);
				fputc (ch, in);
				fseek (in, rd, SEEK_SET);
			}
			else {
				fputc (ch, out);
			}
		}	
		else {
			// Запись символа в файл, если нужно
			if (! overwrite) {
				fputc (ch, out);
			}
		}
	}
	
	if (in) fclose (in);
	if (out) fclose (out);
	return 0;
}
