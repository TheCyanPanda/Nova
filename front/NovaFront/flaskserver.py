"""
./Novaserver    - start daemon  on port 1234
python flaskserver.py  - start flask server on port 5000
telnet localhost 1234 -> udp_start localhost 39009  - run console command to daemon to start udp server on port 39009


"""
import flask
import cv2
from flask import Flask, request
import threading
from werkzeug.serving import make_server


app = Flask(__name__)


class WebServer(threading.Thread):
    print(f'{__name__} -< __name__')
    host: str
    port: int
    debug: bool
    threaded: bool
    app = Flask(__name__)

    def __init__(self, host: str = '0.0.0.0', port: int = 5000, debug: bool = False):
        threading.Thread.__init__(self)
        self.host = host
        self.port = port
        self.debug = debug
        self.is_active = True
        self.server = None
        self.server = make_server('0.0.0.0', port, app)
        self.ctx = app.app_context()
        self.ctx.push()

    def run(self):
        print('Starting web server ...')
        self.server.serve_forever()
        print('done')

    def shutdown(self):
        print('Shutting down web server')
        self.server.shutdown()
        print('done')


def webserver(host: str = '0.0.0.0', port: int = 5000, debug: bool = True) -> WebServer:
    return WebServer(host=host, port=port, debug=debug)


@app.route('/')
@app.route('/index')
def index():
    user = {'username': 'Nova'}
    return flask.render_template('index.html', title='Home', user=user)


@app.route('/test')
def test():
    return 'Hello, test'


@app.route('/video_feed')
def video_feed():
    return flask.Response(start_stream(), mimetype='multipart/x-mixed-replace; boundary=frame')


def start_stream():

    d_size = (640, 360)
    cap = cv2.VideoCapture('udp://127.0.0.1:39009', cv2.CAP_FFMPEG)
    if not cap.isOpened():
        print('VideoCapture not opened')
        exit(-1)

    cv2.namedWindow('image')
    print("Opened steam!")

    while True:
        success, frame = cap.read()  # read the camera frame
        frame = cv2.resize(frame, d_size)   # todo: to be performed by daemon by CLI (or buttons in web)
        if not success:
            break
        else:
            ret, buffer = cv2.imencode('.jpg', frame)
            frame = buffer.tobytes()
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')  # concat frame one by one and show result


# @app.route('/video_feed')
# def video_feed():
#     return flask.Response(start_stream(), mimetype='multipart/x-mixed-replace; boundary=frame')


if __name__ == '__main__':
    pass
    #app.run(host='0.0.0.0', port=5000, debug=True, threaded=True)
