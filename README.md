1.	About UAV-RTDS
For the majority of unmanned aerial vehicle (UAV) remote sensing applications, the data need to be downloaded to the ground devices for processing, which cannot meet the real-time target detection application’s requirements. As such, we have designed an integrated system of image acquisition, target detection, result transmission, result display, and management based on the real-time system structure and framework of unmanned airborne embedded equipment. That is, we designed and implemented a UAV real-time target detection system (UAV-RTDS). Compared with the existing similar researches, our research are different in detection model, real-time data transmission, system integration and optimization. This study are committed to exploring the key technologies of a universal and interactive quasi real-time target detection system, so as to provide convenience and reference for other similar target detection applications.

2.	Composition of the Material
The research has been submitted to the journal of International Journal of Digital Earth, which is the code implementation of the system and as the auxiliary material of the paper. Please read the paper carefully for the hardware composition, wording principle, experimental process and some other relevant technical details of the system. The auxiliary materials mainly include three parts: (1) code and description, (2) corresponding dataset, and (3) tutorial video. Note: GitHub only contains (1), (2) and (3) on the network disk (please see the link below.), due to space limitations on GitHub.
Tutorial video: https://pan.baidu.com/s/15yDbnopukPXCcJ3DinHbmQ?pwd=jesn
Dataset: https://pan.baidu.com/s/1CnVQm6-PXe5_JU5ySXo2hw?pwd=qmpl

3.	Composition of the UAV-RTDS
The codes for the entire system is divided into three parts, viz., (1) the UAV client part that is deployed on the TX2 development board on the UAV, (2) the ground server part that is deployed on the ground laptop, and (3) the front-end system part that is primarily accessed by the user through the browser. Among them, the framework includes a data communication module used to connect the UAV client and the ground server, and a detection module to complete the target detection task.

4.	How To Use the Codes for UAV-RTDS
4.1 Environment
(a) uav-server running on a laptop
JDK >= 1.8 
(b) uav-client running on a TX2
JDK >= 1.8 and Darknet (https://github.com/AlexeyAB/darknet)

If the code is not compiled, compile it with the "make" command. At the same time, make sure the consistency and correctness of some paths and configuration files.

4.2. Datasets
You can prepare your own related equipment, install the corresponding software and its auxiliary software packages, and actually collect your own data set in the field; alternatively, you also can use our existing datasets for testing. If the latter, you need to download the dataset, i.e., "./Data_UAV-RTDS/Experiment_Result/Data", from the above link. And then copy the images in the dataset and placed it into the place of "./uav client/data/camera", as the original data of vehicle detection.

4.3 Launch UAV-RTDS
(a) Launch the server
java -jar uav-server-1.0-SNAPSHOT-GPS.jar 9000 ./recv 

Here, 9000 is data transmission port, and “./recv” is data storage location, both depend on your own local environment.

(b) Launch the client
java -jar uav-client.jar -h 192.168.199.113 -p 9000 -f ./data/result -n TX2 -c 1024

Here,
-h IP of server, depend on your local environment;
-p Data transmission port of server;
-f File directory of the result of vehicle detection to be transferred;
-n Name of UAV, defined by the user;
-c The number of bytes transmitted at a time, defined by the user.

(c) Launch the front-end system
Open the URL, http://localhost:8080/, to browse the status of UAV and operate the system. You can conduct some operational instructions, and the vehicle detection results will be viewed on the webpage.

Enjoy it!
