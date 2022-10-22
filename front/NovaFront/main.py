"""
todo: Write docstring here
"""
import flask
import cv2
from flask import Flask
app = Flask(__name__)


@app.route('/')
@app.route('/index')
def index():
    user = {'username': 'Nova'}
    return flask.render_template('index.html', title='Home', user=user)


@app.route('/test')
def test():
    return 'Hello, test'


def start_stream():

    cap = cv2.VideoCapture('udp://127.0.0.1:39009', cv2.CAP_FFMPEG)
    if not cap.isOpened():
        print('VideoCapture not opened')
        exit(-1)

    cv2.namedWindow('image')
    print("Opened steam!")

    while True:
        success, frame = cap.read()  # read the camera frame
        if not success:
            break
        else:
            ret, buffer = cv2.imencode('.jpg', frame)
            frame = buffer.tobytes()
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')  # concat frame one by one and show result


@app.route('/video_feed')
def video_feed():
    return flask.Response(start_stream(), mimetype='multipart/x-mixed-replace; boundary=frame')


def main():
    pass


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True, threaded=True)
