#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>
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

void print_usage() {
    std::cout << "can-mqtt-bridge: Decode and forward CAN bus signals to an MQTT broker.\n" << std::endl;
    std::cout << "Usage: can-mqtt-bridge [options] <CAN interface>\n" << std::endl;
    std::cout << "Options: -d FILE          DBC file to decode CAN frames" << std::endl;
    std::cout << "         -f FREQUENCY     Maximum frequency to forward to MQTT for each signal" << std::endl;
    std::cout << "         -v               Verbose mode, prints all decoded values" << std::endl;
    std::cout << "         -H               MQTT host to publish to" << std::endl;
    std::cout << "         -U               MQTT username" << std::endl;
    std::cout << "         -P               MQTT password" << std::endl;
    std::cout << "         -p               MQTT port" << std::endl;
    std::cout << "         -h               Display this help message\n" << std::endl;
    std::cout << "Example: can-mqtt-bridge -d can.dbc -f 5 -H mqtt.example.com -U admin -P admin -p 1883 can0\n" << std::endl;
}

void log(std::string msg) {
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);

    std::cout << '[' << std::put_time(&tm, "%F %T") << ']';
    std::cout << ' ' << msg << std::endl;
}

int main(int argc, char **argv) {
    int opt;
    int frequency = 0;
    int verbose = 0;

    std::string dbc_path = "can.dbc";
    std::string mqtt_host = "";
    std::string mqtt_username = "";
    std::string mqtt_password = "";
    int mqtt_port = 1883;

    if (argc < 2) {
        print_usage();
        return 1;
    }

    while ((opt = getopt(argc, argv, "d:f:vH:U:P:p:h")) != -1) {
        switch (opt) {
            case 'd':
                dbc_path = optarg;
                break;

            case 'f':
                frequency = atoi(optarg);
                break;
            
            case 'v':
                verbose = 1;
                break;
 
            case 'H':
                mqtt_host = optarg;
                break;
 
            case 'U':
                mqtt_username = optarg;
                break;
 
            case 'P':
                mqtt_password = optarg;
                break;
 
            case 'p':
                mqtt_port = atoi(optarg);
                break;
 
            case 'h':
                print_usage();
                exit(1);
                break;
        }
    }

    std::string iface = argv[argc-1];

    log("using DBC file " + dbc_path);
    log("using frequency " + std::to_string(frequency) + " Hz");
    log("using MQTT host " + mqtt_host + ":" + std::to_string(mqtt_port));

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
        log("bind error");
        return 1;
    }

    while (1) {
        nbytes = read(s, &frame, sizeof(struct can_frame));

        const Message* msg = net->getMessageById(frame.can_id);
        
        if (msg) {
            (*msg).forEachSignal(
                [&](const Signal& signal) {
                    double raw = signal.decode(frame.data);
                    log(signal.getComment() + " " + std::to_string(signal.rawToPhys(raw)));
                });
        }
    }
}
