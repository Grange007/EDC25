# Hello World Example
#
# Welcome to the CanMV IDE! Click on the green run arrow button below to run the script!

import sensor, image, time, math
from machine import UART
from fpioa_manager import fm

fm.register(10, fm.fpioa.UART1_TX, force=True)
fm.register(11, fm.fpioa.UART1_RX, force=True)
uart_A = UART(UART.UART1, 115200, 8, 0, 0, timeout=1000, read_buf_len=4096)

#更改阈值
thresholds=[(42, 98, -106, 110, -128, 16),(43, 99, -63, 109, 10, 122),(0, 99, -4, 127, -62, -18),(2, 25, -127, 32, -117, 112)]#42开头是白色 43开头是黄色 0开头是蓝色 2开头是灰色
                  # Clear lcd screen.

sensor.reset()                      # Reset and initialize the sensor.
sensor.set_pixformat(sensor.RGB565) # Set pixel format to RGB565 (or GRAYSCALE)
sensor.set_framesize(sensor.QVGA)   # Set frame size to QVGA (320x240)
sensor.skip_frames(time = 2000)     # Wait for settings take effect.
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)
clock = time.clock()                # Create a clock object to track the FPS.
#输出规则：检测到色块就输出，这样不用修改循环。输出格式为x,y,color。摄像头拍到的画面与真实画面上下颠倒
col=8
row=8
flag1=0 #找到最大mask后flag=1,开始检测具体色块
flag3=0 #找到两个家的位置，用counter 判断
counter=0
wmaskx=0
wmasky=0
wmaskw=10000
wmaskh=10000 #白纸作mask排除外部阴影
standw=20
standh=20 #这里用黄色方块的长和宽来表示标准的长和宽
home1x=0 #home1是右上角的家
home1y=3000
home2x=4000 #home2是左下角的家
home2y=0
while(True):
    clock.tick()                    # Update the FPS clock.
    img = sensor.snapshot()         # Take a picture and return the image.
    write_str = ''
    for blob in img.find_blobs(thresholds,x_stride=1,y_stride=1,pixels_threshold=10,area_threshold=10,merge=False):            # Note: CanMV Cam runs about half as fast when connected
        if blob.code()==1:
            img.draw_rectangle(blob.rect())
            img.draw_cross(blob.cx(),blob.cy())
            img.draw_string(blob.x()+2,blob.y()+2,"w")
            if blob.area()>=30000 and flag1==0:
                flag1=1
                wmaskx=blob.x()
                wmasky=blob.y()
                wmaskw=blob.w()
                wmaskh=blob.h()

          #  img.draw_keypoints([(blob.cx(),blob.cy(),int(math.degrees(blob.rotation())))],size=20)
        if blob.code()==2:
            if flag1==0:
                continue

            if flag3==0:
                continue
            if  blob.x()>=home2x and blob.x()<=home1x and blob.y()>=home1y and blob.y()<=home2y:
                x=int((blob.x()-home2x)/standw)
                y=int((blob.y()-home1y)/standh)
                for i in range(int(blob.w()/standw)):
                    for a in range(int(blob.h()/standh)):
                        print(7-y-a,',',x+i,',','y')
                        write_str = str(7-y-a) + ',' + str(x+i) + ',' + str(2) + '\n' # yellow for gold
                        uart_A.write(write_str)
                        print('home1x:',home1x,'home1y:',home1y)
                        print('home2x:',home2x,'home2y:',home2y)
                        print('standw:',standw,'standh:',standh)
            img.draw_rectangle(blob.rect())
            img.draw_cross(blob.cx(),blob.cy())
            img.draw_string(blob.x()+2,blob.y()+2,"y")
        if blob.code()==4:
            if flag1==0 or flag3==0:
                continue
            if  blob.x()>=home2x and blob.x()<=home1x and blob.y()>=home1y and blob.y()<=home2y:
                x=int((blob.x()-home2x)/standw)
                y=int((blob.y()-home1y)/standh)
                for i in range(int(blob.w()/standw)):
                    for a in range(int(blob.h()/standh)):
                        print(7-y-a,',',x+i,',','b')
                        write_str = str(7-y-a) + ',' + str(x+i) + ',' + str(3) + '\n' # blue for diamond
                        uart_A.write(write_str)
                        print('home1x:',home1x,'home1y:',home1y)
                        print('home2x:',home2x,'home2y:',home2y)
                        print('standw:',standw,'standh:',standh)
            img.draw_rectangle(blob.rect())
            img.draw_cross(blob.cx(),blob.cy())
            img.draw_string(blob.x()+2,blob.y()+2,"b")
        if blob.code()==8:
            if flag1==0:
                continue
            if(counter<1000):
                counter+=1
            else:
                flag3=1
            if wmaskx<=blob.x()<=wmaskx+wmaskw and wmasky<=blob.y()<=wmasky+wmaskh and flag3==0 and wmaskx<=blob.x()+blob.w()<=wmasky+wmaskh and wmasky<=blob.y()+blob.h()<=wmasky+wmaskh and blob.area()>=400:
                if blob.x()+blob.w()>home1x and blob.y()<home1y:
                    home1x=blob.x()+blob.w()
                    home1y=blob.y()
                if blob.x()<home2x and blob.y()+blob.h()>home2y:
                    home2x=blob.x()
                    home2y=blob.y()+blob.h()
            if  blob.x()>=home2x and blob.x()<=home1x and blob.y()>=home1y and blob.y()<=home2y:
                x=int((blob.x()-home2x)/standw)
                y=int((blob.y()-home1y)/standh)
                for i in range(int(blob.w()/standw)):
                    for a in range(int(blob.h()/standh)):
                        print(7-y-a,',',x+i,',','g')
                        write_str = str(7-y-a) + ',' + str(x+i) + ',' + str(1) + '\n' # grey for iron
                        uart_A.write(write_str)
                        print('home1x:',home1x,'home1y:',home1y)
                        print('home2x:',home2x,'home2y:',home2y)
                        print('standw:',standw,'standh:',standh)
            img.draw_rectangle(blob.rect())
            img.draw_cross(blob.cx(),blob.cy())
            img.draw_string(blob.x()+2,blob.y()+2,"g")

uart_A.deinit()
del uart_A
