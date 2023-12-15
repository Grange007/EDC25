# Hello World Example
#
# Welcome to the CanMV IDE! Click on the green run arrow button below to run the script!

import sensor, image, time, math
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
lengthcount=0
hsum=0
hcount=0
wsum=0
wcount=0
prearea=0
flagw=0 #等于0代表下取整 等于1代表上取整
flagh=0
while(lengthcount<400):#找白色mask
    clock.tick()                    # Update the FPS clock.
    img = sensor.snapshot()         # Take a picture and return the image.
    for blob in img.find_blobs(thresholds,x_stride=1,y_stride=1,pixels_threshold=10,area_threshold=10,merge=False):
        if blob.code()==1:
           if blob.area()>=prearea and blob.area()>20000:
                prearea=blob.area()
                wmaskx=blob.x()
                wmasky=blob.y()
                wmaskw=blob.w()
                wmaskh=blob.h()
        lengthcount+=1
lengthcount=0
while(lengthcount<400):#找标准standh和standw
    clock.tick()                    # Update the FPS clock.
    img = sensor.snapshot()         # Take a picture and return the image.
    for blob in img.find_blobs(thresholds,x_stride=1,y_stride=1,pixels_threshold=10,area_threshold=10,merge=False):
        if blob.code()==4:
           if blob.w()>=10:
              wcount+=1
              wsum+=blob.w()
           if blob.h()>=10:
              hcount+=1
              hsum+=blob.h()
        lengthcount+=1
standw=wsum/wcount
standh=hsum/hcount

while(True):
    clock.tick()                    # Update the FPS clock.
    img = sensor.snapshot()         # Take a picture and return the image.
    for blob in img.find_blobs(thresholds,x_stride=1,y_stride=1,pixels_threshold=100,area_threshold=100,merge=False):            # Note: CanMV Cam runs about half as fast when connected
        if blob.code()==1:
           img.draw_rectangle(blob.rect())
           img.draw_string(blob.x()+2,blob.y()+2,"white")
          #  img.draw_keypoints([(blob.cx(),blob.cy(),int(math.degrees(blob.rotation())))],size=20)
        if blob.code()==2:
            if flag3==0:
                continue
            if  blob.x()>=home2x and blob.x()<=home1x and blob.y()>=home1y and blob.y()<=home2y:
                x=math.floor((blob.cx()-home2x)/standw)
                y=math.floor((blob.cy()-home1y)/standh)

                img.draw_rectangle(blob.rect())
                img.draw_cross(blob.cx(),blob.cy())
                img.draw_string(blob.x()+2,blob.y()+2,"yellow")

                print(7-y-a,',',x+i,',','yellow')
                print('home1x:',home1x,'home1y:',home1y)
                print('home2x:',home2x,'home2y:',home2y)
                print('standw:',standw,'standh:',standh)

        if blob.code()==4:
            if flag3==0:
                continue
            if  blob.x()>=home2x and blob.x()<=home1x and blob.y()>=home1y and blob.y()<=home2y:
                x=math.floor((blob.cx()-home2x)/standw)
                y=math.floor((blob.cy()-home1y)/standh)
                img.draw_rectangle(blob.rect())
                img.draw_cross(blob.cx(),blob.cy())
                img.draw_string(blob.x()+2,blob.y()+2,"blue")
                print(7-y-a,',',x+i,',','blue')
                print('home1x:',home1x,'home1y:',home1y)
                print('home2x:',home2x,'home2y:',home2y)
                print('standw:',standw,'standh:',standh)

        if blob.code()==8:
            if(counter<500):
                counter+=1
            else:
                flag3=1
                if home1x-home2x >=8*standw:
                   flagw=1
                if home2y-home1y>=8*standh:
                   flgah=1
                standw=(home1x-home2x)/8
                standh=(home2y-home1y)/8
            if wmaskx<=blob.x()<=wmaskx+wmaskw and wmasky<=blob.y()<=wmasky+wmaskh and flag3==0 and wmaskx<=blob.x()+blob.w()<=wmaskx+wmaskw and wmasky<=blob.y()+blob.h()<=wmasky+wmaskh and blob.area()>=100:
                if blob.x()+blob.w()>home1x and blob.y()<home1y:
                    home1x=blob.x()+blob.w()
                    home1y=blob.y()
                if blob.x()<home2x and blob.y()+blob.h()>home2y:
                    home2x=blob.x()
                    home2y=blob.y()+blob.h()
                img.draw_rectangle(blob.rect())
                img.draw_cross(blob.cx(),blob.cy())
                img.draw_string(blob.x()+2,blob.y()+2,"g")
            if  blob.x()>=home2x and blob.x()<=home1x and blob.y()>=home1y and blob.y()<=home2y:
                x=math.floor((blob.x()-home2x)/standw+0.5)
                y=math.floor((blob.y()-home1y)/standh+0.5)

                for i in range(int(blob.w()/standw+0.5)):
                    for a in range(int(blob.h()/standh+0.5)):
                        print(7-y-a,',',x+i,',','g')
                        print('home1x:',home1x,'home1y:',home1y)
                        print('home2x:',home2x,'home2y:',home2y)
                        print('standw:',standw,'standh:',standh)
                        print('blobw:',blob.w(),'blobh:',blob.h())


