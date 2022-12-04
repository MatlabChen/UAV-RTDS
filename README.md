# UAV-RTDS
This is the implement of IJDE "Deep Learning on embedded system for real-time target detection from unmanned aerial vehicle images: vehicle detection as a case study"

Tutorial video: https://pan.baidu.com/s/15yDbnopukPXCcJ3DinHbmQ?pwd=jesn 


###### **0.Environment**

(a) uav-server

JDK >= 1.8 

(b) uav-client

JDK >= 1.8 and Darknet (https://github.com/AlexeyAB/darknet)

###### **1.Dataset**

Link: https://pan.baidu.com/s/1CnVQm6-PXe5_JU5ySXo2hw?pwd=qmpl

###### **2.Launch UAV-RTDS**

(a) Launch server

java -jar uav-server-1.0-SNAPSHOT-GPS.jar 9000 ./recv 

9000 is data transmission port and ./recv is data storage location, both can change

(b) Launch Client

java -jar uav-client.jar -h 192.168.199.113 -p 9000 -f ./data/result -n TX2 -c 1024

-h IP of server

-p Data transmission port of server

-f File directory of the image to be transferred

-n Name of UAV

-c The number of bytes transmitted at a time

(c) Launch front

Browser access http://localhost:8080/
