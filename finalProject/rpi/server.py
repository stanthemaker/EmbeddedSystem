from flask import Flask, render_template, request
from flask_cors import CORS
from flask_socketio import SocketIO


app = Flask(__name__)
CORS(app)
socketio = SocketIO(app)


@app.route("/")
def index():
    return render_template("index.html")


@app.route("/stm32_data", methods=["GET"])
def stm32_data():
    # data = request.json
    data = request.args.get('data')
    print("[backend]:", data)
    # socketio.emit("stm32_data", data)  # Emitting to browser clients
    socketio.emit('stm32_data', {'data': data})
    return "Data received", 200


if __name__ == "__main__":
    socketio.run(app, debug=True, host="0.0.0.0", port=5050)
