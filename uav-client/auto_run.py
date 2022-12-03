#coding=utf-8

import os
import sys
import subprocess
import time
import threading
from threading import Timer

# 配置类：用户存放程序入参
# 入参有：
#     （1）文件接收服务端IP
#     （2）文件接收服务端端口
class Config(object):
    def __init__(self, *args):
        if len(args) == 0:
            args = ("127.0.0.1", "9000");
        self.server_host, self.server_port = args;
        print("-----------" + self.server_port)
        
    def get_server_host(self):
        return self.server_host;
        
    def get_server_port(self):
        return self.server_port;

# directory要扫描的路径
# prefix 扫描文件的前缀，比如prefix等于abc，那么只扫描directory路径下的文件名是以abc开头的文件
# postfix 扫描文件的前缀，比如postfix等于.jpg，那么只扫描directory路径下的文件名是以.jpg结尾的文件
def scan_files(directory, prefix=None, postfix=None):
    files_list=[]
    
    # for循环取出directory的根目录root、子目录sub_dirs以及该文件目录下的文件files
    for root, sub_dirs, files in os.walk(directory):
        # 遍历文件
        for special_file in files:
            # 如果文件前缀不为空，且扫描的文件前缀和函数参数prefix制定的前缀相同，则把文件放到结果数组files_list中
            if postfix:
                if special_file.endswith(postfix):
                    files_list.append(os.path.join(root,special_file))
            # 如果文件后缀不为空，且扫描的文件后缀和函数参数postfix制定的后缀相同，则把文件放到结果数组files_list中
            elif prefix:
                if special_file.startswith(prefix):
                    files_list.append(os.path.join(root,special_file))
            # 既没有指定前缀也没有指定后缀
            else:
                files_list.append(os.path.join(root, special_file)) #根路径和文件名拼接起来返回
                          
    return files_list

def yolov4_process():
    # 定义变量保存已经处理过的文件
    processed_file_set = set(());
    while True:
        print("-----------------识别处理等待----------------------");
        time.sleep(4);
        # 扫描文件
        # file_list = scan_files("/home/yjchen/darknet-master/data", postfix="g.jpg");
        file_list = scan_files("./data/camera", postfix=".jpg");
        for file_name in file_list:
            # 先判断是否已经处理过，如果已经处理过了，continue退出本次处理，进行下一个文件
            if file_name in processed_file_set:
                continue;
            try:
                # yolov4 运行命令
                #cmd = "./darknet detect cfg/yolov4-custom.cfg yolov4.weights data/dog.jpg";
                cmd = "./darknet detect cfg/yolov4-custom.cfg backup/yolov4-custom_9000.weights " + file_name + " -thresh .1";
                print("正在识别处理的文件是：" + cmd + "\n");
                subprocess.call(cmd, shell=True);
                # 结果文件重命名
                start_index = file_name.rfind('/');
                end_index = file_name.rfind('.');
                rename_cmd = "mv predictions.jpg ./data/result/" + file_name[start_index: end_index] + "_predict.jpg";
                subprocess.call(rename_cmd, shell=True);
                processed_file_set.add(file_name);
            except Exception as e:
                print("识别程序出现异常:", e);

# 识别程序线程    
class DarknetThread (threading.Thread):
    def __init__(self, thread_id, name):
        threading.Thread.__init__(self);
        self.thread_id = thread_id;
        self.name = name;
        
    def run(self):
        print ("开始线程：" + self.name)
        yolov4_process();
        print ("退出线程：" + self.name)    

# 调用拍照程序
def camera_process():
    try:
        cmd = "./auto_camera -p ./data/camera";
        print("摄像头拍照程序开始拍照：" + cmd + "\n");
        subprocess.call(cmd, shell=True);
    except Exception as e:
        print("拍照程序出现异常:", e);
    
# 摄像头拍照线程    
class CameraThread (threading.Thread):
    def __init__(self, thread_id, name):
        threading.Thread.__init__(self);
        self.thread_id = thread_id;
        self.name = name;
        
    def run(self):
        print ("开始线程：" + self.name)
        camera_process();
        print ("退出线程：" + self.name)
        
# 调用文件传输程序
def file_transfer(config):
    cmd = "java -jar uav-client.jar -h 192.77.108.247 -p 9000 -f ./data/result -n TX2 -c 1024";
    print("文件传输程序已启动：" + cmd + "\n");
    subprocess.call(cmd, shell=True);
	
# 解析程序入参	
def parse_args():
    server_host = "";
    server_port = "";
    for index in range(len(sys.argv)):
        if sys.argv[index] == "-h":
            server_host = sys.argv[index + 1];	
        if sys.argv[index] == "-p":
            server_port = sys.argv[index + 1];
    return Config(server_host, server_port);

# 文件传输线程    
class TransferThread (threading.Thread):
    def __init__(self, thread_id, name):
        threading.Thread.__init__(self);
        self.thread_id = thread_id;
        self.name = name;
        
    def run(self):
        print ("开始线程：" + self.name)
        # 解析程序入参
        config = parse_args();
        try:
            file_transfer(config)
        except Exception as e:
            print("文件程序出现异常:", e);
            print("退出线程：" + self.name)
        
def main_process():
    # 
    #t = Timer(1, yolov_process);
    #t.start();
    # 创建线程后启动线程，调用run方法，
    transfer = TransferThread(1, "transfer_thread");
    # 启动线程 
    transfer.start();
    
    camera = CameraThread(2, "camera_thread");
    # 启动线程  
    camera.start();

    darknet = DarknetThread(3, "yolov_thread");
    darknet.start();
    
    
    # 等待线程
    camera.join();
    transfer.join();
    darknet.start();
    print("退出主线程");
	
if __name__ == '__main__':
    main_process();
    #print(sys.argv[1]);
