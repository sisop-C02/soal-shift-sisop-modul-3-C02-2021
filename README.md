# lapres-sisop-modul-3-C02-2021
Laporan resmi berisi dokumentasi soal shift Sisop Modul 3.
---
Kelompok C-02:
- [Jason Andrew Gunawan](https://github.com/jasandgun): 05111940000085
- [Muchamad Maroqi Abdul Jalil](https://github.com/maroqijalil): 05111940000143
- [Muhammad Zhafran Musyaffa](https://github.com/franszhafran): 05111940000147
---


## Soal 1
### Penjelasan Soal
Keverk adalah orang yang cukup ambisius dan terkenal di angkatannya. Sebelum dia menjadi ketua departemen di HMTC, dia pernah mengerjakan suatu proyek dimana keverk tersebut meminta untuk membuat server database buku. Proyek ini diminta agar dapat digunakan oleh pemilik aplikasi dan diharapkan bantuannya dari pengguna aplikasi ini. 

Di dalam proyek itu, Keverk diminta: 

- (a) Pada saat client tersambung dengan server, terdapat dua pilihan pertama, yaitu register dan login. Jika memilih register, client akan diminta input id dan passwordnya untuk dikirimkan ke server. User juga dapat melakukan login. Login berhasil jika id dan password yang dikirim dari aplikasi client sesuai dengan list akun yang ada didalam aplikasi server. Sistem ini juga dapat menerima multi-connections. Koneksi terhitung ketika aplikasi client tersambung dengan server. Jika terdapat 2 koneksi atau lebih maka harus menunggu sampai client pertama keluar untuk bisa melakukan login dan mengakses aplikasinya. Keverk menginginkan lokasi penyimpanan id dan password pada file bernama **akun.txt** dengan format:

**akun.txt**
```
id:password
id2:password2
```

- (b) Sistem memiliki sebuah database yang bernama **files.tsv**. Isi dari files.tsv ini adalah **path file saat berada di server, publisher, dan tahun publikasi**. Setiap penambahan dan penghapusan file pada folder file yang bernama **FILES** pada server akan memengaruhi isi dari **files.tsv**. Folder **FILES** otomatis dibuat saat server dijalankan.

- (c) Tidak hanya itu, Keverk juga diminta membuat fitur agar client dapat menambah file baru ke dalam server. Direktori **FILES** memiliki struktur direktori di bawah ini: 

Direktori **FILES** 
```
File1.ekstensi
File2.ekstensi
```
Pertama client mengirimkan input ke server dengan struktur sebagai berikut :

Contoh Command Client :
```
add
```
Output Client Console:
```
Publisher:
Tahun Publikasi:
Filepath:
```
Kemudian, dari aplikasi client akan dimasukan data buku tersebut (perlu diingat bahwa Filepath ini merupakan **path file yang akan dikirim ke server**). Lalu client nanti akan melakukan pengiriman file ke aplikasi server dengan menggunakan socket. Ketika file diterima di server, maka row dari files.tsv akan bertambah sesuai dengan data terbaru yang ditambahkan.

- (d) Dan client dapat mendownload file yang telah ada dalam folder FILES di server, sehingga sistem harus dapat mengirim file ke client. Server **harus** melihat dari files.tsv untuk melakukan pengecekan apakah file tersebut valid. Jika tidak valid, maka mengirimkan pesan error balik ke client. Jika berhasil, file akan dikirim dan akan diterima ke client di **folder client** tersebut.

Contoh Command client
```
download TEMPfile.pdf
```

- (e) Setelah itu, client juga dapat menghapus file yang tersimpan di server. Akan tetapi, Keverk takut file yang dibuang adalah file yang penting, maka file hanya akan diganti namanya menjadi ```old-NamaFile.ekstensi```. Ketika file telah diubah namanya, maka row dari file tersebut di file.tsv akan terhapus.

Contoh Command Client:
```
delete TEMPfile.pdf
```

- (f) Client dapat melihat semua isi files.tsv dengan memanggil suatu perintah yang bernama see. Output dari perintah tersebut keluar dengan format. 
Contoh Command Client:
```
see
```
Contoh Format Output pada Client:
```
Nama:
Publisher:
Tahun publishing:
Ekstensi File : 
Filepath : 

Nama:
Publisher:
Tahun publishing:
Ekstensi File : 
Filepath : 

```

- (g) Aplikasi client juga dapat melakukan pencarian dengan memberikan suatu string. Hasilnya adalah semua nama file yang mengandung string tersebut. Format output seperti format output f. 
Contoh Command Client:
```
find TEMP
```

- (h) Dikarenakan Keverk waspada dengan pertambahan dan penghapusan file di server, maka Keverk membuat suatu log untuk server yang bernama **running.log**. Contoh isi dari log ini adalah 
**running.log**
```
Tambah : File1.ektensi (id:pass)
Hapus : File2.ektensi (id:pass)
```

Akan tetapi, kevek saat itu masih belum paham dengan sistem server dan client. Bantulah Keverk untuk menyelesaikan proyek ini!


**Note:**
- Dilarang menggunakan system() dan execv(). Silahkan dikerjakan sepenuhnya dengan thread dan socket programming.
- Untuk download dan upload silahkan menggunakan file teks dengan ekstensi dan isi bebas (yang ada isinya bukan touch saja dan tidak kosong) dan requirement untuk benar adalah percobaan dengan minimum 5 data.

**Struktur Direktori:**
```
.
├── Client
│   ├── client.c
│   ├── File2.extensi
│   └── File1.extensi
└── Server
    ├── akun.txt
    ├── files.tsv
    ├── server.c
    ├── running.log
    └── FILES
            ├── File2.extensi
            └── File1.ekstensi

```


### Solusi dan Penjelasannya

#### 1(a) Multiple clients, login, register
Untuk mengatur multiple clients, digunakan variabel ```isOccupied``` yang akan menjadi true apabila sudah ada client yang terhubung. Apabila tidak ada client yang terhubung (kosong), server akan mengirimkan pesan ```free``` sebagai string. Client akan berjalan sesuai dengan ada atau tidaknya string free tersebut.
```c
// server.c
void *connection_handler(void *socket_desc)
{
	//Get the socket descriptor
	int sock = *(int*)socket_desc;
	int read_size;
	char client_message[2000]; memset(client_message,0,sizeof(client_message));
	char credentials[50]; memset(credentials,0,50);
	
	// Cek multiple clients
	while(isOccupied);
	isOccupied = true;

	// Kirim ke client
	send(sock , "free" , strlen("free") , 0);
	while(true) {
        ...
        // Jika exit
        isOccupied = false;
        ...
    }
    ...
}
```
```c
// client.c
bool check_empty() {
	// Check availability
	char empty[1000] = {0}, usrcreds[101] = {0};
	recv(sock , empty , sizeof(empty) , 0);
	while(strcmp(empty, "free")) {
		memset(empty, 0, sizeof(empty));
		recv(sock , empty , sizeof(empty) , 0);
	}
	memset(empty, 0, sizeof(empty));
	return true;
}
```

![Screenshot from 2021-05-23 14-54-33](https://user-images.githubusercontent.com/55174646/119256177-92d2e180-bbe9-11eb-8c12-55b3bee16b9a.png)

![Screenshot from 2021-05-23 14-54-53](https://user-images.githubusercontent.com/55174646/119256188-a54d1b00-bbe9-11eb-8381-5504bfb65261.png)


Proses login dan register adalah sebagai berikut.
```c
// server.c
...
    if (credentials[0] == 'r') {
        status = regis(credentials, sock);
    } else {
        status = login(credentials, sock);
    }
...
bool login(char credentials[], int sock) {
	...
	FILE *fptr;
    fptr = fopen("akun.txt", "a+");
    while(fscanf(fptr, "%s\n", listofcreds[counter]) != EOF)
    	counter++;
    fclose(fptr);
    for (int i = 0; i < counter; ++i) {
        if (!strcmp(listofcreds[i], cleancreds)) {
            char *msg_success = "sukses";
            get_client(cleancreds);
            send(sock , msg_success , strlen(msg_success), 0);
            return true;
        }
    }
    ...
}
bool regis(char credentials[], int sock) {
    ...
	FILE *fptr;
    fptr = fopen("akun.txt", "a+");
    while(fscanf(fptr, "%s\n", listofcreds[counter]) != EOF)
    	counter++;
    // Cek apakah sudah ada username
    for (int i = 0; i < counter; ++i) {
    	if (strstr(listofcreds[i], usr_name) != NULL) {
    		fclose(fptr);
    		char *msg_gagal = "gagal";
    		send(sock , msg_gagal , strlen(msg_gagal), 0);
    		return false;
    	}
    }
    ...
}
```
```c
// client.c
...
    while(true) {
		// Initialize
		int command = greetings();
		
		// [3.] Exit
		if (command == 3) {
			puts("Terima kasih telah menggunakan aplikasi ini.\nSampai jumpa");
			send(sock , "exit" , strlen("exit") , 0);
			break;
		} 
		// [1.] dan [2.]
		else {
			memset(credentials,0,sizeof(credentials));
			procedures(command);
		}
		
		// Mengirim info user ke server
		puts("Mengirim data ke server...");
		send(sock , credentials , strlen(credentials) , 0);
	    ...
    }
...
int greetings() {
	int userchoice;
	printf("Selamat datang! Silahkan pilih salah satu dari opsi-opsi berikut\n");
	printf("[1.] Register\n");
	printf("[2.] Login\n");
	printf("[3.] Keluar\n");
	...
}
void procedures(int userchoice) {
	...
	// Menggabungkan input
	if (userchoice == 1) {
		strcpy(credentials, "r");
		strcat(credentials, userid);
	} else {
		strcpy(credentials, "l");
		strcat(credentials, userid);
	}
}
```

![Screenshot from 2021-05-23 14-58-33](https://user-images.githubusercontent.com/55174646/119256215-d88faa00-bbe9-11eb-89c6-e958292338c6.png)

![Screenshot from 2021-05-23 14-59-22](https://user-images.githubusercontent.com/55174646/119256415-dda12900-bbea-11eb-8c81-896ec81b0359.png)


#### 1(b) Folder files dan files.tsv
Membuat folder ```FOLDER``` serta ```files.tsv``` dan update ke array
```c
...
    // soalb
	strcpy(folderpath, server_path);
	mkdir(strcat(folderpath, "FOLDER/"), 0777);
	// tsv
	strcpy(tsvpath,server_path);
	strcat(tsvpath, "files.tsv");
	curr_numf = 0;
	update_tsv('r', "none");
...
void update_tsv(char command, char to_tsv[500]) {
	FILE *ftsv;

	if (command == 'r') {
		ftsv = fopen(tsvpath, "a+");
		char curr_path[500], curr_publ[50], curr_year[50];
		int counter = 0;
		while(fscanf(ftsv, "%s\t%s\t%s\n", curr_path, curr_publ, curr_year) != EOF) {
			strcpy(file_list[counter].path, curr_path);
		    strcpy(file_list[counter].publ, curr_publ);
		    strcpy(file_list[counter].year, curr_year);
		    counter++;
		}
		curr_numf = counter;
		fclose(ftsv);
	}
	...
}
```

![Screenshot from 2021-05-23 15-01-54](https://user-images.githubusercontent.com/55174646/119257274-b8aeb500-bbee-11eb-88d9-91e4b26f04dd.png)


#### 1(c) Add files
Menambahkan file dari client ke server, file berupa ```file.txt```. Ketika client menambahkan file, maka spesifikasi file akan ditambahkan ke ```files.tsv```. 
```c
// server.c
bool split_soal(int sock) {
	// Menerima isi dari command
	char chg_tsv[150], newstring[150];
	memset(newstring, 0, sizeof(newstring));
	recv(sock , newstring , 150 , 0);
    ...
	if (check_split == 'a') {
		puts("[client add]");
		fin_val = soal_c(sock, chg_tsv);
    }
    ...
}
bool soal_c(int sock, char add_tsv[]) {
	...
    // Prepare files
    FILE *src_ptr, *dest_ptr;
    src_ptr = fopen(src_pth, "r");
    if (src_ptr == NULL) {
    	puts("[failed]");
    	send(sock , "op_gagal" , strlen("op_gagal"), 0);
    	return false;
    }
    dest_ptr = fopen(dest_pth,"w");
    printf("%s\n", dest_pth);
    
    // Copy file
    char temp = fgetc(src_ptr);
	while(temp != EOF){
		fputc(temp, dest_ptr);
		temp = fgetc(src_ptr);
	}
    // Close file pointers
    fclose(src_ptr); fclose(dest_ptr);
    puts("[success]");
    send(sock , "op_sukses" , strlen("op_sukses"), 0);

    // Membuat file.tsv
    char to_tsv[200];
    sprintf(to_tsv, "%s\t%s\t%s", src_pth, curr_publ,curr_year);
    update_tsv('a', to_tsv);
    ...
}
void update_tsv(char command, char to_tsv[500]) {
	...
	else if (command == 'a') {
	    ftsv = fopen(tsvpath, "a+");
	    fprintf(ftsv, "%s\n", to_tsv);
	    fclose(ftsv);
	}
	...
}
```
```c
// client.c
void split_activities(int chosen_activity) {
	// Menerima perintah
	char comm[10];
	strcpy(comm, avail_activity[chosen_activity]);

	// Perintah add
	if (!strcmp(comm,"add")) {
		char temp[50], add_tsv[200];
		getchar();
		printf("Publisher: "); scanf("%s", temp);
		strcpy(add_tsv, temp); strcat(add_tsv, "\t");
		getchar();
		printf("Tahun publikasi: "); scanf("%s", temp);
		strcat(add_tsv, temp); strcat(add_tsv, "\t");
		getchar();
		printf("Filepath: "); scanf("%s", temp);
		strcat(add_tsv, temp);
		strcat(add_tsv, "a");
		//mengirim isi command
		send(sock , add_tsv , strlen(add_tsv) , 0);
		puts("\nMengirimkan file ke server...");
		//Menerima balasan server
		char activ_check[20] = {0}; memset(activ_check,0,sizeof(activ_check));
		recv(sock , activ_check , sizeof(activ_check) , 0);
		strcpy(curr_t, activ_check);
	}
    ...
}
```

![Screenshot from 2021-05-23 15-09-43](https://user-images.githubusercontent.com/55174646/119257308-dda32800-bbee-11eb-9485-bfa4d794c4d9.png)

![Screenshot from 2021-05-23 15-09-43](https://user-images.githubusercontent.com/55174646/119257313-e4319f80-bbee-11eb-8670-87fea28969c6.png)


#### 1(d) Download files
Menambahkan file dari server ke client. Akan diminta input berupa nama file. File berupa ```file.txt```. 
```c
// server.c
bool split_soal(int sock) {
	// Menerima isi dari command
	char chg_tsv[150], newstring[150];
	memset(newstring, 0, sizeof(newstring));
	recv(sock , newstring , 150 , 0);
    ...
	else if (check_split == 'd') {
		puts("[client download]");
		fin_val = soal_d(sock, chg_tsv);
	} 
	...
}
bool soal_d(int sock, char dow_tsv[]) {
	...
	// Action
	if (found) {
		// Prepare files
	    char temps[450]; 
	    strcpy(temps, folderpath); strcat(temps, name);
	    printf("%s\n", temps);
	    FILE *source, *destination;
	    source = fopen(temps, "r");
	    destination = fopen(seek_name,"w");
	    // Copy file
	    char tempc = fgetc(source);
		while(tempc != EOF){
			fputc(tempc, destination);
			tempc = fgetc(source);
		}
	    // Close file pointers
	    fclose(source); fclose(destination);
	    puts("[success]");
	    send(sock , "op_sukses" , strlen("op_sukses"), 0);
	    return true;

	} else {
		puts("[failed]");
		send(sock , "op_gagal" , strlen("op_gagal"), 0);
    	return false;
	}
}
```
```c
// client.c
void split_activities(int chosen_activity) {
	// Menerima perintah
	char comm[10];
	strcpy(comm, avail_activity[chosen_activity]);
    ...
    // Perintah download
	else if (!strcmp(comm,"download")) {
		// char temp_down[10]; scanf("%s", temp_down);
		char down_tsv[50];
		scanf("%s", down_tsv);
		strcat(down_tsv, "d");
		//mengirim isi command
		send(sock , down_tsv , strlen(down_tsv) , 0);
		puts("\nMenerima file dari server...");
		//Menerima balasan server
		char activ_check[20] = {0}; memset(activ_check,0,sizeof(activ_check));
		recv(sock , activ_check , sizeof(activ_check) , 0);
		strcpy(curr_t, activ_check);
	}
    ...
}
```
![Screenshot from 2021-05-23 15-20-01](https://user-images.githubusercontent.com/55174646/119258382-8c496780-bbf3-11eb-93a2-7d050e451b2e.png)

![Screenshot from 2021-05-23 15-21-14](https://user-images.githubusercontent.com/55174646/119258384-8e132b00-bbf3-11eb-8783-55c663b38869.png)

![Screenshot from 2021-05-23 15-21-32](https://user-images.githubusercontent.com/55174646/119258385-90758500-bbf3-11eb-96f4-9461449d12fd.png)


#### 1(e) Delete files
Menghapus file dari server. File yang dihapus akan di-rename dengan prefix ```old-```. Setelah sebuah file dihapus, spesifikasi file tersebut akan dihapus dari ```files.tsv```.
```c
// server.c
bool split_soal(int sock) {
	// Menerima isi dari command
	char chg_tsv[150], newstring[150];
	memset(newstring, 0, sizeof(newstring));
	recv(sock , newstring , 150 , 0);
    ...
	else if (check_split == 'e') {
		puts("[client delete]");
		fin_val = soal_e(sock, chg_tsv);
	}
    ...
}
bool soal_e(int sock, char del_tsv[]){
    ...
	// Action
	if (found) {
		// rename file
		char newname[300] = {0}, oldname[300] = {0};
		sprintf(newname, "%sold-%s", folderpath, name);
		sprintf(oldname, "%s%s", folderpath, name);
		rename(oldname, newname);

		// Menghapus isi array
		for (int i = location; i < curr_numf - 1; ++i) {
			strcpy(file_list[i].path, file_list[i+1].path);
			strcpy(file_list[i].publ, file_list[i+1].publ);
			strcpy(file_list[i].year, file_list[i+1].year);
		}
		curr_numf--;
		for (int i = 0; i < curr_numf; ++i)
		{
			printf("%s\t%s\t%s\n", file_list[i].path, file_list[i].publ, file_list[i].year);
		}
		
		update_tsv('w', "none");
        ...
	} 
	else {
		puts("[failed]");
		send(sock , "op_gagal" , strlen("op_gagal"), 0);
    	return false;
	}
}
```
```c
// client.c
void split_activities(int chosen_activity) {
	// Menerima perintah
	char comm[10];
	strcpy(comm, avail_activity[chosen_activity]);
    ...
    // Perintah delete
	else if (!strcmp(comm,"delete")) {
		char del_tsv[50];
		scanf("%s", del_tsv);
		strcat(del_tsv, "e");
		//mengirim isi command
		send(sock , del_tsv , strlen(del_tsv) , 0);
		puts("\nMenunggu proses delete file...");
		//Menerima balasan server
		char activ_check[20] = {0}; memset(activ_check,0,sizeof(activ_check));
		recv(sock , activ_check , sizeof(activ_check) , 0);
		strcpy(curr_t, activ_check);
	}
    ...
}
```

![Screenshot from 2021-05-23 15-22-56](https://user-images.githubusercontent.com/55174646/119259917-9e7ad400-bbfa-11eb-81f8-1be2a4a9c040.png)

![Screenshot from 2021-05-23 15-23-32](https://user-images.githubusercontent.com/55174646/119259920-a5094b80-bbfa-11eb-9d06-6124be2d25ca.png)


#### 1(f) See files
Menampilkan file yang tersedia di server, melalui isi dari ```files.tsv```.
```c
// server.c
bool split_soal(int sock) {
	// Menerima isi dari command
	char chg_tsv[150], newstring[150];
	memset(newstring, 0, sizeof(newstring));
	recv(sock , newstring , 150 , 0);
    ...
	else if (check_split == 's') {
		puts("[client see]");
		fin_val = soal_f(sock);
	}
    ...
}
bool soal_f(int sock){
	char multi_string[1000] = {0};
	memset(multi_string,0,1000);memset(multi_string,0,1000);
	char name[50], ext[10];
	for (int i = 0; i < curr_numf; ++i) {
		// nama
		strcpy(multi_string, "Nama: ");
		strcpy(name, get_name(i));
		strcat(multi_string, name);
		// publisher
		strcat(multi_string, "\nPublisher: ");
		strcat(multi_string, file_list[i].publ);
		// tahun
		strcat(multi_string, "\nTahun publishing: ");
		strcat(multi_string, file_list[i].year);
		// extensi
		strcat(multi_string, "\nEkstensi File: ");
		strcpy(ext, get_ext(i));
		strcat(multi_string, ext);
		// filepath
		strcat(multi_string, "\nFilepath: ");
		strcat(multi_string, file_list[i].path);
		// kirim string
		send(sock , multi_string , strlen(multi_string), 0);
		send(sock , "\n" , strlen("\n"), 0);
		
		printf("\n%s\n", multi_string);
		puts("sent");
		memset(multi_string,0,1000);
	}
	send(sock , "done" , strlen("done"), 0);
	puts("[success]");
	return true;
}
char* get_name(int name_idx) {
	char *temp;
	char namecopy[500]; strcpy(namecopy,file_list[name_idx].path);
	temp = strrchr(namecopy, '/');
	for (int i = 0; i < strlen(temp); ++i)
		temp[i] = temp[i+1];
  	return temp;
}
char* get_ext(int ext_idx) {
	char *temp;
	char extcopy[500]; strcpy(extcopy,file_list[ext_idx].path);
	temp = strrchr(extcopy, '.');
	for (int i = 0; i < strlen(temp); ++i)
		temp[i] = temp[i+1];
  	return temp;
}
```
```c
// client.c
void split_activities(int chosen_activity) {
	// Menerima perintah
	char comm[10];
	strcpy(comm, avail_activity[chosen_activity]);
    ...
    // Perintah see
	else if (!strcmp(comm,"see")) {
		//mengirim isi command
		send(sock , "sees" , strlen("sees") , 0);
		puts("Menunggu respon...\n");

		// Menerima dari server
		char see_reply[1000] = {0}; 
		memset(see_reply,0,1000);
		recv(sock , see_reply , sizeof(see_reply) , 0);
		while(strcmp(see_reply, "done") != 0) {
			printf("\n%s", see_reply);
			memset(see_reply,0,1000);
			recv(sock , see_reply , sizeof(see_reply) , 0);
		}
		strcpy(curr_t, see_reply);
		return;
	}
    ...
}
```

![Screenshot from 2021-05-23 15-26-08](https://user-images.githubusercontent.com/55174646/119259933-b5212b00-bbfa-11eb-9c4b-e043310ebe59.png)

![Screenshot from 2021-05-23 15-26-17](https://user-images.githubusercontent.com/55174646/119259936-b94d4880-bbfa-11eb-8102-e5aee88e3a4c.png)


#### 1(g) Find files
Menampilkan file yang tersedia di server, melalui isi dari ```files.tsv```. File yang ditampilkan sesuai dengan query yang diberikan oleh client.
```c
// server.c
bool split_soal(int sock) {
	// Menerima isi dari command
	char chg_tsv[150], newstring[150];
	memset(newstring, 0, sizeof(newstring));
	recv(sock , newstring , 150 , 0);
    ...
	else if (check_split == 'f') {
		puts("[client find]");
		fin_val = soal_g(sock, chg_tsv);
	}
    ...
}
bool soal_g(int sock, char find_tsv[]){
	// Menyimpan nama file
	char temp_name[50]; strcpy(temp_name, find_tsv);
	bool found = false;
	char multi_string[1000] = {0};memset(multi_string,0,1000);
	for (int i = 0; i < curr_numf; ++i) {
		char name[50], ext[10];
		strcpy(name, get_name(i));
		printf("t:%s n:%s\n", temp_name, name);
		if (strstr(name, temp_name) != NULL) {
			found = true;
			// nama
			strcpy(multi_string, "Nama: ");
			strcat(multi_string, name);
			// publisher
			strcat(multi_string, "\nPublisher: ");
			strcat(multi_string, file_list[i].publ);
			// tahun
			strcat(multi_string, "\nTahun publishing: ");
			strcat(multi_string, file_list[i].year);
			// extensi
			strcat(multi_string, "\nEkstensi File: ");
			strcpy(ext, get_ext(i));
			strcat(multi_string, ext);
			// filepath
			strcat(multi_string, "\nFilepath: ");
			strcat(multi_string, file_list[i].path);
			strcat(multi_string,"\n");
			// kirim string
			send(sock , multi_string , strlen(multi_string), 0);
			memset(multi_string,0,1000);
		} else {
			continue;
		}
	}
	if (found) {
		send(sock , "done" , strlen("done"), 0);
		puts("[success]");
		// send(sock , "op_sukses" , strlen("op_sukses"), 0);
		// send(sock , "op_sukses" , strlen("op_sukses"), 0);
		// puts("[success]");
		return true;
	} else {
		send(sock , "op_gagal" , strlen("op_gagal"), 0);
		send(sock , "op_gagal" , strlen("op_gagal"), 0);
		puts("[failed]");
		return false;
	}
}
```
```c
// client.c
void split_activities(int chosen_activity) {
	// Menerima perintah
	char comm[10];
	strcpy(comm, avail_activity[chosen_activity]);
    ...
    // Perintah find
	else if (!strcmp(comm,"find")) {
		char find_tsv[50];
		scanf("%s", find_tsv);
		strcat(find_tsv, "f");
		//mengirim isi command
		send(sock , find_tsv , strlen(find_tsv) , 0);

		// Menerima dari server
		char see_reply[1000] = {0}; memset(see_reply,0,1000);
		recv(sock , see_reply , sizeof(see_reply) , 0);
		while(strcmp(see_reply, "done") != 0) {
			printf("\n%s", see_reply);
			memset(see_reply,0,1000);
			recv(sock , see_reply , sizeof(see_reply) , 0);
		}
		strcpy(curr_t, see_reply);
		return;

	}
    ...
}
```

![Screenshot from 2021-05-23 15-27-28](https://user-images.githubusercontent.com/55174646/119259954-cd914580-bbfa-11eb-8025-7687bb0aa2e7.png)

![Screenshot from 2021-05-23 15-28-53](https://user-images.githubusercontent.com/55174646/119259959-d97d0780-bbfa-11eb-8c45-7a5f865a27cb.png)


#### 1(h) Log
Mencatat perubahan yang terjadi di server, setelah add dan delete.
```c
// server.c
bool soal_c(int sock, char add_tsv[]) {
    ...
    //LOG
    FILE *log_ptr;
    log_ptr = fopen(logpath,"a+");
    char log_add[500];
    sprintf(log_add, "Tambah: %s (%s:%s)", curr_path, curr_client.username, curr_client.password);
    fprintf(log_ptr, "%s\n", log_add);
    fclose(log_ptr);
    ...
}
bool soal_e(int sock, char del_tsv[]){
    ...
    //LOG
    FILE *log_ptr;
    log_ptr = fopen(logpath,"a+");
    char log_add[500];
    sprintf(log_add, "Hapus: %s (%s:%s)", name, curr_client.username, curr_client.password);
    fprintf(log_ptr, "%s\n", log_add);
    fclose(log_ptr);
    ...
}
```

![Screenshot from 2021-05-23 15-30-02](https://user-images.githubusercontent.com/55174646/119260198-f108c000-bbfb-11eb-96a7-039035b1292c.png)
![Screenshot from 2021-05-23 15-30-17](https://user-images.githubusercontent.com/55174646/119260204-f6660a80-bbfb-11eb-8b87-33faa8413d8d.png)
![Screenshot from 2021-05-23 15-31-29](https://user-images.githubusercontent.com/55174646/119260207-fb2abe80-bbfb-11eb-972c-c97c9369834c.png)

### Kendala
> Terdapat kendala karena waktu untuk praktikum bersamaan dengan waktu untuk mengerjakan ETS. Sehingga hanya dapat mengerjakan 1a-1c sebelum revisi. 


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
