import random

# Function to generate a random number between 0 and 100
def generate_random_number():
    # Adjust the probabilities as needed
    random_number = random.random()

    if random_number < 0.8:
        return 0
    elif random_number < 0.85:
        return -1
    elif random_number < 0.9:
        return -2
    elif random_number < 0.95:
        return 1
    else:
        return 2
    
