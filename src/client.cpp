#include <opencv2/opencv.hpp>
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define MAX_PACKET_SIZE 65507 

int main() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error: Could not create socket\n";
        return -1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Error: Could not bind socket\n";
        return -1;
    }

    while (true) {
        int frame_size;
        int received_size = recvfrom(sockfd, &frame_size, sizeof(frame_size), 0, NULL, NULL);
        if (received_size != sizeof(frame_size)) {
            std::cerr << "Error: Could not receive frame size\n";
            continue;
        }

        std::vector<uchar> encoded_frame(frame_size);
        received_size = recvfrom(sockfd, encoded_frame.data(), frame_size, 0, NULL, NULL);
        if (received_size != frame_size) {
            std::cerr << "Incomplete frame received, discarding\n";
            continue;
        }

        cv::Mat frame = cv::imdecode(encoded_frame, cv::IMREAD_COLOR);
        if (frame.empty()) {
            std::cerr << "Error: Could not decode frame\n";
            continue;
        }

        cv::imshow("Client - Video Stream", frame);
        if (cv::waitKey(10) == 27) break; 
    }

    close(sockfd);
    return 0;
}

