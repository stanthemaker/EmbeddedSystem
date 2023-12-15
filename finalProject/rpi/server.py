from flask import Flask, render_template
from flask_cors import CORS
from generate_rand_num import generate_random_number  # Assuming generate_random_number is defined in gesture_ble module
import time

app = Flask(__name__)
CORS(app)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/random_number')
def get_random_number():
    # Generate a random number using the function
    random_number = generate_random_number()
    return {'random_number': random_number}

if __name__ == '__main__':
    app.run(debug=True)
