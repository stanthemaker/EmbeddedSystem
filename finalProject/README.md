# Embedded Systems Lab Final Project - Laser Gesture YT Stream Master
<p align="center"> <i> 揮揮手指就能暫停/快轉，再也不用找遙控器啦 </i> </p>
<p align="right">翁瑋杉 B09901104 <br> 楊倢綺 B09901055 </p>



## Motivation
Picture this: you're comfortably settled on your sofa, engrossed in a Netflix drama or a YouTube documentary, surrounded by the delicious scents of fried chicken and pizza. But here comes the twist - your hands are too messy to use the remote, or perhaps even more relatable, the remote has once again disappeared into the mysterious void of couch cushions. This is where our project steps in.
We aim to enhance the user experience by offering a **hands-free** and also **sound-free solution** for controlling streaming services like Netflix, YouTube, and others.

Slides for our final presentation can be found [here](https://docs.google.com/presentation/d/1WA--AGeGdAIDSCOyNIQFAUT-dZnZ6YrAOZue6e1CReI/edit#slide=id.g2a9458a8410_0_0).

Demo Video: https://youtu.be/fiZ1K2Q7q24
## Abstract
The **Laser Gesture YT Stream Master** project leverages the STM32 board and VL53L0X Time-of-Flight (ToF) ranging sensor for gesture-based control of streaming services. It utilizes a combination of hardware and software, including the STM32 microcontroller for processing gesture data and a Flask-based web server for handling communication with streaming services like YouTube. The project employs an algorithm to translate gesture durations into playback controls, enabling users to interact with their media in a novel, hands-free manner. This innovative approach allows for an intuitive and engaging user experience, enhancing the convenience and enjoyment of home entertainment.

## Tech Use
- Multithreading
- Web sever 
- Socket communication

## Technical Details

### 1. Gesture Detection System

We employ two VL53L0X laser modules to measure distances and categorize them into distinct gestures. These modules are linked to the STM32 through the I2C protocol, transmitting integer values that denote the distances between objects and the sensors.

Distance measurements are obtained through polling, maintaining a fixed sample rate. Once the data reaches the STM32, it undergoes processing using a finite state machine. This state machine yields the gesture based on the detected distances, pattern, and sequence. The gesture duration is also recorded by the STM32 internal clock.

![](https://cdn.store-assets.com/s/236824/i/14591209.jpeg?width=300)

#### Finite State Machine

We use the finite state machine to catagorize gestures. The code is sourced from the github repository [here](https://github.com/stm32duino/Proximity_Gesture/tree/main/src). The original code has three states, which only use one layser module. We extend the code into a seven-state mechanism to implement bidirectional gesture detection with two sensors. Here's the block diagram of it:

![擷取](https://hackmd.io/_uploads/ry8X6rYwa.png)


For example, once the user starts to interact with the device, it will enter the initial `start` state. Subsequently, if the user keep its hand on the sensors, it will go to the `pending` state and turns out to be a `tap` signal. Finally, it returns to the `wait` state to wait for the next input arrival.


### 2. STM32 Control
We've configured the STM32 as a remote controller to transmit signals to the PC, where gesture information are translated into APIs for controlling stream video status. We built a socket on STM32 and implemented a server on the PC using FlaskIO, establishing the communication between STM32 and PC through http protocol. 

Subsequently, we retrieve data from sensors on the STM32 and relay corresponding instructions to the PC. The laser module serves to detect hand movements, signaling the PC to interpret whether it's a single tap or a directional swipe. To ensure precision in execution, a straightforward threshold is implemented, filtering out noise and allowing only distinct hand movements in close proximity to be recognized.

#### Message Queue
Mbed RTOS API message queue is utilized to enable simultaneous data transmission and distance measurement

After the measured distance and the gestures are subsequently identified by the finite state machine, the gestures information are queued. Concurrently, socket threads extract these gesture data from the queue and send it to the PC server. This integration ensures that gesture-based controls for video playback are responsive and synchronized, significantly improving the system's efficiency in real-time gesture recognition and media control.

![](https://os.mbed.com/docs/mbed-os/v6.16/apis/images/queue.png)


### 3. Web Browser Video Streaming

Utilizing a web browser for video streaming in your demo is a strategic choice for several reasons:

1. **Limited Device Memory**: Small devices like the Raspberry Pi (RPI) often have constrained memory. Storing large MP4 files directly on these devices could quickly deplete the available memory, making it impractical. Video streaming allows for efficient use of resources by fetching and rendering only the portions of the video needed for playback.

2. **Flexibility and User Interface**: Adopting the format of a web browser aligns with the practices of major video streaming companies like YouTube and Netflix. These companies leverage web-based interfaces to provide users with a flexible and user-friendly experience. A browser-based approach allows for a dynamic user interface, making it easier for users to navigate, control, and personalize their video streaming experience.

#### Flask-SocketIO


We employ the Python package SocketIO to facilitate data exchange between the frontend and backend. SocketIO serves as a higher-level abstraction, enabling bidirectional communication between the server and client. Upon receiving data from the STM32 device, the server emits this data to the frontend web browser.

The frontend web browser, in turn, monitors the server for incoming data using the `socketio.on()` function. This establishes a communication channel, allowing real-time updates and seamless interaction between the server and the client, enhancing the overall responsiveness and functionality of the application.

![](https://www.asyncapi.com/img/posts/socketio-part2/socketio-ack-sequence-diagram.webp)

#### User Interface

We utilize the **YouTube iFramePlayer API** to facilitate the playback of authorized videos on YouTube. YouTube offers a range of APIs that enable us to manage video status, incorporating functions like  `play()`, `pause()`, and `seekTo()`. A single tap on the video toggles between play and pause states.

For more nuanced control, we have implemented two distinct modes for fast-forwarding and rewinding: fast mode and normal mode. In fast mode, the swipe exceeds a predefined threshold and users can significantly accelerate video playback or rewind. Conversely, normal mode allows users to adjust the timeline in fixed intervals, such as 5 or 10 seconds. This dual-mode approach enhances user interaction by providing a more adaptable and user-friendly interface.


## Usage
1. Run `server.py` on your PC or raspberry pi.
2. Open localhost port 5050
3. Run main.c on STM32
4. Try out our device and enjoy your video!

## Future Work
- Resource Saving Strategy:

     Currently, our STM32 devices consistently acquire data through polling from the laser modules, ensuring that changes are detected at a reliable sample rate.

    However, this approach has led to power and CPU resource inefficiencies. The CPU is obligated to continually poll for data, even when no external changes are anticipated. In our scenario, the video controller may experience prolonged periods of inactivity, as we assume that users will only pause, restart, or rewind the video at infrequent intervals. During these idle times, the STM32 expends unnecessary resources querying data from the sensors.
  
    Consequently, we aim to explore an alternative method for data acquisition, considering the adoption of DMA with interrupts. This approach allows us to optimize CPU resources efficiently without compromising the precision of gesture recognition. The finite state machine for data processing relies on a sequential stream of data, and by employing DMA with interrupts, we can strike a balance between resource efficiency and the accuracy required for effective gesture recognition.
  
  
- Extending our system to function on real-world applications.



## References
STM32
https://github.com/stm32duino/Proximity_Gesture/tree/main/src
https://os.mbed.com/teams/ST/code/HelloWorld_ST_Sensors/docs/tip/main_8cpp_source.html
https://os.mbed.com/docs/mbed-os/v6.16/apis/queue.html

Youtube iFrame API
https://developers.google.com/youtube/iframe_api_reference?hl=zh-tw#Playback_controls
https://www.letswrite.tw/youtube-iframe-api/
https://stackoverflow.com/questions/12421491/can-i-monitor-changes-on-an-html-div
https://ithelp.ithome.com.tw/articles/10305281?sc=rss.iron

Flask  - SocketIO
https://flask-socketio.readthedocs.io/en/latest/
https://medium.com/@charming_rust_oyster_221/%E4%BD%BF%E7%94%A8-flask-socketio-%E5%AE%8C%E6%88%90-websocket-%E9%9B%99%E5%90%91%E9%80%9A%E8%A8%8A-49fd734f52ae
https://hackmd.io/@hychang/B1BvevzHE?type=view
https://socket.io/docs/v4/tutorial/introduction





