#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include <fstream>
#include <dbcppp/Network.h>

using namespace dbcppp;

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

    strcpy(ifr.ifr_name, iface.c_str());
    ioctl(s, SIOCGIFINDEX, &ifr);
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    std::ifstream dbc_file("can.dbc");
    auto net = Network::fromDBC(dbc_file);

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr))) {
        std::cout << "bind error" << std::endl;
        return 0;
    }

    while (1) {
        nbytes = read(s, &frame, sizeof(struct can_frame));

        std::cout << frame.can_id << std::endl;
        std::cout << std::hex << frame.data[0] + 0x00 << std::endl;

        const Message* msg = net->getMessageById(frame.can_id);
        
        if (msg) {
            std::cout << "Received message: " << msg->getName() << std::endl;
            (*msg).forEachSignal(
                [&](const Signal& signal) {
                    double raw = signal.decode(frame.data);
                    std::cout << "\t" << signal.getName() << "=" << signal.rawToPhys(raw) << std::endl;
                });
        }
    }
}
