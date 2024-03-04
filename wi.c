#include <stdio.h>
#include <iwlib.h>
#include <sys/wait.h>

#define IFACE "wlan0"
#define MAX_BUF_SIZE 1024

int bytes(char *tx_rx) {
    // Path to the rx_bytes file
    //const char *file_path = "/sys/class/net/wlo1/statistics/rx_bytes";
    char file_path[MAX_BUF_SIZE];
    snprintf(file_path, MAX_BUF_SIZE, "/sys/class/net/%s/statistics/%s_bytes", IFACE, tx_rx);
    
    // Open the file for reading
    int fd = open(file_path, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Read the contents of the file
    char buffer[MAX_BUF_SIZE];
    ssize_t bytes_read = read(fd, buffer, MAX_BUF_SIZE - 1);
    if (bytes_read == -1) {
        perror("Error reading file");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Null-terminate the buffer
    buffer[bytes_read] = '\0';

    // Convert the read bytes to a long long integer
    long long rx_bytes = atoll(buffer);

    // Close the file
    close(fd);

    // Print the value of rx_bytes
    printf("%s_bytes: %lld\n", tx_rx, rx_bytes);

    return 0;
}

int packets(char *tx_rx) {
    // Path to the rx_bytes file
    //const char *file_path = "/sys/class/net/wlo1/statistics/rx_bytes";
    char file_path[MAX_BUF_SIZE];
    snprintf(file_path, MAX_BUF_SIZE, "/sys/class/net/%s/statistics/%s_packets", IFACE, tx_rx);
    
    // Open the file for reading
    int fd = open(file_path, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Read the contents of the file
    char buffer[MAX_BUF_SIZE];
    ssize_t bytes_read = read(fd, buffer, MAX_BUF_SIZE - 1);
    if (bytes_read == -1) {
        perror("Error reading file");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Null-terminate the buffer
    buffer[bytes_read] = '\0';

    // Convert the read bytes to a long long integer
    long long rx_bytes = atoll(buffer);

    // Close the file
    close(fd);

    // Print the value of rx_bytes
    printf("%s_packets: %lld\n", tx_rx, rx_bytes);

    return 0;
}

int wireless_stats(){
    int sockfd;
    iwstats stats;
    iwrange range;

    // Apri un socket per ottenere le informazioni wireless
    sockfd = iw_sockets_open();
    if (sockfd < 0) {
        perror("Errore nell'apertura del socket");
        return 1;
    }

    int has_range = iw_get_range_info(sockfd, IFACE, &range);
    // Ottieni le informazioni wireless per l'interfaccia specificata
    if (iw_get_stats(sockfd, IFACE, &stats, &range, has_range) < 0) {
        perror("Errore nell'ottenere le informazioni wireless");
        iw_sockets_close(sockfd);
        return 1;
    }

    // results in stats.qual.qual e stats.qual.level
    // thanks to https://github.com/nhoad/python-iwlib/blob/master/iwlib/iwconfig.py

    // Stampa le statistiche ottenute
    printf("Statistiche wireless:\n");
    printf("Qualità del segnale: %d/70\n", stats.qual.qual);
    printf("Livello del segnale: %d dBm\n", (int8_t) stats.qual.level);
    // Puoi stampare altre informazioni disponibili in info

    // Chiudi il socket
    iw_sockets_close(sockfd);
}

int main() {
    int status;
    while(1){
        //wireless_stats(); //non funziona su rockpi...
        bytes("tx");
        packets("tx");
        printf("\n");
        bytes("rx");
        packets("rx");
        printf("\n");
        //int status = system("sudo iw wlan0 set txpower fixed 100");
        //l'execl mi butta giù il programma quindi devo farlo eseguire su un processo child
        int pid;
        if ((pid=fork()) < 0){
        	printf("Fork error!");
		    exit(4);
        }
        if (pid == 0){
            if (execl("/usr/sbin/iw", "iw", "wlan0", "set", "txpower", "fixed", "100", (char*) NULL) < 0) {
                perror("error");
            }
        }
        waitpid(pid, NULL, 0);
        
        sleep(1);
    }
    
    return 0;
}