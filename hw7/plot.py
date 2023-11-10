import matplotlib.pyplot as plt
import numpy as np
from scipy.signal import lfilter

fir_coeffs = np.array(
    [
        -0.0018225230,
        -0.0015879294,
        0.0000000000,
        +0.0036977508,
        +0.0080754303,
        +0.0085302217,
        -0.0000000000,
        -0.0173976984,
        -0.0341458607,
        -0.0333591565,
        +0.0000000000,
        +0.0676308395,
        +0.1522061835,
        +0.2229246956,
        +0.2504960933,
        +0.2229246956,
        +0.1522061835,
        +0.0676308395,
        +0.0000000000,
        -0.0333591565,
        -0.0341458607,
        -0.0173976984,
        -0.0000000000,
        +0.0085302217,
        +0.0080754303,
        +0.0036977508,
        +0.0000000000,
        -0.0015879294,
        -0.0018225230,
    ]
)


# Function to read data from a file
def read_data_from_file(file_path):
    with open(file_path, "r") as file:
        # Read the single line of space-separated numbers
        line = file.readline()
        # Split the line into individual number strings
        number_strings = line.split()
        # Convert each number string to a float
        data = [float(number) for number in number_strings]
    return data


# Function to plot the data in two subplots
def plot_data(input_data, output_data, filtered_signal):
    fig, axs = plt.subplots(3, 1, figsize=(10, 8))  # 2 rows, 1 column

    # Plot input data on the first subplot
    axs[0].plot(input_data, marker="o")
    axs[0].set_title("Input Data")
    axs[0].set_xlabel("Index")
    axs[0].set_ylabel("Value")
    axs[0].grid(True)

    # Plot output data on the second subplot
    axs[1].plot(output_data, marker="x")
    axs[1].set_title("Output Data")
    axs[1].set_xlabel("Index")
    axs[1].set_ylabel("Value")
    axs[1].grid(True)

    # Plot output data on the second subplot
    axs[2].plot(filtered_signal, marker="x")
    axs[2].set_title("verify output data")
    axs[2].set_xlabel("Index")
    axs[2].set_ylabel("Value")
    axs[2].grid(True)

    plt.tight_layout()  # Adjust subplots to fit into figure area.
    plt.show()


# Main code
if __name__ == "__main__":
    input_file_path = "input.txt"  # Replace with your actual file path
    output_file_path = "output.txt"  # Replace with your actual file path

    input_data = read_data_from_file(input_file_path)
    output_data = read_data_from_file(output_file_path)

    filtered_signal = lfilter(fir_coeffs, 1.0, input_data)

    plot_data(input_data, output_data, filtered_signal)
