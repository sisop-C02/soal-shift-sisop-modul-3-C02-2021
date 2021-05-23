# lapres-sisop-modul-2-C02-2021
Laporan resmi berisi dokumentasi soal shift Sisop Modul 2.
---
Kelompok C-02:
- [Jason Andrew Gunawan](https://github.com/jasandgun): 05111940000085
- [Muchamad Maroqi Abdul Jalil](https://github.com/maroqijalil): 05111940000143
- [Muhammad Zhafran Musyaffa](https://github.com/franszhafran): 05111940000147
---

## Soal 2
### Penjelasan Soal.
Crypto (kamu) adalah teman Loba. Suatu pagi, Crypto melihat Loba yang sedang kewalahan mengerjakan tugas dari bosnya. Karena Crypto adalah orang yang sangat menyukai tantangan, dia ingin membantu Loba mengerjakan tugasnya. Detil dari tugas tersebut adalah:
- (a) Membuat program perkalian matrix (4x3 dengan 3x6) dan menampilkan hasilnya. Matriks nantinya akan berisi angka 1-20 (tidak perlu dibuat filter angka).
- (b) Membuat program dengan menggunakan matriks output dari program sebelumnya (program soal2a.c) (Catatan!: gunakan shared memory). Kemudian matriks tersebut akan dilakukan perhitungan dengan matrix baru sebagai berikut contoh perhitungan untuk matriks yang ada. Perhitungannya adalah setiap cel yang berasal dari matriks A menjadi angka untuk faktorial, lalu cel dari matriks B menjadi batas maksimal faktorialnya (dari paling besar ke paling kecil) (Catatan!: gunakan thread untuk perhitungan di setiap cel).
- (c) Karena takut lag dalam pengerjaannya membantu Loba, Crypto juga membuat program (soal2c.c) untuk mengecek 5 proses teratas apa saja yang memakan resource komputernya dengan command “ps aux | sort -nrk 3,3 | head -5” (Catatan!: Harus menggunakan IPC Pipes)

### Solusi dan Penjelasannya
Berikut jawaban per poin untuk soal nomor 2:
- (a) Solusi untuk poin ini terdapat pada `soal2a.c`.
> Meminta input matrix pertama dari user melalui terminal.
```cpp
int row1 = 4;
int col1 = 3;
int matrix_a[row1][col1];
for (int i = 0; i < row1; i++) {
  for (int j = 0; j < col1; j++) {
    scanf("%d", &matrix_a[i][j]);
  }
}
```
> Meminta input matrix kedua dari user melalui terminal.
```cpp
int row2 = 3;
int col2 = 6;
int row_result = 4;
int col_result = 6;
int matrix_result[4][6];
for (int i = 0; i < row2; i++) {
  for (int j = 0; j < col2; j++) {
    int value;
    scanf("%d", &value);

    ...
  }
}
```
> Melakukan operasi perkalian kedua matrix sembari menerima input dari user.
```cpp
int row2 = 3;
int col2 = 6;
int row_result = 4;
int col_result = 6;
int matrix_result[4][6];
for (int i = 0; i < row2; i++) {
  for (int j = 0; j < col2; j++) {
    int value;
    scanf("%d", &value);

    
    for (int k = 0; k < row1; k++) {
      if (i == 0) {
        matrix_result[k][j] = 0;  
      }
      matrix_result[k][j] += matrix_a[k][i] * value;
    }
  }
}
```
> Mendeklarasikan shared memroy dengan key `911`.
```cpp
key_t sh_memory_key = 911;
int sh_memory_id = shmget(sh_memory_key, sizeof(int[row_result][col_result]), IPC_CREAT | 0666);
if (sh_memory_id == -1) {
  perror("shmget");
  exit(EXIT_FAILURE);
}
```
> Mengisi nilai matrix hasil perkalian ke shared memroy pada deklarasi sebelumnya.
```cpp
for (int i = 0; i < row_result; i++) {
  for (int j = 0; j < col_result; j++) {
    result[i][j] = matrix_result[i][j];
    printf("%d ", result[i][j]);
  }
  printf("\n");
}
```
> Menghapus alokasi memori yang digunakan untuk matriks tadi.
```cpp
shmdt(result);
```
Untuk melihat kode program lengkapnya, [klik disini](/soal2/soal2a.c).

- (b) Solusi untuk poin ini terdapat pada `soal2b.c`.
> Meminta input matrix pertama dari user melalui terminal yang memiliki ukuran (4x6) sesuai dengan ukuran pada matrix hasil program poin (a).
```cpp
int row_result = 4;
int col_result = 6;
int matrix_input[row_result][col_result];
for (int i = 0; i < row_result; i++) {
  for (int j = 0; j < col_result; j++) {
    scanf("%d", &matrix_input[i][j]);
  }
}
```
> Mendeklarasikan shared memroy dengan key `911`.
```cpp
key_t sh_memory_key = 911;
int sh_memory_id = shmget(sh_memory_key, sizeof(int[row_result][col_result]), IPC_CREAT | 0666);
if (sh_memory_id == -1) {
  perror("shmget");
  exit(EXIT_FAILURE);
}
```
> Mengambil nilai matrix hasil perkalian dari program poin (a).
```cpp
result = shmat(sh_memory_id, NULL, 0);
if (result == (void *) -1) {
  perror("shmat");
  exit(EXIT_FAILURE);
}
```
> Mendeklarasikan sebuah struct untuk menyimpan pesan yang akan dikirimkan ke setiap thread yang akan dibuat.
```cpp
typedef struct {
  int pos_x;
  int pos_y;
  int fact_limit;
} fact_args;
```
> Mendeklarasikan sebuah fungsi untuk melakukan operasi per-cell yang akan dioperasikan dalam setiap thread.
```cpp
void * count_factorial(void * args) {
  fact_args * fact_item = args;

  if (fact_item->fact_limit == 0) {
    result[fact_item->pos_x][fact_item->pos_y] = 0;
  } else {
    int num = result[fact_item->pos_x][fact_item->pos_y];
    int fact_num = num;
    
    for (int i = 1; i < fact_item->fact_limit; i++) {
      if ((--num) == 0) {
        break;
      } else {
        fact_num *= num;
      }
    }

    result[fact_item->pos_x][fact_item->pos_y] = fact_num;
  }

  free(fact_item);
  threads_mark[fact_item->pos_x][fact_item->pos_y] = 1;
}
```
> Membuat thread sebanyak cell pada matrix hasil perkalian program sebelumnya untuk melanjutkan operasi faktorial pada setiap cell.
```cpp
int fact_thread_index = 0;
pthread_t fact_thread[row_result * col_result];
for (int i = 0; i < row_result; i++) {
  for (int j = 0; j < col_result; j++) {
    fact_args * args = malloc(sizeof(fact_args *));
    args->fact_limit = matrix_input[i][j];
    args->pos_x = i;
    args->pos_y = j;

    if (pthread_create(&fact_thread[fact_thread_index++], NULL, count_factorial, args)) {
      free(args);
    }
  }
}
```
> Melakukan join pada setiap thread untuk menunggu hasil dari keseluruhan cell keluar dengan sesuai.
```cpp
fact_thread_index = 0;
for (int i = 0; i < row_result; i++) {
  for (int j = 0; j < col_result; j++) {
    pthread_join(fact_thread[fact_thread_index++], NULL);
  }
}
```
> Mencetak hasil operasi faktorial.
```cpp
for (int i = 0; i < row_result; i++) {
  for (int j = 0; j < col_result; j++) {
    printf("%d ", result[i][j]);
  }
  printf("\n");
}
```
> Menghapus isi dari shared memory pada key `911`.
```cpp
shmdt(result);
shmctl(sh_memory_id, IPC_RMID, NULL);
```
Untuk melihat kode program lengkapnya, [klik disini](/soal2/soal2b.c).

- (c) Solusi untuk poin ini terdapat pada `soal2c.c`.
> Mendeklarasikan pipe untuk menuliskan output perintah 1 ke pipe kedua.
```cpp
int pipe_ps_to_sort[2];
if (pipe(pipe_ps_to_sort) == -1) {
  perror("pipe failed");
  exit(EXIT_FAILURE);
}
```
> Mendeklarasikan pipe untuk menuliskan output perintah 2 untuk dibaca ketika memanggil perintah 3.
```cpp
int pipe_sort_to_head[2];
if (pipe(pipe_sort_to_head) == -1) {
  perror("pipe failed");
  exit(EXIT_FAILURE);
}
```
> Menuliskan hasil perintah pertama ke pipe untuk nanti dibaca oleh pipe ini dan dijadikan input untuk perintah 2.
```cpp
if (fork() == 0) {
  close(pipe_ps_to_sort[0]);
  dup2(pipe_ps_to_sort[1], 1);
  close(pipe_ps_to_sort[1]);
  
  char * args[] = {"ps", "aux", NULL};
  execv("/bin/ps", args);
}
```
> Membaca hasil perintah pertama pada pipe pertama dan menuliskan hasil perintah kedua ke pipe untuk nanti dibaca oleh pipe ini dan dijadikan input untuk perintah 3.
```cpp
if (fork() == 0) {
  close(pipe_ps_to_sort[1]);
  dup2(pipe_ps_to_sort[0], 0);
  close(pipe_ps_to_sort[0]);
  
  close(pipe_sort_to_head[0]);
  dup2(pipe_sort_to_head[1], 1);
  close(pipe_sort_to_head[1]);
  
  char * args[] = {"sort", "-nrk", "3,3", NULL};
  execv("/bin/sort", args);
}
```
> Membaca hasil perintah kedua pada pipe kedua dan menjadikannya input pada perintah 3.
```cpp
close(pipe_ps_to_sort[0]);
close(pipe_ps_to_sort[1]);

close(pipe_sort_to_head[1]);
dup2(pipe_sort_to_head[0], 0);
close(pipe_sort_to_head[0]);

char * args[] = {"head", "-5", NULL};
execv("/bin/head", args);
```
Untuk melihat kode program lengkapnya, [klik disini](/soal2/soal2c.c).

### Dokumentasi dan Kendala
> Tidak terlalu ada kendala yang berarti selama pengerjaan soal 2 ini. Hanya saja, terdapat kesalahan-kesalahan sintaks yang menyebabkan error sesaat tetapi tidak sempat mendokumentasikannya.
