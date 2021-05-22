#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pthread.h>

pthread_t fileThread[4];
int idx = 0;

void* moveFile(void* arg)
{
	char* fileName = (char *) arg;
    char* file = (char* ) arg;
	char* temp = file;

	while (1) { 
		if (*temp == '\0') break;

		if (*temp == '/')
			file = temp + 1;

		temp++;
	}

	if (access(fileName, F_OK) != 0) {
		if (mode == 0) {
			printf("%s : Sad, gagal :(\n", file);
			note = 0;
		}

		return NULL;
	}

	if (file[0] == '.') { 
		if (access("Hidden", F_OK) != 0)
			if (mkdir("Hidden", 0777) != 0) { 
				printf("%s : Sad, gagal :(\n", file);

				return NULL;
			}

		char fullpath[128]; 
		strcpy(fullpath, "Hidden/");
		strcat(fullpath, file + 1);
	}

	else if (strchr(file, '.') == NULL) { 
		if (access("Unknown", F_OK) != 0) 
			if (mkdir("Unknown", 0777) != 0) { 
				printf("%s : Sad, gagal :(\n", file);

				return NULL;
			}

		char fullpath[128]; 
		strcpy(fullpath, "Unknown/");
		strcat(fullpath, file);
		}
	}

    else {
        char extension[16];
		strcpy(extension, strchr(file, '.') + 1);

		int i;
		for (i = 0; i < strlen(extension); i++)
			extension[i] = tolower(extension[i]);

		if (access(extension, F_OK) != 0) {
			if (mkdir(extension, 0777) != 0) { 
				if (mode == 0) printf("%s : Sad, gagal :(\n", file);
				note = 0;

				return NULL;
			}
        }

		char fullpath[128]; 
		strcpy(fullpath, extension);
		strcat(fullpath, "/");
		strcat(fullpath, file);

		if (rename(fileName, fullpath) == 0)
			printf("%s : note Dikategorikan\n", file);
		else {
			printf("%s : Sad, gagal :(\n", file);
			return NULL;
		}
    }
}

void* moveDir(void* arg) {
	char* path = (char* ) arg;

	DIR* dir; 
	struct dirent* dent;

	dir = opendir(path); 

	if (!dir) { 
		note = 0;
		return NULL;
	}

	char filepath[128]; 

	while ((dent = readdir(dir)) != NULL) {
		if (strcmp(dent->d_name, "..") && strcmp(dent->d_name, ".")) {
			strcpy(filepath, path);
			strcat(filepath, "/");
			strcat(filepath, dent->d_name);

			if (isDirectory(filepath)) {
				moveDir((char* ) filepath); 
			}
		}
	}

	char fileNameList[4][128];
	int count = 0; 
	int error;

	seekdir(dir, 0);

	while ((dent = readdir(dir)) != NULL) {
		if (strcmp(dent->d_name, "..") && strcmp(dent->d_name, ".")) {
			strcpy(filepath, path);
			strcat(filepath, "/");
			strcat(filepath, dent->d_name);

			if (!isDirectory(filepath)) {
				strcpy(fileNameList[idx], filepath);
				idx++;
			}
		}

		if (idx == 4) {
			int j;
			for (j = 0; j < idx; j++) {
				error = pthread_create(&fileThread[j], NULL, moveFile, (void* ) fileNameList[j]);
				if (error) note = 0;
			}

			for (j = 0; j < idx; j++) { 
				pthread_join(fileThread[j], NULL);
			}

			idx = 0;
		}
	}

	if (idx) {
		int j;
		for (j = 0; j < idx; j++) {
			error = pthread_create(&fileThread[j], NULL, moveFile, (void* ) fileNameList[j]);
			if (error) note = 0;
		}

		for (j = 0; j < idx; j++) {
			pthread_join(fileThread[j], NULL);
		}
	}

	closedir(dir);
}

int isDirectory(const char* path) { 
	struct stat statbuf;
	if (stat(path, &statbuf) != 0) return 0;
	return S_ISDIR(statbuf.st_mode);
}

int main(int argc, char ** argv) {
	if (strcmp(argv[1], "-f") == 0) {
		if (argc >= 3) {
            int i;
		    int count = argc;

		    for (i = 2; i < count; i++) {
			    pthread_create(&fileThread[idx], NULL, moveFile, (void *) argv[i]);
			    idx++;

			    if (idx == 4) {
				    int j;
				    for (j = 0; j < idx; j++)
					    pthread_join(fileThread[j], NULL);

				    idx = 0;
			    }
		    }
        }

		for (i = 0; i < idx; i++) {
			pthread_join(fileThread[i], NULL);
		}
	}

	if (strcmp(argv[1], "-d") == 0) {
		if (argc = 3) {
            moveDir((void* ) argv[2]); // kalo pake ./soal3 -d

        if (note)
			printf("Direktori sukses disimpan!\n");
		else
			printf("Yah, gagal disimpan :(\n");
		}		
	}

	if (strcmp(argv[1], "*") == 0) {
        moveDir((void* ) "."); // kalo pake ./soal3 \*
	}
}