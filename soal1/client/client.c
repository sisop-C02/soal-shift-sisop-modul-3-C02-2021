#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

int connection_start();
bool check_empty();
int greetings();
void procedures(int);
int activities();
void split_activities(int);

// Global variable
int sock;
char credentials[101];
char curr_t[101] = {0};
const char * avail_activity[] = {
    "add",
    "download",
    "delete",
    "see",
    "find"
};

int main(int argc , char *argv[])
{
	// Connection attempt
	if( connection_start() )
		return 1;
	// Availability check
	bool isEmpty = check_empty();

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
		
		// Menerima balasan server
		char creds_check[1000] = {0};
		recv(sock , creds_check , sizeof(creds_check) , 0);
		
		// Handle balasan server
		if (!strcmp(creds_check, "gagal"))
		{
			if (command == 1) {
				puts("\nRegister gagal, username sudah tersedia.\nMengembalikan ke halaman awal...\n");
				continue;
			} else {
				puts("\nLogin gagal, username atau password salah.\nMengembalikan ke halaman awal...\n");
				continue;
			}
		} else {
			if (command == 1) {
				puts("\nRegister berhasil! Silahkan kembali login.\nMengembalikan ke halaman awal...");
				continue;
			} 
			else {
				// Setelah login
				puts("\nLogin sukses!");
				// Username
				char curr_uname[50] = {0}; memset(curr_uname,0,sizeof(curr_uname));
				recv(sock , curr_uname , sizeof(curr_uname) , 0);
				printf("Selamat datang, %s\n", curr_uname);
				while(true) 
				{
					// Opsi yang dimiliki pengguna
					int chosen_activity = activities();
					if (chosen_activity == 6) {
						puts("Anda berhasil logout.\n");
						send(sock , "logout" , strlen("logout") , 0);
						break;
					}
					else {
						printf("\nAnda telah memilih: %s\n", avail_activity[chosen_activity - 1]);
						send(sock , avail_activity[chosen_activity - 1] , strlen(avail_activity[chosen_activity - 1]) , 0);
						split_activities(chosen_activity - 1);
						
						if (!strcmp("op_sukses", curr_t) || !strcmp("done", curr_t)) {
							memset(curr_t,0,sizeof(curr_t));
							puts("Perintah sukses dijalankan!\nMengembalikan ke halaman perintah...");
							continue;
						} else if (!strcmp("op_gagal", curr_t)){
							puts("Perintah gagal dijalankan!\nSilahkan coba lagi.");
							continue;
						}
					}
				}
				continue;
			}
		}
	}
	
	close(sock);
	return 0;
}

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

int connection_start() {
	//Create socket
	struct sockaddr_in server;
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1)
		printf("Could not create socket");

	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons( 8888 );
	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0) {
		perror("Connection failed.");
		return 1;
	}
	puts("[connected]");
	puts("Anda terhubung, mohon tunggu sejenak.\n");
	return 0;
}

int greetings() {
	int userchoice;
	printf("Selamat datang! Silahkan pilih salah satu dari opsi-opsi berikut\n");
	printf("[1.] Register\n");
	printf("[2.] Login\n");
	printf("[3.] Keluar\n");
	printf("Masukkan angka yang anda pilih: "); scanf("%d", &userchoice);

	while (userchoice < 1 || userchoice > 3) {
		printf("Angka pilihan tidak tersedia. Silahkan input ulang.\n");
		printf("Masukkan angka yang anda pilih: "); scanf("%d", &userchoice);
	}
	return userchoice;
}

void procedures(int userchoice) {
	// Menampilkan layar yang dipilih
	if (userchoice == 1)
		puts("\nREGISTER");
	else
		puts("\nLOGIN");
	
	// Variables
	char userid[50], password[50];
	
	// Menerima input
	printf("username: ");
	scanf("%s", userid); strcat(userid,":");
	printf("password: ");
	scanf("%s", password); strcat(userid, password);
	
	// Menggabungkan input
	if (userchoice == 1) {
		strcpy(credentials, "r");
		strcat(credentials, userid);
	} else {
		strcpy(credentials, "l");
		strcat(credentials, userid);
	}
}

int activities() {
	// Proses pemilihan opsi
	puts("\nSilahkan pilih salah satu dari perintah berikut.");
	printf("[1.] Add\n[2.] Download\n");
	printf("[3.] Delete\n[4.] See\n");
	printf("[5.] Find\n[6.] Logout\n");
	int userchoice;
	printf("Masukkan angka yang anda pilih: "); scanf("%d", &userchoice);
	while (userchoice < 1 || userchoice > 6) {
		printf("Angka pilihan tidak tersedia. Silahkan input ulang.\n");
		printf("Masukkan angka yang anda pilih: "); scanf("%d", &userchoice);
	}
	return userchoice;
}

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

}