#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main (int argc, char** argv) {
	// �������� ������ �� ���������
	const char* alpha = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz0123456789";
	bool decode = false;
	bool help = false;
	bool err = false;
	
	// ��� ���������� ����������� ����������
	const int maxparc = 3;
	int parc = 0;
	char* parv[maxparc];

	// ��������� ���������
	int key;
	int alphalen;
	bool overwrite;

	// ����������� ������
	FILE *in = 0;
	FILE *out = 0;
	
	int ch;
	int ind;
	const char* pos;
	long rd;
	long wr;
	
	// ���� ������� ����������
	int cur=1; 	
	int next;
	int i;
	while (cur < argc) {
		next = cur + 1;
		// �������� �� ����
		if (argv[cur][0] == '-') {
			// �������� �� ������� ����
			if (argv[cur][1] == '-') {
				int keylen = strlen ("--alphabet=");
				if (strncmp ("--alphabet=", argv[cur], keylen) == 0) {
					alpha = argv[cur] + keylen;
				}
				else if (strcmp ("--alphabet", argv[cur]) == 0) {
					if ((next < argc) && (argv[next][0] != '-')) {	// ���� �������� ?
						alpha = argv[next];
						next ++;
					}
					else {
						printf ("ERROR: No argument for key %s specified\n", argv[cur]);
						err = true;
					}	
				}
				else if (strcmp ("--type", argv[cur]) == 0) {
					if ((next < argc) && (argv[next][0] != '-')) {	// ���� �������� ?
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
			// ������ ������ �������� ������	
			else {
				int curlen = strlen (argv[cur]);
				// ���� �� �������� ������
				for (i=1; i<curlen; i++)
				{
					switch (argv[cur][i]) {
					// ���� -a
					case 'a':
						if ((next < argc) && (argv[next][0] != '-')) {	// ���� �������� ?
							alpha = argv[next];
							next ++;
						}
						else {
							printf ("ERROR: No argument for key -%c specified\n", argv[cur][i]);
							err = true;
						}
						break;
					// ���� -t
					case 't':
						if ((next < argc) && (argv[next][0] != '-')) {	// ���� �������� ?
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
					// ���� -h
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
		// ����������� ��������
		else if (parc < maxparc) {
			parv[parc] = argv[cur];
			parc++;
		}
		// ������� ����� ����������� ����������
		else {
			printf ("ERROR: Extra parameter specified: %s\n", argv[cur]);
			err = true;
		}
		cur = next;
	}
	
	// ���������, ���������� �� ����������� ����������
	if ((! help) && (parc < 2)) {
		printf ("ERROR: Too few parameters specified\n");
		err = true;
	}
	
	// ���� ���� ������ ��� ��������� ������ - ����� ������� � �����
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
	
	// ����� ��������
	alphalen = strlen(alpha);
	
	// ������ ������������ �����
	for (i=0; i<strlen(parv[0]); i++) {
		if (! isdigit(parv[0][i])) {
			printf ("ERROR: Invalid chipher key: %s, must be positive number\n", parv[0]);
			return -1;
		}
	}
	
	// ���������� �����
	key = atoi(parv[0]);
	if (decode) {	// ��� ������������� ������ ���� ����� �� ���������������
		key = alphalen - key;
		if (key < 0) {
			key = alphalen + key % alphalen;
		}
	}
	key %= alphalen;
	
	// ���� ���������� ����� ���, ������� ���� ����������������
	overwrite = (parc == 2); 
	
	// ���������� ������
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
	
	// �����������/�������������
	while (true) {
		wr = ftell (in);
		ch = fgetc(in);
		if (ch == EOF) break;
		
		pos = strchr(alpha, ch);	// ���� ��������� ������ � ��������
		if (pos) {
			// �������������� �������
			ind = pos - alpha;	
			ind = (ind + key) % alphalen;
			ch = alpha[ind];
			// ������ ������� � ����
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
			// ������ ������� � ����, ���� �����
			if (! overwrite) {
				fputc (ch, out);
			}
		}
	}
	
	if (in) fclose (in);
	if (out) fclose (out);
	return 0;
}
