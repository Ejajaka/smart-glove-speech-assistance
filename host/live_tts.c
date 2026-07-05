#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

#define SERIAL_PORT "/dev/ttyACM1"  // Arduino port
#define BAUDRATE B9600              // Match Arduino Serial.begin(9600)
#define TTS_DELAY_US 50000          // 50 milliseconds delay between sentences

int main() {
    int fd = open(SERIAL_PORT, O_RDONLY | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    // Configure serial port
    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        close(fd);
        return 1;
    }

    cfsetospeed(&tty, BAUDRATE);
    cfsetispeed(&tty, BAUDRATE);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_iflag &= ~IGNBRK;
    tty.c_lflag = 0;
    tty.c_oflag = 0;
    tty.c_cc[VMIN]  = 1;
    tty.c_cc[VTIME] = 1;
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~(PARENB | PARODD);
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        close(fd);
        return 1;
    }

    printf("Listening on %s...\n", SERIAL_PORT);

    char buf[256];
    char sentence[512];
    int idx = 0;

    while (1) {
        int n = read(fd, buf, sizeof(buf));
        if (n > 0) {
            for (int i = 0; i < n; i++) {
                char c = buf[i];
                if (c == '\n') {
                    sentence[idx] = '\0';
                    if (idx > 0) {
                        printf("Arduino: %s\n", sentence);
                        fflush(stdout);

                        // Speak sentence
                        char cmd[600];
                        snprintf(cmd, sizeof(cmd),
                                 "espeak -s 180 -v en-us \"%s\" &",
                                 sentence);
                        system(cmd);

                        // Wait 50 milliseconds before processing next sentence
                        usleep(TTS_DELAY_US);
                    }
                    idx = 0;
                } else if (c != '\r' && idx < sizeof(sentence)-1) {
                    sentence[idx++] = c;
                }
            }
        }
    }

    close(fd);
    return 0;
}

