import random
import string

def generate_random_line(length):
    """Generate a line of the specified length with random printable ASCII characters."""
    return ''.join(random.choices(string.ascii_letters + string.digits + string.punctuation + ' ', k=length)) + '\n'

def main():
    num_lines = 100
    line_length = 255
    filename = 'test_tx.txt'

    with open(filename, 'w') as file:
        for _ in range(num_lines):
            file.write(generate_random_line(line_length))
    
    print(f"File '{filename}' with {num_lines} lines of {line_length} bytes each has been created with random content.")

if __name__ == "__main__":
    main()
