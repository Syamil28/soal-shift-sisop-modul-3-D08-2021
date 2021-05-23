# soal-shift-sisop-modul-3-D08-2021

## **Soal 2**
### Penjelasan soal
Crypto (kamu) adalah teman Loba. Suatu pagi, Crypto melihat Loba yang sedang kewalahan mengerjakan tugas dari bosnya. Karena Crypto adalah orang yang sangat menyukai tantangan, dia ingin membantu Loba mengerjakan tugasnya. Detil dari tugas tersebut adalah:

a. Membuat program perkalian matrix (4x3 dengan 3x6) dan menampilkan hasilnya. Matriks nantinya akan berisi angka 1-20 (tidak perlu dibuat filter angka).

b. Membuat program dengan menggunakan matriks output dari program sebelumnya (program soal2a.c) (Catatan!: gunakan shared memory). Kemudian matriks tersebut akan dilakukan perhitungan dengan matrix baru (input user) sebagai berikut contoh perhitungan untuk matriks yang ada. Perhitungannya adalah setiap cel yang berasal dari matriks A menjadi angka untuk faktorial, lalu cel dari matriks B menjadi batas maksimal faktorialnya (dari paling besar ke paling kecil) (Catatan!: gunakan thread untuk perhitungan di setiap cel). 
Ketentuan
If a >= b  -> a!/(a-b)!
If b > a -> a!
If 0 -> 0

c. Karena takut lag dalam pengerjaannya membantu Loba, Crypto juga membuat program (soal2c.c) untuk mengecek 5 proses teratas apa saja yang memakan resource komputernya dengan command “ps aux | sort -nrk 3,3 | head -5” (Catatan!: Harus menggunakan IPC Pipes)

### Penyelesaian
#### Soal 2a 
```c
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
```
Pertama semua library yang akan digunakan di-include.

```c
void main()
{
    int mat1[4][3];
    int mat2[3][6];
    int (*mat3)[6];
    key_t key = 1234;
    int i;
    int j;
    int k;

    ...
}
```
Kemudian di awal fungsi `main`, variabel-variabel yang akan dipakai dideklarasikan. `mat1` dan `mat2` akan menjadi tempat matriks yang berasal dari input user. `mat3` akan menjadi tempat hasil perkalian matriks. `key` adalah nilai yang digunakan sebagai key ketika membuat shared memory. `i`, `j`, `k` akan digunakan dalam looping.

```c
int shmid = shmget(key, sizeof(int[4][6]), IPC_CREAT | 0666);
if (shmid == -1)
{
    fprintf(stderr, "shmget() Failed");
    return;
}

mat3 = shmat(shmid, NULL, 0);
if(mat3 == (void *)-1) {
    fprintf(stderr, "shmat() Failed" ); 
    return;
}
```
selanjutnya shared memory dibuat dan di-attach ke variable `mat3`. Apabila error terjadi dari salah satu proses sebelumnya, maka program langsung dihentikan.

```c
printf("Input matrix 4x3\n");
for (i = 0; i < 4; i++)
{
    for (j = 0; j < 3; j++)
    {
        scanf("%d", &mat1[i][j]);
    }
}
printf("Input matrix 3x6\n");
for (i = 0; i < 3; i++)
{
    for (j = 0; j < 6; j++)
    {
        scanf("%d", &mat2[i][j]);
    }
}
```
Variable `mat1` dan `mat2` diisikan dengan nilai dari input yang dilakukan oleh user.

```c
for (i = 0; i < 4; i++)
{
    for (j = 0; j < 6; j++)
    {
        mat3[i][j] = 0;

        for (k = 0; k < 3; k++)
        {
            mat3[i][j] += mat1[i][k] * mat2[k][j];
        }
    }
}
```
setelah itu program akan melakukan perhitungan hasil perkalian matriks dari matriks `mat1` dan `mat2`. Hasil perhitungan tersebut kemudian akan disimpan di variable `mat3`.

```c
printf("Result\n");
for (i = 0; i < 4; i++)
{
    for (j = 0; j < 6; j++)
    {
        printf("%d ", mat3[i][j]);
    }
    printf("\n");
}
```
Selanjutnya hasil perkalian tersebut akan ditampilkan pada terminal.

```c
char ch;
do
{
    printf("Type c to close\n");
    scanf(" %c", &ch);
} while (ch != 'c');
```
Program dibuat untuk menunggu sampai user meng-inputkan karakter `c` sebelum melanjutkan eksekusi.

