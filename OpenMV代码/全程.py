
import sensor, image, time,pyb,lcd
from pyb import UART


uart = UART(3,115200)

sensor.reset()
sensor.set_pixformat(sensor.RGB565) # 灰度更快
sensor.set_framesize(sensor.QQVGA)
sensor.skip_frames(time = 2000)

clock = time.clock()
lcd.init()

window_roi = (0,0,160,40)

line_threshold = (18, 1, -18, 127, -86, 44)

rect1_1_mean = (50,5)
rect1_2_mean = (80,5)
rect1_3_mean = (110,5)
rect2_1_mean = (35,35)
rect2_2_mean = (125,35)

rect1_1_roi = (55,0,6,6)
rect1_2_roi = (75,0,6,6)
rect1_3_roi = (95,0,6,6)
rect2_1_roi = (30,30,10,10)
rect2_2_roi = (120,30,10,10)

pas = 0     #边线计数

rece = 0    #串口接收
rece_mode1 = b'startcnt1'    #串口接收到'startcnt1'即开始  第一次计数
rece_mode2 = b'startcnt2'    #串口接收到'startcnt2'即开始  第二次计数


#0=不工作  1=倒库   2=侧停
mode = 0

one   = False
two   = False
three = False

#串口接收函数
def receive_data():
     tmp_data = uart.read();  #读取所有可用字符#b'a'
     return tmp_data


#设置窗口大小
sensor.set_windowing(window_roi)


#循环体
while(True):
#上电初始化部分
    clock.tick()
    img = sensor.snapshot().lens_corr(strength = 1.5, zoom = 1.2)#lens_correction畸变矫正
    #使用畸变矫正方法来修复图像失真问题。

    img.binary([line_threshold])#图像部分的全部像素变为1白，将在阈值外的部分全部像素变为0黑


    rece = receive_data()
    if rece == rece_mode1:
        mode = 1
        pas = 0
    elif rece == rece_mode2:
        mode = 2
        pas = 0


#图像信息标注
    img.draw_rectangle(rect1_1_roi,color=(0,255,0))   #上一
    img.draw_rectangle(rect1_3_roi,color=(0,255,0))   #上三
    img.draw_rectangle(rect1_2_roi,color=(0,255,0))   #上二-中心
    img.draw_string(0,20,'mode:'+str(mode),(0,255,0),scale=1)


#标点计数

    if(mode == 1):  #第一次计数
        pix1_1 = img.get_statistics(roi=rect1_1_roi).mean()
        if(pix1_1>90):
            one = True
        if(one):
            pix1_2 = img.get_statistics(roi=rect1_2_roi).mean()
            if(pix1_2>90):
                two = True
        if(one and two):
            pix1_3 = img.get_statistics(roi=rect1_3_roi).mean()
            if(pix1_3>90):
                three = True
        if(one and two and three):
            pas += 1
            one = False
            two = False
            three = False
        if(pas == 4):
            if(mode == 1):
                message = 'stopend'
            for i in range(100):
                uart.write(message)
                print(message)
                mode = 0
    elif(mode == 2):   #第二次计数
        pix1_1 = img.get_statistics(roi=rect1_1_roi).mean()
        if(pix1_1>90):
            one = True
        if(one):
            pix1_2 = img.get_statistics(roi=rect1_2_roi).mean()
            if(pix1_2>90):
                two = True
        if(one and two):
            pix1_3 = img.get_statistics(roi=rect1_3_roi).mean()
            if(pix1_3>90):
                three = True
        if(one and two and three):
            pas += 1
            one = False
            two = False
            three = False
        if(pas == 3):
            if(mode == 2):
                message = 'stopend'
            for i in range(100):
                uart.write(message)
                print(message)
            mode = 0

#计数结束
    img.draw_string(0,0,str(pas),(0,255,0),scale=2)
    lcd.display(img)
    if pas == 4:
        pas = 0




