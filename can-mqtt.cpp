#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <linux/can.h>
#include <linux/can/raw.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "invalid usage" << std::endl;
        return 0;
    }

    std::string iface = argv[1];

    int s = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;

    int nbytes;

    strcpy(ifr.ifr_name, "vcan0");
    ioctl(s, SIOCGIFINDEX, &ifr);
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    bind(s, (struct sockaddr *)&addr, sizeof(addr));

    while (1) {
        nbytes = read(s, &frame, sizeof(struct can_frame));

        std::cout << frame.can_id << std::endl;
        printf("%02X", frame.data[0]);
    }
}
