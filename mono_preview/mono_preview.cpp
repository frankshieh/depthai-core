#include <chrono>
#include "utility.hpp"
#include <iostream>

// Includes common necessary includes for development using depthai library
#include "depthai/depthai.hpp"
using namespace std;
using namespace std::chrono;
int main() {
    // Create pipeline
    dai::Pipeline pipeline;

    // Define sources and outputs
    auto monoLeft = pipeline.create<dai::node::MonoCamera>();
    auto monoRight = pipeline.create<dai::node::MonoCamera>();
    auto xoutLeft = pipeline.create<dai::node::XLinkOut>();
    auto xoutRight = pipeline.create<dai::node::XLinkOut>();

    xoutLeft->setStreamName("left");
    xoutRight->setStreamName("right");

    // Properties
    monoLeft->setBoardSocket(dai::CameraBoardSocket::LEFT);
    monoLeft->setResolution(dai::MonoCameraProperties::SensorResolution::THE_400_P);
    monoRight->setBoardSocket(dai::CameraBoardSocket::RIGHT);
    monoRight->setResolution(dai::MonoCameraProperties::SensorResolution::THE_400_P);

    // Linking
    monoRight->out.link(xoutRight->input);
    monoLeft->out.link(xoutLeft->input);

    // Connect to device and start pipeline
    dai::Device device(pipeline);

    // Output queues will be used to get the grayscale frames from the outputs defined above
    auto qLeft = device.getOutputQueue("left", 4, false);
    auto qRight = device.getOutputQueue("right", 4, false);

	auto startTime = steady_clock::now();
    int counter = 0;
    float fps = 0;
    auto color = cv::Scalar(255, 255, 255);

    while(true) {
        // Instead of get (blocking), we use tryGet (non-blocking) which will return the available data or None otherwise
        auto inLeft = qLeft->tryGet<dai::ImgFrame>();
        auto inRight = qRight->tryGet<dai::ImgFrame>();

		counter++;
        auto currentTime = steady_clock::now();
        auto elapsed = duration_cast<duration<float>>(currentTime - startTime);
        if(elapsed > seconds(1)) {
            fps = counter / elapsed.count();
            counter = 0;
            startTime = currentTime;
        }

		cout << "fps:" << fps << endl;

        if(inLeft) {
            cv::imshow("left", inLeft->getCvFrame());
        }

        if(inRight) {
            cv::imshow("right", inRight->getCvFrame());
        }

        int key = cv::waitKey(1);
        if(key == 'q' || key == 'Q') {
            return 0;
        }
    }
    return 0;
}
