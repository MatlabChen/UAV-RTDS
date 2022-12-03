#coding=utf-8

import os
import sys
import subprocess
import time
import datetime
import threading
from threading import Timer
        
def main_process():
    file_prefix = "manual_" + datetime.datetime.now().strftime("%Y%m%d%H%M%S%f");
    try:
        cmd = "./manual_camera -n ./data/camera/" + file_prefix;
        print("摄像头拍照程序开始拍照：" + cmd + "\n");
        subprocess.call(cmd, shell=True);
    except Exception as e:
        print("拍照程序出现异常:", e);
        return;
    print("开始识别" + file_prefix);
    try:
        file_name = list_file("./data/camera/", file_prefix);
        # yolov4 运行命令
        cmd = "./darknet detect cfg/yolov4.cfg yolov4.weights data/camera/" + file_name + " -thresh .1";
        print("正在识别处理的文件是：" + cmd + "\n");
        subprocess.call(cmd, shell=True);
        # 结果文件重命名
        start_index = file_name.rfind('/');
        end_index = file_name.rfind('.');
        rename_cmd = "mv predictions.jpg ./data/result/" + file_name[0: end_index] + "_predict.jpg";
        subprocess.call(rename_cmd, shell=True);
    except Exception as e:
        print("识别程序出现异常:", e);

def list_file(file_path, prefix):
    for file_name in os.listdir(file_path):
        if prefix in file_name:
            return file_name;
        
if __name__ == '__main__':
    main_process();
    #list_file("./data/camera", "camera");
    #print(sys.argv[1]);
