from flask import Flask, render_template, request
from flask_cors import CORS
from flask_socketio import SocketIO


app = Flask(__name__)
CORS(app)
app.config["SECRET_KEY"] = "secret!"
socketio = SocketIO(app)


@app.route("/")
def index():
    return render_template("index.html")


@app.route("/stm32_data", methods=["GET"])
def stm32_data():
    data = request.args.get("data")
    print("[backend]:", data)
    socketio.emit("stm32_data", {"data": data})
    return "Data received", 200


@socketio.on("disconnect")
def test_disconnect():
    print("[backend]: client disconnect")


@socketio.on("message")
def handle_message(data):
    print("[backend]: received message: " + data)


if __name__ == "__main__":
    socketio.run(app, debug=True, host="0.0.0.0", port=5050)
