# soal-shift-sisop-modul-3-D08-2021


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

