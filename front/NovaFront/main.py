"""
todo: Write docstring here
"""
import flask
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


def main():
    pass


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True, threaded=True)
