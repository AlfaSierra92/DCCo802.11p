#include <stdio.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <linux/wireless.h>

int main() {
    int sock;
    struct ifreq ifr;
    struct iw_statistics stats;

    // Apri un socket di controllo
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("socket");
        return 1;
    }

    // Imposta il nome dell'interfaccia su cui ottenere le statistiche
    strncpy(ifr.ifr_name, "wlo1", IFNAMSIZ - 1);

    // Ottieni le statistiche wireless usando ioctl con SIOCGIWSTATS
    if (ioctl(sock, SIOCGIWSTATS, &ifr) == -1) {
        perror("ioctl");
        close(sock);
        return 1;
    }

    // Copia le statistiche ottenute nella struttura stats
    memcpy(&stats, &ifr.ifr_data, sizeof(stats));

    // Stampare le statistiche ottenute
    printf("Statistiche wireless:\n");
    printf("Pacchetti ricevuti: %u\n", stats.qual.qual);
    printf("Pacchetti persi: %u\n", stats.qual.level);
    // Altre statistiche possono essere stampate in modo simile

    // Chiudi il socket di controllo
    close(sock);

    return 0;
}
