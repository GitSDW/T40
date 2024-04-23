#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#include <fcntl.h>
#include <sys/types.h>

// #include "udp.h"
#include "audio.h"
#include "global_value.h"
#include "video.h"
#include "c_util.h"

char *mypc_ip = "192.168.0.113";
const char *dev_ip 	= "192.168.0.113";

int send_sock = 0, recv_sock = 0;
struct sockaddr_in send_ai_serverAddr;	// Audio In 	-> Udp Out
struct sockaddr_in recv_ao_serverAddr;	// Udp In 		-> Audio Out
struct sockaddr_in send_vm_serverAddr;	// Vedio Main 	-> Udp Out
struct sockaddr_in send_vb_serverAddr;	// Vedio Box  	-> Udp Out

struct sockaddr_in client_addr;
socklen_t client_len = sizeof(client_addr);

int init_udp(uint8_t index, uint16_t host) {
	if (ip[0] != 0)	mypc_ip = ip;
	if (index == RECV_INDEX) {
		recv_sock = socket(AF_INET, SOCK_DGRAM, 0);
		if (recv_sock == -1) {
			perror("Erro Creating Recv Audio Out Socket");
			return -1;
		}
		memset(&recv_ao_serverAddr, 0, sizeof(recv_ao_serverAddr));
		recv_ao_serverAddr.sin_family = AF_INET;
		recv_ao_serverAddr.sin_port = htons(host);
		recv_ao_serverAddr.sin_addr.s_addr = INADDR_ANY;
		// inet_pton(AF_INET, dev_ip, &(recv_ao_serverAddr.sin_addr));

		if(bind(recv_sock, (struct sockaddr*)&recv_ao_serverAddr, sizeof(recv_ao_serverAddr)) == -1) {
			perror("Error Binding Recv Audio Out");
			close(recv_sock);
			return -1;
		}
	}
	else if (index == SEND_INDEX) {
		send_sock = socket(AF_INET, SOCK_DGRAM, 0);
		if (send_sock == -1) {
			perror("Erro Creating Send Audio Input Socket");
			return -1;
		}

		memset(&send_vm_serverAddr, 0, sizeof(send_vm_serverAddr));
		send_vm_serverAddr.sin_family = AF_INET;
		send_vm_serverAddr.sin_port = htons(host);
		inet_pton(AF_INET, mypc_ip, &(send_vm_serverAddr.sin_addr));

		memset(&send_vb_serverAddr, 0, sizeof(send_vb_serverAddr));
		send_vb_serverAddr.sin_family = AF_INET;
		send_vb_serverAddr.sin_port = htons(host+4);
		inet_pton(AF_INET, mypc_ip, &(send_vb_serverAddr.sin_addr));

		memset(&send_ai_serverAddr, 0, sizeof(send_ai_serverAddr));
		send_ai_serverAddr.sin_family = AF_INET;
		send_ai_serverAddr.sin_port = htons(host+1);
		inet_pton(AF_INET, mypc_ip, &(send_ai_serverAddr.sin_addr));

		
	}
	else {
		printf("Udp Setup Index Error!!\n");
		return -1;
	}
	return 0;
}

void deinit_udp(uint8_t index) {
	if (index == RECV_INDEX) 		close(send_sock);
	else if (index == SEND_INDEX) 	close(recv_sock);
	else printf("Udp Deinit Index Error!!\n");
}

#ifdef __H265__
ssize_t udp_vm_send(uint8_t *udp_data, uint16_t len) {
	ssize_t ret = sendto(send_sock, udp_data, len, 0, (struct sockaddr*)&send_vm_serverAddr, sizeof(send_vm_serverAddr));
	if (ret <= 0) {
		perror("Fail to Send Vedio Main UDP\n");
	}
	return ret;
}

ssize_t udp_vb_send(uint8_t *udp_data, uint16_t len) {
	ssize_t ret = sendto(send_sock, udp_data, len, 0, (struct sockaddr*)&send_vb_serverAddr, sizeof(send_vb_serverAddr));
	if (ret <= 0) {
		perror("Fail to Send Vedio Box UDP\n");
	}
	return ret;
}

#else
typedef struct  {
    uint8_t version_padding_extension_cc;
    uint8_t marker_payload_type;
    uint8_t sequence_number[2];
    uint32_t timestamp;
    uint32_t ssrc;
}RTPHeader2;

ssize_t udp_vm_send(uint8_t *udp_data, uint16_t len) {
	static uint16_t seq_num = 0;
	RTPHeader2 header;
	uint8_t *buf;

	buf = malloc(1000);

	header.version_padding_extension_cc = 0x80;
	header.marker_payload_type = 0x61;
	header.sequence_number[0] = (seq_num&0xFF00) >> 8;;
	header.sequence_number[1] = seq_num&0xFF;
	// header.timestamp = sample_gettimeus();
	header.timestamp = 4356433;
	header.ssrc = 123465879;

	memcpy(buf, (uint8_t*)&header, sizeof(RTPHeader2));
	memcpy(buf+sizeof(RTPHeader2), udp_data, len);

	// ssize_t ret = sendto(send_sock, udp_data, len, 0, (struct sockaddr*)&send_vm_serverAddr, sizeof(send_vm_serverAddr));
	ssize_t ret = sendto(send_sock, buf, len+12, 0, (struct sockaddr*)&send_vm_serverAddr, sizeof(send_vm_serverAddr));
	if (ret <= 0) {
		perror("Fail to Send Vedio Main UDP\n");
	}

	seq_num++;

	free(buf);
	return ret;
}

