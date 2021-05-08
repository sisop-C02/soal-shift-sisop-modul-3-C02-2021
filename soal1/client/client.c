#include <stdio.h>	//printf
#include <string.h>	//strlen
#include <sys/socket.h>	//socket
#include <arpa/inet.h>  //inet_addr
#include <unistd.h>
#include <stdbool.h>

int connection_start();
int intro();
void procedures();
void soal_c();

int sock;
int reply_size = 2000;
struct sockaddr_in server;
char message[1000] , server_reply[2000], proced_reply[2000];
char userid[20], password[20];
bool logged_in = false;

int main(int argc , char *argv[])
{
	
	bool isOccupied = false;
	if( connection_start())
		return 1;
	// read(sock, &isOccupied, sizeof(isOccupied));
	// printf("%d\n", isOccupied);
	// if (isOccupied) {
	// 	printf("mohon tunggu\n");
	// 	while(isOccupied) {
	// 		read(sock, &isOccupied, sizeof(isOccupied));
	// 	}
	// }
	if(intro())
		return 1;

	bool incorrect = true;
	while(incorrect)
	{	
		//Kirim pilihan
		if( send(sock , message , strlen(message) , 0) < 0 ) {
			puts("Send failed");
			return 1;
		}
		//Terima balasan
		if( recv(sock , server_reply , reply_size , 0) < 0 ) {
			puts("recv failed");
			break;
		}

		if (!strcmp(server_reply, "err")) {
			printf("Input salah, silahkan pilih antara [login] atau [register]\n");
			printf("Masukkan pilihan : ");
			scanf("%s" , message);
		} else {
			incorrect = false;
			procedures();
			recv(sock , proced_reply , reply_size , 0);
			printf("%s\n", proced_reply);
			if (!strcmp(proced_reply, "login sukses"))
				logged_in = true;
			// if (strcmp(proced_reply, "gagal")) {
			// 	intro();
			// 	incorrect = true;
			// }
		}
		memset(server_reply,0,reply_size);
	}

	if (logged_in) {
		soal_c();
	}
	
	close(sock);
	return 0;
}


int connection_start() {
	//Create socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1)
		printf("Could not create socket");
	puts("Socket created");
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons( 8888 );
	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0) {
		perror("connect failed. Error");
		return 1;
	}
	puts("Connected\n");
	return 0;
}

int intro() {
	//Intro
	if( recv(sock , server_reply , reply_size , 0) < 0) {
		puts("recv failed");
		return 1;
	} else {
		puts(server_reply);
		memset(server_reply,0,reply_size);
		printf("Masukkan pilihan : ");
		scanf("%s" , message);
		return 0;
	}
}

void procedures() {
	printf("\nsilahkan masukkan username: ");
	scanf("%s", userid); strcat(userid,":");
	printf("silahkan masukkan password: ");
	scanf("%s", password); strcat(userid, password);

	char credentials[50];
	if (!strcmp(message, "register")) {
		strcpy(credentials, "r");
		strcat(credentials, userid);
	} else {
		strcpy(credentials, "l");
		strcat(credentials, userid);
	}
	send(sock , credentials , strlen(credentials) , 0);
	return;
}

void soal_c() {
	printf("\nsilahkan masukkan command: ");
	char comm[20]; scanf("%s", comm);

	if (!strcmp(comm,"add")) {
		char temp[50], add_tsv[200];
		printf("Publisher: "); scanf("%s", temp);
		strcpy(add_tsv, temp); strcat(add_tsv, "\t");
		printf("Tahun publikasi: "); scanf("%s", temp);
		strcat(add_tsv, temp); strcat(add_tsv, "\t");
		printf("Filepath: "); scanf("%s", temp);
		strcat(add_tsv, temp);
		//mengirim isi command
		send(sock , add_tsv , strlen(add_tsv) , 0);
	}

}