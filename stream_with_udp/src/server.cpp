#include <opencv2/opencv.hpp>
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define MAX_PACKET_SIZE 65507

int main() {
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open video capture\n";
        return -1;
    }

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error: Could not create socket\n";
        return -1;
    }

    struct sockaddr_in client_addr;
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(PORT);
    client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    std::vector<uchar> encoded_frame;

    while (true) {
        cv::Mat frame, resized_frame;
        if (!cap.read(frame)) {
            std::cerr << "Error: Could not capture frame\n";
            break;
        }

       
        cv::resize(frame, resized_frame, cv::Size(frame.cols / 4, frame.rows / 4));
	cv::imencode(".jpg", resized_frame, encoded_frame, {cv::IMWRITE_JPEG_QUALITY, 50});

        

       
        int frame_size = encoded_frame.size();
        sendto(sockfd, &frame_size, sizeof(frame_size), 0,
               (struct sockaddr *)&client_addr, sizeof(client_addr));
        sendto(sockfd, encoded_frame.data(), frame_size, 0,
               (struct sockaddr *)&client_addr, sizeof(client_addr));
        
        if (cv::waitKey(10) == 27) break; // Exit on 'ESC'
    }

    close(sockfd);
    return 0;
}