ssize_t udp_vb_send(uint8_t *udp_data, uint16_t len) {
	static uint16_t seq_num = 0;
	RTPHeader2 header;
	uint8_t *buf;

	buf = malloc(1000);

	header.version_padding_extension_cc = 0x80;
	header.marker_payload_type = 0x61;
	header.sequence_number[0] = (seq_num&0xFF00) >> 8;;
	header.sequence_number[1] = seq_num&0xFF;
	// header.timestamp = sample_gettimeus();
	header.timestamp = 5356433;
	header.ssrc = 123465879;

	memcpy(buf, (uint8_t*)&header, sizeof(RTPHeader2));
	memcpy(buf+sizeof(RTPHeader2), udp_data, len);

	// ssize_t ret = sendto(send_sock, udp_data, len, 0, (struct sockaddr*)&send_vm_serverAddr, sizeof(send_vm_serverAddr));
	ssize_t ret = sendto(send_sock, buf, len+12, 0, (struct sockaddr*)&send_vb_serverAddr, sizeof(send_vb_serverAddr));
	if (ret <= 0) {
		perror("Fail to Send Vedio Main UDP\n");
	}

	seq_num++;

	free(buf);
	return ret;
}
#endif



ssize_t udp_ai_send(uint8_t *udp_data, uint16_t len) {
	ssize_t ret = sendto(send_sock, udp_data, len, 0, (struct sockaddr*)&send_ai_serverAddr, sizeof(send_ai_serverAddr));
	if (ret <= 0) {
		perror("Fail to Send Audio In UDP\n");
	}
	return ret;
}

ssize_t udp_recv(uint8_t *data_buf) {
	ssize_t recv_len = recvfrom(recv_sock, data_buf, 1000, 0, (struct sockaddr*)&client_addr, &client_len);
	if (recv_len <= 0) {
		perror("Fail to Recv Audio Out UDP\n");
		return -1;
	}
	return recv_len;
}
pthread_mutex_t buffMutex_ao = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t buffMutex_ai = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t buffMutex_vm = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t buffMutex_vb = PTHREAD_MUTEX_INITIALIZER;

int test_out_fd=0;

void *udp_recv_pthread(void *arg) {
	
	ssize_t ret = 0;
	uint8_t *buf, *buff_u8;
	uint16_t len=0;//, rest_cnt=0;
	int buff_space = 0;
	
	buf = (uint8_t*)malloc(4000);

	
	test_out_fd = open("/vtmp/test_out.pcm", O_RDWR | O_CREAT | O_TRUNC, 0777);
	if (test_out_fd < 0) {
		printf("test PCM open Out failed\n");
		return ((void *)-1);
	}

	init_udp(RECV_INDEX, 1102);
	do {
		ret = udp_recv(buf);
		if (ret < 0) {
			perror("Fail Recv Data");
		}
		else {
			if (save_pcm == 1) {
				ret = write(test_out_fd, buf, ret);
				if (ret < 0) {
					printf("Test PCM Save Out Fail!\n");
					return NULL;
				}
			}
			len = ret;
			if(len > 0){
				// printf("UDP Recv Len : %d\n", len);
				// pthread_mutex_lock(&buffMutex_ao);
				if (AO_Cir_Buff.RIndex != AO_Cir_Buff.WIndex) {
					buff_space = (AO_Cir_Buff.RIndex - AO_Cir_Buff.WIndex - 1 + A_BUFF_SIZE) % (500*1024);
				}
				else buff_space = A_BUFF_SIZE;
				if (buff_space >= len) {
					buff_u8 = buf;
					for(int j = 0; j < len; ++j) {
						AO_Cir_Buff.tx[AO_Cir_Buff.WIndex] = buff_u8[j];
						AO_Cir_Buff.WIndex = (AO_Cir_Buff.WIndex+1) % (500*1024);
						if (AO_Cir_Buff.WIndex == AO_Cir_Buff.RIndex) {
							AO_Cir_Buff.RIndex = (AO_Cir_Buff.RIndex+1) % (500*1024);
						}
					}
					// printf("[CIR_BUFF Audio Out]buff_space:%d WIndex:%d RIndex%d\n", buff_space, AO_Cir_Buff.WIndex, AO_Cir_Buff.RIndex);
				}
				else {
					printf("AO Cir Buff Overflow!1\n");
				}
				// pthread_mutex_unlock(&buffMutex_ao);
			}
		}
	}while(!bStrem);
	printf("[UDP] AO Thread END!\n");
	deinit_udp(RECV_INDEX);
	

	pthread_exit(0);
}


