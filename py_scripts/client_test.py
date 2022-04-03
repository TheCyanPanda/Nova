#!/usr/bin/python3
import cv2

#print(cv2.getBuildInformation())

def main():    
	
    cap = cv2.VideoCapture('udp://127.0.0.1:39009', cv2.CAP_FFMPEG)
    if not cap.isOpened():
        print('VideoCapture not opened')
        exit(-1)
    
    cv2.namedWindow('image')
    print("Opened steam!")
    
    while True:
        ret, frame = cap.read()

        if not ret:
            print('frame empty')

        #frame = cv2.imdecode(frame, cv2.IMREAD_UNCHANGED)
        cv2.imshow('image', frame)

        if cv2.waitKey(25) & 0xFF == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()
	

if __name__ == "__main__":
	main()