```c
shmdt(mat3);
shmctl(shmid, IPC_RMID, NULL);
```
Terakhir, shared memory di-dettach dari `mat3` dan shared memory tersebut dihapus.

![Screenshot (81)](https://user-images.githubusercontent.com/62102884/119263685-10fbac00-bc1b-11eb-825f-be007804e8bb.png)

#### Soal 2b
```c
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
```
Sama seperti soal 2a, pertama dilakukan include library yang digunakan.

```c
typedef struct
{
    long long *cell;
    int a;
    int b;
} CalcArgs;
```
Sebuah struct bernama `CalcArgs` dideklarasikan. Struct ini akan digunakan untuk memberikan argumen ke thread.

```c
CalcArgs *makeCalcArgs(long long *cell, int a, int b)
{
    CalcArgs *args = malloc(sizeof(CalcArgs));

    args->cell = cell;
    args->a = a;
    args->b = b;

    return args;
}
```
fungsi bernama `makeCalcArgs` diatas akan mengembalikan pointer ke struct `CalcArgs`. Fungsi ini digunakan untuk membuat sebuah struct baru dan mengisikan nilainya.

```c
void *calculate(void *arguments)
{
    CalcArgs *args = (CalcArgs *)arguments;

    if (args->a == 0 || args->b == 0)
    {
        *(args->cell) = 0;
    }
    else
    {
        int i;
        int x = args->a - args->b > 0 ? args->a - args->b : 1;

        *(args->cell) = args->a;
        for (int i = args->a - 1; i > x; i--)
        {
            *(args->cell) *= i;
        }
    }

    return NULL;
}
```
fungsi `calculate`diatas nantinya akan digunakan dalam thread. Awalnya, dilakukan type casting agar nilai dari `arguments` bisa dibaca. selanjutnya argumen `a` dan `b` dicek, apabila bernilai 0 maka nilai di `cell` akan diset 0. Apabila bukan 0, maka akan dicari nilai batas faktorialnya dan kemudian mengisikan nilai `cell` dengan hasil perkalian nilai-nilai dari `a` sampai batas ditambah 1.

```c
void main()
{
    int (*matA)[6];
    int matB[4][6];
    long long matC[4][6];
    int i;
    int j;
    key_t key = 1234;

    ...
}
```
kemudian pada fungsi `main` dideklarasikan variabel-variabel yang akan dipakai. Variable `matA` digunakan untuk menyimpan hasil dari program soal 2a. Variable `matB` akan diisi dari input yang dilakukan oleh user.  Variable `matC` digunakan untuk tempat menyimpan hasil perhitungan dari perkalian `matA` dan `matB`. Variable `i` dan `j` digunakan dalam looping. Sedangkan variable `key` digunakan sebagai key ketika mengambil shared memory.

```c
int shmid = shmget(key, sizeof(int[4][6]), IPC_CREAT | 0666);
if (shmid == -1)
{
    fprintf(stderr, "shmget() Failed");
    return;
}

matA = shmat(shmid, NULL, 0);
if (matA == (void *)-1)
{
    fprintf(stderr, "shmat() Failed");
    return;
}
```
Sama seperti soal 2a, dibuat shared memory dan di-attach ke variable `matA`. Kemudian dilakukan pengecekan error dan program akan dihentikan apabila terdapat error.

```c
printf("Matrix A\n");
for (i = 0; i < 4; i++)
{
    for (j = 0; j < 6; j++)
    {
        printf("%d ", matA[i][j]);
    }
    printf("\n");
}
```
Matriks yang didapatkan dari shared memory pada soal `2a` akan ditampilkan ke terminal.

```c
printf("Input matrix B 4x6\n");
for (i = 0; i < 4; i++)
{
    for (j = 0; j < 6; j++)
    {
        scanf("%d", &matB[i][j]);
    }
}
```
Variable `matB` akan diisikan melalui input yang dilakukan user.

```c
pthread_t tid[4][6];

for (i = 0; i < 4; i++)
{
    for (j = 0; j < 6; j++)
    {
        pthread_create(&(tid[i][j]), NULL, calculate,
                       (void *)makeCalcArgs(&(matC[i][j]), matA[i][j], matB[i][j]));
    }
}
```
Perhitungan dilakukan dengan cara membuat thread dengan fungsi `calculate` dan argumennya adalah pointer ke cell, nilai a dari `matA`, dan nilai b dari `matB`.  

```c
printf("Result\n");
for (i = 0; i < 4; i++)
{
    for (j = 0; j < 6; j++)
    {
        pthread_join(tid[i][j], NULL);
        printf("%lld ", matC[i][j]);
    }
    printf("\n");
}
```
Selanjutnya Hasil perhitungan akan ditampilkan ke terminal. Thread akan dijoin terlebih dahulu sebelum diprint agar program tidak menampilkan nilai dari suatu cell sebelum threadnya selesai menghitung.

```c
shmdt(matA);
shmctl(shmid, IPC_RMID, NULL);
```
Sama seperti soal 2a, shared memory di-dettach dari variable `matA` dan dihapus.

![Screenshot (82)](https://user-images.githubusercontent.com/62102884/119263704-1f49c800-bc1b-11eb-9f0a-6ffac8ebecfe.png)

#### Soal 2c
```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
```
Pertama dilakukan include library.

```c
void main(int argc, char *argv[]) {
    int fd1[2];

    if (pipe(fd1)==-1) 
	{ 
		fprintf(stderr, "Pipe Failed" ); 
		return; 
	}

    ...
}
```
Di awal fungsi main, pipe pertama bernama `fd1` dibuat. Apabila ada error, program akan langsung dihentikan.

```c
pid_t pid = fork();

if(pid == 0) {
    dup2(fd1[1], STDOUT_FILENO);

    close(fd1[0]);
    close(fd1[1]);

    char *argv[] = {"ps", "aux", NULL};
    execv("/bin/ps", argv);
    }
```
Kemudian program akan di-fork. Pada child process, file descriptor untuk standard output akan diganti menjadi ujung tulis dari pipe `fd1` sebelum kedua ujung dari pipe tersebut ditutup. Kemudian `ps aux` dieksekusi dengan bantuan `execv()`.

```c
int fd2[2];

if (pipe(fd2)==-1) 
{ 
	fprintf(stderr, "Pipe Failed" ); 
	return; 
}
```
Kembali di program parent, pipe kedua yang bernama `fd2` dibuat. Sama seperti sebelumnya, juga dilakukan pengecekan error.

```c
pid = fork();

if(pid == 0) {
    dup2(fd1[0], STDIN_FILENO);

    close(fd1[0]);
    close(fd1[1]);

    dup2(fd2[1], STDOUT_FILENO);

    close(fd2[0]);
    close(fd2[1]);

    char *argv[] = {"sort", "-nrk", "3,3", NULL};
    execv("/bin/sort", argv);
} else if (pid > 0) {

    ...

}
```
Program kemudian kembali di-fork. Kali ini di dalam child process, file descriptor untuk standard input diganti menjadi ujung baca dari pipe `fd1` dan kedua ujung dari pipe tersebut ditutup. File descriptor untuk standard output juga diganti menjadi ujung tulis dari pipe `fd2` dan kedua ujung pipe `fd2` juga ditutup. Kemudian `sort -nrk 3,3` dieksekusi dengan bantuan `execv()`.

```c
close(fd1[0]);
close(fd1[1]);

dup2(fd2[0], STDIN_FILENO);

close(fd2[0]);
close(fd2[1]);

char *argv[] = {"head", "-5", NULL};
execv("/bin/head", argv);
```
Di program parent, pipe `fd1` akan ditutup karena sudah tidak digunakan. File descriptor untuk standard input diganti menjadi ujung baca dari pipe `fd2` dan pipe tersebut ditutup. Terahir, `head -5` akan dieksekusi dengan bantuan `execv()`.

![Screenshot (83)](https://user-images.githubusercontent.com/62102884/119263731-34bef200-bc1b-11eb-8b1e-90c37a520607.png)

###Kendala Pengerjaan
1. Waktu pengerjaan yang bertepatan dengan idul fitri serta ETS.
2. Sempat bingung bagaimana cara melakukan shared memory.

## **Soal 3**

Program ini akan memindahkan file sesuai ekstensinya ke dalam folder sesuai ekstensinya yang folder hasilnya terdapat di working directory ketika program kategori tersebut dijalankan.

### **Penyelesaian**
Fungsi `moveFile()`:
```c
	if (access(fileName, F_OK) != 0) {
		if (mode == 0) {
			printf("%s : Sad, gagal :(\n", file);
			berhasil = 0;
		}

		return NULL;
	}
```
Pertama mengecek apakah file ada atau tidak. Jika tidak ada, print gagal.
```c
	while (1) { // Skip semua tanda /
		if (*temp == '\0') break;

		if (*temp == '/')
			file = temp + 1;

		temp++;
	}
```
Mengambil nama file, tanpa path nya, dengan melewatkan tanda `/`.
```c
		char extension[16];
		strcpy(extension, strchr(file, '.') + 1);
```
Ekstensi file didapatkan setelah tanda `.` terdepan. Untuk mendapatkan ekstensi file, menggunakan function `strchr()` untuk mendapatkan `.`, lalu diambil string setelah tanda `.`, dicopy ke char `extension`.
```c
		int i;
		for (i = 0; i < strlen(extension); i++)
			extension[i] = tolower(extension[i]);
```
Agar ekstensi tidak *case sensitive*, digunakan function `tolower`. Dengan ini setiap karakter akan menjadi *lower case*.
```c
		if (access(extension, F_OK) != 0) {
			if (mkdir(extension, 0777) != 0) {
				printf("%s : Sad, gagal :(\n", file);
				return NULL;
			}
		}
```
Mengecek apakah direktori ada atau tidak. Jika gagal membuat direktori, print gagal.
```c
		char fullpath[128]; // Nama file dengan folder baru
		strcpy(fullpath, extension);
		strcat(fullpath, "/");
		strcat(fullpath, file);

		if (rename(fileName, fullpath) == 0)
			printf("%s : Berhasil Dikategorikan\n", file);
		else {
			printf("%s : Sad, gagal :(\n", file);
			return NULL;
		}
```
Menggabungkan nama file dengan path baru (direktori sesuai ekstensinya). Lalu dicek apakah path file berhasil direname. Jika berhasil print berhasil, jika tidak berhasil print gagal.
Untuk file "Hidden" dan "Unknown", langkahnya sama seperti membuat direktori ekstensi.

Fungsi `moveDir`:
```c
	char* path = (char* ) arg;

	DIR* dir;
	struct dirent* dent;

	dir = opendir(path);

	if (!dir) { 
		return NULL;
	}
```
Pertama membuka *directory* dan membacanya. Jika directory tidak bisa dibaca return `NULL`.
```c
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
```
Menyimpan path file dalam `pathfile`, jika path file merupakan direktori, masuk ke direktorinya lalu memproses file.
```c
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
				if (error) return NULL;
			}

			for (j = 0; j < idx; j++) { // Join semua thread
				pthread_join(fileThread[j], NULL);
			}

			idx = 0;
		}
	}
```
Nama file yang akan dikategorikan disimpan dalam `filepath`. Lalu mencek apakah `filepath` direktori atau file. Jika file, disimpan ke dalam `fileNameList`.
```c
	if (idx) { // Kalau masih ada file yang belom dikategorikan
		int j;
		for (j = 0; j < idx; j++) {
			error = pthread_create(&fileThread[j], NULL, moveFile, (void* ) fileNameList[j]);
			if (error) return NULL;
		}

		for (j = 0; j < idx; j++) {
			pthread_join(fileThread[j], NULL);
		}
	}

```
Jika terdapat file yang belum dikategorikan, diinputkan path file ke dalam `pthread_create`. Jika tidak berhasil, return `NULL`.

Fungsi `isDirectory` untuk mengecek apakah directory atau file:
```c
int isDirectory(const char* path) {
	struct stat statbuf;
	if (stat(path, &statbuf) != 0) return 0;
	return S_ISDIR(statbuf.st_mode);
}
```
Jika objek tidak ada maka bukan directory, return 0.
- 3.a. Program menerima opsi -f, jadi pengguna bisa menambahkan argumen file yang bisa dikategorikan sebanyak yang diinginkan oleh pengguna. 
```c
	if (strcmp(argv[1], "-f") == 0) {
		if (argc >= 3) {
                    int i;
		    int count = argc

		    for (i = 2; i < count; i++) {
			    pthread_create(&fileThread[idx], NULL, moveFile, (void *) argv[i]);
			    idx++;

			    if (idx == 4) {
				    int j;
				    for (j = 0; j < idx; j++){
					    pthread_join(fileThread[j], NULL);
                                    }

				    idx = 0;
			    }
		    }
        }
```
Untuk membaca opsi, menggunakan function `strcmp()` untuk membandingkan dengan opsi -f. Lalu looping dari argumen urutan ketiga sampai terakhir untuk menginput path pada `pthread_create`. Agar proses bisa berjalan lebih cepat, dibuat thread sebanyak 4 untuk berjalan secara parallel. Jika sudah sampai 4, thread diinput ke `pthread_join` untuk menunggu thread lain selesai diproses.

- 3.b. Program juga dapat menerima opsi -d untuk melakukan pengkategorian pada suatu directory. Namun pada opsi -d ini, user hanya bisa memasukkan input 1 directory saja, tidak seperti file yang bebas menginput file sebanyak mungkin. 
```c
	if (strcmp(argv[1], "-d") == 0) {
		if (argc = 3) {
            		moveDir((void* ) argv[2]); // kalo pake ./soal3 -d

        	if (note)
			printf("Direktori sukses disimpan!\n");
		else
			printf("Yah, gagal disimpan :(\n");
		}		
	}
```
Untuk membaca opsi, menggunakan function `strcmp()` untuk membandingkan dengan opsi -d. Karena hanya bisa satu direktori yang dimasukkan, maka jumlah argumen harus ada 3, termasuk `./soal3.c` dan `-d`. Jika arguman ada 3, panggil fungsi `moveDir()` dengan input path direktori. Note menyimpan integer untuk kode apakah direktori berhasil dibuat atau tidak. Jika note bernilai 0 artinya direktori tidak dibuat.

- 3.c. Selain menerima opsi-opsi di atas, program ini menerima opsi \*. Opsi ini akan mengkategorikan seluruh file yang ada di working directory ketika menjalankan program C tersebut.
```c
	if (strcmp(argv[1], "*") == 0) {
        	moveDir((void* ) ".");
```
Untuk membaca opsi, menggunakan function `strcmp()` untuk membandingkan dengan opsi \*. Lalu memanggil fungsi `moveDir`.

- 3.d. Semua file harus berada di dalam folder, jika terdapat file yang tidak memiliki ekstensi, file disimpan dalam folder “Unknown”. Jika file hidden, masuk folder “Hidden”.
Untuk file Hidden:
```c
		if (access("Hidden", F_OK) != 0)
			if (mkdir("Hidden", 0777) != 0) {
				printf("%s : Sad, gagal :(\n", file);
				return NULL;
			}

		char fullpath[128];
		strcpy(fullpath, "Hidden/");
		strcat(fullpath, file + 1);

		if (rename(fileName, fullpath) == 0)
			printf("%s : note Dikategorikan\n", file);
		else {
			printf("%s : Sad, gagal :(\n", file);
			return NULL;
		}
```
Untuk file Unknown:
```c
		if (access("Unknown", F_OK) != 0)
			if (mkdir("Unknown", 0777) != 0) {
				printf("%s : Sad, gagal :(\n", file);
				return NULL;
			}
			
		char fullpath[128];
		strcpy(fullpath, "Unknown/");
		strcat(fullpath, file);

		if (rename(fileName, fullpath) == 0)
			printf("%s : note Dikategorikan\n", file);
		else {
			printf("%s : Sad, gagal :(\n", file);
			return NULL;
		}
```
Penanganan file Unknown dan Hidden mirip seperti pemindahan file dengan ekstensi. Karena tidak ada ekstensinya maka direktorinya tidak perlu dikategorikan.

- 3.e. Setiap 1 file yang dikategorikan dioperasikan oleh 1 thread agar bisa berjalan secara paralel sehingga proses kategori bisa berjalan lebih cepat.

Setiap path dari satu argumen diinputkan ke dalam `pthread_create`, agar dioperasikan satu thread.

### Screenshot
- Isi working directory mula-mula:
![alt text](https://i.postimg.cc/Znm7Kx9N/Virtual-Box-Ubuntu-20-04-2-0-22-05-2021-18-44-16.png)

- Menjalankan opsi `-f`:
![alt text](https://i.postimg.cc/pXW9K5zJ/Virtual-Box-Ubuntu-20-04-2-0-22-05-2021-18-46-16.png)
Akan terbuat direktori sesuai ekstension file, dan file dimasukkan di dalam direktori ekstensionnya.

- Menjalankan opsi `-d`:
![alt text](https://i.postimg.cc/SKfFLRWc/Virtual-Box-Ubuntu-20-04-2-0-22-05-2021-18-47-03.png)
Semua file yang ada di dalam direktori awal akan dipindahkan ke direktori sesuai ekstensionnya di working directory.

- Menjalankan opsi `*`:
![alt text](https://i.postimg.cc/sgn03W96/Virtual-Box-Ubuntu-20-04-2-0-22-05-2021-18-47-44.png)
Awalnya file `soal3.c` masih di luar direktori. Setelah dijalankan file berpindah ke dalam direktori "c".

- Isi direktori Unknown:
![alt text](https://i.postimg.cc/SK2VqvKR/Virtual-Box-Ubuntu-20-04-2-0-22-05-2021-18-48-29.png)

### Kendala
- Kesulitan dalam mengubah direktori awal file ke direktori ekstensionnya
- Membuat direktori baru di working direktori
- Mengetahui bahwa suatu file bersifat Unknown