void *udp_send_pthread(void *arg) {
	
	// ssize_t ret = 0;
	uint8_t *buf;
	int datasize = 0;
	int framesize = 0;
	int ret=0;

	buf = (uint8_t*)malloc(2000);

	int test_fd=0;
	test_fd = open("/vtmp/test_in.pcm", O_RDWR | O_CREAT | O_TRUNC, 0777);
	if (test_fd < 0) {
		printf("test PCM open In failed\n");
		return ((void *)-1);
	}

	init_udp(SEND_INDEX, 18883);
	do {
		/////////// Vedio Main IN -> UDP Out /////////////////////////////////////////
		if (VM_Frame_Buff.cnt > 0) {
			framesize = VM_Frame_Buff.len[VM_Frame_Buff.Rindex];
			for(int i=0; framesize > 0; i++){
				pthread_mutex_lock(&buffMutex_vm);
				datasize = (framesize > V_SEND_SIZE) ? V_SEND_SIZE : framesize;
				udp_vm_send(VM_Frame_Buff.tx[VM_Frame_Buff.Rindex]+(V_SEND_SIZE*i), datasize);
				framesize -= datasize;
				// printf("cnt:%d, total:%d, dsize:%d\n", i, framesize, datasize);
				pthread_mutex_unlock(&buffMutex_vm);
				usleep(1*1000);
			}
			VM_Frame_Buff.Rindex = (VM_Frame_Buff.Rindex+1)%10;
			VM_Frame_Buff.cnt--;
		}
		//////////////////////////////////////////////////////////////////////////

		/////////// Vedio Box IN -> UDP Out /////////////////////////////////////////
	#if 0
		if (VB_Frame_Buff.cnt > 0) {
			framesize = VB_Frame_Buff.len[VB_Frame_Buff.Rindex];
			for(int i=0; framesize > 0; i++){
				pthread_mutex_lock(&buffMutex_vm);
				datasize = (framesize > V_SEND_SIZE) ? V_SEND_SIZE : framesize;
				udp_vb_send(VB_Frame_Buff.tx[VB_Frame_Buff.Rindex]+(V_SEND_SIZE*i), datasize);
				framesize -= datasize;
				// printf("cnt:%d, total:%d, dsize:%d\n", i, framesize, datasize);
				pthread_mutex_unlock(&buffMutex_vm);
				usleep(1*1000);
			}
			VB_Frame_Buff.Rindex = (VB_Frame_Buff.Rindex+1)%10;
			VB_Frame_Buff.cnt--;
		}
	#else
		if (VB_Frame_Buff.cnt > 0) {
			framesize = VB_Frame_Buff.len[VB_Frame_Buff.Rindex];
			for(int i=0; framesize > 0; i++){
				pthread_mutex_lock(&buffMutex_vm);
				datasize = (framesize > V_SEND_SIZE) ? V_SEND_SIZE : framesize;
				udp_vb_send(VB_Frame_Buff.tx[VB_Frame_Buff.Rindex]+(V_SEND_SIZE*i), datasize);
				framesize -= datasize;
				// printf("cnt:%d, total:%d, dsize:%d\n", i, framesize, datasize);
				pthread_mutex_unlock(&buffMutex_vm);
				usleep(1*1000);
			}
			VB_Frame_Buff.Rindex = (VB_Frame_Buff.Rindex+1)%10;
			VB_Frame_Buff.cnt--;
		}
	#endif
		//////////////////////////////////////////////////////////////////////////

		/////////// Audio IN -> UDP Out /////////////////////////////////////////
		pthread_mutex_lock(&buffMutex_ai);
		if (AI_Cir_Buff.RIndex != AI_Cir_Buff.WIndex) {
			datasize = (AI_Cir_Buff.WIndex - AI_Cir_Buff.RIndex + A_BUFF_SIZE) % (500*1024);
			datasize = (datasize > A_SEND_SIZE) ? A_SEND_SIZE : datasize;
			for (int i = 0; i < datasize; ++i) {
				buf[i] = AI_Cir_Buff.tx[AI_Cir_Buff.RIndex];
				AI_Cir_Buff.RIndex = (AI_Cir_Buff.RIndex+1) % (500*1024);
			}

			// printf("[CIR_BUFF_AI]datasize:%d WIndex:%d RIndex%d\n", datasize, AI_Cir_Buff.WIndex, AI_Cir_Buff.RIndex);
			udp_ai_send(buf, datasize);

			if (save_pcm == 1) {
				ret = write(test_fd, buf, datasize);
				if (ret < 0) {
					printf("Test PCM In Save Fail!\n");
					return NULL;
				}
			}
		}
		pthread_mutex_unlock(&buffMutex_ai);
		if (save_pcm == 2){
			close(test_fd);
			close(test_out_fd);
			pcm_in = true;
		}

		//////////////////////////////////////////////////////////////////////////
	} while(!bStrem);

	// close(mfd);
	// close(bfd);
	printf("[UDP] AI Thread END!\n");
	deinit_udp(SEND_INDEX);
	pthread_exit(0);
}

